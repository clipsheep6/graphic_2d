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

#include "../include/webgl/webgl_shader_precision_format.h"  // for WebGLShaderPrecisionFormat

#include "__config"                                          // for std
#include "iosfwd"                                            // for string
#include "js_native_api_types.h"                             // for napi_pro...
#include "memory"                                            // for make_unique
#include "new"                                               // for operator...
#include "string"                                            // for basic_st...
#include "tuple"                                             // for tuple, tie
#include "type_traits"                                       // for move
#include "vector"                                            // for vector

#include "../../common/napi/n_class.h"                       // for NClass
#include "../../common/napi/n_func_arg.h"                    // for NFuncArg
#include "../include/util/log.h"
#include "common/napi/n_val.h"                               // for NVal

namespace OHOS {
namespace Rosen {
using namespace std;

WebGLShaderPrecisionFormat *WebGLShaderPrecisionFormat::GetObjectFromArg(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        LOGE("WebGLShaderPrecisionFormat::GetObjectFromArg invalid arg");
        return nullptr;
    }
    if (funcArg.GetThisVar() == nullptr) {
        LOGE("WebGLShaderPrecisionFormat::GetObjectFromArg invalid arg");
        return nullptr;
    }
    WebGLShaderPrecisionFormat *obj = nullptr;
    napi_status status = napi_unwrap(env, funcArg.GetThisVar(), (void **) &obj);
    if (status != napi_ok) {
        return nullptr;
    }
    return obj;
}

napi_value WebGLShaderPrecisionFormat::GetShaderPrecisionFormatRangeMin(napi_env env, napi_callback_info info)
{
    WebGLShaderPrecisionFormat *obj = GetObjectFromArg(env, info);
    if (obj == nullptr) {
        return nullptr;
    }
    int size = obj->GetShaderPrecisionFormatRangeMin();
    LOGI("WebGLShaderPrecisionFormat::GetShaderPrecisionFormatRangeMin %d", size);
    return NVal::CreateInt64(env, size).val_;
}

napi_value WebGLShaderPrecisionFormat::GetShaderPrecisionFormatRangeMax(napi_env env, napi_callback_info info)
{
    WebGLShaderPrecisionFormat *obj = GetObjectFromArg(env, info);
    if (obj == nullptr) {
        return nullptr;
    }
    int size = obj->GetShaderPrecisionFormatRangeMax();
    LOGI("WebGLShaderPrecisionFormat::GetShaderPrecisionFormatRangeMax %d", size);
    return NVal::CreateInt64(env, size).val_;
}

napi_value WebGLShaderPrecisionFormat::GetShaderPrecisionFormatPrecision(napi_env env, napi_callback_info info)
{
    WebGLShaderPrecisionFormat *obj = GetObjectFromArg(env, info);
    if (obj == nullptr) {
        return nullptr;
    }
    int precision = obj->GetShaderPrecisionFormatPrecision();
    LOGI("WebGLShaderPrecisionFormat::GetShaderPrecisionFormatPrecision %d", precision);
    return NVal::CreateInt64(env, precision).val_;
}

napi_value WebGLShaderPrecisionFormat::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLShaderPrecisionFormat> webGLShaderPrecisionFormat = make_unique<WebGLShaderPrecisionFormat>();
    if (!NClass::SetEntityFor<WebGLShaderPrecisionFormat>(env, funcArg.GetThisVar(),
        move(webGLShaderPrecisionFormat))) {
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLShaderPrecisionFormat::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiGetter("precision", WebGLShaderPrecisionFormat::GetShaderPrecisionFormatPrecision),
        NVal::DeclareNapiGetter("rangeMax", WebGLShaderPrecisionFormat::GetShaderPrecisionFormatRangeMax),
        NVal::DeclareNapiGetter("rangeMin", WebGLShaderPrecisionFormat::GetShaderPrecisionFormatRangeMin)
    };

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) = NClass::DefineClass(exports_.env_, className,
                                          WebGLShaderPrecisionFormat::Constructor, std::move(props));
    if (!succ) {
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLShaderPrecisionFormat::GetClassName()
{
    return WebGLShaderPrecisionFormat::className;
}
} // namespace Rosen
} // namespace OHOS
