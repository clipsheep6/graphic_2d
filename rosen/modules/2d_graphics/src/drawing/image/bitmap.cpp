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

#include "image/bitmap.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Bitmap::Bitmap()
    : bmpImplPtr(ImplFactory::CreateBitmapImpl()),
      format_({ COLORTYPE_UNKNOWN, ALPHATYPE_UNKNOWN })
{}

Bitmap::~Bitmap() {}

void Bitmap::Build(int32_t width, int32_t height, const BitmapFormat& format, int32_t stride)
{
    format_ = format;
    bmpImplPtr->Build(width, height, format, stride);
}

void Bitmap::Build(const ImageInfo& imageInfo, int32_t stride)
{
    imageInfo_ = imageInfo;
    bmpImplPtr->Build(imageInfo_, stride);
}

int Bitmap::GetWidth() const
{
    return bmpImplPtr->GetWidth();
}

int Bitmap::GetHeight() const
{
    return bmpImplPtr->GetHeight();
}

int Bitmap::GetRowBytes() const
{
    return bmpImplPtr->GetRowBytes();
}

ColorType Bitmap::GetColorType() const
{
    return bmpImplPtr->GetColorType();
}

AlphaType Bitmap::GetAlphaType() const
{
    return bmpImplPtr->GetAlphaType();
}

void Bitmap::SetPixels(void* pixel)
{
    bmpImplPtr->SetPixels(pixel);
}

void* Bitmap::GetPixels() const
{
    return bmpImplPtr->GetPixels();
}

void Bitmap::CopyPixels(Bitmap& dst, int srcLeft, int srcTop, int width, int height) const
{
    bmpImplPtr->CopyPixels(dst, srcLeft, srcTop, width, height);
}

bool Bitmap::IsImmutable()
{
    return bmpImplPtr->IsImmutable();
}

void Bitmap::SetImmutable()
{
    bmpImplPtr->SetImmutable();
}

void Bitmap::ClearWithColor(const ColorQuad& color) const
{
    bmpImplPtr->ClearWithColor(color);
}

bool Bitmap::IsValid() const
{
    return bmpImplPtr->IsValid();
}

ColorQuad Bitmap::GetColor(int x, int y) const
{
    return bmpImplPtr->GetColor(x, y);
}

void Bitmap::Free()
{
    bmpImplPtr->Free();
}

BitmapFormat Bitmap::GetFormat() const
{
    return format_;
}

ImageInfo Bitmap::GetImageInfo() const
{
    return imageInfo_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
