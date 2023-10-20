/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASE_IMPL
#define ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASE_IMPL

#include <GLES2/gl2.h>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include "securec.h"
#include "napi/n_exporter.h"
#include "webgl/webgl_object.h"
#include "webgl/webgl_arg.h"
#include "webgl/webgl_shader.h"
#include "webgl/webgl_buffer.h"
#include "webgl/webgl_framebuffer.h"
#include "webgl/webgl_program.h"
#include "webgl/webgl_renderbuffer.h"
#include "webgl/webgl_texture.h"
#include "webgl/webgl_uniform_location.h"
#include "webgl/webgl_active_info.h"
#include "webgl/webgl_shader_precision_format.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
#define IMGE_2D_COMM_ARG \
    int func; \
    GLenum target; \
    GLint level; \
    GLint internalFormat; \
    GLenum format; \
    GLenum type; \
    GLint xoffset; \
    GLint yoffset

struct Image2DCommArg {
    IMGE_2D_COMM_ARG;
};
using Image2DFunc = enum : int {
    IMAGE_2D_FUNC_TEXT_IMAGE_2D,
    IMAGE_2D_FUNC_SUB_IMAGE_2D,
    IMAGE_2D_FUNC_COMPRESSED_TEX_IMAGE_2D,
    IMAGE_2D_FUNC_COMPRESSED_TEX_SUB_IMAGE_2D,
    IMAGE_2D_FUNC_COPY_TEX_IMAGE_2D,
    IMAGE_2D_FUNC_COPY_TEX_SUB_IMAGE_2D,
};

struct Image2DArg {
    IMGE_2D_COMM_ARG;
    GLsizei width;
    GLsizei height;
    GLsizei border;
    GLuint srcOffset;
    GLint x;
    GLint y;
};

struct VertexAttribArg {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    GLintptr offset;
};

struct PixelsArg {
    GLint x;
    GLint y;
    GLsizei width;
    GLsizei height;
    GLenum format;
    GLenum type;
};

struct UniformExtInfo {
    uint32_t dimension;
    GLuint srcOffset;
    GLuint srcLength;
};

class WebGLRenderingContextBaseImpl  {
public:
    inline static const int MAX_COUNT_ATTACHED_SHADER = 128;
    inline static const GLuint MAX_LOCATION_LENGTH = 512;
    static const int PARAMETERS_NUM_1 = 1;
    static const int PARAMETERS_NUM_2 = 2;
    static const int PARAMETERS_NUM_3 = 3;
    static const int PARAMETERS_NUM_4 = 4;

    explicit WebGLRenderingContextBaseImpl(int32_t version) : version_(version) {}
    virtual ~WebGLRenderingContextBaseImpl();

    void Init();

    // common interface
    bool IsHighWebGL()
    {
        return version_ > 1;
    }
    void SetError(GLenum error)
    {
        lastError_ = error;
    }

    // webgl  Texture
    napi_value CreateTexture(napi_env env);
    napi_value ActiveTexture(napi_env env, GLenum texture);
    napi_value BindTexture(napi_env env, GLenum target, napi_value texture);

    napi_value TexImage2D(napi_env env, const Image2DArg *info, GLintptr pbOffset);
    napi_value TexImage2D(napi_env env, const Image2DArg *info, napi_value pixels, GLuint srcOffset);
    napi_value TexImage2D(napi_env env, const Image2DArg *info, napi_value imageData);

    napi_value TexSubImage2D(napi_env env, const Image2DArg *info, GLintptr pbOffset);
    napi_value TexSubImage2D(napi_env env, const Image2DArg *imgArg, napi_value pixels, GLuint srcOffset);
    napi_value TexSubImage2D(napi_env env, const Image2DArg *info, napi_value imageData);
    napi_value CompressedTexImage2D(napi_env env, const Image2DArg *arg, GLsizei imageSize, GLintptr offset);
    napi_value CompressedTexImage2D(napi_env env, const Image2DArg *arg, napi_value srcData,
        GLuint srcOffset, GLuint srcLengthOverride);
    napi_value CompressedTexSubImage2D(napi_env env, const Image2DArg *imgArg, napi_value srcData,
        GLuint srcOffset, GLuint srcLengthOverride);
    napi_value CompressedTexSubImage2D(napi_env env, const Image2DArg *imgArg, GLsizei imageSize, GLintptr offset);

    napi_value CopyTexImage2D(napi_env env, const Image2DArg *imgArg);
    napi_value CopyTexSubImage2D(napi_env env, const Image2DArg *imgArg);

    napi_value ReadPixels(napi_env env, const PixelsArg *arg, GLintptr offset);
    napi_value ReadPixels(napi_env env, const PixelsArg *arg, napi_value buffer, GLuint dstOffset);

    // for buffer
    napi_value CreateBuffer(napi_env env);
    napi_value BindBuffer(napi_env env, GLenum target, napi_value object);
    napi_value CreateFrameBuffer(napi_env env);
    napi_value BindFrameBuffer(napi_env env, GLenum target, napi_value object);
    napi_value CreateRenderBuffer(napi_env env);
    napi_value BindRenderBuffer(napi_env env, GLenum target, napi_value object);
    napi_value DeleteBuffer(napi_env env, napi_value object);
    napi_value DeleteFrameBuffer(napi_env env, napi_value object);
    napi_value DeleteShader(napi_env env, napi_value object);
    napi_value DeleteRenderBuffer(napi_env env, napi_value object);
    napi_value DeleteTexture(napi_env env, napi_value object);

    napi_value CreateShader(napi_env env, GLenum type);
    napi_value GetAttachedShaders(napi_env env, napi_value program);

    napi_value CreateProgram(napi_env env);
    napi_value DeleteProgram(napi_env env, napi_value object);
    napi_value GetProgramParameter(napi_env env, napi_value object, GLenum pname);
    napi_value UseProgram(napi_env env, napi_value object);

    napi_value GetUniformLocation(napi_env env, napi_value object, const char *name);

    napi_value GetVertexAttrib(napi_env env, GLenum pname, GLuint index);

    napi_value GetFrameBufferAttachmentParameter(napi_env env, GLenum target, GLenum attachment, GLenum pname);
    napi_value GetParameter(napi_env env, GLenum pname);

    napi_value FrameBufferRenderBuffer(napi_env env,
        GLenum target, GLenum attachment, GLenum renderBufferTarget, napi_value object);
    napi_value FrameBufferTexture2D(napi_env env,
        GLenum target, GLenum attachment, GLenum textarget, napi_value texture, GLint level);

    napi_value DrawElements(napi_env env, GLenum mode, GLsizei count, GLenum type, GLintptr offset);
    napi_value DrawArrays(napi_env env, GLenum mode, GLint first, GLsizei count);

    // webgl 1
    napi_value BufferData(napi_env env, GLenum target, GLsizeiptr size, GLenum usage);
    // webgl 2
    napi_value BufferData(napi_env env, GLenum target, napi_value buffer,
        GLenum usage, GLuint srcOffset, GLuint length);

    napi_value BufferSubData(napi_env env, GLenum target, GLintptr offset, napi_value buffer,
        GLuint srcOffset, GLuint length);

    napi_value PixelStorei(napi_env env, GLenum pname, GLint param);
    napi_value RenderBufferStorage(napi_env env,
        GLenum target, GLenum internalFormat, GLsizei width, GLsizei height);

    // Vertex Attrib
    napi_value VertexAttribPointer(napi_env env, const VertexAttribArg *vertexInfo);
    napi_value EnableVertexAttribArray(napi_env env, int64_t index);
    napi_value DisableVertexAttribArray(napi_env env, int64_t index);
    napi_value GetVertexAttribOffset(napi_env env, GLuint index, GLenum pname);

    napi_value IsEnabled(napi_env env, GLenum cap);
    napi_value Disable(napi_env env, GLenum cap);
    napi_value Enable(napi_env env, GLenum cap);

    napi_value ClearColor(napi_env env, GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    napi_value Clear(napi_env env, GLbitfield mask);

    napi_value BindAttribLocation(napi_env env, napi_value program, GLuint index, const std::string &name);

    napi_value GenerateMipmap(napi_env env, GLenum target);

    napi_value UniformXf(napi_env env, napi_value location, uint32_t count, const GLfloat *data);
    napi_value UniformXi(napi_env env, napi_value location, uint32_t count, const GLint *data);
    napi_value Uniformfv(napi_env env, napi_value locationObj, napi_value data, const UniformExtInfo *info);
    napi_value Uniformiv(napi_env env, napi_value locationObj, napi_value data, const UniformExtInfo *info);
    napi_value UniformMatrixfv(napi_env env, napi_value locationObj, napi_value data,
        GLboolean transpose, const UniformExtInfo *info);

    napi_value GetError(napi_env env);

    // static
    static GLuint GetMaxVertexAttribs();
    static bool CheckString(const std::string& str);
    static bool CheckReservedPrefix(const std::string& name);

    // set bit ptr
    void SetBitMapPtr(char *bitMapPtr, int bitMapWidth, int bitMapHeight)
    {
        bitMapPtr_ = bitMapPtr;
        mitMapWidth_ = bitMapWidth;
        mitMapHeight_ = bitMapHeight;
    }
private:
    // napi_env env_;
    uint32_t version_;
    WebGLRenderingContextBaseImpl(const WebGLRenderingContextBaseImpl&) = delete;
    WebGLRenderingContextBaseImpl& operator=(const WebGLRenderingContextBaseImpl&) = delete;

    // error process
    GLenum lastError_ = 0;

    // for texture
    GLint maxTextureImageUnits = 0;
    GLint maxTextureSize = 0;
    GLint maxCubeMapTextureSize = 0;
    GLint maxRenderBufferSize = 0;
    GLint maxTextureLevel = 0;
    GLint maxCubeMapTextureLevel = 0;
    uint32_t activeTextureIndex = 0;

    // for webgl 1.x
    std::vector<uint32_t> boundTexture2D;
    std::vector<uint32_t> boundTextureCubeMap;

    // for buffer
    GLuint boundElementBufferId = 0;
    GLuint boundBufferId = 0;
    GLuint boundFrameBufferId = 0;
    GLuint boundRenderBufferId = 0;
    GLuint currentProgramId = 0;

    bool unpackFlipY = false;
    bool unpackPremultiplyAlpha = false;
    GLenum unpackColorspaceConversion = 0;

    char *bitMapPtr_ = nullptr;
    int mitMapWidth_ = 0;
    int mitMapHeight_ = 0;

    // object mananger
    template<class T>
    bool AddObject(napi_env env, uint64_t key, napi_value obj);
    template<class T>
    napi_value GetNapiValue(napi_env env, uint64_t key);
    template<class T>
    napi_value GetObject(napi_env env, uint64_t key);
    template<class T>
    void DeleteObject(napi_env env, uint64_t key);
    template<class T>
    T *GetObjectInstance(napi_env env, uint64_t id);

    std::map<uint64_t, napi_ref> objects[WebGLObject::WEBGL_OBJECT_MAX] = {};

    // private interface
    WebGLTexture* GetBoundTexture(napi_env env, GLenum target, bool cubeMapExt);
    WebGLFramebuffer *GetBoundFrameBuffer(napi_env env, GLenum target);

    GLenum  GetError_();
    napi_value BufferData_(napi_env env,
        GLenum target, GLsizeiptr size, GLenum usage, WebGLReadBufferArg *bufferData);
    WebGLBuffer *CheckAndGetBoundBuffer(napi_env env, GLenum target);

    // check
    bool CheckInternalFormat(napi_env env, GLenum internalFormat);
    bool CheckBufferDataUsage(napi_env env, GLenum usage);
    bool CheckDrawMode(napi_env env, GLenum mode);
    bool CheckAttachment(napi_env env, GLenum attachment);
    bool CheckRenderBufferTarget(napi_env env, GLenum target);
    bool CheckTexture2DTarget(napi_env env, GLenum target);
    bool CheckCap(napi_env env, GLenum cap);
    bool CheckPixelsFormat(napi_env env, GLenum format);
    bool CheckPixelsType(napi_env env, GLenum type);
    bool CheckTextureTargetAndLevel(GLenum target, GLint level);
    GLenum CheckTexImage2D(napi_env env, const Image2DArg *imgArg);
    GLint GetMaxTextureLevelForTarget(GLenum target);

    napi_value GetFloatParameter(napi_env env, GLenum pname);
    napi_value GetTextureParameter(napi_env env, GLenum pname);
    napi_value GetBufferParameter(napi_env env, GLenum pname);
    napi_value GetFramebufferParameter(napi_env env, GLenum pname);
    napi_value GetProgramParameter(napi_env env, GLenum pname);
    napi_value GetRenderbufferParameter(napi_env env, GLenum pname);
    napi_value GetAliasedrParameter(napi_env env, GLenum pname);
    napi_value GetColorParameter(napi_env env, GLenum pname);
    napi_value GetCompressedTextureFormatsParameter(napi_env env, GLenum pname);
    napi_value GetMaxViewportDimsParameter(napi_env env, GLenum pname);
    napi_value GetScissorBoxParameter(napi_env env, GLenum pname);
    napi_value GetBoolParameter(napi_env env, GLenum pname);
    napi_value GetColorWriteMaskParameter(napi_env env, GLenum pname);
    napi_value GetIntegervParameter(napi_env env, GLenum pname);

public:
    bool CheckFrameBufferTarget(napi_env env, GLenum target);
    bool CheckGLenum(napi_env env, GLenum type,
        const std::vector<GLenum> &glSupport, const std::vector<GLenum> &g2Support);
};
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASE_IMPL