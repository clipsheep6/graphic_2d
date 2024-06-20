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
namespace Drawing {
const std::string CLASS_NAME = "Bitmap";
thread_local napi_ref JsBitmap::constructor_ = nullptr;
std::shared_ptr<Bitmap> drawingBitmap;
napi_value JsBitmap::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
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