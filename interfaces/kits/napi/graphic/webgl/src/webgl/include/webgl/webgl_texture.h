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

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl31.h>

#include "napi/n_exporter.h"
#include "webgl_arg.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
struct TexImageArg;
struct TexStorageArg;
class TextureLevelCtrl;
struct TextureLevelInfo;

class WebGLTexture final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLTexture";
    inline static const int objectType = WEBGL_OBJECT_TEXTURE;
    inline static const int DEFAULT_TEXTURE = 0;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLTexture** instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLTexture>(env, instance);
    }

    void SetTexture(uint32_t texture)
    {
        texture_ = texture;
    }

    uint32_t GetTexture() const
    {
        return texture_;
    }

    bool SetTarget(GLenum target);
    GLenum GetTarget()
    {
        return target_;
    }

    explicit WebGLTexture() : texture_(0) {};

    WebGLTexture(napi_env env, napi_value exports) : NExporter(env, exports), texture_(0) {};

    ~WebGLTexture();

    static WebGLTexture* GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLTexture>(env, obj);
    }

    bool SetTextureLevel(const TexImageArg& arg);
    bool SetTexStorageInfo(const TexStorageArg* arg);

    GLenum GetInternalFormat(GLenum target, GLint level) const;
    GLenum GetType(GLenum target, GLint level) const;
    GLsizei GetWidth(GLenum target, GLint level) const;
    GLsizei GetHeight(GLenum target, GLint level) const;
    GLsizei GetDepth(GLenum target, GLint level) const;
    bool CheckImmutable() const
    {
        return immutable_;
    }
    bool CheckValid(GLenum target, GLint level) const;

    static int64_t ComputeTextureLevel(int64_t width, int64_t height, int64_t depth);
    static GLint GetMaxTextureLevelForTarget(GLenum target, bool highWebGL);
    static GLenum GetTypeFromInternalFormat(GLenum internalFormat);
    // validateTexFuncFormatAndType
    static bool CheckTextureFormatAndType(GLenum internalFormat, GLenum format, GLenum type, GLint level, bool isHigh)
    {
        return true;
    }
    static bool CheckNPOT(GLsizei width, GLsizei height)
    {
        if (!width || !height)
            return false;
        if ((width & (width - 1)) || (height & (height - 1)))
            return true;
        return false;
    }
    static bool CheckTextureSize(GLsizei offset, GLsizei w, GLsizei real);
    static const std::vector<GLenum> &GetSupportInternalFormatGroup1();
    static const std::vector<GLenum> &GetSupportInternalFormatGroup2();
private:
    const TextureLevelInfo* GetTextureLevel(GLenum target, GLint level) const;
    GLenum target_ { 0 };
    uint32_t texture_;
    bool immutable_ { false };
    std::vector<TextureLevelCtrl> textureLevelInfos_ {};
};

class TextureLevelCtrl {
public:
    explicit TextureLevelCtrl() {}
    ~TextureLevelCtrl()
    {
        textureInfos_.clear();
    }

    void Init(GLenum target);
    const TextureLevelInfo* GetTextureLevel(GLint level) const
    {
        if (textureInfos_.size() <= static_cast<size_t>(level)) {
            return nullptr;
        }
        return &textureInfos_[level];
    }

private:
    std::vector<TextureLevelInfo> textureInfos_ = {};
};

struct TextureLevelInfo {
    bool valid { false };
    GLenum internalFormat { 0 };
    GLsizei width { 0 };
    GLsizei height { 0 };
    GLsizei depth { 0 };
    GLenum type { 0 };
};

struct TexImageArg {
    int func;
    GLenum target;
    GLint level;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    GLsizei width;
    GLsizei height;
    GLsizei border;
    GLsizei depth;
    TexImageArg()
    : func(0), target(0), level(0), internalFormat(0), format(0), type(0), width(0), height(0), border(0), depth(0) {}
    TexImageArg(const TexImageArg& arg)
    {
        func = arg.func;
        target = arg.target;
        level = arg.level;
        internalFormat = arg.internalFormat;
        format = arg.format;
        type = arg.type;
        width = arg.width;
        height = arg.height;
        border = arg.border;
        depth = arg.depth;
    }
    TexImageArg(
        GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum type)
    {
        this->target = target;
        this->level = level;
        this->internalFormat = internalFormat;
        this->type = type;
        this->width = width;
        this->height = height;
        this->depth = depth;
    }
    void Dump(const std::string& info) const;
};

struct TexStorageArg {
    int func;
    GLenum target;
    GLsizei levels;
    GLenum internalFormat;
    GLsizei width;
    GLsizei height;
    GLsizei depth;
    TexStorageArg() : func(0), target(0), levels(0), internalFormat(0), width(0), height(0), depth(0) {}
    TexStorageArg(const TexStorageArg& arg)
    {
        func = arg.func;
        target = arg.target;
        levels = arg.levels;
        internalFormat = arg.internalFormat;
        width = arg.width;
        height = arg.height;
        depth = arg.depth;
    }
    void Dump(const std::string& info) const;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_TEXTRUE