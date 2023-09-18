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

#include "../include/webgl/webgl_texture.h"

#include "../../common/napi/n_class.h"
#include "../../common/napi/n_func_arg.h"
#include "../include/util/util.h"
#include "../include/context/webgl_rendering_context_base.h"
#include "../include/context/webgl_rendering_context_basic_base.h"
#include "../include/context/webgl2_rendering_context_base.h"

namespace OHOS {
namespace Rosen {
using namespace std;

napi_value WebGLTexture::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLTexture> webGlTexture = make_unique<WebGLTexture>();
    if (!NClass::SetEntityFor<WebGLTexture>(env, funcArg.GetThisVar(), move(webGlTexture))) {
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLTexture::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {};

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) = NClass::DefineClass(exports_.env_, className, WebGLTexture::Constructor, std::move(props));
    if (!succ) {
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLTexture::GetClassName()
{
    return WebGLTexture::className;
}
} // namespace Rosen
} // namespace OHOS
