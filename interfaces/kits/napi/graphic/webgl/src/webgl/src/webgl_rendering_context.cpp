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

#include "../include/context/webgl_rendering_context.h"

#include "../include/util/object_manager.h"
#include "../include/util/log.h"
#include "../include/util/egl_manager.h"
#include "../../common/napi/n_class.h"
#include "../../common/napi/n_func_arg.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;
bool WebGLRenderingContext::Export(napi_env env, napi_value exports)
{
    LOGI("WebGL WebGLRenderingContext::Export env %{public}p mContextRef %{public}p", env, mContextRef);
    napi_value instanceValue = GetContextInstance(env,
        GetClassName(), [](napi_env env, napi_callback_info info) -> napi_value {
            napi_value thisVar = nullptr;
            napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
            LOGI("WebGL WebGLRenderingContext::create");
            return thisVar;
        }, [](napi_env env, void* data, void* hint) {
            auto entity = static_cast<WebGLRenderingContext *>(data);
            LOGI("WebGL WebGLRenderingContext::delete");
            if (entity->mContextRef) {
                napi_delete_reference(env, entity->mContextRef);
            }
            if (entity->mEGLSurface != nullptr) {
                eglDestroySurface(EglManager::GetInstance().GetEGLDisplay(), entity->mEGLSurface);
                entity->mEGLSurface = nullptr;
            }
            delete entity;
        }
    );
    if (instanceValue == nullptr) {
        LOGE("Failed to create instance");
        return false;
    }

    std::vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("bufferData", WebGLRenderingContextOverloads::BufferData),
        NVal::DeclareNapiFunction("bufferSubData", WebGLRenderingContextOverloads::BufferSubData),
        NVal::DeclareNapiFunction("compressedTexImage2D", WebGLRenderingContextOverloads::CompressedTexImage2D),
        NVal::DeclareNapiFunction("compressedTexSubImage2D", WebGLRenderingContextOverloads::CompressedTexSubImage2D),
        NVal::DeclareNapiFunction("readPixels", WebGLRenderingContextOverloads::ReadPixels),
        NVal::DeclareNapiFunction("texImage2D", WebGLRenderingContextOverloads::TexImage2D),
        NVal::DeclareNapiFunction("texSubImage2D", WebGLRenderingContextOverloads::TexSubImage2D),
        NVal::DeclareNapiFunction("uniform1fv", WebGLRenderingContextOverloads::Uniform1fv),
        NVal::DeclareNapiFunction("uniform2fv", WebGLRenderingContextOverloads::Uniform2fv),
        NVal::DeclareNapiFunction("uniform3fv", WebGLRenderingContextOverloads::Uniform3fv),
        NVal::DeclareNapiFunction("uniform4fv", WebGLRenderingContextOverloads::Uniform4fv),
        NVal::DeclareNapiFunction("uniform1iv", WebGLRenderingContextOverloads::Uniform1iv),
        NVal::DeclareNapiFunction("uniform2iv", WebGLRenderingContextOverloads::Uniform2iv),
        NVal::DeclareNapiFunction("uniform3iv", WebGLRenderingContextOverloads::Uniform3iv),
        NVal::DeclareNapiFunction("uniform4iv", WebGLRenderingContextOverloads::Uniform4iv),
        NVal::DeclareNapiFunction("uniformMatrix2fv", WebGLRenderingContextOverloads::UniformMatrix2fv),
        NVal::DeclareNapiFunction("uniformMatrix3fv", WebGLRenderingContextOverloads::UniformMatrix3fv),
        NVal::DeclareNapiFunction("uniformMatrix4fv", WebGLRenderingContextOverloads::UniformMatrix4fv),
        NVal::DeclareNapiProperty("WebGLRenderingContext", instanceValue),
    };
    std::vector<napi_property_descriptor> properties = {};
    WebGLRenderingContextBase::GetRenderingContextBasePropertyDesc(properties);
    properties.insert(properties.end(), props.begin(), props.end());
    LOGI("WebGLRenderingContext properties %{public}d", properties.size());
    napi_status status = napi_define_properties(env, exports, properties.size(), properties.data());
    if (status != napi_ok) {
        return false;
    }
    return true;
}

string WebGLRenderingContext::GetClassName()
{
    return WebGLRenderingContext::className;
}

WebGLRenderingContext::~WebGLRenderingContext()
{
    LOGI("WebGLRenderingContext::~WebGLRenderingContext id %{public}p", this);
    auto& webgl1Objects = ObjectManager::GetInstance().GetWebgl1ObjectMap();
    for (auto iter = webgl1Objects.begin(); iter != webgl1Objects.end(); iter++) {
        if (iter->second == this) {
            webgl1Objects.erase(iter);
            break;
        }
    }
}

WebGLRenderingContext::WebGLRenderingContext(napi_env env, napi_value exports)
    : WebGLRenderingContextBasicBase(), NExporter(env, exports), contextImpl_(0)
{
    LOGI("WebGLRenderingContext::WebGLRenderingContext id %{public}p", this);
}

void WebGLRenderingContext::Init()
{
    LOGI("WebGLRenderingContext::Init id %{public}p", this);
    WebGLRenderingContextBasicBase::Init();
    contextImpl_.Init();
}

void WebGLRenderingContext::SetBitMapPtr(char *bitMapPtr, int bitMapWidth, int bitMapHeight)
{
    LOGI("WebGLRenderingContext::SetBitMapPtr %{public}p", this);
    WebGLRenderingContextBasicBase::SetBitMapPtr(bitMapPtr, bitMapWidth, bitMapHeight);
    contextImpl_.SetBitMapPtr(bitMapPtr, bitMapWidth, bitMapHeight);
}

} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
