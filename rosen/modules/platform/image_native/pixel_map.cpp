/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "pixel_map.h"
#include "hilog/log.h"

static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0xD001200, "PixelMap::Create"};

#define HILOGE(...) OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, ##__VA_ARGS__)
#define HILOGW(...) OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, ##__VA_ARGS__)
#define HILOGI(...) OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, ##__VA_ARGS__)
#define HILOGD(...) OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, ##__VA_ARGS__)

namespace OHOS {
namespace Media {
PixelMap::~PixelMap()
{
}

std::unique_ptr<PixelMap> PixelMap::Create(const InitializationOptions &opts)
{
    return nullptr;
}

int32_t PixelMap::GetRowBytes()
{
    return rowDataSize_;
}

int32_t PixelMap::GetWidth()
{
    HILOGE("GetWidth FFFFFFFFFFFFFFFFFFF");
    return imageInfo_.size.width;
}

int32_t PixelMap::GetHeight()
{
    HILOGE("GetHeight FFFFFFFFFFFFFFFFFFF");
    return imageInfo_.size.height;
}

int32_t PixelMap::GetByteCount()
{
    return rowDataSize_ * imageInfo_.size.height;
}

const uint32_t *PixelMap::GetPixel32(int32_t x, int32_t y)
{
    return reinterpret_cast<uint32_t *>(data_ + y * rowStride_ + (static_cast<uint32_t>(x) << 2));
}

void PixelMap::GetImageInfo(ImageInfo &imageInfo)
{
    imageInfo = imageInfo_;
    HILOGE("PixelMap GetImageInfo w %{public}d h %{public}d!!", imageInfo.size.width, imageInfo.size.height);
    HILOGE("PixelMap GetImageInfo a %{public}d f %{public}d!!", imageInfo.alphaType, imageInfo.pixelFormat);
}
int32_t PixelMap::GetRowStride() const
{
    HILOGE("PixelMap GetRowStride %{public}d!!", rowStride_);
    return rowStride_;
}


const uint8_t *PixelMap::GetPixels()
{
    HILOGE("PixelMap GetPixels !!!!!!");
    HILOGE("GetPixels %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x ",
        data_[0], data_[1], data_[2], data_[3], data_[4], data_[5], data_[6], data_[7], data_[8], data_[9]);
    return data_;
}

bool PixelMap::Marshalling(Parcel &data) const
{
    HILOGE("PixelMap Marshalling !!!!!!");
    return false;
}

PixelMap *PixelMap::Unmarshalling(Parcel &data)
{
    HILOGE("PixelMap Unmarshalling !!");
    return nullptr;
}
uint32_t PixelMap::SetImageInfo(ImageInfo &info)
{
    HILOGE("SetImageInfo info %{public}d %{public}d", info.size.width, info.size.height);
    switch (info.pixelFormat) {
        case PixelFormat::RGBA_8888: {
            pixelBytes_ = 4;
            break;
        }
        case PixelFormat::BGRA_8888: {
            pixelBytes_ = 4;
            break;
        }
        case PixelFormat::ARGB_8888: {
            pixelBytes_ = 4;
            break;
        }
        case PixelFormat::ALPHA_8: {
            pixelBytes_ = 1;
            break;
        }
        case PixelFormat::RGB_565: {
            pixelBytes_ = 2;
            break;
        }
        case PixelFormat::RGB_888: {
            pixelBytes_ = 3;
            break;
        }
        case PixelFormat::NV12:
        case PixelFormat::NV21: {
            pixelBytes_ = 2;
            break;
        }
        case PixelFormat::CMYK:
            pixelBytes_ = 4;
            break;
        case PixelFormat::RGBA_F16:
            pixelBytes_ = 8;
            break;
        default:
            break;
    }
    rowStride_ = pixelBytes_ * info.size.width;
    imageInfo_ = info;
    return 0;
}
void PixelMap::SetPixelsAddr(void *addr, void *context, uint32_t size, AllocatorType type, CustomFreePixelMap func)
{
    
    data_ = static_cast<uint8_t *>(addr);
    HILOGE("SetPixelsAddr %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x %{public}02x ",
        data_[0], data_[1], data_[2], data_[3], data_[4], data_[5], data_[6], data_[7], data_[8], data_[9]);
    context_ = context;
    pixelsSize_ = size;
    allocatorType_ = type;
    custFreePixelMap_ = func;
}
} // namespace Media
} // namespace OHOS
