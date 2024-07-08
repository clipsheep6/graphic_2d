/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "drawing_bitmap.h"

#include "drawing_canvas_utils.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Bitmap* CastToBitmap(OH_Drawing_Bitmap* cBitmap)
{
    return reinterpret_cast<Bitmap*>(cBitmap);
}

static const Bitmap* CastToBitmap(const OH_Drawing_Bitmap* cBitmap)
{
    return reinterpret_cast<const Bitmap*>(cBitmap);
}

static const Bitmap& CastToBitmap(const OH_Drawing_Bitmap& cBitmap)
{
    return reinterpret_cast<const Bitmap&>(cBitmap);
}

static const Rect& CastToRect(const OH_Drawing_Rect& cRect)
{
    return reinterpret_cast<const Rect&>(cRect);
}

static ColorSpace* CastToColorSpace(OH_Drawing_ColorSpace* colorSpace)
{
    return reinterpret_cast<ColorSpace*>(colorSpace);
}

static OH_Drawing_ColorSpaceType ColorSpaceTypeCastToCColorSpaceType(ColorSpace::ColorSpaceType colorSpaceType)
{
    OH_Drawing_ColorSpaceType cColorSpaceType = NO_TYPE;
    switch (colorSpaceType) {
        case ColorSpace::ColorSpaceType::NO_TYPE:
            cColorSpaceType = NO_TYPE;
            break;
        case ColorSpace::ColorSpaceType::SRGB:
            cColorSpaceType = SRGB;
            break;
        case ColorSpace::ColorSpaceType::SRGB_LINEAR:
            cColorSpaceType = SRGB_LINEAR;
            break;
        case ColorSpace::ColorSpaceType::REF_IMAGE:
            cColorSpaceType = REF_IMAGE;
            break;
        case ColorSpace::ColorSpaceType::RGB:
            cColorSpaceType = RGB;
            break;
        default:
            break;
    }
    return cColorSpaceType;
}

OH_Drawing_Bitmap* OH_Drawing_BitmapCreate()
{
    return (OH_Drawing_Bitmap*)new Bitmap;
}

void OH_Drawing_BitmapDestroy(OH_Drawing_Bitmap* cBitmap)
{
    delete CastToBitmap(cBitmap);
}

OH_Drawing_Bitmap* OH_Drawing_BitmapCreateFromPixels(OH_Drawing_Image_Info* cImageInfo, void* pixels, uint32_t rowBytes)
{
    if (cImageInfo == nullptr || pixels == nullptr || rowBytes == 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }

    ImageInfo imageInfo(cImageInfo->width, cImageInfo->height,
        static_cast<ColorType>(cImageInfo->colorType), static_cast<AlphaType>(cImageInfo->alphaType));

    Bitmap* bitmap = new Bitmap;
    bool ret = bitmap->InstallPixels(imageInfo, pixels, rowBytes);
    if (!ret) {
        delete bitmap;
        return nullptr;
    }
    return (OH_Drawing_Bitmap*)bitmap;
}

void OH_Drawing_BitmapBuild(OH_Drawing_Bitmap* cBitmap, const uint32_t width, const uint32_t height,
    const OH_Drawing_BitmapFormat* cBitmapFormat)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr || cBitmapFormat == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }

    BitmapFormat format { static_cast<ColorType>(cBitmapFormat->colorFormat),
        static_cast<AlphaType>(cBitmapFormat->alphaFormat) };
    bitmap->Build(width, height, format);
}

uint32_t OH_Drawing_BitmapGetWidth(OH_Drawing_Bitmap* cBitmap)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return bitmap->GetWidth();
}

uint32_t OH_Drawing_BitmapGetHeight(OH_Drawing_Bitmap* cBitmap)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return bitmap->GetHeight();
}

OH_Drawing_ColorFormat OH_Drawing_BitmapGetColorFormat(OH_Drawing_Bitmap* cBitmap)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return COLOR_FORMAT_UNKNOWN;
    }
    return static_cast<OH_Drawing_ColorFormat>(bitmap->GetColorType());
}

OH_Drawing_AlphaFormat OH_Drawing_BitmapGetAlphaFormat(OH_Drawing_Bitmap* cBitmap)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return ALPHA_FORMAT_UNKNOWN;
    }
    return static_cast<OH_Drawing_AlphaFormat>(bitmap->GetAlphaType());
}
void* OH_Drawing_BitmapGetPixels(OH_Drawing_Bitmap* cBitmap)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    return bitmap->GetPixels();
}

void OH_Drawing_BitmapGetImageInfo(OH_Drawing_Bitmap* cBitmap, OH_Drawing_Image_Info* cImageInfo)
{
    if (cBitmap == nullptr || cImageInfo == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    ImageInfo imageInfo = CastToBitmap(cBitmap)->GetImageInfo();

    cImageInfo->width = imageInfo.GetWidth();
    cImageInfo->height = imageInfo.GetHeight();
    cImageInfo->colorType = static_cast<OH_Drawing_ColorFormat>(imageInfo.GetColorType());
    cImageInfo->alphaType = static_cast<OH_Drawing_AlphaFormat>(imageInfo.GetAlphaType());
}

bool OH_Drawing_BitmapReadPixels(OH_Drawing_Bitmap* cBitmap, const OH_Drawing_Image_Info* dstInfo,
    void* dstPixels, size_t dstRowBytes, int32_t srcX, int32_t srcY)
{
    if (cBitmap == nullptr || dstInfo == nullptr || dstPixels == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }

    ImageInfo imageInfo(dstInfo->width, dstInfo->height,
        static_cast<ColorType>(dstInfo->colorType), static_cast<AlphaType>(dstInfo->alphaType));

    return CastToBitmap(cBitmap)->ReadPixels(imageInfo, dstPixels, dstRowBytes, srcX, srcY);
}

OH_Drawing_Bitmap* OH_Drawing_BitmapCreateFromImageInfo(
    const OH_Drawing_Image_Info* cImageInfo, OH_Drawing_ColorSpace* cColorSpace, int32_t rowBytes)
{
    Bitmap* bitmap = new Bitmap;
    if (bitmap == nullptr || cColorSpace == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }

    ImageInfo imageInfo(cImageInfo->width, cImageInfo->height, static_cast<ColorType>(cImageInfo->colorType),
        static_cast<AlphaType>(cImageInfo->alphaType), std::shared_ptr<ColorSpace>{CastToColorSpace(cColorSpace),
        [](auto p) {}});

    if (!bitmap->Build(imageInfo, rowBytes)) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        delete bitmap;
        return nullptr;
    }

    return (OH_Drawing_Bitmap*)bitmap;
}

uint32_t OH_Drawing_BitmapGetRowBytes(const OH_Drawing_Bitmap* cBitmap)
{
    const Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return bitmap->GetRowBytes();
}

OH_Drawing_ColorSpaceType OH_Drawing_BitmapGetColorSpaceType(OH_Drawing_Bitmap* cBitmap)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return NO_TYPE;
    }

    ColorSpace* colorSpace = bitmap->GetColorSpace().get();
    if (colorSpace == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return NO_TYPE;
    }

    return ColorSpaceTypeCastToCColorSpaceType(bitmap->GetColorSpace()->GetType());
}

bool OH_Drawing_BitmapExtractSubset(
    const OH_Drawing_Bitmap* cBitmap, const OH_Drawing_Bitmap* cDstBitmap, const OH_Drawing_Rect* cSubset)
{
    const Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr || cDstBitmap == nullptr || cSubset == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }

    Rect rect = CastToRect(*cSubset);
    Bitmap dstBitmap = CastToBitmap(*cDstBitmap);
    return bitmap->ExtractSubset(dstBitmap, rect);
}

void OH_Drawing_BitmapSetImmutable(OH_Drawing_Bitmap* cBitmap)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    bitmap->SetImmutable();
}

bool OH_Drawing_BitmapIsImmutable(const OH_Drawing_Bitmap* cBitmap)
{
    const Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return bitmap->IsImmutable();
}

bool OH_Drawing_BitmapTryAllocPixels(OH_Drawing_Bitmap* cBitmap, const OH_Drawing_Image_Info* cImageInfo)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr || cImageInfo == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }

    ImageInfo imageInfo(cImageInfo->width, cImageInfo->height, static_cast<ColorType>(cImageInfo->colorType),
        static_cast<AlphaType>(cImageInfo->alphaType));
    return bitmap->TryAllocPixels(imageInfo);
}

OH_Drawing_Bitmap* OH_Drawing_BitmapCreateFromImageInfo(
    const OH_Drawing_Image_Info* cImageInfo, OH_Drawing_ColorSpace* cColorSpace, int32_t rowBytes)
{
    Bitmap* bitmap = new Bitmap;
    if (bitmap == nullptr || cColorSpace == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }

    ImageInfo imageInfo(cImageInfo->width, cImageInfo->height, static_cast<ColorType>(cImageInfo->colorType),
        static_cast<AlphaType>(cImageInfo->alphaType), std::shared_ptr<ColorSpace>{CastToColorSpace(cColorSpace),
        [](auto p) {}});

    if (!bitmap->Build(imageInfo, rowBytes)) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        delete bitmap;
        return nullptr;
    }

    return (OH_Drawing_Bitmap*)bitmap;
}

uint32_t OH_Drawing_BitmapGetRowBytes(const OH_Drawing_Bitmap* cBitmap)
{
    const Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return bitmap->GetRowBytes();
}

OH_Drawing_ColorSpaceType OH_Drawing_BitmapGetColorSpaceType(OH_Drawing_Bitmap* cBitmap)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return NO_TYPE;
    }

    ColorSpace* colorSpace = bitmap->GetColorSpace().get();
    if (colorSpace == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return NO_TYPE;
    }

    return ColorSpaceTypeCastToCColorSpaceType(bitmap->GetColorSpace()->GetType());
}

bool OH_Drawing_BitmapExtractSubset(
    const OH_Drawing_Bitmap* cBitmap, const OH_Drawing_Bitmap* cDstBitmap, const OH_Drawing_Rect* cSubset)
{
    const Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr || cDstBitmap == nullptr || cSubset == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }

    Rect rect = CastToRect(*cSubset);
    Bitmap dstBitmap = CastToBitmap(*cDstBitmap);
    return bitmap->ExtractSubset(dstBitmap, rect);
}

void OH_Drawing_BitmapSetImmutable(OH_Drawing_Bitmap* cBitmap)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    bitmap->SetImmutable();
}

bool OH_Drawing_BitmapIsImmutable(const OH_Drawing_Bitmap* cBitmap)
{
    const Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return bitmap->IsImmutable();
}

bool OH_Drawing_BitmapTryAllocPixels(OH_Drawing_Bitmap* cBitmap, const OH_Drawing_Image_Info* cImageInfo)
{
    Bitmap* bitmap = CastToBitmap(cBitmap);
    if (bitmap == nullptr || cImageInfo == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }

    ImageInfo imageInfo(cImageInfo->width, cImageInfo->height, static_cast<ColorType>(cImageInfo->colorType),
        static_cast<AlphaType>(cImageInfo->alphaType));
    return bitmap->TryAllocPixels(imageInfo);
}
