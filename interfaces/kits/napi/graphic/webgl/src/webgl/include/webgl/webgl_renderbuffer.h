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

#ifndef ROSENRENDER_ROSEN_WEBGL_RENDERBUFFER
#define ROSENRENDER_ROSEN_WEBGL_RENDERBUFFER

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLRenderbuffer final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLRenderbuffer";
    inline static const int objectType = WEBGL_OBJECT_RENDER_BUFFER;
    inline static const int DEFAULT_RENDER_BUFFER = 0;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLRenderbuffer **instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLRenderbuffer>(env, instance);
    }

    void SetRenderbuffer(uint32_t renderbuffer)
    {
        m_renderbuffer = renderbuffer;
    }

    uint32_t GetRenderbuffer() const
    {
        return m_renderbuffer;
    }

    explicit WebGLRenderbuffer() : m_renderbuffer(0) {};

    WebGLRenderbuffer(napi_env env, napi_value exports) : NExporter(env, exports), m_renderbuffer(0) {};

    ~WebGLRenderbuffer() {};

    static WebGLRenderbuffer *GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLRenderbuffer>(env, obj);
    }

    GLenum GetTarget() const { return m_target; }
    void SetTarget(GLenum target) { m_target = target; }

    void SetInternalFormat(GLenum internalFormat) { m_internalFormat = internalFormat; }
    GLenum GetInternalFormat() const { return m_internalFormat; }

    void SetSize(GLsizei width, GLsizei height)
    {
        m_width = width;
        m_height = height;
    }
    GLsizei GetWidth() const { return m_width; }
    GLsizei GetHeight() const { return m_height; }
private:
    uint32_t m_renderbuffer;
    GLenum m_target { 0 };
    GLenum m_internalFormat { GL_RGBA4 };
    GLsizei m_width { 0 };
    GLsizei m_height { 0 };
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_RENDERBUFFER
