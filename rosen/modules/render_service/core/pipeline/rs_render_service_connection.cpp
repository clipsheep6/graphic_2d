/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_render_service_connection.h"

#include "common/rs_background_thread.h"
#include "hgm_core.h"
#include "hgm_command.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_render_service_listener.h"
#include "pipeline/rs_surface_capture_task.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_ui_capture.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/rs_jank_stats.h"
#include "rs_main_thread.h"
#include "rs_trace.h"

#ifdef TP_FEATURE_ENABLE
#include "touch_screen/touch_screen.h"
#endif

namespace OHOS {
namespace Rosen {
// we guarantee that when constructing this object,
// all these pointers are valid, so will not check them.
RSRenderServiceConnection::RSRenderServiceConnection(
    pid_t remotePid,
    wptr<RSRenderService> renderService,
    RSMainThread* mainThread,
    sptr<RSScreenManager> screenManager,
    sptr<IRemoteObject> token,
    sptr<VSyncDistributor> distributor)
    : remotePid_(remotePid),
      renderService_(renderService),
      mainThread_(mainThread),
      screenManager_(screenManager),
      token_(token),
      connDeathRecipient_(new RSConnectionDeathRecipient(this)),
      ApplicationDeathRecipient_(new RSApplicationRenderThreadDeathRecipient(this)),
      appVSyncDistributor_(distributor)
{
    if (!token_->AddDeathRecipient(connDeathRecipient_)) {
        RS_LOGW("RSRenderServiceConnection: Failed to set death recipient.");
    }
}

RSRenderServiceConnection::~RSRenderServiceConnection() noexcept
{
    if (token_ && connDeathRecipient_) {
        token_->RemoveDeathRecipient(connDeathRecipient_);
    }
    CleanAll();
}

void RSRenderServiceConnection::CleanVirtualScreens() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto id : virtualScreenIds_) {
        screenManager_->RemoveVirtualScreen(id);
    }
    virtualScreenIds_.clear();

    if (screenChangeCallback_ != nullptr) {
        screenManager_->RemoveScreenChangeCallback(screenChangeCallback_);
        screenChangeCallback_ = nullptr;
    }
}

void RSRenderServiceConnection::CleanRenderNodes() noexcept
{
    auto& context = mainThread_->GetContext();
    auto& nodeMap = context.GetMutableNodeMap();

    nodeMap.FilterNodeByPid(remotePid_);
}

void RSRenderServiceConnection::CleanAll(bool toDelete) noexcept
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (cleanDone_) {
            return;
        }
    }
    RS_LOGD("RSRenderServiceConnection::CleanAll() start.");
    mainThread_->ScheduleTask(
        [this]() {
            RS_TRACE_NAME_FMT("CleanVirtualScreens %d", remotePid_);
            CleanVirtualScreens();
        }).wait();
    mainThread_->ScheduleTask(
        [this]() {
            RS_TRACE_NAME_FMT("CleanRenderNodes %d", remotePid_);
            CleanRenderNodes();
        }).wait();
    mainThread_->ScheduleTask(
        [this]() {
            RS_TRACE_NAME_FMT("ClearTransactionDataPidInfo %d", remotePid_);
            mainThread_->ClearTransactionDataPidInfo(remotePid_);
        }).wait();
    mainThread_->ScheduleTask(
        [this]() {
            RS_TRACE_NAME_FMT("UnRegisterCallback %d", remotePid_);
            HgmConfigCallbackManager::GetInstance()->UnRegisterHgmConfigChangeCallback(remotePid_);
            mainThread_->UnRegisterOcclusionChangeCallback(remotePid_);
            mainThread_->ClearSurfaceOcclusionChangeCallback(remotePid_);
        }).wait();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        cleanDone_ = true;
    }

    if (toDelete) {
        auto renderService = renderService_.promote();
        if (renderService == nullptr) {
            RS_LOGW("RSRenderServiceConnection::CleanAll() RenderService is dead.");
        } else {
            renderService->RemoveConnection(GetToken());
        }
    }

    RS_LOGD("RSRenderServiceConnection::CleanAll() end.");
}

RSRenderServiceConnection::RSConnectionDeathRecipient::RSConnectionDeathRecipient(
    wptr<RSRenderServiceConnection> conn) : conn_(conn)
{
}

void RSRenderServiceConnection::RSConnectionDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        RS_LOGW("RSConnectionDeathRecipient::OnRemoteDied: can't promote remote object.");
        return;
    }

    auto rsConn = conn_.promote();
    if (rsConn == nullptr) {
        RS_LOGW("RSConnectionDeathRecipient::OnRemoteDied: RSRenderServiceConnection was dead, do nothing.");
        return;
    }

    if (rsConn->GetToken() != tokenSptr) {
        RS_LOGI("RSConnectionDeathRecipient::OnRemoteDied: token doesn't match, ignore it.");
        return;
    }

    rsConn->CleanAll(true);
}

RSRenderServiceConnection::RSApplicationRenderThreadDeathRecipient::RSApplicationRenderThreadDeathRecipient(
    wptr<RSRenderServiceConnection> conn) : conn_(conn)
{}

void RSRenderServiceConnection::RSApplicationRenderThreadDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        RS_LOGW("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: can't promote remote object.");
        return;
    }

    auto rsConn = conn_.promote();
    if (rsConn == nullptr) {
        RS_LOGW("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: RSRenderServiceConnection was dead, do nothing.");
        return;
    }

    RS_LOGI("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: Unregister.");
    auto app = iface_cast<IApplicationAgent>(tokenSptr);
    rsConn->UnRegisterApplicationAgent(app);
}

void RSRenderServiceConnection::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    mainThread_->RecvRSTransactionData(transactionData);
}

void RSRenderServiceConnection::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    auto cv = std::make_shared<std::condition_variable>();
    auto& mainThread = mainThread_;
    mainThread->PostTask([task, cv, &mainThread]() {
        if (task == nullptr || cv == nullptr) {
            return;
        }
        task->Process(mainThread->GetContext());
        cv->notify_all();
    });
    cv->wait_for(lock, std::chrono::nanoseconds(task->GetTimeout()));
}

bool RSRenderServiceConnection::GetUniRenderEnabled()
{
    return RSUniRenderJudgement::IsUniRender();
}

bool RSRenderServiceConnection::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    std::shared_ptr<RSSurfaceRenderNode> node =
        std::make_shared<RSSurfaceRenderNode>(config, mainThread_->GetContext().weak_from_this());
    if (node == nullptr) {
        RS_LOGE("RSRenderService::CreateNode fail");
        return false;
    }
    std::function<void()> registerNode = [node, this]() -> void {
        this->mainThread_->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    mainThread_->PostTask(registerNode);
    return true;
}

sptr<Surface> RSRenderServiceConnection::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    std::shared_ptr<RSSurfaceRenderNode> node =
        std::make_shared<RSSurfaceRenderNode>(config, mainThread_->GetContext().weak_from_this());
    if (node == nullptr) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface CreateNode fail");
        return nullptr;
    }
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(config.name);
    if (surface == nullptr) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface get consumer surface fail");
        return nullptr;
    }
    const std::string& surfaceName = surface->GetName();
    RS_LOGI("RsDebug RSRenderService::CreateNodeAndSurface node" \
        "id:%{public}" PRIu64 " name:%{public}s bundleName:%{public}s surface id:%{public}" PRIu64 " name:%{public}s",
        node->GetId(), node->GetName().c_str(), node->GetBundleName().c_str(),
        surface->GetUniqueId(), surfaceName.c_str());
    node->SetConsumer(surface);
    std::function<void()> registerNode = [node, this]() -> void {
        this->mainThread_->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    mainThread_->PostTask(registerNode);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(node);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode);
    SurfaceError ret = surface->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface Register Consumer Listener fail");
        return nullptr;
    }
    sptr<IBufferProducer> producer = surface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    return pSurface;
}


sptr<IVSyncConnection> RSRenderServiceConnection::CreateVSyncConnection(const std::string& name,
                                                                        const sptr<VSyncIConnectionToken>& token)
{
    sptr<VSyncConnection> conn = new VSyncConnection(appVSyncDistributor_, name, token->AsObject());
    auto ret = appVSyncDistributor_->AddConnection(conn);
    if (ret != VSYNC_ERROR_OK) {
        return nullptr;
    }
    return conn;
}

int32_t RSRenderServiceConnection::SetFocusAppInfo(
    int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName, uint64_t focusNodeId)
{
    mainThread_->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
    return SUCCESS;
}

ScreenId RSRenderServiceConnection::GetDefaultScreenId()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenManager_->GetDefaultScreenId();
}

std::vector<ScreenId> RSRenderServiceConnection::GetAllScreenIds()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenManager_->GetAllScreenIds();
}

ScreenId RSRenderServiceConnection::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int32_t flags)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto newVirtualScreenId = screenManager_->CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
    virtualScreenIds_.insert(newVirtualScreenId);
    auto& hgmCore = HgmCore::Instance();
    hgmCore.StartScreenScene(SceneType::SCREEN_RECORD);
    return newVirtualScreenId;
}

int32_t RSRenderServiceConnection::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenManager_->SetVirtualScreenSurface(id, surface);
}

void RSRenderServiceConnection::RemoveVirtualScreen(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    screenManager_->RemoveVirtualScreen(id);
    virtualScreenIds_.erase(id);
    auto& hgmCore = HgmCore::Instance();
    hgmCore.StopScreenScene(SceneType::SCREEN_RECORD);
    hgmCore.SetModeBySettingConfig();
}

int32_t RSRenderServiceConnection::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (screenChangeCallback_ == callback) {
        return INVALID_ARGUMENTS;
    }

    if (screenChangeCallback_ != nullptr) {
        // remove the old callback
        screenManager_->RemoveScreenChangeCallback(screenChangeCallback_);
    }

    // update
    int32_t status = screenManager_->AddScreenChangeCallback(callback);
    auto tmp = screenChangeCallback_;
    screenChangeCallback_ = callback;
    lock.unlock();
    return status;
}

void RSRenderServiceConnection::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { screenManager_->SetScreenActiveMode(id, modeId); }).wait();
    } else {
        return mainThread_->ScheduleTask(
            [=]() { screenManager_->SetScreenActiveMode(id, modeId); }).wait();
    }
}

void RSRenderServiceConnection::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderService::SetScreenRefreshRate");
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    int32_t setResult = hgmCore.SetScreenRefreshRate(id, sceneId, rate);
    if (setResult != 0) {
        RS_LOGW("SetScreenRefreshRate request of screen %{public}" PRIu64 " of rate %{public}d is refused",
            id, rate);
        return;
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSRenderServiceConnection::SetRefreshRateMode(int32_t refreshRateMode)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderService::SetRefreshRateMode");
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    int32_t setResult = hgmCore.SetRefreshRateMode(static_cast<RefreshRateMode>(refreshRateMode));
    if (setResult != 0) {
        RS_LOGW("SetRefreshRateMode mode %{public}d is not supported", refreshRateMode);
        return;
    }
    RSSystemProperties::SetHgmRefreshRateModesEnabled(std::to_string(refreshRateMode));
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

uint32_t RSRenderServiceConnection::GetScreenCurrentRefreshRate(ScreenId id)
{
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    uint32_t rate = hgmCore.GetScreenCurrentRefreshRate(id);
    if (rate == 0) {
        RS_LOGW("GetScreenCurrentRefreshRate failed to get current refreshrate of"
            " screen : %{public}" PRIu64 "", id);
    }
    return rate;
}

std::vector<int32_t> RSRenderServiceConnection::GetScreenSupportedRefreshRates(ScreenId id)
{
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    std::vector<int32_t> rates = hgmCore.GetScreenComponentRefreshRates(id);
    return rates;
}

int32_t RSRenderServiceConnection::GetCurrentRefreshRateMode()
{
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    int32_t refreshRateMode = hgmCore.GetCurrentRefreshRateMode();
    return refreshRateMode;
}

int32_t RSRenderServiceConnection::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetVirtualScreenResolution(id, width, height); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetVirtualScreenResolution(id, width, height); }).get();
    }
}

void RSRenderServiceConnection::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        RSHardwareThread::Instance().ScheduleTask(
            [=]() { screenManager_->SetScreenPowerStatus(id, status); }).wait();
    } else {
        mainThread_->ScheduleTask(
            [=]() { screenManager_->SetScreenPowerStatus(id, status); }).wait();
    }
}

void RSRenderServiceConnection::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    float scaleX, float scaleY, SurfaceCaptureType surfaceCaptureType)
{
    if (surfaceCaptureType == SurfaceCaptureType::DEFAULT_CAPTURE) {
        std::function<void()> captureTask = [scaleY, scaleX, callback, id]() -> void {
            RS_LOGD("RSRenderService::TakeSurfaceCapture callback->OnSurfaceCapture nodeId:[%{public}" PRIu64 "]", id);
            ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderService::TakeSurfaceCapture");
            RSSurfaceCaptureTask task(id, scaleX, scaleY);
            if (!task.Run(callback)) {
                callback->OnSurfaceCapture(id, nullptr);
            }
            ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        };
        mainThread_->PostTask(captureTask);
    } else {
        TakeSurfaceCaptureForUIWithUni(id, callback, scaleX, scaleY);
    }
}

void RSRenderServiceConnection::TakeSurfaceCaptureForUIWithUni(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    float scaleX, float scaleY)
{
    std::function<void()> offscreenRenderTask = [scaleY, scaleX, callback, id]() -> void {
        RS_LOGD("RSRenderService::TakeSurfaceCaptureForUIWithUni callback->OnOffscreenRender"
            " nodeId:[%{public}" PRIu64 "]", id);
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderService::TakeSurfaceCaptureForUIWithUni");
        std::shared_ptr<RSUniUICapture> rsUniUICapture = std::make_shared<RSUniUICapture>(id, scaleX, scaleY);
        std::shared_ptr<Media::PixelMap> pixelmap = rsUniUICapture->TakeLocalCapture();
        callback->OnSurfaceCapture(id, pixelmap.get());
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        RSOffscreenRenderThread::Instance().FinishOffscreenRenderTask();
    };
    RSOffscreenRenderThread::Instance().DoOffscreenRenderTask(offscreenRenderTask);
}

void RSRenderServiceConnection::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    auto captureTask = [=]() -> void {
        mainThread_->RegisterApplicationAgent(pid, app);
    };
    mainThread_->PostTask(captureTask);

    app->AsObject()->AddDeathRecipient(ApplicationDeathRecipient_);
}

void RSRenderServiceConnection::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    auto captureTask = [=]() -> void {
        RSMainThread::Instance()->UnRegisterApplicationAgent(app);
    };
    RSMainThread::Instance()->ScheduleTask(captureTask).wait();
}

RSVirtualScreenResolution RSRenderServiceConnection::GetVirtualScreenResolution(ScreenId id)
{
    RSVirtualScreenResolution virtualScreenResolution;
    screenManager_->GetVirtualScreenResolution(id, virtualScreenResolution);
    return virtualScreenResolution;
}

RSScreenModeInfo RSRenderServiceConnection::GetScreenActiveMode(ScreenId id)
{
    RSScreenModeInfo screenModeInfo;
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        RSHardwareThread::Instance().ScheduleTask(
            [=, &screenModeInfo]() { return screenManager_->GetScreenActiveMode(id, screenModeInfo); }).wait();
    } else {
        mainThread_->ScheduleTask(
            [=, &screenModeInfo]() { return screenManager_->GetScreenActiveMode(id, screenModeInfo); }).wait();
    }
    return screenModeInfo;
}

bool RSRenderServiceConnection::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    RSMainThread::Instance()->GetAppMemoryInMB(cpuMemSize, gpuMemSize);
    gpuMemSize += RSSubThreadManager::Instance()->GetAppGpuMemoryInMB();
    return true;
}

MemoryGraphic RSRenderServiceConnection::GetMemoryGraphic(int pid)
{
    MemoryGraphic memoryGraphic;
    if (!RSMainThread::Instance()->GetContext().GetNodeMap().ContainPid(pid)) {
        return memoryGraphic;
    }
    if (GetUniRenderEnabled()) {
        mainThread_->ScheduleTask([this, &pid, &memoryGraphic]() { return mainThread_->CountMem(pid, memoryGraphic); })
            .wait();
        return memoryGraphic;
    } else {
        return memoryGraphic;
    }
}

std::vector<MemoryGraphic> RSRenderServiceConnection::GetMemoryGraphics()
{
    std::vector<MemoryGraphic> memoryGraphics;
    if (GetUniRenderEnabled()) {
        mainThread_->ScheduleTask(
            [this, &memoryGraphics]() { return mainThread_->CountMem(memoryGraphics); }).wait();
        return memoryGraphics;
    } else {
        return memoryGraphics;
    }
}

std::vector<RSScreenModeInfo> RSRenderServiceConnection::GetScreenSupportedModes(ScreenId id)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->GetScreenSupportedModes(id); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenSupportedModes(id); }).get();
    }
}

RSScreenCapability RSRenderServiceConnection::GetScreenCapability(ScreenId id)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->GetScreenCapability(id); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenCapability(id); }).get();
    }
}

ScreenPowerStatus RSRenderServiceConnection::GetScreenPowerStatus(ScreenId id)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->GetScreenPowerStatus(id); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenPowerStatus(id); }).get();
    }
}

RSScreenData RSRenderServiceConnection::GetScreenData(ScreenId id)
{
    RSScreenData screenData;
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->GetScreenData(id); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenData(id); }).get();
    }
}

int32_t RSRenderServiceConnection::GetScreenBacklight(ScreenId id)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->GetScreenBacklight(id); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->GetScreenBacklight(id); }).get();
    }
}

void RSRenderServiceConnection::SetScreenBacklight(ScreenId id, uint32_t level)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetScreenBacklight(id, level); }).wait();
    } else {
        mainThread_->ScheduleTask(
            [=]() { screenManager_->SetScreenBacklight(id, level); }).wait();
    }
}

void RSRenderServiceConnection::RegisterBufferClearListener(
    NodeId id, sptr<RSIBufferClearCallback> callback)
{
    auto registerBufferClearListener = [id, callback, this]() -> bool {
        if (auto node = this->mainThread_->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
            node->RegisterBufferClearListener(callback);
            return true;
        }
        return false;
    };
    if (!registerBufferClearListener()) {
        mainThread_->PostTask(registerBufferClearListener);
    }
}

void RSRenderServiceConnection::RegisterBufferAvailableListener(
    NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    auto registerBufferAvailableListener = [id, callback, isFromRenderThread, this]() -> bool {
        if (auto node = this->mainThread_->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
            node->RegisterBufferAvailableListener(callback, isFromRenderThread);
            return true;
        }
        return false;
    };
    if (!registerBufferAvailableListener()) {
        RS_LOGI("RegisterBufferAvailableListener: node not found, post task to retry");
        mainThread_->PostTask(registerBufferAvailableListener);
    }
}

int32_t RSRenderServiceConnection::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenSupportedColorGamuts(id, mode); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenSupportedColorGamuts(id, mode); }).get();
    }
}

int32_t RSRenderServiceConnection::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &keys]() { return screenManager_->GetScreenSupportedMetaDataKeys(id, keys); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=, &keys]() { return screenManager_->GetScreenSupportedMetaDataKeys(id, keys); }).get();
    }
}

int32_t RSRenderServiceConnection::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenColorGamut(id, mode); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenColorGamut(id, mode); }).get();
    }
}

int32_t RSRenderServiceConnection::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetScreenColorGamut(id, modeIdx); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenColorGamut(id, modeIdx); }).get();
    }
}

int32_t RSRenderServiceConnection::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetScreenGamutMap(id, mode); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenGamutMap(id, mode); }).get();
    }
}

int32_t RSRenderServiceConnection::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenGamutMap(id, mode); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=, &mode]() { return screenManager_->GetScreenGamutMap(id, mode); }).get();
    }
}

int32_t RSRenderServiceConnection::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenManager_->GetScreenHDRCapability(id, screenHdrCapability);
}

int32_t RSRenderServiceConnection::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenManager_->GetScreenType(id, screenType);
}

#ifndef USE_ROSEN_DRAWING
bool RSRenderServiceConnection::GetBitmap(NodeId id, SkBitmap& bitmap)
#else
bool RSRenderServiceConnection::GetBitmap(NodeId id, Drawing::Bitmap& bitmap)
#endif
{
    if (!mainThread_->IsIdle()) {
        return false;
    }
    auto node = mainThread_->GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(id);
    if (node == nullptr) {
        RS_LOGE("RSRenderServiceConnection::GetBitmap cannot find NodeId: [%{public}" PRIu64 "]", id);
        return false;
    }
    if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
        RS_LOGE("RSRenderServiceConnection::GetBitmap RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
        return false;
    }
    auto getBitmapTask = [&node, &bitmap]() { bitmap = node->GetBitmap(); };
    mainThread_->PostSyncTask(getBitmapTask);
#ifndef USE_ROSEN_DRAWING
    return !bitmap.empty();
#else
    return bitmap.IsValid();
#endif
}

bool RSRenderServiceConnection::GetPixelmap(
    NodeId id, const std::shared_ptr<Media::PixelMap> pixelmap, const SkRect* rect)
{
    auto node = mainThread_->GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(id);
    if (node == nullptr) {
        RS_LOGE("RSRenderServiceConnection::GetPixelmap: cannot find NodeId: [%{public}" PRIu64 "]", id);
        return false;
    }
    if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
        RS_LOGE("RSRenderServiceConnection::GetPixelmap: RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
        return false;
    }
    bool result = false;
    auto getPixelmapTask = [&node, &pixelmap, rect, &result]() { result = node->GetPixelmap(pixelmap, rect); };
    mainThread_->PostSyncTask(getPixelmapTask);
    return result;
}

int32_t RSRenderServiceConnection::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        return RSHardwareThread::Instance().ScheduleTask(
            [=]() { return screenManager_->SetScreenSkipFrameInterval(id, skipFrameInterval); }).get();
    } else {
        return mainThread_->ScheduleTask(
            [=]() { return screenManager_->SetScreenSkipFrameInterval(id, skipFrameInterval); }).get();
    }
}

int32_t RSRenderServiceConnection::RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callback) {
        RS_LOGD("RSRenderServiceConnection::RegisterOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->RegisterOcclusionChangeCallback(remotePid_, callback);
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callback) {
        RS_LOGD("RSRenderServiceConnection::RegisterSurfaceOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->RegisterSurfaceOcclusionChangeCallback(id, remotePid_, callback);
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    mainThread_->UnRegisterSurfaceOcclusionChangeCallback(id);
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::RegisterHgmConfigChangeCallback(sptr<RSIHgmConfigChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callback) {
        RS_LOGD("RSRenderServiceConnection::RegisterHgmConfigChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }

    HgmConfigCallbackManager::GetInstance()->RegisterHgmConfigChangeCallback(remotePid_, callback);
    return StatusCode::SUCCESS;
}

void RSRenderServiceConnection::SetAppWindowNum(uint32_t num)
{
    auto task = [this, num]() -> void {
        mainThread_->SetAppWindowNum(num);
    };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    auto task = [this, watermarkImg, isShow]() -> void {
        mainThread_->ShowWatermark(watermarkImg, isShow);
    };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::ReportJankStats()
{
    auto task = [this]() -> void { RSJankStats::GetInstance().ReportJankStats(); };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::ReportEventResponse(DataBaseRs info)
{
    auto task = [this, info]() -> void {
        RSJankStats::GetInstance().SetReportEventResponse(info);
    };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::ReportEventComplete(DataBaseRs info)
{
    auto task = [this, info]() -> void {
        RSJankStats::GetInstance().SetReportEventComplete(info);
    };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::ReportEventJankFrame(DataBaseRs info)
{
    auto task = [this, info]() -> void {
        RSJankStats::GetInstance().SetReportEventJankFrame(info);
    };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::SetHardwareEnabled(NodeId id, bool isEnabled)
{
    auto task = [this, id, isEnabled]() -> void {
        auto& context = mainThread_->GetContext();
        auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
        if (node) {
            node->SetHardwareEnabled(isEnabled);
        }
    };
    mainThread_->PostTask(task);
}

void RSRenderServiceConnection::SetCacheEnabledForRotation(bool isEnabled)
{
    RSSystemProperties::SetCacheEnabledForRotation(isEnabled);
}

#ifdef TP_FEATURE_ENABLE
void RSRenderServiceConnection::SetTpFeatureConfig(int32_t feature, const char* config)
{
    if (TOUCH_SCREEN->tsSetFeatureConfig_ == nullptr) {
        RS_LOGW("RSRenderServiceConnection::SetTpFeatureConfig: touch screen function symbol is nullptr.");
        return;
    }
    if (TOUCH_SCREEN->tsSetFeatureConfig_(feature, config) < 0) {
        RS_LOGW("RSRenderServiceConnection::SetTpFeatureConfig: tsSetFeatureConfig_ failed.");
        return;
    }
}
#endif
} // namespace Rosen
} // namespace OHOS
