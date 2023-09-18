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

#include "../include/webgl/webgl_buffer.h"  // for WebGLBuffer, WebGLBuffer:...
#include "../include/context/webgl_rendering_context_base.h"

#include "__config"                         // for std
#include "iosfwd"                           // for string
#include "js_native_api_types.h"            // for napi_property_descriptor
#include "memory"                           // for make_unique, unique_ptr
#include "new"                              // for operator delete
#include "string"                           // for basic_string
#include "tuple"                            // for tuple, tie
#include "type_traits"                      // for move
#include "vector"                           // for vector

#include "common/napi/n_class.h"      // for NClass
#include "common/napi/n_func_arg.h"   // for NFuncArg, NARG_CNT, ZERO
#include "common/napi/n_val.h"              // for NVal
#include "../include/util/log.h"

namespace OHOS {
namespace Rosen {
using namespace std;

napi_value WebGLBuffer::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLBuffer> webGlBuffer = make_unique<WebGLBuffer>();
    if (!NClass::SetEntityFor<WebGLBuffer>(env, funcArg.GetThisVar(), move(webGlBuffer))) {
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLBuffer::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {};

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) = NClass::DefineClass(exports_.env_, className, WebGLBuffer::Constructor, std::move(props));
    if (!succ) {
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLBuffer::GetClassName()
{
    return WebGLBuffer::className;
}

WebGLBuffer::~WebGLBuffer()
{
    LOGE("Free WebGLBuffer %{public}p", this);
    if (m_bufferData) {
        delete m_bufferData;
    }
}
} // namespace Rosen
} // namespace OHOS
