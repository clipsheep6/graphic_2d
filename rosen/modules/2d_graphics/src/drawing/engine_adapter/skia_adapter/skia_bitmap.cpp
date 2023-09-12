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

#include "skia_bitmap.h"

#include "include/core/SkImageInfo.h"

#include "skia_image_info.h"

#include "image/bitmap.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaBitmap::SkiaBitmap() : skiaBitmap_() {}

static inline SkImageInfo MakeSkImageInfo(const int width, const int height, const BitmapFormat& format)
{
    auto imageInfo = SkImageInfo::Make(
        width, height, ConvertToSkColorType(format.colorType), ConvertToSkAlphaType(format.alphaType));
    return imageInfo;
}

void SkiaBitmap::Build(int32_t width, int32_t height, const BitmapFormat& format, int32_t stride)
{
    auto imageInfo = MakeSkImageInfo(width, height, format);
    skiaBitmap_.setInfo(imageInfo, stride);
    skiaBitmap_.allocPixels();
}

void SkiaBitmap::Build(const ImageInfo& imageInfo, int32_t stride)
{
    auto skImageInfo = ConvertToSkImageInfo(imageInfo);
    skiaBitmap_.setInfo(skImageInfo, stride);
    skiaBitmap_.allocPixels();
}

int SkiaBitmap::GetWidth() const
{
    return skiaBitmap_.width();
}

int SkiaBitmap::GetHeight() const
{
    return skiaBitmap_.height();
}

void* SkiaBitmap::GetPixels() const
{
    return skiaBitmap_.getPixels();
}

void SkiaBitmap::SetPixels(void* pixels)
{
    skiaBitmap_.setPixels(pixels);
}

const SkBitmap& SkiaBitmap::ExportSkiaBitmap() const
{
    return skiaBitmap_;
}

void SkiaBitmap::CopyPixels(Bitmap& dst, int srcLeft, int srcTop, int width, int height) const
{
    ImageInfo imageInfo = dst.GetImageInfo();
    void* dstPixels = dst.GetPixels();

    SkImageInfo skImageInfo = ConvertToSkImageInfo(imageInfo);
    int srcX = srcLeft;
    int srcY = srcTop;
    size_t dstRowBytes = static_cast<size_t>(width * height);

    skiaBitmap_.readPixels(skImageInfo, dstPixels, dstRowBytes, srcX, srcY);
}

void SkiaBitmap::ClearWithColor(const ColorQuad& color) const
{
    SkColor skColor = static_cast<SkColor>(color);
    skiaBitmap_.eraseColor(skColor);
}

ColorQuad SkiaBitmap::GetColor(int x, int y) const
{
    SkColor color;
    color = skiaBitmap_.getColor(x, y);
    return static_cast<ColorQuad>(color);
}

void SkiaBitmap::Free()
{
    skiaBitmap_.reset();
}

bool SkiaBitmap::IsValid() const
{
    return skiaBitmap_.drawsNothing();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
