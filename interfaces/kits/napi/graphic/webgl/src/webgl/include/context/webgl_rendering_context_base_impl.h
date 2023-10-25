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
#include <GLES2/gl2ext.h>
#include <GLES3/gl31.h>
#include <vector>

#include "napi/n_exporter.h"
#include "securec.h"
#include "webgl/webgl_active_info.h"
#include "webgl/webgl_arg.h"
#include "webgl/webgl_buffer.h"
#include "webgl/webgl_framebuffer.h"
#include "webgl/webgl_object.h"
#include "webgl/webgl_program.h"
#include "webgl/webgl_renderbuffer.h"
#include "webgl/webgl_shader.h"
#include "webgl/webgl_shader_precision_format.h"
#include "webgl/webgl_texture.h"
#include "webgl/webgl_uniform_location.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using Image2DFunc = enum : int {
    IMAGE_2D_FUNC_TEXT_IMAGE_2D,
    IMAGE_2D_FUNC_SUB_IMAGE_2D,
    IMAGE_2D_FUNC_COMPRESSED_TEX_IMAGE_2D,
    IMAGE_2D_FUNC_COMPRESSED_TEX_SUB_IMAGE_2D,
    IMAGE_2D_FUNC_COPY_TEX_IMAGE_2D,
    IMAGE_2D_FUNC_COPY_TEX_SUB_IMAGE_2D,
};
enum BoundBufferType : uint32_t {
    ARRAY_BUFFER,
    ELEMENT_ARRAY_BUFFER,
    COPY_READ_BUFFER,
    COPY_WRITE_BUFFER,
    TRANSFORM_FEEDBACK_BUFFER,
    UNIFORM_BUFFER,
    PIXEL_PACK_BUFFER,
    PIXEL_UNPACK_BUFFER,
    BUFFER_MAX
};

enum BoundFrameBufferType : uint32_t {
    FRAMEBUFFER,
    DRAW_FRAMEBUFFER,
    READ_FRAMEBUFFER,
    FRAMEBUFFER_MAX
};

enum BoundRenderBufferType : uint32_t {
    RENDERBUFFER,
    RENDERBUFFER_MAX
};

enum BoundTextureType : uint32_t {
    TEXTURE_2D,
    TEXTURE_CUBE_MAP,
    TEXTURE_3D,
    TEXTURE_2D_ARRAY,
    TEXTURE_MAX
};

enum BoundShaderType : uint32_t {
    VERTEX_SHADER,
    FRAGMENT_SHADER,
    SHADER_MAX
};

struct TexSubImage2DArg : public TexImageArg {
    GLint xOffset;
    GLint yOffset;
    TexSubImage2DArg() : TexImageArg(), xOffset(0), yOffset{0} {}
};

struct CopyTexSubImage2DArg : public TexSubImage2DArg {
    GLint x;
    GLint y;
    CopyTexSubImage2DArg() : TexSubImage2DArg(), x(0), y(0) {}
};

struct CopyTexImage2DArg : public TexImageArg {
    GLint x;
    GLint y;
    CopyTexImage2DArg() : TexImageArg(), x(0), y{0} {}
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
    GLsizei dimension;
    GLuint srcOffset;
    GLuint srcLength;
};

struct UniformTypeMap {
    GLenum type;
    GLenum baseType;
    GLsizei length;
    BufferDataType srcType;
    BufferDataType dstType;
};

// getVertexAttrib
struct VertexAttribDesc {
    bool enabled;
    bool normalized;
    // VERTEX_ATTRIB_ARRAY_BUFFER_BINDING
    GLuint boundBufferId;
    // VERTEX_ATTRIB_ARRAY_NORMALIZED
    GLsizei bytesPerElement;
    // VERTEX_ATTRIB_ARRAY_SIZE
    GLint size;
    // VERTEX_ATTRIB_ARRAY_TYPE
    GLenum type;
    // VERTEX_ATTRIB_ARRAY_STRIDE
    GLsizei stride;
    GLsizei originalStride;
    GLintptr offset;
    // by this func VertexAttribDivisor
    GLuint divisor;
};

struct VertexAttribInfo {
    BufferDataType type;
};

struct BufferPosition {
    GLint x;
    GLint y;
};

struct BufferSize {
    GLsizei width;
    GLsizei height;
};

struct BufferExt {
    GLuint dstOffset;
    GLuint length;
};

struct DrawElementArg {
    GLenum mode;
    GLsizei count;
    GLenum type;
    GLintptr offset;
};

class WebGLRenderingContextBaseImpl {
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
    napi_value DeleteTexture(napi_env env, napi_value object);
    napi_value BindTexture(napi_env env, GLenum target, napi_value texture);
    napi_value TexParameteri(napi_env env, GLenum target, GLenum pname, GLint param);
    napi_value TexParameterf(napi_env env, GLenum target, GLenum pname, GLfloat param);
    napi_value GetTexParameter(napi_env env, GLenum target, GLenum pname);

    napi_value TexImage2D(napi_env env, const TexImageArg* info, GLintptr pbOffset);
    napi_value TexImage2D(napi_env env, const TexImageArg* info, napi_value pixels, GLuint srcOffset);
    napi_value TexImage2D(napi_env env, const TexImageArg* info, napi_value imageData);

    napi_value TexSubImage2D(napi_env env, const TexSubImage2DArg* info, GLintptr pbOffset);
    napi_value TexSubImage2D(napi_env env, const TexSubImage2DArg* imgArg, napi_value pixels, GLuint srcOffset);
    napi_value TexSubImage2D(napi_env env, const TexSubImage2DArg* info, napi_value imageData);
    napi_value CompressedTexImage2D(napi_env env, const TexImageArg* arg, GLsizei imageSize, GLintptr offset);
    napi_value CompressedTexImage2D(
        napi_env env, const TexImageArg* arg, napi_value srcData, GLuint srcOffset, GLuint srcLengthOverride);
    napi_value CompressedTexSubImage2D(napi_env env, const TexSubImage2DArg* imgArg,
        napi_value srcData, GLuint srcOffset, GLuint srcLengthOverride);
    napi_value CompressedTexSubImage2D(napi_env env, const TexSubImage2DArg* imgArg,
        GLsizei imageSize, GLintptr offset);

    napi_value CopyTexImage2D(napi_env env, const CopyTexImage2DArg* imgArg);
    napi_value CopyTexSubImage2D(napi_env env, const CopyTexSubImage2DArg* imgArg);

    napi_value ReadPixels(napi_env env, const PixelsArg* arg, GLintptr offset);
    napi_value ReadPixels(napi_env env, const PixelsArg* arg, napi_value buffer, GLuint dstOffset);

    // for buffer
    napi_value CreateBuffer(napi_env env);
    napi_value BindBuffer(napi_env env, GLenum target, napi_value object);
    napi_value IsBuffer(napi_env env, napi_value object);
    napi_value DeleteBuffer(napi_env env, napi_value object);

    napi_value CreateFrameBuffer(napi_env env);
    napi_value IsFrameBuffer(napi_env env, napi_value object);
    napi_value BindFrameBuffer(napi_env env, GLenum target, napi_value object);
    napi_value DeleteFrameBuffer(napi_env env, napi_value object);
    napi_value GetBufferParameter(napi_env env, GLenum target, GLenum pname);

    napi_value CreateRenderBuffer(napi_env env);
    napi_value BindRenderBuffer(napi_env env, GLenum target, napi_value object);
    napi_value DeleteRenderBuffer(napi_env env, napi_value object);
    napi_value GetRenderBufferParameter(napi_env env, GLenum target, GLenum pname);
    napi_value RenderBufferStorage(napi_env env, const TexStorageArg& arg);

    napi_value CreateShader(napi_env env, GLenum type);
    napi_value GetAttachedShaders(napi_env env, napi_value program);
    napi_value CompileShader(napi_env env, napi_value object);
    napi_value ShaderSource(napi_env env, napi_value object, const std::string& source);
    napi_value AttachShader(napi_env env, napi_value programObj, napi_value shaderObj);
    napi_value DetachShader(napi_env env, napi_value programObj, napi_value shaderObj);
    napi_value DeleteShader(napi_env env, napi_value object);
    napi_value GetShaderParameter(napi_env env, napi_value object, GLenum pname);
    napi_value GetShaderPrecisionFormat(napi_env env, GLenum shaderType, GLenum precisionType);

    napi_value CreateProgram(napi_env env);
    napi_value DeleteProgram(napi_env env, napi_value object);
    napi_value GetProgramParameter(napi_env env, napi_value object, GLenum pname);
    napi_value UseProgram(napi_env env, napi_value object);
    napi_value ValidateProgram(napi_env env, napi_value object);
    napi_value LinkProgram(napi_env env, napi_value object);

    napi_value GetUniformLocation(napi_env env, napi_value object, const std::string& name);
    napi_value GetUniform(napi_env env, napi_value programObj, napi_value uniformObj);

    napi_value GetFrameBufferAttachmentParameter(napi_env env, GLenum target, GLenum attachment, GLenum pname);
    napi_value GetParameter(napi_env env, GLenum pname);
    napi_value CheckFrameBufferStatus(napi_env env, GLenum target);

    napi_value FrameBufferRenderBuffer(
        napi_env env, GLenum target, GLenum attachment, GLenum renderBufferTarget, napi_value object);
    napi_value FrameBufferTexture2D(
        napi_env env, GLenum target, GLenum attachment, GLenum textarget, napi_value texture, GLint level);

    napi_value DrawElements(napi_env env, GLenum mode, GLsizei count, GLenum type, GLintptr offset);
    napi_value DrawArrays(napi_env env, GLenum mode, GLint first, GLsizei count);

    // webgl 1
    napi_value BufferData(napi_env env, GLenum target, GLsizeiptr size, GLenum usage);
    // webgl 2
    napi_value BufferData(
        napi_env env, GLenum target, napi_value buffer, GLenum usage, GLuint srcOffset, GLuint length);

    napi_value BufferSubData(
        napi_env env, GLenum target, GLintptr offset, napi_value buffer, GLuint srcOffset, GLuint length);

    napi_value PixelStorei(napi_env env, GLenum pname, GLint param);

    // Vertex Attrib
    napi_value VertexAttribPointer(napi_env env, const VertexAttribArg* vertexInfo);
    napi_value EnableVertexAttribArray(napi_env env, int64_t index);
    napi_value DisableVertexAttribArray(napi_env env, int64_t index);
    napi_value GetVertexAttribOffset(napi_env env, GLuint index, GLenum pname);
    napi_value VertexAttribfv(napi_env env, GLuint index, int count, napi_value dataObj);
    napi_value VertexAttribf(napi_env env, GLuint index, int count, GLfloat* data);
    napi_value GetAttribLocation(napi_env env, napi_value object, const std::string& name);
    napi_value GetVertexAttrib(napi_env env, GLenum pname, GLuint index);

    napi_value IsEnabled(napi_env env, GLenum cap);
    napi_value Disable(napi_env env, GLenum cap);
    napi_value Enable(napi_env env, GLenum cap);

    napi_value BindAttribLocation(napi_env env, napi_value program, GLuint index, const std::string& name);

    napi_value GenerateMipmap(napi_env env, GLenum target);

    napi_value Uniform_f(napi_env env, napi_value location, uint32_t count, const GLfloat* data);
    napi_value Uniform_i(napi_env env, napi_value location, uint32_t count, const GLint* data);
    napi_value Uniform_ui(napi_env env, napi_value locationObj, uint32_t count, const GLuint* data);

    napi_value Uniform_uiv(napi_env env, napi_value locationObj, napi_value data, const UniformExtInfo* info);
    napi_value Uniform_fv(napi_env env, napi_value locationObj, napi_value data, const UniformExtInfo* info);
    napi_value Uniform_iv(napi_env env, napi_value locationObj, napi_value data, const UniformExtInfo* info);
    napi_value UniformMatrix_fv(
        napi_env env, napi_value locationObj, napi_value data, GLboolean transpose, const UniformExtInfo* info);

    napi_value StencilMaskSeparate(napi_env env, GLenum face, GLuint mask);
    napi_value StencilMask(napi_env env, GLuint mask);
    napi_value StencilFunc(napi_env env, GLenum func, GLint ref, GLuint mask);
    napi_value StencilFuncSeparate(napi_env env, GLenum face, GLenum func, GLint ref, GLuint mask);
    napi_value DepthMask(napi_env env, bool flag);

    napi_value ClearColor(napi_env env, GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    napi_value Clear(napi_env env, GLbitfield mask);
    napi_value ColorMask(napi_env env, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
    napi_value ClearDepth(napi_env env, GLclampf depth);
    napi_value ClearStencil(napi_env env, GLint s);
    napi_value GetError(napi_env env);

    // static
    static bool CheckString(const std::string& str);
    static bool CheckReservedPrefix(const std::string& name);
    static bool GetUniformExtInfo(napi_env env, const NFuncArg& funcArg, Impl::UniformExtInfo* info, int start);

    // set bit ptr
    void SetBitMapPtr(char* bitMapPtr, int bitMapWidth, int bitMapHeight)
    {
        bitMapPtr_ = bitMapPtr;
        mitMapWidth_ = bitMapWidth;
        mitMapHeight_ = bitMapHeight;
    }
    GLuint GetMaxVertexAttribs()
    {
        return maxVertexAttribs;
    }
    bool CheckGLenum(
        napi_env env, GLenum type, const std::vector<GLenum>& glSupport, const std::vector<GLenum>& g2Support);
    GLenum CheckReadPixelsArg(const PixelsArg* arg, uint64_t bufferSize);
    // 需要1.x和2.x实现自己不同的逻辑
protected:
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
    T* GetObjectInstance(napi_env env, uint64_t id);

    // private interface
    WebGLTexture* GetBoundTexture(napi_env env, GLenum target, bool cubeMapExt);
    WebGLFramebuffer* GetBoundFrameBuffer(napi_env env, GLenum target);
    WebGLRenderbuffer* GetBoundRenderBuffer(napi_env env, GLenum target);
    WebGLBuffer* GetBoundBuffer(napi_env env, GLenum target);
    GLenum GetError_();
    bool CheckInList(napi_env env, GLenum type, const std::vector<GLenum>& glSupport);
    WebGLRenderbuffer* CheckRenderBufferStorage(napi_env env, const TexStorageArg& arg);
    bool CheckVertexAttribPointer(napi_env env, const VertexAttribArg* vertexInfo);

    VertexAttribInfo* GetVertexAttribInfo(GLint index)
    {
        if (index >= static_cast<GLint>(arrayVertexAttribs.size())) {
            return nullptr;
        }
        return &arrayVertexAttribs[index];
    }
    void TexImage2D_(const TexImageArg* imgArg, WebGLTexture *texture, const void* pixels, bool changeUnpackAlignment);
    void TexSubImage2D_(const TexSubImage2DArg* imgArg,
        WebGLTexture *texture, const void* pixels, bool changeUnpackAlignment);

    // TODO 需要补充逻辑
    bool CheckFrameBufferBoundComplete(napi_env env);

    WebGLRenderingContextBaseImpl(const WebGLRenderingContextBaseImpl&) = delete;
    WebGLRenderingContextBaseImpl& operator=(const WebGLRenderingContextBaseImpl&) = delete;

    napi_value BufferData_(napi_env env, GLenum target, GLsizeiptr size, GLenum usage, WebGLReadBufferArg* bufferData);
    WebGLBuffer* CheckAndGetBoundBuffer(napi_env env, GLenum target);
    // check
    bool CheckBufferTarget(napi_env env, GLenum target, uint32_t& index);
    bool CheckFrameBufferTarget(napi_env env, GLenum target, uint32_t& index);
    bool CheckRenderBufferTarget(napi_env env, GLenum target, uint32_t& index);
    bool CheckTextureTarget(napi_env env, GLenum target, uint32_t& index);
    bool CheckShaderType(napi_env env, GLenum type, uint32_t& index);
    bool CheckInternalFormat(napi_env env, GLenum internalFormat);
    bool CheckBufferDataUsage(napi_env env, GLenum usage);
    bool CheckDrawMode(napi_env env, GLenum mode);
    bool CheckAttachment(napi_env env, GLenum attachment);
    bool CheckTexture2DTarget(napi_env env, GLenum target);
    bool CheckCap(napi_env env, GLenum cap);
    bool CheckPixelsFormat(napi_env env, GLenum format);
    bool CheckPixelsType(napi_env env, GLenum type);
    bool CheckTextureLevel(GLenum target, GLint level);
    std::tuple<GLenum, WebGLTexture *> CheckTextureInfo(napi_env env, const TexImageArg* imgArg);
    GLenum CheckTexSubImage2D(napi_env env, const TexSubImage2DArg* textureInfo, WebGLTexture* texture);
    GLenum GetValidInternalFormat(GLenum internalFormat, GLenum type);

    GLenum CheckTexFuncDimensions(const TexImageArg* textureInfo);
    GLenum CheckCompressedTexDimensions(const TexImageArg* textureInfo, size_t dataLen);
    bool CheckTexImageInternalFormat(int func, GLenum internalFormat);
    bool CheckSettableTexFormat(GLenum format);
    bool CheckCompressedTexImage2D(
        napi_env env, const TexImageArg* textureInfo, size_t imageSize);

    bool CheckStencil(napi_env env);
    bool CheckLocationName(const std::string& name);

    template<class T>
    GLenum CheckTexParameter(napi_env env, GLenum target, GLenum pname, T param, bool isFloat);
    bool CheckProgramLinkStatus(WebGLProgram* program);
    const UniformTypeMap* GetUniformTypeMap(GLenum type);
    bool GetUniformType(napi_env env, GLuint programId, GLint locationId, GLenum& type);

    // napi_env env_;
    uint32_t version_;
    // error process
    GLenum lastError_ = 0;

    // for texture
    GLint maxTextureImageUnits = 0;
    GLint maxTextureSize = 0;
    GLint maxCubeMapTextureSize = 0;
    GLint maxRenderBufferSize = 0;
    uint32_t activeTextureIndex = 0;

    std::vector<uint32_t> boundTexture[BoundTextureType::TEXTURE_MAX] = {};

    // for buffer 0: ARRAY_BUFFER 1:ELEMENT_ARRAY_BUFFER
    GLuint boundBufferIds[BoundBufferType::BUFFER_MAX] = { 0 };
    GLuint boundFrameBufferIds[BoundFrameBufferType::FRAMEBUFFER_MAX] = { 0 };
    GLuint boundRenderBufferIds[BoundRenderBufferType::RENDERBUFFER_MAX] = { 0 };

    GLuint currentProgramId { 0 };

    GLuint maxVertexAttribs { 0 };
    std::vector<VertexAttribInfo> arrayVertexAttribs {};

    bool unpackFlipY_ { false };
    bool unpackPremultiplyAlpha_ { false };
    GLenum unpackColorspaceConversion { 0 };
    GLint packAlignment_ { 4 };
    GLint unpackAlignment_ { 4 };

    bool stencilEnabled { false };
    bool scissorEnabled { false };
    // for FRONT and BACK
    GLuint stencilMask[2] { 0 };
    GLint stencilFuncRef[2] { 0 };
    GLuint stencilFuncMask[2] { 0 };

    // for color
    GLfloat clearColor[4] { 0 };
    GLboolean colorMask[4] { false };
    GLint clearStencil { 0 };
    GLfloat clearDepth { 0 };
    bool depthMask { false };

    // for Vertex Attrib
    char* bitMapPtr_ = nullptr;
    int mitMapWidth_ = 0;
    int mitMapHeight_ = 0;

    std::map<uint64_t, napi_ref> objects[WebGLObject::WEBGL_OBJECT_MAX] = {};
};

template<class T>
bool WebGLRenderingContextBaseImpl::AddObject(napi_env env, uint64_t key, napi_value obj)
{
    if (T::objectType > WebGLObject::WEBGL_OBJECT_MAX) {
        return false;
    }
    if (objects[T::objectType].find(key) != objects[T::objectType].end()) {
        LOGE("AddObject exit %{public}u %{public}u", T::objectType, static_cast<uint32_t>(key));
        return false;
    }
    napi_ref ref;
    napi_status status = napi_create_reference(env, obj, 1, &ref);
    if (status != napi_ok) {
        LOGE("AddObject %{public}u status %{public}u", static_cast<uint32_t>(key), status);
        return false;
    }
    LOGI("AddObject %{public}u %{public}p %{public}u ", T::objectType, obj, static_cast<uint32_t>(key));
    objects[T::objectType].insert({ key, ref });
    return true;
}

template<class T>
napi_value WebGLRenderingContextBaseImpl::GetNapiValue(napi_env env, uint64_t key)
{
    if (T::objectType > WebGLObject::WEBGL_OBJECT_MAX) {
        return nullptr;
    }
    auto it = objects[T::objectType].find(key);
    if (it == objects[T::objectType].end()) {
        LOGI("GetObject %{public}u %{public}u", T::objectType, static_cast<uint32_t>(key));
        return nullptr;
    }
    napi_value obj;
    napi_status status = napi_get_reference_value(env, it->second, &obj);
    LOGI("GetNapiValue %{public}u %{public}p %{public}u ", T::objectType, obj, static_cast<uint32_t>(key));
    if (status != napi_ok) {
        return nullptr;
    }
    return obj;
}

template<class T>
napi_value WebGLRenderingContextBaseImpl::GetObject(napi_env env, uint64_t key)
{
    napi_value retNull = nullptr;
    napi_get_null(env, &retNull);
    napi_value obj = GetNapiValue<T>(env, key);
    if (obj == nullptr) {
        return retNull;
    }
    return obj;
}

template<class T>
void WebGLRenderingContextBaseImpl::DeleteObject(napi_env env, uint64_t key)
{
    if (T::objectType > WebGLObject::WEBGL_OBJECT_MAX) {
        return;
    }
    auto it = objects[T::objectType].find(key);
    if (it == objects[T::objectType].end()) {
        LOGE("WebGL can not delete %{public}u %{public}u", T::objectType, static_cast<uint32_t>(key));
        return;
    }
    objects[T::objectType].erase(it);
    napi_delete_reference(env, it->second);
}

template<class T>
T* WebGLRenderingContextBaseImpl::GetObjectInstance(napi_env env, uint64_t id)
{
    napi_value tmp = GetNapiValue<T>(env, id);
    return NClass::GetEntityOf<T>(env, tmp);
}

} // namespace Impl
} // namespace Rosen
} // namespace OHOS
#endif // ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASE_IMPL