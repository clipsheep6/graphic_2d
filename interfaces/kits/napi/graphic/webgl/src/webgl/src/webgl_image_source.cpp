/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "../include/webgl/webgl_image_source.h"  // for WebGLBuffer, WebGLBuffer:...

#include "__config"                         // for std
#include "iosfwd"                           // for string
#include "js_native_api_types.h"            // for napi_property_descriptor
#include "memory"                           // for make_unique, unique_ptr
#include "new"                              // for operator delete
#include "string"                           // for basic_string
#include "tuple"                            // for tuple, tie
#include "type_traits"                      // for move
#include "vector"                           // for vector

#include "../../common/napi/n_class.h"      // for NClass
#include "../../common/napi/n_func_arg.h"   // for NFuncArg, NARG_CNT, ZERO
#include "../../common/napi/n_val.h"              // for NVal
#include "../include/util/log.h"

namespace OHOS {
namespace Rosen {
using namespace std;

std::unique_ptr<WebGLImageSource> WebGLImageSource::GetImageSource(napi_env env, napi_value texImageSource)
{
    std::unique_ptr<WebGLImageSource> imageSource = std::make_unique<WebGLImageSource>();

    bool succ = false;
    napi_value resultObject = nullptr;
    napi_status statusObject = napi_coerce_to_object(env, texImageSource, &resultObject);
    if (statusObject != napi_ok) {
        LOGE("WebGl TexImage2D failed to get imageSource");
        return nullptr;
    }
    napi_value resultWidth = nullptr;
    napi_status widthStatus = napi_get_named_property(env, resultObject, "width", &resultWidth);
    if (widthStatus != napi_ok) {
        LOGE("WebGl TexImage2D failed to parse width");
        return nullptr;
    }

    tie(succ, imageSource->width) = NVal(env, resultWidth).ToInt64();
    if (!succ) {
        LOGE("WebGl TexImage2D failed to get width");
        return nullptr;
    }
    napi_value resultHeight = nullptr;
    napi_status heightStatus = napi_get_named_property(env, resultObject, "height", &resultHeight);
    if (heightStatus != napi_ok) {
        LOGE("WebGl TexImage2D failed to parse height");
        return nullptr;
    }
    tie(succ, imageSource->height) = NVal(env, resultHeight).ToInt64();
    if (!succ) {
        LOGE("WebGl TexImage2D failed to get height");
        return nullptr;
    }
    napi_value resultData = nullptr;
    napi_status dataRes = napi_get_named_property(env, resultObject, "data", &resultData);
    if (dataRes != napi_ok) {
        LOGE("WebGl TexImage2D failed to get data");
        return nullptr;
    }
    napi_value resultStr;
    napi_status rsuStatus = napi_coerce_to_string(env, resultData, &resultStr);
    if (rsuStatus != napi_ok) {
        LOGE("WebGl TexImage2D resultStr is not ok");
        return nullptr;
    }
    tie(succ, imageSource->source, ignore) = NVal(env, resultStr).ToUTF8String();
    if (!succ) {
        LOGI("WebGl TexImage2D ToUTF8String is not ok");
        return nullptr;
    }
    return imageSource;
}

void WebGLImage2DArg::Dump()
{
    LOGI("WebGl Image2DArg target: %{public}u", target);
    LOGI("WebGl Image2DArg level: %{public}u", level);
    LOGI("WebGl Image2DArg internalFormat: %{public}u", internalFormat);
    LOGI("WebGl Image2DArg width: %{public}u", width);
    LOGI("WebGl Image2DArg height: %{public}u", height);
    LOGI("WebGl Image2DArg border: %{public}u", border);
    LOGI("WebGl Image2DArg format: %{public}u", format);
    LOGI("WebGl Image2DArg type: %{public}u", type);
    LOGI("WebGl Image2DArg source: %{public}p", source);
}

void WebGLSubImage2DArg::Dump()
{
    LOGI("WebGLSubImage2DArg target: %{public}u", target);
    LOGI("WebGLSubImage2DArg level: %{public}u", level);
    LOGI("WebGLSubImage2DArg xoffset: %{public}u", xoffset);
    LOGI("WebGLSubImage2DArg xoffset: %{public}u", yoffset);
    LOGI("WebGLSubImage2DArg width: %{public}u", width);
    LOGI("WebGLSubImage2DArg height: %{public}u", height);
    LOGI("WebGLSubImage2DArg format: %{public}u", format);
    LOGI("WebGLSubImage2DArg type: %{public}u", type);
    LOGI("WebGLSubImage2DArg source: %{public}p", source);
}

} // namespace Rosen
} // namespace OHOS
