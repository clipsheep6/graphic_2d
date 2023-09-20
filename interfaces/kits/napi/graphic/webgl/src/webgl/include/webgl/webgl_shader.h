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

#ifndef ROSENRENDER_ROSEN_WEBGL_SHADER
#define ROSENRENDER_ROSEN_WEBGL_SHADER

#include "../../../common/napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLShader final : public NExporter, WebGLObject {
public:
    inline static const std::string className = "WebGLShader";
    inline static const int objectType = WEBGL_OBJECT_SHADER;
    inline static const int DEFAULT_SHADER_ID = 0;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLShader **instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLShader>(env, instance);
    }

    void SetShaderId(uint32_t shaderId)
    {
        m_shaderId = shaderId;
    }

    uint32_t GetShaderId() const
    {
        return m_shaderId;
    }

    void SetShaderRes(const std::string &res)
    {
        m_res = std::move(res);
    }

    const std::string &GetShaderRes() const
    {
        return m_res;
    }

    explicit WebGLShader() : m_shaderId(0) {};

    WebGLShader(napi_env env, napi_value exports) : NExporter(env, exports), m_shaderId(0) {};

    ~WebGLShader() {};

    static WebGLShader *GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLShader>(env, obj);
    }
private:
    std::string m_res = {};
    uint32_t m_shaderId;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_SHADER
