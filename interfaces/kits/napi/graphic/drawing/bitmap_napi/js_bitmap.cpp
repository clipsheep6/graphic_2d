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

#include "js_bitmap.h"
#include "../js_drawing_utils.h"
#include "native_value.h"
#include "../font_napi/js_font.h"
#include "../js_common.h"

namespace OHOS::Rosen {
static Drawing::AlphaType AlphaTypeToDrawingAlphaType(uint32_t alphaType)
{
    switch (alphaType) {
        case Drawing::AlphaType::ALPHATYPE_UNKNOWN:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
        case Drawing::AlphaType::ALPHATYPE_OPAQUE:
            return Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case Drawing::AlphaType::ALPHATYPE_PREMUL:
            return Drawing::AlphaType::ALPHATYPE_PREMUL;
        case Drawing::AlphaType::ALPHATYPE_UNPREMUL:
            return Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        default:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}

static Drawing::ColorType ColorTypeToDrawingColorType(uint32_t colorType)
{
    switch (colorType) {
        case Drawing::ColorType::COLORTYPE_UNKNOWN:
            return Drawing::ColorType::COLORTYPE_UNKNOWN ;
        case Drawing::ColorType::COLORTYPE_ALPHA_8:
            return Drawing::ColorType::COLORTYPE_ALPHA_8;
        case Drawing::ColorType::COLORTYPE_RGB_565:
            return Drawing::ColorType::COLORTYPE_RGB_565;
        case Drawing::ColorType::COLORTYPE_ARGB_4444:
            return Drawing::ColorType::COLORTYPE_ARGB_4444;
        case Drawing::ColorType::COLORTYPE_RGBA_8888:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        case Drawing::ColorType::COLORTYPE_BGRA_8888:
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        default:
            return Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}
namespace Drawing {
const std::string CLASS_NAME = "Bitmap";
thread_local napi_ref JsBitmap::constructor_ = nullptr;
std::shared_ptr<Bitmap> drawingBitmap;
napi_value JsBitmap::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("build", JsBitmap::Build),
        DECLARE_NAPI_FUNCTION("getWidth", JsBitmap::GetWidth),
        DECLARE_NAPI_FUNCTION("getHeight", JsBitmap::GetHeight),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to define Bitmap class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to set constructor");
        return nullptr;
    }

    status = napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to define static function");
        return nullptr;
    }
    return exportObj;
}

JsBitmap::JsBitmap()
{
    bitmap_ = new Bitmap();
}

JsBitmap::~JsBitmap()
{
    delete bitmap_;
}

napi_value JsBitmap::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    JsBitmap *jsBitmap = new(std::nothrow) JsBitmap();

    status = napi_wrap(env, jsThis, jsBitmap,
                       JsBitmap::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsBitmap;
        ROSEN_LOGE("Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsBitmap::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsBitmap *napi = reinterpret_cast<JsBitmap *>(nativeObject);
        delete napi;
    }
}

napi_value JsBitmap::Build(napi_env env, napi_callback_info info)
{
    JsBitmap* me = CheckParamsAndGetThis<JsBitmap>(env, info);
    return (me != nullptr) ? me->OnBuild(env, info) : nullptr;
}

napi_value JsBitmap::GetWidth(napi_env env, napi_callback_info info)
{
    JsBitmap* me = CheckParamsAndGetThis<JsBitmap>(env, info);
    return (me != nullptr) ? me->OnGetWidth(env, info) : nullptr;
}

napi_value JsBitmap::GetHeight(napi_env env, napi_callback_info info)
{
    JsBitmap* me = CheckParamsAndGetThis<JsBitmap>(env, info);
    return (me != nullptr) ? me->OnGetHeight(env, info) : nullptr;
}

napi_value JsBitmap::OnBuild(napi_env env, napi_callback_info info)
{
    if (bitmap_ == nullptr) {
        ROSEN_LOGE("Drawing_napi: OnBuild bitmap is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("Drawing_napi: OnBuild Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    int32_t width = 0;
    int32_t height = 0;
    ConvertFromJsValue(env, argv[0], width);
    ConvertFromJsValue(env, argv[ARGC_ONE], height);

    uint32_t colorType = 0;
    uint32_t alphaType = 0;
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argv[ARGC_TWO], "colorFormat", &tempValue);
    napi_get_value_uint32(env, tempValue, &colorType);
    napi_get_named_property(env, argv[ARGC_TWO], "alphaFormat", &tempValue);
    napi_get_value_uint32(env, tempValue, &alphaType);


    BitmapFormat bitmapFormat;
    bitmapFormat.colorType = ColorTypeToDrawingColorType(colorType);
    bitmapFormat.alphaType = AlphaTypeToDrawingAlphaType(alphaType);

    bitmap_->Build(width, height, bitmapFormat);
    return NapiGetUndefined(env);
}

napi_value JsBitmap::OnGetWidth(napi_env env, napi_callback_info info)
{
    if (bitmap_ == nullptr) {
        ROSEN_LOGE("Drawing_napi: OnGetWidth bitmap is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    
    return CreateJsNumber(env,bitmap_->GetWidth());
}

napi_value JsBitmap::OnGetHeight(napi_env env, napi_callback_info info)
{
    if (bitmap_ == nullptr) {
        ROSEN_LOGE("Drawing_napi: OnGetHeight bitmap is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsNumber(env,bitmap_->GetHeight());
}
napi_value JsBitmap::CreateJsBitmap(napi_env env, const std::shared_ptr<Bitmap> bitmap)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        drawingBitmap = bitmap;
        status = napi_new_instance(env, constructor, 0, nullptr, &result);
        if (status == napi_ok) {
            return result;
        } else {
            ROSEN_LOGE("JsBitmap::CreateJsBitmap New instance could not be obtained");
        }
    }
    return result;
}

Bitmap* JsBitmap::GetBitmap()
{
    return bitmap_;
}
} // namespace Drawing
} // namespace OHOS::Rosen