/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_backend.h"

#include <scoped_bytrace.h>
#include "surface_buffer.h"

namespace OHOS {
namespace Rosen {

HdiBackend* HdiBackend::GetInstance()
{
    static HdiBackend instance;

    return &instance;
}

RosenError HdiBackend::RegScreenHotplug(OnScreenHotplugFunc func, void* data)
{
    if (func == nullptr) {
        HLOGE("OnScreenHotplugFunc is null");
        return ROSEN_ERROR_INVALID_ARGUMENTS;
    }

    onScreenHotplugCb_ = func;
    onHotPlugCbData_ = data;

    RosenError retCode = InitDevice();
    if (retCode != ROSEN_ERROR_OK) {
        return retCode;
    }

    int32_t ret = device_->RegHotPlugCallback(HdiBackend::OnHdiBackendHotPlugEvent, this);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("RegHotPlugCallback failed, ret is %{public}d", ret);
        return ROSEN_ERROR_API_FAILED;
    }

    return ROSEN_ERROR_OK;
}

RosenError HdiBackend::RegSeamlessChange(OnSeamlessChangeFunc callback, void *data)
{
    if (callback == nullptr) {
        HLOGE("OnSeamlessChangeFunc is null");
        return ROSEN_ERROR_INVALID_ARGUMENTS;
    }

    onSeamlessChangeCb_ = callback;
    onSeamlessChangeData_ = data;

    RosenError retCode = InitDevice();
    if (retCode != ROSEN_ERROR_OK) {
        return retCode;
    }

    int32_t ret = device_->RegSeamlessChangeCallback(HdiBackend::OnSeamlessChangeEvent, this);
    if (!ret) {
        HLOGE("RegSeamlessChangeCallback failed, ret is %{public}d", ret);
        return ROSEN_ERROR_API_FAILED;
    }

    return ROSEN_ERROR_OK;
}

RosenError HdiBackend::RegPrepareComplete(OnPrepareCompleteFunc func, void* data)
{
    if (func == nullptr) {
        HLOGE("OnPrepareCompleteFunc is null");
        return ROSEN_ERROR_INVALID_ARGUMENTS;
    }

    onPrepareCompleteCb_ = func;
    onPrepareCompleteCbData_ = data;

    return ROSEN_ERROR_OK;
}

RosenError HdiBackend::RegHwcDeadListener(OnHwcDeadCallback func, void* data)
{
    if (func == nullptr) {
        HLOGE("onHwcDeadCallbackFunc is null.");
        return ROSEN_ERROR_INVALID_ARGUMENTS;
    }

    RosenError retCode = InitDevice();
    if (retCode != ROSEN_ERROR_OK) {
        return retCode;
    }

    int32_t ret = device_->RegHwcDeadCallback(func, data);
    if (!ret) {
        HLOGE("RegHwcDeadCallback failed, ret is %{public}d", ret);
        return ROSEN_ERROR_API_FAILED;
    }

    return ROSEN_ERROR_OK;
}

int32_t HdiBackend::PrepareCompleteIfNeed(const OutputPtr &output, bool needFlush)
{
    std::vector<LayerPtr> compClientLayers;
    std::vector<LayerInfoPtr> newLayerInfos;
    const std::unordered_map<uint32_t, LayerPtr> &layersMap = output->GetLayers();
    for (auto iter = layersMap.begin(); iter != layersMap.end(); ++iter) {
        const LayerPtr &layer = iter->second;
        newLayerInfos.emplace_back(layer->GetLayerInfo());
        if (layer->GetLayerInfo()->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT ||
            layer->GetLayerInfo()->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT_CLEAR ||
            layer->GetLayerInfo()->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_TUNNEL) {
            compClientLayers.emplace_back(layer);
        }
    }

    if (compClientLayers.size() > 0) {
        needFlush = true;
        HLOGD("Need flush framebuffer, client composition layer num is %{public}zu", compClientLayers.size());
    }

    OnPrepareComplete(needFlush, output, newLayerInfos);
    if (needFlush) {
        return output->FlushScreen(compClientLayers);
    }
    return GRAPHIC_DISPLAY_SUCCESS;
}

void HdiBackend::Repaint(const OutputPtr &output)
{
    ScopedBytrace bytrace(__func__);
    HLOGD("%{public}s: start", __func__);

    if (output == nullptr) {
        HLOGE("output is nullptr.");
        return;
    }

    bool needFlush = false;
    int32_t ret = output->PreProcessLayersComp(needFlush);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return;
    }

    ret = PrepareCompleteIfNeed(output, needFlush);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return;
    }
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    ret = output->Commit(fbFence);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        HLOGE("commit failed, ret is %{public}d", ret);
        // return
    }

    ret = output->UpdateInfosAfterCommit(fbFence);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return;
    }

    ret = output->ReleaseFramebuffer(fbFence);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return;
    }
    HLOGD("%{public}s: end", __func__);
}

void HdiBackend::ResetDevice()
{
    if (device_) {
        device_->Destroy();
        device_ = nullptr;
    }
    outputs_.clear();
}

void HdiBackend::OnPrepareComplete(bool needFlush, const OutputPtr &output, std::vector<LayerInfoPtr> &newLayerInfos)
{
    if (needFlush) {
        ReorderLayerInfo(newLayerInfos);
    }

    struct PrepareCompleteParam param = {
        .needFlushFramebuffer = needFlush,
        .layers = newLayerInfos,
        .screenId = output->GetScreenId(),
    };

    auto fbSurface = output->GetFrameBufferSurface();
    if (onPrepareCompleteCb_ != nullptr) {
        onPrepareCompleteCb_(fbSurface, param, onPrepareCompleteCbData_);
    }
}

static inline bool Cmp(const LayerInfoPtr &layer1, const LayerInfoPtr &layer2)
{
    return layer1->GetZorder() < layer2->GetZorder();
}

void HdiBackend::ReorderLayerInfo(std::vector<LayerInfoPtr> &newLayerInfos)
{
    std::sort(newLayerInfos.begin(), newLayerInfos.end(), Cmp);
}

void HdiBackend::OnSeamlessChange(uint32_t screenId)
{
    auto iter = outputs_.find(screenId);
    if (iter == outputs_.end()) {
        HLOGE("invalid hotplug screen id[%{public}u]", screenId);
        return;
    }

    if (onSeamlessChangeCb_ != nullptr) {
        onSeamlessChangeCb_(iter->second, onHotPlugCbData_);
    }
}

void HdiBackend::OnHdiBackendHotPlugEvent(uint32_t screenId, bool connected, void *data)
{
    HLOGI("HotPlugEvent, screenId is %{public}u, connected is %{public}u", screenId, connected);
    HdiBackend *hdiBackend = nullptr;
    if (data != nullptr) {
        hdiBackend = static_cast<HdiBackend *>(data);
    } else {
        hdiBackend = HdiBackend::GetInstance();
    }

    hdiBackend->OnHdiBackendConnected(screenId, connected);
}

void HdiBackend::OnSeamlessChangeEvent(uint32_t deviceId, void *data)
{
    HLOGI("SeamlessChangeEvent, screenId is %{public}u", deviceId);
    HdiBackend *hdiBackend = nullptr;
    if (data != nullptr) {
        hdiBackend = static_cast<HdiBackend *>(data);
    } else {
        hdiBackend = HdiBackend::GetInstance();
    }

    hdiBackend->OnSeamlessChange(deviceId);
}

void HdiBackend::OnHdiBackendConnected(uint32_t screenId, bool connected)
{
    if (connected) {
        CreateHdiOutput(screenId);
    }

    OnScreenHotplug(screenId, connected);
}

void HdiBackend::CreateHdiOutput(uint32_t screenId)
{
    OutputPtr newOutput = HdiOutput::CreateHdiOutput(screenId);
    newOutput->Init();
    outputs_.emplace(screenId, newOutput);
}

void HdiBackend::OnScreenHotplug(uint32_t screenId, bool connected)
{
    auto iter = outputs_.find(screenId);
    if (iter == outputs_.end()) {
        HLOGE("invalid hotplug screen id[%{public}u]", screenId);
        return;
    }

    if (onScreenHotplugCb_ != nullptr) {
        onScreenHotplugCb_(iter->second, connected, onHotPlugCbData_);
    }

    if (!connected) {
        outputs_.erase(iter);
    }
}

RosenError HdiBackend::InitDevice()
{
    if (device_ != nullptr) {
        return ROSEN_ERROR_OK;
    }

    device_ = HdiDevice::GetInstance();
    if (device_ == nullptr) {
        HLOGE("Get HdiDevice failed");
        return ROSEN_ERROR_NOT_INIT;
    }

    HLOGI("Init device succeed");
    return ROSEN_ERROR_OK;
}

RosenError HdiBackend::SetHdiBackendDevice(HdiDevice* device)
{
    if (device == nullptr) {
        HLOGE("Input HdiDevice is null");
        return ROSEN_ERROR_INVALID_ARGUMENTS;
    }

    if (device_ != nullptr) {
        HLOGW("HdiDevice has been changed");
        return ROSEN_ERROR_OK;
    }
    device_ = device;
    return ROSEN_ERROR_OK;
}

} // namespace Rosen
} // namespace OHOS
