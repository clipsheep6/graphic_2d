/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "color_space_object_convertor.h"

#include "native_engine/native_reference.h"

#include "js_color_space.h"
#include "js_color_space_utils.h"

namespace OHOS {
namespace ColorManager {
napi_value CreateJsColorSpaceObject(napi_env env, std::shared_ptr<ColorSpace>& colorSpace)
{
    if (colorSpace == nullptr) {
        CMLOGE("[NAPI]colorSpace is nullptr");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
            "[NAPI]colorSpace is nullptr"));
        return nullptr;
    }
    napi_value object = nullptr;
    napi_create_object(env, &object);
    if (object == nullptr) {
        CMLOGE("[NAPI]Fail to convert to js object");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
            "[NAPI]Fail to convert to js object"));
        napi_get_undefined(env, &object);
        return object;
    }

    std::unique_ptr<JsColorSpace> jsColorSpace = std::make_unique<JsColorSpace>(colorSpace);
    napi_wrap(env, object, jsColorSpace.release(), JsColorSpace::Finalizer, nullptr, nullptr);
    BindFunctions(env, object);

    std::shared_ptr<NativeReference> jsColorSpaceNativeRef;
    napi_ref jsColorSpaceRef = nullptr;
    napi_create_reference(env, object, 1, &jsColorSpaceRef);
    jsColorSpaceNativeRef.reset(reinterpret_cast<NativeReference*>(jsColorSpaceRef));
    return object;
}

std::shared_ptr<ColorSpace> GetColorSpaceByJSObject(napi_env env, napi_value object)
{
    if (object == nullptr) {
        CMLOGE("[NAPI]GetColorSpaceByJSObject::jsObject is nullptr");
        return nullptr;
    }
    JsColorSpace* jsColorSpace = nullptr;
    napi_unwrap(env, object, (void **)&jsColorSpace);
    if (jsColorSpace == nullptr) {
        CMLOGE("[NAPI]GetColorSpaceByJSObject::jsColorSpace is nullptr");
        return nullptr;
    }
    return jsColorSpace->GetColorSpaceToken();
}
}  // namespace ColorManager
}  // namespace OHOS
