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

#include "webgl/webgl_program.h"  // for WebGLProgram, WebGLProgr...

#include "napi/n_class.h"       // for NClass
#include "napi/n_func_arg.h"    // for NFuncArg, NARG_CNT, ZERO
#include "napi/n_val.h"               // for NVal
#include "util/log.h"
#include "webgl/webgl_program.h"

namespace OHOS {
namespace Rosen {
using namespace std;

napi_value WebGLProgram::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLProgram> webGlProgram = make_unique<WebGLProgram>();
    if (!NClass::SetEntityFor<WebGLProgram>(env, funcArg.GetThisVar(), move(webGlProgram))) {
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLProgram::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {};
    LOGE("WebGLProgram::Export %{public}p", this);
    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) = NClass::DefineClass(exports_.env_, className, WebGLProgram::Constructor, std::move(props));
    if (!succ) {
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLProgram::GetClassName()
{
    return WebGLProgram::className;
}

WebGLProgram::WebGLProgram(napi_env env, napi_value exports) : NExporter(env, exports), m_programId(0)
{
    LOGE("WebGLProgram::WebGLProgram %{public}p", this);
};

WebGLProgram::~WebGLProgram()
{
    LOGE("~WebGLProgram::WebGLProgram %{public}p %{public}u", this, m_programId);
}

bool WebGLProgram::AttachShader(uint32_t index, uint32_t shaderId)
{
    if (index >= (sizeof(m_attachedShader) / sizeof(m_attachedShader[0]))) {
        return false;
    }
    if (m_attachedShader[index]) {
        return false;
    }
    m_attachedShader[index] = shaderId;
    return true;
}

bool WebGLProgram::DetachShader(uint32_t index, uint32_t shaderId)
{
    if (index >= (sizeof(m_attachedShader) / sizeof(m_attachedShader[0]))) {
        return false;
    }
    if (m_attachedShader[index] && m_attachedShader[index] == shaderId) {
        m_attachedShader[index] = 0;
        return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS
