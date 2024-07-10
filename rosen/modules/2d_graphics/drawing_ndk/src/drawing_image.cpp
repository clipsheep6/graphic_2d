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

#include "image/image.h"
#include <mutex>
#include <unordered_map>
#include "utils/data.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static std::mutex g_imageLockMutex;
static std::unordered_map<void*, std::shared_ptr<Image>> g_imageMap;

static Image* CastToImage(OH_Drawing_Image* cImage)
{
    return reinterpret_cast<Image*>(cImage);
}

static Bitmap& CastToBitmap(OH_Drawing_Bitmap& cBitmap)
{
    return reinterpret_cast<Bitmap&>(cBitmap);
}

OH_Drawing_Image* OH_Drawing_ImageCreate()
{
    return (OH_Drawing_Image*)new Image();
}

void OH_Drawing_ImageDestroy(OH_Drawing_Image* cImage)
{
    std::lock_guard<std::mutex> lock(g_imageLockMutex);
    auto it = g_imageMap.find(cImage);
    if (it != g_imageMap.end()) {
        g_imageMap.erase(it);
        return;
    }
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

OH_Drawing_ErrorCode OH_Drawing_ImageIsOpaque(OH_Drawing_Image* cImage, bool* isOpaque)
{
    if (cImage == nullptr || isOpaque == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *isOpaque = CastToImage(cImage)->IsOpaque();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_Image* OH_Drawing_ImageCreateFromRaster(OH_Drawing_Pixmap* pixmap,
    void (*rasterReleaseProc)(const OH_Drawing_Pixmap* pixmap, void* releaseContext), void* releaseContext)
{
    if (pixmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    auto image = Image::MakeFromRaster(*reinterpret_cast<Pixmap*>(pixmap), (RasterReleaseProc)rasterReleaseProc,
                                       reinterpret_cast<ReleaseContext>(releaseContext));
    std::lock_guard<std::mutex> lock(g_imageLockMutex);
    g_imageMap.insert({image.get(), image});
    return (OH_Drawing_Image*)image.get();
}