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

#include "js_image.h"

#include "../js_drawing_utils.h"
#include "../js_common.h"
#include "native_value.h"
#include "image/image.h"
#include "image/bitmap.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsImage::constructor_ = nullptr;
const std::string CLASS_NAME = "Image";

napi_value JsImage::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("buildFromBitmap", JsImage::BuildFromBitmap),
        DECLARE_NAPI_FUNCTION("getWidth", JsImage::GetWidth),
        DECLARE_NAPI_FUNCTION("getHeight", JsImage::GetHeight),
        DECLARE_NAPI_FUNCTION("isOpaque", JsImage::IsOpaque),
        DECLARE_NAPI_FUNCTION("getImageInfo", JsImage::GetImageInfo),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define image class");
        return nullptr;
    }
    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to create reference of constructor");
        return nullptr;
    }
    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to set constructor");
        return nullptr;
    }
    return exportObj;
}

napi_value JsImage::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }
    JsImage *jsImage = new(std::nothrow) JsImage(std::make_shared<Image>());
    status = napi_wrap(env, jsThis, jsImage, JsImage::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsImage;
        ROSEN_LOGE("JsImage::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsImage::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsImage *napi = reinterpret_cast<JsImage *>(nativeObject);
        delete napi;
    }
}

napi_value JsImage::BuildFromBitmap(napi_env env, napi_callback_info info)
{
    JsImage* me = CheckParamsAndGetThis<JsImage>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    auto image = me->GetImage();
    if (image == nullptr) {
        ROSEN_LOGE("JsImage::BuildFromBitmap image is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc != ARGC_ONE) {
        ROSEN_LOGE("JsImage::BuildFromBitmap Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
#if 0
    JsBitmap* jsBitmap = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsBitmap));
    if (jsBitmap == nullptr) {
        return nullptr;
    }
    auto bitmap = jsBitmap->GetBitmap();
#else
    Bitmap *bitmap = new Bitmap();
    BitmapFormat bitmapFormat {
        .colorType = ColorType::COLORTYPE_ARGB_4444,
        .alphaType = AlphaType::ALPHATYPE_OPAQUE
    };
    bitmap->Build(400, 200, bitmapFormat);
#endif
    image->BuildFromBitmap(*bitmap);
    return NapiGetUndefined(env);
}

napi_value JsImage::GetWidth(napi_env env, napi_callback_info info)
{
    JsImage* me = CheckParamsAndGetThis<JsImage>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    auto image = me->GetImage();
    if (image == nullptr) {
        ROSEN_LOGE("JsImage::GetWidth image is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = 0;
    napi_value argv[ARGC_TWO] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc != 0) {
        ROSEN_LOGE("JsImage::GetWidth Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    auto width = image->GetWidth();
    return CreateJsValue(env, width);
}

napi_value JsImage::GetHeight(napi_env env, napi_callback_info info)
{
    JsImage* me = CheckParamsAndGetThis<JsImage>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    auto image = me->GetImage();
    if (image == nullptr) {
        ROSEN_LOGE("JsImage::GetHeight image is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = 0;
    napi_value argv[ARGC_TWO] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc != 0) {
        ROSEN_LOGE("JsImage::GetHeight Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    auto height = image->GetHeight();
    return CreateJsValue(env, height);
}

napi_value JsImage::IsOpaque(napi_env env, napi_callback_info info)
{
    JsImage* me = CheckParamsAndGetThis<JsImage>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    auto image = me->GetImage();
    if (image == nullptr) {
        ROSEN_LOGE("JsImage::IsOpaque image is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = 0;
    napi_value argv[ARGC_TWO] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc != 0) {
        ROSEN_LOGE("JsImage::IsOpaque Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    auto isOpaque = image->IsOpaque();
    return CreateJsValue(env, isOpaque);
}

napi_value JsImage::GetImageInfo(napi_env env, napi_callback_info info)
{
    JsImage* me = CheckParamsAndGetThis<JsImage>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    auto image = me->GetImage();
    if (image == nullptr) {
        ROSEN_LOGE("JsImage::GetImageInfo image is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = 0;
    napi_value argv[ARGC_TWO] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc != 0) {
        ROSEN_LOGE("JsImage::GetImageInfo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    return CreateImageInfoJsValue(env, image);
}

std::shared_ptr<Image> JsImage::GetImage()
{
    return m_image;
}
} // namespace Drawing
} // namespace OHOS::Rosen