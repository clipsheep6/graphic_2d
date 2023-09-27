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

#include "image/image.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
BackendTexture::BackendTexture(bool isValid) noexcept
    : isValid_(isValid), imageImplPtr(ImplFactory::CreateImageImpl()) {}

bool BackendTexture::IsValid() const
{
    return isValid_;
}

void BackendTexture::SetTextureInfo(const TextureInfo& textureInfo)
{
    textureInfo_ = textureInfo;
}

const TextureInfo BackendTexture::GetTextureInfo() const
{
    return textureInfo_;
}

Image::Image() noexcept : imageImplPtr(ImplFactory::CreateImageImpl()) {}

Image::Image(void* rawImg) noexcept : imageImplPtr(ImplFactory::CreateImageImpl(rawImg)) {}

Image* Image::BuildFromBitmap(const Bitmap& bitmap)
{
    return static_cast<Image*>(imageImplPtr->BuildFromBitmap(bitmap));
}

Image* Image::BuildFromPicture(const Picture& picture, const SizeI& dimensions, const Matrix& matrix,
    const Brush& brush, BitDepth bitDepth, std::shared_ptr<ColorSpace> colorSpace)
{
    return static_cast<Image*>(
        imageImplPtr->BuildFromPicture(picture, dimensions, matrix, brush, bitDepth, colorSpace));
}

#ifdef ACE_ENABLE_GPU
bool Image::BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap)
{
    return imageImplPtr->BuildFromBitmap(gpuContext, bitmap);
}

bool Image::BuildFromCompressed(GPUContext& gpuContext, const std::shared_ptr<Data>& data, int width, int height,
    CompressedType type)
{
    return imageImplPtr->BuildFromCompressed(gpuContext, data, width, height, type);
}

bool Image::BuildFromTexture(GPUContext& gpuContext, const TextureInfo& info, TextureOrigin origin,
    BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace)
{
    return imageImplPtr->BuildFromTexture(gpuContext, info, origin, bitmapFormat, colorSpace);
}

BackendTexture Image::GetBackendTexture(bool flushPendingGrContextIO, TextureOrigin* origin) const
{
    return imageImplPtr->GetBackendTexture(flushPendingGrContextIO, origin);
}

bool Image::IsValid(GPUContext* context) const
{
    return imageImplPtr->IsValid(context);
}
#endif

int Image::GetWidth() const
{
    return imageImplPtr->GetWidth();
}

int Image::GetHeight() const
{
    return imageImplPtr->GetHeight();
}

ColorType Image::GetColorType() const
{
    return imageImplPtr->GetColorType();
}

AlphaType Image::GetAlphaType() const
{
    return imageImplPtr->GetAlphaType();
}

uint32_t Image::GetUniqueID() const
{
    return imageImplPtr->GetUniqueID();
}

ImageInfo Image::GetImageInfo()
{
    return imageImplPtr->GetImageInfo();
}

bool Image::ReadPixels(Bitmap& bitmap, int x, int y)
{
    return imageImplPtr->ReadPixels(bitmap, x, y);
}

bool Image::IsTextureBacked() const
{
    return imageImplPtr->IsTextureBacked();
}

bool Image::ScalePixels(const Bitmap& bitmap, const SamplingOptions& sampling, bool allowCachingHint) const
{
    return imageImplPtr->ScalePixels(bitmap, sampling, allowCachingHint);
}

std::shared_ptr<Data> Image::EncodeToData(EncodedImageFormat& encodedImageFormat, int quality) const
{
    return imageImplPtr->EncodeToData(encodedImageFormat, quality);
}

bool Image::IsLazyGenerated() const
{
    return imageImplPtr->IsLazyGenerated();
}

std::shared_ptr<Data> Image::Serialize() const
{
    return imageImplPtr->Serialize();
}

bool Image::Deserialize(std::shared_ptr<Data> data)
{
    return imageImplPtr->Deserialize(data);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
