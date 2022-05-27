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

#include "color_picker.h"
#include "hilog/log.h"
#include "effect_errors.h"
#include "effect_napi_utils.h"
#include "color.h"
#include "pixel_map.h"
//#include "core/SkImageInfo.h"
//#include "core/SkPixmap.h"

#include "include/core/SkBitmap.h"
#include "include/core/SkRect.h"
#include "include/core/SkImageFilter.h"
#include "include/effects/SkImageFilters.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkFont.h"
#include "include/core/SkTypeface.h"




#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using OHOS::HiviewDFX::HiLog;
//using namespace OHOS::HiviewDFX;

std::shared_ptr<ColorPicker> ColorPicker::CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap, uint32_t &errorCode)
{
    ColorPicker *colorPicker = new (std::nothrow) ColorPicker(pixmap);
    if (colorPicker == nullptr) {
        HiLog::Info(LABEL, "[ColorPicker]failed to create ColorPicker with pixmap.");
        errorCode = ERR_EFFECT_INVALID_VALUE;
        return nullptr;
    }
    errorCode = SUCCESS;
    return std::shared_ptr<ColorPicker>(colorPicker);
}

std::shared_ptr<Media::PixelMap> ColorPicker::GetPixelMap()
{
    // 1 print pixelmap info
    auto width = pixelmap_->GetWidth();
    auto height = pixelmap_->GetHeight();
    auto pixelFormat = pixelmap_->GetPixelFormat();
    auto alphaType = pixelmap_->GetAlphaType();
    auto pixelAddr = const_cast<uint8_t *>(pixelmap_->GetPixels());
    auto rowBytes = pixelmap_->GetRowBytes();
    HiLog::Info(LABEL, ".[pixelmap_]size=%{public}d X %{public}d", width, height);   // 1634 x 919
    HiLog::Info(LABEL, ".[pixelmap_]format=%{public}d alpha= %{public}d", pixelFormat, alphaType);   // 3  1
    HiLog::Info(LABEL, ".[pixelmap_]pix.addr=%{public}p rByte=%{public}d", pixelAddr, rowBytes);

#if 1
    // 2 Create pixelmap
    OHOS::Media::InitializationOptions options;
    options.alphaType = alphaType;
    options.pixelFormat = pixelFormat;
    options.scaleMode = OHOS::Media::ScaleMode::FIT_TARGET_SIZE;
    options.size.width = 1;
    options.size.height = 1;
    options.editable = true;
    std::unique_ptr<Media::PixelMap> newPixelMap = Media::PixelMap::Create(*pixelmap_.get(), options);

    HiLog::Info(LABEL, "[newpix].wh=%{public}d x %{public}d" , pixelmap_->GetWidth(), pixelmap_->GetHeight());
    HiLog::Info(LABEL, "[newpix].rbyte=%{public}d, bCnt=%{public}d" , pixelmap_->GetRowBytes(), pixelmap_->GetByteCount() );
    HiLog::Info(LABEL, "[newpix].pixByte=%{public}d" , pixelmap_->GetPixelBytes() );
#endif
    return std::move(newPixelMap);
}

#if 0
static SkColorType PixelFormatConvert(const Media::PixelFormat &pixelFormat)
{
    SkColorType colorType;
    switch (pixelFormat) {
        case Media::PixelFormat::BGRA_8888:
            colorType = SkColorType::kBGRA_8888_SkColorType;
            break;
        case Media::PixelFormat::RGBA_8888:
            colorType = SkColorType::kRGBA_8888_SkColorType;
            break;
        case Media::PixelFormat::RGB_565:
            colorType = SkColorType::kRGB_565_SkColorType;
            break;
        case Media::PixelFormat::ALPHA_8:
            colorType = SkColorType::kAlpha_8_SkColorType;
            break;
        default:
            colorType = SkColorType::kUnknown_SkColorType;
            break;
    }
    return colorType;
}
#endif

uint32_t ColorPicker::GetMainColor(Color &color)
{
    if (pixelmap_ == nullptr){
        return ERR_EFFECT_INVALID_VALUE;
    }
    //color = Color(0x00, 0xff, 0x00, 0xff);

    // 1 print pixelmap info
//    auto width = pixelmap_->GetWidth();
//    auto height = pixelmap_->GetHeight();
    auto pixelFormat = pixelmap_->GetPixelFormat();
    auto alphaType = pixelmap_->GetAlphaType();
//    auto pixelAddr = const_cast<uint8_t *>(pixelmap_->GetPixels());
//    auto rowBytes = pixelmap_->GetRowBytes();
    
#if 1
    // Create pixelmap
    OHOS::Media::InitializationOptions options;
    options.alphaType = alphaType;
    options.pixelFormat = pixelFormat;
    options.scaleMode = OHOS::Media::ScaleMode::FIT_TARGET_SIZE;
    options.size.width = 1;
    options.size.height = 1;
    options.editable = true;
    std::unique_ptr<Media::PixelMap> newPixelMap = Media::PixelMap::Create(*pixelmap_.get(), options);

    // get color
    uint32_t colorVal = 0;
    int x = 0;
    int y = 0;
    bool bSucc = newPixelMap->GetARGB32Color(x, y, colorVal);
    HiLog::Info(LABEL, "[newpix].argb.ret=%{public}d, %{public}x", bSucc, colorVal);
    color = Color(colorVal);
    
#endif

    return SUCCESS;
}


ColorPicker::ColorPicker(std::shared_ptr<Media::PixelMap> pixmap)
{
    if(pixmap == nullptr) {
        return ;
    }
    pixelmap_ = pixmap;

#if 0
    //  
    SkImageInfo drawImageInfo = SkImageInfo::Make(width, height,
                                                PixelFormatConvert(pixelFormat),
                                                static_cast<SkAlphaType>(alphaType));
    // SkPixmap drawPixmap(drawImageInfo, pixelAddr, rowBytes);
    SkBitmap bitmap;
    bitmap.installPixels(drawImageInfo, pixelAddr, rowBytes);

    HiLog::Info(LABEL, "[bitmap]size=%{public}d x %{public}d", bitmap.width(), bitmap.height());

    std::unique_ptr<SkCanvas> offscreen = SkCanvas::MakeRasterDirect(drawImageInfo, pixelAddr, rowBytes);
    offscreen->drawBitmap(bitmap, 0, 0);
    offscreen->scale(.5f, .5f);

    SkPaint p;
    p.setColor(SK_ColorRED);
    p.setAntiAlias(true);

    offscreen->translate(0, 0);
    offscreen->rotate(10);
    offscreen->drawRect({ 20, 20, 200, 200 }, p);
    
    SkFont font(SkTypeface::MakeFromName("monospace", SkFontStyle()), 100);
    offscreen->drawString("ABC", 20, 160, font, p);
    offscreen->drawString("DEF", 20, 160, font, p);

    HiLog::Info(LABEL, "[bitmap.scale]size=%{public}d x %{public}d", bitmap.width(), bitmap.height());


#endif    
}



} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
