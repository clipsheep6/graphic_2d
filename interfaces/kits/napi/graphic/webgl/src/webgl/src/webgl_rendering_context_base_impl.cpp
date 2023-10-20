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

#include "context/webgl_rendering_context_basic_base.h"
#include "context/webgl_rendering_context_base.h"
#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_context_attributes.h"
#include "context/webgl_rendering_context.h"
#include "napi/n_func_arg.h"
#include "napi/n_class.h"
#include "util/log.h"
#include "util/egl_manager.h"
#include "util/object_source.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;

#define SET_ERROR(error, ...) \
do { \
    LOGE(__VA_ARGS__); \
    SetError(error); \
} while (0)

WebGLRenderingContextBaseImpl::~WebGLRenderingContextBaseImpl()
{
}

static int64_t ComputeTextureLevel(int64_t width, int64_t height, int64_t depth)
{
    int64_t max = std::max(std::max(width, height), depth);
    if (max <= 0) {
        return 0;
    }

    double result = log2(max);
    return ceil(result);
}

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
    objects[T::objectType].insert({key, ref});
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
T *WebGLRenderingContextBaseImpl::GetObjectInstance(napi_env env, uint64_t id)
{
    napi_value tmp = GetNapiValue<T>(env, id);
    return NClass::GetEntityOf<T>(env, tmp);
}

void WebGLRenderingContextBaseImpl::Init()
{
    if (maxTextureImageUnits != 0) {
        return;
    }
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureImageUnits);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    boundTexture2D.resize(maxTextureSize);

    maxTextureLevel = ComputeTextureLevel(maxTextureSize, maxTextureSize, 1);
    maxCubeMapTextureSize = 0;
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapTextureSize);
    maxCubeMapTextureLevel = ComputeTextureLevel(maxCubeMapTextureSize, maxCubeMapTextureSize, 1);
    boundTextureCubeMap.resize(maxCubeMapTextureSize);

    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);

    unpackColorspaceConversion = WebGLRenderingContextBase::BROWSER_DEFAULT_WEBGL;

    LOGI("WebGL Init maxTextureImageUnits %{public}u", maxTextureImageUnits);
    LOGI("WebGL Init maxTextureSize %{public}u", maxTextureSize);
    LOGI("WebGL Init maxTextureLevel %{public}u", maxTextureLevel);
    LOGI("WebGL Init maxCubeMapTextureSize %{public}u", maxCubeMapTextureSize);
    LOGI("WebGL Init maxCubeMapTextureLevel %{public}u", maxCubeMapTextureLevel);
    LOGI("WebGL Init maxRenderBufferSize %{public}u", maxRenderBufferSize);
}

napi_value WebGLRenderingContextBaseImpl::CreateTexture(napi_env env)
{
    WebGLTexture *webGlTexture = nullptr;
    napi_value objTexture = WebGLTexture::CreateObjectInstance(env, &webGlTexture).val_;
    if (!webGlTexture) {
        return NVal::CreateNull(env).val_;
    }
    GLuint textureId;
    glGenTextures(1, &textureId);
    webGlTexture->SetTexture(textureId);
    LOGI("WebGL createTexture textureId %{public}u", textureId);
    AddObject<WebGLTexture>(env, textureId, objTexture);
    return objTexture;
}

napi_value WebGLRenderingContextBaseImpl::ActiveTexture(napi_env env, GLenum texture)
{
    if (texture < GL_TEXTURE0 || static_cast<GLint>(texture - GL_TEXTURE0) >= maxTextureImageUnits) {
        SET_ERROR(GL_INVALID_ENUM, "activeTexture texture unit out of range");
        return nullptr;
    }
    activeTextureIndex = texture - GL_TEXTURE0;
    LOGI("WebGL activeTexture texture %{public}u", texture);
    glActiveTexture(texture);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BindTexture(napi_env env, GLenum target, napi_value textureRef)
{
    GLuint texture = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture *webGlTexture = WebGLTexture::GetObjectInstance(env, textureRef);
    if (webGlTexture != nullptr) {
        texture = webGlTexture->GetTexture();
    }
    LOGI("WebGL bindTexture target %{public}u textureId %{public}u", target, texture);
    LOGI("WebGL bindTexture target %{public}u textureId %{public}u", target, texture);
    switch (target) {
        case WebGLRenderingContextBase::TEXTURE_2D:
            boundTexture2D[activeTextureIndex] = texture;
            break;
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP:
            boundTextureCubeMap[activeTextureIndex] = texture;
            break;
        case WebGL2RenderingContextBase::TEXTURE_3D:
        case WebGL2RenderingContextBase::TEXTURE_2D_ARRAY:
            if (IsHighWebGL()) {
                break;
            }
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM, "bindTexture Invalid target");
            return nullptr;
    }

    glBindTexture(target, texture);
    if (webGlTexture) {
        webGlTexture->SetTarget(target);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const Image2DArg *imgArg,
    napi_value pixels, GLuint srcOffset)
{
    LOGE("WebGL texImage2D target [%{public}u %{public}d internalFormat %{public}u format %{public}u type %{public}u]",
        imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->format, imgArg->type);

    WebGLReadBufferArg bufferData(env);
    GLvoid *data = nullptr;
    if (!NVal(env, pixels).IsNull()) {
        napi_status status = bufferData.GenBufferData(pixels);
        if (status != napi_ok) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texImage2D get buffer fail");
            return nullptr;
        }
        data = reinterpret_cast<void*>(bufferData.GetBuffer());
    }
    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texImage2D error %{public}u", error);
        SetError(error);
    }

    glTexImage2D(imgArg->target, imgArg->level, imgArg->internalFormat,
        imgArg->width, imgArg->height, imgArg->border,
        imgArg->format, imgArg->type, data);
    LOGE("WebGL texImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const Image2DArg *info, napi_value source)
{
    Image2DArg *imgArg = const_cast<Image2DArg *>(info);
    LOGE("WebGL texImage2D target [%{public}u %{public}u %{public}u %{public}u %{public}u]",
        imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->format, imgArg->type);

    GLvoid *data = nullptr;
    WebGLImageSource imageSource;
    if (!NVal(env, source).IsNull()) {
        bool succ = imageSource.GenImageSource(env, source);
        if (!succ) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texImage2D Image source invalid");
            return nullptr;
        }
        imgArg->width = imageSource.GetWidth();
        imgArg->height = imageSource.GetHeight();
        data = reinterpret_cast<void*>(imageSource.GetImageSourceData(unpackFlipY));
    }

    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texImage2D error %{public}u", error);
        SetError(error);
    }

    glTexImage2D(imgArg->target, imgArg->level, imgArg->internalFormat,
        imgArg->width, imgArg->height, imgArg->border,
        imgArg->format, imgArg->type, data);
    LOGE("WebGL texImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const Image2DArg *imgArg, GLintptr pbOffset)
{
    LOGE("WebGL texImage2D target [%{public}u %{public}u %{public}u %{public}u]",
        imgArg->target, imgArg->level, imgArg->format, imgArg->type);
    LOGE("WebGL texImage2D offset [%{public}d %{public}d %{public}d %{public}d]",
        imgArg->xoffset, imgArg->yoffset, imgArg->width, imgArg->height);
    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texImage2D error %{public}u", error);
        SetError(error);
    }
    LOGE("WebGL texImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(napi_env env, const Image2DArg *imgArg, GLintptr pbOffset)
{
    LOGE("WebGL texSubImage2D target [%{public}u %{public}u %{public}u %{public}u]",
        imgArg->target, imgArg->level, imgArg->format, imgArg->type);
    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SetError(error);
    }
    LOGE("WebGL texSubImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(napi_env env, const Image2DArg *imgArg,
    napi_value pixels, GLuint srcOffset)
{
    LOGE("WebGL texSubImage2D target [%{public}u %{public}u %{public}u %{public}u]",
        imgArg->target, imgArg->level, imgArg->format, imgArg->type);

    WebGLReadBufferArg bufferData(env);
    GLvoid *data = nullptr;
    if (!NVal(env, pixels).IsNull()) {
        napi_status status = bufferData.GenBufferData(pixels);
        if (status != napi_ok) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texSubImage2D get buffer fail");
            return nullptr;
        }
        data = reinterpret_cast<void*>(bufferData.GetBuffer());
    }

    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SetError(error);
    }

    glTexSubImage2D(imgArg->target, imgArg->level, imgArg->xoffset, imgArg->yoffset,
        imgArg->width, imgArg->height, imgArg->format, imgArg->type, data);
    LOGE("WebGL texSubImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}
napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(napi_env env, const Image2DArg *info, napi_value imageData)
{
    Image2DArg *imgArg = const_cast<Image2DArg *>(info);
    LOGE("WebGL texSubImage2D target [%{public}u %{public}u %{public}u %{public}u]",
        imgArg->target, imgArg->level, imgArg->format, imgArg->type);

    GLvoid *data = nullptr;
    WebGLImageSource imageSource;
    if (!NVal(env, imageData).IsNull()) {
        bool succ = imageSource.GenImageSource(env, imageData);
        if (!succ) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texSubImage2D Image source invalid");
            return nullptr;
        }
        imgArg->width = imageSource.GetWidth();
        imgArg->height = imageSource.GetHeight();
        data = reinterpret_cast<void*>(imageSource.GetImageSourceData(unpackFlipY));
    }

    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SetError(error);
    }

    glTexSubImage2D(imgArg->target, imgArg->level, imgArg->xoffset, imgArg->yoffset,
        imgArg->width, imgArg->height, imgArg->format, imgArg->type, data);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BindBuffer(napi_env env, GLenum target, napi_value object)
{
    LOGI("WebGL bindBuffer target %{public}u ", target);
    // support default value
    uint32_t bufferId = WebGLBuffer::DEFAULT_BUFFER;
    WebGLBuffer *webGlBuffer = WebGLBuffer::GetObjectInstance(env, object);
    if (webGlBuffer != nullptr && !webGlBuffer->HasBound()) {
        LOGE("WebGL bindBuffer webGlBuffer %{public}p", webGlBuffer);
        bufferId = webGlBuffer->GetBufferId();
    }

    LOGI("WebGL bindBuffer target %{public}u bufferId %{public}u", target, bufferId);
    glBindBuffer(target, static_cast<GLuint>(bufferId));
    switch (target) {
        case WebGLRenderingContextBase::ARRAY_BUFFER:
            boundBufferId = bufferId;
            break;
        case WebGLRenderingContextBase::ELEMENT_ARRAY_BUFFER:
            boundElementBufferId = bufferId;
            break;
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM, "BindBuffer Invalid target");
            return nullptr;
    }
    // save bufferId
    if (webGlBuffer) {
        webGlBuffer->SetTarget(target);
        webGlBuffer->SetBound(true);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BindFrameBuffer(napi_env env, GLenum target, napi_value object)
{
    if (!CheckFrameBufferTarget(env, target)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM, "BindFrameBuffer Invalid target");
        return nullptr;
    }
    LOGI("WebGL bindFramebuffer target %{public}u ", target);

    GLuint frameBufferId = WebGLFramebuffer::DEFAULT_FRAME_BUFFER;
    WebGLFramebuffer *webGlFramebuffer = WebGLFramebuffer::GetObjectInstance(env, object);
    if (webGlFramebuffer != nullptr && !webGlFramebuffer->HasBound()) {
        frameBufferId = webGlFramebuffer->GetFramebuffer();
    }
    LOGI("WebGL bindFramebuffer framebuffer %{public}u", frameBufferId);
    glBindFramebuffer(target, frameBufferId);

    boundFrameBufferId = frameBufferId;
    if (webGlFramebuffer) {
        webGlFramebuffer->SetBound(true);
        webGlFramebuffer->SetTarget(target);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BindRenderBuffer(napi_env env, GLenum target, napi_value object)
{
    LOGI("WebGL bindRenderbuffer target %{public}u", target);
    if (!CheckGLenum(env, target, {
        WebGLRenderingContextBase::RENDERBUFFER
    }, {})) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM, "BindRenderBuffer Invalid target");
        return nullptr;
    }

    GLuint renderbuffer = WebGLRenderbuffer::DEFAULT_RENDER_BUFFER;
    WebGLRenderbuffer *webGlRenderbuffer = WebGLRenderbuffer::GetObjectInstance(env, object);
    if (webGlRenderbuffer != nullptr && !webGlRenderbuffer->HasBound()) {
        renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    }

    LOGI("WebGL bindRenderbuffer target %{public}u renderbuffer %{public}u", target, renderbuffer);
    glBindRenderbuffer(target, renderbuffer);

    // record render buffer
    boundRenderBufferId = renderbuffer;
    if (webGlRenderbuffer) {
        webGlRenderbuffer->SetTarget(target);
    }
    LOGE("WebGL bindRenderbuffer %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CreateBuffer(napi_env env)
{
    WebGLBuffer *webGlBuffer = nullptr;
    napi_value objBuffer = WebGLBuffer::CreateObjectInstance(env, &webGlBuffer).val_;
    if (webGlBuffer == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    unsigned int bufferId;
    glGenBuffers(1, &bufferId);
    webGlBuffer->SetBufferId(bufferId);
    bool ret = AddObject<WebGLBuffer>(env, bufferId, objBuffer);
    LOGI("WebGL createBuffer bufferId %{public}u %{public}u", bufferId, ret);
    return objBuffer;
}

napi_value WebGLRenderingContextBaseImpl::CreateFrameBuffer(napi_env env)
{
    WebGLFramebuffer *webGlFramebuffer = nullptr;
    napi_value objFramebuffer = WebGLFramebuffer::CreateObjectInstance(env, &webGlFramebuffer).val_;
    if (!webGlFramebuffer) {
        return NVal::CreateNull(env).val_;
    }
    unsigned int framebufferId;
    glGenFramebuffers(1, &framebufferId);
    webGlFramebuffer->SetFramebuffer(framebufferId);
    (void)AddObject<WebGLFramebuffer>(env, framebufferId, objFramebuffer);
    LOGI("WebGL createFramebuffer framebufferId %{public}u", framebufferId);
    return objFramebuffer;
}

napi_value WebGLRenderingContextBaseImpl::CreateProgram(napi_env env)
{
    WebGLProgram *webGlProgram = nullptr;
    napi_value objProgram = WebGLProgram::CreateObjectInstance(env, &webGlProgram).val_;
    if (webGlProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    GLuint programId = glCreateProgram();
    if (programId == 0) {
        LOGI("WebGL createProgram fail programId %{public}u", programId);
        return NVal::CreateNull(env).val_;
    }
    webGlProgram->SetProgramId(programId);

    bool succ = AddObject<WebGLProgram>(env, programId, objProgram);
    LOGI("WebGL createProgram programId %{public}u %{public}u", programId, succ);
    return objProgram;
}

napi_value WebGLRenderingContextBaseImpl::CreateRenderBuffer(napi_env env)
{
    WebGLRenderbuffer *webGlRenderbuffer = nullptr;
    napi_value objRenderbuffer = WebGLRenderbuffer::CreateObjectInstance(env, &webGlRenderbuffer).val_;
    if (!webGlRenderbuffer) {
        return NVal::CreateNull(env).val_;
    }
    GLuint renderbufferId;
    glGenRenderbuffers(1, &renderbufferId);
    webGlRenderbuffer->SetRenderbuffer(renderbufferId);
    (void)AddObject<WebGLRenderbuffer>(env, renderbufferId, objRenderbuffer);
    LOGI("WebGL createRenderbuffer renderbufferId %{public}u result %{public}p", renderbufferId, objRenderbuffer);
    return objRenderbuffer;
}

napi_value WebGLRenderingContextBaseImpl::CreateShader(napi_env env, GLenum type)
{
    if (type != WebGLRenderingContextBase::VERTEX_SHADER && type != WebGLRenderingContextBase::FRAGMENT_SHADER) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM, "createShader Invalid type");
        return NVal::CreateNull(env).val_;
    }
    WebGLShader *webGlShader = nullptr;
    napi_value objShader = WebGLShader::CreateObjectInstance(env, &webGlShader).val_;
    if (!webGlShader) {
        return NVal::CreateNull(env).val_;
    }
    GLuint shaderId = glCreateShader(type);
    webGlShader->SetShaderId(shaderId);
    LOGI("WebGL createShader shaderId = %{public}d", shaderId);
    if (shaderId == 0) {
        LOGE("WebGL createShader failed. type %40{public}x", type);
        return NVal::CreateNull(env).val_;
    }
    (void)AddObject<WebGLShader>(env, shaderId, objShader);
    return objShader;
}

napi_value WebGLRenderingContextBaseImpl::DeleteBuffer(napi_env env, napi_value object)
{
    LOGI("WebGL deleteBuffer ");
    WebGLBuffer *webGlBuffer = WebGLObject::GetObjectInstance<WebGLBuffer>(env, object);
    if (webGlBuffer == nullptr) {
        return nullptr;
    }
    GLuint buffer = webGlBuffer->GetBufferId();
    DeleteObject<WebGLBuffer>(env, buffer);

    glDeleteBuffers(1, &buffer);
    LOGI("WebGL deleteBuffer bufferId %{public}u %{public}p", buffer, webGlBuffer);
    if (!webGlBuffer->IsValid()) {
        return nullptr;
    }
    switch (webGlBuffer->GetTarget()) {
        case WebGLRenderingContextBase::ARRAY_BUFFER:
            boundBufferId = (buffer == boundBufferId) ? 0 : boundBufferId;
            break;
        case WebGLRenderingContextBase::ELEMENT_ARRAY_BUFFER:
            boundElementBufferId = (buffer == boundElementBufferId) ? 0 : boundElementBufferId;
            break;
        default:
            break;
    }
    webGlBuffer->Delete();
    LOGI("WebGL deleteBuffer  error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DeleteFrameBuffer(napi_env env, napi_value object)
{
    LOGI("WebGL deleteFramebuffer ");
    WebGLFramebuffer *webGlFramebuffer = WebGLObject::GetObjectInstance<WebGLFramebuffer>(env, object);
    if (webGlFramebuffer == nullptr) {
        return nullptr;
    }
    GLuint frameBufferId = webGlFramebuffer->GetFramebuffer();
    DeleteObject<WebGLFramebuffer>(env, frameBufferId);

    LOGI("WebGL deleteFramebuffer framebufferId %{public}u", frameBufferId);
    glDeleteFramebuffers(1, &frameBufferId);
    if (boundFrameBufferId == frameBufferId) {
        boundFrameBufferId = 0;
    }
    webGlFramebuffer->Delete();
    LOGI("WebGL deleteFramebuffer  error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DeleteProgram(napi_env env, napi_value object)
{
    WebGLProgram *webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram == nullptr) {
        return nullptr;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    DeleteObject<WebGLProgram>(env, programId);
    glDeleteProgram(static_cast<GLuint>(programId));
    LOGI("WebGL deleteProgram programId %{public}u", programId);
    webGlProgram->Delete();
    LOGI("WebGL deleteProgram  error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DeleteRenderBuffer(napi_env env, napi_value object)
{
    LOGI("WebGL deleteRenderbuffer ");
    WebGLRenderbuffer *webGlRenderbuffer = WebGLObject::GetObjectInstance<WebGLRenderbuffer>(env, object);
    if (webGlRenderbuffer == nullptr) {
        return nullptr;
    }

    uint32_t renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    DeleteObject<WebGLRenderbuffer>(env, renderbuffer);
    glDeleteRenderbuffers(1, &renderbuffer);
    LOGI("WebGL deleteRenderbuffer renderbuffer %{public}u", renderbuffer);
    if (renderbuffer == boundRenderBufferId) {
        boundRenderBufferId = 0;
    }
    webGlRenderbuffer->Delete();
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DeleteShader(napi_env env, napi_value object)
{
    uint32_t shaderId = WebGLShader::DEFAULT_SHADER_ID;
    WebGLShader *webGlShader = WebGLObject::GetObjectInstance<WebGLShader>(env, object);
    if (webGlShader != nullptr) {
        shaderId = webGlShader->GetShaderId();
    }
    DeleteObject<WebGLShader>(env, shaderId);
    glDeleteShader(static_cast<GLuint>(shaderId));
    LOGI("WebGL deleteShader shaderId %{public}u", shaderId);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DeleteTexture(napi_env env, napi_value object)
{
    uint32_t texture = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture *webGlTexture = WebGLObject::GetObjectInstance<WebGLTexture>(env, object);
    if (webGlTexture != nullptr) {
        texture = webGlTexture->GetTexture();
    }
    LOGI("WebGL deleteTexture textureId %{public}u", texture);
    DeleteObject<WebGLTexture>(env, texture);
    glDeleteTextures(1, &texture);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GetFrameBufferAttachmentParameter(napi_env env,
    GLenum target, GLenum attachment, GLenum pname)
{
    LOGI("WebGL getFramebufferAttachmentParameter target %{public}u %{public}u %{public}u",
        target, attachment, pname);
    if (!CheckFrameBufferTarget(env, target)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM, "getFramebufferAttachmentParameter Invalid target");
        return nullptr;
    }
    if (!CheckAttachment(env, attachment)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    GLint type = 0;
    if (pname == WebGLRenderingContextBase::FRAMEBUFFER_ATTACHMENT_OBJECT_NAME) {
        glGetFramebufferAttachmentParameteriv(target, attachment,
            WebGLRenderingContextBase::FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type);
        GLint objectId;
        glGetFramebufferAttachmentParameteriv(target, attachment, pname, &objectId);
        LOGI("WebGL getFramebufferAttachmentParameter objectId %{public}u", objectId);
        if (type == WebGLRenderingContextBase::RENDERBUFFER) {
            return GetObject<WebGLRenderbuffer>(env, objectId);
        } else if (type == WebGLRenderingContextBase::TEXTURE) {
            return GetObject<WebGLTexture>(env, objectId);
        }
    } else if (CheckGLenum(env, pname, {
        WebGLRenderingContextBase::FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
        WebGLRenderingContextBase::FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
        WebGLRenderingContextBase::FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
        WebGLRenderingContextBase::FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE
    }, {
        WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,
        WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,
        WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING,
        WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE,
        WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,
        WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_GREEN_SIZE,
        WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_RED_SIZE,
        WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,
        WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER
    })) {
        GLint params;
        glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
        LOGI("WebGL getFramebufferAttachmentParameter result %{public}u", params);
        return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
    }
    SetError(WebGLRenderingContextBase::INVALID_ENUM);
    LOGI("WebGL getFramebufferAttachmentParameter : no image is attached pname %{public}u %{public}u", pname, type);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::FrameBufferRenderBuffer(napi_env env,
    GLenum target, GLenum attachment, GLenum renderBufferTarget, napi_value object)
{
    LOGI("WebGL framebufferRenderbuffer target %{public}u %{public}u %{public}u",
        target, attachment, renderBufferTarget);
    if (!CheckFrameBufferTarget(env, target)) {
        LOGI("WebGL framebufferRenderbuffer invalid target %{public}u", target);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckAttachment(env, attachment)) {
        LOGI("WebGL framebufferRenderbuffer invalid attachment %{public}u", attachment);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckRenderBufferTarget(env, renderBufferTarget)) {
        LOGI("WebGL framebufferRenderbuffer invalid renderBufferTarget %{public}u", renderBufferTarget);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLFramebuffer *frameBuffer = WebGLRenderingContextBaseImpl::GetBoundFrameBuffer(env, target);
    if (frameBuffer == nullptr) {
        LOGI("WebGL framebufferRenderbuffer can not find bind frame buffer %{public}u", boundFrameBufferId);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    GLuint renderbuffer = WebGLRenderbuffer::DEFAULT_RENDER_BUFFER;
    WebGLRenderbuffer *webGlRenderbuffer = WebGLRenderbuffer::GetObjectInstance(env, object);
    if (webGlRenderbuffer != nullptr) {
        renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    }
    LOGI("WebGL framebufferRenderbuffer renderbuffer %{public}u", renderbuffer);
    if (attachment == WebGLRenderingContextBase::DEPTH_STENCIL_ATTACHMENT) {
        glFramebufferRenderbuffer(target, attachment, WebGLRenderingContextBase::DEPTH_ATTACHMENT, renderbuffer);
        LOGI("WebGL framebufferRenderbuffer renderbuffer %{public}u error %{public}u", renderbuffer, GetError_());
        glFramebufferRenderbuffer(target, attachment, WebGLRenderingContextBase::STENCIL_ATTACHMENT, renderbuffer);
    } else {
        glFramebufferRenderbuffer(target, attachment, renderBufferTarget, renderbuffer);
    }
    LOGI("WebGL framebufferRenderbuffer renderbuffer %{public}u error %{public}u", renderbuffer, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::FrameBufferTexture2D(napi_env env,
    GLenum target, GLenum attachment, GLenum textarget, napi_value texture, GLint level)
{
    LOGI("WebGL framebufferTexture2D target %{public}u %{public}u %{public}u", target, attachment, textarget);
    if (!CheckFrameBufferTarget(env, target)) {
        LOGI("WebGL framebufferTexture2D invalid target %{public}u", target);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckAttachment(env, attachment)) {
        LOGI("WebGL framebufferTexture2D invalid attachment %{public}u", attachment);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckTexture2DTarget(env, textarget)) {
        LOGI("WebGL framebufferTexture2D invalid textarget %{public}u", textarget);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLFramebuffer *frameBuffer = GetBoundFrameBuffer(env, target);
    if (frameBuffer == nullptr) {
        LOGI("WebGL framebufferTexture2D can not find bind frame buffer %{public}u", boundFrameBufferId);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    unsigned int textureId = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture *webGlTexture = WebGLTexture::GetObjectInstance(env, texture);
    if (webGlTexture != nullptr) {
        textureId = webGlTexture->GetTexture();
    }
    LOGI("WebGL framebufferTexture2D texture %{public}u", textureId);
    glFramebufferTexture2D(target, attachment, textarget, static_cast<GLuint>(textureId), level);
    LOGI("WebGL framebufferTexture2D texture %{public}u result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GetProgramParameter(napi_env env, napi_value object, GLenum pname)
{
    uint32_t programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram *webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram != nullptr) {
        programId = webGlProgram->GetProgramId();
    }
    LOGI("WebGL getProgramParameter programId %{public}u", programId);

    GLint params = 0;
    glGetProgramiv(static_cast<GLuint>(programId), pname, &params);
    LOGI("WebGL getProgramParameter pname %{public}u params %{public}d", pname, params);

    if (pname == GL_DELETE_STATUS || pname == GL_LINK_STATUS || pname == GL_VALIDATE_STATUS) {
        bool res = (params == GL_FALSE) ? false : true;
        return NVal::CreateBool(env, res).val_;
    } else if (pname == GL_ATTACHED_SHADERS || pname == GL_ACTIVE_ATTRIBUTES || pname == GL_ACTIVE_UNIFORMS) {
        int64_t res = static_cast<int64_t>(params);
        return NVal::CreateInt64(env, res).val_;
    }
    LOGI("WebGL getProgramParameter : pname is wrong");
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GetAttachedShaders(napi_env env, napi_value object)
{
    uint32_t programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram *webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram != nullptr) {
        programId = webGlProgram->GetProgramId();
    }
    LOGI("WebGL getAttachedShaders programId %{public}u", programId);

    GLsizei count = 1;
    std::vector<GLuint> shaderId(MAX_COUNT_ATTACHED_SHADER);
    glGetAttachedShaders(static_cast<GLuint>(programId),
        MAX_COUNT_ATTACHED_SHADER, &count, static_cast<GLuint*>(shaderId.data()));
    LOGI("WebGL getAttachedShaders count %{public}u", count);
    GLsizei i;
    napi_value shaderArr = nullptr;
    napi_create_array(env, &shaderArr);
    for (i = 0; i < count; i++) {
        napi_value objShader = GetObject<WebGLShader>(env, shaderId[i]);
        napi_status programStatus = napi_set_element(env, shaderArr, i, objShader);
        LOGI("WebGL getAttachedShaders %{public}u %{public}u", programStatus, shaderId[i]);
    }
    return shaderArr;
}

napi_value WebGLRenderingContextBaseImpl::GetUniformLocation(napi_env env, napi_value object, const char *name)
{
    WebGLProgram *webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    LOGI("WebGL getUniformLocation programId %{public}u", programId);
    LOGI("WebGL getUniformLocation name %{public}s", name);

    GLint locationId = glGetUniformLocation(static_cast<GLuint>(programId), name);
    LOGI("WebGL getUniformLocation locationId %{public}d", locationId);
    if (locationId == -1) {
        return NVal::CreateNull(env).val_;
    }
    // check if exit
    LOGI("WebGL getUniformLocation locationId %{public}d ", locationId);
    napi_value objUniformLocation = GetNapiValue<WebGLUniformLocation>(env, locationId);
    if (objUniformLocation == nullptr) { // create new
        WebGLUniformLocation *webGLUniformLocation = nullptr;
        objUniformLocation = WebGLUniformLocation::CreateObjectInstance(env, &webGLUniformLocation).val_;
        if (!objUniformLocation) {
            LOGI("WebGL getUniformLocation locationId %{public}d null", locationId);
            return NVal::CreateNull(env).val_;
        }
        webGLUniformLocation->SetUniformLocationId(locationId);
        AddObject<WebGLUniformLocation>(env, locationId, objUniformLocation);
        LOGI("WebGL getUniformLocation locationId %{public}d create", locationId);
    }
    return objUniformLocation;
}

napi_value WebGLRenderingContextBaseImpl::GetVertexAttrib(napi_env env, GLenum pname, GLuint index)
{
    LOGI("WebGL getVertexAttrib index %{public}u %{public}u", index, pname);
    if (index >= GetMaxVertexAttribs()) {
        LOGI("WebGL getVertexAttrib index %{public}u", index);
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "getVertexAttrib Invalid index");
        return nullptr;
    }
    int32_t params;
    if (pname == GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING) {  // webglBuffer
        glGetVertexAttribiv(index, pname, &params);
        LOGI("WebGL getVertexAttrib %{public}u", params);
        return GetObject<WebGLBuffer>(env, params);
    } else if (pname == GL_VERTEX_ATTRIB_ARRAY_ENABLED || pname == GL_VERTEX_ATTRIB_ARRAY_NORMALIZED ||
        pname == GL_VERTEX_ATTRIB_ARRAY_INTEGER) {
        glGetVertexAttribiv(index, pname, &params);
        bool res = static_cast<bool>(params);
        LOGI("WebGL getVertexAttrib %{public}u %{public}u", res, params);
        return NVal::CreateBool(env, res).val_;
    } else if (pname == GL_VERTEX_ATTRIB_ARRAY_SIZE || pname == GL_VERTEX_ATTRIB_ARRAY_STRIDE ||
        pname == GL_VERTEX_ATTRIB_ARRAY_DIVISOR || pname == GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE) {
        glGetVertexAttribiv(index, pname, &params);
        LOGI("WebGL getVertexAttrib %{public}u", params);
        return NVal::CreateInt64(env, params).val_;
    } else if (pname == GL_VERTEX_ATTRIB_ARRAY_TYPE) { // GLenum
        glGetVertexAttribiv(index, pname, &params);
        LOGI("WebGL getVertexAttrib %{public}u", params);
        return NVal::CreateInt64(env, params).val_;
    } else if (pname == GL_CURRENT_VERTEX_ATTRIB) { // float32Array
        WebGLWriteBufferArg writeBuffer(env);
        GLfloat *params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(4 * sizeof(GLfloat))); // 4 args
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetVertexAttribfv(index, pname, params);
        return writeBuffer.ToNormalArray(BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32);
    } else { // return result from gl
        glGetVertexAttribiv(index, pname, &params);
        LOGI("WebGL getVertexAttrib getVertexAttrib end");
        return nullptr;
    }
}

static napi_value GetExtensionsParameter(napi_env env, GLenum pname)
{
    const unsigned char* extensions = glGetString(pname);
    if (extensions == nullptr) {
        return nullptr;
    }
    LOGI("WebGL GetExtensionsParameter %{public}s", extensions);
    std::string str = const_cast<char*>(reinterpret_cast<const char*>(extensions));
    vector<std::string> vec;
    Util::SplitString(str, vec, " ");
    LOGI("WebGL GetExtensionsParameter %{public}s %{public}d", str.c_str(), vec.size());
    napi_value result = nullptr;
    napi_create_array_with_length(env, vec.size(), &result);
    for (vector<std::string>::size_type i = 0; i != vec.size(); ++i) {
        napi_set_element(env, result, i, NVal::CreateUTF8String(env, vec[i]).val_);
        LOGI("WebGL GetExtensionsParameter %{public}s %{public}d", vec[i].c_str(), (int)i);
    }
    return result;
}

napi_value WebGLRenderingContextBaseImpl::GetFloatParameter(napi_env env, GLenum pname)
{
    GLfloat params;
    glGetFloatv(pname, &params);
    float res = static_cast<float>(params);
    LOGI("WebGL getParameter params %{public}f", params);
    return NVal::CreateDouble(env, (double)res).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetTextureParameter(napi_env env, GLenum pname)
{
    GLint params;
    glGetIntegerv(pname, &params);
    return GetObject<WebGLTexture>(env, params);
}

napi_value WebGLRenderingContextBaseImpl::GetBufferParameter(napi_env env, GLenum pname)
{
    GLint params;
    glGetIntegerv(pname, &params);
    LOGI("WebGL getParameter params %{public}d", params);
    return GetObject<WebGLBuffer>(env, params);
}

napi_value WebGLRenderingContextBaseImpl::GetFramebufferParameter(napi_env env, GLenum pname)
{
    GLint params;
    glGetIntegerv(pname, &params);
    LOGI("WebGL getParameter params %{public}d", params);
    return GetObject<WebGLFramebuffer>(env, params);
}

napi_value WebGLRenderingContextBaseImpl::GetProgramParameter(napi_env env, GLenum pname)
{
    GLint params;
    glGetIntegerv(pname, &params);
    LOGI("WebGL getParameter params %{public}d", params);
    return GetObject<WebGLProgram>(env, params);
}

napi_value WebGLRenderingContextBaseImpl::GetRenderbufferParameter(napi_env env, GLenum pname)
{
    GLint params;
    glGetIntegerv(pname, &params);
    LOGI("WebGL getParameter params %{public}d %{public}p", params, GetObject<WebGLRenderbuffer>(env, params));
    return GetObject<WebGLRenderbuffer>(env, params);
}

napi_value WebGLRenderingContextBaseImpl::GetAliasedrParameter(napi_env env, GLenum pname)
{
    WebGLWriteBufferArg writeBuffer(env);
    GLfloat* params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(2 * sizeof(GLfloat))); // 2 args
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetFloatv(static_cast<GLenum>(pname), params);
    return writeBuffer.ToNormalArray(BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32);
}

napi_value WebGLRenderingContextBaseImpl::GetColorParameter(napi_env env, GLenum pname)
{
    WebGLWriteBufferArg writeBuffer(env);
    GLfloat* params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(4 * sizeof(GLfloat))); // 4 args
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetFloatv(static_cast<GLenum>(pname), params);
    return writeBuffer.ToNormalArray(BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32);
}

napi_value WebGLRenderingContextBaseImpl::GetCompressedTextureFormatsParameter(napi_env env, GLenum pname)
{
    GLint count = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &count); // 16383
    WebGLWriteBufferArg writeBuffer(env);
    GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(count * sizeof(GLint)));
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("getParameter GL_COMPRESSED_TEXTURE_FORMATS %{public}u", count);
    glGetIntegerv(static_cast<GLenum>(pname), params);
    return writeBuffer.ToExternalArray(BUFFER_DATA_INT_32);
}

napi_value WebGLRenderingContextBaseImpl::GetMaxViewportDimsParameter(napi_env env, GLenum pname)
{
    WebGLWriteBufferArg writeBuffer(env);
    GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(2 * sizeof(GLint))); // 2 args
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetIntegerv(pname, params);
    return writeBuffer.ToExternalArray(BUFFER_DATA_INT_32);
}

napi_value WebGLRenderingContextBaseImpl::GetScissorBoxParameter(napi_env env, GLenum pname)
{
    WebGLWriteBufferArg writeBuffer(env);
    GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(4 * sizeof(GLint))); // 4 args
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetIntegerv(static_cast<GLenum>(pname), params);
    return writeBuffer.ToExternalArray(BUFFER_DATA_INT_32);
}

napi_value WebGLRenderingContextBaseImpl::GetBoolParameter(napi_env env, GLenum pname)
{
    GLboolean params;
    glGetBooleanv(pname, &params);
    bool res = static_cast<bool>(params);
    LOGI("WebGL getParameter end");
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetColorWriteMaskParameter(napi_env env, GLenum pname)
{
    WebGLWriteBufferArg writeBuffer(env);
    GLboolean* params = reinterpret_cast<GLboolean*>(writeBuffer.AllocBuffer(4 * sizeof(GLboolean))); // 4 args
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetBooleanv(pname, static_cast<GLboolean*>(params));
    return writeBuffer.ToNormalArray(BUFFER_DATA_BOOLEAN, BUFFER_DATA_BOOLEAN);
}

napi_value WebGLRenderingContextBaseImpl::GetIntegervParameter(napi_env env, GLenum pname)
{
    GLint params;
    glGetIntegerv(pname, &params);
    int64_t res = static_cast<int64_t>(params);
    LOGI("WebGL getParameter params %{public}d", params);
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetParameter(napi_env env, GLenum pname)
{
    LOGI("WebGL getParameter pname = %{public}u", pname);
    if (pname == GL_DEPTH_CLEAR_VALUE || pname == GL_LINE_WIDTH || pname == GL_POLYGON_OFFSET_FACTOR ||
        pname == GL_POLYGON_OFFSET_UNITS || pname == GL_SAMPLE_COVERAGE_VALUE) {
        return GetFloatParameter(env, pname);
    } else if (pname == GL_TEXTURE_BINDING_2D || pname == GL_TEXTURE_BINDING_CUBE_MAP) {
        return GetTextureParameter(env, pname);
    } else if (pname == GL_RENDERER || pname == GL_SHADING_LANGUAGE_VERSION ||
        pname == GL_VENDOR || pname == GL_VERSION) {
        return GetExtensionsParameter(env, pname);
    } else if (pname == GL_ARRAY_BUFFER_BINDING || pname == GL_ELEMENT_ARRAY_BUFFER_BINDING) {
        return GetBufferParameter(env, pname);
    } else if (pname == GL_FRAMEBUFFER_BINDING) {
        return GetFramebufferParameter(env, pname);
    } else if (pname == GL_CURRENT_PROGRAM) {
        return GetProgramParameter(env, pname);
    } else if (pname == GL_RENDERBUFFER_BINDING) {
        return GetRenderbufferParameter(env, pname);
    } else if (pname == GL_ALIASED_LINE_WIDTH_RANGE || pname == GL_ALIASED_POINT_SIZE_RANGE ||
        pname == GL_DEPTH_RANGE) {
        return GetAliasedrParameter(env, pname);
    } else if (pname == GL_COLOR_CLEAR_VALUE || pname == GL_BLEND_COLOR) {
        return GetColorParameter(env, pname);
    } else if (pname == GL_COMPRESSED_TEXTURE_FORMATS) {
        return GetCompressedTextureFormatsParameter(env, pname);
    } else if (pname == GL_MAX_VIEWPORT_DIMS) {
        return GetMaxViewportDimsParameter(env, pname);
    } else if (pname == GL_SCISSOR_BOX || pname == WebGLRenderingContextBase::VIEWPORT) {
        return GetScissorBoxParameter(env, pname);
    } else if (pname == GL_BLEND || pname == GL_CULL_FACE || pname == GL_DEPTH_TEST ||
        pname == GL_DEPTH_WRITEMASK || pname == GL_DITHER || pname == GL_POLYGON_OFFSET_FILL ||
        pname == GL_SAMPLE_ALPHA_TO_COVERAGE || pname == GL_SAMPLE_COVERAGE ||
        pname == GL_SAMPLE_COVERAGE_INVERT || pname == GL_SCISSOR_TEST || pname == GL_STENCIL_TEST) {
        return GetBoolParameter(env, pname);
    } else if (pname == GL_COLOR_WRITEMASK) {
        return GetColorWriteMaskParameter(env, pname);
    } else if (pname == GL_ACTIVE_TEXTURE || pname == GL_ALPHA_BITS || pname == GL_BLEND_DST_ALPHA ||
        pname == GL_BLEND_DST_RGB || pname == GL_BLEND_EQUATION_ALPHA || pname == GL_BLEND_EQUATION_RGB ||
        pname == GL_BLEND_SRC_ALPHA || pname == GL_BLEND_SRC_RGB || pname == GL_BLUE_BITS ||
        pname == GL_CULL_FACE_MODE || pname == GL_DEPTH_BITS || pname == GL_DEPTH_FUNC ||
        pname == GL_FRONT_FACE || pname == GL_GENERATE_MIPMAP_HINT || pname == GL_GREEN_BITS ||
        pname == GL_IMPLEMENTATION_COLOR_READ_FORMAT || pname == GL_IMPLEMENTATION_COLOR_READ_TYPE ||
        pname == GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS || pname == GL_MAX_CUBE_MAP_TEXTURE_SIZE ||
        pname == GL_MAX_FRAGMENT_UNIFORM_VECTORS || pname == GL_MAX_RENDERBUFFER_SIZE ||
        pname == GL_MAX_TEXTURE_IMAGE_UNITS || pname == GL_MAX_TEXTURE_SIZE || pname == GL_MAX_VARYING_VECTORS ||
        pname == GL_MAX_VERTEX_ATTRIBS || pname == GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS ||
        pname == GL_MAX_VERTEX_UNIFORM_VECTORS || pname == GL_PACK_ALIGNMENT || pname == GL_RED_BITS ||
        pname == GL_SAMPLE_BUFFERS || pname == GL_SAMPLES || pname == GL_STENCIL_BACK_FAIL ||
        pname == GL_STENCIL_BACK_FUNC || pname == GL_STENCIL_BACK_PASS_DEPTH_FAIL ||
        pname == GL_STENCIL_BACK_PASS_DEPTH_PASS || pname == GL_STENCIL_BACK_REF ||
        pname == GL_STENCIL_BACK_VALUE_MASK || pname == GL_STENCIL_BACK_WRITEMASK ||
        pname == GL_STENCIL_BITS || pname == GL_STENCIL_CLEAR_VALUE || pname == GL_STENCIL_FAIL ||
        pname == GL_STENCIL_FUNC || pname == GL_STENCIL_PASS_DEPTH_FAIL || pname == GL_STENCIL_PASS_DEPTH_PASS ||
        pname == GL_STENCIL_REF || pname == GL_STENCIL_VALUE_MASK || pname == GL_STENCIL_WRITEMASK ||
        pname == GL_SUBPIXEL_BITS || pname == GL_UNPACK_ALIGNMENT) {
        return GetIntegervParameter(env, pname);
    } else if (pname == WebGLRenderingContextBase::UNPACK_COLORSPACE_CONVERSION_WEBGL) {
        return NVal::CreateInt64(env, unpackColorspaceConversion).val_;
    } else if (pname == WebGLRenderingContextBase::UNPACK_FLIP_Y_WEBGL) {
        return NVal::CreateBool(env, unpackFlipY).val_;
    } else if (pname == WebGLRenderingContextBase::UNPACK_PREMULTIPLY_ALPHA_WEBGL) {
        return NVal::CreateBool(env, unpackPremultiplyAlpha).val_;
    }
    LOGI("WebGL getParameter : pname is wrong");
    SetError(WebGLRenderingContextBase::INVALID_ENUM);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::DrawElements(napi_env env, GLenum mode, GLsizei count,
    GLenum type, GLintptr offset)
{
    LOGI("WebGL drawElements mode %{public}u %{public}u %{public}u %{public}u", mode, count, type, offset);
    uint32_t size = 1;
    switch (type) {
        case WebGLRenderingContextBase::UNSIGNED_BYTE:
            break;
        case WebGLRenderingContextBase::UNSIGNED_SHORT:
            size = sizeof(short);
            break;
        case WebGLRenderingContextBase::UNSIGNED_INT:
            size = sizeof(int);
            if (IsHighWebGL()) {
                break;
            }
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM, "drawElements Invalid type");
            return nullptr;
    }
    if ((offset % size) != 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "drawElements Invalid offset");
        return nullptr;
    }

    WebGLBuffer *webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundElementBufferId);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferData() == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION, "drawElements Can not find bound buffer");
        return nullptr;
    }
    LOGI("WebGL drawElements buffer length %{public}u", webGLBuffer->GetBufferData()->GetBufferLength());

    // check count
    if (size * count > webGLBuffer->GetBufferData()->GetBufferLength()) {
        LOGE("WebGL drawElements Insufficient buffer size %{public}u", count);
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
    }
    if (offset < 0 || static_cast<size_t>(offset) >= webGLBuffer->GetBufferData()->GetBufferLength()) {
        LOGE("WebGL drawElements invalid offset %{public}u", offset);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glDrawElements(mode, static_cast<GLsizei>(count), type, reinterpret_cast<GLvoid *>(offset));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DrawArrays(napi_env env, GLenum mode, GLint first, GLsizei count)
{
    LOGI("WebGL drawArrays count %{public}u %{public}u %{public}u", mode, first, count);
    if (!CheckDrawMode(env, mode)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (first < 0 || count < 0) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    if (!currentProgramId) {
        LOGE("WebGL drawArrays no valid shader program in use");
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    glDrawArrays(mode, first, count);
    LOGI("WebGL drawArrays %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ReadPixels(napi_env env, const PixelsArg *arg, GLintptr offset)
{
    glReadPixels(arg->x, arg->y, arg->width, arg->height, arg->format, arg->type,
        reinterpret_cast<void*>(offset));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ReadPixels(napi_env env, const PixelsArg *arg,
    napi_value buffer, GLuint dstOffset)
{
    LOGI("WebGL readPixels pixels [%{public}d %{public}d %{public}d %{public}d %{public}u %{public}u]",
        arg->x, arg->y, arg->width, arg->height, arg->format, arg->type);

    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(buffer);
    if (status != 0) {
        LOGE("WebGL readPixels failed to getbuffer data");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (!CheckPixelsFormat(env, arg->format)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckPixelsType(env, arg->type)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    LOGI("WebGL readPixels pixels %{public}p", bufferData.GetBuffer());
    glReadPixels(arg->x, arg->y, arg->width, arg->height, arg->format, arg->type,
        static_cast<void*>(bufferData.GetBuffer()));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ClearColor(napi_env env, GLclampf red, GLclampf green,
    GLclampf blue, GLclampf alpha)
{
    LOGI("WebGL clearColor [%{public}f, %{public}f, %{public}f, %{public}f]", red, green, blue, alpha);
    glClearColor(red, green, blue, alpha);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::Clear(napi_env env, GLbitfield mask)
{
    LOGI("WebGL clear mask %{public}x", mask);
    if (mask & ~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glClear(mask);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::UseProgram(napi_env env, napi_value object)
{
    uint32_t program = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, object);
    if (webGLProgram != nullptr) {
        program = webGLProgram->GetProgramId();
    }
    LOGI("WebGL useProgram programId %{public}u %{public}u", program, currentProgramId);
    if ((currentProgramId != program) || (currentProgramId == 0)) {
        glUseProgram(static_cast<GLuint>(program));
        currentProgramId = program;
    }
    LOGI("WebGL useProgram programId %{public}u result %{public}u", program, GetError_());
    return nullptr;
}

WebGLBuffer *WebGLRenderingContextBaseImpl::CheckAndGetBoundBuffer(napi_env env, GLenum target)
{
    WebGLBuffer *webGLBuffer = nullptr;
    switch (target) {
        case WebGLRenderingContextBase::ELEMENT_ARRAY_BUFFER:
            webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundElementBufferId);
            break;
        case WebGLRenderingContextBase::ARRAY_BUFFER:
            LOGI("WebGL bufferData boundBufferId %{public}u,", boundBufferId);
            webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferId);
            break;
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    if (webGLBuffer == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
    }
    return webGLBuffer;
}

napi_value WebGLRenderingContextBaseImpl::BufferData_(napi_env env,
    GLenum target, GLsizeiptr size, GLenum usage, WebGLReadBufferArg *bufferData)
{
    LOGI("WebGL bufferData target %{public}u, usage %{public}u size %{public}u", target, usage, size);
    WebGLBuffer *webGLBuffer = CheckAndGetBoundBuffer(env, target);
    if (webGLBuffer == nullptr) {
        LOGE("WebGL bufferData can not get bound buffer id %{public}u", boundBufferId);
        return nullptr;
    }
    if (!CheckBufferDataUsage(env, usage)) {
        LOGE("WebGL bufferData invalid usage %{public}u", usage);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (webGLBuffer->GetTarget() != target) {
        LOGE("WebGL bufferData Invalid buffer target %{public}u %{public}u", webGLBuffer->GetTarget(), target);
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    if (bufferData != nullptr) {
        webGLBuffer->SaveBuffer(bufferData);
        glBufferData(target, size, bufferData->GetBuffer(), usage);
    } else {
        glBufferData(target, size, nullptr, usage);
    }
    webGLBuffer->SetTarget(target);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData(napi_env env, GLenum target, GLsizeiptr size, GLenum usage)
{
    LOGI("WebGL bufferData target %{public}u, usage %{public}u size %{public}u", target, usage, size);
    BufferData_(env, target, size, usage, nullptr);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData(napi_env env,
    GLenum target, napi_value data, GLenum usage, GLuint srcOffset, GLuint length)
{
    std::unique_ptr<WebGLReadBufferArg> bufferData = std::make_unique<WebGLReadBufferArg>(env);
    if (bufferData == nullptr) {
        LOGE("bufferData invalid input buffer");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    bool succ = bufferData->GenBufferData(data, BUFFER_DATA_FLOAT_32) == napi_ok;
    if (!succ) {
        LOGE("bufferData invalid buffer, can not get data");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGI("WebGL bufferData target %{public}u, usage %{public}u buffer size %{public}u",
        target, usage, bufferData->GetBufferLength());

    // change
    srcOffset = srcOffset * bufferData->GetBufferDataSize();
    length = length == 0 ? bufferData->GetBufferLength() : length * bufferData->GetBufferDataSize();
    bufferData->SetRealBuffer(length, srcOffset);

    BufferData_(env, target, length, usage, bufferData.get());
    bufferData.release();
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferSubData(napi_env env, GLenum target,
    GLintptr offset, napi_value buffer, GLuint srcOffset, GLuint length)
{
    LOGI("WebGL bufferSubData target %{public}u offset %{public}u %{public}u %{public}u",
        target, offset, srcOffset, length);

    WebGLBuffer *webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferId);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferData() == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION, "bufferSubData Can not find bound buffer");
        return nullptr;
    }

    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(buffer);
    if (status != 0) {
        LOGE("WebGL bufferSubData invalid buffer data ");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    // check sub buffer
    if (offset + bufferData.GetBufferLength() > webGLBuffer->GetBufferData()->GetBufferLength()) {
        LOGE("WebGL bufferSubData invalid buffer size %{public}u offset %{public}u ",
            bufferData.GetBufferLength(), webGLBuffer->GetBufferData()->GetBufferLength());
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    LOGI("WebGL bufferSubData size %{public}u", bufferData.GetBufferLength());

    glBufferSubData(target, offset, static_cast<GLsizeiptr>(bufferData.GetBufferLength()),
        static_cast<uint8_t*>(bufferData.GetBuffer()));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::PixelStorei(napi_env env, GLenum pname, GLint param)
{
    LOGI("WebGL pixelStorei pname %{public}u param %{public}u", pname, param);
    switch (pname) {
        case WebGLRenderingContextBase::UNPACK_FLIP_Y_WEBGL:
            unpackFlipY = (param == 1);
            return nullptr;
        case WebGLRenderingContextBase::UNPACK_PREMULTIPLY_ALPHA_WEBGL:
            unpackPremultiplyAlpha = (param == 1);
            return nullptr;
        case WebGLRenderingContextBase::PACK_ALIGNMENT:
        case WebGLRenderingContextBase::UNPACK_ALIGNMENT:
            break;
        case WebGLRenderingContextBase::UNPACK_COLORSPACE_CONVERSION_WEBGL:
            if (static_cast<GLenum>(param) == WebGLRenderingContextBase::BROWSER_DEFAULT_WEBGL || param == GL_NONE) {
                unpackColorspaceConversion = static_cast<GLenum>(param);
            } else {
                LOGE("WebGL pixelStorei invalid parameter for UNPACK_COLORSPACE_CONVERSION_WEBGL %{public}u", pname);
                SetError(WebGLRenderingContextBase::INVALID_VALUE);
                return nullptr;
            }
            break;
        default:
            LOGE("WebGL pixelStorei invalid pname  %{public}u", pname);
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    glPixelStorei(pname, param);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::RenderBufferStorage(napi_env env,
    GLenum target, GLenum internalFormat, GLsizei width, GLsizei height)
{
    LOGI("WebGL renderbufferStorage target %{public}u %{public}u %{public}d %{public}d",
        target, internalFormat, width, height);
    if (target != WebGLRenderingContextBase::RENDERBUFFER) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (!CheckInternalFormat(env, internalFormat)) {
        LOGE("WebGL renderbufferStorage invalid internalFormat %{public}u", internalFormat);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (internalFormat == WebGLRenderingContextBase::DEPTH_STENCIL) {
        internalFormat = GL_DEPTH24_STENCIL8;
    }

    WebGLRenderbuffer *renderBuffer = GetObjectInstance<WebGLRenderbuffer>(env, boundRenderBufferId);
    if (renderBuffer == nullptr) {
        LOGE("WebGL renderbufferStorage can not get bound render buffer Id %{public}u", boundRenderBufferId);
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (width < 0 || height < 0) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glRenderbufferStorage(target, internalFormat, width, height);
    LOGE("WebGL renderbufferStorage error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexImage2D(napi_env env,
    const Image2DArg *imgArg, GLsizei imageSize, GLintptr offset)
{
    LOGI("WebGL compressedTexSubImage2D target %{public}u %{public}d %{public}u ",
        imgArg->target, imgArg->level, imgArg->internalFormat);
    LOGI("WebGL renderbufferStorage target %{public}d %{public}d %{public}d ",
        imgArg->width, imgArg->height, imgArg->border);

    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL compressedTexSubImage2D error %{public}u", error);
        SetError(error);
    }

    glCompressedTexImage2D(imgArg->target, imgArg->level, imgArg->internalFormat,
        imgArg->width, imgArg->height, imgArg->border, imageSize,
        reinterpret_cast<void*>(offset));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexImage2D(napi_env env,
    const Image2DArg *imgArg, napi_value srcData, GLuint srcOffset, GLuint srcLengthOverride)
{
    LOGI("WebGL compressedTexSubImage2D target %{public}u %{public}d %{public}u ",
        imgArg->target, imgArg->level, imgArg->internalFormat);
    LOGI("WebGL renderbufferStorage target %{public}d %{public}d %{public}d ",
        imgArg->width, imgArg->height, imgArg->border);

    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL compressedTexSubImage2D error %{public}u", error);
        SetError(error);
    }

    WebGLReadBufferArg bufferData(env);
    // support ArrayBufferView
    bool succ = bufferData.GenBufferData(srcData, BUFFER_DATA_FLOAT_32) == napi_ok;
    if (!succ) {
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    glCompressedTexImage2D(imgArg->target, imgArg->level, imgArg->internalFormat,
        imgArg->width, imgArg->height, imgArg->border, static_cast<GLsizei>(bufferData.GetBufferLength()),
        static_cast<void*>(bufferData.GetBuffer()));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexSubImage2D(napi_env env, const Image2DArg *imgArg,
    napi_value srcData, GLuint srcOffset, GLuint srcLengthOverride)
{
    LOGI("WebGL compressedTexSubImage2D target %{public}u %{public}d %{public}u ",
        imgArg->target, imgArg->level, imgArg->format);
    LOGI("WebGL compressedTexSubImage2D target %{public}d %{public}d %{public}d %{public}d",
        imgArg->xoffset, imgArg->yoffset, imgArg->width, imgArg->height);

    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL compressedTexSubImage2D error %{public}u", error);
        SetError(error);
    }

    WebGLReadBufferArg bufferData(env);
    GLvoid *data = nullptr;
    GLsizei length = 0;
    if (NVal(env, srcData).IsNull()) {
        // support ArrayBufferView
        bool succ = bufferData.GenBufferData(srcData, BUFFER_DATA_FLOAT_32) == napi_ok;
        if (!succ) {
            return nullptr;
        }
        bufferData.DumpBuffer(bufferData.GetBufferDataType());
        data = reinterpret_cast<void*>(bufferData.GetBuffer());
        length = static_cast<GLsizei>(bufferData.GetBufferLength());
    }
    glCompressedTexSubImage2D(imgArg->target, imgArg->level,
        imgArg->xoffset, imgArg->yoffset, imgArg->width, imgArg->height, imgArg->format, length, data);
    LOGE("WebGL compressedTexSubImage2D error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexSubImage2D(napi_env env,
    const Image2DArg *imgArg, GLsizei imageSize, GLintptr offset)
{
    LOGI("WebGL compressedTexSubImage2D target %{public}u %{public}d %{public}u ",
        imgArg->target, imgArg->level, imgArg->format);
    LOGI("WebGL compressedTexSubImage2D target %{public}d %{public}d %{public}d %{public}d",
        imgArg->xoffset, imgArg->yoffset, imgArg->width, imgArg->height);

    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL compressedTexSubImage2D error %{public}u", error);
        SetError(error);
    }

    glCompressedTexSubImage2D(imgArg->target, imgArg->level,
        imgArg->xoffset, imgArg->yoffset, imgArg->width, imgArg->height, imgArg->format,
        imageSize, reinterpret_cast<void*>(offset));
    LOGE("WebGL compressedTexSubImage2D error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CopyTexImage2D(napi_env env, const Image2DArg *imgArg)
{
    LOGI("WebGL copyTexImage2D target %{public}u %{public}d %{public}u ",
        imgArg->target, imgArg->level, imgArg->internalFormat);
    LOGI("WebGL copyTexImage2D target %{public}d %{public}d %{public}d %{public}d %{public}d",
        imgArg->x, imgArg->y, imgArg->width, imgArg->height, imgArg->border);

    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL copyTexImage2D error %{public}u", error);
        SetError(error);
    }

    glCopyTexImage2D(imgArg->target, imgArg->level, imgArg->internalFormat,
        imgArg->x, imgArg->y, imgArg->width, imgArg->height, imgArg->border);
    LOGE("WebGL copyTexImage2D error %{public}u", GetError_());
    return nullptr;
}
napi_value WebGLRenderingContextBaseImpl::CopyTexSubImage2D(napi_env env, const Image2DArg *imgArg)
{
    LOGI("WebGL copyTexSubImage2D target %{public}u %{public}d %{public}d %{public}d",
        imgArg->target, imgArg->level, imgArg->xoffset, imgArg->yoffset);
    LOGI("WebGL copyTexSubImage2D target %{public}d %{public}d %{public}d %{public}d ",
        imgArg->x, imgArg->y, imgArg->width, imgArg->height);

    GLenum error = CheckTexImage2D(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL copyTexSubImage2D error %{public}u", error);
        SetError(error);
    }

    glCopyTexSubImage2D(imgArg->target, imgArg->level, imgArg->xoffset, imgArg->yoffset,
        imgArg->x, imgArg->y, imgArg->width, imgArg->height);
    LOGE("WebGL copyTexSubImage2D error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::VertexAttribPointer(napi_env env, const VertexAttribArg *vertexInfo)
{
    LOGI("WebGL vertexAttribPointer index %{public}u size %{public}u type %{public}u",
        vertexInfo->index, vertexInfo->size, vertexInfo->type);
    if (vertexInfo->index >= GetMaxVertexAttribs()) {
        LOGE("WebGL vertexAttribPointer invalid index %{public}d", vertexInfo->index);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    if (!CheckGLenum(env, vertexInfo->type, {
        GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_FLOAT
    }, {})) {
        LOGE("WebGL vertexAttribPointer invalid type %{public}d", vertexInfo->type);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
    }
    // check size, 255 is max, 4 is vertexInfo size maximum
    if (vertexInfo->size < 1 || vertexInfo->size > 4 || vertexInfo->stride < 0 || vertexInfo->stride > 255) {
        LOGE("WebGL vertexAttribPointer invalid size %{public}u %{public}u", vertexInfo->size, vertexInfo->stride);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    // check offset
    WebGLBuffer *webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferId);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferData() == nullptr) {
        LOGE("WebGL vertexAttribPointer can not bind buffer %{public}p %{public}u", webGLBuffer, boundBufferId);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    // check offset
    if (vertexInfo->offset < 0 || vertexInfo->offset >= webGLBuffer->GetBufferData()->GetBufferLength()) {
        LOGE("WebGL vertexAttribPointer invalid offset %{public}u", vertexInfo->offset);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    glVertexAttribPointer(vertexInfo->index, vertexInfo->size, vertexInfo->type, vertexInfo->normalized,
        vertexInfo->stride, reinterpret_cast<GLvoid *>(vertexInfo->offset));
    LOGI("WebGL vertexAttribPointer index %{public}u offset %{public}u", vertexInfo->index, vertexInfo->offset);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GetVertexAttribOffset(napi_env env, GLuint index, GLenum pname)
{
    LOGI("WebGL getVertexAttribOffset index %{public}u %{public}u", index, pname);
    if (pname != WebGLRenderingContextBase::VERTEX_ATTRIB_ARRAY_POINTER) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateInt64(env, 0).val_;
    }
    GLvoid *point = nullptr;
    glGetVertexAttribPointerv(index, pname, &point);
    LOGI("WebGL getVertexAttribOffset point %{public}p", point);
    return NVal::CreateInt64(env, reinterpret_cast<int64_t>(point)).val_;
}

napi_value WebGLRenderingContextBaseImpl::EnableVertexAttribArray(napi_env env, int64_t index)
{
    if (index < 0 || index >= GetMaxVertexAttribs()) {
        LOGE("WebGL enableVertexAttribArray invalid index %{public}ld", index);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGI("WebGL enableVertexAttribArray index %{public}u", index);
    glEnableVertexAttribArray(index);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DisableVertexAttribArray(napi_env env, int64_t index)
{
    if (index < 0 || index >= GetMaxVertexAttribs()) {
        LOGE("WebGL disableVertexAttribArray invalid index %{public}ld", index);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGI("WebGL disableVertexAttribArray index %{public}u", index);
    glDisableVertexAttribArray(index);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::IsEnabled(napi_env env, GLenum cap)
{
    LOGI("WebGL isEnabled cap %{public}u ", cap);
    bool result = false;
    if (!CheckCap(env, cap)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateBool(env, result).val_;
    }
    result = static_cast<bool>(glIsEnabled(cap));
    LOGI("WebGL isEnabled cap %{public}u result %{public}u", cap, result);
    return NVal::CreateBool(env, result).val_;
}

napi_value WebGLRenderingContextBaseImpl::Disable(napi_env env, GLenum cap)
{
    LOGI("WebGL disable cap %{public}u ", cap);
    if (!CheckCap(env, cap)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    glDisable(cap);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::Enable(napi_env env, GLenum cap)
{
    LOGI("WebGL enable cap %{public}u ", cap);
    if (!CheckCap(env, cap)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    glEnable(cap);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BindAttribLocation(napi_env env, napi_value program,
    GLuint index, const std::string &name)
{
    LOGE("WebGL bindAttribLocation index %{public}u name %{public}s", index, name.c_str());
    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, program);
    if (webGLProgram == nullptr) {
        LOGI("WebGL bindAttribLocation invalid object");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    uint32_t programId = webGLProgram->GetProgramId();

    if (index >= GetMaxVertexAttribs()) {
        LOGE("WebGL bindAttribLocation invalid index %{public}ld", index);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    if (name.size() > MAX_LOCATION_LENGTH) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (!CheckString(name)) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (CheckReservedPrefix(name)) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    LOGI("WebGL bindAttribLocation programId %{public}d index %{public}u", programId, index);
    glBindAttribLocation(static_cast<GLuint>(programId), index, const_cast<char *>(name.c_str()));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GenerateMipmap(napi_env env, GLenum target)
{
    LOGI("WebGL generateMipmap target %{public}u", target);
    if (!CheckGLenum(env, target, {
        WebGLRenderingContextBase::TEXTURE_2D,
        WebGLRenderingContextBase::TEXTURE_CUBE_MAP
    }, {
        WebGL2RenderingContextBase::TEXTURE_3D,
        WebGL2RenderingContextBase::TEXTURE_2D_ARRAY
    })) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    WebGLTexture *webGLTexture = GetBoundTexture(env, target, false);
    if (webGLTexture == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glGenerateMipmap(target);
    return nullptr;
}

GLenum WebGLRenderingContextBaseImpl::GetError_()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        lastError_ = err;
    }
    return lastError_;
}

napi_value WebGLRenderingContextBaseImpl::GetError(napi_env env)
{
    GLenum err = GetError_();
    LOGE("getError error %{public}u", err);
    lastError_ = GL_NO_ERROR;
    return NVal::CreateInt64(env, static_cast<int64_t>(err)).val_;
}

napi_value WebGLRenderingContextBaseImpl::UniformXf(napi_env env, napi_value locationObj,
    uint32_t count, const GLfloat *data)
{
    WebGLUniformLocation *webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniformXf can not find uniform");
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    LOGI("WebGL uniform location %{public}d [%{public}f %{public}f %{public}f %{public}f]",
        location, data[0], data[1], data[2], data[3]); // 0, 1, 2, 3 are array index
    switch (count) {
        case PARAMETERS_NUM_1:
            glUniform1f(location, data[0]);
            return nullptr;
        case PARAMETERS_NUM_2:
            glUniform2f(location, data[0], data[1]);
            return nullptr;
        case PARAMETERS_NUM_3:
            glUniform3f(location, data[0], data[1], data[2]); // 0, 1, 2 are array index
            return nullptr;
        case PARAMETERS_NUM_4:
            glUniform4f(location, data[0], data[1], data[2], data[3]); // 0, 1, 2, 3 are array index
            return nullptr;
        default:
            break;
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::UniformXi(napi_env env, napi_value locationObj, uint32_t count,
    const GLint *data)
{
    WebGLUniformLocation *webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniformXf can not find uniform");
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    LOGI("WebGL uniform location %{public}d [%{public}d %{public}d %{public}d %{public}d]",
        location, data[0], data[1], data[2], data[3]); // 0, 1, 2, 3 are array index
    switch (count) {
        case PARAMETERS_NUM_1:
            glUniform1i(location, data[0]);
            return nullptr;
        case PARAMETERS_NUM_2:
            glUniform2i(location, data[0], data[1]);
            return nullptr;
        case PARAMETERS_NUM_3:
            glUniform3i(location, data[0], data[1], data[2]); // 0, 1, 2 are array index
            return nullptr;
        case PARAMETERS_NUM_4:
            glUniform4i(location, data[0], data[1], data[2], data[3]); // 0, 1, 2, 3 are array index
            return nullptr;
        default:
            break;
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::Uniformfv(napi_env env, napi_value locationObj, napi_value data,
    const UniformExtInfo *info)
{
    WebGLUniformLocation *webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniformfv can not find uniform");
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    WebGLReadBufferArg readData(env);
    napi_status status = readData.GenBufferData(data, BUFFER_DATA_FLOAT_32);
    if (status != napi_ok) {
        LOGE("WebGL uniformfv failed to getbuffer data");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    readData.DumpBuffer(readData.GetBufferDataType());

    if (readData.GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
        LOGE("WebGL uniformfv not support data type %{public}d", readData.GetBufferDataType());
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLsizei count = readData.GetBufferLength() / sizeof(float);
    LOGI("WebGL uniformfv count = %{public}u length %{public}u", count, readData.GetBufferLength());
    if (count < info->dimension || (count % info->dimension)) {
        LOGE("WebGL uniformfv invalid value, count %{public}d", count);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    switch (info->dimension) {
        case PARAMETERS_NUM_1:
            glUniform1fv(location, 1, reinterpret_cast<GLfloat*>(readData.GetBuffer()));
            return nullptr;
        case PARAMETERS_NUM_2:
            glUniform2fv(location, count >> 1, reinterpret_cast<GLfloat*>(readData.GetBuffer()));
            return nullptr;
        case PARAMETERS_NUM_3:
            glUniform3fv(location, count / WebGLArg::UNIFORM_3V_REQUIRE_MIN_SIZE,
                reinterpret_cast<GLfloat*>(readData.GetBuffer()));
            return nullptr;
        case PARAMETERS_NUM_4:
            glUniform4fv(location, count / WebGLArg::UNIFORM_4V_REQUIRE_MIN_SIZE,
                reinterpret_cast<GLfloat*>(readData.GetBuffer()));
            return nullptr;
        default:
            break;
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::Uniformiv(napi_env env, napi_value locationObj, napi_value data,
    const UniformExtInfo *info)
{
    WebGLUniformLocation *webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniformXf can not find uniform");
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    WebGLReadBufferArg readData(env);
    napi_status status = readData.GenBufferData(data, BUFFER_DATA_INT_32);
    if (status != napi_ok) {
        LOGE("WebGL uniformfv failed to getbuffer data");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    readData.DumpBuffer(readData.GetBufferDataType());

    if (readData.GetBufferDataType() != BUFFER_DATA_INT_32) {
        LOGE("WebGL uniformfv not support data type %{public}d", readData.GetBufferDataType());
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLsizei count = readData.GetBufferLength() / sizeof(float);
    LOGI("WebGL uniformfv count = %{public}u length %{public}u", count, readData.GetBufferLength());
    if (count < info->dimension || (count % info->dimension)) {
        LOGE("WebGL uniformfv invalid value, count %{public}d", count);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    switch (info->dimension) {
        case PARAMETERS_NUM_1:
            glUniform1iv(location, 1, reinterpret_cast<GLint*>(readData.GetBuffer()));
            return nullptr;
        case PARAMETERS_NUM_2:
            glUniform2iv(location, count / WebGLArg::UNIFORM_2V_REQUIRE_MIN_SIZE,
                reinterpret_cast<GLint*>(readData.GetBuffer()));
            return nullptr;
        case PARAMETERS_NUM_3:
            glUniform3iv(location, count / WebGLArg::UNIFORM_3V_REQUIRE_MIN_SIZE,
                reinterpret_cast<GLint*>(readData.GetBuffer()));
            return nullptr;
        case PARAMETERS_NUM_4:
            glUniform4iv(location, count / WebGLArg::UNIFORM_4V_REQUIRE_MIN_SIZE,
                reinterpret_cast<GLint*>(readData.GetBuffer()));
            return nullptr;
        default:
            break;
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::UniformMatrixfv(napi_env env,
    napi_value locationObj, napi_value data, GLboolean transpose, const UniformExtInfo *info)
{
    WebGLUniformLocation *webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniformMatrixfv can not find uniform");
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    WebGLReadBufferArg readData(env);
    napi_status status = readData.GenBufferData(data, BUFFER_DATA_FLOAT_32);
    if (status != napi_ok) {
        LOGE("WebGL uniformMatrixfv failed to getbuffer data");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    readData.DumpBuffer(readData.GetBufferDataType());

    if (readData.GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
        LOGE("WebGL uniformMatrixfv not support data type %{public}d", readData.GetBufferDataType());
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLsizei count = readData.GetBufferLength() / sizeof(float);
    LOGI("WebGL uniformMatrixfv count = %{public}u length %{public}u", count, readData.GetBufferLength());
    if (count < info->dimension || (count % info->dimension)) {
        LOGE("WebGL uniformMatrixfv invalid value, count %{public}d", count);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    switch (info->dimension) {
        case WebGLArg::MATRIX_2X2_REQUIRE_MIN_SIZE:
            glUniformMatrix2fv(location, count >> WebGLArg::UNIFORM_2V_REQUIRE_MIN_SIZE, transpose,
                reinterpret_cast<GLfloat *>(readData.GetBuffer()));
            return nullptr;
        case WebGLArg::MATRIX_3X3_REQUIRE_MIN_SIZE:
            glUniformMatrix3fv(location, count >> WebGLArg::UNIFORM_3V_REQUIRE_MIN_SIZE, transpose,
                reinterpret_cast<GLfloat *>(readData.GetBuffer()));
            return nullptr;
        case WebGLArg::MATRIX_4X4_REQUIRE_MIN_SIZE:
            glUniformMatrix4fv(location, count >> WebGLArg::UNIFORM_4V_REQUIRE_MIN_SIZE, transpose,
                reinterpret_cast<GLfloat *>(readData.GetBuffer()));
            return nullptr;
        default:
            return nullptr;
    }
    return nullptr;
}

bool WebGLRenderingContextBaseImpl::CheckGLenum(napi_env env, GLenum type,
    const std::vector<GLenum> &glSupport, const std::vector<GLenum> &g2Support)
{
    auto it = std::find(glSupport.begin(), glSupport.end(), type);
    if (it != glSupport.end()) {
        return true;
    }
    if (!IsHighWebGL()) {
        return false;
    }
    auto it2 = std::find(g2Support.begin(), g2Support.end(), type);
    if (it2 != g2Support.end()) {
        return true;
    }
    return false;
}

bool WebGLRenderingContextBaseImpl::CheckCap(napi_env env, GLenum cap)
{
    return CheckGLenum(env, cap, {
        WebGLRenderingContextBase::BLEND,
        WebGLRenderingContextBase::CULL_FACE,
        WebGLRenderingContextBase::DEPTH_TEST,
        WebGLRenderingContextBase::DITHER,
        WebGLRenderingContextBase::POLYGON_OFFSET_FILL,
        WebGLRenderingContextBase::SAMPLE_ALPHA_TO_COVERAGE,
        WebGLRenderingContextBase::SAMPLE_COVERAGE,
        WebGLRenderingContextBase::SCISSOR_TEST,
        WebGLRenderingContextBase::STENCIL_TEST
    }, {
        WebGL2RenderingContextBase::RASTERIZER_DISCARD
    });
}

bool WebGLRenderingContextBaseImpl::CheckInternalFormat(napi_env env, GLenum internalFormat)
{
    static const std::vector<GLenum> glSupport = {
        WebGLRenderingContextBase::RGBA4,
        WebGLRenderingContextBase::RGB565,
        WebGLRenderingContextBase::RGB5_A1,
        WebGLRenderingContextBase::DEPTH_COMPONENT16,
        WebGLRenderingContextBase::STENCIL_INDEX8,
        WebGLRenderingContextBase::DEPTH_STENCIL
    };
    static const std::vector<GLenum> gl2Support = {
        WebGL2RenderingContextBase::R8,
        WebGL2RenderingContextBase::R8UI,
        WebGL2RenderingContextBase::R8I,
        WebGL2RenderingContextBase::R16UI,
        WebGL2RenderingContextBase::R16I,
        WebGL2RenderingContextBase::R32UI,
        WebGL2RenderingContextBase::R32I,

        WebGL2RenderingContextBase::RG8,
        WebGL2RenderingContextBase::RG8UI,
        WebGL2RenderingContextBase::RG8I,
        WebGL2RenderingContextBase::RG16UI,
        WebGL2RenderingContextBase::RG16I,
        WebGL2RenderingContextBase::RG32UI,
        WebGL2RenderingContextBase::RG32I,

        WebGL2RenderingContextBase::RGB8,
        WebGL2RenderingContextBase::RGBA8,
        WebGL2RenderingContextBase::SRGB8_ALPHA8,
        WebGL2RenderingContextBase::RGB10_A2,
        WebGL2RenderingContextBase::RGBA8UI,
        WebGL2RenderingContextBase::RGBA8I,
        WebGL2RenderingContextBase::RGB10_A2UI,

        WebGL2RenderingContextBase::RGBA16UI,
        WebGL2RenderingContextBase::RGBA16I,
        WebGL2RenderingContextBase::RGBA32I,
        WebGL2RenderingContextBase::RGBA32UI,
        WebGL2RenderingContextBase::DEPTH_COMPONENT24,
        WebGL2RenderingContextBase::DEPTH_COMPONENT32F,
        WebGL2RenderingContextBase::DEPTH24_STENCIL8,
        WebGL2RenderingContextBase::DEPTH32F_STENCIL8
    };
    return CheckGLenum(env, internalFormat, glSupport, gl2Support);
}

bool WebGLRenderingContextBaseImpl::CheckAttachment(napi_env env, GLenum attachment)
{
    static const std::vector<GLenum> glSupport = {
        WebGLRenderingContextBase::COLOR_ATTACHMENT0,
        WebGLRenderingContextBase::DEPTH_ATTACHMENT,
        WebGLRenderingContextBase::STENCIL_ATTACHMENT,
        WebGLRenderingContextBase::DEPTH_STENCIL_ATTACHMENT
    };

    static const std::vector<GLenum> gl2Support = {
        WebGL2RenderingContextBase::COLOR_ATTACHMENT1,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT2,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT3,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT4,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT5,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT6,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT7,

        WebGL2RenderingContextBase::COLOR_ATTACHMENT8,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT9,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT10,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT11,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT12,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT13,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT14,
        WebGL2RenderingContextBase::COLOR_ATTACHMENT15
    };
    return CheckGLenum(env, attachment, glSupport, gl2Support);
}

WebGLTexture* WebGLRenderingContextBaseImpl::GetBoundTexture(napi_env env, GLenum target, bool cubeMapExt)
{
    WebGLTexture* texture = nullptr;
    switch (target) {
        case WebGLRenderingContextBase::TEXTURE_2D:
            if (activeTextureIndex >= boundTexture2D.size()) {
                return nullptr;
            }
            texture = GetObjectInstance<WebGLTexture>(env, boundTexture2D[activeTextureIndex]);
            break;
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_X:
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_X:
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Y:
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Z:
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Z:
            if (!cubeMapExt) {
                return nullptr;
            }
            if (activeTextureIndex >= boundTextureCubeMap.size()) {
                return nullptr;
            }
            texture = GetObjectInstance<WebGLTexture>(env, boundTextureCubeMap[activeTextureIndex]);
            break;
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP:
            if (cubeMapExt) {
                return nullptr;
            }
            if (activeTextureIndex >= boundTextureCubeMap.size()) {
                return nullptr;
            }
            texture = GetObjectInstance<WebGLTexture>(env, boundTextureCubeMap[activeTextureIndex]);
            break;
        default:
            return nullptr;
    }
    return texture;
}

WebGLFramebuffer *WebGLRenderingContextBaseImpl::GetBoundFrameBuffer(napi_env env, GLenum target)
{
    WebGLFramebuffer *webGLFrameBuffer = nullptr;
    switch (target) {
        case WebGLRenderingContextBase::FRAMEBUFFER:
            webGLFrameBuffer = GetObjectInstance<WebGLFramebuffer>(env, boundFrameBufferId);
            break;
        case WebGL2RenderingContextBase::READ_FRAMEBUFFER:
            if (!IsHighWebGL()) {
                break;
            }
            webGLFrameBuffer = GetObjectInstance<WebGLFramebuffer>(env, boundFrameBufferId);
            break;
        default:
            return nullptr;
    }
    return webGLFrameBuffer;
}

bool WebGLRenderingContextBaseImpl::CheckDrawMode(napi_env env, GLenum mode)
{
    return CheckGLenum(env, mode, {
        WebGLRenderingContextBase::POINTS,
        WebGLRenderingContextBase::LINE_STRIP,
        WebGLRenderingContextBase::LINE_LOOP,
        WebGLRenderingContextBase::LINES,
        WebGLRenderingContextBase::TRIANGLE_STRIP,
        WebGLRenderingContextBase::TRIANGLE_FAN,
        WebGLRenderingContextBase::TRIANGLES,

    }, {});
}

bool WebGLRenderingContextBaseImpl::CheckFrameBufferTarget(napi_env env, GLenum target)
{
    return CheckGLenum(env, target, {
        WebGLRenderingContextBase::FRAMEBUFFER
    }, {
        WebGL2RenderingContextBase::DRAW_FRAMEBUFFER,
        WebGL2RenderingContextBase::READ_FRAMEBUFFER
    });
}

bool WebGLRenderingContextBaseImpl::CheckRenderBufferTarget(napi_env env, GLenum target)
{
    return CheckGLenum(env, target, {
        WebGLRenderingContextBase::RENDERBUFFER
    }, {
    });
}

bool WebGLRenderingContextBaseImpl::CheckTexture2DTarget(napi_env env, GLenum target)
{
    return CheckGLenum(env, target, {
        WebGLRenderingContextBase::TEXTURE_2D,
        WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_X,
        WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_X,
        WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Y,
        WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Y,
        WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Z,
        WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Z
    }, {
    });
}


bool WebGLRenderingContextBaseImpl::CheckBufferDataUsage(napi_env env, GLenum usage)
{
    return CheckGLenum(env, usage, {
        GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW
    }, {
    });
}

bool WebGLRenderingContextBaseImpl::CheckPixelsFormat(napi_env env, GLenum format)
{
    return CheckGLenum(env, format, {
        GL_ALPHA, GL_RGB, GL_RGBA
    }, {
    });
}

bool WebGLRenderingContextBaseImpl::CheckPixelsType(napi_env env, GLenum type)
{
    return CheckGLenum(env, type, {
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT_5_6_5,
        GL_UNSIGNED_SHORT_4_4_4_4,
        GL_UNSIGNED_SHORT_5_5_5_1,
        GL_FLOAT, GL_HALF_FLOAT_OES
    }, {
    });
}

GLuint WebGLRenderingContextBaseImpl::GetMaxVertexAttribs()
{
    static GLuint maxVertexAttribs = 0;
    if (maxVertexAttribs == 0) {
        GLint max = 0;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);
        maxVertexAttribs = static_cast<GLuint>(max);
        LOGI("WebGL maxVertexAttribs %{public}u", maxVertexAttribs);
    }
    return maxVertexAttribs;
}

bool WebGLRenderingContextBaseImpl::CheckString(const std::string& str)
{
    for (size_t i = 0; i < str.length(); ++i) {
        unsigned char c = str[i];
        // Horizontal tab, line feed, vertical tab, form feed, carriage return are also valid
        if (c >= 9 && c <= 13) { // 9, 13 is range
            continue;
        }
        // Printing characters are valid except " $ ` @ \ ' DEL.
        if (isprint(c) && c != '"' && c != '$' && c != '`' && c != '@' && c != '\\' && c != '\'') {
            continue;
        }
        return false;
    }
    return true;
}

bool WebGLRenderingContextBaseImpl::CheckReservedPrefix(const std::string& name)
{
    if (strncmp("webgl_", name.c_str(), 6) == 0 || strncmp("_webgl_", name.c_str(), 7) == 0) { // 6, 7 is name length
        return true;
    }
    return false;
}

GLint WebGLRenderingContextBaseImpl::GetMaxTextureLevelForTarget(GLenum target)
{
    switch (target) {
        case GL_TEXTURE_2D:
            return maxTextureLevel;
        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            return maxCubeMapTextureLevel;
        default:
            break;
    }
    return 0;
}

bool WebGLRenderingContextBaseImpl::CheckTextureTargetAndLevel(GLenum target, GLint level)
{
    if ((level < 0) || level > GetMaxTextureLevelForTarget(target)) {
        return false;
    }
    return true;
}

GLenum WebGLRenderingContextBaseImpl::CheckTexImage2D(napi_env env, const Image2DArg *imgArg)
{
    if (!CheckTextureTargetAndLevel(imgArg->target, imgArg->level)) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    WebGLTexture* texture = GetBoundTexture(env, imgArg->target, true);
    if (!texture) {
        LOGE("Can not find texture ");
        return WebGLRenderingContextBase::INVALID_ENUM;
    }

    if (imgArg->xoffset < 0 || imgArg->yoffset < 0 ||
        imgArg->x < 0 || imgArg->y < 0 || imgArg->width < 0 || imgArg->height < 0) {
        LOGE("Invalid offset or size ");
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (imgArg->func == IMAGE_2D_FUNC_TEXT_IMAGE_2D ||
        imgArg->func == IMAGE_2D_FUNC_COMPRESSED_TEX_IMAGE_2D ||
        imgArg->func == IMAGE_2D_FUNC_COPY_TEX_IMAGE_2D) {
    } else {
        if (imgArg->xoffset + imgArg->width < 0 || imgArg->yoffset + imgArg->height < 0) {
            return WebGLRenderingContextBase::INVALID_VALUE;
        }
    }
    return WebGLRenderingContextBase::NO_ERROR;
}
} // namespace Impl
} // namespace Rosen
} // namespace OHOS
