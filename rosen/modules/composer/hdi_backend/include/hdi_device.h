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

#ifndef HDI_BACKEND_HDI_DEVICE_H
#define HDI_BACKEND_HDI_DEVICE_H

#include <vector>
#include <refbase.h>

#include <sync_fence.h>

#include "hdi_log.h"
#include "display_device.h"
#include "display_layer.h"

namespace OHOS {
namespace Rosen {

class HdiDevice {
public:
    HdiDevice();
    virtual ~HdiDevice();

    static HdiDevice* GetInstance();

    /* set & get device screen info begin */
    int32_t RegHotPlugCallback(HotPlugCallback callback, void *data);
    int32_t RegScreenVBlankCallback(uint32_t screenId, VBlankCallback callback, void *data);
    int32_t GetScreenCapability(uint32_t screenId, DisplayCapability &info);
    int32_t GetScreenSuppportedModes(uint32_t screenId, std::vector<DisplayModeInfo> &modes);
    int32_t GetScreenMode(uint32_t screenId, uint32_t &modeId);
    int32_t SetScreenMode(uint32_t screenId, uint32_t modeId);
    int32_t GetScreenPowerStatus(uint32_t screenId, DispPowerStatus &status);
    int32_t SetScreenPowerStatus(uint32_t screenId, DispPowerStatus status);
    int32_t GetScreenBacklight(uint32_t screenId, uint32_t &level);
    int32_t SetScreenBacklight(uint32_t screenId, uint32_t level);
    int32_t PrepareScreenLayers(uint32_t screenId, bool &needFlushFb);
    int32_t GetScreenCompChange(uint32_t screenId, std::vector<uint32_t> &layersId,
                                std::vector<int32_t> &types);
    int32_t SetScreenClientBuffer(uint32_t screenId, const BufferHandle *buffer,
                                  const sptr<SyncFence> &fence);
    int32_t SetScreenClientDamage(uint32_t screenId, uint32_t num, IRect &damageRect);
    int32_t SetScreenVsyncEnabled(uint32_t screenId, bool enabled);
    int32_t GetScreenReleaseFence(uint32_t screenId, std::vector<uint32_t> &layersId,
                                  std::vector<sptr<SyncFence>> &fences);
    int32_t Commit(uint32_t screenId, sptr<SyncFence> &fence);
    /* set & get device screen info end */

    /* set & get device layer info begin */
    int32_t SetLayerAlpha(uint32_t screenId, uint32_t layerId, LayerAlpha &alpha);
    int32_t SetLayerSize(uint32_t screenId, uint32_t layerId, IRect &layerRect);
    int32_t SetTransformMode(uint32_t screenId, uint32_t layerId, TransformType type);
    int32_t SetLayerVisibleRegion(uint32_t screenId, uint32_t layerId, uint32_t num, IRect &visable);
    int32_t SetLayerDirtyRegion(uint32_t screenId, uint32_t layerId, IRect &dirty);
    int32_t SetLayerBuffer(uint32_t screenId, uint32_t layerId, const BufferHandle *handle,
                           const sptr<SyncFence> &acquireFence);
    int32_t SetLayerCompositionType(uint32_t screenId, uint32_t layerId, CompositionType type);
    int32_t SetLayerBlendType(uint32_t screenId, uint32_t layerId, BlendType type);
    int32_t SetLayerCrop(uint32_t screenId, uint32_t layerId, IRect &crop);
    int32_t SetLayerZorder(uint32_t screenId, uint32_t layerId, uint32_t zorder);
    int32_t SetLayerPreMulti(uint32_t screenId, uint32_t layerId, bool isPreMulti);
    /* set & get device layer info end */

    int32_t CreateLayer(uint32_t screenId, const LayerInfo &layerInfo, uint32_t &layerId);
    int32_t CloseLayer(uint32_t screenId, uint32_t layerId);

private:
    HdiDevice(const HdiDevice& rhs) = delete;
    HdiDevice& operator=(const HdiDevice& rhs) = delete;
    HdiDevice(HdiDevice&& rhs) = delete;
    HdiDevice& operator=(HdiDevice&& rhs) = delete;

    DeviceFuncs *deviceFuncs_ = nullptr;
    LayerFuncs *layerFuncs_ = nullptr;

    RosenError Init();
    void Destory();
};

} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_DEVICE_H