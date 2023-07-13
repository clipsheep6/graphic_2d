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

#include <functional>

#include "rs_interfaces.h"
#include "rs_trace.h"

#include "platform/common/rs_system_properties.h"
#include "pipeline/rs_divided_ui_capture.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "ui/rs_proxy_node.h"

namespace OHOS {
namespace Rosen {
RSInterfaces &RSInterfaces::GetInstance()
{
    static RSInterfaces instance;
    return instance;
}

RSInterfaces::RSInterfaces() : renderServiceClient_(std::make_unique<RSRenderServiceClient>())
{
}

RSInterfaces::~RSInterfaces() noexcept
{
}

int32_t RSInterfaces::SetFocusAppInfo(FocusAppInfo& info)
{
    int32_t pid = info.pid;
    int32_t uid = info.uid;
    const std::string bundleName = info.bundleName;
    const std::string abilityName = info.abilityName;
    uint64_t focusNodeId = info.focusNodeId;
    return renderServiceClient_->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
}

ScreenId RSInterfaces::GetDefaultScreenId()
{
    return renderServiceClient_->GetDefaultScreenId();
}

std::vector<ScreenId> RSInterfaces::GetAllScreenIds()
{
    return renderServiceClient_->GetAllScreenIds();
}

#ifndef ROSEN_CROSS_PLATFORM
ScreenId RSInterfaces::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int flags)
{
    return renderServiceClient_->CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
}

int32_t RSInterfaces::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    return renderServiceClient_->SetVirtualScreenSurface(id, surface);
}
#endif

void RSInterfaces::RemoveVirtualScreen(ScreenId id)
{
    renderServiceClient_->RemoveVirtualScreen(id);
}

int32_t RSInterfaces::SetScreenChangeCallback(const ScreenChangeCallback &callback)
{
    return renderServiceClient_->SetScreenChangeCallback(callback);
}

bool RSInterfaces::TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    if (!node) {
        ROSEN_LOGW("node is nullptr");
        return false;
    }
    return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, scaleX, scaleY);
}

bool RSInterfaces::TakeSurfaceCapture(std::shared_ptr<RSDisplayNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    if (!node) {
        ROSEN_LOGW("node is nullptr");
        return false;
    }
    return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, scaleX, scaleY);
}

bool RSInterfaces::TakeSurfaceCapture(NodeId id,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    return renderServiceClient_->TakeSurfaceCapture(id, callback, scaleX, scaleY);
}

void RSInterfaces::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    renderServiceClient_->SetScreenActiveMode(id, modeId);
}

void RSInterfaces::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    renderServiceClient_->SetScreenRefreshRate(id, sceneId, rate);
}

void RSInterfaces::SetRefreshRateMode(int32_t refreshRateMode)
{
    renderServiceClient_->SetRefreshRateMode(refreshRateMode);
}

uint32_t RSInterfaces::GetScreenCurrentRefreshRate(ScreenId id)
{
    return renderServiceClient_->GetScreenCurrentRefreshRate(id);
}

std::vector<uint32_t> RSInterfaces::GetScreenSupportedRefreshRates(ScreenId id)
{
    return renderServiceClient_->GetScreenSupportedRefreshRates(id);
}

bool RSInterfaces::TakeSurfaceCaptureForUI(
    std::shared_ptr<RSNode> node, std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    if (!node) {
        ROSEN_LOGW("RSInterfaces::TakeSurfaceCaptureForUI rsnode is nullpter return");
        return false;
    }
    if (!((node->GetType() == RSUINodeType::ROOT_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_NODE) ||
          (node->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSInterfaces::TakeSurfaceCaptureForUI unsupported node type return");
        return false;
    }
    if (RSSystemProperties::GetUniRenderEnabled()) {
        return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, scaleX, scaleY);
    } else {
        return TakeSurfaceCaptureForUIWithoutUni(node->GetId(), callback, scaleX, scaleY);
    }
}

int32_t RSInterfaces::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    return renderServiceClient_->SetVirtualScreenResolution(id, width, height);
}

RSVirtualScreenResolution RSInterfaces::GetVirtualScreenResolution(ScreenId id)
{
    return renderServiceClient_->GetVirtualScreenResolution(id);
}

void RSInterfaces::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    renderServiceClient_->SetScreenPowerStatus(id, status);
}

bool RSInterfaces::TakeSurfaceCaptureForUIWithoutUni(NodeId id,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    std::function<void()> offscreenRenderTask = [scaleX, scaleY, callback, id, this]() -> void {
        ROSEN_LOGD(
            "RSInterfaces::TakeSurfaceCaptureForUIWithoutUni callback->OnOffscreenRender nodeId:[%" PRIu64 "]", id);
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderThread::TakeSurfaceCaptureForUIWithoutUni");
        std::shared_ptr<RSDividedUICapture> rsDividedUICapture =
            std::make_shared<RSDividedUICapture>(id, scaleX, scaleY);
        std::shared_ptr<Media::PixelMap> pixelmap = rsDividedUICapture->TakeLocalCapture();
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        callback->OnSurfaceCapture(pixelmap);
        std::lock_guard<std::mutex> lock(offscreenRenderMutex_);
        offscreenRenderNum_--;
        if (offscreenRenderNum_ == 0) {
            RSOffscreenRenderThread::Instance().Stop();
        }
    };
    {
        std::lock_guard<std::mutex> lock(offscreenRenderMutex_);
        if (offscreenRenderNum_ == 0) {
            RSOffscreenRenderThread::Instance().Start();
        }
        offscreenRenderNum_++;
    }
    RSOffscreenRenderThread::Instance().PostTask(offscreenRenderTask);
    return true;
}

RSScreenModeInfo RSInterfaces::GetScreenActiveMode(ScreenId id)
{
    return renderServiceClient_->GetScreenActiveMode(id);
}

std::vector<RSScreenModeInfo> RSInterfaces::GetScreenSupportedModes(ScreenId id)
{
    return renderServiceClient_->GetScreenSupportedModes(id);
}

RSScreenCapability RSInterfaces::GetScreenCapability(ScreenId id)
{
    return renderServiceClient_->GetScreenCapability(id);
}

ScreenPowerStatus RSInterfaces::GetScreenPowerStatus(ScreenId id)
{
    return renderServiceClient_->GetScreenPowerStatus(id);
}

RSScreenData RSInterfaces::GetScreenData(ScreenId id)
{
    return renderServiceClient_->GetScreenData(id);
}

int32_t RSInterfaces::GetScreenBacklight(ScreenId id)
{
    return renderServiceClient_->GetScreenBacklight(id);
}

void RSInterfaces::SetScreenBacklight(ScreenId id, uint32_t level)
{
    renderServiceClient_->SetScreenBacklight(id, level);
}

int32_t RSInterfaces::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    return renderServiceClient_->GetScreenSupportedColorGamuts(id, mode);
}

int32_t RSInterfaces::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    return renderServiceClient_->GetScreenSupportedMetaDataKeys(id, keys);
}

int32_t RSInterfaces::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    return renderServiceClient_->GetScreenColorGamut(id, mode);
}

int32_t RSInterfaces::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    return renderServiceClient_->SetScreenColorGamut(id, modeIdx);
}

int32_t RSInterfaces::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    return renderServiceClient_->SetScreenGamutMap(id, mode);
}

int32_t RSInterfaces::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    return renderServiceClient_->GetScreenGamutMap(id, mode);
}

std::shared_ptr<VSyncReceiver> RSInterfaces::CreateVSyncReceiver(
    const std::string& name,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper)
{
    return renderServiceClient_->CreateVSyncReceiver(name, looper);
}

int32_t RSInterfaces::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    return renderServiceClient_->GetScreenHDRCapability(id, screenHdrCapability);
}

int32_t RSInterfaces::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    return renderServiceClient_->GetScreenType(id, screenType);
}

int32_t RSInterfaces::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    return renderServiceClient_->SetScreenSkipFrameInterval(id, skipFrameInterval);
}

int32_t RSInterfaces::RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback)
{
    return renderServiceClient_->RegisterOcclusionChangeCallback(callback);
}

void RSInterfaces::SetAppWindowNum(uint32_t num)
{
    renderServiceClient_->SetAppWindowNum(num);
}

void RSInterfaces::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    renderServiceClient_->ShowWatermark(watermarkImg, isShow);
}

MemoryGraphic RSInterfaces::GetMemoryGraphic(int pid)
{
    return renderServiceClient_->GetMemoryGraphic(pid);
}

std::vector<MemoryGraphic> RSInterfaces::GetMemoryGraphics()
{
    return renderServiceClient_->GetMemoryGraphics();
}

void RSInterfaces::ReportJankStats()
{
    renderServiceClient_->ReportJankStats();
}

void RSInterfaces::ReportEventResponse(DataBaseRs info)
{
    renderServiceClient_->ReportEventResponse(info);
}

void RSInterfaces::ReportEventComplete(DataBaseRs info)
{
    renderServiceClient_->ReportEventComplete(info);
}

void RSInterfaces::ReportEventJankFrame(DataBaseRs info)
{
    renderServiceClient_->ReportEventJankFrame(info);
}

void RSInterfaces::ReportEventFirstFrame(DataBaseRs info)
{
    renderServiceClient_->ReportEventFirstFrame(info);
}
} // namespace Rosen
} // namespace OHOS
