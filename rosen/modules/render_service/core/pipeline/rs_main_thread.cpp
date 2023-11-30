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
#define EGL_EGLEXT_PROTOTYPES
#include "pipeline/rs_main_thread.h"

#include <list>
#include <algorithm>
#include "hgm_core.h"
#include "include/core/SkGraphics.h"
#include "memory/rs_memory_graphic.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include <securec.h>
#include <stdint.h>
#include <string>
#include <unistd.h>
#include <malloc.h>
#ifdef RS_ENABLE_GL
#include "GLES3/gl3.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#endif
#include "include/gpu/GrDirectContext.h"
#include "rs_trace.h"

#include "animation/rs_animation_fraction.h"
#include "command/rs_message_processor.h"
#include "common/rs_background_thread.h"
#ifdef RS_ENABLE_PARALLEL_UPLOAD
#include "common/rs_upload_texture_thread.h"
#endif
#include "delegate/rs_functional_delegate.h"
#include "memory/rs_memory_manager.h"
#include "memory/rs_memory_track.h"
#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "platform/ohos/rs_jank_stats.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_frame_report.h"
#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_render_service_visitor.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_unmarshal_thread.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_occlusion_config.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_system_properties.h"
#include "platform/drawing/rs_vsync_client.h"
#include "property/rs_property_trace.h"
#include "property/rs_properties_painter.h"
#include "property/rs_point_light_manager.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context/memory_handler.h"
#endif
#include "render/rs_pixel_map_util.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_transaction_proxy.h"

#include "rs_qos_thread.h"
#include "xcollie/watchdog.h"

#include "render_frame_trace.h"

#if defined(ACCESSIBILITY_ENABLE)
#include "accessibility_config.h"
#endif

#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif

#if defined(RS_ENABLE_DRIVEN_RENDER)
#include "pipeline/driven_render/rs_driven_render_manager.h"
#endif

#include "pipeline/round_corner_display/rs_rcd_render_manager.h"
#include "scene_board_judgement.h"
#include "vsync_iconnection_token.h"

#include "mem_mgr_client.h"

using namespace FRAME_TRACE;
static const std::string RS_INTERVAL_NAME = "renderservice";

#if defined(ACCESSIBILITY_ENABLE)
using namespace OHOS::AccessibilityConfig;
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t REQUEST_VSYNC_NUMBER_LIMIT = 10;
constexpr uint64_t REFRESH_PERIOD = 16666667;
constexpr int32_t PERF_MULTI_WINDOW_REQUESTED_CODE = 10026;
constexpr int32_t VISIBLEAREARATIO_FORQOS = 3;
constexpr uint64_t PERF_PERIOD = 250000000;
constexpr uint64_t CLEAN_CACHE_FREQ = 60;
constexpr uint64_t SKIP_COMMAND_FREQ_LIMIT = 30;
constexpr uint64_t PERF_PERIOD_BLUR = 80000000;
constexpr uint64_t SK_RELEASE_RESOURCE_PERIOD = 5000000000;
constexpr uint64_t MAX_DYNAMIC_STATUS_TIME = 5000000000;
constexpr uint64_t PERF_PERIOD_MULTI_WINDOW = 80000000;
constexpr uint32_t MULTI_WINDOW_PERF_START_NUM = 2;
constexpr uint32_t MULTI_WINDOW_PERF_END_NUM = 4;
constexpr uint32_t WAIT_FOR_RELEASED_BUFFER_TIMEOUT = 3000;
constexpr uint32_t WAIT_FOR_HARDWARE_THREAD_TASK_TIMEOUT = 3000;
constexpr uint32_t HARDWARE_THREAD_TASK_NUM = 2;
constexpr uint32_t WAIT_FOR_MEM_MGR_SERVICE = 100;
constexpr uint32_t CAL_NODE_PREFERRED_FPS_LIMIT = 50;
constexpr const char* WALLPAPER_VIEW = "WallpaperView";
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr const char* MEM_MGR = "MemMgr";
#ifdef RS_ENABLE_GL
constexpr size_t DEFAULT_SKIA_CACHE_SIZE        = 96 * (1 << 20);
constexpr int DEFAULT_SKIA_CACHE_COUNT          = 2 * (1 << 12);
#endif
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
constexpr const char* MEM_GPU_TYPE = "gpu";
#endif
const std::map<int, int32_t> BLUR_CNT_TO_BLUR_CODE {
    { 1, 10021 },
    { 2, 10022 },
    { 3, 10023 },
};

bool Compare(const std::unique_ptr<RSTransactionData>& data1, const std::unique_ptr<RSTransactionData>& data2)
{
    if (!data1 || !data2) {
        RS_LOGW("Compare RSTransactionData: nullptr!");
        return true;
    }
    return data1->GetIndex() < data2->GetIndex();
}

void InsertToEnd(std::vector<std::unique_ptr<RSTransactionData>>& source,
    std::vector<std::unique_ptr<RSTransactionData>>& target)
{
    target.insert(target.end(), std::make_move_iterator(source.begin()), std::make_move_iterator(source.end()));
    source.clear();
}

void PerfRequest(int32_t perfRequestCode, bool onOffTag)
{
#ifdef SOC_PERF_ENABLE
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(perfRequestCode, onOffTag, "");
    RS_LOGD("RSMainThread::soc perf info [%{public}d %{public}d]", perfRequestCode, onOffTag);
#endif
}
}

#if defined(ACCESSIBILITY_ENABLE)
class AccessibilityObserver : public AccessibilityConfigObserver {
public:
    AccessibilityObserver() = default;
    void OnConfigChanged(const CONFIG_ID id, const ConfigValue &value) override
    {
        RS_LOGD("AccessibilityObserver OnConfigChanged configId: %{public}d", id);
        ColorFilterMode mode = ColorFilterMode::COLOR_FILTER_END;
        if (id == CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER) {
            switch (value.daltonizationColorFilter) {
                case Protanomaly:
                    mode = ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE;
                    break;
                case Deuteranomaly:
                    mode = ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE;
                    break;
                case Tritanomaly:
                    mode = ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE;
                    break;
                case Normal:
                    mode = ColorFilterMode::DALTONIZATION_NORMAL_MODE;
                    break;
                default:
                    break;
            }
            RSBaseRenderEngine::SetColorFilterMode(mode);
        } else if (id == CONFIG_ID::CONFIG_INVERT_COLOR) {
            mode = value.invertColor ? ColorFilterMode::INVERT_COLOR_ENABLE_MODE :
                                        ColorFilterMode::INVERT_COLOR_DISABLE_MODE;
            RSBaseRenderEngine::SetColorFilterMode(mode);
        } else if (id == CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT) {
            RSBaseRenderEngine::SetHighContrast(value.highContrastText);
        } else {
            RS_LOGW("AccessibilityObserver configId: %{public}d is not supported yet.", id);
        }
        RSMainThread::Instance()->PostTask([]() {
            RSMainThread::Instance()->SetAccessibilityConfigChanged();
            RSMainThread::Instance()->RequestNextVSync();
        });
    }
};
#endif
RSMainThread* RSMainThread::Instance()
{
    static RSMainThread instance;
    RSAnimationFraction::Init();
    return &instance;
}

RSMainThread::RSMainThread() : mainThreadId_(std::this_thread::get_id())
{
    context_ = std::make_shared<RSContext>();
}

RSMainThread::~RSMainThread() noexcept
{
    RemoveRSEventDetector();
    RSInnovation::CloseInnovationSo();
    if (rsAppStateListener_) {
        Memory::MemMgrClient::GetInstance().UnsubscribeAppState(*rsAppStateListener_);
    }
}

void RSMainThread::Init()
{
    mainLoop_ = [&]() {
        RenderFrameStart(timestamp_);
        PerfMultiWindow();
        SetRSEventDetectorLoopStartTag();
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSMainThread::DoComposition");
        ConsumeAndUpdateAllNodes();
        WaitUntilUnmarshallingTaskFinished();
        ProcessCommand();
        Animate(timestamp_);
        ApplyModifiers();
        ProcessHgmFrameRate(timestamp_);
        CollectInfoForHardwareComposer();
#if defined(RS_ENABLE_DRIVEN_RENDER)
        CollectInfoForDrivenRender();
#endif
        Render();
        InformHgmNodeInfo();
        ReleaseAllNodesBuffer();
        SendCommands();
        context_->activeNodesInRoot_.clear();
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        SetRSEventDetectorLoopFinishTag();
        rsEventManager_.UpdateParam();
        if (timestamp_ - preSKReleaseResourceTimestamp_ > SK_RELEASE_RESOURCE_PERIOD) {
            SKResourceManager::Instance().ReleaseResource();
            preSKReleaseResourceTimestamp_ = timestamp_;
        }
        RSUploadTextureThread::Instance().ReleaseNotUsedPinnedViews();
    };
    isUniRender_ = RSUniRenderJudgement::IsUniRender();
    SetDeviceType();
    qosPidCal_ = deviceType_ == DeviceType::PC;
    auto taskDispatchFunc = [](const RSTaskDispatcher::RSTask& task, bool isSyncTask = false) {
        RSMainThread::Instance()->PostTask(task);
    };
    context_->SetTaskRunner(taskDispatchFunc);
    RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(gettid(), taskDispatchFunc);
    RsFrameReport::GetInstance().Init();
    if (isUniRender_) {
        unmarshalBarrierTask_ = [this]() {
            auto cachedTransactionData = RSUnmarshalThread::Instance().GetCachedTransactionData();
            MergeToEffectiveTransactionDataMap(cachedTransactionData);
            {
                std::lock_guard<std::mutex> lock(unmarshalMutex_);
                ++unmarshalFinishedCount_;
            }

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)
#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
            RSUploadTextureThread::Instance().PostTask(uploadTextureBarrierTask_);
#endif
#endif
            unmarshalTaskCond_.notify_all();
        };
        RSUnmarshalThread::Instance().Start();
    }

    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread("RenderService", handler_);
    if (ret != 0) {
        RS_LOGW("Add watchdog thread failed");
    }
    InitRSEventDetector();
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    sptr<VSyncConnection> conn = new VSyncConnection(rsVSyncDistributor_, "rs", token->AsObject());
    rsFrameRateLinker_ = std::make_shared<RSRenderFrameRateLinker>();
    conn->id_ = rsFrameRateLinker_->GetId();
    rsVSyncDistributor_->AddConnection(conn);
    receiver_ = std::make_shared<VSyncReceiver>(conn, token->AsObject(), handler_);
    receiver_->Init();
    if (isUniRender_) {
        uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
        uniRenderEngine_->Init();
    } else {
        renderEngine_ = std::make_shared<RSRenderEngine>();
        renderEngine_->Init();
    }
#ifdef RS_ENABLE_GL
    int cacheLimitsTimes = 3;
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    auto grContext = isUniRender_? uniRenderEngine_->GetDrawingContext()->GetDrawingContext() :
        renderEngine_->GetDrawingContext()->GetDrawingContext();
#else
    auto grContext = isUniRender_? uniRenderEngine_->GetRenderContext()->GetGrContext() :
        renderEngine_->GetRenderContext()->GetGrContext();
#endif
    int maxResources = 0;
    size_t maxResourcesSize = 0;
    grContext->getResourceCacheLimits(&maxResources, &maxResourcesSize);
    if (maxResourcesSize > 0) {
        grContext->setResourceCacheLimits(cacheLimitsTimes * maxResources, cacheLimitsTimes *
            std::fmin(maxResourcesSize, DEFAULT_SKIA_CACHE_SIZE));
    } else {
        grContext->setResourceCacheLimits(DEFAULT_SKIA_CACHE_COUNT, DEFAULT_SKIA_CACHE_SIZE);
    }
#else
    auto gpuContext = isUniRender_? uniRenderEngine_->GetRenderContext()->GetDrGPUContext() :
        renderEngine_->GetRenderContext()->GetDrGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGE("RSMainThread::Init gpuContext is nullptr!");
        return;
    }
    int32_t maxResources = 0;
    size_t maxResourcesSize = 0;
    gpuContext->GetResourceCacheLimits(&maxResources, &maxResourcesSize);
    if (maxResourcesSize > 0) {
        gpuContext->SetResourceCacheLimits(cacheLimitsTimes * maxResources, cacheLimitsTimes *
            std::fmin(maxResourcesSize, DEFAULT_SKIA_CACHE_SIZE));
    } else {
        gpuContext->SetResourceCacheLimits(DEFAULT_SKIA_CACHE_COUNT, DEFAULT_SKIA_CACHE_SIZE);
    }
#endif
#endif
    RSInnovation::OpenInnovationSo();
#if defined(RS_ENABLE_DRIVEN_RENDER)
    RSDrivenRenderManager::InitInstance();
    RSBackgroundThread::Instance().InitRenderContext(GetRenderEngine()->GetRenderContext().get());
#endif

    RSRcdRenderManager::InitInstance();

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)
#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
    uploadTextureBarrierTask_ = [this]() {
        auto renderContext = GetRenderEngine()->GetRenderContext().get();
        uploadTextureFence = eglCreateSyncKHR(renderContext->GetEGLDisplay(), EGL_SYNC_FENCE_KHR, nullptr);
        {
            std::lock_guard<std::mutex> lock(uploadTextureMutex_);
            ++uploadTextureFinishedCount_;
        }
        uploadTextureTaskCond_.notify_all();
    };
    RSUploadTextureThread::Instance().InitRenderContext(GetRenderEngine()->GetRenderContext().get());
#endif
#endif

#if defined(ACCESSIBILITY_ENABLE)
    accessibilityObserver_ = std::make_shared<AccessibilityObserver>();
    auto &config = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    config.InitializeContext();
    config.SubscribeConfigObserver(CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER, accessibilityObserver_);
    config.SubscribeConfigObserver(CONFIG_ID::CONFIG_INVERT_COLOR, accessibilityObserver_);
    if (isUniRender_) {
        config.SubscribeConfigObserver(CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT, accessibilityObserver_);
    }
#endif

    auto delegate = RSFunctionalDelegate::Create();
    delegate->SetRepaintCallback([]() { RSMainThread::Instance()->RequestNextVSync(); });
    RSOverdrawController::GetInstance().SetDelegate(delegate);

    frameRateMgr_ = std::make_unique<HgmFrameRateManager>();
    frameRateMgr_->Init(rsVSyncController_, appVSyncController_, vsyncGenerator_);
    frameRateMgr_->SetTimerExpiredCallback([]() {
        RSMainThread::Instance()->PostTask([]() {
            RS_OPTIONAL_TRACE_NAME("RSMainThread::TimerExpiredCallback Run");
            RSMainThread::Instance()->SetForceUpdateUniRenderFlag(true);
            RSMainThread::Instance()->RequestNextVSync();
        });
    });
    SubscribeAppState();
}

void RSMainThread::RsEventParamDump(std::string& dumpString)
{
    rsEventManager_.DumpAllEventParam(dumpString);
}

void RSMainThread::RemoveRSEventDetector()
{
    if (rsCompositionTimeoutDetector_ != nullptr) {
        rsEventManager_.RemoveEvent(rsCompositionTimeoutDetector_->GetStringId());
    }
}

void RSMainThread::InitRSEventDetector()
{
    // default Threshold value of Timeout Event: 100ms
    rsCompositionTimeoutDetector_ = RSBaseEventDetector::CreateRSTimeOutDetector(100, "RS_COMPOSITION_TIMEOUT");
    if (rsCompositionTimeoutDetector_ != nullptr) {
        rsEventManager_.AddEvent(rsCompositionTimeoutDetector_, 60000); // report Internal 1min:60s：60000ms
        RS_LOGD("InitRSEventDetector finish");
    }
}

void RSMainThread::SetDeviceType()
{
    auto deviceTypeStr = system::GetParameter("const.product.devicetype", "pc");
    if (deviceTypeStr == "pc" || deviceTypeStr == "2in1") {
        deviceType_ = DeviceType::PC;
    } else if (deviceTypeStr == "tablet") {
        deviceType_ = DeviceType::TABLET;
    } else if (deviceTypeStr == "phone") {
        deviceType_ = DeviceType::PHONE;
    } else {
        deviceType_ = DeviceType::OTHERS;
    }
}

DeviceType RSMainThread::GetDeviceType() const
{
    return deviceType_;
}

uint64_t RSMainThread::GetFocusNodeId() const
{
    return focusNodeId_;
}

uint64_t RSMainThread::GetFocusLeashWindowId() const
{
    return focusLeashWindowId_;
}

void RSMainThread::SetFocusLeashWindowId()
{
    const auto& nodeMap = context_->GetNodeMap();
    auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodeMap.GetRenderNode(focusNodeId_));
    if (node != nullptr) {
        auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node->GetParent().lock());
        if (node->IsAppWindow() && parent && parent->IsLeashWindow()) {
            focusLeashWindowId_ = parent->GetId();
        }
    }
}

void RSMainThread::SetIsCachedSurfaceUpdated(bool isCachedSurfaceUpdated)
{
    isCachedSurfaceUpdated_ = isCachedSurfaceUpdated;
}

void RSMainThread::SetRSEventDetectorLoopStartTag()
{
    if (rsCompositionTimeoutDetector_ != nullptr) {
        rsCompositionTimeoutDetector_->SetLoopStartTag();
    }
}

void RSMainThread::SetRSEventDetectorLoopFinishTag()
{
    if (rsCompositionTimeoutDetector_ != nullptr) {
        if (isUniRender_) {
            rsCompositionTimeoutDetector_->SetLoopFinishTag(
                focusAppPid_, focusAppUid_, focusAppBundleName_, focusAppAbilityName_);
        } else {
            std::string defaultFocusAppInfo = "";
            rsCompositionTimeoutDetector_->SetLoopFinishTag(
                -1, -1, defaultFocusAppInfo, defaultFocusAppInfo);
        }
    }
}

void RSMainThread::SetFocusAppInfo(
    int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName, uint64_t focusNodeId)
{
    focusAppPid_ = pid;
    focusAppUid_ = uid;
    focusAppBundleName_ = bundleName;
    focusAppAbilityName_ = abilityName;
    focusNodeId_ = focusNodeId;
}

std::string RSMainThread::GetFocusAppBundleName() const
{
    return focusAppBundleName_;
}

void RSMainThread::Start()
{
    if (runner_) {
        runner_->Run();
    }
}

void RSMainThread::ProcessCommand()
{
    // To improve overall responsiveness, we make animations start on LAST frame instead of THIS frame.
    // If last frame is too far away (earlier than 1 vsync from now), we use currentTimestamp_ - REFRESH_PERIOD as
    // 'virtual' last frame timestamp.
    if (timestamp_ - lastAnimateTimestamp_ > REFRESH_PERIOD) { // if last frame is earlier than 1 vsync from now
        context_->currentTimestamp_ = timestamp_ - REFRESH_PERIOD;
    } else {
        context_->currentTimestamp_ = lastAnimateTimestamp_;
    }
    if (isUniRender_) {
        ProcessCommandForUniRender();
    } else {
        ProcessCommandForDividedRender();
    }
    switch(context_->purgeType_) {
        case RSContext::PurgeType::GENTLY:
            ClearMemoryCache(false);
            break;
        case RSContext::PurgeType::STRONGLY:
            ClearMemoryCache(true);
            break;
        default:
            break;
    }
    context_->purgeType_ = RSContext::PurgeType::NONE;
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().AnimateStart();
    }
}

void RSMainThread::CacheCommands()
{
    RS_OPTIONAL_TRACE_FUNC();
    for (auto& skipTransactionData : cachedSkipTransactionDataMap_) {
        pid_t pid = skipTransactionData.first;
        RS_TRACE_NAME("cacheCmd pid: " + std::to_string(pid));
        auto& skipTransactionDataVec = skipTransactionData.second;
        cachedTransactionDataMap_[pid].insert(cachedTransactionDataMap_[pid].begin(),
            std::make_move_iterator(skipTransactionDataVec.begin()),
            std::make_move_iterator(skipTransactionDataVec.end()));
    }
}

void RSMainThread::CheckIfNodeIsBundle(std::shared_ptr<RSSurfaceRenderNode> node)
{
    currentBundleName_ = node->GetBundleName();
    if (node->GetName() == WALLPAPER_VIEW) {
        noBundle_ = true;
    }
}

void RSMainThread::InformHgmNodeInfo()
{
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    int32_t informResult = EXEC_SUCCESS;
    if (currentBundleName_ != "") {
        informResult = hgmCore.RefreshBundleName(currentBundleName_);
    } else if (noBundle_) {
        currentBundleName_ = "";
        informResult = hgmCore.RefreshBundleName(currentBundleName_);
        noBundle_ = false;
    }
    if (informResult != EXEC_SUCCESS) {
        RS_LOGE("RSMainThread::InformHgmNodeInfo failed to refresh bundle name in hgm");
    }
}

std::unordered_map<NodeId, bool> RSMainThread::GetCacheCmdSkippedNodes() const
{
    return cacheCmdSkippedNodes_;
}

void RSMainThread::ResetSubThreadGrContext()
{
#ifdef RS_ENABLE_VK
    constexpr uint64_t delayNumOfFrameCount_ = 2;
    if (!needResetSubThreadGrContext_) {
        needResetSubThreadGrContext_ = true;
        frameCountForResetSubThreadGrContext_ = frameCount_.load();
    } else if (frameCount_.load() > frameCountForResetSubThreadGrContext_ + delayNumOfFrameCount_) {
        needResetSubThreadGrContext_ = false;
        RSSubThreadManager::Instance()->ResetSubThreadGrContext();
    }
#else
    RSSubThreadManager::Instance()->ResetSubThreadGrContext();
#endif
}

void RSMainThread::CheckParallelSubThreadNodesStatusImplementation()
{
    for (auto& node : subThreadNodes_) {
        if (node == nullptr) {
            RS_LOGE("RSMainThread::CheckParallelSubThreadNodesStatus sunThreadNode is nullptr!");
            continue;
        }
        if (node->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DOING) {
            RS_TRACE_NAME("node:[ " + node->GetName() + "]");
            pid_t pid = 0;
            if (node->IsAppWindow()) {
                pid = ExtractPid(node->GetId());
            } else if (node->IsLeashWindow()) {
                for (auto& child : node->GetSortedChildren()) {
                    auto surfaceNodePtr = child->ReinterpretCastTo<RSSurfaceRenderNode>();
                    if (surfaceNodePtr && surfaceNodePtr->IsAppWindow()) {
                        pid = ExtractPid(child->GetId());
                        break;
                    }
                }
            }
            cacheCmdSkippedNodes_[node->GetId()] = false;
            if (pid == 0) {
                continue;
            }
            RS_LOGD("RSMainThread::CheckParallelSubThreadNodesStatus pid = %{public}s, node name: %{public}s,"
                "id: %{public}" PRIu64 "", std::to_string(pid).c_str(), node->GetName().c_str(), node->GetId());
            if (cacheCmdSkippedInfo_.count(pid) == 0) {
                cacheCmdSkippedInfo_[pid] = std::make_pair(std::vector<NodeId>{node->GetId()}, false);
            } else {
                cacheCmdSkippedInfo_[pid].first.push_back(node->GetId());
            }
            if (!node->HasAbilityComponent()) {
                continue;
            }
            for (auto& nodeId : node->GetAbilityNodeIds()) {
                pid_t abilityNodePid = ExtractPid(nodeId);
                if (cacheCmdSkippedInfo_.count(abilityNodePid) == 0) {
                    cacheCmdSkippedInfo_[abilityNodePid] = std::make_pair(std::vector<NodeId>{node->GetId()}, true);
                } else {
                    cacheCmdSkippedInfo_[abilityNodePid].first.push_back(node->GetId());
                }
            }
        }
    }
}

bool RSMainThread::CheckParallelSubThreadNodesStatus()
{
    RS_OPTIONAL_TRACE_FUNC();
    cacheCmdSkippedInfo_.clear();
    cacheCmdSkippedNodes_.clear();
    if (subThreadNodes_.empty()) {
        ResetSubThreadGrContext();
        return false;
    } else {
#ifdef RS_ENABLE_VK
        needResetSubThreadGrContext_ = false;
#endif
    }
    CheckParallelSubThreadNodesStatusImplementation();
    if (!cacheCmdSkippedNodes_.empty()) {
        return true;
    }
    if (!isUiFirstOn_) {
        // clear subThreadNodes_ when UIFirst off and none of subThreadNodes_ is in the state of doing
        subThreadNodes_.clear();
    }
    return false;
}

bool RSMainThread::IsNeedSkip(NodeId instanceRootNodeId, pid_t pid)
{
    return std::any_of(cacheCmdSkippedInfo_[pid].first.begin(), cacheCmdSkippedInfo_[pid].first.end(),
        [instanceRootNodeId](const auto& cacheCmdSkipNodeId) {
            return cacheCmdSkipNodeId == instanceRootNodeId;
        });
}

void RSMainThread::SkipCommandByNodeId(std::vector<std::unique_ptr<RSTransactionData>>& transactionVec, pid_t pid)
{
    if (cacheCmdSkippedInfo_.find(pid) == cacheCmdSkippedInfo_.end()) {
        return;
    }
    std::vector<std::unique_ptr<RSTransactionData>> skipTransactionVec;
    const auto& nodeMap = context_->GetNodeMap();
    for (auto& transactionData: transactionVec) {
        std::vector<std::tuple<NodeId, FollowType, std::unique_ptr<RSCommand>>> skipPayload;
        std::vector<size_t> skipPayloadIndexVec;
        auto& processPayload = transactionData->GetPayload();
        for (size_t index = 0; index < processPayload.size(); ++index) {
            auto& elem = processPayload[index];
            if (std::get<2>(elem) == nullptr) { // check elem is valid
                continue;
            }
            NodeId nodeId = std::get<2>(elem)->GetNodeId();
            auto node = nodeMap.GetRenderNode(nodeId);
            if (node == nullptr) {
                continue;
            }
            NodeId firstLevelNodeId = node->GetFirstLevelNodeId();
            if (IsNeedSkip(firstLevelNodeId, pid)) {
                skipPayload.emplace_back(std::move(elem));
                skipPayloadIndexVec.push_back(index);
            }
        }
        if (!skipPayload.empty()) {
            std::unique_ptr<RSTransactionData> skipTransactionData = std::make_unique<RSTransactionData>();
            skipTransactionData->SetTimestamp(transactionData->GetTimestamp());
            std::string ablityName = transactionData->GetAbilityName();
            skipTransactionData->SetAbilityName(ablityName);
            skipTransactionData->SetSendingPid(transactionData->GetSendingPid());
            skipTransactionData->SetIndex(transactionData->GetIndex());
            skipTransactionData->GetPayload() = std::move(skipPayload);
            skipTransactionData->SetIsCached(true);
            skipTransactionVec.emplace_back(std::move(skipTransactionData));
        }
        for (auto iter = skipPayloadIndexVec.rbegin(); iter != skipPayloadIndexVec.rend(); ++iter) {
            processPayload.erase(processPayload.begin() + *iter);
        }
    }
    if (!skipTransactionVec.empty()) {
        cachedSkipTransactionDataMap_[pid] = std::move(skipTransactionVec);
    }
}

void RSMainThread::CheckAndUpdateTransactionIndex(std::shared_ptr<TransactionDataMap>& transactionDataEffective,
    std::string& transactionFlags)
{
    for (auto& rsTransactionElem: effectiveTransactionDataIndexMap_) {
        auto pid = rsTransactionElem.first;
        auto& lastIndex = rsTransactionElem.second.first;
        auto& transactionVec = rsTransactionElem.second.second;
        auto iter = transactionVec.begin();
        for (; iter != transactionVec.end(); ++iter) {
            if ((*iter) == nullptr) {
                continue;
            }
            if ((*iter)->GetIsCached()) {
                continue;
            }
            auto curIndex = (*iter)->GetIndex();
            if (curIndex == lastIndex + 1) {
                if ((*iter)->GetTimestamp() >= timestamp_) {
                    break;
                }
                ++lastIndex;
                transactionFlags += " [" + std::to_string(pid) + "," + std::to_string(curIndex) + "]";
            } else {
                RS_LOGE("%{public}s wait curIndex:%{public}" PRIu64 ", lastIndex:%{public}" PRIu64 ", pid:%{public}d",
                    __FUNCTION__, curIndex, lastIndex, pid);
                if (transactionDataLastWaitTime_[pid] == 0) {
                    transactionDataLastWaitTime_[pid] = timestamp_;
                }
                if ((timestamp_ - transactionDataLastWaitTime_[pid]) / REFRESH_PERIOD > SKIP_COMMAND_FREQ_LIMIT) {
                    transactionDataLastWaitTime_[pid] = 0;
                    lastIndex = curIndex;
                    transactionFlags += " skip to[" + std::to_string(pid) + "," + std::to_string(curIndex) + "]";
                    RS_LOGE("%{public}s skip to index:%{public}" PRIu64 ", pid:%{public}d",
                        __FUNCTION__, curIndex, pid);
                    continue;
                }
                break;
            }
        }
        if (iter != transactionVec.begin()) {
            (*transactionDataEffective)[pid].insert((*transactionDataEffective)[pid].end(),
                std::make_move_iterator(transactionVec.begin()), std::make_move_iterator(iter));
            transactionVec.erase(transactionVec.begin(), iter);
        }
    }
}

void RSMainThread::ProcessCommandForUniRender()
{
    std::shared_ptr<TransactionDataMap> transactionDataEffective = std::make_shared<TransactionDataMap>();
    std::string transactionFlags;
    bool isNeedCacheCmd = CheckParallelSubThreadNodesStatus();
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        cachedSkipTransactionDataMap_.clear();
        for (auto& rsTransactionElem: effectiveTransactionDataIndexMap_) {
            auto& transactionVec = rsTransactionElem.second.second;
            if (isNeedCacheCmd) {
                auto pid = rsTransactionElem.first;
                SkipCommandByNodeId(transactionVec, pid);
            }
            std::sort(transactionVec.begin(), transactionVec.end(), Compare);
        }
        if (isNeedCacheCmd) {
            CacheCommands();
        }
        CheckAndUpdateTransactionIndex(transactionDataEffective, transactionFlags);
    }
    if (!transactionDataEffective->empty()) {
        doDirectComposition_ = false;
    }
    RS_TRACE_NAME("RSMainThread::ProcessCommandUni" + transactionFlags);
    for (auto& rsTransactionElem: *transactionDataEffective) {
        for (auto& rsTransaction: rsTransactionElem.second) {
            if (rsTransaction) {
                if (rsTransaction->IsNeedSync() || syncTransactionData_.count(rsTransactionElem.first) > 0) {
                    ProcessSyncRSTransactionData(rsTransaction, rsTransactionElem.first);
                    continue;
                }
                ProcessRSTransactionData(rsTransaction, rsTransactionElem.first);
            }
        }
    }
    RSBackgroundThread::Instance().PostTask([ transactionDataEffective ] () {
        RS_TRACE_NAME("RSMainThread::ProcessCommandForUniRender transactionDataEffective clear");
        transactionDataEffective->clear();
    });
}

void RSMainThread::ProcessCommandForDividedRender()
{
    const auto& nodeMap = context_->GetNodeMap();
    RS_TRACE_BEGIN("RSMainThread::ProcessCommand");
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        if (!pendingEffectiveCommands_.empty()) {
            effectiveCommands_.swap(pendingEffectiveCommands_);
        }
        for (auto& [surfaceNodeId, commandMap] : cachedCommands_) {
            auto node = nodeMap.GetRenderNode<RSSurfaceRenderNode>(surfaceNodeId);
            auto bufferTimestamp = bufferTimestamps_.find(surfaceNodeId);
            std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>>::iterator effectIter;

            if (!node || !node->IsOnTheTree() || bufferTimestamp == bufferTimestamps_.end()) {
                // If node has been destructed or is not on the tree or has no valid buffer,
                // for all command cached in commandMap should be executed immediately
                effectIter = commandMap.end();
            } else {
                uint64_t timestamp = bufferTimestamp->second;
                effectIter = commandMap.upper_bound(timestamp);
            }

            for (auto it = commandMap.begin(); it != effectIter; it++) {
                effectiveCommands_[it->first].insert(effectiveCommands_[it->first].end(),
                    std::make_move_iterator(it->second.begin()), std::make_move_iterator(it->second.end()));
            }
            commandMap.erase(commandMap.begin(), effectIter);
        }
    }
    for (auto& [timestamp, commands] : effectiveCommands_) {
        context_->transactionTimestamp_ = timestamp;
        for (auto& command : commands) {
            if (command) {
                command->Process(*context_);
            }
        }
    }
    effectiveCommands_.clear();
    RS_TRACE_END();
}

void RSMainThread::ProcessRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid)
{
    context_->transactionTimestamp_ = rsTransactionData->GetTimestamp();
    rsTransactionData->Process(*context_);
}

void RSMainThread::ProcessSyncRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid)
{
    if (!rsTransactionData->IsNeedSync()) {
        syncTransactionData_[pid].emplace_back(std::move(rsTransactionData));
        return;
    }

    if (!syncTransactionData_.empty() && syncTransactionData_.begin()->second.front() &&
        (syncTransactionData_.begin()->second.front()->GetSyncId() > rsTransactionData->GetSyncId())) {
        ROSEN_LOGD("RSMainThread ProcessSyncRSTransactionData while syncId less GetCommandCount: %{public}lu"
            "pid: %{public}d", rsTransactionData->GetCommandCount(), rsTransactionData->GetSendingPid());
        ProcessRSTransactionData(rsTransactionData, pid);
        return;
    }

    bool isNeedCloseSync = rsTransactionData->IsNeedCloseSync();
    if (syncTransactionData_.empty()) {
        if (handler_) {
            auto task = [this, syncId = rsTransactionData->GetSyncId()]() {
                if (!syncTransactionData_.empty() && syncTransactionData_.begin()->second.front() &&
                        syncTransactionData_.begin()->second.front()->GetSyncId() != syncId) {
                    return;
                }
                ROSEN_LOGD("RSMainThread ProcessAllSyncTransactionData timeout task");
                ProcessAllSyncTransactionData();
            };
            handler_->PostTask(task, "ProcessAllSyncTransactionsTimeoutTask",
                RSSystemProperties::GetSyncTransactionWaitDelay());
        }
    }
    if (!syncTransactionData_.empty() && syncTransactionData_.begin()->second.front() &&
        (syncTransactionData_.begin()->second.front()->GetSyncId() != rsTransactionData->GetSyncId())) {
        ProcessAllSyncTransactionData();
    }
    if (syncTransactionData_.count(pid) == 0) {
        syncTransactionData_.insert({ pid, std::vector<std::unique_ptr<RSTransactionData>>() });
    }
    if (isNeedCloseSync) {
        syncTransactionCount_ += rsTransactionData->GetSyncTransactionNum();
    } else {
        syncTransactionCount_ -= 1;
    }
    syncTransactionData_[pid].emplace_back(std::move(rsTransactionData));
    if (syncTransactionCount_ == 0) {
        ProcessAllSyncTransactionData();
    }
}

void RSMainThread::ProcessAllSyncTransactionData()
{
    for (auto& [pid, transactions] : syncTransactionData_) {
        for (auto& transaction: transactions) {
            ROSEN_LOGD("RSMainThread ProcessAllSyncTransactionData GetCommandCount: %{public}lu pid: %{public}d",
                transaction->GetCommandCount(), pid);
            ProcessRSTransactionData(transaction, pid);
        }
    }
    syncTransactionData_.clear();
    syncTransactionCount_ = 0;
    RequestNextVSync();
}

void RSMainThread::ConsumeAndUpdateAllNodes()
{
    if (isUniRender_) {
        ResetHardwareEnabledState();
    }
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::ConsumeAndUpdateAllNodes");
    bool needRequestNextVsync = false;
    bufferTimestamps_.clear();
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes(
        [this, &needRequestNextVsync](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr) {
            return;
        }
        // Reset BasicGeoTrans info at the beginning of cmd process
        if (surfaceNode->IsMainWindowType()) {
            surfaceNode->ResetIsOnlyBasicGeoTransfrom();
        }
        if (surfaceNode->IsHardwareEnabledType()
            && CheckSubThreadNodeStatusIsDoing(surfaceNode->GetInstanceRootNodeId())) {
            RS_LOGD("SubThread is processing %{public}s, skip acquire buffer", surfaceNode->GetName().c_str());
            return;
        }
        auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*surfaceNode);
        surfaceHandler.ResetCurrentFrameBufferConsumed();
        if (RSBaseRenderUtil::ConsumeAndUpdateBuffer(surfaceHandler)) {
            this->bufferTimestamps_[surfaceNode->GetId()] = static_cast<uint64_t>(surfaceNode->GetTimestamp());
            if (surfaceNode->IsCurrentFrameBufferConsumed() && !surfaceNode->IsHardwareEnabledType()) {
                surfaceNode->SetContentDirty();
                doDirectComposition_ = false;
            }
        }

        // still have buffer(s) to consume.
        if (surfaceHandler.GetAvailableBufferCount() > 0) {
            needRequestNextVsync = true;
        }
    });

    if (needRequestNextVsync) {
        RequestNextVSync();
    }
    RS_OPTIONAL_TRACE_END();
}

bool RSMainThread::CheckSubThreadNodeStatusIsDoing(NodeId appNodeId) const
{
    for (auto& node : subThreadNodes_) {
        if (node->GetCacheSurfaceProcessedStatus() != CacheProcessStatus::DOING) {
            continue;
        }
        if (node->GetId() == appNodeId) {
            return true;
        }
        for (auto& child : node->GetSortedChildren()) {
            auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            if (surfaceNode && surfaceNode->GetId() == appNodeId) {
                return true;
            }
        }
    }
    return false;
}

void RSMainThread::CollectInfoForHardwareComposer()
{
    if (!isUniRender_) {
        return;
    }
    CheckIfHardwareForcedDisabled();
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes(
        [this, &nodeMap](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if (surfaceNode == nullptr || !surfaceNode->IsOnTheTree()) {
                return;
            }
            if (surfaceNode->GetBuffer() != nullptr) {
                selfDrawingNodes_.emplace_back(surfaceNode);
            }
            if (!surfaceNode->IsHardwareEnabledType()) {
                return;
            }

            // if hwc node is set on the tree this frame, mark its parent app node to be prepared
            auto appNodeId = surfaceNode->GetInstanceRootNodeId();
            if (surfaceNode->IsNewOnTree()) {
                context_->AddActiveNode(nodeMap.GetRenderNode(appNodeId));
                surfaceNode->ResetIsNewOnTree();
            }

            if (surfaceNode->GetBuffer() != nullptr) {
                // collect hwc nodes vector, used for display node skip and direct composition cases
                hardwareEnabledNodes_.emplace_back(surfaceNode);
            }

            // set content dirty for hwc node if needed
            if (isHardwareForcedDisabled_) {
                // buffer updated or hwc -> gpu
                if (surfaceNode->IsCurrentFrameBufferConsumed() || surfaceNode->IsLastFrameHardwareEnabled()) {
                    surfaceNode->SetContentDirty();
                }
            } else { // gpu -> hwc
                if (!surfaceNode->IsLastFrameHardwareEnabled()) {
                    if (!IsLastFrameUIFirstEnabled(appNodeId)) {
                        surfaceNode->SetContentDirty();
                    }
                    doDirectComposition_ = false;
                }
            }

            if (surfaceNode->IsCurrentFrameBufferConsumed()) {
                isHardwareEnabledBufferUpdated_ = true;
            }
        });
}

bool RSMainThread::IsLastFrameUIFirstEnabled(NodeId appNodeId) const
{
    for (auto& node : subThreadNodes_) {
        if (node->IsAppWindow()) {
            if (node->GetId() == appNodeId) {
                return true;
            }
        } else {
            for (auto& child : node->GetSortedChildren()) {
                auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
                if (surfaceNode && surfaceNode->IsAppWindow() && surfaceNode->GetId() == appNodeId) {
                    return true;
                }
            }
        }
    }
    return false;
}

void RSMainThread::CheckIfHardwareForcedDisabled()
{
    ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
    bool hasColorFilter = colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
        colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE;
    std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    bool isMultiDisplay = rootNode && rootNode->GetChildrenCount() > 1;

    // [PLANNING] GetChildrenCount > 1 indicates multi display, only Mirror Mode need be marked here
    // Mirror Mode reuses display node's buffer, so mark it and disable hardware composer in this case
    isHardwareForcedDisabled_ = isHardwareForcedDisabled_ || doWindowAnimate_ || isMultiDisplay || hasColorFilter;
}

void RSMainThread::CollectInfoForDrivenRender()
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    hasDrivenNodeOnUniTree_ = false;
    hasDrivenNodeMarkRender_ = false;
    if (!isUniRender_ || !RSSystemProperties::GetHardwareComposerEnabled() ||
        !RSDrivenRenderManager::GetInstance().GetDrivenRenderEnabled()) {
        return;
    }

    std::vector<std::shared_ptr<RSRenderNode>> drivenNodes;
    std::vector<std::shared_ptr<RSRenderNode>> markRenderDrivenNodes;

    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseDrivenRenderNodes(
        [&](const std::shared_ptr<RSRenderNode>& node) mutable {
            if (node == nullptr || !node->IsOnTheTree()) {
                return;
            }
            drivenNodes.emplace_back(node);
            if (node->GetPaintState()) {
                markRenderDrivenNodes.emplace_back(node);
            }
        });

    for (auto& node : drivenNodes) {
        node->SetPaintState(false);
        node->SetIsMarkDrivenRender(false);
    }
    if (!drivenNodes.empty()) {
        hasDrivenNodeOnUniTree_ = true;
    } else {
        hasDrivenNodeOnUniTree_ = false;
    }
    if (markRenderDrivenNodes.size() == 1) { // only support 1 driven node
        auto node = markRenderDrivenNodes.front();
        node->SetIsMarkDrivenRender(true);
        hasDrivenNodeMarkRender_ = true;
    } else {
        hasDrivenNodeMarkRender_ = false;
    }
#endif
}

void RSMainThread::ReleaseAllNodesBuffer()
{
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::ReleaseAllNodesBuffer");
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([this](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr) {
            return;
        }
        // surfaceNode's buffer will be released in hardware thread if last frame enables hardware composer
        if (surfaceNode->IsHardwareEnabledType()) {
            if (surfaceNode->IsLastFrameHardwareEnabled()) {
                if (!surfaceNode->IsCurrentFrameHardwareEnabled()) {
                    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*surfaceNode);
                    auto& preBuffer = surfaceHandler.GetPreBuffer();
                    if (preBuffer.buffer != nullptr) {
                        auto releaseTask = [buffer = preBuffer.buffer, consumer = surfaceHandler.GetConsumer(),
                            fence = preBuffer.releaseFence]() mutable {
                            auto ret = consumer->ReleaseBuffer(buffer, fence);
                            if (ret != OHOS::SURFACE_ERROR_OK) {
                                RS_LOGW("surfaceHandler ReleaseBuffer failed(ret: %{public}d)!", ret);
                            }
                        };
                        preBuffer.Reset();
                        RSHardwareThread::Instance().PostTask(releaseTask);
                    }
                }
                surfaceNode->ResetCurrentFrameHardwareEnabledState();
                return;
            }
            surfaceNode->ResetCurrentFrameHardwareEnabledState();
        }
        RSBaseRenderUtil::ReleaseBuffer(static_cast<RSSurfaceHandler&>(*surfaceNode));
    });
    RS_OPTIONAL_TRACE_END();
}

uint32_t RSMainThread::GetRefreshRate() const
{
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSMainThread::GetRefreshRate screenManager is nullptr");
        return 0;
    }
    return OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(screenManager->GetDefaultScreenId());
}

void RSMainThread::ClearMemoryCache(bool deeply)
{
    if (!RSSystemProperties::GetReleaseResourceEnabled()) {
        return;
    }
    this->clearMemoryFinished_ = false;
    PostTask([this, deeply]() {
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
        auto grContext = GetRenderEngine()->GetDrawingContext()->GetDrawingContext();
#else
        auto grContext = GetRenderEngine()->GetRenderContext()->GetGrContext();
#endif
        if (!grContext) {
            return;
        }
        RS_LOGD("Clear memory cache");
        RS_TRACE_NAME_FMT("Clear memory cache");
        grContext->flush();
        SkGraphics::PurgeAllCaches(); // clear cpu cache
        if (deeply) {
            MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(grContext);
        } else {
            MemoryManager::ReleaseUnlockGpuResource(grContext);
        }
#else
        auto grContext = GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
        if (grContext) {
            RS_LOGD("clear gpu cache");
            grContext->FlushAndSubmit(true);
            grContext->PurgeUnlockAndSafeCacheGpuResources();
        }
#endif
        grContext->flushAndSubmit(true);
        this->clearMemoryFinished_ = true;
    }, CLEAR_GPU_CACHE, 3000 / GetRefreshRate()); // The unit is milliseconds
}

void RSMainThread::WaitUtilUniRenderFinished()
{
    std::unique_lock<std::mutex> lock(uniRenderMutex_);
    if (uniRenderFinished_) {
        return;
    }
    RS_TRACE_NAME("RSMainThread::WaitUtilUniRenderFinished");
    uniRenderCond_.wait(lock, [this]() { return uniRenderFinished_; });
    uniRenderFinished_ = false;
}

bool RSMainThread::WaitUntilDisplayNodeBufferReleased(RSDisplayRenderNode& node)
{
    std::unique_lock<std::mutex> lock(displayNodeBufferReleasedMutex_);
    displayNodeBufferReleased_ = false; // prevent spurious wakeup of condition variable
    if (node.GetConsumer()->QueryIfBufferAvailable()) {
        return true;
    }
    return displayNodeBufferReleasedCond_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_RELEASED_BUFFER_TIMEOUT), [this]() { return displayNodeBufferReleased_; });
}

void RSMainThread::WaitUtilDrivenRenderFinished()
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    std::unique_lock<std::mutex> lock(drivenRenderMutex_);
    if (drivenRenderFinished_) {
        return;
    }
    drivenRenderCond_.wait(lock, [this]() { return drivenRenderFinished_; });
    drivenRenderFinished_ = false;
#endif
}

#if defined(RS_ENABLE_PARALLEL_UPLOAD) && defined(RS_ENABLE_GL)
void RSMainThread::WaitUntilUploadTextureTaskFinished()
{
    if (!isUniRender_) {
        return;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::WaitUntilUploadTextureTaskFinished");
    {
        std::unique_lock<std::mutex> lock(uploadTextureMutex_);
        //upload texture maximum waiting time is 100ms
        //otherwise main thread upload texture
        static const uint32_t WAIT_FOR_UPLOAD_FINISH_TIMEOUT = 100; 
        uploadTextureTaskCond_.wait_until(lock, std::chrono::system_clock::now() +
            std::chrono::milliseconds(WAIT_FOR_UPLOAD_FINISH_TIMEOUT), [this]() {
                 return uploadTextureFinishedCount_ > 0; });
        --uploadTextureFinishedCount_;
    }
    if (uploadTextureFence != EGL_NO_SYNC_KHR) {
        auto diplayID = GetRenderEngine()->GetRenderContext().get()->GetEGLDisplay();
        EGLint waitStatus = eglWaitSyncKHR(diplayID, uploadTextureFence, 0);
        if (waitStatus == EGL_FALSE) {
            ROSEN_LOGE("eglClientWaitSyncKHR error 0x%{public}x", eglGetError());
        } else if (waitStatus == EGL_TIMEOUT_EXPIRED_KHR) {
            ROSEN_LOGE("create eglClientWaitSyncKHR timeout");
        }
        eglDestroySyncKHR(diplayID, uploadTextureFence);
    }
    uploadTextureFence = EGL_NO_SYNC_KHR;
    RS_OPTIONAL_TRACE_END();
}
#endif

void RSMainThread::WaitUntilUnmarshallingTaskFinished()
{
    if (!isUniRender_) {
        return;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::WaitUntilUnmarshallingTaskFinished");
    std::unique_lock<std::mutex> lock(unmarshalMutex_);
    unmarshalTaskCond_.wait(lock, [this]() { return unmarshalFinishedCount_ > 0; });
    --unmarshalFinishedCount_;
    RS_OPTIONAL_TRACE_END();
}

void RSMainThread::MergeToEffectiveTransactionDataMap(TransactionDataMap& cachedTransactionDataMap)
{
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    for (auto& elem : cachedTransactionDataMap) {
        auto pid = elem.first;
        if (effectiveTransactionDataIndexMap_.count(pid) == 0) {
            RS_LOGE("RSMainThread::MergeToEffectiveTransactionDataMap pid:%{public}d not valid, skip it", pid);
            continue;
        }
        InsertToEnd(elem.second, effectiveTransactionDataIndexMap_[pid].second);
    }
    cachedTransactionDataMap.clear();
}

void RSMainThread::NotifyUniRenderFinish()
{
    if (std::this_thread::get_id() != Id()) {
        std::lock_guard<std::mutex> lock(uniRenderMutex_);
        uniRenderFinished_ = true;
        uniRenderCond_.notify_one();
    } else {
        uniRenderFinished_ = true;
    }
}

void RSMainThread::NotifyDisplayNodeBufferReleased()
{
    RS_TRACE_NAME("RSMainThread::NotifyDisplayNodeBufferReleased");
    std::lock_guard<std::mutex> lock(displayNodeBufferReleasedMutex_);
    displayNodeBufferReleased_ = true;
    displayNodeBufferReleasedCond_.notify_one();
}

void RSMainThread::NotifyDrivenRenderFinish()
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    if (std::this_thread::get_id() != Id()) {
        std::lock_guard<std::mutex> lock(drivenRenderMutex_);
        drivenRenderFinished_ = true;
        drivenRenderCond_.notify_one();
    } else {
        drivenRenderFinished_ = true;
    }
#endif
}

void RSMainThread::ProcessHgmFrameRate(uint64_t timestamp)
{
    RS_TRACE_FUNC();
    if (rsFrameRateLinker_ != nullptr) {
        rsFrameRateLinker_->SetExpectedRange(rsCurrRange_);
        RS_TRACE_NAME_FMT("rsCurrRange = (%d, %d, %d)", rsCurrRange_.min_, rsCurrRange_.max_, rsCurrRange_.preferred_);
    }
    // Check and processing refresh rate task.
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    hgmCore.SetTimestamp(timestamp);
    auto pendingRefreshRate = frameRateMgr_->GetPendingRefreshRate();
    if (pendingRefreshRate != nullptr) {
        hgmCore.SetPendingScreenRefreshRate(*pendingRefreshRate);
        frameRateMgr_->ResetPendingRefreshRate();
        currentRefreshRate_ = *pendingRefreshRate;
        RS_TRACE_NAME("RSMainThread::ProcessHgmFrameRate pendingRefreshRate: " + std::to_string(*pendingRefreshRate));
    }

    auto appFrameLinkers = GetContext().GetFrameRateLinkerMap().GetFrameRateLinkerMap();
    frameRateMgr_->UniProcessData(0, timestamp, rsFrameRateLinker_, appFrameLinkers, forceUpdateUniRenderFlag_);
}

bool RSMainThread::GetParallelCompositionEnabled()
{
    return doParallelComposition_;
}

void RSMainThread::UniRender(std::shared_ptr<RSBaseRenderNode> rootNode)
{
    UpdateUIFirstSwitch();
    UpdateRogSizeIfNeeded();
    auto uniVisitor = std::make_shared<RSUniRenderVisitor>();
#if defined(RS_ENABLE_DRIVEN_RENDER)
    uniVisitor->SetDrivenRenderFlag(hasDrivenNodeOnUniTree_, hasDrivenNodeMarkRender_);
#endif
    uniVisitor->SetHardwareEnabledNodes(hardwareEnabledNodes_);
    uniVisitor->SetAppWindowNum(appWindowNum_);
    uniVisitor->SetProcessorRenderEngine(GetRenderEngine());
    uniVisitor->SetForceUpdateFlag(forceUpdateUniRenderFlag_);

    if (isHardwareForcedDisabled_) {
        uniVisitor->MarkHardwareForcedDisabled();
        doDirectComposition_ = false;
    }
    bool needTraverseNodeTree = true;
    if (doDirectComposition_ && !isDirty_ && !isAccessibilityConfigChanged_
        && !isCachedSurfaceUpdated_ && !forceUpdateUniRenderFlag_) {
        if (isHardwareEnabledBufferUpdated_) {
            needTraverseNodeTree = !uniVisitor->DoDirectComposition(rootNode);
        } else {
            RS_LOGI("RSMainThread::Render nothing to update");
            for (auto& node: hardwareEnabledNodes_) {
                if (!node->IsHardwareForcedDisabled()) {
                    node->MarkCurrentFrameHardwareEnabled();
                }
            }
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)
#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
            WaitUntilUploadTextureTaskFinished();
#endif
#endif
            return;
        }
    }
    isCachedSurfaceUpdated_ = false;
    if (needTraverseNodeTree) {
        uniVisitor->SetAnimateState(doWindowAnimate_);
        uniVisitor->SetDirtyFlag(isDirty_ || isAccessibilityConfigChanged_);
        isAccessibilityConfigChanged_ = false;
        SetFocusLeashWindowId();
        uniVisitor->SetFocusedNodeId(focusNodeId_, focusLeashWindowId_);
        rootNode->Prepare(uniVisitor);
        uniVisitor->SetCurrentRefreshRate(currentRefreshRate_);
        RSPointLightManager::Instance()->PrepareLight();
        CalcOcclusion();
        doParallelComposition_ = RSInnovation::GetParallelCompositionEnabled(isUniRender_) &&
                                 rootNode->GetChildrenCount() > 1;
        if (doParallelComposition_) {
            RS_LOGD("RSMainThread::Render multi-threads parallel composition begin.");
            if (uniVisitor->ParallelComposition(rootNode)) {
                RS_LOGD("RSMainThread::Render multi-threads parallel composition end.");
                isDirty_ = false;
                PerfForBlurIfNeeded();
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)
#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
                WaitUntilUploadTextureTaskFinished();
#endif
#endif
                return;
            }
        }
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)
#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
        WaitUntilUploadTextureTaskFinished();
#endif
#endif
        if (IsUIFirstOn()) {
            auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(
                rootNode->GetSortedChildren().front());
            std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
            std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
            RSUniRenderUtil::AssignWindowNodes(displayNode, mainThreadNodes, subThreadNodes, focusNodeId_, deviceType_);
            const auto& nodeMap = context_->GetNodeMap();
            RSUniRenderUtil::ClearSurfaceIfNeed(nodeMap, displayNode, oldDisplayChildren_, deviceType_);
            uniVisitor->DrawSurfaceLayer(displayNode, subThreadNodes);
            RSUniRenderUtil::CacheSubThreadNodes(subThreadNodes_, subThreadNodes);
        }
        rootNode->Process(uniVisitor);
    } else {
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)
#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
        WaitUntilUploadTextureTaskFinished();
#endif
#endif
    }
    isDirty_ = false;
    forceUpdateUniRenderFlag_ = false;
}

void RSMainThread::Render()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)
#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
        WaitUntilUploadTextureTaskFinished();
#endif
#endif
        RS_LOGE("RSMainThread::Render GetGlobalRootRenderNode fail");
        return;
    }
    if (RSSystemProperties::GetRenderNodeTraceEnabled()) {
        RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    }

    if (isUniRender_) {
        UniRender(rootNode);
    } else {
        auto rsVisitor = std::make_shared<RSRenderServiceVisitor>();
        rsVisitor->SetAnimateState(doWindowAnimate_);
        rootNode->Prepare(rsVisitor);
        CalcOcclusion();
        bool doParallelComposition = false;
        if (!rsVisitor->ShouldForceSerial() && RSInnovation::GetParallelCompositionEnabled(isUniRender_)) {
            doParallelComposition = DoParallelComposition(rootNode);
        }
        if (doParallelComposition) {
            renderEngine_->ShrinkCachesIfNeeded();
            return;
        }
        rootNode->Process(rsVisitor);
        renderEngine_->ShrinkCachesIfNeeded();
    }
    CallbackDrawContextStatusToWMS();
    PerfForBlurIfNeeded();
}

void RSMainThread::CallbackDrawContextStatusToWMS()
{
    VisibleData drawStatusVec;
    for (auto dynamicNodeId : curDrawStatusVec_) {
        if (lastDrawStatusMap_.find(dynamicNodeId) == lastDrawStatusMap_.end()) {
            drawStatusVec.emplace_back(std::make_pair(dynamicNodeId,
                WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS));
            RS_LOGD("%{public}s nodeId[%{public}" PRIu64 "] status[%{public}d]",
                __func__, dynamicNodeId, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS);
        }
        lastDrawStatusMap_[dynamicNodeId] = timestamp_;
    }
    auto drawStatusIter = lastDrawStatusMap_.begin();
    while (drawStatusIter != lastDrawStatusMap_.end()) {
        if (timestamp_ - drawStatusIter->second > MAX_DYNAMIC_STATUS_TIME) {
            drawStatusVec.emplace_back(std::make_pair(drawStatusIter->first,
                WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_STATIC_STATUS));
            RS_LOGD("%{public}s nodeId[%{public}" PRIu64 "] status[%{public}d]",
                __func__, drawStatusIter->first, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_STATIC_STATUS);
            auto tmpIter = drawStatusIter++;
            lastDrawStatusMap_.erase(tmpIter);
        } else {
            drawStatusIter++;
        }
    }
    curDrawStatusVec_.clear();
    if (!drawStatusVec.empty()) {
        std::lock_guard<std::mutex> lock(occlusionMutex_);
        for (auto it = occlusionListeners_.begin(); it != occlusionListeners_.end(); it++) {
            if (it->second) {
                it->second->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>(drawStatusVec));
            }
        }
    }
}

bool RSMainThread::CheckSurfaceNeedProcess(OcclusionRectISet& occlusionSurfaces,
    std::shared_ptr<RSSurfaceRenderNode> curSurface)
{
    bool needProcess = false;
    if (curSurface->IsFocusedNode(focusNodeId_)) {
        needProcess = true;
        if (!curSurface->HasContainerWindow() && !curSurface->IsTransparent() &&
            !curSurface->GetAnimateState() && // when node animating (i.e. 3d animation), the region cannot be trusted
            curSurface->GetName().find("hisearch") == std::string::npos) {
            occlusionSurfaces.insert(curSurface->GetDstRect());
        }
    } else {
        size_t beforeSize = occlusionSurfaces.size();
        occlusionSurfaces.insert(curSurface->GetDstRect());
        bool insertSuccess = occlusionSurfaces.size() > beforeSize ? true : false;
        if (insertSuccess) {
            needProcess = true;
            if (curSurface->IsTransparent() ||
                curSurface->GetAnimateState() || // when node animating(i.e. 3d animation), the region cannot be trusted
                curSurface->GetName().find("hisearch") != std::string::npos) {
                auto iter = std::find_if(occlusionSurfaces.begin(), occlusionSurfaces.end(),
                    [&curSurface](RectI r) -> bool {return r == curSurface->GetDstRect();});
                if (iter != occlusionSurfaces.end()) {
                    occlusionSurfaces.erase(iter);
                }
            }
        }
    }

    if (needProcess) {
        CheckIfNodeIsBundle(curSurface);
    }
    return needProcess;
}

RSVisibleLevel RSMainThread::GetRegionVisibleLevel(const Occlusion::Region& curRegion,
    const Occlusion::Region& visibleRegion)
{
    if (visibleRegion.GetSize() == 0) {
        return RSVisibleLevel::RS_INVISIBLE;
    } else if (visibleRegion.Area() == curRegion.Area()) {
        return RSVisibleLevel::RS_ALL_VISIBLE;
    } else if (visibleRegion.Area() < (curRegion.Area() >> VISIBLEAREARATIO_FORQOS)) {
        return RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE;
    }
    return RSVisibleLevel::RS_SEMI_NONDEFAULT_VISIBLE;
}

void RSMainThread::CalcOcclusionImplementation(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    Occlusion::Region accumulatedRegion;
    VisibleData curVisVec;
    OcclusionRectISet occlusionSurfaces;
    std::map<uint32_t, RSVisibleLevel> pidVisMap;
    bool hasFilterCacheOcclusion = false;
    bool filterCacheOcclusionEnabled = RSSystemParameters::GetFilterCacheOcculusionEnabled();
    for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
        auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (curSurface == nullptr || curSurface->GetDstRect().IsEmpty() || curSurface->IsLeashWindow()) {
            continue;
        }
        Occlusion::Rect occlusionRect = curSurface->GetSurfaceOcclusionRect(isUniRender_);
        curSurface->setQosCal(qosPidCal_);
        if (CheckSurfaceNeedProcess(occlusionSurfaces, curSurface)) {
            Occlusion::Region curRegion { occlusionRect };
            Occlusion::Region subResult = curRegion.Sub(accumulatedRegion);
            RSVisibleLevel visibleLevel = GetRegionVisibleLevel(curRegion, subResult);
            RS_LOGD("%{public}s nodeId[%{public}" PRIu64 "] visibleLevel[%{public}d]",
                __func__, curSurface->GetId(), visibleLevel);
            curSurface->SetVisibleRegionRecursive(subResult, curVisVec, pidVisMap, true, visibleLevel);
            curSurface->AccumulateOcclusionRegion(accumulatedRegion, curRegion, hasFilterCacheOcclusion, isUniRender_,
                filterCacheOcclusionEnabled);
        } else {
            curSurface->SetVisibleRegionRecursive({}, curVisVec, pidVisMap);
        }
    }

    // if there are valid filter cache occlusion, recalculate surfacenode visibleregionforcallback for WMS/QOS callback
    if (hasFilterCacheOcclusion && isUniRender_) {
        curVisVec.clear();
        pidVisMap.clear();
        occlusionSurfaces.clear();
        accumulatedRegion = {};
        for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
            auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
            if (curSurface == nullptr || curSurface->GetDstRect().IsEmpty() || curSurface->IsLeashWindow()) {
                continue;
            }
            Occlusion::Rect occlusionRect = curSurface->GetSurfaceOcclusionRect(isUniRender_);
            curSurface->setQosCal(qosPidCal_);
            if (CheckSurfaceNeedProcess(occlusionSurfaces, curSurface)) {
                Occlusion::Region curRegion { occlusionRect };
                Occlusion::Region subResult = curRegion.Sub(accumulatedRegion);
                RSVisibleLevel visibleLevel = GetRegionVisibleLevel(curRegion, subResult);
                curSurface->SetVisibleRegionRecursive(subResult, curVisVec, pidVisMap, false, visibleLevel);
                curSurface->AccumulateOcclusionRegion(accumulatedRegion, curRegion, hasFilterCacheOcclusion,
                    isUniRender_, false);
            } else {
                curSurface->SetVisibleRegionRecursive({}, curVisVec, pidVisMap, false);
            }
        }
    }

    // Callback to WMS and QOS
    CallbackToWMS(curVisVec);
    CallbackToQOS(pidVisMap);
    // Callback for registered self drawing surfacenode
    SurfaceOcclusionCallback();
}

void RSMainThread::CalcOcclusion()
{
    RS_OPTIONAL_TRACE_NAME("RSMainThread::CalcOcclusion");
    RS_LOGD("RSMainThread::CalcOcclusion animate:%{public}d isUniRender:%{public}d",
        doWindowAnimate_.load(), isUniRender_);
    if (doWindowAnimate_ && !isUniRender_) {
        return;
    }
    const std::shared_ptr<RSBaseRenderNode> node = context_->GetGlobalRootRenderNode();
    if (node == nullptr) {
        RS_LOGE("RSMainThread::CalcOcclusion GetGlobalRootRenderNode fail");
        return;
    }
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    if (node->GetChildrenCount()== 1) {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(
            node->GetSortedChildren().front());
        if (displayNode) {
            curAllSurfaces = displayNode->GetCurAllSurfaces();
        }
    } else {
        node->CollectSurface(node, curAllSurfaces, isUniRender_, false);
    }
    // Judge whether it is dirty
    // Surface cnt changed or surface DstRectChanged or surface ZorderChanged
    std::vector<NodeId> curSurfaceIds;
    curSurfaceIds.reserve(curAllSurfaces.size());
    for (auto it = curAllSurfaces.begin(); it != curAllSurfaces.end(); ++it) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surface == nullptr) {
            continue;
        }
        curSurfaceIds.emplace_back(surface->GetId());
    }
    bool winDirty = (lastSurfaceIds_ != curSurfaceIds || isDirty_ ||
        lastFocusNodeId_ != focusNodeId_);
    lastSurfaceIds_ = std::move(curSurfaceIds);
    lastFocusNodeId_ = focusNodeId_;
    if (!winDirty) {
        for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
            auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
            if (surface == nullptr || surface->IsLeashWindow()) {
                continue;
            }
            if (surface->GetZorderChanged() || surface->GetDstRectChanged() ||
                surface->IsOpaqueRegionChanged() ||
                surface->GetAlphaChanged() || (isUniRender_ && surface->IsDirtyRegionUpdated())) {
                winDirty = true;
            } else if (RSSystemParameters::GetFilterCacheOcculusionEnabled() &&
                surface->IsTransparent() && surface->IsFilterCacheStatusChanged()) {
                // When current frame's filter cache is valid or last frame's occlusion use filter cache as opaque
                // The occlusion needs to be recalculated
                winDirty = true;
            }
            surface->CleanDstRectChanged();
            surface->CleanAlphaChanged();
            surface->CleanOpaqueRegionChanged();
            surface->CleanDirtyRegionUpdated();
        }
    }
    if (!winDirty) {
        if (SurfaceOcclusionCallBackIfOnTreeStateChanged()) {
            SurfaceOcclusionCallback();
        }
        return;
    }
    CalcOcclusionImplementation(curAllSurfaces);
}

bool RSMainThread::CheckQosVisChanged(std::map<uint32_t, RSVisibleLevel>& pidVisMap)
{
    bool isVisibleChanged = pidVisMap.size() != lastPidVisMap_.size();
    if (!isVisibleChanged) {
        auto iterCur = pidVisMap.begin();
        auto iterLast = lastPidVisMap_.begin();
        for (; iterCur != pidVisMap.end(); iterCur++, iterLast++) {
            if (iterCur->first != iterLast->first ||
                iterCur->second != iterLast->second) {
                isVisibleChanged = true;
                break;
            }
        }
    }

    if (isVisibleChanged) {
        lastPidVisMap_ = pidVisMap;
    }
    return isVisibleChanged;
}

void RSMainThread::CallbackToQOS(std::map<uint32_t, RSVisibleLevel>& pidVisMap)
{
    RSQosThread::GetInstance()->SetQosCal(qosPidCal_);
    if (!CheckQosVisChanged(pidVisMap)) {
        return;
    }
    RS_TRACE_NAME("RSQosThread::OnRSVisibilityChangeCB");
    RSQosThread::GetInstance()->OnRSVisibilityChangeCB(pidVisMap);
}

void RSMainThread::CallbackToWMS(VisibleData& curVisVec)
{
    // if visible surfaces changed callback to WMS：
    // 1. curVisVec size changed
    // 2. curVisVec content changed
    bool visibleChanged = curVisVec.size() != lastVisVec_.size();
    std::sort(curVisVec.begin(), curVisVec.end());
    if (!visibleChanged) {
        for (uint32_t i = 0; i < curVisVec.size(); i++) {
            if ((curVisVec[i].first != lastVisVec_[i].first) || (curVisVec[i].second != lastVisVec_[i].second)) {
                visibleChanged = true;
                break;
            }
        }
    }
    if (visibleChanged) {
        std::lock_guard<std::mutex> lock(occlusionMutex_);
        for (auto it = occlusionListeners_.begin(); it != occlusionListeners_.end(); it++) {
            if (it->second) {
                it->second->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>(curVisVec));
            }
        }
    }
    lastVisVec_.clear();
    std::swap(lastVisVec_, curVisVec);
}

void RSMainThread::SurfaceOcclusionCallback()
{
    const auto& nodeMap = context_->GetNodeMap();
    for (auto &listener : surfaceOcclusionListeners_) {
        if (savedAppWindowNode_.find(listener.first) == savedAppWindowNode_.end()) {
            auto node = nodeMap.GetRenderNode(listener.first);
            if (!node || !node->IsOnTheTree()) {
                RS_LOGW("RSMainThread::SurfaceOcclusionCallback cannot find surfacenode %{public}"
                    PRIu64 ".", listener.first);
                continue;
            }
            auto appWindowNodeId = node->GetInstanceRootNodeId();
            if (appWindowNodeId == INVALID_NODEID) {
                RS_LOGW("RSMainThread::SurfaceOcclusionCallback surfacenode %{public}"
                    PRIu64 " cannot find app window node.", listener.first);
                continue;
            }
            auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
            auto appWindowNode = nodeMap.GetRenderNode(appWindowNodeId)->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (!surfaceNode || !appWindowNode) {
                RS_LOGW("RSMainThread::SurfaceOcclusionCallback ReinterpretCastTo fail.");
                continue;
            }
            savedAppWindowNode_[listener.first] = std::make_pair(surfaceNode, appWindowNode);
        }
        uint8_t level = 0;
        float visibleAreaRatio = 0.0f;
        bool isOnTheTree = savedAppWindowNode_[listener.first].first->IsOnTheTree();
        if (isOnTheTree) {
            const auto& property = savedAppWindowNode_[listener.first].first->GetRenderProperties();
            auto dstRect = property.GetBoundsGeometry()->GetAbsRect();
            if (dstRect.IsEmpty()) {
                continue;
            }
            visibleAreaRatio = static_cast<float>(savedAppWindowNode_[listener.first].second->
                GetVisibleRegionForCallBack().IntersectArea(dstRect)) /
                static_cast<float>(dstRect.GetWidth() * dstRect.GetHeight());
            auto& partitionVector = std::get<2>(listener.second); // get tuple 2 partition points vector
            bool vectorEmpty = partitionVector.empty();
            if (vectorEmpty && (visibleAreaRatio > 0.0f)) {
                level = 1;
            } else if (!vectorEmpty && ROSEN_EQ(visibleAreaRatio, 1.0f)) {
                level = partitionVector.size();
            } else if (!vectorEmpty && (visibleAreaRatio > 0.0f)) {
                for (auto &point : partitionVector) {
                    if (visibleAreaRatio > point) {
                        level += 1;
                        continue;
                    }
                    break;
                }
            }
        }
        auto& savedLevel = std::get<3>(listener.second); // tuple 3, check visible is changed
        if (savedLevel != level) {
            RS_LOGD("RSMainThread::SurfaceOcclusionCallback surfacenode: %{public}" PRIu64 ".", listener.first);
            savedLevel = level;
            if (isOnTheTree) {
                std::get<1>(listener.second)->OnSurfaceOcclusionVisibleChanged(visibleAreaRatio);
            }
        }
    }
}

bool RSMainThread::WaitHardwareThreadTaskExcute()
{
    std::unique_lock<std::mutex> lock(hardwareThreadTaskMutex_);
    return hardwareThreadTaskCond_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_HARDWARE_THREAD_TASK_TIMEOUT),
        []() { return RSHardwareThread::Instance().GetunExcuteTaskNum() <= HARDWARE_THREAD_TASK_NUM; });
}

void RSMainThread::NotifyHardwareThreadCanExcuteTask()
{
    RS_TRACE_NAME("RSMainThread::NotifyHardwareThreadCanExcuteTask");
    std::lock_guard<std::mutex> lock(hardwareThreadTaskMutex_);
    hardwareThreadTaskCond_.notify_one();
}

void RSMainThread::RequestNextVSync()
{
    RS_OPTIONAL_TRACE_FUNC();
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = [this](uint64_t timestamp, void* data) { OnVsync(timestamp, data); },
    };
    if (receiver_ != nullptr) {
        requestNextVsyncNum_++;
        if (requestNextVsyncNum_ > REQUEST_VSYNC_NUMBER_LIMIT) {
            RS_LOGW("RSMainThread::RequestNextVSync too many times:%{public}d", requestNextVsyncNum_);
        }
        receiver_->RequestNextVSync(fcb);
    }
}

void RSMainThread::OnVsync(uint64_t timestamp, void* data)
{
    RSJankStats::GetInstance().SetStartTime();
    timestamp_ = timestamp;
    curTime_ = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    requestNextVsyncNum_ = 0;
    frameCount_++;
    if (isUniRender_) {
        MergeToEffectiveTransactionDataMap(cachedTransactionDataMap_);
        RSUnmarshalThread::Instance().PostTask(unmarshalBarrierTask_);
    }
    mainLoop_();
    auto screenManager_ = CreateOrGetScreenManager();
    if (screenManager_ != nullptr) {
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            RSHardwareThread::Instance().PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
        } else {
            PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
        }
    }
    RSJankStats::GetInstance().SetEndTime();
}

void RSMainThread::Animate(uint64_t timestamp)
{
    RS_TRACE_FUNC();
    lastAnimateTimestamp_ = timestamp;

    if (context_->animatingNodeList_.empty()) {
        if (doWindowAnimate_ && RSInnovation::UpdateQosVsyncEnabled()) {
            // Preventing Occlusion Calculation from Being Completed in Advance
            RSQosThread::GetInstance()->OnRSVisibilityChangeCB(lastPidVisMap_);
        }
        doWindowAnimate_ = false;
        return;
    }
    doDirectComposition_ = false;
    bool curWinAnim = false;
    bool needRequestNextVsync = false;
    // isCalculateAnimationValue is embedded modify for stat animate frame drop
    bool isCalculateAnimationValue = false;
    bool isCalcPreferredFps = context_->animatingNodeList_.size() > CAL_NODE_PREFERRED_FPS_LIMIT ? false : true;
    bool isDisplaySyncEnabled =
        HgmCore::Instance().GetCurrentRefreshRateMode() == HGM_REFRESHRATE_MODE_AUTO ? true : false;
    int64_t period = 0;
    if (receiver_) {
        receiver_->GetVSyncPeriod(period);
    }
    // iterate and animate all animating nodes, remove if animation finished
    EraseIf(context_->animatingNodeList_,
        [this, timestamp, period, isDisplaySyncEnabled, isCalcPreferredFps,
        &curWinAnim, &needRequestNextVsync, &isCalculateAnimationValue](const auto& iter) -> bool {
        auto node = iter.second.lock();
        if (node == nullptr) {
            RS_LOGD("RSMainThread::Animate removing expired animating node");
            return true;
        }
        node->SetIsCalcPreferredFps(isCalcPreferredFps);
        if (cacheCmdSkippedInfo_.count(ExtractPid(node->GetId())) > 0) {
            RS_LOGD("RSMainThread::Animate skip the cached node");
            return false;
        }
        auto [hasRunningAnimation, nodeNeedRequestNextVsync, nodeCalculateAnimationValue] =
            node->Animate(timestamp, period, isDisplaySyncEnabled);
        if (!hasRunningAnimation) {
            node->InActivateDisplaySync();
            RS_LOGD("RSMainThread::Animate removing finished animating node %{public}" PRIu64, node->GetId());
        }
        // request vsync if: 1. node has running animation, or 2. transition animation just ended
        needRequestNextVsync = needRequestNextVsync || nodeNeedRequestNextVsync || (node.use_count() == 1);
        isCalculateAnimationValue = isCalculateAnimationValue || nodeCalculateAnimationValue;
        if (node->template IsInstanceOf<RSSurfaceRenderNode>() && hasRunningAnimation) {
            if (isUniRender_) {
                auto surfacenode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
                surfacenode->SetAnimateState();
            }
            curWinAnim = true;
        }
        return !hasRunningAnimation;
    });
    if (!doWindowAnimate_ && curWinAnim && RSInnovation::UpdateQosVsyncEnabled()) {
        RSQosThread::ResetQosPid();
    }
    if (!isCalculateAnimationValue && needRequestNextVsync) {
        RS_TRACE_NAME("Animation running empty");
    }

    doWindowAnimate_ = curWinAnim;
    RS_LOGD("RSMainThread::Animate end, animating nodes remains, has window animation: %{public}d", curWinAnim);

    if (needRequestNextVsync) {
        RequestNextVSync();
    }
    PerfAfterAnim(needRequestNextVsync);
}

void RSMainThread::ProcessDataBySingleFrameComposer(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    if (!rsTransactionData || !RSSystemProperties::GetSingleFrameComposerEnabled() ||
        !RSSingleFrameComposer::IsShouldProcessByIpcThread(rsTransactionData->GetSendingPid())) {
        return;
    }

    RSSingleFrameComposer::SetSingleFrameFlag(std::this_thread::get_id());
    if (isUniRender_) {
        context_->transactionTimestamp_ = rsTransactionData->GetTimestamp();
        rsTransactionData->ProcessBySingleFrameComposer(*context_);
    }
}

void RSMainThread::RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    if (!rsTransactionData) {
        return;
    }
    if (isUniRender_) {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        cachedTransactionDataMap_[rsTransactionData->GetSendingPid()].emplace_back(std::move(rsTransactionData));
    } else {
        ClassifyRSTransactionData(rsTransactionData);
    }
    RequestNextVSync();
}

void RSMainThread::ClassifyRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    const auto& nodeMap = context_->GetNodeMap();
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    std::unique_ptr<RSTransactionData> transactionData(std::move(rsTransactionData));
    auto timestamp = transactionData->GetTimestamp();
    RS_LOGD("RSMainThread::RecvRSTransactionData timestamp = %{public}" PRIu64, timestamp);
    for (auto& [nodeId, followType, command] : transactionData->GetPayload()) {
        if (nodeId == 0 || followType == FollowType::NONE) {
            pendingEffectiveCommands_[timestamp].emplace_back(std::move(command));
            continue;
        }
        auto node = nodeMap.GetRenderNode(nodeId);
        if (node && followType == FollowType::FOLLOW_TO_PARENT) {
            auto parentNode = node->GetParent().lock();
            if (parentNode) {
                nodeId = parentNode->GetId();
            } else {
                pendingEffectiveCommands_[timestamp].emplace_back(std::move(command));
                continue;
            }
        }
        cachedCommands_[nodeId][timestamp].emplace_back(std::move(command));
    }
}

void RSMainThread::PostTask(RSTaskMessage::RSTask task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSMainThread::PostTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
    AppExecFwk::EventQueue::Priority priority)
{
    if (handler_) {
        handler_->PostTask(task, name, delayTime, priority);
    }
}

void RSMainThread::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask(name);
    }
}

void RSMainThread::PostSyncTask(RSTaskMessage::RSTask task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

bool RSMainThread::IsIdle() const
{
    return handler_ ? handler_->IsIdle() : false;
}

void RSMainThread::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    applicationAgentMap_.emplace(pid, app);
}

void RSMainThread::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    EraseIf(applicationAgentMap_,
        [&app](const auto& iter) { return iter.second && app && iter.second->AsObject() == app->AsObject(); });
}

void RSMainThread::RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(occlusionMutex_);
    occlusionListeners_[pid] = callback;
}

void RSMainThread::UnRegisterOcclusionChangeCallback(pid_t pid)
{
    std::lock_guard<std::mutex> lock(occlusionMutex_);
    occlusionListeners_.erase(pid);
}

void RSMainThread::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    std::lock_guard<std::mutex> lock(surfaceOcclusionMutex_);
    uint8_t level = 1;
    if (!partitionPoints.empty()) {
        level = partitionPoints.size();
    }
    surfaceOcclusionListeners_[id] = std::make_tuple(pid, callback, partitionPoints, level);
}

void RSMainThread::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    std::lock_guard<std::mutex> lock(surfaceOcclusionMutex_);
    surfaceOcclusionListeners_.erase(id);
    savedAppWindowNode_.erase(id);
}

void RSMainThread::ClearSurfaceOcclusionChangeCallback(pid_t pid)
{
    std::lock_guard<std::mutex> lock(surfaceOcclusionMutex_);
    for (auto it = surfaceOcclusionListeners_.begin(); it != surfaceOcclusionListeners_.end();) {
        if (std::get<0>(it->second) == pid) {
            if (savedAppWindowNode_.find(it->first) != savedAppWindowNode_.end()) {
                savedAppWindowNode_.erase(it->first);
            }
            surfaceOcclusionListeners_.erase(it++);
        } else {
            it++;
        }
    }
}

bool RSMainThread::SurfaceOcclusionCallBackIfOnTreeStateChanged()
{
    std::vector<NodeId> registeredSurfaceOnTree;
    for (auto it = savedAppWindowNode_.begin(); it != savedAppWindowNode_.end(); ++it) {
        if (it->second.first->IsOnTheTree()) {
            registeredSurfaceOnTree.push_back(it->first);
        }
    }
    if (lastRegisteredSurfaceOnTree_ != registeredSurfaceOnTree) {
        lastRegisteredSurfaceOnTree_ = registeredSurfaceOnTree;
        return true;
    }
    return false;
}

void RSMainThread::SendCommands()
{
    RS_OPTIONAL_TRACE_FUNC();
    RsFrameReport& fr = RsFrameReport::GetInstance();
    if (fr.GetEnable()) {
        fr.SendCommandsStart();
        fr.RenderEnd();
    }
    if (!RSMessageProcessor::Instance().HasTransaction()) {
        return;
    }

    // dispatch messages to corresponding application
    auto transactionMapPtr = std::make_shared<std::unordered_map<uint32_t, std::shared_ptr<RSTransactionData>>>(
        RSMessageProcessor::Instance().GetAllTransactions());
    RSMessageProcessor::Instance().ReInitializeMovedMap();
    PostTask([this, transactionMapPtr]() {
        for (auto& transactionIter : *transactionMapPtr) {
            auto pid = transactionIter.first;
            auto appIter = applicationAgentMap_.find(pid);
            if (appIter == applicationAgentMap_.end()) {
                RS_LOGW("RSMainThread::SendCommand no application agent registered as pid %{public}d,"
                    "this will cause memory leak!", pid);
                continue;
            }
            auto& app = appIter->second;
            auto transactionPtr = transactionIter.second;
            app->OnTransaction(transactionPtr);
        }
    });
}

void RSMainThread::QosStateDump(std::string& dumpString)
{
    if (RSQosThread::GetInstance()->GetQosCal()) {
        dumpString.append("QOS is enabled\n");
    } else {
        dumpString.append("QOS is disabled\n");
    }
}

void RSMainThread::RenderServiceTreeDump(std::string& dumpString)
{
    dumpString.append("Animating Node: [");
    for (auto& [nodeId, _]: context_->animatingNodeList_) {
        dumpString.append(std::to_string(nodeId) + ", ");
    }
    dumpString.append("];\n");
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        dumpString.append("rootNode is null\n");
        return;
    }
    rootNode->DumpTree(0, dumpString);
}

bool RSMainThread::DoParallelComposition(std::shared_ptr<RSBaseRenderNode> rootNode)
{
    using CreateParallelSyncSignalFunc = void* (*)(uint32_t);
    using SignalCountDownFunc = void (*)(void*);
    using SignalAwaitFunc = void (*)(void*);
    using AssignTaskFunc = void (*)(std::function<void()>);
    using RemoveStoppedThreadsFunc = void (*)();

    auto CreateParallelSyncSignal = (CreateParallelSyncSignalFunc)RSInnovation::_s_createParallelSyncSignal;
    auto SignalCountDown = (SignalCountDownFunc)RSInnovation::_s_signalCountDown;
    auto SignalAwait = (SignalAwaitFunc)RSInnovation::_s_signalAwait;
    auto AssignTask = (AssignTaskFunc)RSInnovation::_s_assignTask;
    auto RemoveStoppedThreads = (RemoveStoppedThreadsFunc)RSInnovation::_s_removeStoppedThreads;

    void* syncSignal = (*CreateParallelSyncSignal)(rootNode->GetChildrenCount());
    if (!syncSignal) {
        return false;
    }

    (*RemoveStoppedThreads)();

    auto children = rootNode->GetSortedChildren();
    bool animate_ = doWindowAnimate_;
    for (auto it = children.rbegin(); it != children.rend(); it++) {
        auto child = *it;
        auto task = [&syncSignal, SignalCountDown, child, animate_]() {
            std::shared_ptr<RSNodeVisitor> visitor;
            auto rsVisitor = std::make_shared<RSRenderServiceVisitor>(true);
            rsVisitor->SetAnimateState(animate_);
            visitor = rsVisitor;
            child->Process(visitor);
            (*SignalCountDown)(syncSignal);
        };
        if (*it == *children.begin()) {
            task();
        } else {
            (*AssignTask)(task);
        }
    }
    (*SignalAwait)(syncSignal);
    return true;
}

void RSMainThread::ClearTransactionDataPidInfo(pid_t remotePid)
{
    if (!isUniRender_) {
        return;
    }
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    if (effectiveTransactionDataIndexMap_.count(remotePid) > 0 &&
        !effectiveTransactionDataIndexMap_[remotePid].second.empty()) {
        RS_LOGD("RSMainThread::ClearTransactionDataPidInfo process:%{public}d destroyed, skip commands", remotePid);
    }
    effectiveTransactionDataIndexMap_.erase(remotePid);
    transactionDataLastWaitTime_.erase(remotePid);

    // clear cpu cache when process exit
    // CLEAN_CACHE_FREQ to prevent multiple cleanups in a short period of time
    if ((timestamp_ - lastCleanCacheTimestamp_) / REFRESH_PERIOD > CLEAN_CACHE_FREQ) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        RS_LOGD("RSMainThread: clear cpu cache pid:%{public}d", remotePid);
        if (IsRenderedProcess(remotePid)) {
            ClearMemoryCache(true);
        }
        lastCleanCacheTimestamp_ = timestamp_;
#endif
    }
}

bool RSMainThread::IsRenderedProcess(pid_t pid) const
{
    bool isRenderProcess = false;
    auto task = [&isRenderProcess, pid](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) {
        if (!surfaceNode) {
            return;
        }
        if (ExtractPid(surfaceNode->GetId()) == pid) {
            isRenderProcess = true;
            return;
        }
    };
    context_->GetNodeMap().TraverseSurfaceNodes(task);
    return isRenderProcess;
}

void RSMainThread::TrimMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!RSUniRenderJudgement::IsUniRender()) {
        dumpString.append("\n---------------\nNot in UniRender and no resource can be released");
        return;
    }
    std::string type;
    argSets.erase(u"trimMem");
    if (!argSets.empty()) {
        type = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
    }
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    auto grContext = GetRenderEngine()->GetDrawingContext()->GetDrawingContext();
#else
    auto grContext = GetRenderEngine()->GetRenderContext()->GetGrContext();
#endif
    if (type.empty()) {
        grContext->flush();
        SkGraphics::PurgeAllCaches();
        grContext->freeGpuResources();
        grContext->purgeUnlockedResources(true);
#ifdef NEW_RENDER_CONTEXT
        MemoryHandler::ClearShader();
#else
        std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
        rendercontext->CleanAllShaderCache();
#endif
        grContext->flushAndSubmit(true);
    } else if (type == "cpu") {
        grContext->flush();
        SkGraphics::PurgeAllCaches();
        grContext->flushAndSubmit(true);
    } else if (type == "gpu") {
        grContext->flush();
        grContext->freeGpuResources();
        grContext->flushAndSubmit(true);
    } else if (type == "uihidden") {
        grContext->flush();
        grContext->purgeUnlockedResources(true);
        grContext->flushAndSubmit(true);
    } else if (type == "shader") {
#ifdef NEW_RENDER_CONTEXT
        MemoryHandler::ClearShader();
#else
        std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
        rendercontext->CleanAllShaderCache();
#endif
    } else if (type == "flushcache") {
        int ret = mallopt(M_FLUSH_THREAD_CACHE, 0);
        dumpString.append("flushcache " + std::to_string(ret) + "\n");
    } else {
        uint32_t pid = static_cast<uint32_t>(std::stoll(type));
        GrGpuResourceTag tag(pid, 0, 0, 0);
        MemoryManager::ReleaseAllGpuResource(grContext, tag);
    }
    dumpString.append("trimMem: " + type + "\n");
#else
#ifdef NEW_RENDER_CONTEXT
    auto grContext = GetRenderEngine()->GetDrawingContext()->GetDrawingContext();
#else
    auto grContext = GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
#endif
    if (type.empty()) {
        gpuContext->Flush();
        SkGraphics::PurgeAllCaches();
        gpuContext->FreeGpuResources();
        gpuContext->PurgeUnlockedResources(true);
#ifdef NEW_RENDER_CONTEXT
        MemoryHandler::ClearShader();
#else
        std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
        rendercontext->CleanAllShaderCache();
#endif
        gpuContext->FlushAndSubmit(true);
    } else if (type == "cpu") {
        gpuContext->Flush();
        SkGraphics::PurgeAllCaches();
        gpuContext->FlushAndSubmit(true);
    } else if (type == "gpu") {
        gpuContext->Flush();
        gpuContext->FreeGpuResources();
        gpuContext->FlushAndSubmit(true);
    } else if (type == "uihidden") {
        gpuContext->Flush();
        gpuContext->PurgeUnlockedResources(true);
        gpuContext->FlushAndSubmit(true);
    } else if (type == "shader") {
#ifdef NEW_RENDER_CONTEXT
        MemoryHandler::ClearShader();
#else
        std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
        rendercontext->CleanAllShaderCache();
#endif
    } else if (type == "flushcache") {
        int ret = mallopt(M_FLUSH_THREAD_CACHE, 0);
        dumpString.append("flushcache " + std::to_string(ret) + "\n");
    } else {
        uint32_t pid = static_cast<uint32_t>(std::stoll(type));
        Drawing::GPUResourceTag tag(pid, 0, 0, 0);
        MemoryManager::ReleaseAllGpuResource(gpuContext, tag);
    }
    dumpString.append("trimMem: " + type + "\n");
#endif
#endif
}

void RSMainThread::DumpNode(std::string& result, uint64_t nodeId) const
{
    const auto& nodeMap = context_->GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (!node) {
        result.append("have no this node");
        return;
    }
    DfxString log;
    node->DumpNodeInfo(log);
    result.append(log.GetString());
}

void RSMainThread::DumpMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString,
    std::string& type, int pid)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    DfxString log;
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    auto grContext = GetRenderEngine()->GetDrawingContext()->GetDrawingContext();
#else
    auto grContext = GetRenderEngine()->GetRenderContext()->GetGrContext();
#endif
    if (pid != 0) {
        MemoryManager::DumpPidMemory(log, pid, grContext);
    } else {
        MemoryManager::DumpMemoryUsage(log, grContext, type);
    }
#else
    auto gpuContext = GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
    if (gpuContext != nullptr) {
        if (pid != 0) {
            MemoryManager::DumpPidMemory(log, pid, gpuContext);
        } else {
            MemoryManager::DumpMemoryUsage(log, gpuContext, type);
        }
    }
#endif
    if (type.empty() || type == MEM_GPU_TYPE) {
        auto subThreadManager = RSSubThreadManager::Instance();
        if (subThreadManager) {
            subThreadManager->DumpMem(log);
        }
    }
    dumpString.append("dumpMem: " + type + "\n");
    dumpString.append(log.GetString());
#else
    dumpString.append("No GPU in this device");
#endif
}

void RSMainThread::CountMem(int pid, MemoryGraphic& mem)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    mem = MemoryManager::CountPidMemory(pid, GetRenderEngine()->GetDrawingContext()->GetDrawingContext());
    mem += MemoryManager::CountSubMemory(pid, GetRenderEngine()->GetDrawingContext()->GetDrawingContext());
#else
    mem = MemoryManager::CountPidMemory(pid, GetRenderEngine()->GetRenderContext()->GetGrContext());
    mem += MemoryManager::CountSubMemory(pid, GetRenderEngine()->GetRenderContext()->GetGrContext());
#endif
#else
    mem = MemoryManager::CountPidMemory(pid, GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
    mem += MemoryManager::CountSubMemory(pid, GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
#endif
#endif
}

void RSMainThread::CountMem(std::vector<MemoryGraphic>& mems)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!context_) {
        RS_LOGE("RSMainThread::CountMem Context is nullptr");
        return;
    }
    const auto& nodeMap = context_->GetNodeMap();
    std::vector<pid_t> pids;
    nodeMap.TraverseSurfaceNodes([&pids] (const std::shared_ptr<RSSurfaceRenderNode>& node) {
        auto pid = ExtractPid(node->GetId());
        if (std::find(pids.begin(), pids.end(), pid) == pids.end()) {
            pids.emplace_back(pid);
        }
    });
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    MemoryManager::CountMemory(pids, GetRenderEngine()->GetDrawingContext()->GetDrawingContext(), mems);
#else
    MemoryManager::CountMemory(pids, GetRenderEngine()->GetRenderContext()->GetGrContext(), mems);
#endif
#else
    MemoryManager::CountMemory(pids, GetRenderEngine()->GetRenderContext()->GetDrGPUContext(), mems);
#endif
#endif
}

void RSMainThread::AddTransactionDataPidInfo(pid_t remotePid)
{
    if (!isUniRender_) {
        return;
    }
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    if (effectiveTransactionDataIndexMap_.count(remotePid) > 0) {
        RS_LOGW("RSMainThread::AddTransactionDataPidInfo remotePid:%{public}d already exists", remotePid);
    }
    effectiveTransactionDataIndexMap_[remotePid].first = 0;
}

void RSMainThread::SetDirtyFlag()
{
    isDirty_ = true;
}

void RSMainThread::SetAccessibilityConfigChanged()
{
    isAccessibilityConfigChanged_ = true;
}

void RSMainThread::PerfAfterAnim(bool needRequestNextVsync)
{
    if (!isUniRender_) {
        return;
    }
    if (needRequestNextVsync && timestamp_ - prePerfTimestamp_ > PERF_PERIOD) {
        RS_LOGD("RSMainThread:: soc perf to render_service_animation");
        prePerfTimestamp_ = timestamp_;
    } else if (!needRequestNextVsync && prePerfTimestamp_) {
        RS_LOGD("RSMainThread:: soc perf off render_service_animation");
        prePerfTimestamp_ = 0;
    }
}

void RSMainThread::ForceRefreshForUni()
{
    if (isUniRender_) {
        PostTask([=]() {
            MergeToEffectiveTransactionDataMap(cachedTransactionDataMap_);
            RSUnmarshalThread::Instance().PostTask(unmarshalBarrierTask_);
            auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            timestamp_ = timestamp_ + (now - curTime_);
            curTime_ = now;
            RS_TRACE_NAME("RSMainThread::ForceRefreshForUni timestamp:" + std::to_string(timestamp_));
            mainLoop_();
        });
        auto screenManager_ = CreateOrGetScreenManager();
        if (screenManager_ != nullptr) {
            auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
            if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
                RSHardwareThread::Instance().PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
            } else {
                PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
            }
        }
    } else {
        RequestNextVSync();
    }
}

void RSMainThread::PerfForBlurIfNeeded()
{
    static int preBlurCnt = 0;
    int blurCnt = RSPropertiesPainter::GetAndResetBlurCnt();
    // clamp blurCnt to 0~3.
    blurCnt = std::clamp<int>(blurCnt, 0, 3);
    if (blurCnt != preBlurCnt && preBlurCnt != 0) {
        PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(preBlurCnt), false);
        preBlurCnt = 0;
    }
    if (blurCnt == 0) {
        return;
    }
    static uint64_t prePerfTimestamp = 0;
    if (timestamp_ - prePerfTimestamp > PERF_PERIOD_BLUR || blurCnt != preBlurCnt) {
        PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(blurCnt), true);
        prePerfTimestamp = timestamp_;
        preBlurCnt = blurCnt;
    }
}

void RSMainThread::PerfMultiWindow()
{
    if (!isUniRender_) {
        return;
    }
    static uint64_t lastPerfTimestamp = 0;
    if (appWindowNum_ >= MULTI_WINDOW_PERF_START_NUM && appWindowNum_ <= MULTI_WINDOW_PERF_END_NUM
        && timestamp_ - lastPerfTimestamp > PERF_PERIOD_MULTI_WINDOW) {
        RS_LOGD("RSMainThread::PerfMultiWindow soc perf");
        PerfRequest(PERF_MULTI_WINDOW_REQUESTED_CODE, true);
        lastPerfTimestamp = timestamp_;
    } else if ((appWindowNum_ < MULTI_WINDOW_PERF_START_NUM || appWindowNum_ > MULTI_WINDOW_PERF_END_NUM)
        && timestamp_ - lastPerfTimestamp < PERF_PERIOD_MULTI_WINDOW) {
        RS_LOGD("RSMainThread::PerfMultiWindow soc perf off");
        PerfRequest(PERF_MULTI_WINDOW_REQUESTED_CODE, false);
    }
}

void RSMainThread::RenderFrameStart(uint64_t timestamp)
{
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().RenderStart(timestamp);
    }
    RenderFrameTrace::GetInstance().RenderStartFrameTrace(RS_INTERVAL_NAME);
}

void RSMainThread::SetAppWindowNum(uint32_t num)
{
    appWindowNum_ = num;
}

bool RSMainThread::CheckNodeHasToBePreparedByPid(NodeId nodeId, bool isClassifyByRoot)
{
    if (context_->activeNodesInRoot_.empty() || nodeId == INVALID_NODEID) {
        return false;
    }
    if (!isClassifyByRoot) {
        // Match by PID
        auto pid = ExtractPid(nodeId);
        return std::any_of(context_->activeNodesInRoot_.begin(), context_->activeNodesInRoot_.end(),
            [pid](const auto& iter) { return ExtractPid(iter.first) == pid; });
    } else {
        return context_->activeNodesInRoot_.count(nodeId);
    }
}

bool RSMainThread::IsDrawingGroupChanged(RSRenderNode& cacheRootNode) const
{
    auto iter = context_->activeNodesInRoot_.find(cacheRootNode.GetInstanceRootNodeId());
    if (iter != context_->activeNodesInRoot_.end()) {
        const std::unordered_map<NodeId, std::shared_ptr<RSRenderNode>>& activeNodeIds = iter->second;
        // do not need to check cacheroot node itself
        // in case of tree change, parent node would set content dirty and reject before
        auto cacheRootId = cacheRootNode.GetId();
        auto groupNodeIds = cacheRootNode.GetVisitedCacheRootIds();
        for (auto [id, subNode] : activeNodeIds) {
            if (subNode == nullptr || id == cacheRootId) {
                continue;
            }
            if (groupNodeIds.find(subNode->GetDrawingCacheRootId()) != groupNodeIds.end()) {
                return true;
            }
        }
    }
    return false;
}

bool RSMainThread::CheckIfInstanceOnlySurfaceBasicGeoTransform(NodeId instanceNodeId) const
{
    if (instanceNodeId == INVALID_NODEID) {
        RS_LOGE("CheckIfInstanceOnlySurfaceBasicGeoTransform instanceNodeId invalid.");
        return false;
    }
    auto iter = context_->activeNodesInRoot_.find(instanceNodeId);
    if (iter != context_->activeNodesInRoot_.end()) {
        const std::unordered_map<NodeId, std::shared_ptr<RSRenderNode>>& activeNodeIds = iter->second;
        for (auto [id, subNode] : activeNodeIds) {
            if (subNode == nullptr) {
                continue;
            }
            // filter active nodes except instance surface itself
            if (id != instanceNodeId || !subNode->IsOnlyBasicGeoTransfrom()) {
                return false;
            }
        }
    }
    return true;
}

FrameRateRange RSMainThread::CalcAnimateFrameRateRange(std::shared_ptr<RSRenderNode> node)
{
    int32_t preferredFps = 0;
    if (node->IsCalcPreferredFps()) {
        for (auto &profile : node->GetHgmModifierProfileList()) {
            auto modifierPreferred = frameRateMgr_->CalModifierPreferred(profile);
            preferredFps = std::max(preferredFps, modifierPreferred);
        }
    } else {
        preferredFps = OLED_120_HZ;
    }
    return node->CalcExpectedFrameRateRange(preferredFps);
}

void RSMainThread::ApplyModifiers()
{
    rsCurrRange_.Reset();
    if (context_->activeNodesInRoot_.empty()) {
        return;
    }
    RS_TRACE_NAME_FMT("ApplyModifiers (PropertyDrawableEnable %s)",
        RSSystemProperties::GetPropertyDrawableEnable() ? "TRUE" : "FALSE");
    for (const auto& [root, nodeSet] : context_->activeNodesInRoot_) {
        for (const auto& [id, nodePtr] : nodeSet) {
            bool isZOrderChanged = nodePtr->ApplyModifiers();
            rsCurrRange_.Merge(CalcAnimateFrameRateRange(nodePtr));
            if (!isZOrderChanged) {
                continue;
            }
            if (auto parent = nodePtr->GetParent().lock()) {
                parent->isChildrenSorted_ = false;
            }
        }
    }
}

void RSMainThread::ResetHardwareEnabledState()
{
    isHardwareForcedDisabled_ = !RSSystemProperties::GetHardwareComposerEnabled();
    doDirectComposition_ = !isHardwareForcedDisabled_;
    isHardwareEnabledBufferUpdated_ = false;
    hardwareEnabledNodes_.clear();
    selfDrawingNodes_.clear();
}

const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& RSMainThread::GetSelfDrawingNodes() const
{
    return selfDrawingNodes_;
}

void RSMainThread::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    std::lock_guard<std::mutex> lock(watermarkMutex_);
    isShow_ = isShow;
    if (isShow_) {
#ifndef USE_ROSEN_DRAWING
        watermarkImg_ = RSPixelMapUtil::ExtractSkImage(std::move(watermarkImg));
#else
        watermarkImg_ = RSPixelMapUtil::ExtractDrawingImage(std::move(watermarkImg));
#endif
    } else {
        watermarkImg_ = nullptr;
    }
    SetDirtyFlag();
    RequestNextVSync();
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> RSMainThread::GetWatermarkImg()
#else
std::shared_ptr<Drawing::Image> RSMainThread::GetWatermarkImg()
#endif
{
    return watermarkImg_;
}

bool RSMainThread::GetWatermarkFlag()
{
    return isShow_;
}

bool RSMainThread::IsSingleDisplay()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        RS_LOGE("RSMainThread::IsSingleDisplay GetGlobalRootRenderNode fail");
        return false;
    }
    return rootNode->GetChildrenCount() == 1;
}

void RSMainThread::UpdateRogSizeIfNeeded()
{
    if (!RSSystemProperties::IsPhoneType() || RSSystemProperties::IsFoldScreenFlag()) {
        return;
    }
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (!rootNode) {
        return;
    }
    std::list<RSBaseRenderNode::SharedPtr> children = rootNode->GetSortedChildren();
    if (!children.empty()) {
        auto child = children.front();
        if (child != nullptr && child->IsInstanceOf<RSDisplayRenderNode>()) {
            auto displayNode = child->ReinterpretCastTo<RSDisplayRenderNode>();
            if (displayNode) {
                auto screenManager_ = CreateOrGetScreenManager();
                screenManager_->SetRogScreenResolution(displayNode->GetScreenId(),
                    displayNode->GetRogWidth(), displayNode->GetRogHeight());
            }
        }
    }
}

const uint32_t UIFIRST_MINIMUM_NODE_NUMBER = 12; // minimum window number(12) for enabling UIFirst

void RSMainThread::UpdateUIFirstSwitch()
{
    if (deviceType_ == DeviceType::PHONE) {
        isUiFirstOn_ = (RSSystemProperties::GetUIFirstEnabled() && IsSingleDisplay());
        return;
    }
    isUiFirstOn_ = false;
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode && IsSingleDisplay()) {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(
            rootNode->GetSortedChildren().front());
        if (displayNode) {
            uint32_t childrenCount = 0;
            if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
                std::vector<RSBaseRenderNode::SharedPtr> curAllSurfacesVec;
                displayNode->CollectSurface(displayNode, curAllSurfacesVec, true, true);
                childrenCount = curAllSurfacesVec.size();
            } else {
                childrenCount = displayNode->GetChildrenCount();
            }
            isUiFirstOn_ = RSSystemProperties::GetUIFirstEnabled() &&
                (childrenCount >= UIFIRST_MINIMUM_NODE_NUMBER);
        }
    }
}

bool RSMainThread::IsUIFirstOn() const
{
    return isUiFirstOn_;
}

void RSMainThread::ReleaseSurface()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (tmpSurfaces_.size() > 0) {
        auto tmp = tmpSurfaces_.front();
        tmpSurfaces_.pop();
        tmp = nullptr;
    }
}

#ifndef USE_ROSEN_DRAWING
void RSMainThread::AddToReleaseQueue(sk_sp<SkSurface>&& surface)
#else
void RSMainThread::AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface)
#endif
{
    std::lock_guard<std::mutex> lock(mutex_);
    tmpSurfaces_.push(std::move(surface));
}

void RSMainThread::GetAppMemoryInMB(float& cpuMemSize, float& gpuMemSize)
{
    PostSyncTask([&cpuMemSize, &gpuMemSize, this]() {
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
        gpuMemSize = MemoryManager::GetAppGpuMemoryInMB(GetRenderEngine()->GetDrawingContext()->GetDrawingContext());
#else
        gpuMemSize = MemoryManager::GetAppGpuMemoryInMB(GetRenderEngine()->GetRenderContext()->GetGrContext());
#endif
#else
        gpuMemSize = MemoryManager::GetAppGpuMemoryInMB(GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
#endif
        cpuMemSize = MemoryTrack::Instance().GetAppMemorySizeInMB();
    });
}

void RSMainThread::SubscribeAppState()
{
    PostTask([this]() {
        rsAppStateListener_ = std::make_shared<RSAppStateListener>();
        if (Memory::MemMgrClient::GetInstance().SubscribeAppState(*rsAppStateListener_) != -1) {
            RS_LOGD("Subscribe MemMgr Success");
            subscribeFailCount_ = 0;
            return;
        } else {
            RS_LOGE("Subscribe Failed, try again");
            subscribeFailCount_++;
            if (subscribeFailCount_ < 10) { // The maximum number of failures is 10
                SubscribeAppState();
            } else {
                RS_LOGE("Subscribe Failed 10 times, exiting");
            }
        }
    }, MEM_MGR, WAIT_FOR_MEM_MGR_SERVICE);
}

void RSMainThread::HandleOnTrim(Memory::SystemMemoryLevel level)
{
    if (handler_) {
        handler_->PostTask(
            [level, this]() {
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
                auto grContext = GetRenderEngine()->GetDrawingContext()->GetDrawingContext();
#else
                auto grContext = GetRenderEngine()->GetRenderContext()->GetGrContext();
#endif
#else
                auto grContext = GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
#endif
                RS_LOGD("Enter level:%{public}d, OnTrim Success", level);
                switch (level) {
                    case Memory::SystemMemoryLevel::MEMORY_LEVEL_CRITICAL:
                    case Memory::SystemMemoryLevel::MEMORY_LEVEL_LOW:
                    case Memory::SystemMemoryLevel::MEMORY_LEVEL_MODERATE:
                    case Memory::SystemMemoryLevel::MEMORY_LEVEL_PURGEABLE:
                        MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(grContext);
                        break;
                    default:
                        break;
                }
            },
            AppExecFwk::EventQueue::Priority::IDLE);
    }
}

} // namespace Rosen
} // namespace OHOS
