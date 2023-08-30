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

#include "../include/context/webgl_rendering_context_basic_base.h"
#include "../include/context/webgl_rendering_context_base.h"
#include "../include/context/webgl2_rendering_context_base.h"
#include "../include/context/webgl_context_attributes.h"
#include "../include/context/webgl_rendering_context.h"
#include "../../common/napi/n_func_arg.h"
#include "../../common/napi/n_class.h"
#include "../include/webgl/webgl_shader.h"
#include "../include/webgl/webgl_buffer.h"
#include "../include/webgl/webgl_framebuffer.h"
#include "../include/webgl/webgl_program.h"
#include "../include/webgl/webgl_renderbuffer.h"
#include "../include/webgl/webgl_texture.h"
#include "../include/webgl/webgl_uniform_location.h"
#include "../include/webgl/webgl_active_info.h"
#include "../include/webgl/webgl_shader_precision_format.h"
#include "../include/util/buffer_data.h"
#include "../include/util/log.h"
#include "../include/util/egl_manager.h"
#include "../include/util/object_source.h"
#include "../include/util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;
using handleParameterGet = napi_value (*)(napi_env env, const NFuncArg &funcArg, int64_t pname);
static const GLsizei MAX_COUNT_ATTACHED_SHADER = 128;

static WebGLObjectManager *GetWebGLProgramMaps(napi_env env, napi_value thisVar)
{
    return WebGLObjectManager::GetWebGLObjectManager(env, thisVar, WebGLObjectManager::WEBGL_OBJECT_PROGRAM);
}

static WebGLObjectManager *GetWebGLShaderMaps(napi_env env, napi_value thisVar)
{
    return WebGLObjectManager::GetWebGLObjectManager(env, thisVar, WebGLObjectManager::WEBGL_OBJECT_SHADER);
}

static WebGLObjectManager *GetWebGLBufferMaps(napi_env env, napi_value thisVar)
{
    return WebGLObjectManager::GetWebGLObjectManager(env, thisVar, WebGLObjectManager::WEBGL_OBJECT_BUFFER);
}

static WebGLObjectManager *GetWebGLFrameBufferMaps(napi_env env, napi_value thisVar)
{
    return WebGLObjectManager::GetWebGLObjectManager(env, thisVar, WebGLObjectManager::WEBGL_OBJECT_FRAME_BUFFER);
}

static WebGLObjectManager *GetWebGLRenderBufferMaps(napi_env env, napi_value thisVar)
{
    return WebGLObjectManager::GetWebGLObjectManager(env, thisVar, WebGLObjectManager::WEBGL_OBJECT_RENDER_BUFFER);
}

static WebGLObjectManager *GetWebGLTextureMaps(napi_env env, napi_value thisVar)
{
    return WebGLObjectManager::GetWebGLObjectManager(env, thisVar, WebGLObjectManager::WEBGL_OBJECT_TEXTURE);
}

static WebGLObjectManager *GetWebGLUniformLocationMaps(napi_env env, napi_value thisVar)
{
    return WebGLObjectManager::GetWebGLObjectManager(env, thisVar, WebGLObjectManager::WEBGL_OBJECT_UNIFORM_LOCATION);
}

napi_value WebGLRenderingContextBase::GetContextAttributes(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *obj = Util::GetContextObject(env, funcArg.GetThisVar(), "webgl");
    if (obj == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLContextAttributes *webGlContextAttributes = obj->webGlContextAttributes;
    if (webGlContextAttributes == nullptr) {
        webGlContextAttributes = new WebGLContextAttributes();
        obj->webGlContextAttributes = webGlContextAttributes;
    }
    NVal res = NVal::CreateObject(env);
    napi_value alpha = NVal::CreateBool(env, webGlContextAttributes->alpha).val_;
    napi_value antialias = NVal::CreateBool(env, webGlContextAttributes->antialias).val_;
    napi_value depth = NVal::CreateBool(env, webGlContextAttributes->depth).val_;
    napi_value failIfMajorPerformanceCaveat =
        NVal::CreateBool(env, webGlContextAttributes->failIfMajorPerformanceCaveat).val_;
    napi_value desynchronized = NVal::CreateBool(env, webGlContextAttributes->desynchronized).val_;
    napi_value premultipliedAlpha = NVal::CreateBool(env, webGlContextAttributes->premultipliedAlpha).val_;
    napi_value preserveDrawingBuffer = NVal::CreateBool(env, webGlContextAttributes->preserveDrawingBuffer).val_;
    napi_value stencil = NVal::CreateBool(env, webGlContextAttributes->stencil).val_;
    napi_value powerPreference = NVal::CreateUTF8String(env, webGlContextAttributes->powerPreference).val_;
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

napi_value WebGLRenderingContextBase::IsContextLost(napi_env env, napi_callback_info info)
{
    bool res = false;
    if (EglManager::GetInstance().GetEGLContext() == nullptr) {
        res = true;
    };
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::GetSupportedExtensions(napi_env env, napi_callback_info info)
{
    LOGI("WebGL getSupportedExtensions start");
    const char* extensions = eglQueryString(EglManager::GetInstance().GetEGLDisplay(), EGL_EXTENSIONS);
    string str = extensions;
    vector<string> vec;
    Util::SplitString(str, vec, " ");
    napi_value result = nullptr;
    napi_create_array_with_length(env, vec.size(), &result);
    for (vector<string>::size_type i = 0; i != vec.size(); ++i) {
        napi_set_element(env, result, i, NVal::CreateUTF8String(env, vec[i]).val_);
    }
    LOGI("WebGL EGL_VENDOR %{public}s", eglQueryString(EglManager::GetInstance().GetEGLDisplay(), EGL_VENDOR));
    LOGI("WebGL EGL_VERSION %{public}s", eglQueryString(EglManager::GetInstance().GetEGLDisplay(), EGL_VERSION));
    LOGI("WebGL EGL_EXTENSIONS  %{public}s", eglQueryString(EglManager::GetInstance().GetEGLDisplay(), EGL_EXTENSIONS));
    return result;
}

napi_value WebGLRenderingContextBase::GetExtension(napi_env env, napi_callback_info info)
{
    if (EglManager::GetInstance().GetEGLContext() == nullptr) {
        return nullptr;
    };
    const char* extensions = eglQueryString(EglManager::GetInstance().GetEGLDisplay(), EGL_EXTENSIONS);
    if (extensions == nullptr) {
        return nullptr;
    }
    return nullptr;
}

napi_value WebGLRenderingContextBase::ActiveTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL activeTexture start");
    int64_t texture;
    tie(succ, texture) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL activeTexture texture %{public}u", texture);
    glActiveTexture(static_cast<GLenum>(texture));
    LOGI("WebGL activeTexture end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::AttachShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr || funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    uint32_t programId = webGlProgram->GetProgramId();

    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGI("WebGL attachShader programId %{public}u, shaderId %{public}u", programId, shaderId);
    glAttachShader(static_cast<GLuint>(programId), static_cast<GLuint>(shaderId));
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindAttribLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindAttribLocation start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    uint32_t programId = webGlProgram->GetProgramId();

    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL bindAttribLocation programId %{public}d index %{public}u", programId, index);

    unique_ptr<char[]> name;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::THIRD]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    glBindAttribLocation(static_cast<GLuint>(programId), static_cast<GLuint>(index),
        const_cast<char *>(name.get()));
    LOGI("WebGL bindAttribLocation end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }

    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLBuffer *webGlBuffer = nullptr;
    napi_status bufferStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlBuffer);
    if (bufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int buffer = webGlBuffer->GetBuffer();
    LOGI("WebGL bindBuffer target %{public}u buffer %{public}u", target, buffer);
    glBindBuffer(static_cast<GLenum>(target), static_cast<GLuint>(buffer));
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindFramebuffer start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }

    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLFramebuffer *webGlFramebuffer = nullptr;
    napi_status framebufferStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlFramebuffer);
    if (framebufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int framebuffer = webGlFramebuffer->GetFramebuffer();
    LOGI("WebGL bindFramebuffer target %{public}u framebuffer %{public}u", target, framebuffer);
    glBindFramebuffer(static_cast<GLenum>(target), static_cast<GLuint>(framebuffer));
    LOGI("WebGL bindFramebuffer end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindRenderbuffer start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }

    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLRenderbuffer *webGlRenderbuffer = nullptr;
    napi_status renderbufferStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlRenderbuffer);
    if (renderbufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    LOGI("WebGL bindFramebuffer target %{public}u renderbuffer %{public}u", target, renderbuffer);
    glBindRenderbuffer(static_cast<GLenum>(target), static_cast<GLuint>(renderbuffer));
    LOGI("WebGL bindRenderbuffer end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindTexture start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }

    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLTexture *webGlTexture = nullptr;
    napi_status textureStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlTexture);
    if (textureStatus != napi_ok) {
        return nullptr;
    }
    unsigned int texture = webGlTexture->GetTexture();
    LOGI("WebGL bindTexture target %{public}u textureId %{public}u", target, texture);
    glBindTexture(static_cast<GLenum>(target), static_cast<GLuint>(texture));
    LOGI("WebGL bindTexture end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendColor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blendColor start");
    double red;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double green;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double blue;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double alpha;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    glBlendColor(static_cast<GLclampf>(red), static_cast<GLclampf>(green),
        static_cast<GLclampf>(blue), static_cast<GLclampf>(alpha));
    LOGI("WebGL blendColor end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendEquation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blendEquation start");
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendEquation mode %{public}u", mode);
    glBlendEquation(static_cast<GLenum>(mode));
    LOGI("WebGL blendEquation end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendEquationSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blendEquationSeparate start");
    int64_t modeRGB;
    tie(succ, modeRGB) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendEquationSeparate modeRGB %{public}u", modeRGB);
    int64_t modeAlpha;
    tie(succ, modeAlpha) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendEquationSeparate modeAlpha %{public}u", modeAlpha);
    glBlendEquationSeparate(static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    LOGI("WebGL blendEquationSeparate end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendFunc(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blendFunc start");
    int64_t sFactor;
    tie(succ, sFactor) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFunc sFactor %{public}u", sFactor);
    int64_t dFactor;
    tie(succ, dFactor) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFunc dFactor %{public}u", dFactor);
    glBlendFunc(static_cast<GLenum>(sFactor), static_cast<GLenum>(dFactor));
    LOGI("WebGL blendFunc end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendFuncSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blendFuncSeparate start");
    int64_t srcRGB;
    tie(succ, srcRGB) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFuncSeparate srcRGB %{public}u", srcRGB);
    int64_t dstRGB;
    tie(succ, dstRGB) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFuncSeparate dstRGB %{public}u", dstRGB);
    int64_t srcAlpha;
    tie(succ, srcAlpha) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFuncSeparate srcAlpha %{public}u", srcAlpha);
    int64_t dstAlpha;
    tie(succ, dstAlpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFuncSeparate dstAlpha %{public}u", dstAlpha);
    glBlendFuncSeparate(static_cast<GLenum>(srcRGB), static_cast<GLenum>(dstRGB), static_cast<GLenum>(srcAlpha),
        static_cast<GLenum>(dstAlpha));
    LOGI("WebGL blendFuncSeparate end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::CheckFramebufferStatus(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL checkFramebufferStatus start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL checkFramebufferStatus target %{public}u", target);
    int64_t res = static_cast<int64_t>(glCheckFramebufferStatus(static_cast<GLenum>(target)));
    LOGI("WebGL checkFramebufferStatus end");
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGLRenderingContextBase::Clear(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    int64_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL clear mask %{public}x", mask);
    glClear(static_cast<GLbitfield>(mask));
    return nullptr;
}

napi_value WebGLRenderingContextBase::ClearColor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    double red;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double green;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double blue;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double alpha;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL clearColor red %{public}f, green %{public}f, blue %{public}f, alpha %{public}f",
        red, green, blue, alpha);
    glClearColor(static_cast<GLclampf>(red), static_cast<GLclampf>(green),
        static_cast<GLclampf>(blue), static_cast<GLclampf>(alpha));
    return nullptr;
}

napi_value WebGLRenderingContextBase::ClearDepth(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL clearDepth start");
    double depth;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    glClearDepthf(static_cast<GLclampf>(depth));
    LOGI("WebGL clearDepth end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::ClearStencil(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL clearStencil start");
    int64_t s;
    tie(succ, s) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL clearStencil s %{public}u", s);
    glClearStencil(static_cast<GLint>(s));
    LOGI("WebGL clearStencil end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::ColorMask(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL colorMask start");
    bool red = false;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL colorMask red %{public}u", red);
    bool green = false;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL colorMask green %{public}u", green);
    bool blue = false;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL colorMask blue %{public}u", blue);
    bool alpha = false;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL colorMask alpha %{public}u", alpha);
    glColorMask(static_cast<GLboolean>(red), static_cast<GLboolean>(green), static_cast<GLboolean>(blue),
        static_cast<GLboolean>(alpha));
    LOGI("WebGL colorMask end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::CompileShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    glCompileShader(static_cast<GLuint>(shaderId));
    LOGI("WebGL compileShader shaderId %{public}u", shaderId);
    return nullptr;
}

napi_value WebGLRenderingContextBase::CopyTexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::EIGHT)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL copyTexImage2D start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexImage2D target %{public}u", target);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexImage2D level %{public}u", level);
    int64_t internalformat;
    tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexImage2D internalformat %{public}u", internalformat);
    int64_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexImage2D x %{public}u", x);
    int64_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexImage2D y %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexImage2D width %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexImage2D height %{public}u", height);
    int64_t border;
    tie(succ, border) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexImage2D border %{public}u", border);
    glCopyTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
        static_cast<GLenum>(internalformat), static_cast<GLint>(x), static_cast<GLint>(y),
        static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLint>(border));
    LOGI("WebGL copyTexImage2D end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::CopyTexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::EIGHT)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL copyTexSubImage2D start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexSubImage2D target %{public}u", target);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexSubImage2D level %{public}u", level);
    int64_t xoffset;
    tie(succ, xoffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexSubImage2D xoffset %{public}u", xoffset);
    int64_t yoffset;
    tie(succ, yoffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexSubImage2D yoffset %{public}u", yoffset);
    int64_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexSubImage2D x %{public}u", x);
    int64_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexSubImage2D y %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexSubImage2D width %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL copyTexSubImage2D height %{public}u", height);
    glCopyTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(xoffset),
        static_cast<GLint>(yoffset), static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
        static_cast<GLsizei>(height));
    LOGI("WebGL copyTexSubImage2D end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::CreateBuffer(napi_env env, napi_callback_info info)
{
    LOGI("WebGL createBuffer start");
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLObjectManager *maps = GetWebGLBufferMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLBuffer *webGlBuffer = nullptr;
    napi_value objBuffer = WebGLBuffer::CreateObjectInstance(env, &webGlBuffer).val_;
    if (webGlBuffer == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    unsigned int bufferId;
    glGenBuffers(1, &bufferId);
    webGlBuffer->SetBuffer(bufferId);
    bool ret = maps->AddObject(bufferId, objBuffer);
    LOGI("WebGL add WebGLBuffer bufferId %{public}u %{public}u", bufferId, ret);
    return objBuffer;
}

napi_value WebGLRenderingContextBase::CreateFramebuffer(napi_env env, napi_callback_info info)
{
    LOGI("WebGL createFramebuffer start");
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLObjectManager *maps = GetWebGLFrameBufferMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLFramebuffer *webGlFramebuffer = nullptr;
    napi_value objFramebuffer = WebGLFramebuffer::CreateObjectInstance(env, &webGlFramebuffer).val_;
    if (!webGlFramebuffer) {
        return NVal::CreateNull(env).val_;
    }
    unsigned int framebufferId;
    glGenFramebuffers(1, &framebufferId);
    webGlFramebuffer->SetFramebuffer(framebufferId);
    (void)maps->AddObject(framebufferId, objFramebuffer);
    LOGI("WebGL add WebGLFrameBuffer framebufferId %{public}u", framebufferId);
    return objFramebuffer;
}

napi_value WebGLRenderingContextBase::CreateProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLObjectManager *maps = GetWebGLProgramMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL createProgram start");
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
    LOGI("WebGL createProgram programId %{public}u", programId);
    webGlProgram->SetProgramId(programId);
    bool ret = maps->AddObject(programId, objProgram);
    LOGI("WebGL objectMaps add webGlProgram programId %{public}u %{public}u", programId, ret);
    return objProgram;
}

napi_value WebGLRenderingContextBase::CreateRenderbuffer(napi_env env, napi_callback_info info)
{
    LOGI("WebGL createRenderbuffer start");
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLObjectManager *maps = GetWebGLRenderBufferMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderbuffer *webGlRenderbuffer = nullptr;
    napi_value objRenderbuffer = WebGLRenderbuffer::CreateObjectInstance(env, &webGlRenderbuffer).val_;
    if (!webGlRenderbuffer) {
        return NVal::CreateNull(env).val_;
    }
    GLuint renderbufferId;
    glGenRenderbuffers(1, &renderbufferId);
    webGlRenderbuffer->SetRenderbuffer(renderbufferId);
    bool ret = maps->AddObject(renderbufferId, objRenderbuffer);
    LOGI("WebGL add WebGLRenderbuffer renderbufferId %{public}u %{public}u", renderbufferId, ret);
    return objRenderbuffer;
}

napi_value WebGLRenderingContextBase::CreateShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLObjectManager *maps = GetWebGLShaderMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        LOGI("WebGL createShader fail to get WebGLShaderMaps");
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL createShader type %40{public}x", type);
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
    (void)maps->AddObject(shaderId, objShader);
    return objShader;
}

napi_value WebGLRenderingContextBase::CreateTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL createTexture start");
    WebGLObjectManager *maps = GetWebGLTextureMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLTexture *webGlTexture = nullptr;
    napi_value objTexture = WebGLTexture::CreateObjectInstance(env, &webGlTexture).val_;
    if (!webGlTexture) {
        return NVal::CreateNull(env).val_;
    }
    GLuint textureId;
    glGenTextures(1, &textureId);
    webGlTexture->SetTexture(textureId);
    LOGI("WebGL createTexture textureId %{public}u", textureId);
    (void)maps->AddObject(textureId, objTexture);
    LOGI("WebGL add WebGLTexture textureId %{public}u ", textureId);
    return objTexture;
}

napi_value WebGLRenderingContextBase::CullFace(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL cullFace start");
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL cullFace mode %{public}u", mode);
    glCullFace(static_cast<GLenum>(mode));
    LOGI("WebGL cullFace end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLBuffer *webGlBuffer = nullptr;
    napi_status bufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlBuffer);
    if (bufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int buffer = webGlBuffer->GetBuffer();
    WebGLObjectManager *maps = GetWebGLBufferMaps(env, funcArg.GetThisVar());
    if (maps != nullptr) {
        maps->DeleteObject(buffer);
        LOGI("WebGL delete WebGLBuffer bufferId %{public}u", buffer);
    }
    glDeleteBuffers(1, &buffer);
    LOGI("WebGL deleteBuffer bufferId %{public}u", buffer);
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLFramebuffer *webGlFramebuffer = nullptr;
    napi_status framebufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlFramebuffer);
    if (framebufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int framebuffer = webGlFramebuffer->GetFramebuffer();
    glDeleteFramebuffers(1, &framebuffer);
    WebGLObjectManager *maps = GetWebGLFrameBufferMaps(env, funcArg.GetThisVar());
    if (maps != nullptr) {
        maps->DeleteObject(framebuffer);
    }
    LOGI("WebGL deleteFramebuffer framebufferId %{public}u", framebuffer);
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    WebGLObjectManager *objectMaps = GetWebGLProgramMaps(env, funcArg.GetThisVar());
    if (objectMaps != nullptr) {
        objectMaps->DeleteObject(programId);
    }
    glDeleteProgram(static_cast<GLuint>(programId));
    LOGI("WebGL glDeleteProgram programId %{public}u", programId);
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLRenderbuffer *webGlRenderbuffer = nullptr;
    napi_status renderbufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlRenderbuffer);
    if (renderbufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    WebGLObjectManager *objectMaps = GetWebGLRenderBufferMaps(env, funcArg.GetThisVar());
    if (objectMaps != nullptr) {
        objectMaps->DeleteObject(renderbuffer);
        LOGI("WebGL delete WebGLRenderbuffer renderbufferId %{public}u", renderbuffer);
    }
    glDeleteRenderbuffers(1, &renderbuffer);
    LOGI("WebGL glDeleteRenderbuffers renderbuffer %{public}u", renderbuffer);
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    WebGLObjectManager *maps = GetWebGLShaderMaps(env, funcArg.GetThisVar());
    if (maps != nullptr) {
        maps->DeleteObject(shaderId);
        LOGI("WebGL delete WebGLShader shaderId %{public}u", shaderId);
    }
    glDeleteShader(static_cast<GLuint>(shaderId));
    LOGI("WebGL glDeleteShader shaderId %{public}u", shaderId);
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLTexture *webGlTexture = nullptr;
    napi_status textureStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlTexture);
    if (textureStatus != napi_ok) {
        return nullptr;
    }
    unsigned int texture = webGlTexture->GetTexture();
    WebGLObjectManager *maps = GetWebGLTextureMaps(env, funcArg.GetThisVar());
    if (maps != nullptr) {
        maps->DeleteObject(texture);
        LOGI("WebGL delete WebGLTexture textureId %{public}u", texture);
    }
    glDeleteTextures(1, &texture);
    LOGI("WebGL glDeleteTextures texture %{public}u", texture);
    return nullptr;
}

napi_value WebGLRenderingContextBase::DepthFunc(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    GLenum func;
    tie(succ, func) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL depthFunc func %{public}u", func);
    glDepthFunc(func);
    return nullptr;
}

napi_value WebGLRenderingContextBase::DepthMask(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    bool flag = false;
    tie(succ, flag) = NVal(env, funcArg[NARG_POS::FIRST]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL depthMask flag %{public}u", flag);
    glDepthMask(static_cast<GLboolean>(flag));
    return nullptr;
}

napi_value WebGLRenderingContextBase::DepthRange(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    double zNear;
    tie(succ, zNear) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double zFar;
    tie(succ, zFar) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL depthRange zNear %{public}f  zFar %{public}f ", zNear, zFar);
    glDepthRangef(static_cast<GLclampf>(zNear), static_cast<GLclampf>(zFar));
    return nullptr;
}

napi_value WebGLRenderingContextBase::DetachShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr || funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    uint32_t programId = webGlProgram->GetProgramId();

    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGI("WebGL glDetachShader programId %{public}u, shaderId %{public}u", programId, shaderId);
    glDetachShader(static_cast<GLuint>(programId), static_cast<GLuint>(shaderId));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Disable(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    GLenum cap;
    tie(succ, cap) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL disable cap %{public}u", cap);
    glDisable(cap);
    return nullptr;
}

napi_value WebGLRenderingContextBase::DisableVertexAttribArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL disableVertexAttribArray index %{public}u", index);
    glDisableVertexAttribArray(static_cast<GLuint>(index));
    return nullptr;
}

napi_value WebGLRenderingContextBase::DrawArrays(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL drawArrays mode %{public}u", mode);
    int64_t first;
    tie(succ, first) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL drawArrays first %{public}u", first);
    int64_t count;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL drawArrays count %{public}u", count);
    glDrawArrays(static_cast<GLenum>(mode), static_cast<GLint>(first), static_cast<GLsizei>(count));
    LOGI("WebGL drawArrays end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DrawElements(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL drawElements mode %{public}u", mode);
    int64_t count;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL drawElements count %{public}u", count);
    int64_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL drawElements type %{public}u", type);
    int64_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL drawElements offset %{public}u", offset);
    glDrawElements(static_cast<GLenum>(mode), static_cast<GLsizei>(count), static_cast<GLenum>(type),
        reinterpret_cast<GLvoid *>(static_cast<intptr_t>(offset)));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Enable(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    GLenum cap;
    tie(succ, cap) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL enable cap %{public}u", cap);
    glEnable(cap);
    return nullptr;
}

napi_value WebGLRenderingContextBase::EnableVertexAttribArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL enableVertexAttribArray index %{public}u", index);
    glEnableVertexAttribArray(static_cast<GLuint>(index));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Finish(napi_env env, napi_callback_info info)
{
    glFinish();
    LOGI("WebGL finish end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Flush(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *obj = Util::GetContextObject(env, funcArg.GetThisVar(), "webgl");
    if (obj == nullptr) {
        LOGE("WebGL flush obj == nullptr");
        return nullptr;
    }
    glFlush();
    obj->Update();
    LOGI("WebGL flush end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::FramebufferRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FOURTH] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL framebufferRenderbuffer target %{public}u", target);

    int64_t attachment;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL framebufferRenderbuffer attachment %{public}u", attachment);

    int64_t renderBufferTarget;
    tie(succ, renderBufferTarget) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }

    WebGLRenderbuffer *webGlRenderbuffer = nullptr;
    napi_status renderbufferStatus = napi_unwrap(env, funcArg[NARG_POS::FOURTH], (void **) &webGlRenderbuffer);
    if (renderbufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    LOGI("WebGL framebufferRenderbuffer renderbuffer %{public}u", renderbuffer);
    glFramebufferRenderbuffer(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderBufferTarget), static_cast<GLuint>(renderbuffer));
    return nullptr;
}


napi_value WebGLRenderingContextBase::FramebufferTexture2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FOURTH] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL framebufferTexture2D target %{public}u", target);

    int64_t attachment;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL framebufferTexture2D attachment %{public}u", attachment);

    int64_t textarget;
    tie(succ, textarget) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL framebufferTexture2D textarget %{public}u", textarget);
    unsigned int texture = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        WebGLTexture *webGlTexture = nullptr;
        napi_status textureStatus = napi_unwrap(env, funcArg[NARG_POS::FOURTH], (void **) &webGlTexture);
        if (textureStatus != napi_ok) {
            return nullptr;
        }
        texture = webGlTexture->GetTexture();
    }
    LOGI("WebGL framebufferTexture2D texture %{public}u", texture);

    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL framebufferTexture2D level %{public}u", level);
    glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget), static_cast<GLuint>(texture), static_cast<GLint>(level));
    return nullptr;
}

napi_value WebGLRenderingContextBase::GetUniformLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    LOGI("WebGL getUniformLocation programId %{public}u", programId);
    unique_ptr<char[]> name;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL getUniformLocation name %{public}s", name.get());

    GLint locationId = glGetUniformLocation(static_cast<GLuint>(programId), const_cast<char *>(name.get()));
    LOGI("WebGL getUniformLocation locationId %{public}d", locationId);
    if (locationId == -1) {
        return NVal::CreateNull(env).val_;
    }
    // check if exit
    WebGLObjectManager *maps = GetWebGLUniformLocationMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        LOGI("WebGL getUniformLocation locationId %{public}d get fail", locationId);
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL getUniformLocation locationId %{public}d ", locationId);
    napi_value objUniformLocation = maps->GetNapiValue(locationId);
    if (objUniformLocation == nullptr) { // create new
        WebGLUniformLocation *webGLUniformLocation = nullptr;
        objUniformLocation = WebGLUniformLocation::CreateObjectInstance(env, &webGLUniformLocation).val_;
        if (!objUniformLocation) {
            LOGI("WebGL getUniformLocation locationId %{public}d null", locationId);
            return NVal::CreateNull(env).val_;
        }
        webGLUniformLocation->SetUniformLocationId(locationId);
        maps->AddObject(locationId, objUniformLocation);
        LOGI("WebGL getUniformLocation locationId %{public}d create", locationId);
    }
    return objUniformLocation;
}

napi_value WebGLRenderingContextBase::GetVertexAttribOffset(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    uint32_t pnameindex;
    tie(succ, pnameindex) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getVertexAttribOffset index %{public}u", pnameindex);
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getVertexAttribOffset pname %{public}u", pname);
    GLvoid *point = nullptr;
    glGetVertexAttribPointerv(static_cast<GLint>(pnameindex), pname, &point);
    LOGI("WebGL getVertexAttribOffset point %{public}p", point);
    return NVal::CreateInt64(env, reinterpret_cast<int64_t>(point)).val_;
}

napi_value WebGLRenderingContextBase::ShaderSource(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGI("WebGL shaderSource shaderId %{public}u", shaderId);
    unique_ptr<char[]> source;
    tie(succ, source, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }

    LOGI("WebGL shaderSource source %{public}s", source.get());
    const char *shaderString = source.get();
    glShaderSource(static_cast<GLuint>(shaderId), 1, &shaderString, nullptr);
    webGlShader->SetShaderRes(shaderString);
    return nullptr;
}

napi_value WebGLRenderingContextBase::Hint(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL hint target %{public}u", target);
    GLenum mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL hint mode %{public}u", mode);
    glHint(static_cast<GLenum>(target), static_cast<GLenum>(mode));
    return nullptr;
}

napi_value WebGLRenderingContextBase::IsBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    WebGLBuffer *webGlBuffer = nullptr;
    napi_status bufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlBuffer);
    if (bufferStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int buffer = webGlBuffer->GetBuffer();
    res = static_cast<bool>(glIsBuffer(static_cast<GLuint>(buffer)));
    LOGI("WebGL isBuffer res %{public}u  %{public}u", res, buffer);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsEnabled(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    bool succ = false;
    GLenum cap;
    tie(succ, cap) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateBool(env, res).val_;
    }
    res = static_cast<bool>(glIsEnabled(static_cast<GLenum>(cap)));
    LOGI("WebGL isEnabled cap %{public}u res %{public}u", cap, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::RenderbufferStorage(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL renderbufferStorage target %{public}u", target);
    GLenum internalformat;
    tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL renderbufferStorage internalformat %{public}u", internalformat);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL renderbufferStorage width %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL renderbufferStorage height %{public}u", height);
    glRenderbufferStorage(static_cast<GLenum>(target), static_cast<GLenum>(internalformat),
                          static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    return nullptr;
}

napi_value WebGLRenderingContextBase::SampleCoverage(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    double value;
    tie(succ, value) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL sampleCoverage value %{public}f", value);
    bool invert = false;
    tie(succ, invert) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL sampleCoverage invert %{public}u", invert);
    glSampleCoverage(static_cast<GLclampf>(value), static_cast<GLboolean>(invert));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Scissor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    int32_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL scissor x %{public}u", x);
    int32_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL scissor y %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL scissor width %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL scissor height %{public}u", height);
    glScissor(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
              static_cast<GLsizei>(height));
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilFunc(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL stencilFunc start");
    int32_t func;
    tie(succ, func) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilFunc func %{public}u", func);
    int32_t ref;
    tie(succ, ref) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilFunc ref %{public}u", ref);
    uint32_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::THIRD]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilFunc mask %{public}u", mask);
    glStencilFunc(static_cast<GLenum>(func), static_cast<GLint>(ref), static_cast<GLuint>(mask));
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilFuncSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL stencilFuncSeparate start");
    GLenum face;
    tie(succ, face) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilFuncSeparate face %{public}u", face);
    GLenum func;
    tie(succ, func) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilFuncSeparate func %{public}u", func);
    int32_t ref;
    tie(succ, ref) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilFuncSeparate ref %{public}u", ref);
    uint32_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::FOURTH]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilFuncSeparate mask %{public}u", mask);
    glStencilFuncSeparate(static_cast<GLenum>(face), static_cast<GLenum>(func), static_cast<GLint>(ref),
                          static_cast<GLuint>(mask));
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilMask(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    uint32_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilMask mask %{public}u", mask);
    glStencilMask(static_cast<GLuint>(mask));
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilMaskSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum face;
    tie(succ, face) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilMaskSeparate face %{public}u", face);
    uint32_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilMaskSeparate mask %{public}u", mask);
    glStencilMaskSeparate(static_cast<GLenum>(face), static_cast<GLuint>(mask));
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilOp(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL stencilOp start");
    GLenum fail;
    tie(succ, fail) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilOp fail %{public}u", fail);
    GLenum zfail;
    tie(succ, zfail) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilOp zfail %{public}u", zfail);
    GLenum zpass;
    tie(succ, zpass) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilOp zpass %{public}u", zpass);
    glStencilOp(fail, zfail, zpass);
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilOpSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    GLenum face;
    tie(succ, face) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilOpSeparate face %{public}u", face);
    GLenum sfail;
    tie(succ, sfail) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilOpSeparate sfail %{public}u", sfail);
    GLenum dpfail;
    tie(succ, dpfail) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilOpSeparate dpfail %{public}u", dpfail);
    GLenum dppass;
    tie(succ, dppass) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL stencilOpSeparate dppass %{public}u", dppass);
    glStencilOpSeparate(face, sfail, dpfail, dppass);
    return nullptr;
}

napi_value WebGLRenderingContextBase::TexParameterf(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL texParameterf target %{public}u", target);
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL texParameterf pname %{public}u", pname);
    double param;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL texParameterf param %{public}f", param);
    glTexParameterf(target, pname, static_cast<GLfloat>(param));
    return nullptr;
}

napi_value WebGLRenderingContextBase::TexParameteri(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL texParameteri start");
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL texParameteri target %{public}u", target);
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL texParameteri pname %{public}u", pname);
    int32_t param;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL texParameteri param %{public}u", param);
    glTexParameteri(target, pname, static_cast<GLint>(param));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform1f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    int location = 0;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    location = webGLUniformLocation->GetUniformLocationId();

    double v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform1f v0 %{public}f location %{public}d", v0, location);
    glUniform1f(static_cast<GLint>(location), static_cast<GLfloat>(v0));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform2f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        LOGI("WebGL uniform2f webGLUniformLocation null");
        return nullptr;
    }
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    double v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform2f v0 %{public}f location %{public}d", v0, location);
    double v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform2f v1 %{public}f", v1);
    glUniform2f(static_cast<GLint>(location), static_cast<GLfloat>(v0), static_cast<GLfloat>(v1));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform3f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        LOGI("WebGL uniform3f webGLUniformLocation null");
        return nullptr;
    }
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    double v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform3f v0 %{public}f", v0);
    double v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform3f v1 %{public}f", v1);
    double v2;
    tie(succ, v2) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform3f v2 %{public}f", v2);
    glUniform3f(static_cast<GLint>(location),
        static_cast<GLfloat>(v0), static_cast<GLfloat>(v1), static_cast<GLfloat>(v2));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform4f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        LOGI("WebGL uniform4f webGLUniformLocation null");
        return nullptr;
    }
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    double v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform4f v0 %{public}f", v0);
    double v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform4f v1 %{public}f", v1);
    double v2;
    tie(succ, v2) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform4f v2 %{public}f", v2);
    double v3;
    tie(succ, v3) = NVal(env, funcArg[NARG_POS::FIFTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform4f v3 %{public}f", v3);
    glUniform4f(static_cast<GLint>(location),
        static_cast<GLfloat>(v0), static_cast<GLfloat>(v1), static_cast<GLfloat>(v2), static_cast<GLfloat>(v3));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform1i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        LOGI("WebGL uniform1i webGLUniformLocation null");
        return nullptr;
    }
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform1i v0 %{public}f location %{public}d", v0, location);
    glUniform1i(static_cast<GLint>(location), static_cast<GLint>(v0));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform2i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        LOGI("WebGL uniform2i webGLUniformLocation null");
        return nullptr;
    }
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform2i v0 %{public}u", v0);
    int32_t v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform2i v1 %{public}f location %{public}d", v1, location);
    glUniform2i(static_cast<GLint>(location), static_cast<GLint>(v0), static_cast<GLint>(v1));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform3i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    int location = 0;
    if (funcArg[NARG_POS::FIRST] != nullptr) { // TODO
        WebGLUniformLocation *webGLUniformLocation = nullptr;
        napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
        if (locationStatus != napi_ok) {
            return nullptr;
        }
        location = webGLUniformLocation->GetUniformLocationId();
    }
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform3i v0 %{public}u", v0);
    int32_t v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform3i v1 %{public}u", v1);
    int32_t v2;
    tie(succ, v2) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform3i v2 %{public}d location %{public}d", v2, location);
    glUniform3i(static_cast<GLint>(location), static_cast<GLint>(v0), static_cast<GLint>(v1),
                static_cast<GLint>(v2));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform4i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) { // TODO
        LOGI("WebGL uniform4i webGLUniformLocation null");
        return nullptr;
    }
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform4i v0 %{public}u", v0);
    int32_t v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform4i v1 %{public}u", v1);
    int32_t v2;
    tie(succ, v2) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform4i v2 %{public}u", v2);
    int32_t v3;
    tie(succ, v3) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniform4i v3 %{public}f location %{public}d", v3, location);
    glUniform4i(static_cast<GLint>(location), static_cast<GLint>(v0), static_cast<GLint>(v1),
                static_cast<GLint>(v2), static_cast<GLint>(v3));
    return nullptr;
}

napi_value WebGLRenderingContextBase::UseProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    uint32_t program = webGlProgram->GetProgramId();
    glUseProgram(static_cast<GLuint>(program));
    LOGI("WebGL useProgram programId %{public}u", program);
    return nullptr;
}

napi_value WebGLRenderingContextBase::ValidateProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    uint32_t program = webGlProgram->GetProgramId();
    LOGI("WebGL validateProgram program %{public}u", program);
    glValidateProgram(static_cast<GLuint>(program));
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib1f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLuint index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib1f index %{public}u", index);
    double v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib1f v0 %{public}f", v0);
    glVertexAttrib1f(static_cast<GLuint>(index), static_cast<GLfloat>(v0));
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib2f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib2f index %{public}u", index);
    double x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib2f x %{public}f", x);
    double y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib2f y %{public}f", y);
    glVertexAttrib2f(static_cast<GLint>(index), static_cast<GLfloat>(x), static_cast<GLfloat>(y));
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib3f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib3f index %{public}u", index);
    double x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib3f x %{public}f", x);
    double y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib3f y %{public}f", y);
    double z;
    tie(succ, z) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib3f z %{public}f", z);
    glVertexAttrib3f(static_cast<GLuint>(index), static_cast<GLfloat>(x), static_cast<GLfloat>(y),
                     static_cast<GLfloat>(z));
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib4f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4f index %{public}u", index);
    double x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4f x %{public}f", x);
    double y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4f y %{public}f", y);
    double z;
    tie(succ, z) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4f z %{public}f", z);
    double w;
    tie(succ, w) = NVal(env, funcArg[NARG_POS::FIFTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4f w %{public}f", w);
    glVertexAttrib4f(static_cast<GLuint>(index), static_cast<GLfloat>(x), static_cast<GLfloat>(y),
                     static_cast<GLfloat>(z), static_cast<GLfloat>(w));
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttribPointer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::SIX)) {
        return nullptr;
    }
    bool succ = false;
    GLuint index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttribPointer index %{public}u", index);
    int32_t size;
    tie(succ, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttribPointer size %{public}u", size);
    GLenum type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttribPointer type %{public}u", type);
    bool normalized = false;
    tie(succ, normalized) = NVal(env, funcArg[NARG_POS::FOURTH]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttribPointer normalized %{public}u", normalized);
    int64_t stride;
    tie(succ, stride) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttribPointer stride %{public}u", stride);
    int64_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttribPointer offset %{public}u", offset);
    glVertexAttribPointer(static_cast<GLuint>(index), static_cast<GLint>(size), static_cast<GLenum>(type),
                          static_cast<GLboolean>(normalized), static_cast<GLsizei>(stride),
                          reinterpret_cast<GLvoid *>(static_cast<intptr_t>(offset)));
    return nullptr;
}

napi_value WebGLRenderingContextBase::Viewport(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    int32_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("webgl viewport x %{public}u", x);
    int32_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("webgl viewport y %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("webgl viewport width %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("webgl viewport height %{public}u", height);
    glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
               static_cast<GLsizei>(height));
    return nullptr;
}

napi_value WebGLRenderingContextBase::IsFramebuffer(napi_env env, napi_callback_info info)
{
    LOGI("WebGL isFramebuffer into one");
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    WebGLFramebuffer *webGLFramebuffer = nullptr;
    napi_status framebufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLFramebuffer);
    if (framebufferStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int framebuffer = webGLFramebuffer->GetFramebuffer();
    res = static_cast<bool>(glIsFramebuffer(static_cast<GLuint>(framebuffer)));
    LOGI("WebGL isFramebuffer framebuffer %{public}u res %{public}u", framebuffer, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    WebGLProgram *webGLProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLProgram);
    if (programStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    uint32_t program = webGLProgram->GetProgramId();
    res = static_cast<bool>(glIsProgram(static_cast<GLuint>(program)));
    LOGI("WebGL isProgram programId %{public}u res %{public}u", program, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    LOGI("WebGL isRenderbuffer start");
    WebGLRenderbuffer *webGLRenderbuffer = nullptr;
    napi_status renderbufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLRenderbuffer);
    if (renderbufferStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int renderbuffer = webGLRenderbuffer->GetRenderbuffer();
    res = static_cast<bool>(glIsRenderbuffer(static_cast<GLuint>(renderbuffer)));
    LOGI("WebGL isRenderbuffer renderbuffer %{public}u res %{public}u", renderbuffer, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    WebGLShader *webGLShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLShader);
    if (shaderStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    uint32_t shaderId = webGLShader->GetShaderId();
    res = static_cast<bool>(glIsShader(static_cast<GLuint>(shaderId)));
    LOGI("WebGL isShader shaderId %{public}u  res %{public}u", shaderId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    LOGI("WebGL isTexture start");
    WebGLTexture *webGLTexture = nullptr;
    napi_status textureStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLTexture);
    if (textureStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int texture = webGLTexture->GetTexture();
    res = static_cast<bool>(glIsTexture(static_cast<GLuint>(texture)));
    LOGI("WebGL isTexture texture %{public}u  res %{public}u", texture, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::LineWidth(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    double linewidth;
    tie(succ, linewidth) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL lineWidth linewidth %{public}f", linewidth);
    glLineWidth(static_cast<GLfloat>(linewidth));
    return nullptr;
}

napi_value WebGLRenderingContextBase::LinkProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGLProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    uint32_t linkprogram = webGLProgram->GetProgramId();
    LOGI("WebGL linkProgram programId %{public}u", linkprogram);
    glLinkProgram(static_cast<GLuint>(linkprogram));
    return nullptr;
}

napi_value WebGLRenderingContextBase::PixelStorei(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL pixelStorei pname %{public}u", pname);
    int32_t param = 0;
    bool isbool = NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_boolean);
    if (isbool) {
        bool parambool = false;
        tie(succ, parambool) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
        if (succ) {
            return nullptr;
        }
        param = parambool ? 1 : 0;
    } else {
        tie(succ, param) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return nullptr;
        }
    }
    LOGI("WebGL pixelStorei pname %{public}u", param);
    glPixelStorei(static_cast<GLenum>(pname), static_cast<GLint>(param));
    return nullptr;
}

napi_value WebGLRenderingContextBase::PolygonOffset(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    double factor;
    tie(succ, factor) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL polygonOffset factor %{public}f", factor);
    double units;
    tie(succ, units) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL polygonOffset units %{public}f", units);
    glPolygonOffset(static_cast<GLfloat>(factor), static_cast<GLfloat>(units));
    return nullptr;
}

napi_value WebGLRenderingContextBase::FrontFace(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL frontFace mode %{public}u", mode);
    glFrontFace(static_cast<GLenum>(mode));
    return nullptr;
}

napi_value WebGLRenderingContextBase::GenerateMipmap(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL generateMipmap target %{public}u", target);
    glGenerateMipmap(static_cast<GLenum>(target));
    return nullptr;
}

static napi_value CreateWebGLActiveInfo(napi_env env, napi_callback_info info,
    void (*getActiveInfo)(uint32_t programId, int64_t index, GLint &size, GLenum &type, GLchar *name))
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        LOGE("WebGL getActiveAttrib invalid param");
        return NVal::CreateNull(env).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    LOGI("WebGL CreateWebGLActiveInfo programId %{public}u", programId);
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL CreateWebGLActiveInfo index %{public}u", index);
    WebGLActiveInfo *webGLActiveInfo = nullptr;
    napi_value objActiveInfo = WebGLActiveInfo::CreateObjectInstance(env, &webGLActiveInfo).val_;
    if (!objActiveInfo) {
        return NVal::CreateNull(env).val_;
    }
    GLint size;
    GLenum type;
    GLchar name[WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH + 1] = {0};
    getActiveInfo(programId, index, size, type, name);
    webGLActiveInfo->SetActiveName(name);
    webGLActiveInfo->SetActiveType(type);
    webGLActiveInfo->SetActiveSize(size);
    return objActiveInfo; // TODO
}

napi_value WebGLRenderingContextBase::GetActiveAttrib(napi_env env, napi_callback_info info)
{
    return CreateWebGLActiveInfo(env, info, [](uint32_t programId, int64_t index, GLint &size, GLenum &type, GLchar *name) {
        GLsizei bufSize = WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH;
        GLsizei length;
        glGetActiveAttrib(static_cast<GLuint>(programId), static_cast<GLuint>(index), bufSize, &length, &size, &type, name);
        if (length > WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH) {
            LOGE("WebGL: getActiveAttrib: [error] bufSize exceed!");
        }
        LOGI("WebGL getActiveAttrib %{public}s %{public}u %{public}d %{public}u", name, type, size, length);
    });
}

napi_value WebGLRenderingContextBase::GetActiveUniform(napi_env env, napi_callback_info info)
{
    return CreateWebGLActiveInfo(env, info, [](uint32_t programId, int64_t index, GLint &size, GLenum &type, GLchar *name) {
        GLsizei bufSize = WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH;
        GLsizei length;
        glGetActiveUniform(static_cast<GLuint>(programId), static_cast<GLuint>(index), bufSize, &length, &size, &type, name);
        if (length > WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH) {
            LOGE("WebGL: getActiveUniform: [error] bufSize exceed!");
        }
        LOGI("WebGL GetActiveUniform %{public}s %{public}d %{public}d", name, type, size);
    });
}

napi_value WebGLRenderingContextBase::GetBufferParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getBufferParameter target 0x%{public}x", target);
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    GLint params;
    glGetBufferParameteriv(target, pname, &params);
    LOGI("WebGL getBufferParameter pname 0x%{public}x params %{public}d", pname, params);
    if (pname == GL_BUFFER_SIZE || pname == GL_BUFFER_USAGE) {
        int64_t res = static_cast<int64_t>(params);
        return NVal::CreateInt64(env, res).val_;
    } else { // TODO
        LOGI("WebGL getBufferParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetError(napi_env env, napi_callback_info info)
{
    // TODO 怎么记录内部错误
    NFuncArg funcArg(env, info);
    int64_t res = static_cast<int64_t>(glGetError());
    LOGI("WebGL getError res %{public}u", res);
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGLRenderingContextBase::GetFramebufferAttachmentObjName(
    napi_env env, napi_value thisVar, GLenum target, GLenum attachment, GLenum pname)
{
    GLint type;
    glGetFramebufferAttachmentParameteriv(target, attachment, FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type);
    if (type == WebGLRenderingContextBase::RENDERBUFFER) {
        WebGLObjectManager *maps = GetWebGLRenderBufferMaps(env, thisVar);
        if (maps == nullptr) {
            return nullptr;
        }
        GLint framebufferId;
        glGetFramebufferAttachmentParameteriv(target, attachment, pname, &framebufferId);
        LOGI("WebGL GetFramebufferAttachmentObjName framebufferId %{public}u", framebufferId);
        return maps->GetObject(framebufferId);
    } else if (type == WebGLRenderingContextBase::TEXTURE) {
        WebGLObjectManager *maps = GetWebGLTextureMaps(env, thisVar);
        if (maps == nullptr) {
            return nullptr;
        }
        GLint textureId;
        glGetFramebufferAttachmentParameteriv(target, attachment, pname, &textureId);
        LOGI("WebGL GetFramebufferAttachmentObjName textureId %{public}u", textureId);
        return maps->GetObject(textureId);
    }
    LOGI("WebGL GetFramebufferAttachmentObjName : no image is attached type %{public}u", type);
    return nullptr;
}

napi_value WebGLRenderingContextBase::GetFramebufferAttachmentParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getFramebufferAttachmentParameter target %{public}u", target);
    GLenum attachment;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getFramebufferAttachmentParameter attachment %{public}u", attachment);
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getFramebufferAttachmentParameter pname %{public}u", pname);
    if (pname == FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE ||
        pname == FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL ||
        pname == FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_BLUE_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_GREEN_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_RED_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER) {
        GLint params;
        glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
        LOGI("WebGL getFramebufferAttachmentParameter res %{public}u", params);
        return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
    } else if (pname == FRAMEBUFFER_ATTACHMENT_OBJECT_NAME) {
        return GetFramebufferAttachmentObjName(env, funcArg.GetThisVar(), target, attachment, pname);
    } else {
        GLint params;
        glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
        LOGI("WebGL getFramebufferAttachmentParameter : pname is wrong %{public}u", pname);
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetProgramParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    LOGI("WebGL getProgramParameter programId %{public}u", programId);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    // TODO 如何适配webgl2的版本
    GLint params = 0;
    glGetProgramiv(static_cast<GLuint>(programId), static_cast<GLenum>(pname), &params);
    LOGI("WebGL getProgramParameter pname %{public}x params %{public}d", pname, params);
    if (pname == GL_DELETE_STATUS || pname == GL_LINK_STATUS || pname == GL_VALIDATE_STATUS) {
        bool res = (params == GL_FALSE) ? false : true;
        return NVal::CreateBool(env, res).val_;
    } else if (pname == GL_ATTACHED_SHADERS || pname == GL_ACTIVE_ATTRIBUTES || pname == GL_ACTIVE_UNIFORMS) {
        int64_t res = static_cast<int64_t>(params);
        return NVal::CreateInt64(env, res).val_;
    } else {
        LOGI("WebGL getProgramParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetRenderbufferParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getRenderbufferParameter start");
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getRenderbufferParameter target %{public}u", target);
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    GLint params;
    glGetRenderbufferParameteriv(target, pname, &params);
    LOGI("WebGL getRenderbufferParameter pname %{public}u params %{public}d", pname, params);
    if (pname == GL_RENDERBUFFER_WIDTH ||
        pname == GL_RENDERBUFFER_HEIGHT ||
        pname == GL_RENDERBUFFER_INTERNAL_FORMAT ||
        pname == GL_RENDERBUFFER_RED_SIZE ||
        pname == GL_RENDERBUFFER_GREEN_SIZE ||
        pname == GL_RENDERBUFFER_BLUE_SIZE ||
        pname == GL_RENDERBUFFER_ALPHA_SIZE ||
        pname == GL_RENDERBUFFER_DEPTH_SIZE ||
        pname == GL_RENDERBUFFER_STENCIL_SIZE) {
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL getRenderbufferParameter params %{public}u", params);
        return NVal::CreateInt64(env, res).val_;
    } else {
        LOGI("WebGL getRenderbufferParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetTexParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getTexParameter start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getTexParameter target %{public}u", target);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getTexParameter pname %{public}u", pname);
    if (pname == GL_TEXTURE_MAX_ANISOTROPY_EXT) {
        GLfloat params;
        glGetTexParameterfv(static_cast<GLenum>(target), static_cast<GLenum>(pname), &params);
        float res = static_cast<float>(params);
        LOGI("WebGL getTexParameter params %{public}u", params);
        return NVal::CreateDouble(env, (double)res).val_;
    } else if (pname == GL_TEXTURE_MAG_FILTER || pname == GL_TEXTURE_MIN_FILTER ||
               pname == GL_TEXTURE_WRAP_S || pname == GL_TEXTURE_WRAP_T) {
        GLint params;
        glGetTexParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), &params);
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL getTexParameter params %{public}u", params);
        return NVal::CreateInt64(env, res).val_;
    } else { // get error
        GLfloat params;
        glGetTexParameterfv(static_cast<GLenum>(target), static_cast<GLenum>(pname), &params);
        LOGI("WebGL getTexParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetShaderParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGI("WebGL getShaderParameter shaderId %{public}u", shaderId);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    GLint params;
    glGetShaderiv(static_cast<GLuint>(shaderId), static_cast<GLenum>(pname), &params);
    LOGI("WebGL getShaderParameter pname %{public}u params %{public}u", pname, params);
    if (pname == GL_SHADER_TYPE) {
        int64_t res = static_cast<int64_t>(params);
        return NVal::CreateInt64(env, res).val_;
    } else if (pname == GL_DELETE_STATUS || pname == GL_COMPILE_STATUS) {
        bool res = (params == GL_FALSE) ? false : true;
        return NVal::CreateBool(env, res).val_;
    } else {
        LOGI("WebGL getShaderParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetAttribLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    LOGI("WebGL getAttribLocation programId %{public}u", programId);
    unique_ptr<char[]> name;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getAttribLocation name %{public}s", name.get());
    GLint returnValue = glGetAttribLocation(static_cast<GLuint>(programId), const_cast<char *>(name.get()));
    int64_t res = static_cast<int64_t>(returnValue);
    LOGI("WebGL getAttribLocation returnValue %{public}u", res);
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGLRenderingContextBase::VertexAttrib1fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib1fv index %{public}u", index);
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL vertexAttrib1fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL :vertexAttrib2fv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    glVertexAttrib1fv(static_cast<GLuint>(index), reinterpret_cast<GLfloat *>(bufferData.GetBuffer()));
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib2fv index %{public}u", index);
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL vertexAttrib2fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL :vertexAttrib2fv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    LOGI("WebGL vertexAttrib2fv index %{public}u", bufferData.GetBufferLength());
    glVertexAttrib2fv(static_cast<GLuint>(index), reinterpret_cast<GLfloat *>(bufferData.GetBuffer()));
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib3fv index %{public}u", index);
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL vertexAttrib3fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL :vertexAttrib3fv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    glVertexAttrib3fv(static_cast<GLuint>(index), reinterpret_cast<GLfloat *>(bufferData.GetBuffer()));
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4fv index %{public}u", index);
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL vertexAttrib4fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL :vertexAttrib4fv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    glVertexAttrib4fv(static_cast<GLuint>(index), reinterpret_cast<GLfloat *>(bufferData.GetBuffer()));
    return nullptr;
}

napi_value WebGLRenderingContextBase::GetVertexAttrib(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    uint32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getVertexAttrib index %{public}u", index);
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getVertexAttrib pname %{public}u", pname);
    if (pname == GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING) {  // webglBuffer
        int32_t params;
        glGetVertexAttribiv(static_cast<GLuint>(index), static_cast<GLenum>(pname), &params);
        WebGLObjectManager *maps = GetWebGLBufferMaps(env, funcArg.GetThisVar());
        if (maps == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        LOGI("WebGL get WebGLBuffer bufferId %{public}u", params);
        return maps->GetObject(params);
    } else if (pname == GL_VERTEX_ATTRIB_ARRAY_ENABLED || pname == GL_VERTEX_ATTRIB_ARRAY_NORMALIZED ||
        pname == GL_VERTEX_ATTRIB_ARRAY_INTEGER) {
        int32_t params;
        glGetVertexAttribiv(static_cast<GLuint>(index), static_cast<GLenum>(pname), &params);
        bool res = static_cast<bool>(params);
        LOGI("WebGL getVertexAttrib pname is GLBoolean end");
        return NVal::CreateBool(env, res).val_;
    } else if (pname == GL_VERTEX_ATTRIB_ARRAY_SIZE || pname == GL_VERTEX_ATTRIB_ARRAY_STRIDE ||
        pname == GL_VERTEX_ATTRIB_ARRAY_DIVISOR || pname == GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE) {
        int32_t params;
        glGetVertexAttribiv(static_cast<GLuint>(index), static_cast<GLenum>(pname), &params);
        int res = static_cast<int>(params);
        LOGI("WebGL getVertexAttrib pname is GLint end");
        return NVal::CreateInt64(env, res).val_;
    } else if (pname == GL_VERTEX_ATTRIB_ARRAY_TYPE) {  // GLenum
        int32_t params;
        glGetVertexAttribiv(static_cast<GLuint>(index), static_cast<GLenum>(pname), &params);
        int64_t res = static_cast<int>(params);
        LOGI("WebGL getVertexAttrib pname is Glenum end");
        return NVal::CreateInt64(env, res).val_;
    } else if (pname == GL_CURRENT_VERTEX_ATTRIB) {   // float32Array
        WriteBufferData writeBuffer(env);
        GLfloat *params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(4 * sizeof(GLfloat)));  // 4 args
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetVertexAttribfv(static_cast<GLuint>(index), static_cast<GLenum>(pname), params);
        return writeBuffer.ToExternalArray(buffer_data_float32);
    } else { // return result from gl
        int32_t params;
        glGetVertexAttribiv(static_cast<GLuint>(index), static_cast<GLenum>(pname), &params);
        LOGI("WebGL getVertexAttrib getVertexAttrib end");
        return nullptr;
    }
}

static napi_value GetDoubleParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    GLfloat params;
    glGetFloatv(static_cast<GLenum>(pname), &params);
    float res = static_cast<float>(params);
    return NVal::CreateDouble(env, (double)res).val_;
}

static napi_value GetTextureParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    GLint params;
    glGetIntegerv(static_cast<GLenum>(pname), &params);
    WebGLObjectManager *maps = GetWebGLTextureMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return nullptr;
    }
    LOGI("WebGL get WebGLTexture textureId %{public}u", params);
    return maps->GetObject(params);
}

static napi_value GetExtensionsParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    const unsigned char* extensions = glGetString(static_cast<GLenum>(pname));
    if (extensions == nullptr) {
        return nullptr;
    }
    LOGI("WebGL GetExtensionsParameter %{public}s", extensions);
    string str = const_cast<char*>(reinterpret_cast<const char*>(extensions));
    vector<string> vec;
    Util::SplitString(str, vec, " ");
    LOGI("WebGL GetExtensionsParameter %{public}s %{public}d", str.c_str(), vec.size());
    napi_value result = nullptr;
    napi_create_array_with_length(env, vec.size(), &result);
    for (vector<string>::size_type i = 0; i != vec.size(); ++i) {
        napi_set_element(env, result, i, NVal::CreateUTF8String(env, vec[i]).val_);
        LOGI("WebGL GetExtensionsParameter %{public}s %{public}d", vec[i].c_str(), (int)i);
    }
    return result;
}

static napi_value GetBufferParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    GLint params;
    glGetIntegerv(static_cast<GLenum>(pname), &params);
    WebGLObjectManager *maps = GetWebGLBufferMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return nullptr;
    }
    napi_value objBuffer = maps->GetObject(params);
    LOGI("WebGL get WebGLBuffer bufferId %{public}u", params);
    return objBuffer;
}

static napi_value GetFrameBufferParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    GLint params;
    glGetIntegerv(static_cast<GLenum>(pname), &params);
    WebGLObjectManager *maps = GetWebGLFrameBufferMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return nullptr;
    }
    napi_value objFramebuffer = maps->GetObject(params);
    LOGI("WebGL get WebGLFrameBuffer bufferId %{public}u", params);
    return objFramebuffer;
}

static napi_value GetProgramParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    GLint params;
    glGetIntegerv(static_cast<GLenum>(pname), &params);
    LOGI("GetProgramParameter programId %{public}u", params);
    WebGLObjectManager *maps = GetWebGLBufferMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return nullptr;
    }
    napi_value objProgram = maps->GetObject(params);
    WebGLProgram *instance = NClass::GetEntityOf<WebGLProgram>(env, objProgram);
    if (instance == nullptr) {
        return nullptr;
    }
    LOGI("WebGL get webGlProgram programId %{public}u", instance->GetProgramId());
    return objProgram;
}

static napi_value GetRenderBufferParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    GLint params;
    glGetIntegerv(static_cast<GLenum>(pname), &params);
    WebGLObjectManager *maps = GetWebGLRenderBufferMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return nullptr;
    }
    napi_value objRenderbuffer = maps->GetObject(params);
    LOGI("WebGL get WebGLRenderbuffer renderbufferId %{public}u 222222222", params);
    return objRenderbuffer;
}

static napi_value GetFloatTwoArrayParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    WriteBufferData writeBuffer(env);
    GLfloat *params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(2 * sizeof(GLfloat)));  // 2 args
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetFloatv(static_cast<GLenum>(pname), params);
    return writeBuffer.ToExternalArray(buffer_data_float32);
}

static napi_value GetFloatFourArrayParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    WriteBufferData writeBuffer(env);
    GLfloat *params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(4 * sizeof(GLfloat)));  // 4 args
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetFloatv(static_cast<GLenum>(pname), params);
    return writeBuffer.ToExternalArray(buffer_data_float32);
}

static napi_value GetIntArrayParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    GLint count = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &count); // 16383
    WriteBufferData writeBuffer(env);
    GLint *params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(count * sizeof(GLint)));
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("GetProgramParameter GL_MAX_TEXTURE_SIZE %{public}u", count);
    glGetIntegerv(static_cast<GLenum>(pname), params);
    return writeBuffer.ToExternalArray(buffer_data_int32);
}

static napi_value GetIntTwoArrayParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    WriteBufferData writeBuffer(env);
    GLint *params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(2 * sizeof(GLint)));  // 2 args
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetIntegerv(static_cast<GLenum>(pname), params);
    return writeBuffer.ToExternalArray(buffer_data_int32);
}

static napi_value GetIntFourArrayParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    WriteBufferData writeBuffer(env);
    GLint *params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(4 * sizeof(GLint)));  // 4 args
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetIntegerv(static_cast<GLenum>(pname), params);
    return writeBuffer.ToExternalArray(buffer_data_int32);
}

static napi_value GetBooleanParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    GLboolean params;
    glGetBooleanv(static_cast<GLenum>(pname), &params);
    bool res = static_cast<bool>(params);
    return NVal::CreateBool(env, res).val_;
}

static napi_value GetBooleanFourParameter(napi_env env, const NFuncArg &funcArg, int64_t pname)
{
    WriteBufferData writeBuffer(env);
    GLboolean *params = reinterpret_cast<GLboolean*>(writeBuffer.AllocBuffer(4 * sizeof(GLboolean)));  // 4 args
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetBooleanv(static_cast<GLenum>(pname), params);
    return writeBuffer.ToNormalArray(buffer_data_boolean, buffer_data_boolean);
}

static std::map<int, handleParameterGet> g_handleParameter = {};
static void InitParameterMaps()
{
    static bool initFunc = false;
    if (initFunc) {
        return;
    }
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_DEPTH_CLEAR_VALUE, GetDoubleParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_LINE_WIDTH, GetDoubleParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_POLYGON_OFFSET_FACTOR, GetDoubleParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_POLYGON_OFFSET_UNITS, GetDoubleParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_SAMPLE_COVERAGE_VALUE, GetDoubleParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_TEXTURE_BINDING_2D, GetTextureParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_TEXTURE_BINDING_CUBE_MAP, GetTextureParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_RENDERER, GetExtensionsParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_SHADING_LANGUAGE_VERSION, GetExtensionsParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_VENDOR, GetExtensionsParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_VERSION, GetExtensionsParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_ARRAY_BUFFER_BINDING, GetBufferParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_ELEMENT_ARRAY_BUFFER_BINDING, GetBufferParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_FRAMEBUFFER_BINDING, GetFrameBufferParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_CURRENT_PROGRAM, GetProgramParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_RENDERBUFFER_BINDING, GetRenderBufferParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_ALIASED_LINE_WIDTH_RANGE, GetFloatTwoArrayParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_ALIASED_POINT_SIZE_RANGE, GetFloatTwoArrayParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_DEPTH_RANGE, GetFloatTwoArrayParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_COLOR_CLEAR_VALUE, GetFloatFourArrayParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_BLEND_COLOR, GetFloatFourArrayParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_COMPRESSED_TEXTURE_FORMATS, GetIntArrayParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_MAX_VIEWPORT_DIMS, GetIntTwoArrayParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_SCISSOR_BOX, GetIntFourArrayParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_BLEND, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_CULL_FACE, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_DEPTH_TEST, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_DEPTH_WRITEMASK, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_DITHER, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_POLYGON_OFFSET_FILL, GetBooleanParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_SAMPLE_ALPHA_TO_COVERAGE, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_SAMPLE_COVERAGE, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_SAMPLE_COVERAGE_INVERT, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_SCISSOR_TEST, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(GL_STENCIL_TEST, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(
        WebGLRenderingContextBase::UNPACK_FLIP_Y_WEBGL, GetBooleanParameter));
    g_handleParameter.insert(pair<int, handleParameterGet>(
        WebGLRenderingContextBase::UNPACK_PREMULTIPLY_ALPHA_WEBGL, GetBooleanParameter));

    g_handleParameter.insert(pair<int, handleParameterGet>(GL_COLOR_WRITEMASK, GetBooleanFourParameter));
    initFunc = true;
    return;
}

napi_value WebGLRenderingContextBase::GetParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getParameter start pname %{public}u", pname);
    InitParameterMaps();
    auto handler = g_handleParameter.find(pname);
    if (handler != g_handleParameter.end()) {
        return handler->second(env, funcArg, pname);
    }

    // default process
    GLint params = -1;
    glGetIntegerv(static_cast<GLenum>(pname), &params);
    LOGI("WebGL getParameter : pname %{public}u param %{public}d", pname, params);
    if (params == -1) {
        return nullptr;
    }
    int64_t res = static_cast<int64_t>(params);
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGLRenderingContextBase::GetAttachedShaders(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL getAttachedShaders start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    LOGI("WebGL getAttachedShaders programId %{public}u", programId);
    WebGLObjectManager *maps = GetWebGLShaderMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        LOGI("WebGL getAttachedShaders failed. programId %{public}u", programId);
        return NVal::CreateNull(env).val_;
    }
    GLsizei count = 1;
    std::vector<GLuint> shaderId(MAX_COUNT_ATTACHED_SHADER);
    glGetAttachedShaders(static_cast<GLuint>(programId), MAX_COUNT_ATTACHED_SHADER, &count,
                         static_cast<GLuint*>(shaderId.data()));
    LOGI("WebGL getAttachedShaders count %{public}u", count);
    GLsizei i;
    napi_value shaderArr = nullptr;
    napi_create_array(env, &shaderArr);
    for (i = 0; i < count; i++) {
        napi_value objShader = maps->GetObject(shaderId[i]);
        programStatus = napi_set_element(env, shaderArr, i, objShader);
        LOGI("WebGL getAttachedShaders programStatus %{public}u %{public}u", programStatus, shaderId[i]);
    }
    LOGI("WebGL getAttachedShaders end");
    return shaderArr;
}

napi_value WebGLRenderingContextBase::GetShaderPrecisionFormat(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getShaderPrecisionFormat start");
    int64_t shaderType;
    tie(succ, shaderType) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getShaderPrecisionFormat shaderType %{public}u", shaderType);
    int64_t precisionType;
    tie(succ, precisionType) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL getShaderPrecisionFormat precisionType %{public}u", precisionType);
    WebGLShaderPrecisionFormat *webGLShaderPrecisionFormat = nullptr;
    napi_value objShaderPrecisionFormat = WebGLShaderPrecisionFormat::CreateObjectInstance(env, &webGLShaderPrecisionFormat).val_;
    if (objShaderPrecisionFormat == nullptr) {
        LOGI("WebGL getShaderPrecisionFormat fail oshaderType %{public}u", shaderType);
        return NVal::CreateNull(env).val_;
    }

    GLint range[2] { };
    GLint precision = 0;
    glGetShaderPrecisionFormat(static_cast<GLenum>(shaderType), static_cast<GLenum>(precisionType), range, &precision);
    LOGI("WebGL getShaderPrecisionFormat [%{public}u %{public}u] precision %{public}u", range[0], range[1], precision);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatRangeMin(range[0]);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatRangeMax(range[1]);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatPrecision(precision);
    return objShaderPrecisionFormat;
}

napi_value WebGLRenderingContextBase::GetShaderInfoLog(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL getShaderInfoLog start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlShader);
    if (shaderStatus != napi_ok) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    GLint length = 0;
    glGetShaderiv(static_cast<GLuint>(shaderId), GL_INFO_LOG_LENGTH, &length);
    LOGI("WebGL getShaderInfoLog shaderId %{public}u length %{public}u", shaderId, length);
    if (length == 0) {
        return NVal::CreateNull(env).val_;
    }
    GLsizei size = 0;
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length);
    if (buf == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    glGetShaderInfoLog(shaderId, length, &size, buf.get());
    LOGI("WebGL getShaderInfoLog length %{public}d %{public}s ",
        static_cast<int>(length), buf.get(),  static_cast<int>(size));
    string str = buf.get();
    LOGI("WebGL getShaderInfoLog length %{public}d %{public}s %{public}d ",
        static_cast<int>(length), str.c_str(), static_cast<int>(str.size()));
    return NVal::CreateUTF8String(env, str).val_;
}

napi_value WebGLRenderingContextBase::GetProgramInfoLog(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL getProgramInfoLog start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    LOGI("WebGL getProgramInfoLog programId %{public}u", programId);
    GLint length = 0;
    GLsizei size = 0;
    glGetProgramiv(static_cast<GLuint>(programId), GL_INFO_LOG_LENGTH, &length);
    if (length == 0) {
        return NVal::CreateNull(env).val_;
    }
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length);
    LOGI("WebGL getProgramInfoLog bufSize %{public}u", length);
    if (buf == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetProgramInfoLog(programId, length, &size, buf.get());
    string str = buf.get();
    LOGI("WebGL getProgramInfoLog %{public}u ", str.size());
    return NVal::CreateUTF8String(env, str).val_;
}

napi_value WebGLRenderingContextBase::GetShaderSource(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGI("WebGL getShaderSource shaderId %{public}u", shaderId);
    return NVal::CreateUTF8String(env, webGlShader->GetShaderRes()).val_;
}

static napi_value GetUniformInActive(napi_env env, GLuint programId, GLint locationId, GLenum type)
{
    static const int typeMaps[][4] = {
        {GL_FLOAT_VEC2, 2, buffer_data_float32, buffer_data_float32},
        {GL_FLOAT_VEC3, 3, buffer_data_float32, buffer_data_float32},
        {GL_FLOAT_VEC4, 4, buffer_data_float32, buffer_data_float32},
        {GL_FLOAT_MAT2, 4, buffer_data_float32, buffer_data_float32},
        {GL_FLOAT_MAT3, 9, buffer_data_float32, buffer_data_float32},
        {GL_FLOAT_MAT4, 16, buffer_data_float32, buffer_data_float32},

        {GL_INT_VEC2, 2, buffer_data_int32, buffer_data_int32},
        {GL_INT_VEC3, 3, buffer_data_int32, buffer_data_int32},
        {GL_INT_VEC4, 4, buffer_data_int32, buffer_data_int32},

        {GL_BOOL_VEC2, 2, buffer_data_int32, buffer_data_boolean},
        {GL_BOOL_VEC4, 4, buffer_data_int32, buffer_data_boolean},
    };

    if (type == GL_INT || type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE) {
        GLint params;
        glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), &params);
        int64_t res = static_cast<int64_t>(params);
        return NVal::CreateInt64(env, res).val_;
    } else if (type == GL_FLOAT) {
        GLfloat params;
        glGetUniformfv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), &params);
        float res = static_cast<float>(params);
        return NVal::CreateDouble(env, (double) res).val_;
    } else if (type == GL_BOOL) {
        GLint params;
        glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), &params);
        bool res = (params == GL_FALSE) ? false : true;
        return NVal::CreateBool(env, res).val_;
    }

    size_t index = 0;
    for (; index < sizeof(typeMaps) / sizeof(typeMaps[0]); index++) {
        if (type == typeMaps[index][0]) {
            break;
        }
    }
    if (index >= sizeof(typeMaps) / sizeof(typeMaps[0])) {
        LOGI("WebGL getUniform end");
        return nullptr;
    }

    WriteBufferData writeBuffer(env);
    if (typeMaps[index][3] == buffer_data_float32) {
        GLfloat *params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(typeMaps[index][1] * sizeof(GLfloat)));
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetUniformfv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), params);
        writeBuffer.DumpBuffer(buffer_data_float32);
        return writeBuffer.ToExternalArray(buffer_data_float32);
    } else if (typeMaps[index][3] == buffer_data_int32) {
        GLint *params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(typeMaps[index][1] * sizeof(GLint)));
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), params);
        writeBuffer.DumpBuffer(buffer_data_int32);
        return writeBuffer.ToExternalArray(buffer_data_int32);
    } else if (typeMaps[index][3] == buffer_data_boolean) {
        GLint *params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(typeMaps[index][1] * sizeof(GLint)));
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), params);
        writeBuffer.DumpBuffer(buffer_data_int32);
        return writeBuffer.ToNormalArray(buffer_data_int32, buffer_data_boolean);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBase::GetUniform(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr || funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    LOGI("WebGL getUniform programId %{public}d", programId);
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL getUniform locationId %{public}u", locationId);
    GLint maxNameLength = -1;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
    LOGI("WebGL getUniform maxNameLength %{public}d", maxNameLength);
    if (maxNameLength <= 0) {
        return nullptr;
    }
    GLint activeUniforms = 0;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &activeUniforms);
    LOGI("WebGL getUniform activeUniforms %{public}d", activeUniforms);

    for (GLint i = 0; i < activeUniforms; i++) {
        char name_ptr[maxNameLength];
        GLsizei name_length = 0;
        GLint size = -1;
        GLenum type = 0;
        glGetActiveUniform(programId, i, maxNameLength, &name_length, &size, &type,
                           reinterpret_cast<GLchar *>(name_ptr));
        LOGI("WebGL getUniform type 0x%{public}x", type);
        return GetUniformInActive(env, static_cast<GLuint>(programId), static_cast<GLint>(locationId), type);
    }
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
