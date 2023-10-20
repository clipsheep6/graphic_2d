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

#ifndef ROSENRENDER_ROSEN_WEBGL_TEXTRUE
#define ROSENRENDER_ROSEN_WEBGL_TEXTRUE

#include "napi/n_exporter.h"
#include "webgl_object.h"
#include "webgl_arg.h"

namespace OHOS {
namespace Rosen {
class WebGLTexture final : public NExporter, WebGLObject {
public:
    inline static const std::string className = "WebGLTexture";
    inline static const int objectType = WEBGL_OBJECT_TEXTURE;
    inline static const int DEFAULT_TEXTURE = 0;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLTexture **instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLTexture>(env, instance);
    }

    void SetTexture(uint32_t texture)
    {
        m_texture = texture;
    }

    uint32_t GetTexture() const
    {
        return m_texture;
    }

    void SetTarget(GLenum target) { m_target = target; }
    GLenum GetTarget() { return m_target; }

    explicit WebGLTexture() : m_texture(0) {};

    WebGLTexture(napi_env env, napi_value exports) : NExporter(env, exports), m_texture(0) {};

    ~WebGLTexture() {};

    static WebGLTexture *GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLTexture>(env, obj);
    }

private:
    GLenum m_target;
    uint32_t m_texture;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_TEXTRUE