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
#include "context/webgl_rendering_context_base_impl.h"

#include "context/webgl_rendering_context_base.h"
#include "context/webgl2_rendering_context_base.h"
#include "napi/n_class.h"
#include "util/log.h"
#include "util/object_source.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;
#define SET_ERROR(error) \
    do {                      \
        LOGE("WebGL error code %{public}u", error);    \
        SetError(error);      \
    } while (0)

#define SET_ERROR_WITH_LOG(error, info, ...) \
    do {                      \
        LOGE("WebGL error code %{public}u" info, error, ##__VA_ARGS__);    \
        SetError(error);      \
    } while (0)

WebGLRenderingContextBaseImpl::~WebGLRenderingContextBaseImpl() {}

void WebGLRenderingContextBaseImpl::Init()
{
    if (maxTextureImageUnits_ != 0) {
        return;
    }

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureImageUnits_);
    boundTexture_[BoundTextureType::TEXTURE_3D].resize(maxTextureImageUnits_);
    boundTexture_[BoundTextureType::TEXTURE_2D_ARRAY].resize(maxTextureImageUnits_);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize_);
    boundTexture_[BoundTextureType::TEXTURE_2D].resize(maxTextureSize_);

    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapTextureSize_);
    boundTexture_[BoundTextureType::TEXTURE_CUBE_MAP].resize(maxCubeMapTextureSize_);

    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize_);

    unpackColorspaceConversion_ = WebGLRenderingContextBase::BROWSER_DEFAULT_WEBGL;

    GLint max = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);
    maxVertexAttribs_ = static_cast<GLuint>(max);
    arrayVertexAttribs_.resize(maxVertexAttribs_);

    stencilMask_[0] = 0xffffffff;
    stencilMask_[1] = 0xffffffff;
    stencilFuncMask_[0] = 0xffffffff;
    stencilFuncMask_[1] = 0xffffffff;
    stencilFuncRef_[0] = 0;
    stencilFuncRef_[1] = 0;

    clearColor_[0] = 0;
    clearColor_[1] = 0;
    clearColor_[2] = 0;
    clearColor_[3] = 0;

    colorMask_[0] = false;
    colorMask_[1] = false;
    colorMask_[2] = false;
    colorMask_[3] = false;

    LOGI("WebGL Init maxTextureImageUnits %{public}u", maxTextureImageUnits_);
    LOGI("WebGL Init maxTextureSize %{public}u", maxTextureSize_);
    LOGI("WebGL Init maxCubeMapTextureSize %{public}u", maxCubeMapTextureSize_);
    LOGI("WebGL Init maxRenderBufferSize %{public}u", maxRenderBufferSize_);
    LOGI("WebGL Init maxVertexAttribs %{public}u", maxVertexAttribs_);
}

napi_value WebGLRenderingContextBaseImpl::GetContextAttributes(napi_env env)
{
    WebGLContextAttributes *webGlContextAttributes = webGLRenderingContext_->GetWebGlContextAttributes();
    if (webGlContextAttributes == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    NVal res = NVal::CreateObject(env);
    napi_value alpha = NVal::CreateBool(env, webGlContextAttributes->alpha_).val_;
    napi_value antialias = NVal::CreateBool(env, webGlContextAttributes->antialias_).val_;
    napi_value depth = NVal::CreateBool(env, webGlContextAttributes->depth_).val_;
    napi_value failIfMajorPerformanceCaveat =
        NVal::CreateBool(env, webGlContextAttributes->failIfMajorPerformanceCaveat_).val_;
    napi_value desynchronized = NVal::CreateBool(env, webGlContextAttributes->desynchronized_).val_;
    napi_value premultipliedAlpha = NVal::CreateBool(env, webGlContextAttributes->premultipliedAlpha_).val_;
    napi_value preserveDrawingBuffer = NVal::CreateBool(env, webGlContextAttributes->preserveDrawingBuffer_).val_;
    napi_value stencil = NVal::CreateBool(env, webGlContextAttributes->stencil_).val_;
    napi_value powerPreference = NVal::CreateUTF8String(env, webGlContextAttributes->powerPreference_).val_;
    res.AddProp("alpha", alpha);
    res.AddProp("antialias", antialias);
    res.AddProp("depth", depth);
    res.AddProp("failIfMajorPerformanceCaveat", failIfMajorPerformanceCaveat);
    res.AddProp("desynchronized", desynchronized);
    res.AddProp("premultipliedAlpha", premultipliedAlpha);
    res.AddProp("preserveDrawingBuffer", preserveDrawingBuffer);
    res.AddProp("stencil", stencil);
    res.AddProp("powerPreference", powerPreference);
    return res.val_;
}


napi_value WebGLRenderingContextBaseImpl::CreateTextureObject(napi_env env)
{
    WebGLTexture* webGlTexture = nullptr;
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
    if (texture < GL_TEXTURE0 || static_cast<GLint>(texture - GL_TEXTURE0) >= maxTextureImageUnits_) {
        LOGE("WebGL activeTexture texture unit out of range %{public}u", texture);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    activeTextureIndex_ = texture - GL_TEXTURE0;
    LOGI("WebGL activeTexture texture %{public}u", texture);
    glActiveTexture(texture);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BindTexture(napi_env env, GLenum target, napi_value textureRef)
{
    GLuint texture = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture* webGlTexture = WebGLTexture::GetObjectInstance(env, textureRef);
    if (webGlTexture != nullptr) {
        texture = webGlTexture->GetTexture();
        if (webGlTexture->GetTarget() && webGlTexture->GetTarget() != target) {
            LOGE("WebGL bindTexture textureId %{public}u has been bound", texture);
            SetError(WebGLRenderingContextBase::INVALID_OPERATION);
            return nullptr;
        }
    }
    LOGI("WebGL bindTexture target %{public}u textureId %{public}u activeTextureIndex %{public}u",
        target, texture, activeTextureIndex_);

    uint32_t index = 0;
    if (!WebGLRenderingContextBaseImpl::CheckTextureTarget(env, target, index)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (activeTextureIndex_ >= boundTexture_[index].size()) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (boundTexture_[index][activeTextureIndex_] && boundTexture_[index][activeTextureIndex_] != texture) {
        LOGE("WebGL bindTexture textureId %{public}u", boundTexture_[index][activeTextureIndex_]);
    }
    boundTexture_[index][activeTextureIndex_] = texture;

    glBindTexture(target, texture);
    if (webGlTexture) {
        webGlTexture->SetTarget(target);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::IsBuffer(napi_env env, napi_value object)
{
    WebGLBuffer* webGlBuffer = WebGLBuffer::GetObjectInstance(env, object);
    if (webGlBuffer == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    unsigned int buffer = webGlBuffer->GetBufferId();
    bool result = static_cast<bool>(glIsBuffer(static_cast<GLuint>(buffer)));
    LOGI("WebGL isBuffer res %{public}u  %{public}u", result, buffer);
    return NVal::CreateBool(env, result).val_;
}

napi_value WebGLRenderingContextBaseImpl::BindBuffer(napi_env env, GLenum target, napi_value object)
{
    LOGI("WebGL bindBuffer target %{public}u ", target);
    // support default value
    uint32_t bufferId = WebGLBuffer::DEFAULT_BUFFER;
    WebGLBuffer* webGlBuffer = WebGLBuffer::GetObjectInstance(env, object);
    if (webGlBuffer != nullptr && !webGlBuffer->HasBound()) {
        LOGE("WebGL bindBuffer webGlBuffer %{public}p", webGlBuffer);
        bufferId = webGlBuffer->GetBufferId();
    }
    LOGI("WebGL bindBuffer target %{public}u bufferId %{public}u", target, bufferId);
    uint32_t index = BoundBufferType::ARRAY_BUFFER;
    if (!CheckBufferTarget(env, target, index)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (webGlBuffer && webGlBuffer->GetTarget() && webGlBuffer->GetTarget() != target) {
        LOGE("WebGL bindBuffer invalid target %{public}u", webGlBuffer->GetTarget());
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (boundBufferIds_[index] && boundBufferIds_[index] != bufferId) {
        LOGE("WebGL bindBuffer has been bound bufferId %{public}u", boundBufferIds_[index]);
    }
    boundBufferIds_[index] = bufferId;

    glBindBuffer(target, static_cast<GLuint>(bufferId));
    // save bufferId
    if (webGlBuffer) {
        webGlBuffer->SetTarget(target);
        webGlBuffer->SetBound(true);
    }
    LOGI("WebGL bindBuffer bufferId %{public}u %{public}u", bufferId, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::IsFrameBuffer(napi_env env, napi_value object)
{
    GLuint frameBufferId = WebGLFramebuffer::DEFAULT_FRAME_BUFFER;
    WebGLFramebuffer* webGlFramebuffer = WebGLFramebuffer::GetObjectInstance(env, object);
    if (webGlFramebuffer == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    frameBufferId = webGlFramebuffer->GetFramebuffer();
    LOGI("WebGL isFramebuffer framebuffer %{public}u", frameBufferId);
    bool res = static_cast<bool>(glIsFramebuffer(frameBufferId));
    LOGI("WebGL isFramebuffer framebuffer %{public}u result %{public}u", frameBufferId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBaseImpl::BindFrameBuffer(napi_env env, GLenum target, napi_value object)
{
    LOGI("WebGL bindFramebuffer target %{public}u ", target);
    uint32_t index = 0;
    if (!CheckFrameBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "BindFrameBuffer Invalid target");
        return nullptr;
    }

    GLuint frameBufferId = WebGLFramebuffer::DEFAULT_FRAME_BUFFER;
    WebGLFramebuffer* webGlFramebuffer = WebGLFramebuffer::GetObjectInstance(env, object);
    if (webGlFramebuffer != nullptr && !webGlFramebuffer->HasBound()) {
        frameBufferId = webGlFramebuffer->GetFramebuffer();
    }
    LOGI("WebGL bindFramebuffer framebuffer %{public}u", frameBufferId);
    if (boundFrameBufferIds_[index] && boundFrameBufferIds_[index] != frameBufferId) {
        LOGE("WebGL bindFramebuffer has been bound %{public}u", boundFrameBufferIds_[index]);
    }
    glBindFramebuffer(target, frameBufferId);

    boundFrameBufferIds_[index] = frameBufferId;
    if (IsHighWebGL()) {
        if (target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER) {
            boundFrameBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER] = frameBufferId;
        }
    }
    if (target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER) {
        boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER] = frameBufferId;
    }

    if (webGlFramebuffer) {
        webGlFramebuffer->SetBound(true);
        webGlFramebuffer->SetTarget(target);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BindRenderBuffer(napi_env env, GLenum target, napi_value object)
{
    LOGI("WebGL bindRenderbuffer target %{public}u", target);
    uint32_t index = 0;
    if (!CheckRenderBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "BindRenderBuffer Invalid target");
        return nullptr;
    }

    GLuint renderbuffer = WebGLRenderbuffer::DEFAULT_RENDER_BUFFER;
    WebGLRenderbuffer* webGlRenderbuffer = WebGLRenderbuffer::GetObjectInstance(env, object);
    if (webGlRenderbuffer != nullptr && !webGlRenderbuffer->HasBound()) {
        renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    }

    LOGI("WebGL bindRenderbuffer target %{public}u renderbuffer %{public}u", target, renderbuffer);
    glBindRenderbuffer(target, renderbuffer);

    // record render buffer
    if (boundRenderBufferIds_[index] && boundRenderBufferIds_[index] != renderbuffer) {
        LOGE("WebGL bindRenderbuffer has been bound %{public}u", boundRenderBufferIds_[index]);
    }
    boundRenderBufferIds_[index] = renderbuffer;

    if (webGlRenderbuffer) {
        webGlRenderbuffer->SetTarget(target);
    }
    LOGE("WebGL bindRenderbuffer %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CreateBuffer(napi_env env)
{
    WebGLBuffer* webGlBuffer = nullptr;
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
    WebGLFramebuffer* webGlFramebuffer = nullptr;
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
    WebGLProgram* webGlProgram = nullptr;
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

    (void)AddObject<WebGLProgram>(env, programId, objProgram);
    LOGI("WebGL createProgram programId %{public}u result %{public}u", programId, GetError_());
    return objProgram;
}

napi_value WebGLRenderingContextBaseImpl::CreateRenderBuffer(napi_env env)
{
    WebGLRenderbuffer* webGlRenderbuffer = nullptr;
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
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "createShader Invalid type");
        return NVal::CreateNull(env).val_;
    }
    WebGLShader* webGlShader = nullptr;
    napi_value objShader = WebGLShader::CreateObjectInstance(env, &webGlShader).val_;
    if (!webGlShader) {
        return NVal::CreateNull(env).val_;
    }
    GLuint shaderId = glCreateShader(type);
    webGlShader->SetShaderId(shaderId);
    LOGI("WebGL createShader shaderId = %{public}u type %{public}u", shaderId, type);
    if (shaderId == 0) {
        LOGE("WebGL createShader failed. type %40{public}x", type);
        return NVal::CreateNull(env).val_;
    }
    (void)AddObject<WebGLShader>(env, shaderId, objShader);
    webGlShader->SetShaderType(type);
    return objShader;
}

napi_value WebGLRenderingContextBaseImpl::AttachShader(napi_env env, napi_value programObj, napi_value shaderObj)
{
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        LOGE("WebGL attachShader invalid program");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    uint32_t programId = webGLProgram->GetProgramId();

    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, shaderObj);
    if (webGlShader == nullptr) {
        LOGE("WebGL attachShader invalid shader");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGI("WebGL attachShader programId %{public}u, shaderId %{public}u", programId, shaderId);
    uint32_t index = 0;
    if (!CheckShaderType(env, webGlShader->GetShaderType(), index) || !webGLProgram->AttachShader(index, shaderId)) {
        LOGE("WebGL attachShader invalid shader");
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glAttachShader(static_cast<GLuint>(programId), static_cast<GLuint>(shaderId));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DetachShader(napi_env env, napi_value programObj, napi_value shaderObj)
{
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    uint32_t programId = webGLProgram->GetProgramId();

    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, shaderObj);
    if (webGlShader == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGI("WebGL detachShader programId %{public}u, shaderId %{public}u", programId, shaderId);
    uint32_t index = 0;
    if (!CheckShaderType(env, webGlShader->GetShaderType(), index) || !webGLProgram->DetachShader(index, shaderId)) {
        LOGE("Invalid shader");
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glDetachShader(static_cast<GLuint>(programId), static_cast<GLuint>(shaderId));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompileShader(napi_env env, napi_value object)
{
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, object);
    if (webGlShader == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    glCompileShader(static_cast<GLuint>(shaderId));
    LOGI("WebGL compileShader shaderId %{public}u result %{public}u", shaderId, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ShaderSource(napi_env env, napi_value object, const std::string& source)
{
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, object);
    if (webGlShader == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGI("WebGL shaderSource shaderId %{public}u  source: '%{public}s'", shaderId, source.c_str());
    if (!CheckString(source)) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLint length = static_cast<GLint>(source.size());
    GLchar* str = const_cast<GLchar*>(source.c_str());
    glShaderSource(static_cast<GLuint>(shaderId), 1, &str, &length);
    webGlShader->SetShaderRes(source);
    LOGI("WebGL shaderSource shaderId %{public}u result %{public}u", shaderId, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DeleteBuffer(napi_env env, napi_value object)
{
    LOGI("WebGL deleteBuffer ");
    WebGLBuffer* webGlBuffer = WebGLObject::GetObjectInstance<WebGLBuffer>(env, object);
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

    uint32_t index = 0;
    if (!CheckBufferTarget(env, webGlBuffer->GetTarget(), index)) {
        return nullptr;
    }
    boundBufferIds_[index] = (boundBufferIds_[index] == buffer) ? 0 : boundBufferIds_[index];
    LOGI("WebGL deleteBuffer  error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DeleteFrameBuffer(napi_env env, napi_value object)
{
    LOGI("WebGL deleteFramebuffer ");
    WebGLFramebuffer* webGlFramebuffer = WebGLObject::GetObjectInstance<WebGLFramebuffer>(env, object);
    if (webGlFramebuffer == nullptr) {
        return nullptr;
    }
    GLuint frameBufferId = webGlFramebuffer->GetFramebuffer();
    DeleteObject<WebGLFramebuffer>(env, frameBufferId);
    glDeleteFramebuffers(1, &frameBufferId);

    LOGI("WebGL deleteFramebuffer framebufferId %{public}u", frameBufferId);
    if (!IsHighWebGL()) {
        if (boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER] == frameBufferId) {
            boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER] = 0;
        }
        return nullptr;
    }

    GLenum target = 0;
    if (frameBufferId == boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER]) {
        if (frameBufferId == boundFrameBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER]) {
            target = GL_FRAMEBUFFER;
            boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER] = 0;
            boundFrameBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER] = 0;
        } else {
            target = GL_DRAW_FRAMEBUFFER;
            boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER] = 0;
        }
    } else if (frameBufferId == boundFrameBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER]) {
        target = GL_READ_FRAMEBUFFER;
        boundFrameBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER] = 0;
    }
    LOGI("WebGL deleteFramebuffer  error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GetBufferParameter(napi_env env, GLenum target, GLenum pname)
{
    LOGI("WebGL getBufferParameter target %{public}u %{public}u", target, pname);
    uint32_t index = 0;
    if (!CheckBufferTarget(env, target, index)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    switch (pname) {
        case WebGLRenderingContextBase::BUFFER_USAGE:
        case WebGLRenderingContextBase::BUFFER_SIZE: {
            GLint value = 0;
            glGetBufferParameteriv(target, pname, &value);
            return NVal::CreateInt64(env, value).val_;
        }
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
}

napi_value WebGLRenderingContextBaseImpl::DeleteProgram(napi_env env, napi_value object)
{
    WebGLProgram* webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram == nullptr) {
        return nullptr;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    DeleteObject<WebGLProgram>(env, programId);
    glDeleteProgram(static_cast<GLuint>(programId));
    LOGI("WebGL deleteProgram programId %{public}u", programId);
    if (currentProgramId_ == programId) {
        currentProgramId_ = 0;
    } else if (currentProgramId_ != 0) {
        LOGI("WebGL deleteProgram  current programId %{public}u", currentProgramId_);
    }
    LOGI("WebGL deleteProgram  error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DeleteRenderBuffer(napi_env env, napi_value object)
{
    LOGI("WebGL deleteRenderbuffer ");
    WebGLRenderbuffer* webGlRenderbuffer = WebGLObject::GetObjectInstance<WebGLRenderbuffer>(env, object);
    if (webGlRenderbuffer == nullptr) {
        return nullptr;
    }

    uint32_t renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    WebGLFramebuffer* framebufferBinding = GetBoundFrameBuffer(env, WebGLRenderingContextBase::FRAMEBUFFER);
    if (framebufferBinding) {
        framebufferBinding->RemoveAttachment(renderbuffer, AttachmentType::RENDER_BUFFER);
    }
    framebufferBinding = GetBoundFrameBuffer(env, WebGL2RenderingContextBase::READ_FRAMEBUFFER);
    if (framebufferBinding != nullptr) {
        framebufferBinding->RemoveAttachment(renderbuffer, AttachmentType::RENDER_BUFFER);
    }
    DeleteObject<WebGLRenderbuffer>(env, renderbuffer);
    glDeleteRenderbuffers(1, &renderbuffer);
    LOGI("WebGL deleteRenderbuffer renderbuffer %{public}u", renderbuffer);
    uint32_t index = 0;
    if (!CheckRenderBufferTarget(env, webGlRenderbuffer->GetTarget(), index)) {
        return nullptr;
    }
    boundRenderBufferIds_[index] = 0;
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GetRenderBufferParameter(napi_env env, GLenum target, GLenum pname)
{
    uint32_t index = 0;
    LOGI("WebGL getRenderbufferParameter target %{public}u %{public}u", target, pname);
    if (!CheckRenderBufferTarget(env, target, index)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLRenderbuffer* renderBuffer = GetObjectInstance<WebGLRenderbuffer>(env, boundRenderBufferIds_[index]);
    if (renderBuffer == nullptr) {
        LOGE("WebGL getRenderbufferParameter can not get bound render buffer Id %{public}u",
            boundRenderBufferIds_[index]);
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    GLint params;
    switch (pname) {
        case WebGL2RenderingContextBase::RENDERBUFFER_SAMPLES:
            if (!IsHighWebGL()) {
                SetError(WebGLRenderingContextBase::INVALID_ENUM);
                return NVal::CreateNull(env).val_;
            }
            [[fallthrough]];
        case WebGLRenderingContextBase::RENDERBUFFER_WIDTH:
        case WebGLRenderingContextBase::RENDERBUFFER_HEIGHT:
        case WebGLRenderingContextBase::RENDERBUFFER_RED_SIZE:
        case WebGLRenderingContextBase::RENDERBUFFER_GREEN_SIZE:
        case WebGLRenderingContextBase::RENDERBUFFER_BLUE_SIZE:
        case WebGLRenderingContextBase::RENDERBUFFER_ALPHA_SIZE:
        case WebGLRenderingContextBase::RENDERBUFFER_DEPTH_SIZE:
        case WebGLRenderingContextBase::RENDERBUFFER_STENCIL_SIZE:
            glGetRenderbufferParameteriv(target, pname, &params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        case WebGLRenderingContextBase::RENDERBUFFER_INTERNAL_FORMAT:
            return NVal::CreateInt64(env, static_cast<int64_t>(renderBuffer->GetInternalFormat())).val_;
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
}

napi_value WebGLRenderingContextBaseImpl::DeleteShader(napi_env env, napi_value object)
{
    uint32_t shaderId = WebGLShader::DEFAULT_SHADER_ID;
    WebGLShader* webGlShader = WebGLObject::GetObjectInstance<WebGLShader>(env, object);
    if (webGlShader != nullptr) {
        shaderId = webGlShader->GetShaderId();
    }
    DeleteObject<WebGLShader>(env, shaderId);
    glDeleteShader(static_cast<GLuint>(shaderId));
    LOGI("WebGL deleteShader shaderId %{public}u", shaderId);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GetShaderParameter(napi_env env, napi_value object, GLenum pname)
{
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, object);
    if (webGlShader == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGI("WebGL getShaderParameter shaderId %{public}u", shaderId);

    GLint params;
    if (pname == WebGLRenderingContextBase::SHADER_TYPE) {
        glGetShaderiv(static_cast<GLuint>(shaderId), pname, &params);
        int64_t res = static_cast<int64_t>(params);
        return NVal::CreateInt64(env, res).val_;
    } else if (pname == WebGLRenderingContextBase::DELETE_STATUS ||
               pname == WebGLRenderingContextBase::COMPILE_STATUS) {
        glGetShaderiv(static_cast<GLuint>(shaderId), pname, &params);
        bool res = (params == GL_FALSE) ? false : true;
        return NVal::CreateBool(env, res).val_;
    }
    SetError(WebGLRenderingContextBase::INVALID_ENUM);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetShaderPrecisionFormat(
    napi_env env, GLenum shaderType, GLenum precisionType)
{
    LOGI("WebGL getShaderPrecisionFormat shaderType %{public}u precisionType %{public}u", shaderType, precisionType);
    uint32_t index = 0;
    if (!CheckShaderType(env, shaderType, index)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckGLenum(env, precisionType,
            { WebGLRenderingContextBase::LOW_FLOAT, WebGLRenderingContextBase::MEDIUM_FLOAT,
                WebGLRenderingContextBase::HIGH_FLOAT, WebGLRenderingContextBase::LOW_INT,
                WebGLRenderingContextBase::MEDIUM_INT, WebGLRenderingContextBase::HIGH_INT },
            {})) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLShaderPrecisionFormat* webGLShaderPrecisionFormat = nullptr;
    napi_value objShaderPrecisionFormat =
        WebGLShaderPrecisionFormat::CreateObjectInstance(env, &webGLShaderPrecisionFormat).val_;
    if (objShaderPrecisionFormat == nullptr) {
        LOGI("WebGL getShaderPrecisionFormat fail oshaderType %{public}u", shaderType);
        return NVal::CreateNull(env).val_;
    }

    GLint range[2] = { 0 };
    GLint precision = 0;
    glGetShaderPrecisionFormat(shaderType, precisionType, range, &precision);
    LOGI("WebGL getShaderPrecisionFormat [%{public}u %{public}u] precision %{public}u", range[0], range[1], precision);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatRangeMin(range[0]);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatRangeMax(range[1]);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatPrecision(precision);
    return objShaderPrecisionFormat;
}

napi_value WebGLRenderingContextBaseImpl::DeleteTexture(napi_env env, napi_value object)
{
    uint32_t texture = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture* webGlTexture = WebGLObject::GetObjectInstance<WebGLTexture>(env, object);
    if (webGlTexture != nullptr) {
        texture = webGlTexture->GetTexture();
    }
    LOGI("WebGL deleteTexture textureId %{public}u", texture);
    WebGLFramebuffer* framebufferBinding = GetBoundFrameBuffer(env, WebGLRenderingContextBase::FRAMEBUFFER);
    if (framebufferBinding) {
        framebufferBinding->RemoveAttachment(texture, AttachmentType::TEXTURE);
    }
    framebufferBinding = GetBoundFrameBuffer(env, WebGL2RenderingContextBase::READ_FRAMEBUFFER);
    if (framebufferBinding != nullptr) {
        framebufferBinding->RemoveAttachment(texture, AttachmentType::TEXTURE);
    }
    DeleteObject<WebGLTexture>(env, texture);
    glDeleteTextures(1, &texture);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GetFrameBufferAttachmentParameter(
    napi_env env, GLenum target, GLenum attachment, GLenum pname)
{
    LOGI("WebGL getFramebufferAttachmentParameter target %{public}u %{public}u %{public}u", target, attachment, pname);
    uint32_t index = 0;
    if (!CheckFrameBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "getFramebufferAttachmentParameter Invalid target");
        return NVal::CreateNull(env).val_;
    }
    if (!CheckAttachment(env, attachment)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }

    WebGLFramebuffer* frameBuffer = GetBoundFrameBuffer(env, WebGLRenderingContextBase::FRAMEBUFFER);
    if (frameBuffer == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLint type = 0;
    glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type);
    LOGI("WebGL getFramebufferAttachmentParameter type %{public}u", type);
    if ((type != WebGLRenderingContextBase::RENDERBUFFER) && (type != WebGLRenderingContextBase::TEXTURE)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    GLint params = 0;
    switch (pname) {
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:
            return NVal::CreateInt64(env, static_cast<int64_t>(type)).val_;
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME: {
            glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
            LOGI("WebGL getFramebufferAttachmentParameter objectId %{public}u", params);
            return (type == WebGLRenderingContextBase::RENDERBUFFER) ? GetObject<WebGLRenderbuffer>(env, params)
                                                                     : GetObject<WebGLTexture>(env, params);
        }
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER:
            if (!IsHighWebGL()) {
                break;
            }
        [[fallthrough]];
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL:
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE: {
            if (type == WebGLRenderingContextBase::RENDERBUFFER) {
                break;
            }
            glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
            LOGI("WebGL getFramebufferAttachmentParameter result %{public}u", params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        case GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE:
            if (attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
                SetError(WebGLRenderingContextBase::INVALID_OPERATION);
                return NVal::CreateNull(env).val_;
            }
        [[fallthrough]];
        case GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING: {
            if (!IsHighWebGL()) {
                break;
            }
            glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
            LOGI("WebGL getFramebufferAttachmentParameter result %{public}u", params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        default:
            break;
    }
    SetError(WebGLRenderingContextBase::INVALID_ENUM);
    LOGI("WebGL getFramebufferAttachmentParameter : no image is attached pname %{public}u %{public}u", pname, type);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::FrameBufferRenderBuffer(
    napi_env env, GLenum target, GLenum attachment, GLenum renderBufferTarget, napi_value object)
{
    LOGI("WebGL framebufferRenderbuffer target %{public}u %{public}u %{public}u", target, attachment,
        renderBufferTarget);
    uint32_t frameBufferTargetIdx = 0;
    if (!CheckFrameBufferTarget(env, target, frameBufferTargetIdx)) {
        LOGI("WebGL framebufferRenderbuffer invalid target %{public}u", target);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckAttachment(env, attachment)) {
        LOGI("WebGL framebufferRenderbuffer invalid attachment %{public}u", attachment);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    uint32_t renderBufferTargetIdx = 0;
    if (!CheckRenderBufferTarget(env, renderBufferTarget, renderBufferTargetIdx)) {
        LOGI("WebGL framebufferRenderbuffer invalid renderBufferTarget %{public}u", renderBufferTarget);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLFramebuffer* frameBuffer = GetObjectInstance<WebGLFramebuffer>(env, boundFrameBufferIds_[frameBufferTargetIdx]);
    if (frameBuffer == nullptr) {
        LOGI("WebGL framebufferRenderbuffer can not find bind frame buffer");
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    GLuint renderbuffer = WebGLRenderbuffer::DEFAULT_RENDER_BUFFER;
    WebGLRenderbuffer* webGlRenderbuffer = WebGLRenderbuffer::GetObjectInstance(env, object);
    if (webGlRenderbuffer != nullptr) {
        renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    }

    LOGI("WebGL framebufferRenderbuffer renderbuffer %{public}u", renderbuffer);
    if (attachment == WebGLRenderingContextBase::DEPTH_STENCIL_ATTACHMENT) {
        // TODO
        glFramebufferRenderbuffer(target, attachment, WebGLRenderingContextBase::DEPTH_ATTACHMENT, renderbuffer);
        LOGI("WebGL framebufferRenderbuffer renderbuffer %{public}u error %{public}u", renderbuffer, GetError_());
        glFramebufferRenderbuffer(target, attachment, WebGLRenderingContextBase::STENCIL_ATTACHMENT, renderbuffer);
        GetError(env);
    } else {
        glFramebufferRenderbuffer(target, attachment, renderBufferTarget, renderbuffer);
    }

    if (attachment == WebGLRenderingContextBase::DEPTH_STENCIL_ATTACHMENT && IsHighWebGL()) {
        frameBuffer->AddAttachment(WebGLRenderingContextBase::DEPTH_ATTACHMENT, renderbuffer);
        frameBuffer->AddAttachment(WebGLRenderingContextBase::STENCIL_ATTACHMENT, renderbuffer);
    } else {
        frameBuffer->AddAttachment(attachment, renderbuffer);
    }

    LOGI("WebGL framebufferRenderbuffer renderbuffer %{public}u error %{public}u", renderbuffer, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::FrameBufferTexture2D(
    napi_env env, GLenum target, GLenum attachment, GLenum textarget, napi_value texture, GLint level)
{
    LOGI("WebGL framebufferTexture2D target %{public}u %{public}u %{public}u", target, attachment, textarget);
    uint32_t index = 0;
    if (!CheckFrameBufferTarget(env, target, index)) {
        LOGI("WebGL framebufferTexture2D invalid target %{public}u", target);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckAttachment(env, attachment)) {
        LOGI("WebGL framebufferTexture2D invalid attachment %{public}u", attachment);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    // TODO
    if (!CheckTexture2DTarget(env, textarget)) {
        LOGI("WebGL framebufferTexture2D invalid textarget %{public}u", textarget);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLFramebuffer* frameBuffer = GetBoundFrameBuffer(env, target);
    if (frameBuffer == nullptr) {
        LOGI("WebGL framebufferTexture2D can not find bind frame buffer");
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    unsigned int textureId = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture* webGlTexture = WebGLTexture::GetObjectInstance(env, texture);
    if (webGlTexture != nullptr) {
        textureId = webGlTexture->GetTexture();
    }
    LOGI("WebGL framebufferTexture2D texture %{public}u", textureId);
    glFramebufferTexture2D(target, attachment, textarget, static_cast<GLuint>(textureId), level);

    frameBuffer->AddAttachment(attachment, static_cast<GLuint>(textureId), target, level);
    LOGI("WebGL framebufferTexture2D texture %{public}u result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GetProgramParameter(napi_env env, napi_value object, GLenum pname)
{
    uint32_t programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    programId = webGlProgram->GetProgramId();
    LOGI("WebGL getProgramParameter programId %{public}u pname %{public}u", programId, pname);
    switch (pname) {
        case WebGLRenderingContextBase::DELETE_STATUS:
        case WebGLRenderingContextBase::LINK_STATUS:
        case WebGLRenderingContextBase::VALIDATE_STATUS: {
            GLint params = 0;
            glGetProgramiv(static_cast<GLuint>(programId), pname, &params);
            LOGI("WebGL getProgramParameter params %{public}d", params);
            return NVal::CreateBool(env, params ? true : false).val_;
        }
        case WebGLRenderingContextBase::ATTACHED_SHADERS:
        case WebGLRenderingContextBase::ACTIVE_ATTRIBUTES:
        case WebGLRenderingContextBase::ACTIVE_UNIFORMS: {
            GLint params = 0;
            glGetProgramiv(static_cast<GLuint>(programId), pname, &params);
            LOGI("WebGL getProgramParameter params %{public}d", params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        case WebGL2RenderingContextBase::ACTIVE_UNIFORM_BLOCKS:
        case WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_VARYINGS:
        case WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BUFFER_MODE: {
            if (!IsHighWebGL()) {
                SetError(WebGLRenderingContextBase::INVALID_ENUM);
                return NVal::CreateNull(env).val_;
            }
            GLint params = 0;
            glGetProgramiv(static_cast<GLuint>(programId), pname, &params);
            LOGI("WebGL getProgramParameter params %{public}d", params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            break;
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetAttachedShaders(napi_env env, napi_value object)
{
    uint32_t programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram != nullptr) {
        programId = webGlProgram->GetProgramId();
    }
    LOGI("WebGL getAttachedShaders programId %{public}u", programId);

    GLsizei count = 1;
    std::vector<GLuint> shaderId(MAX_COUNT_ATTACHED_SHADER);
    glGetAttachedShaders(
        static_cast<GLuint>(programId), MAX_COUNT_ATTACHED_SHADER, &count, static_cast<GLuint*>(shaderId.data()));
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

napi_value WebGLRenderingContextBaseImpl::GetUniformLocation(napi_env env, napi_value object, const std::string& name)
{
    WebGLProgram* webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    LOGI("WebGL getUniformLocation programId %{public}u", programId);
    LOGI("WebGL getUniformLocation name '%{public}s'", name.c_str());
    if (!CheckLocationName(name)) {
        return NVal::CreateNull(env).val_;
    }

    GLint locationId = glGetUniformLocation(static_cast<GLuint>(programId), name.c_str());
    LOGI("WebGL getUniformLocation locationId %{public}d", locationId);
    if (locationId == -1) {
        return NVal::CreateNull(env).val_;
    }
    // check if exit
    WebGLUniformLocation* webGLUniformLocation = GetObjectInstance<WebGLUniformLocation>(env, locationId);
    napi_value objUniformLocation = GetNapiValue<WebGLUniformLocation>(env, locationId);
    if (webGLUniformLocation == nullptr) { // create new
        objUniformLocation = WebGLUniformLocation::CreateObjectInstance(env, &webGLUniformLocation).val_;
        if (!objUniformLocation) {
            LOGI("WebGL getUniformLocation locationId %{public}d null", locationId);
            return NVal::CreateNull(env).val_;
        }
        webGLUniformLocation->SetUniformLocationId(locationId);
        AddObject<WebGLUniformLocation>(env, locationId, objUniformLocation);
        LOGI("WebGL getUniformLocation locationId %{public}d create", locationId);
    } else {
        webGLUniformLocation->SetUniformLocationName(name);
    }
    return objUniformLocation;
}

napi_value WebGLRenderingContextBaseImpl::GetAttribLocation(napi_env env, napi_value object, const std::string& name)
{
    WebGLProgram* webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    LOGI("WebGL getAttribLocation programId %{public}u", programId);
    LOGI("WebGL getAttribLocation name %{public}s", name.c_str());

    if (!CheckLocationName(name)) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }

    if (!CheckProgramLinkStatus(webGlProgram)) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }

    GLint returnValue = glGetAttribLocation(static_cast<GLuint>(programId), const_cast<char*>(name.c_str()));
    LOGI("WebGL getAttribLocation location %{public}d", returnValue);
    return NVal::CreateInt64(env, static_cast<int64_t>(returnValue)).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetVertexAttrib(napi_env env, GLenum pname, GLuint index)
{
    LOGI("WebGL getVertexAttrib index %{public}u %{public}u", index, pname);
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    int32_t params;
    if (pname == GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING) { // webglBuffer
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
    } else if (pname != GL_CURRENT_VERTEX_ATTRIB) { // float32Array
        glGetVertexAttribiv(index, pname, &params);
        LOGI("WebGL getVertexAttrib getVertexAttrib end");
        return nullptr;
    }
    WebGLWriteBufferArg writeBuffer(env);
    void* result = reinterpret_cast<void*>(writeBuffer.AllocBuffer(4 * sizeof(GLfloat))); // 4 args
    if (result == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL getVertexAttrib type %{public}u", info->type);
    switch (info->type) {
        case BUFFER_DATA_FLOAT_32:
            glGetVertexAttribfv(index, pname, reinterpret_cast<GLfloat*>(result));
            return writeBuffer.ToNormalArray(BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32);
        case BUFFER_DATA_INT_32:
            glGetVertexAttribIuiv(index, pname, reinterpret_cast<GLuint*>(result));
            return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_INT_32);
        case BUFFER_DATA_UINT_32:
            glGetVertexAttribIuiv(index, pname, reinterpret_cast<GLuint*>(result));
            return writeBuffer.ToNormalArray(BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32);
        default:
            break;
    }
    return NVal::CreateNull(env).val_;
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

napi_value WebGLRenderingContextBaseImpl::GetParameter(napi_env env, GLenum pname)
{
    LOGI("WebGL getParameter pname = %{public}u", pname);
    if (pname == GL_DEPTH_CLEAR_VALUE || pname == GL_LINE_WIDTH || pname == GL_POLYGON_OFFSET_FACTOR ||
        pname == GL_POLYGON_OFFSET_UNITS || pname == GL_SAMPLE_COVERAGE_VALUE) {
        GLfloat params;
        glGetFloatv(pname, &params);
        float res = static_cast<float>(params);
        LOGI("WebGL getParameter params %{public}f", params);
        return NVal::CreateDouble(env, (double)res).val_;
    } else if (pname == GL_TEXTURE_BINDING_2D || pname == GL_TEXTURE_BINDING_CUBE_MAP) {
        GLint params;
        glGetIntegerv(pname, &params);
        return GetObject<WebGLTexture>(env, params);
    } else if (pname == GL_RENDERER || pname == GL_SHADING_LANGUAGE_VERSION || pname == GL_VENDOR ||
               pname == GL_VERSION) {
        return GetExtensionsParameter(env, pname);
    } else if (pname == GL_ARRAY_BUFFER_BINDING || pname == GL_ELEMENT_ARRAY_BUFFER_BINDING) {
        GLint params;
        glGetIntegerv(pname, &params);
        LOGI("WebGL getParameter params %{public}d", params);
        return GetObject<WebGLBuffer>(env, params);
    } else if (pname == GL_FRAMEBUFFER_BINDING) {
        GLint params;
        glGetIntegerv(pname, &params);
        LOGI("WebGL getParameter params %{public}d", params);
        return GetObject<WebGLFramebuffer>(env, params);
    } else if (pname == GL_CURRENT_PROGRAM) {
        GLint params;
        glGetIntegerv(pname, &params);
        LOGI("WebGL getParameter params %{public}d", params);
        return GetObject<WebGLProgram>(env, params);
    } else if (pname == GL_RENDERBUFFER_BINDING) {
        GLint params;
        glGetIntegerv(pname, &params);
        LOGI("WebGL getParameter params %{public}d %{public}p", params, GetObject<WebGLRenderbuffer>(env, params));
        return GetObject<WebGLRenderbuffer>(env, params);
    } else if (pname == GL_ALIASED_LINE_WIDTH_RANGE || pname == GL_ALIASED_POINT_SIZE_RANGE ||
               pname == GL_DEPTH_RANGE) {
        WebGLWriteBufferArg writeBuffer(env);
        GLfloat* params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(2 * sizeof(GLfloat))); // 2 args
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetFloatv(static_cast<GLenum>(pname), params);
        return writeBuffer.ToNormalArray(BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32);
    } else if (pname == GL_COLOR_CLEAR_VALUE || pname == GL_BLEND_COLOR) {
        WebGLWriteBufferArg writeBuffer(env);
        GLfloat* params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(4 * sizeof(GLfloat))); // 4 args
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetFloatv(static_cast<GLenum>(pname), params);
        return writeBuffer.ToNormalArray(BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32);
    } else if (pname == GL_COMPRESSED_TEXTURE_FORMATS) {
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
    } else if (pname == GL_MAX_VIEWPORT_DIMS) {
        WebGLWriteBufferArg writeBuffer(env);
        GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(2 * sizeof(GLint))); // 2 args
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetIntegerv(pname, params);
        return writeBuffer.ToExternalArray(BUFFER_DATA_INT_32);
    } else if (pname == GL_SCISSOR_BOX || pname == WebGLRenderingContextBase::VIEWPORT) {
        WebGLWriteBufferArg writeBuffer(env);
        GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(4 * sizeof(GLint))); // 4 args
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetIntegerv(static_cast<GLenum>(pname), params);
        return writeBuffer.ToExternalArray(BUFFER_DATA_INT_32);
    } else if (pname == GL_BLEND || pname == GL_CULL_FACE || pname == GL_DEPTH_TEST || pname == GL_DEPTH_WRITEMASK ||
               pname == GL_DITHER || pname == GL_POLYGON_OFFSET_FILL || pname == GL_SAMPLE_ALPHA_TO_COVERAGE ||
               pname == GL_SAMPLE_COVERAGE || pname == GL_SAMPLE_COVERAGE_INVERT || pname == GL_SCISSOR_TEST ||
               pname == GL_STENCIL_TEST) {
        GLboolean params;
        glGetBooleanv(pname, &params);
        bool res = static_cast<bool>(params);
        LOGI("WebGL getParameter end");
        return NVal::CreateBool(env, res).val_;
    } else if (pname == GL_COLOR_WRITEMASK) {
        WebGLWriteBufferArg writeBuffer(env);
        GLboolean* params = reinterpret_cast<GLboolean*>(writeBuffer.AllocBuffer(4 * sizeof(GLboolean))); // 4 args
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetBooleanv(pname, static_cast<GLboolean*>(params));
        return writeBuffer.ToNormalArray(BUFFER_DATA_BOOLEAN, BUFFER_DATA_BOOLEAN);
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
               pname == GL_STENCIL_BACK_VALUE_MASK || pname == GL_STENCIL_BACK_WRITEMASK || pname == GL_STENCIL_BITS ||
               pname == GL_STENCIL_CLEAR_VALUE || pname == GL_STENCIL_FAIL || pname == GL_STENCIL_FUNC ||
               pname == GL_STENCIL_PASS_DEPTH_FAIL || pname == GL_STENCIL_PASS_DEPTH_PASS || pname == GL_STENCIL_REF ||
               pname == GL_STENCIL_VALUE_MASK || pname == GL_STENCIL_WRITEMASK || pname == GL_SUBPIXEL_BITS ||
               pname == GL_UNPACK_ALIGNMENT) {
        GLint params;
        glGetIntegerv(pname, &params);
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL getParameter params %{public}d", params);
        return NVal::CreateInt64(env, res).val_;
    } else if (pname == WebGLRenderingContextBase::UNPACK_COLORSPACE_CONVERSION_WEBGL) {
        return NVal::CreateInt64(env, unpackColorspaceConversion_).val_;
    } else if (pname == WebGLRenderingContextBase::UNPACK_FLIP_Y_WEBGL) {
        return NVal::CreateBool(env, unpackFlipY_).val_;
    } else if (pname == WebGLRenderingContextBase::UNPACK_PREMULTIPLY_ALPHA_WEBGL) {
        return NVal::CreateBool(env, unpackPremultiplyAlpha_).val_;
    }
    LOGI("WebGL getParameter : pname is wrong");
    SetError(WebGLRenderingContextBase::INVALID_ENUM);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::ClearColor(
    napi_env env, GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    LOGI("WebGL clearColor [%{public}f, %{public}f, %{public}f, %{public}f]", red, green, blue, alpha);
    clearColor_[0] = red;
    clearColor_[1] = green;
    clearColor_[2] = blue;
    clearColor_[3] = alpha;
    glClearColor(red, green, blue, alpha);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ColorMask(
    napi_env env, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    colorMask_[0] = red;
    colorMask_[1] = green;
    colorMask_[2] = blue;
    colorMask_[3] = alpha;
    glColorMask(static_cast<GLboolean>(red), static_cast<GLboolean>(green), static_cast<GLboolean>(blue),
        static_cast<GLboolean>(alpha));
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

napi_value WebGLRenderingContextBaseImpl::ClearDepth(napi_env env, GLclampf depth)
{
    LOGI("WebGL clearDepth %{public}f", depth);
    clearDepth_ = depth;
    glClearDepthf(depth);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ClearStencil(napi_env env, GLint s)
{
    LOGI("WebGL clearStencil s %{public}d", s);
    clearStencil_ = static_cast<GLint>(s);
    glClearStencil(static_cast<GLint>(s));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::UseProgram(napi_env env, napi_value object)
{
    GetError(env); // TODO clear error
    LOGI("WebGL useProgram start %{public}u", GetError_());
    uint32_t program = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, object);
    if (webGLProgram != nullptr) {
        program = webGLProgram->GetProgramId();
    }
    LOGI("WebGL useProgram programId %{public}u %{public}u", program, currentProgramId_);
    if ((currentProgramId_ != program) || (currentProgramId_ == 0)) {
        currentProgramId_ = program;
    }
    glUseProgram(static_cast<GLuint>(program));
    LOGI("WebGL useProgram programId %{public}u result %{public}u", program, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ValidateProgram(napi_env env, napi_value object)
{
    uint32_t program = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, object);
    if (webGLProgram != nullptr) {
        program = webGLProgram->GetProgramId();
    }
    LOGI("WebGL validateProgram program %{public}u", program);
    glValidateProgram(static_cast<GLuint>(program));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::LinkProgram(napi_env env, napi_value object)
{
    uint32_t program = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, object);
    if (webGLProgram != nullptr) {
        program = webGLProgram->GetProgramId();
    }
    glLinkProgram(static_cast<GLuint>(program));
    LOGI("WebGL linkProgram program %{public}u result %{public}u", program, GetError_());
    return nullptr;
}

WebGLBuffer* WebGLRenderingContextBaseImpl::CheckAndGetBoundBuffer(napi_env env, GLenum target)
{
    uint32_t index = 0;
    if (!CheckBufferTarget(env, target, index)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    WebGLBuffer* webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[index]);
    if (webGLBuffer == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
    }
    return webGLBuffer;
}

napi_value WebGLRenderingContextBaseImpl::RenderBufferStorage(napi_env env, const TexStorageArg& arg)
{
    LOGI("WebGL renderbufferStorage target %{public}u %{public}u %{public}d %{public}d",
        arg.target, arg.internalFormat, arg.width, arg.height);
    WebGLRenderbuffer* renderBuffer = CheckRenderBufferStorage(env, arg);
    if (renderBuffer == nullptr) {
        return nullptr;
    }

    switch (arg.internalFormat) {
        case WebGLRenderingContextBase::DEPTH_COMPONENT16:
        case WebGLRenderingContextBase::RGBA4:
        case WebGLRenderingContextBase::RGB5_A1:
        case WebGLRenderingContextBase::RGB565:
        case WebGLRenderingContextBase::STENCIL_INDEX8: {
            glRenderbufferStorage(arg.target, arg.internalFormat, arg.width, arg.height);
            renderBuffer->SetInternalFormat(arg.internalFormat);
            renderBuffer->SetSize(arg.width, arg.height);
            break;
        }
        case WebGLRenderingContextBase::DEPTH_STENCIL: {
            // TODO
            glRenderbufferStorage(arg.target, GL_DEPTH_COMPONENT16, arg.width, arg.height);
            glRenderbufferStorage(arg.target, GL_STENCIL_INDEX8, arg.width, arg.height);
            renderBuffer->SetInternalFormat(arg.internalFormat);
            renderBuffer->SetSize(arg.width, arg.height);
            break;
        }
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    LOGE("WebGL renderbufferStorage result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::VertexAttribPointer(napi_env env, const VertexAttribArg* vertexInfo)
{
    LOGI("WebGL vertexAttribPointer index %{public}u size %{public}u type %{public}u", vertexInfo->index,
        vertexInfo->size, vertexInfo->type);

    if (!CheckGLenum(env, vertexInfo->type, { GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_FLOAT }, {})) {
        LOGE("WebGL vertexAttribPointer invalid type %{public}d", vertexInfo->type);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
    }
    GLenum result = CheckVertexAttribPointer(env, vertexInfo);
    if (result) {
        SetError(result);
        return nullptr;
    }
    glVertexAttribPointer(vertexInfo->index, vertexInfo->size, vertexInfo->type, vertexInfo->normalized,
        vertexInfo->stride, reinterpret_cast<GLvoid*>(vertexInfo->offset));
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
    GLvoid* point = nullptr;
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
    LOGI("WebGL enableVertexAttribArray index %{public}ld", index);
    glEnableVertexAttribArray(static_cast<GLuint>(index));
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
    if (cap == WebGLRenderingContextBase::STENCIL_TEST) {
        stencilEnabled_ = true;
    }
    if (cap == WebGLRenderingContextBase::SCISSOR_TEST) {
        scissorEnabled_ = true;
    }
    glEnable(cap);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BindAttribLocation(
    napi_env env, napi_value program, GLuint index, const std::string& name)
{
    LOGE("WebGL bindAttribLocation index %{public}u name %{public}s", index, name.c_str());
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, program);
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

    if (!CheckLocationName(name)) {
        return nullptr;
    }
    LOGI("WebGL bindAttribLocation programId %{public}d index %{public}u", programId, index);
    glBindAttribLocation(static_cast<GLuint>(programId), index, const_cast<char*>(name.c_str()));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GenerateMipmap(napi_env env, GLenum target)
{
    LOGI("WebGL generateMipmap target %{public}u", target);
    if (!CheckGLenum(env, target,
            { WebGLRenderingContextBase::TEXTURE_2D, WebGLRenderingContextBase::TEXTURE_CUBE_MAP },
            { WebGL2RenderingContextBase::TEXTURE_3D, WebGL2RenderingContextBase::TEXTURE_2D_ARRAY })) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    WebGLTexture* webGLTexture = GetBoundTexture(env, target, false);
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

napi_value WebGLRenderingContextBaseImpl::Uniform_f(
    napi_env env, napi_value locationObj, uint32_t count, const GLfloat* data)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL Uniform_f can not find uniform");
        // SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    LOGI("WebGL uniform location %{public}d [%{public}f %{public}f %{public}f %{public}f]", location, data[0], data[1],
        data[2], data[3]);
    switch (count) {
        case 1:
            glUniform1f(location, data[0]);
            return nullptr;
        case 2:
            glUniform2f(location, data[0], data[1]);
            return nullptr;
        case 3:
            glUniform3f(location, data[0], data[1], data[2]);
            return nullptr;
        case 4:
            glUniform4f(location, data[0], data[1], data[2], data[3]);
            return nullptr;
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::Uniform_i(
    napi_env env, napi_value locationObj, uint32_t count, const GLint* data)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL Uniform_f can not find uniform");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    LOGI("WebGL uniform location %{public}d [%{public}d %{public}d %{public}d %{public}d]", location, data[0], data[1],
        data[2], data[3]);
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
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::Uniform_ui(
    napi_env env, napi_value locationObj, uint32_t count, const GLuint* data)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniformXui can not find uniform");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    LOGI("WebGL uniformXui location %{public}d [%{public}u %{public}u %{public}u %{public}u]",
        location, data[0], data[1], data[2], data[3]);
    switch (count) {
        case PARAMETERS_NUM_1:
            glUniform1ui(location, data[0]);
            break;
        case PARAMETERS_NUM_2:
            glUniform2ui(location, data[0], data[1]);
            break;
        case PARAMETERS_NUM_3:
            glUniform3ui(location, data[0], data[1], data[2]); // 0, 1, 2 are array index
            break;
        case PARAMETERS_NUM_4:
            glUniform4ui(location, data[0], data[1], data[2], data[3]); // 0, 1, 2, 3 are array index
            break;
    }
    LOGI("WebGL uniformXui result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::Uniform_fv(
    napi_env env, napi_value locationObj, napi_value data, const UniformExtInfo* info)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniformfv can not find uniform");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
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
    GLsizei count = static_cast<GLsizei>(readData.GetBufferLength() / sizeof(float));
    GLfloat* srcData = reinterpret_cast<GLfloat*>(readData.GetBuffer());
    if (!IsHighWebGL()) {
        LOGI("WebGL uniformfv count = %{public}u length %{public}zu", count, readData.GetBufferLength());
        if (count < info->dimension || (count % info->dimension)) {
            LOGE("WebGL uniformfv invalid value, count %{public}d", count);
            SetError(WebGLRenderingContextBase::INVALID_VALUE);
            return nullptr;
        }
    } else {
        count = info->srcLength != 0 ? info->srcLength : count;
        srcData = srcData + info->srcOffset;
    }
    switch (info->dimension) {
        case PARAMETERS_NUM_1:
            glUniform1fv(location, 1, srcData);
            return nullptr;
        case PARAMETERS_NUM_2:
            glUniform2fv(location, count >> 1, srcData); // 1 array size
            return nullptr;
        case PARAMETERS_NUM_3:
            glUniform3fv(location, count / 3, srcData); // 3 array size
            return nullptr;
        case PARAMETERS_NUM_4:
            glUniform4fv(location, count / 4, srcData); // 4 array size
            return nullptr;
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::Uniform_iv(
    napi_env env, napi_value locationObj, napi_value data, const UniformExtInfo* info)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL Uniform_f can not find uniform");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
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
    GLsizei count = static_cast<GLsizei>(readData.GetBufferLength() / sizeof(GLint));
    GLint* srcData = reinterpret_cast<GLint*>(readData.GetBuffer());
    if (!IsHighWebGL()) {
        LOGI("WebGL uniformfv count = %{public}u length %{public}zu", count, readData.GetBufferLength());
        if (count < info->dimension || (count % info->dimension)) {
            LOGE("WebGL uniformfv invalid value, count %{public}d", count);
            SetError(WebGLRenderingContextBase::INVALID_VALUE);
            return nullptr;
        }
    } else {
        count = info->srcLength != 0 ? info->srcLength : count;
        srcData = srcData + info->srcOffset;
    }
    switch (info->dimension) {
        case PARAMETERS_NUM_1:
            glUniform1iv(location, 1, srcData);
            return nullptr;
        case PARAMETERS_NUM_2:
            glUniform2iv(location, count / 2, srcData); // 2 array size
            return nullptr;
        case PARAMETERS_NUM_3:
            glUniform3iv(location, count / 3, srcData); // 3 array size
            return nullptr;
        case PARAMETERS_NUM_4:
            glUniform4iv(location, count / 4, srcData); // 4 array size
            return nullptr;
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::Uniform_uiv(
    napi_env env, napi_value locationObj, napi_value dataObj, const UniformExtInfo* info)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL Uniform_f can not find uniform");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    WebGLReadBufferArg readData(env);
    napi_status status = readData.GenBufferData(dataObj, BUFFER_DATA_UINT_32);
    if (status != napi_ok) {
        LOGE("WebGL uniformfv failed to getbuffer data");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    readData.DumpBuffer(readData.GetBufferDataType());

    if (readData.GetBufferDataType() != BUFFER_DATA_UINT_32) {
        LOGE("WebGL uniformfv not support data type %{public}d", readData.GetBufferDataType());
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLsizei count = static_cast<GLsizei>(readData.GetBufferLength() / sizeof(GLuint));
    GLuint* data = reinterpret_cast<GLuint*>(readData.GetBuffer());
    if (!IsHighWebGL()) {
        LOGI("WebGL uniformfv count = %{public}u length %{public}zu", count, readData.GetBufferLength());
        if (count < info->dimension || (count % info->dimension)) {
            LOGE("WebGL uniformfv invalid value, count %{public}d", count);
            SetError(WebGLRenderingContextBase::INVALID_VALUE);
            return nullptr;
        }
    } else {
        count = info->srcLength != 0 ? info->srcLength : count;
        data = data + info->srcOffset;
    }
    switch (info->dimension) {
        case PARAMETERS_NUM_1:
            glUniform1uiv(location, 1, data);
            return nullptr;
        case PARAMETERS_NUM_2:
            glUniform2uiv(location, count / PARAMETERS_NUM_2, data);
            return nullptr;
        case PARAMETERS_NUM_3:
            glUniform3uiv(location, count / PARAMETERS_NUM_3, data);
            return nullptr;
        case PARAMETERS_NUM_4:
            glUniform4uiv(location, count / PARAMETERS_NUM_4, data);
            return nullptr;
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::UniformMatrix_fv(
    napi_env env, napi_value locationObj, napi_value data, GLboolean transpose, const UniformExtInfo* info)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniformMatrixfv can not find uniform");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
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
    GLsizei count = static_cast<GLsizei>(readData.GetBufferLength() / sizeof(float));
    GLfloat* srcData = reinterpret_cast<GLfloat*>(readData.GetBuffer());
    if (!IsHighWebGL()) {
        LOGI("WebGL uniformMatrixfv count = %{public}u length %{public}zu", count, readData.GetBufferLength());
        if (count < info->dimension || (count % info->dimension)) {
            LOGE("WebGL uniformMatrixfv invalid value, count %{public}d", count);
            SetError(WebGLRenderingContextBase::INVALID_VALUE);
            return nullptr;
        }
    } else {
        count = info->srcLength != 0 ? info->srcLength : count;
        srcData = srcData + info->srcOffset;
    }
    switch (info->dimension) {
        case WebGLArg::MATRIX_2X2_REQUIRE_MIN_SIZE:
            glUniformMatrix2fv(location, count >> 2, transpose, srcData); // 2 matrix
            return nullptr;
        case WebGLArg::MATRIX_3X3_REQUIRE_MIN_SIZE:
            glUniformMatrix3fv(location, count >> 3, transpose, srcData); // 3 matrix
            return nullptr;
        case WebGLArg::MATRIX_4X4_REQUIRE_MIN_SIZE:
            glUniformMatrix4fv(location, count >> 4, transpose, srcData); // 4 matrix
            return nullptr;
        case WebGLArg::MATRIX_3X2_REQUIRE_MIN_SIZE:
            glUniformMatrix3x2fv(location, count / 6, transpose, srcData); // 6 matrix
            return nullptr;
        case WebGLArg::MATRIX_4X2_REQUIRE_MIN_SIZE:
            glUniformMatrix4x2fv(location, count / 8, transpose, srcData); // 8 matrix
            return nullptr;
        case WebGLArg::MATRIX_2X3_REQUIRE_MIN_SIZE:
            glUniformMatrix2x3fv(location, count / 6, transpose, srcData); // 6 matrix
            return nullptr;
        case WebGLArg::MATRIX_4X3_REQUIRE_MIN_SIZE:
            glUniformMatrix4x3fv(location, count / 12, transpose, srcData); // 12 matrix
            return nullptr;
        case WebGLArg::MATRIX_2X4_REQUIRE_MIN_SIZE:
            glUniformMatrix2x4fv(location, count / 8, transpose, srcData); // 8 matrix
            return nullptr;
        case WebGLArg::MATRIX_3X4_REQUIRE_MIN_SIZE:
            glUniformMatrix3x4fv(location, count / 12, transpose, srcData); // 12 matrix
            return nullptr;
    }
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CheckFrameBufferStatus(napi_env env, GLenum target)
{
    LOGI("WebGL checkFramebufferStatus target %{public}u", target);
    uint32_t index = 0;
    if (!CheckFrameBufferTarget(env, target, index)) {
        return nullptr;
    }
    GLenum res = glCheckFramebufferStatus(target);
    LOGI("WebGL checkFramebufferStatus target %{public}u result %{public}u", target, res);
    return NVal::CreateInt64(env, static_cast<int64_t>(res)).val_;
}

napi_value WebGLRenderingContextBaseImpl::DepthMask(napi_env env, bool flag)
{
    LOGI("WebGL depthMask flag %{public}u", flag);
    depthMask_ = flag;
    glDepthMask(static_cast<GLboolean>(flag));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::StencilMask(napi_env env, GLuint mask)
{
    LOGI("WebGL stencilMask mask %{public}u", mask);
    stencilMask_[0] = mask;
    stencilMask_[1] = mask;
    glStencilMask(mask);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::StencilMaskSeparate(napi_env env, GLenum face, GLuint mask)
{
    LOGI("WebGL stencilMaskSeparate face %{public}u mask %{public}u", face, mask);
    switch (face) {
        case WebGLRenderingContextBase::FRONT_AND_BACK:
            stencilMask_[0] = mask;
            stencilMask_[1] = mask;
            break;
        case WebGLRenderingContextBase::FRONT:
            stencilMask_[0] = mask;
            break;
        case WebGLRenderingContextBase::BACK:
            stencilMask_[1] = mask;
            break;
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    glStencilMaskSeparate(face, mask);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::StencilFunc(napi_env env, GLenum func, GLint ref, GLuint mask)
{
    LOGI("WebGL stencilFunc func %{public}u %{public}d %{public}u", func, ref, mask);
    if (!CheckGLenum(env, func,
            { WebGLRenderingContextBase::NEVER, WebGLRenderingContextBase::LESS, WebGLRenderingContextBase::EQUAL,
                WebGLRenderingContextBase::LEQUAL, WebGLRenderingContextBase::GREATER,
                WebGLRenderingContextBase::NOTEQUAL, WebGLRenderingContextBase::GEQUAL,
                WebGLRenderingContextBase::ALWAYS },
            {})) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    stencilFuncRef_[0] = ref;
    stencilFuncRef_[1] = ref;
    stencilFuncMask_[0] = mask;
    stencilFuncMask_[1] = mask;
    glStencilFunc(func, ref, mask);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::StencilFuncSeparate(
    napi_env env, GLenum face, GLenum func, GLint ref, GLuint mask)
{
    LOGI("WebGL stencilFuncSeparate face %{public}u func %{public}u %{public}d %{public}u", face, func, ref, mask);
    switch (face) {
        case WebGLRenderingContextBase::FRONT_AND_BACK: {
            stencilFuncRef_[0] = ref;
            stencilFuncRef_[1] = ref;
            stencilFuncMask_[0] = mask;
            stencilFuncMask_[1] = mask;
            break;
        }
        case WebGLRenderingContextBase::FRONT: {
            stencilFuncRef_[0] = ref;
            stencilFuncMask_[0] = mask;
            break;
        }
        case WebGLRenderingContextBase::BACK: {
            stencilFuncRef_[1] = ref;
            stencilFuncMask_[1] = mask;
            break;
        }
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    if (!CheckGLenum(env, func,
            { WebGLRenderingContextBase::NEVER, WebGLRenderingContextBase::LESS, WebGLRenderingContextBase::EQUAL,
                WebGLRenderingContextBase::LEQUAL, WebGLRenderingContextBase::GREATER,
                WebGLRenderingContextBase::NOTEQUAL, WebGLRenderingContextBase::GEQUAL,
                WebGLRenderingContextBase::ALWAYS },
            {})) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    glStencilFuncSeparate(face, func, ref, mask);
    return nullptr;
}

template<class T>
GLenum WebGLRenderingContextBaseImpl::CheckTexParameter(
    napi_env env, GLenum target, GLenum pname, T param, bool isFloat)
{
    WebGLTexture* textuer = GetBoundTexture(env, target, false);
    if (textuer == nullptr) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    switch (pname) {
        case WebGLRenderingContextBase::TEXTURE_MIN_FILTER:
        case WebGLRenderingContextBase::TEXTURE_MAG_FILTER:
            break;
        case WebGL2RenderingContextBase::TEXTURE_WRAP_R:
            if (!IsHighWebGL()) {
                return WebGLRenderingContextBase::INVALID_ENUM;
            }
        case WebGLRenderingContextBase::TEXTURE_WRAP_S:
        case WebGLRenderingContextBase::TEXTURE_WRAP_T:
            if (param == WebGLRenderingContextBase::CLAMP_TO_EDGE ||
                param == WebGLRenderingContextBase::MIRRORED_REPEAT || param == WebGLRenderingContextBase::REPEAT) {
                break;
            }
            return WebGLRenderingContextBase::INVALID_ENUM;
        case WebGL2RenderingContextBase::TEXTURE_COMPARE_FUNC:
        case WebGL2RenderingContextBase::TEXTURE_COMPARE_MODE:
        case WebGL2RenderingContextBase::TEXTURE_BASE_LEVEL:
        case WebGL2RenderingContextBase::TEXTURE_MAX_LEVEL:
        case WebGL2RenderingContextBase::TEXTURE_MAX_LOD:
        case WebGL2RenderingContextBase::TEXTURE_MIN_LOD:
            if (!IsHighWebGL()) {
                return WebGLRenderingContextBase::INVALID_ENUM;
            }
            break;
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }

    // TODO 是否保留 tex->setParameterf(pname, paramf)
    return WebGLRenderingContextBase::NO_ERROR;
}

napi_value WebGLRenderingContextBaseImpl::TexParameteri(napi_env env, GLenum target, GLenum pname, GLint param)
{
    LOGI("WebGL texParameteri target %{public}u %{public}u %{public}d", target, pname, param);
    GLenum result = CheckTexParameter<GLint>(env, target, pname, param, false);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SetError(result);
        return nullptr;
    }
    glTexParameteri(target, pname, static_cast<GLint>(param));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexParameterf(napi_env env, GLenum target, GLenum pname, GLfloat param)
{
    LOGI("WebGL texParameteri target %{public}u %{public}u %{public}f", target, pname, param);
    GLenum result = CheckTexParameter<GLfloat>(env, target, pname, param, false);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SetError(result);
        return nullptr;
    }
    glTexParameterf(target, pname, static_cast<GLfloat>(param));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::GetTexParameter(napi_env env, GLenum target, GLenum pname)
{
    WebGLTexture* texture = GetBoundTexture(env, target, false);
    if (!texture) {
        return NVal::CreateNull(env).val_;
    }
    if (IsHighWebGL()) {
        switch (pname) {
            case GL_TEXTURE_WRAP_R:
            case GL_TEXTURE_COMPARE_FUNC:
            case GL_TEXTURE_COMPARE_MODE:
            case GL_TEXTURE_BASE_LEVEL:
            case GL_TEXTURE_MAX_LEVEL:
            case GL_TEXTURE_IMMUTABLE_LEVELS: {
                    GLint params = 0;
                    glGetTexParameteriv(target, pname, &params);
                    return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
                }
            case GL_TEXTURE_IMMUTABLE_FORMAT:{
                    GLint params = 0;
                    glGetTexParameteriv(target, pname, &params);
                    return NVal::CreateBool(env, params == 1).val_;
                }
            case GL_TEXTURE_MAX_LOD:
            case GL_TEXTURE_MIN_LOD: {
                    GLfloat params = 0.f;
                    glGetTexParameterfv(target, pname, &params);
                    return NVal::CreateDouble(env, static_cast<double>(params)).val_;
                }
            default:
                break;
        }
    }

    switch (pname) {
        case WebGLRenderingContextBase::TEXTURE_MAG_FILTER:
        case WebGLRenderingContextBase::TEXTURE_MIN_FILTER:
        case WebGLRenderingContextBase::TEXTURE_WRAP_S:
        case WebGLRenderingContextBase::TEXTURE_WRAP_T: {
            GLint params = 0;
            glGetTexParameteriv(target, pname, &params);
            LOGI("WebGL getTexParameter params %{public}u", static_cast<int64_t>(params));
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
}

napi_value WebGLRenderingContextBaseImpl::VertexAttribfv(napi_env env, GLuint index, int count, napi_value dataObj)
{
    LOGI("WebGL vertexAttribfv index %{public}u %{public}u", index, count);
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(dataObj, BUFFER_DATA_FLOAT_32);
    if (status != 0) {
        LOGE("WebGL vertexAttribfv failed to getbuffer data");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
        LOGE("WebGL :vertexAttribfv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    if (static_cast<int>(bufferData.GetBufferLength() / sizeof(float)) < count) {
        LOGE("WebGL vertexAttribfv invalid data length %{public}zu", bufferData.GetBufferLength());
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    switch (count) {
        case PARAMETERS_NUM_1:
            glVertexAttrib1fv(index, reinterpret_cast<GLfloat*>(bufferData.GetBuffer()));
            break;
        case PARAMETERS_NUM_2:
            glVertexAttrib2fv(index, reinterpret_cast<GLfloat*>(bufferData.GetBuffer()));
            break;
        case PARAMETERS_NUM_3:
            glVertexAttrib3fv(index, reinterpret_cast<GLfloat*>(bufferData.GetBuffer()));
            break;
        case PARAMETERS_NUM_4:
            glVertexAttrib4fv(index, reinterpret_cast<GLfloat*>(bufferData.GetBuffer()));
            break;
        default:
            break;
    }
    info->type = BUFFER_DATA_FLOAT_32;
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::VertexAttribf(napi_env env, GLuint index, int count, GLfloat* data)
{
    LOGI("WebGL vertexAttribf index %{public}u %{public}u", index, count);
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    // In GL, we skip setting vertexAttrib0 values.
    switch (count) {
        case PARAMETERS_NUM_1:
            glVertexAttrib1f(index, data[0]);
            break;
        case PARAMETERS_NUM_2:
            glVertexAttrib2f(index, data[0], data[1]);
            break;
        case PARAMETERS_NUM_3:
            glVertexAttrib3f(index, data[0], data[1], data[2]); // 1, 2 index
            break;
        case PARAMETERS_NUM_4:
            glVertexAttrib4f(index, data[0], data[1], data[2], data[3]); // 1, 2, 3 index
            break;
        default:
            break;
    }
    LOGI("WebGL vertexAttribf index %{public}u result %{public}u", index, GetError_());
    info->type = BUFFER_DATA_FLOAT_32;
    return nullptr;
}

bool WebGLRenderingContextBaseImpl::GetUniformType(napi_env env, GLuint programId, GLint locationId, GLenum& type)
{
    GLint maxNameLength = -1;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
    LOGI("WebGL getUniform maxNameLength %{public}d", maxNameLength);
    if (maxNameLength <= 0) {
        return false;
    }

    GLint activeUniforms = 0;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &activeUniforms);
    LOGI("WebGL getUniform activeUniforms %{public}d", activeUniforms);
    if (locationId >= activeUniforms) {
        return false;
    }
    std::vector<GLchar> name(maxNameLength + 1);
    name[maxNameLength] = '\0';
    for (int i = 0; i < activeUniforms; i++) {
        GLsizei nameLength = 0;
        GLint size = -1;
        glGetActiveUniform(programId, i, maxNameLength, &nameLength, &size, &type, name.data());
        LOGI("WebGL getUniform index %{public}d type 0x%{public}x name %{public}s ", i, type, name.data());
        if (locationId == i) {
            break;
        }
    }
    return true;
}

napi_value WebGLRenderingContextBaseImpl::GetUniform(napi_env env, napi_value programObj, napi_value uniformObj)
{
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, uniformObj);
    if (webGLUniformLocation == nullptr || webGLProgram == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    GLuint programId = webGLProgram->GetProgramId();
    GLint locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL getUniform programId %{public}u locationId %{public}u %{public}s",
        programId, locationId, webGLUniformLocation->GetUniformLocationName().c_str());
    GLenum type = 0;
    if (!GetUniformType(env, programId, locationId, type)) {
        LOGI("WebGL getUniform invalid uniform ");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }

    const UniformTypeMap* typeMap = GetUniformTypeMap(type);
    if (typeMap == nullptr) {
        LOGI("WebGL getUniform can not find type map");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }

    WebGLWriteBufferArg writeBuffer(env);
    void* data = reinterpret_cast<void*>(writeBuffer.AllocBuffer(typeMap->length * sizeof(GLfloat)));
    if (data == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }
    if (typeMap->baseType == GL_FLOAT) {
        GLfloat* params = reinterpret_cast<GLfloat*>(data);
        glGetUniformfv(programId, locationId, params);
        if (typeMap->length == 1) {
            return NVal::CreateDouble(env, params[0]).val_;
        }
    } else if (typeMap->baseType == GL_INT) {
        GLint* params = reinterpret_cast<GLint*>(data);
        glGetUniformiv(programId, locationId, params);
        if (typeMap->length == 1) {
            return NVal::CreateInt64(env, params[0]).val_;
        }
    } else if (typeMap->baseType == GL_BOOL) {
        GLint* params = reinterpret_cast<GLint*>(data);
        glGetUniformiv(programId, locationId, params);
        if (typeMap->length == 1) {
            return NVal::CreateBool(env, params[0]).val_;
        }
    } else if (typeMap->baseType == GL_UNSIGNED_INT) {
        GLuint* params = reinterpret_cast<GLuint*>(data);
        glGetUniformuiv(programId, locationId, params);
        LOGI("getUniform result %{public}u", params[0]);
        if (typeMap->length == 1) {
            return NVal::CreateInt64(env, params[0]).val_;
        }
    } else {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    writeBuffer.DumpBuffer(typeMap->dstType);
    return writeBuffer.ToNormalArray(typeMap->srcType, typeMap->dstType);
}

bool WebGLRenderingContextBaseImpl::CheckGLenum(
    napi_env env, GLenum type, const std::vector<GLenum>& glSupport, const std::vector<GLenum>& g2Support)
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

bool WebGLRenderingContextBaseImpl::CheckInList(napi_env env, GLenum type, const std::vector<GLenum>& glSupport)
{
    auto it = std::find(glSupport.begin(), glSupport.end(), type);
    if (it != glSupport.end()) {
        return true;
    }
    return false;
}

bool WebGLRenderingContextBaseImpl::CheckCap(napi_env env, GLenum cap)
{
    return CheckGLenum(env, cap,
        { WebGLRenderingContextBase::BLEND, WebGLRenderingContextBase::CULL_FACE, WebGLRenderingContextBase::DEPTH_TEST,
            WebGLRenderingContextBase::DITHER, WebGLRenderingContextBase::POLYGON_OFFSET_FILL,
            WebGLRenderingContextBase::SAMPLE_ALPHA_TO_COVERAGE, WebGLRenderingContextBase::SAMPLE_COVERAGE,
            WebGLRenderingContextBase::SCISSOR_TEST, WebGLRenderingContextBase::STENCIL_TEST },
        { WebGL2RenderingContextBase::RASTERIZER_DISCARD });
}

bool WebGLRenderingContextBaseImpl::CheckInternalFormat(napi_env env, GLenum internalFormat)
{
    static const std::vector<GLenum> glSupport = { WebGLRenderingContextBase::RGBA4, WebGLRenderingContextBase::RGB565,
        WebGLRenderingContextBase::RGB5_A1, WebGLRenderingContextBase::DEPTH_COMPONENT16,
        WebGLRenderingContextBase::STENCIL_INDEX8, WebGLRenderingContextBase::DEPTH_STENCIL };
    static const std::vector<GLenum> gl2Support = { WebGL2RenderingContextBase::R8, WebGL2RenderingContextBase::R8UI,
        WebGL2RenderingContextBase::R8I, WebGL2RenderingContextBase::R16UI, WebGL2RenderingContextBase::R16I,
        WebGL2RenderingContextBase::R32UI, WebGL2RenderingContextBase::R32I,

        WebGL2RenderingContextBase::RG8, WebGL2RenderingContextBase::RG8UI, WebGL2RenderingContextBase::RG8I,
        WebGL2RenderingContextBase::RG16UI, WebGL2RenderingContextBase::RG16I, WebGL2RenderingContextBase::RG32UI,
        WebGL2RenderingContextBase::RG32I,

        WebGL2RenderingContextBase::RGB8, WebGL2RenderingContextBase::RGBA8, WebGL2RenderingContextBase::SRGB8_ALPHA8,
        WebGL2RenderingContextBase::RGB10_A2, WebGL2RenderingContextBase::RGBA8UI, WebGL2RenderingContextBase::RGBA8I,
        WebGL2RenderingContextBase::RGB10_A2UI,

        WebGL2RenderingContextBase::RGBA16UI, WebGL2RenderingContextBase::RGBA16I, WebGL2RenderingContextBase::RGBA32I,
        WebGL2RenderingContextBase::RGBA32UI, WebGL2RenderingContextBase::DEPTH_COMPONENT24,
        WebGL2RenderingContextBase::DEPTH_COMPONENT32F, WebGL2RenderingContextBase::DEPTH24_STENCIL8,
        WebGL2RenderingContextBase::DEPTH32F_STENCIL8 };
    return CheckGLenum(env, internalFormat, glSupport, gl2Support);
}

bool WebGLRenderingContextBaseImpl::CheckAttachment(napi_env env, GLenum attachment)
{
    static const std::vector<GLenum> glSupport = { WebGLRenderingContextBase::COLOR_ATTACHMENT0,
        WebGLRenderingContextBase::DEPTH_ATTACHMENT, WebGLRenderingContextBase::STENCIL_ATTACHMENT,
        WebGLRenderingContextBase::DEPTH_STENCIL_ATTACHMENT };

    if (!CheckGLenum(env, attachment, glSupport, {})) {
        if (IsHighWebGL()) {
            if (attachment < WebGL2RenderingContextBase::COLOR_ATTACHMENT1 ||
                attachment > WebGL2RenderingContextBase::COLOR_ATTACHMENT15) {
                return false;
            }
        }
    }
    return true;
}

WebGLTexture* WebGLRenderingContextBaseImpl::GetBoundTexture(napi_env env, GLenum target, bool cubeMapExt)
{
    uint32_t index = 0;
    switch (target) {
        case WebGLRenderingContextBase::TEXTURE_2D:
            index = BoundTextureType::TEXTURE_2D;
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
            index = BoundTextureType::TEXTURE_CUBE_MAP;
            break;
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP:
            if (cubeMapExt) {
                return nullptr;
            }
            index = BoundTextureType::TEXTURE_CUBE_MAP;
            break;
        default:
            return nullptr;
    }
    if (activeTextureIndex_ >= boundTexture_[index].size()) {
        return nullptr;
    }

    return GetObjectInstance<WebGLTexture>(env, boundTexture_[index][activeTextureIndex_]);
}

WebGLFramebuffer* WebGLRenderingContextBaseImpl::GetBoundFrameBuffer(napi_env env, GLenum target)
{
    uint32_t index = 0;
    if (!CheckFrameBufferTarget(env, target, index)) {
        return nullptr;
    }
    LOGI("Get bound Frame buffer target %{public}u id %{public}u ", target, boundFrameBufferIds_[index]);
    return GetObjectInstance<WebGLFramebuffer>(env, boundFrameBufferIds_[index]);
}

WebGLRenderbuffer* WebGLRenderingContextBaseImpl::GetBoundRenderBuffer(napi_env env, GLenum target)
{
    uint32_t index = 0;
    if (!CheckRenderBufferTarget(env, target, index)) {
        return nullptr;
    }
    LOGI("Get bound render buffer target %{public}u id %{public}u ", target, boundRenderBufferIds_[index]);
    return GetObjectInstance<WebGLRenderbuffer>(env, boundRenderBufferIds_[index]);
}

WebGLBuffer* WebGLRenderingContextBaseImpl::GetBoundBuffer(napi_env env, GLenum target)
{
    uint32_t index = BoundBufferType::ARRAY_BUFFER;
    if (!CheckBufferTarget(env, target, index)) {
        return nullptr;
    }
    LOGI("Get bound buffer target %{public}u id %{public}u ", target, boundBufferIds_[index]);
    return GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[index]);
}

bool WebGLRenderingContextBaseImpl::CheckDrawMode(napi_env env, GLenum mode)
{
    return CheckGLenum(env, mode,
        {
            WebGLRenderingContextBase::POINTS,
            WebGLRenderingContextBase::LINE_STRIP,
            WebGLRenderingContextBase::LINE_LOOP,
            WebGLRenderingContextBase::LINES,
            WebGLRenderingContextBase::TRIANGLE_STRIP,
            WebGLRenderingContextBase::TRIANGLE_FAN,
            WebGLRenderingContextBase::TRIANGLES
        },
        {});
}

bool WebGLRenderingContextBaseImpl::CheckFrameBufferTarget(napi_env env, GLenum target, uint32_t& index)
{
    switch (target) {
        case WebGLRenderingContextBase::FRAMEBUFFER:
            index = BoundFrameBufferType::FRAMEBUFFER;
            return true;
        default:
            if (!IsHighWebGL()) {
                return false;
            }
            break;
    }

    switch (target) {
        case WebGL2RenderingContextBase::DRAW_FRAMEBUFFER:
            index = BoundFrameBufferType::DRAW_FRAMEBUFFER;
            return true;
        case WebGL2RenderingContextBase::READ_FRAMEBUFFER:
            index = BoundFrameBufferType::READ_FRAMEBUFFER;
            return true;
        default:
            break;
    }
    return false;
}

bool WebGLRenderingContextBaseImpl::CheckRenderBufferTarget(napi_env env, GLenum target, uint32_t& index)
{
    switch (target) {
        case WebGLRenderingContextBase::RENDERBUFFER:
            index = BoundRenderBufferType::RENDERBUFFER;
            return true;
        default:
            if (!IsHighWebGL()) {
                return false;
            }
            break;
    }
    return false;
}

bool WebGLRenderingContextBaseImpl::CheckTextureTarget(napi_env env, GLenum target, uint32_t& index)
{
    switch (target) {
        case WebGLRenderingContextBase::TEXTURE_2D:
            index = BoundTextureType::TEXTURE_2D;
            return true;
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP:
            index = BoundTextureType::TEXTURE_CUBE_MAP;
            return true;
        default:
            if (IsHighWebGL()) {
                break;
            }
            return false;
    }

    switch (target) {
        case WebGL2RenderingContextBase::TEXTURE_3D:
            index = BoundTextureType::TEXTURE_3D;
            break;
        case WebGL2RenderingContextBase::TEXTURE_2D_ARRAY:
            index = BoundTextureType::TEXTURE_2D_ARRAY;
            break;
        default:
            return false;
    }
    return true;
}

bool WebGLRenderingContextBaseImpl::CheckShaderType(napi_env env, GLenum type, uint32_t& index)
{
    switch (type) {
        case WebGLRenderingContextBase::VERTEX_SHADER:
            index = BoundShaderType::VERTEX_SHADER;
            break;
        case WebGLRenderingContextBase::FRAGMENT_SHADER:
            index = BoundShaderType::FRAGMENT_SHADER;
            break;
        default:
            return false;
    }
    return true;
}

bool WebGLRenderingContextBaseImpl::CheckTexture2DTarget(napi_env env, GLenum target)
{
    return CheckGLenum(env, target,
        { WebGLRenderingContextBase::TEXTURE_2D, WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_X,
            WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_X,
            WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Y,
            WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Y,
            WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Z,
            WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Z },
        {});
}

bool WebGLRenderingContextBaseImpl::CheckBufferTarget(napi_env env, GLenum target, uint32_t& index)
{
    switch (target) {
        case WebGLRenderingContextBase::ARRAY_BUFFER:
            index = BoundBufferType::ARRAY_BUFFER;
            return true;
        case WebGLRenderingContextBase::ELEMENT_ARRAY_BUFFER:
            index = BoundBufferType::ELEMENT_ARRAY_BUFFER;
            return true;
        default:
            if (!IsHighWebGL()) {
                return false;
            }
            break;
    }

    switch (target) {
        case WebGL2RenderingContextBase::COPY_READ_BUFFER:
            index = BoundBufferType::COPY_READ_BUFFER;
            return true;
        case WebGL2RenderingContextBase::COPY_WRITE_BUFFER:
            index = BoundBufferType::COPY_WRITE_BUFFER;
            return true;
        case WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BUFFER:
            index = BoundBufferType::TRANSFORM_FEEDBACK_BUFFER;
            return true;
        case WebGL2RenderingContextBase::UNIFORM_BUFFER:
            index = BoundBufferType::UNIFORM_BUFFER;
            return true;
        case WebGL2RenderingContextBase::PIXEL_PACK_BUFFER:
            index = BoundBufferType::PIXEL_PACK_BUFFER;
            return true;
        case WebGL2RenderingContextBase::PIXEL_UNPACK_BUFFER:
            index = BoundBufferType::PIXEL_UNPACK_BUFFER;
            return true;
        default:
            break;
    }
    return false;
}

bool WebGLRenderingContextBaseImpl::CheckBufferDataUsage(napi_env env, GLenum usage)
{
    return CheckGLenum(env, usage, { GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW },
        { GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_READ, GL_DYNAMIC_COPY });
}

bool WebGLRenderingContextBaseImpl::CheckPixelsFormat(napi_env env, GLenum format)
{
    return CheckGLenum(env, format, { GL_ALPHA, GL_RGB, GL_RGBA }, {});
}

bool WebGLRenderingContextBaseImpl::CheckPixelsType(napi_env env, GLenum type)
{
    return CheckGLenum(env, type,
        { GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_5_5_1, GL_FLOAT,
            GL_HALF_FLOAT_OES },
        {});
}

bool WebGLRenderingContextBaseImpl::CheckString(const std::string& str)
{
    for (size_t i = 0; i < str.length(); ++i) {
        unsigned char c = str[i];
        // Horizontal tab, line feed, vertical tab, form feed, carriage return are also valid
        if (c >= 9 && c <= 13) {
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
    if (strncmp("webgl_", name.c_str(), 6) == 0 || strncmp("_webgl_", name.c_str(), 7) == 0) {
        return true;
    }
    return false;
}

bool WebGLRenderingContextBaseImpl::GetUniformExtInfo(
    napi_env env, const NFuncArg& funcArg, Impl::UniformExtInfo* info, int start)
{
    bool succ;
    if (funcArg[start] != nullptr) {
        tie(succ, info->srcOffset) = NVal(env, funcArg[start]).ToUint32();
        if (!succ) {
            return false;
        }
        LOGI("WebGL UniformMatrixInfo srcOffset = %{public}u", info->srcOffset);
    }
    if (funcArg[start + 1] != nullptr) {
        tie(succ, info->srcLength) = NVal(env, funcArg[start + 1]).ToUint32();
        if (!succ) {
            return false;
        }
        LOGI("WebGL UniformMatrixInfo srcLength = %{public}u", info->srcLength);
    }
    return true;
}

bool WebGLRenderingContextBaseImpl::CheckStencil(napi_env env)
{
    if (stencilMask_[0] != stencilMask_[1] || stencilFuncRef_[0] != stencilFuncRef_[1] ||
        stencilFuncMask_[0] != stencilFuncMask_[1]) {
        LOGE("front and back stencils settings do not match");
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return false;
    }
    return true;
}

bool WebGLRenderingContextBaseImpl::CheckLocationName(const std::string& name)
{
    if (name.size() > MAX_LOCATION_LENGTH) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return false;
    }
    if (!CheckString(name)) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return false;
    }
    if (CheckReservedPrefix(name)) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return false;
    }
    return true;
}

WebGLRenderbuffer* WebGLRenderingContextBaseImpl::CheckRenderBufferStorage(napi_env env, const TexStorageArg& arg)
{
    uint32_t index = 0;
    if (!CheckRenderBufferTarget(env, arg.target, index)) {
        LOGE("WebGL renderbufferStorage invalid target %{public}u", arg.target);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckInternalFormat(env, arg.internalFormat)) {
        LOGE("WebGL renderbufferStorage invalid internalFormat %{public}u", arg.internalFormat);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (arg.width < 0 || arg.height < 0) {
        LOGE("WebGL renderbufferStorage invalid size %{public}d %{public}d", arg.width, arg.height);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    WebGLRenderbuffer* renderBuffer = GetObjectInstance<WebGLRenderbuffer>(env, boundRenderBufferIds_[index]);
    if (renderBuffer == nullptr) {
        LOGE("WebGL renderbufferStorage can not get bound render buffer Id %{public}u", boundRenderBufferIds_[index]);
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    return renderBuffer;
}

bool WebGLRenderingContextBaseImpl::CheckProgramLinkStatus(WebGLProgram* program)
{
    if (program == nullptr) {
        return false;
    }
    GLint params = 0;
    glGetProgramiv(static_cast<GLuint>(program->GetProgramId()), WebGLRenderingContextBase::LINK_STATUS, &params);
    if (params) {
        return true;
    }
    return false;
}

const UniformTypeMap* WebGLRenderingContextBaseImpl::GetUniformTypeMap(GLenum type)
{
    const static UniformTypeMap baseTypeMap[] = {
        { GL_FLOAT, GL_FLOAT, 1, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_VEC2, GL_FLOAT, 2, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_VEC3, GL_FLOAT, 3, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_VEC4, GL_FLOAT, 4, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT2, GL_FLOAT, 4, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT3, GL_FLOAT, 9, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT4, GL_FLOAT, 16, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },

        { GL_INT, GL_INT, 1, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
        { GL_INT_VEC2, GL_INT, 2, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
        { GL_INT_VEC3, GL_INT, 3, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
        { GL_INT_VEC4, GL_INT, 4, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },

        { GL_BOOL, GL_BOOL, 1, BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN },
        { GL_BOOL_VEC2, GL_BOOL, 2, BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN },
        { GL_BOOL_VEC3, GL_BOOL, 3, BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN },
        { GL_BOOL_VEC4, GL_BOOL, 4, BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN },

        { GL_SAMPLER_2D, GL_INT, 1, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
        { GL_SAMPLER_CUBE, GL_INT, 1, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
    };

    const static UniformTypeMap webgl2BaseTypeMap[] = {
        { GL_UNSIGNED_INT, GL_UNSIGNED_INT, 1, BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32 },
        { GL_UNSIGNED_INT_VEC2, GL_UNSIGNED_INT, 2, BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32 },
        { GL_UNSIGNED_INT_VEC3, GL_UNSIGNED_INT, 3, BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32 },
        { GL_UNSIGNED_INT_VEC4, GL_UNSIGNED_INT, 4, BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32 },

        { GL_FLOAT_MAT2x3, GL_FLOAT, 6, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT2x4, GL_FLOAT, 8, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT3x2, GL_FLOAT, 6, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT3x4, GL_FLOAT, 12, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT4x2, GL_FLOAT, 8, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT4x3, GL_FLOAT, 12, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },

        { GL_SAMPLER_3D, GL_INT, 1, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
        { GL_SAMPLER_2D_ARRAY, GL_INT, 1, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
    };
    for (size_t i = 0; i < sizeof(baseTypeMap) / sizeof(baseTypeMap[0]); i++) {
        if (baseTypeMap[i].type == type) {
            return &baseTypeMap[i];
        }
    }
    if (!IsHighWebGL()) {
        return nullptr;
    }
    for (size_t i = 0; i < sizeof(webgl2BaseTypeMap) / sizeof(webgl2BaseTypeMap[0]); i++) {
        if (webgl2BaseTypeMap[i].type == type) {
            return &webgl2BaseTypeMap[i];
        }
    }
    return nullptr;
}

bool WebGLRenderingContextBaseImpl::CheckFrameBufferBoundComplete(napi_env env)
{
    // not bound
    if (!boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER]) {
        return true;
    }
    return false;
}

GLenum WebGLRenderingContextBaseImpl::CheckReadPixelsArg(const PixelsArg* arg, uint64_t bufferSize)
{
    switch (arg->format) {
        case GL_ALPHA:
        case GL_RGB:
        case GL_RGBA:
            break;
        default:
            return GL_INVALID_ENUM;
    }

    switch (arg->type) {
        case GL_UNSIGNED_BYTE:
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_4_4_4_4:
        case GL_UNSIGNED_SHORT_5_5_5_1:
        case GL_FLOAT:
        case GL_HALF_FLOAT_OES:
            break;
        default:
            return GL_INVALID_ENUM;
    }
    return 0;
}

GLenum WebGLRenderingContextBaseImpl::CheckVertexAttribPointer(napi_env env, const VertexAttribArg* vertexInfo)
{
    if (vertexInfo->index >= GetMaxVertexAttribs()) {
        LOGE("WebGL vertexAttribPointer invalid index %{public}u", vertexInfo->index);
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    // check size
    // 4 size  255 stride
    if (vertexInfo->size < 1 || vertexInfo->size > 4 || vertexInfo->stride < 0 || vertexInfo->stride > 255) {
        LOGE("WebGL vertexAttribPointer invalid size %{public}u %{public}u", vertexInfo->size, vertexInfo->stride);
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    // check offset
    WebGLBuffer* webGLBuffer = GetBoundBuffer(env, WebGLRenderingContextBase::ARRAY_BUFFER);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferSize() == 0) {
        LOGE("WebGL vertexAttribPointer can not bind buffer %{public}p ", webGLBuffer);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    // check offset
    if (vertexInfo->offset >= static_cast<GLintptr>(webGLBuffer->GetBufferSize())) {
        LOGE("WebGL vertexAttribPointer invalid offset %{public}u", vertexInfo->offset);
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    uint32_t typeSize = WebGLArg::GetWebGLDataSize(vertexInfo->type);
    if ((vertexInfo->stride & (typeSize - 1)) || (vertexInfo->offset & (typeSize - 1))) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::GetBoundFrameBufferColorFormat(napi_env env)
{
    WebGLFramebuffer *frameBuffer = GetBoundFrameBuffer(env, WebGLRenderingContextBase::FRAMEBUFFER);
    if (frameBuffer != nullptr) {
        GLint type = 0;
        GLint params = 0;
        glGetFramebufferAttachmentParameteriv(WebGLRenderingContextBase::FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type);
        glGetFramebufferAttachmentParameteriv(WebGLRenderingContextBase::FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &params);
        if (type == WebGLRenderingContextBase::RENDERBUFFER) {
            WebGLRenderbuffer *renderBuffer = GetObjectInstance<WebGLRenderbuffer>(env, params);
            if (renderBuffer) {
                return renderBuffer->GetInternalFormat();
            }
        } else if (type == WebGLRenderingContextBase::TEXTURE) {
            WebGLTexture *texture = GetObjectInstance<WebGLTexture>(env, params);
            if (texture) {
                // return texture->GetInternalFormat();
            }
        }
    }
    WebGLContextAttributes *webGlContextAttributes = webGLRenderingContext_->GetWebGlContextAttributes();
    if (webGlContextAttributes != nullptr && webGlContextAttributes->alpha_) {
        return GL_RGBA;
    }
    return GL_RGB;
}

VertexAttribInfo* WebGLRenderingContextBaseImpl::GetVertexAttribInfo(GLint index)
{
    if (index >= static_cast<GLint>(arrayVertexAttribs_.size())) {
        LOGE("Invalid index for VertexAttrib %{public}d, max %{public}d",
            index, static_cast<GLint>(arrayVertexAttribs_.size()));
        return nullptr;
    }
    return &arrayVertexAttribs_[index];
}

bool WebGLRenderingContextBaseImpl::CheckReadBufferAndGetInfo(napi_env env, GLuint &frameBufferId, GLenum* format, GLenum* type)
{
    GLenum target = IsHighWebGL() ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER;
    WebGLFramebuffer *frameBuffer = GetBoundFrameBuffer(env, target);
    if (frameBuffer) {
        /*
        const char* reason = "framebuffer incomplete";
        if (!readFramebufferBinding->onAccess(webContext(), &reason)) {
            synthesizeGLError(GL_INVALID_FRAMEBUFFER_OPERATION, functionName, reason);
            return false;
        }
        if (!readFramebufferBinding->getReadBufferFormatAndType(format, type)) {
            synthesizeGLError(GL_INVALID_OPERATION, functionName, "no image to read from");
            return false;
        }
        */
    } else {
        /*
        if (m_readBufferOfDefaultFramebuffer == GL_NONE) {
            ASSERT(isWebGL2OrHigher());
            synthesizeGLError(GL_INVALID_OPERATION, functionName, "no image to read from");
            return false;
        }
        // Obtain the default drawing buffer's format and type.
        if (format)
            *format = drawingBuffer()->getActualAttributes().alpha ? GL_RGBA : GL_RGB;
        if (type)
            *type = GL_UNSIGNED_BYTE;
        */
    }
    return true;
}

bool WebGLRenderingContextBaseImpl::CheckReadBufferMode(GLenum mode)
{
    switch (mode) {
    case WebGLRenderingContextBase::BACK:
    case WebGLRenderingContextBase::NONE:
    case WebGLRenderingContextBase::COLOR_ATTACHMENT0:
        return true;
    default:
        if (mode < WebGLRenderingContextBase::COLOR_ATTACHMENT0) {
            return false;
        }
        if (mode > static_cast<GLenum>(WebGLRenderingContextBase::COLOR_ATTACHMENT0 + GetMaxColorAttachments())) {
            return false;
        }
        break;
    }
    return true;
}

GLint WebGLRenderingContextBaseImpl::GetMaxColorAttachments()
{
    if (!maxColorAttachments_) {
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxColorAttachments_);
    }
    return maxColorAttachments_;
}

} // namespace Impl
} // namespace Rosen
} // namespace OHOS
