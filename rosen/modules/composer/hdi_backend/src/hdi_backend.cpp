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

    return InitDevice();
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

void HdiBackend::Repaint(std::vector<OutputPtr> &outputs)
{
    if (device_ == nullptr) {
        HLOGE("device has not been initialized");
        return;
    }

    int32_t ret = DISPLAY_SUCCESS;
    for (auto &output : outputs) {
        const std::unordered_map<uint32_t, LayerPtr> layersMap = output->GetLayers();
        uint32_t screenId = output->GetScreenId();
        for (auto iter = layersMap.begin(); iter != layersMap.end(); ++iter) {
            const LayerPtr &layer = iter->second;
            layer->SetHdiLayerInfo(screenId);
        }

        bool needFlush = false;
        ret = device_->PrepareScreenLayers(screenId, needFlush);
        if (ret != DISPLAY_SUCCESS) {
            HLOGE("PrepareScreenLayers failed, ret is %{public}d", ret);
            // return [TODO]
        }

        std::vector<LayerPtr> changedLayers;
        std::vector<CompositionType> compTypes;
        std::vector<LayerInfoPtr> changedLayerInfos;
        if (needFlush) {
            ret = GetCompChangedLayers(screenId, changedLayers, changedLayerInfos, compTypes, layersMap);
            if (ret != DISPLAY_SUCCESS) {
                // [TODO] GetCompChangedLayers is not realized
                // return
            }
        }

        OnPrepareComplete(needFlush, output, changedLayerInfos, compTypes);

        if (needFlush) {
            if (FlushScreen(screenId, output, changedLayers) != DISPLAY_SUCCESS) {
                // return [TODO]
            }
        }

        sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
        ret = device_->Commit(screenId, fbFence);
        if (ret != DISPLAY_SUCCESS) {
            HLOGE("commit failed, ret is %{public}d", ret);
            // return [TODO]
        }

        ReleaseLayerBuffer(screenId, layersMap);

        output->ReleaseFramebuffer(fbFence);
    }
}

int32_t HdiBackend::GetCompChangedLayers(uint32_t screenId, std::vector<LayerPtr> &changedLayers,
        std::vector<LayerInfoPtr> &changedLayerInfos, std::vector<CompositionType> &compTypes,
        const std::unordered_map<uint32_t, LayerPtr> &layersMap)
{
    std::vector<uint32_t> layersId;
    std::vector<int32_t> types;
    int32_t ret = device_->GetScreenCompChange(screenId, layersId, types);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("GetScreenCompChange failed, ret is %{public}d", ret);
        // return ret [TODO]
    }

    size_t layerNum = layersId.size();
    compTypes.resize(layerNum);
    for (size_t i = 0; i < layerNum; i++) {
        compTypes[i] = static_cast<CompositionType>(types[i]);
    }

    changedLayers.resize(layerNum);
    changedLayerInfos.resize(layerNum);
    for (size_t i = 0; i < layerNum; i++) {
        auto iter = layersMap.find(layersId[i]);
        if (iter != layersMap.end()) {
            changedLayers[i] = iter->second;
            changedLayerInfos[i] = iter->second->GetLayerInfo();
        }
    }

    return DISPLAY_SUCCESS;
}

void HdiBackend::OnPrepareComplete(bool needFlush, OutputPtr &output,
        std::vector<LayerInfoPtr> &changedLayerInfos, std::vector<CompositionType> &compTypes)
{
    struct PrepareCompleteParam param = {
        .needFlushFramebuffer = needFlush,
        .layers = changedLayerInfos,
        .types = compTypes,
    };

    sptr<Surface> producerSurface = output->GetProducerSurface();

    if (onPrepareCompleteCb_ != nullptr) {
        onPrepareCompleteCb_(producerSurface, param, onPrepareCompleteCbData_);
    }
}

int32_t HdiBackend::FlushScreen(uint32_t screenId, OutputPtr &output,
        std::vector<LayerPtr> &changedLayers)
{
    /*
     * We may not be able to get the framebuffer at this time, so we
     * have to wait here. If the framebuffer is available, it'll signal us.
     */
    output->FramebufferSemWait();

    sptr<SyncFence> fbAcquireFence = output->GetFramebufferFence();
    for (auto &layer : changedLayers) {
        layer->MergeWithFramebufferFence(fbAcquireFence);
    }

    return SetScreenClientInfo(screenId, fbAcquireFence, output);
}

int32_t HdiBackend::SetScreenClientInfo(uint32_t screenId, const sptr<SyncFence> &fbAcquireFence,
        OutputPtr &output)
{
    int32_t ret = device_->SetScreenClientBuffer(screenId,
                        output->GetFramebuffer()->GetBufferHandle(), fbAcquireFence);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("SetScreenClientBuffer failed, ret is %{public}d", ret);
        return ret;
    }

    ret = device_->SetScreenClientDamage(screenId, output->GetOutputDamageNum(),
                                         output->GetOutputDamage());
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("SetScreenClientDamage failed, ret is %{public}d", ret);
        return ret;
    }

    return DISPLAY_SUCCESS;
}

void HdiBackend::ReleaseLayerBuffer(uint32_t screenId, const std::unordered_map<uint32_t,
                                    LayerPtr> &layersMap)
{
    std::vector<uint32_t> layersId;
    std::vector<sptr<SyncFence>> fences;
    int32_t ret = device_->GetScreenReleaseFence(screenId, layersId, fences);
    if (ret != DISPLAY_SUCCESS || layersId.size() <= 0 || fences.size() <= 0) {
        HLOGE("GetScreenReleaseFence failed, ret is %{public}d", ret);
        // return [TODO]
    }

    // [TODO] add layer->MergeWithLayerFence(fences[i])

    for (auto iter = layersMap.begin(); iter != layersMap.end(); ++iter) {
        const LayerPtr &layer = iter->second;
        layer->ReleaseBuffer();
    }
}

void HdiBackend::OnHdiBackendHotPlugEvent(uint32_t screenId, bool connected, void *data)
{
    HdiBackend *hdiBackend = nullptr;
    if (data != nullptr) {
        hdiBackend = static_cast<HdiBackend *>(data);
    } else {
        hdiBackend = HdiBackend::GetInstance();
    }

    if (connected) {
        hdiBackend->OnHdiBackendConnected(screenId);
    } else {
        hdiBackend->OnHdiBackendDisconnected(screenId);
    }
}

void HdiBackend::OnHdiBackendConnected(uint32_t screenId)
{
    CreateHdiOutput(screenId);
    OnScreenHotplug(screenId, true);
}

void HdiBackend::OnHdiBackendDisconnected(uint32_t screenId)
{
    OnScreenHotplug(screenId, false);
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

    int32_t ret = device_->RegHotPlugCallback(HdiBackend::OnHdiBackendHotPlugEvent, this);
    if (ret != DISPLAY_SUCCESS) {
        HLOGE("DeviceFuncs.RegHotPlugCallback return %{public}d", ret);
        return ROSEN_ERROR_API_FAILED;
    }

    return ROSEN_ERROR_OK;
}

void HdiBackend::CheckRet(int32_t ret, const char* func)
{
    if (ret != DISPLAY_SUCCESS) {
        HLOGD("call hdi %{public}s failed, ret is %{public}d", func, ret);
    }
}

} // namespace Rosen
} // namespace OHOS
