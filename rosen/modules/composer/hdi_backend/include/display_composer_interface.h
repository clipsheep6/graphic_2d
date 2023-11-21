/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HDI_BACKEND_DISPLAY_COMPOSER_INTERFACE_H
#define HDI_BACKEND_DISPLAY_COMPOSER_INTERFACE_H

#ifndef DRIVERS_INTERFACE_DISPLAY_ENABLE
#include <vector>
#include "hdi_log.h"
#include "parcel.h"
#include "refbase.h"
#include "iremote_object.h"
#include "surface_type.h"
#endif

namespace OHOS {
namespace Rosen {
#ifndef DRIVERS_INTERFACE_DISPLAY_ENABLE
namespace {
enum PixelFormat {};

enum TransformType {};

enum DispPowerStatus {};

enum CompositionType {};

enum LayerType {};

enum BlendType {};

enum RopType {};

enum ColorKey {};

enum InterfaceType {};

struct PropertyObject {
    std::string name;
    unsigned int propId;
    unsigned long value;
};

struct DisplayCapability {
    std::string name;
    enum InterfaceType type;
    unsigned int phyWidth;
    unsigned int phyHeight;
    unsigned int supportLayers;
    unsigned int virtualDispCount;
    bool supportWriteBack;
    unsigned int propertyCount;
    std::vector<PropertyObject> props;
};

struct DisplayModeInfo {
    int width;
    int height;
    unsigned int freshRate;
    int id;
};

struct LayerInfo {
    int width;
    int height;
    enum LayerType type;
    int bpp;
    enum PixelFormat pixFormat;
};

struct LayerAlpha {
    bool enGlobalAlpha;
    bool enPixelAlpha;
    unsigned char alpha0;
    unsigned char alpha1;
    unsigned char gAlpha;
};

struct IRect {
    int x;
    int y;
    int w;
    int h;
};

struct Rectangle {
    struct IRect rect;
    unsigned int color;
};

enum ColorGamut {};

enum GamutMap {};

enum ColorDataSpace {
    COLOR_DATA_SPACE_UNKNOWN = 0,
};

enum HDRFormat {
    NOT_SUPPORT_HDR = 0,
};

struct HDRCapability {
    unsigned int formatCount;
    std::vector<HDRFormat> formats;
    float maxLum;
    float maxAverageLum;
    float minLum;
};

enum HDRMetadataKey {};

struct HDRMetaData {
    enum HDRMetadataKey key;
    float value;
};

enum PresentTimestampType {
    HARDWARE_DISPLAY_PTS_UNSUPPORTED = 0,
};

enum MaskInfo {};

struct PresentTimestamp {
    enum PresentTimestampType type;
    long time;
};

struct ExtDataHandle {
    int fd;
    unsigned int reserveInts;
    int reserve[];
};

class HdifdParcelable : public Parcelable {
    bool Marshalling(Parcel &parcel) const
    {
        return false;
    }
};

struct HdifdInfo {
    int id;
    HdifdParcelable hdiFd;
};

struct LayerColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};
} // namespace

class DisplayComposerInterface : public RefBase {
public:
    DisplayComposerInterface() = default;
    ~DisplayComposerInterface() = default;
    static DisplayComposerInterface* Get()
    {
        HLOGD("%{public}s: drivers_interface_display part is not enabled.", __func__);
        return nullptr;
    }

    bool AddDeathRecipient(const sptr<IRemoteObject::DeathRecipient>& recipient)
    {
        return false;
    }

    bool RemoveDeathRecipient()
    {
        return false;
    }

    int32_t RegHotPlugCallback(HotPlugCallback cb, void* data)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetClientBufferCacheCount(uint32_t devId, uint32_t count)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplayCapability(uint32_t devId, DisplayCapability& info)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplaySupportedModes(uint32_t devId, std::vector<DisplayModeInfo>& modes)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplayMode(uint32_t devId, uint32_t& modeId)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayMode(uint32_t devId, uint32_t modeId)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplayPowerStatus(uint32_t devId, DispPowerStatus& status)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayPowerStatus(uint32_t devId, DispPowerStatus status)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplayBacklight(uint32_t devId, uint32_t& level)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayBacklight(uint32_t devId, uint32_t level)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplayProperty(uint32_t devId, uint32_t id, uint64_t& value)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplayCompChange(uint32_t devId, std::vector<uint32_t>& layers,
        std::vector<int32_t>& types)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayClientCrop(uint32_t devId, const IRect& rect)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayClientBuffer(uint32_t devId, const BufferHandle* buffer, uint32_t seqNo,
        int32_t fence)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayClientDamage(uint32_t devId, std::vector<IRect>& rects)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayVsyncEnabled(uint32_t devId, bool enabled)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t RegDisplayVBlankCallback(uint32_t devId, VBlankCallback cb, void* data)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplayReleaseFence(uint32_t devId, std::vector<uint32_t>& layers,
        std::vector<int32_t>& fences)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t CreateVirtualDisplay(uint32_t width, uint32_t height, int32_t& format, uint32_t& devId)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t DestroyVirtualDisplay(uint32_t devId)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetVirtualDisplayBuffer(uint32_t devId, const BufferHandle& buffer, const int32_t fence)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayProperty(uint32_t devId, uint32_t id, uint64_t value)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t Commit(uint32_t devId, int32_t& fence)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplaySupportedColorGamuts(uint32_t devId, std::vector<ColorGamut>& gamuts)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplayColorGamut(uint32_t devId, ColorGamut& gamut)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayColorGamut(uint32_t devId, const ColorGamut& gamut)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetDisplayGamutMap(uint32_t devId, GamutMap& gamutMap)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayGamutMap(uint32_t devId, const GamutMap& gamutMap)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetDisplayColorTransform(uint32_t devId, const std::vector<float>& matrix)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetHDRCapabilityInfos(uint32_t devId, HDRCapability& info)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetSupportedMetadataKey(uint32_t devId, std::vector<HDRMetadataKey>& keys)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t CreateLayer(uint32_t devId, const LayerInfo& layerInfo, uint32_t cacheCount, uint32_t& layerId)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t DestroyLayer(uint32_t devId, uint32_t layerId)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t PrepareDisplayLayers(uint32_t devId, bool& needFlushFb)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerAlpha(uint32_t devId, uint32_t layerId, const LayerAlpha& alpha)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerRegion(uint32_t devId, uint32_t layerId, const IRect& rect)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerCrop(uint32_t devId, uint32_t layerId, const IRect& rect)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerZorder(uint32_t devId, uint32_t layerId, uint32_t zorder)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerPreMulti(uint32_t devId, uint32_t layerId, bool preMul)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerTransformMode(uint32_t devId, uint32_t layerId, TransformType type)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerDirtyRegion(uint32_t devId, uint32_t layerId, std::vector<IRect>& regions)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerVisibleRegion(uint32_t devId, uint32_t layerId, std::vector<IRect>& rects)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerBuffer(uint32_t devId, uint32_t layerId, const BufferHandle* buffer, uint32_t seqNo,
        int32_t fence, const std::vector<uint32_t>& deletingList)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerCompositionType(uint32_t devId, uint32_t layerId, CompositionType type)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerBlendType(uint32_t devId, uint32_t layerId, BlendType type)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerColorTransform(uint32_t devId, uint32_t layerId, const std::vector<float>& matrix)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerColorDataSpace(uint32_t devId, uint32_t layerId, const ColorDataSpace colorSpace)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetLayerColorDataSpace(uint32_t devId, uint32_t layerId, ColorDataSpace& colorSpace)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerMetaData(uint32_t devId, uint32_t layerId, const std::vector<HDRMetaData>& metaData)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerMetaDataSet(uint32_t devId, uint32_t layerId, HDRMetadataKey key,
         const std::vector<uint8_t>& metaData)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetSupportedPresentTimestamp(uint32_t devId, uint32_t layerId, PresentTimestampType& type)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t GetHwPresentTimestamp(uint32_t devId, uint32_t layerId, PresentTimestamp& pts)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerTunnelHandle(uint32_t devId, uint32_t layerId, const ExtDataHandle& handle)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerMaskInfo(uint32_t devId, uint32_t layerId, const MaskInfo maskInfo)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }

    int32_t SetLayerColor(uint32_t devId, uint32_t layerId, const LayerColor& layerColor)
    {
        return GRAPHIC_DISPLAY_NULL_PTR;
    }
};
#endif

} // namespace Rosen
} // namespace OHOS
#endif // HDI_BACKEND_DISPLAY_COMPOSER_INTERFACE_H
