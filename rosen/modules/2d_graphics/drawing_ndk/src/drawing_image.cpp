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

#include "drawing_image.h"

#include "drawing_canvas_utils.h"

#include "effect/color_space.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "utils/data.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Image* CastToImage(OH_Drawing_Image* cImage)
{
    return reinterpret_cast<Image*>(cImage);
}

static const Image* CastToImage(const OH_Drawing_Image* cImage)
{
    return reinterpret_cast<const Image*>(cImage);
}

static Bitmap& CastToBitmap(OH_Drawing_Bitmap& cBitmap)
{
    return reinterpret_cast<Bitmap&>(cBitmap);
}

static const Bitmap* CastToBitmap(const OH_Drawing_Bitmap* cBitmap)
{
    return reinterpret_cast<const Bitmap*>(cBitmap);
}

static const SamplingOptions* CastToSamplingOptions(const OH_Drawing_SamplingOptions* cSamplingOptions)
{
    return reinterpret_cast<const SamplingOptions*>(cSamplingOptions);
}

OH_Drawing_Image* OH_Drawing_ImageCreate()
{
    return (OH_Drawing_Image*)new Image();
}

void OH_Drawing_ImageDestroy(OH_Drawing_Image* cImage)
{
    delete CastToImage(cImage);
}

bool OH_Drawing_ImageBuildFromBitmap(OH_Drawing_Image* cImage, OH_Drawing_Bitmap* cBitmap)
{
    if (cImage == nullptr || cBitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return CastToImage(cImage)->BuildFromBitmap(CastToBitmap(*cBitmap));
}

OH_Drawing_ColorSpace* OH_Drawing_ImageGetColorSpace(const OH_Drawing_Image* cImage)
{
    const Image* image = CastToImage(cImage);
    if (image == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    std::shared_ptr<ColorSpace> colorSpacePtr = image->GetColorSpace();

    if (colorSpacePtr == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    return (OH_Drawing_ColorSpace*)colorSpacePtr.get();
}

int32_t OH_Drawing_ImageGetWidth(OH_Drawing_Image* cImage)
{
    if (cImage == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return -1;
    }
    return CastToImage(cImage)->GetWidth();
}

int32_t OH_Drawing_ImageGetHeight(OH_Drawing_Image* cImage)
{
    if (cImage == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return -1;
    }
    return CastToImage(cImage)->GetHeight();
}

void OH_Drawing_ImageGetImageInfo(OH_Drawing_Image* cImage, OH_Drawing_Image_Info* cImageInfo)
{
    if (cImage == nullptr || cImageInfo == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    ImageInfo imageInfo = CastToImage(cImage)->GetImageInfo();

    cImageInfo->width = imageInfo.GetWidth();
    cImageInfo->height = imageInfo.GetHeight();
    cImageInfo->colorType = static_cast<OH_Drawing_ColorFormat>(imageInfo.GetColorType());
    cImageInfo->alphaType = static_cast<OH_Drawing_AlphaFormat>(imageInfo.GetAlphaType());
}

uint32_t OH_Drawing_ImageGetUniqueID(const OH_Drawing_Image* cImage)
{
    const Image* image = CastToImage(cImage);
    if (image == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return image->GetUniqueID();
}

bool OH_Drawing_ImageIsOpaque(const OH_Drawing_Image* cImage)
{
    const Image* image = CastToImage(cImage);
    if (image == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return image->IsOpaque();
}

bool OH_Drawing_ImageScalePixels(const OH_Drawing_Image* cImage, const OH_Drawing_Bitmap* cBitmap,
    const OH_Drawing_SamplingOptions* cSampling, bool isAllowCachingHint)
{
    const Image* image = CastToImage(cImage);
    const Bitmap* bitmap = CastToBitmap(cBitmap);
    const SamplingOptions* sampling = CastToSamplingOptions(cSampling);
    if (image == nullptr || bitmap == nullptr || sampling == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return image->ScalePixels(*bitmap, *sampling, isAllowCachingHint);
}