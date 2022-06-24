/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "hdidevice_fuzzer.h"

#include "hdi_device.h"
using namespace OHOS::Rosen;

namespace OHOS {
    namespace {
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        std::memcpy(&object, data_ + pos, objectSize);
        pos += objectSize;
        return object;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size <= 0) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        uint32_t screenId = GetData<uint32_t>();
        uint32_t modeId = GetData<uint32_t>();
        uint32_t layerId = GetData<uint32_t>();
        LayerInfo layerInfo = GetData<LayerInfo>();
        DisplayCapability displayinfo = GetData<DisplayCapability>();
        DispPowerStatus status = GetData<DispPowerStatus>();
        uint32_t level = GetData<uint32_t>();
        bool needFlushFb = GetData<bool>();
        BufferHandle buffer = GetData<BufferHandle>();
        uint32_t num = GetData<uint32_t>();
        IRect damageRect = GetData<IRect>();
        bool enabled = GetData<bool>();
        ColorGamut gamut = GetData<ColorGamut>();
        GamutMap gamutMap = GetData<GamutMap>();
        float matrix = GetData<float>();
        HDRCapability hdrinfo = GetData<HDRCapability>();
        LayerAlpha alpha = GetData<LayerAlpha>();
        IRect layerRect = GetData<IRect>();
        TransformType ttype = GetData<TransformType>();
        IRect visable = GetData<IRect>();
        IRect dirty = GetData<IRect>();
        BufferHandle handle = GetData<BufferHandle>();
        CompositionType ctype = GetData<CompositionType>();
        BlendType btype = GetData<BlendType>();
        IRect crop = GetData<IRect>();
        uint32_t zorder = GetData<uint32_t>();
        bool isPreMulti = GetData<bool>();
        ColorDataSpace colorSpace = GetData<ColorDataSpace>();
        std::vector<HDRMetaData> hdrmetaData;
        for (int i = 0; i < 10; i++) { // add 10 elements to the vector
            HDRMetaData meta = GetData<HDRMetaData>();
            hdrmetaData.push_back(meta);
        }
        HDRMetadataKey key = GetData<HDRMetadataKey>();
        std::vector<uint8_t> metaData;
        for (int i = 0; i < 10; i++) { // add 10 elements to the vector
            uint8_t meta = GetData<uint8_t>();
            metaData.push_back(meta);
        }

        // test
        HdiDevice* device = HdiDevice::GetInstance();
        HotPlugCallback hotplug_callback = [](uint32_t, bool, void*) -> void {};
        VBlankCallback vblank_callback = [](unsigned int, uint64_t, void*) -> void {};
        device->RegHotPlugCallback(hotplug_callback, (void*)data);
        device->RegScreenVBlankCallback(screenId, vblank_callback, (void*)data);
        device->GetScreenCapability(screenId, displayinfo);
        std::vector<DisplayModeInfo> modes;
        device->GetScreenSupportedModes(screenId, modes);
        device->SetScreenMode(screenId, modeId);
        device->GetScreenMode(screenId, modeId);
        device->SetScreenPowerStatus(screenId, status);
        device->GetScreenPowerStatus(screenId, status);
        device->SetScreenBacklight(screenId, level);
        device->GetScreenBacklight(screenId, level);
        device->PrepareScreenLayers(screenId, needFlushFb);
        std::vector<uint32_t> layersId;
        std::vector<int32_t> types;
        device->GetScreenCompChange(screenId, layersId, types);
        sptr<SyncFence> syncfence = SyncFence::INVALID_FENCE;
        device->SetScreenClientBuffer(screenId, &buffer, syncfence);
        device->SetScreenClientDamage(screenId, num, damageRect);
        device->SetScreenVsyncEnabled(screenId, enabled);
        std::vector<sptr<SyncFence>> fences;
        device->GetScreenReleaseFence(screenId, layersId, fences);
        std::vector<ColorGamut> gamuts;
        device->GetScreenSupportedColorGamuts(screenId, gamuts);
        device->SetScreenColorGamut(screenId, gamut);
        device->GetScreenColorGamut(screenId, gamut);
        device->SetScreenGamutMap(screenId, gamutMap);
        device->GetScreenGamutMap(screenId, gamutMap);
        device->SetScreenColorTransform(screenId, &matrix);
        device->GetHDRCapabilityInfos(screenId, hdrinfo);
        std::vector<HDRMetadataKey> keys;
        device->GetSupportedMetaDataKey(screenId, keys);
        sptr<SyncFence> sync_fence = SyncFence::INVALID_FENCE;
        device->Commit(screenId, sync_fence);
        device->CreateLayer(screenId, layerInfo, layerId);
        device->SetLayerAlpha(screenId, layerId, alpha);
        device->SetLayerSize(screenId, layerId, layerRect);
        device->SetTransformMode(screenId, layerId, ttype);
        device->SetLayerVisibleRegion(screenId, layerId, num, visable);
        device->SetLayerDirtyRegion(screenId, layerId, dirty);
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        device->SetLayerBuffer(screenId, layerId, &handle, acquireFence);
        device->SetLayerCompositionType(screenId, layerId, ctype);
        device->SetLayerBlendType(screenId, layerId, btype);
        device->SetLayerCrop(screenId, layerId, crop);
        device->SetLayerZorder(screenId, layerId, zorder);
        device->SetLayerPreMulti(screenId, layerId, isPreMulti);
        device->SetLayerColorTransform(screenId, layerId, &matrix);
        device->SetLayerColorDataSpace(screenId, layerId, colorSpace);
        device->GetLayerColorDataSpace(screenId, layerId, colorSpace);
        device->SetLayerMetaData(screenId, layerId, hdrmetaData);
        device->SetLayerMetaDataSet(screenId, layerId, key, metaData);
        device->CloseLayer(screenId, layerId);
        
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

