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

#ifndef ROSENRENDER_ROSEN_WEBGL_FRAMEBUFFER
#define ROSENRENDER_ROSEN_WEBGL_FRAMEBUFFER

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLFramebuffer final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLFramebuffer";
    inline static const int objectType = WEBGL_OBJECT_FRAME_BUFFER;
    inline static const int DEFAULT_FRAME_BUFFER = 0;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLFramebuffer **instance);

    void SetFramebuffer(unsigned int framebuffer)
    {
        m_framebuffer = framebuffer;
    }

    unsigned int GetFramebuffer() const
    {
        return m_framebuffer;
    }

    explicit WebGLFramebuffer() : m_framebuffer(0) {};

    WebGLFramebuffer(napi_env env, napi_value exports) : NExporter(env, exports), m_framebuffer(0) {};

    ~WebGLFramebuffer() {};

    static WebGLFramebuffer *GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLFramebuffer>(env, obj);
    }

    GLenum GetTarget() const { return m_target; }
    void SetTarget(GLenum target) { m_target = target; }
private:
    GLenum m_target;
    unsigned int m_framebuffer;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_FRAMEBUFFER
