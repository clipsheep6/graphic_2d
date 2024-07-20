/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "js_drawing_utils.h"

#ifdef ROSEN_OHOS
#include <parameters.h>
#endif

#include "draw/color.h"

namespace OHOS::Rosen {

#ifdef ROSEN_OHOS
bool JsDrawingTestUtils::closeDrawingTest_ =
    std::atoi((OHOS::system::GetParameter("persist.sys.graphic.drawing.test", "0").c_str())) != 1;
#else
bool JsDrawingTestUtils::closeDrawingTest_ = true;
#endif
namespace Drawing {
const char* const JSPROPERTY[4] = {"alpha", "red", "green", "blue"};
void BindNativeFunction(napi_env env, napi_value object, const char* name, const char* moduleName, napi_callback func)
{
    std::string fullName;
    if (moduleName) {
        fullName = moduleName;
        fullName += '.';
    }
    fullName += name;
    napi_value funcValue = nullptr;
    napi_create_function(env, fullName.c_str(), fullName.size(), func, nullptr, &funcValue);
    napi_set_named_property(env, object, fullName.c_str(), funcValue);
}

napi_value CreateJsError(napi_env env, int32_t errCode, const std::string& message)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, errCode), CreateJsValue(env, message), &result);
    return result;
}

bool ConvertFromJsTextEncoding(napi_env env, TextEncoding& textEncoding, napi_value nativeType)
{
    napi_value type = nativeType;
    if (type == nullptr) {
        return false;
    }
    uint32_t resultValue = 0;
    napi_get_value_uint32(env, type, &resultValue);

    switch (resultValue) {
        case 0: // 0: TextEncoding::UTF8
            textEncoding = TextEncoding::UTF8;
            break;
        case 1: // 1: TextEncoding::UTF16
            textEncoding = TextEncoding::UTF16;
            break;
        case 2: // 2: TextEncoding::UTF32
            textEncoding = TextEncoding::UTF32;
            break;
        case 3: // 3: TextEncoding::GLYPH_ID
            textEncoding = TextEncoding::GLYPH_ID;
            break;
        default: // default: TextEncoding::UTF8
            textEncoding = TextEncoding::UTF8;
            break;
    }
    return true;
}

napi_value NapiThrowError(napi_env env, DrawingErrorCode err, const std::string& message)
{
    napi_throw(env, CreateJsError(env, static_cast<int32_t>(err), message));
    return nullptr;
}

static const char* g_argbString[4] = {"alpha", "red", "green", "blue"};
static const char* g_ltrbString[4] = {"left", "top", "right", "bottom"};

bool ConvertFromJsColor(napi_env env, napi_value jsValue, int32_t* argb, size_t size)
{
    napi_value tempValue = nullptr;
    for (size_t idx = 0; idx < size; idx++) {
        int32_t* curChannel = argb + idx;
        napi_get_named_property(env, jsValue, g_argbString[idx], &tempValue);
        if (napi_get_value_int32(env, tempValue, curChannel) != napi_ok ||
            *curChannel < 0 || *curChannel > Color::RGB_MAX) {
            return false;
        }
    }
    return true;
}

bool ConvertFromJsRect(napi_env env, napi_value jsValue, double* ltrb, size_t size)
{
    napi_value tempValue = nullptr;
    for (size_t idx = 0; idx < size; idx++) {
        double* curEdge = ltrb + idx;
        napi_get_named_property(env, jsValue, g_ltrbString[idx], &tempValue);
        if (napi_get_value_double(env, tempValue, curEdge) != napi_ok) {
            return false;
        }
    }
    return true;
}

bool ConvertFromJsIRect(napi_env env, napi_value jsValue, int32_t* ltrb, size_t size)
{
    napi_value tempValue = nullptr;
    for (size_t idx = 0; idx < size; idx++) {
        int32_t* curEdge = ltrb + idx;
        napi_get_named_property(env, jsValue, g_ltrbString[idx], &tempValue);
        if (napi_get_value_int32(env, tempValue, curEdge) != napi_ok) {
            return false;
        }
    }
    return true;
}

bool ConvertFromJsPointsArray(napi_env env, napi_value array, Drawing::Point* points, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)  {
        napi_value tempPoint = nullptr;
        if (napi_get_element(env, array, i, &tempPoint) != napi_ok) {
            return false;
        }
        if (!GetPointFromJsValue(env, tempPoint, points[i])) {
            return false;
        }
    }
    return true;
}

napi_value GetFontMetricsAndConvertToJsValue(napi_env env, FontMetrics* metrics)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (metrics != nullptr && objValue != nullptr) {
        napi_set_named_property(env, objValue, "top", CreateJsNumber(env, metrics->fTop));
        napi_set_named_property(env, objValue, "ascent", CreateJsNumber(env, metrics->fAscent));
        napi_set_named_property(env, objValue, "descent", CreateJsNumber(env, metrics->fDescent));
        napi_set_named_property(env, objValue, "bottom", CreateJsNumber(env, metrics->fBottom));
        napi_set_named_property(env, objValue, "leading", CreateJsNumber(env, metrics->fLeading));
        napi_set_named_property(env, objValue, "flags", CreateJsNumber(env, metrics->fFlags));
        napi_set_named_property(env, objValue, "avgCharWidth", CreateJsNumber(env, metrics->fAvgCharWidth));
        napi_set_named_property(env, objValue, "maxCharWidth", CreateJsNumber(env, metrics->fMaxCharWidth));
        napi_set_named_property(env, objValue, "xMin", CreateJsNumber(env, metrics->fXMin));
        napi_set_named_property(env, objValue, "xMax", CreateJsNumber(env, metrics->fXMax));
        napi_set_named_property(env, objValue, "xHeight", CreateJsNumber(env, metrics->fXHeight));
        napi_set_named_property(env, objValue, "capHeight", CreateJsNumber(env, metrics->fCapHeight));
        napi_set_named_property(env, objValue, "underlineThickness", CreateJsNumber(env,
            metrics->fUnderlineThickness));
        napi_set_named_property(env, objValue, "underlinePosition", CreateJsNumber(env,
            metrics->fUnderlinePosition));
        napi_set_named_property(env, objValue, "strikethroughThickness", CreateJsNumber(env,
            metrics->fStrikeoutThickness));
        napi_set_named_property(env, objValue, "strikethroughPosition", CreateJsNumber(env,
            metrics->fStrikeoutPosition));
    }
    return objValue;
}

#ifdef ROSEN_OHOS
using namespace OHOS::Media;
std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(OHOS::Media::ColorSpace colorSpace)
{
    switch (colorSpace) {
        case OHOS::Media::ColorSpace::DISPLAY_P3:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        case OHOS::Media::ColorSpace::LINEAR_SRGB:
            return Drawing::ColorSpace::CreateSRGBLinear();
        case OHOS::Media::ColorSpace::SRGB:
            return Drawing::ColorSpace::CreateSRGB();
        default:
            return Drawing::ColorSpace::CreateSRGB();
    }
}

Drawing::ColorType PixelFormatToDrawingColorType(PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::RGB_565:
            return Drawing::ColorType::COLORTYPE_RGB_565;
        case PixelFormat::RGBA_8888:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        case PixelFormat::BGRA_8888:
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        case PixelFormat::ALPHA_8:
            return Drawing::ColorType::COLORTYPE_ALPHA_8;
        case PixelFormat::RGBA_F16:
            return Drawing::ColorType::COLORTYPE_RGBA_F16;
        case PixelFormat::UNKNOWN:
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGB_888:
        case PixelFormat::NV21:
        case PixelFormat::NV12:
        case PixelFormat::CMYK:
        default:
            return Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}

Drawing::AlphaType AlphaTypeToDrawingAlphaType(OHOS::Media::AlphaType alphaType)
{
    switch (alphaType) {
        case OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
        case OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return Drawing::AlphaType::ALPHATYPE_PREMUL;
        case OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        default:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}

struct PixelMapReleaseContext {
    explicit PixelMapReleaseContext(std::shared_ptr<PixelMap> pixelMap) : pixelMap_(pixelMap) {}

    ~PixelMapReleaseContext()
    {
        pixelMap_ = nullptr;
    }

private:
    std::shared_ptr<PixelMap> pixelMap_;
};

static void PixelMapReleaseProc(const void* /* pixels */, void* context)
{
    PixelMapReleaseContext* ctx = static_cast<PixelMapReleaseContext*>(context);
    if (ctx) {
        delete ctx;
        ctx = nullptr;
    }
}

std::shared_ptr<Drawing::Image> ExtractDrawingImage(std::shared_ptr<PixelMap> pixelMap)
{
    if (!pixelMap) {
        ROSEN_LOGE("ExtractDrawingImage pixelMap is null");
        return nullptr;
    }
    OHOS::Media::ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        PixelFormatToDrawingColorType(imageInfo.pixelFormat),
        AlphaTypeToDrawingAlphaType(imageInfo.alphaType),
        ColorSpaceToDrawingColorSpace(imageInfo.colorSpace) };
    Drawing::Pixmap imagePixmap(drawingImageInfo,
        reinterpret_cast<const void*>(pixelMap->GetPixels()), pixelMap->GetRowStride());
    PixelMapReleaseContext* releaseContext = new PixelMapReleaseContext(pixelMap);
    auto image = Drawing::Image::MakeFromRaster(imagePixmap, PixelMapReleaseProc, releaseContext);
    if (!image) {
        ROSEN_LOGE("ExtractDrawingImage Image::MakeFromRaster returned null");
        delete releaseContext;
        releaseContext = nullptr;
    }
    return image;
}
#endif
} // namespace Drawing
} // namespace OHOS::Rosen
