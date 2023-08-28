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

#include <map>

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
enum AttachmentType : uint32_t {
    RENDER_BUFFER,
    TEXTURE,
    INVALID_TYPE,
};

struct WebGLAttachment {
    WebGLAttachment(AttachmentType type, GLenum attachment, GLuint id)
    {
        this->type = type;
        this->attachment = attachment;
        this->id = id;
    }
    bool IsTexture()
    {
        return type == AttachmentType::TEXTURE;
    }
    bool IsRenderBuffer()
    {
        return type == AttachmentType::RENDER_BUFFER;
    }
    AttachmentType type;
    GLenum attachment;
    GLuint id;
};

struct AttachmentTexture : public WebGLAttachment {
    AttachmentTexture(AttachmentType type, GLenum attachment, GLuint id)
    : WebGLAttachment(type, attachment, id) {}
    GLenum target {};
    GLint level {};
};

class WebGLFramebuffer : public NExporter, public WebGLObject {
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
        framebuffer_ = framebuffer;
    }

    unsigned int GetFramebuffer() const
    {
        return framebuffer_;
    }

    explicit WebGLFramebuffer() : framebuffer_(0) {};

    WebGLFramebuffer(napi_env env, napi_value exports) : NExporter(env, exports), framebuffer_(0) {};

    ~WebGLFramebuffer();

    static WebGLFramebuffer *GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLFramebuffer>(env, obj);
    }

    GLenum GetTarget() const
    {
        return target_;
    }

    void SetTarget(GLenum target)
    {
        target_ = target;
    }
    bool AddAttachment(GLenum attachment, GLuint id);
    bool AddAttachment(GLenum attachment, GLuint id, GLenum target, GLint level);
    WebGLAttachment* GetAttachment(GLenum attachment);
    void RemoveAttachment(GLenum attachment);
    void RemoveAttachment(GLuint id, AttachmentType type);
private:
    void Clear();
    std::map<GLenum, WebGLAttachment *> attachments_ {};
    GLenum target_;
    unsigned int framebuffer_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_FRAMEBUFFER
