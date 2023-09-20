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
#include "../include/context/webgl_rendering_context_base_impl.h"
#include "../include/context/webgl2_rendering_context_base.h"

#include "../include/context/webgl_context_attributes.h"
#include "../include/context/webgl_rendering_context.h"
#include "../../common/napi/n_func_arg.h"
#include "../../common/napi/n_class.h"
#include "../include/webgl/webgl_arg.h"
#include "../include/webgl/webgl_shader.h"
#include "../include/webgl/webgl_buffer.h"
#include "../include/webgl/webgl_framebuffer.h"
#include "../include/webgl/webgl_program.h"
#include "../include/webgl/webgl_renderbuffer.h"
#include "../include/webgl/webgl_texture.h"
#include "../include/webgl/webgl_uniform_location.h"
#include "../include/webgl/webgl_active_info.h"
#include "../include/webgl/webgl_shader_precision_format.h"
#include "../include/util/log.h"
#include "../include/util/egl_manager.h"
#include "../include/util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;
static bool GetVertexAttribIndex(napi_env env, napi_value data, GLuint &index)
{
    bool succ;
    tie(succ, index) = NVal(env, data).ToUint32();
    if (!succ || index >= Impl::WebGLRenderingContextBaseImpl::GetMaxVertexAttribs()) {
        return false;
    }
    return true;
}

static bool IsHighWebGL(napi_env env, napi_value thisVar)
{
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, thisVar);
    if (context == nullptr) {
        return false;
    }
    return context->GetWebGLRenderingContextImpl().IsHighWebGL();
}

static bool CheckGLenum(napi_env env, napi_value thisVar, GLenum type,
    const std::vector<GLenum> &glSupport, const std::vector<GLenum> &gl2Support)
{
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, thisVar);
    if (context == nullptr) {
        return false;
    }
    return context->GetWebGLRenderingContextImpl().CheckGLenum(env, type, glSupport, gl2Support);
}

napi_value WebGLRenderingContextBase::GetContextAttributes(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLContextAttributes *webGlContextAttributes = context->webGlContextAttributes;
    if (webGlContextAttributes == nullptr) {
        webGlContextAttributes = new WebGLContextAttributes();
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
    if (EglManager::GetInstance().GetEGLContext() == nullptr) {
        return NVal::CreateNull(env).val_;
    };
    const char* extensions = eglQueryString(EglManager::GetInstance().GetEGLDisplay(), EGL_EXTENSIONS);
    string str = extensions;
    vector<string> vec;
    Util::SplitString(str, vec, " ");
    napi_value result = nullptr;
    napi_create_array_with_length(env, vec.size(), &result);
    for (vector<string>::size_type i = 0; i != vec.size(); ++i) {
        LOGI("WebGL GetSupportedExtensions vec %{public}s", vec[i].c_str());
        napi_set_element(env, result, i, NVal::CreateUTF8String(env, vec[i]).val_);
    }
    return result;
}

napi_value WebGLRenderingContextBase::GetExtension(napi_env env, napi_callback_info info)
{
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::ActiveTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    GLenum texture;
    tie(succ, texture) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().ActiveTexture(env, texture);
}

napi_value WebGLRenderingContextBase::AttachShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        return nullptr;
    }
    uint32_t programId = webGLProgram->GetProgramId();

    WebGLShader *webGlShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::SECOND]);
    if (webGlShader == nullptr) {
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
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    bool succ = false;
    int64_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    if (index < 0) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    unique_ptr<char[]> name;
    uint32_t nameLen = 0;
    tie(succ, name, nameLen) = NVal(env, funcArg[NARG_POS::THIRD]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().BindAttribLocation(env,
        funcArg[NARG_POS::FIRST], static_cast<GLuint>(index), name.get());
}

napi_value WebGLRenderingContextBase::BindBuffer(napi_env env, napi_callback_info info)
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
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context != nullptr) {
        context->GetWebGLRenderingContextImpl().BindBuffer(env, target, funcArg[NARG_POS::SECOND]);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context != nullptr) {
        context->GetWebGLRenderingContextImpl().BindFrameBuffer(env, target, funcArg[NARG_POS::SECOND]);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context != nullptr) {
        context->GetWebGLRenderingContextImpl().BindRenderBuffer(env, target, funcArg[NARG_POS::SECOND]);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().BindTexture(env, target, funcArg[NARG_POS::SECOND]);
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
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendEquation mode %{public}u", mode);
    glBlendEquation(static_cast<GLenum>(mode));
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendEquationSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum modeRGB;
    tie(succ, modeRGB) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    LOGI("WebGL blendEquationSeparate modeRGB %{public}u", modeRGB);
    if (!CheckGLenum(env, funcArg.GetThisVar(), modeRGB, {
        WebGLRenderingContextBase::FUNC_ADD,
        WebGLRenderingContextBase::FUNC_SUBTRACT,
        WebGLRenderingContextBase::FUNC_REVERSE_SUBTRACT
    }, {
        WebGL2RenderingContextBase::MIN,
        WebGL2RenderingContextBase::MAX
    })) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    GLenum modeAlpha;
    tie(succ, modeAlpha) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    /*
     * When using the EXT_blend_minmax extension:
     *   ext.MIN_EXT: Minimum of source and destination,
     *   ext.MAX_EXT: Maximum of source and destination.
    */
    if (!CheckGLenum(env, funcArg.GetThisVar(), modeAlpha, {
        WebGLRenderingContextBase::FUNC_ADD,
        WebGLRenderingContextBase::FUNC_SUBTRACT,
        WebGLRenderingContextBase::FUNC_REVERSE_SUBTRACT
    }, {
        WebGL2RenderingContextBase::MIN,
        WebGL2RenderingContextBase::MAX
    })) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    LOGI("WebGL blendEquationSeparate modeAlpha %{public}u", modeAlpha);
    glBlendEquationSeparate(modeRGB, modeAlpha);
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendFunc(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum sFactor;
    tie(succ, sFactor) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFunc sFactor %{public}u", sFactor);
    GLenum dFactor;
    tie(succ, dFactor) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFunc dFactor %{public}u", dFactor);
    glBlendFunc(sFactor, dFactor);
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendFuncSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    GLenum srcRGB;
    tie(succ, srcRGB) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFuncSeparate srcRGB %{public}u", srcRGB);
    GLenum dstRGB;
    tie(succ, dstRGB) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFuncSeparate dstRGB %{public}u", dstRGB);
    GLenum srcAlpha;
    tie(succ, srcAlpha) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFuncSeparate srcAlpha %{public}u", srcAlpha);
    GLenum dstAlpha;
    tie(succ, dstAlpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL blendFuncSeparate dstAlpha %{public}u", dstAlpha);
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    return nullptr;
}

napi_value WebGLRenderingContextBase::CheckFramebufferStatus(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL checkFramebufferStatus target %{public}u", target);
    if (!context->GetWebGLRenderingContextImpl().CheckFrameBufferTarget(env, target)) {
        return nullptr;
    }
    GLenum res = glCheckFramebufferStatus(static_cast<GLenum>(target));
    LOGI("WebGL checkFramebufferStatus target %{public}u res %{public}u", target, res);
    return NVal::CreateInt64(env, static_cast<int64_t>(res)).val_;
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
        mask = 0;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Clear(env, static_cast<GLbitfield>(mask));
}

napi_value WebGLRenderingContextBase::ClearColor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    float red;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToFloat();
    if (!succ || std::isnan(red)) {
        red = 0;
    }
    float green;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToFloat();
    if (!succ || std::isnan(green)) {
        green = 0;
    }
    float blue;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToFloat();
    if (!succ || std::isnan(blue)) {
        blue = 0;
    }
    float alpha;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToFloat();
    if (!succ || std::isnan(alpha)) {
        alpha = 0;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().ClearColor(env,
        static_cast<GLclampf>(red), static_cast<GLclampf>(green),
        static_cast<GLclampf>(blue), static_cast<GLclampf>(alpha));
}

napi_value WebGLRenderingContextBase::ClearDepth(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    double depth;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL clearDepth %{public}f", depth);
    glClearDepthf(static_cast<GLclampf>(depth));
    return nullptr;
}

napi_value WebGLRenderingContextBase::ClearStencil(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    int s;
    tie(succ, s) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL clearStencil s %{public}u", s);
    glClearStencil(static_cast<GLint>(s));
    return nullptr;
}

napi_value WebGLRenderingContextBase::ColorMask(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
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

    WebGLShader *webGlShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGlShader == nullptr) {
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

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    Impl::Image2DArg imgArg = { 0 };
    imgArg.func = Impl::IMAGE_2D_FUNC_COPY_TEX_IMAGE_2D;
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.x) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.y) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToGLsizei();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.border) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().CopyTexImage2D(env, &imgArg);
}

napi_value WebGLRenderingContextBase::CopyTexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::EIGHT)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    Impl::Image2DArg imgArg = { 0 };
    imgArg.func = Impl::IMAGE_2D_FUNC_COPY_TEX_SUB_IMAGE_2D;
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.xoffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.yoffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.x) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.x) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToGLsizei();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLsizei();
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().CopyTexSubImage2D(env, &imgArg);
}

napi_value WebGLRenderingContextBase::CreateBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().CreateBuffer(env);
}

napi_value WebGLRenderingContextBase::CreateFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().CreateFrameBuffer(env);
}

napi_value WebGLRenderingContextBase::CreateProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().CreateProgram(env);
}

napi_value WebGLRenderingContextBase::CreateRenderbuffer(napi_env env, napi_callback_info info)
{
    LOGI("WebGL createRenderbuffer start");
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().CreateRenderBuffer(env);
}

napi_value WebGLRenderingContextBase::CreateShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().CreateShader(env, type);
}

napi_value WebGLRenderingContextBase::CreateTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().CreateTexture(env);
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

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().DeleteBuffer(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::DeleteFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().DeleteFrameBuffer(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::DeleteProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().DeleteProgram(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::DeleteRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().DeleteRenderBuffer(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::DeleteShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().DeleteShader(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::DeleteTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().DeleteTexture(env, funcArg[NARG_POS::FIRST]);
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

    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        return nullptr;
    }
    WebGLShader *webGlShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::SECOND]);
    if (webGlShader == nullptr) {
        return nullptr;
    }
    uint32_t programId = webGLProgram->GetProgramId();
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
        cap = 0;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Disable(env, cap);
}

napi_value WebGLRenderingContextBase::DrawArrays(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    bool succ = false;
    GLenum mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
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
    return context->GetWebGLRenderingContextImpl().DrawArrays(env, mode, static_cast<GLint>(first),
        static_cast<GLsizei>(count));
}

napi_value WebGLRenderingContextBase::DrawElements(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    GLenum mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    int64_t count;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    LOGI("WebGL drawElements type %{public}u", type);
    int64_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL drawElements offset %{public}u", offset);
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().DrawElements(env,
        mode, static_cast<GLsizei>(count), type, static_cast<GLintptr>(offset));
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
        cap = 0;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Enable(env, cap);
}

napi_value WebGLRenderingContextBase::EnableVertexAttribArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    int64_t index;
    bool succ;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().EnableVertexAttribArray(env, index);
}

napi_value WebGLRenderingContextBase::DisableVertexAttribArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    int64_t index;
    bool succ;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().DisableVertexAttribArray(env, index);
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
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("WebGL flush context == nullptr");
        return nullptr;
    }
    glFlush();
    context->Update();
    LOGI("WebGL flush end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::FramebufferRenderbuffer(napi_env env, napi_callback_info info)
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
    GLenum attachment;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL framebufferRenderbuffer attachment %{public}u", attachment);

    GLenum renderBufferTarget;
    tie(succ, renderBufferTarget) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("WebGL flush context == nullptr");
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().FrameBufferRenderBuffer(env,
        target, attachment, renderBufferTarget, funcArg[NARG_POS::FOURTH]);
}

napi_value WebGLRenderingContextBase::FramebufferTexture2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }

    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    GLenum attachment;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    GLenum textarget;
    tie(succ, textarget) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("WebGL flush context == nullptr");
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().FrameBufferTexture2D(env,
        target, attachment, textarget, funcArg[NARG_POS::FOURTH], static_cast<GLint>(level));
}

napi_value WebGLRenderingContextBase::GetUniformLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGI("WebGL getUniformLocation fail to get context");
        return nullptr;
    }
    bool succ;
    unique_ptr<char[]> name;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().GetUniformLocation(env, funcArg[NARG_POS::FIRST], name.get());
}

napi_value WebGLRenderingContextBase::GetVertexAttribOffset(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGI("WebGL getUniformLocation fail to get context");
        return nullptr;
    }
    bool succ = false;
    uint32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().GetVertexAttribOffset(env, index, pname);
}

napi_value WebGLRenderingContextBase::ShaderSource(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    bool succ = false;
    WebGLShader *webGlShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGlShader == nullptr) {
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
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    LOGI("WebGL hint target %{public}u", target);
    switch (target) {
        case WebGLRenderingContextBase::GENERATE_MIPMAP_HINT:
            break;
        case WebGL2RenderingContextBase::FRAGMENT_SHADER_DERIVATIVE_HINT:
            if (IsHighWebGL(env, funcArg.GetThisVar())) {
                break;
            }
            [[fallthrough]];
        default:
            SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    GLenum mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    LOGI("WebGL hint mode %{public}u", mode);
    switch (mode) {
        case WebGLRenderingContextBase::DONT_CARE:
        case WebGLRenderingContextBase::FASTEST:
        case WebGLRenderingContextBase::NICEST:
            break;
        default:
            SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    glHint(target, mode);
    return nullptr;
}

napi_value WebGLRenderingContextBase::IsBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }

    WebGLBuffer *webGlBuffer = WebGLBuffer::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGlBuffer == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int buffer = webGlBuffer->GetBufferId();
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
        cap = 0;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    return context->GetWebGLRenderingContextImpl().IsEnabled(env, cap);
}

napi_value WebGLRenderingContextBase::RenderbufferStorage(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    GLenum internalFormat;
    tie(succ, internalFormat) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().RenderBufferStorage(env,
        target, internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
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
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    LOGI("WebGL stencilFuncSeparate face %{public}u", face);
    GLenum func;
    tie(succ, func) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
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
    if (!CheckGLenum(env, funcArg.GetThisVar(), face, {
        WebGLRenderingContextBase::FRONT_AND_BACK,
        WebGLRenderingContextBase::FRONT,
        WebGLRenderingContextBase::BACK
    }, {})) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckGLenum(env, funcArg.GetThisVar(), func, {
        WebGLRenderingContextBase::NEVER,
        WebGLRenderingContextBase::LESS,
        WebGLRenderingContextBase::EQUAL,
        WebGLRenderingContextBase::LEQUAL,
        WebGLRenderingContextBase::GREATER,
        WebGLRenderingContextBase::NOTEQUAL,
        WebGLRenderingContextBase::GEQUAL,
        WebGLRenderingContextBase::ALWAYS
    }, {})) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    LOGI("WebGL stencilFuncSeparate mask %{public}u", mask);
    glStencilFuncSeparate(face, func, static_cast<GLint>(ref), static_cast<GLuint>(mask));
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
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    LOGI("WebGL stencilMaskSeparate face %{public}u", face);
    uint32_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
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
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL stencilOp start");
    GLenum fail;
    tie(succ, fail) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        fail = WebGLRenderingContextBase::KEEP;
    }
    LOGI("WebGL stencilOp fail %{public}u", fail);
    GLenum zfail;
    tie(succ, zfail) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        zfail = WebGLRenderingContextBase::KEEP;
    }
    LOGI("WebGL stencilOp zfail %{public}u", zfail);
    GLenum zpass;
    tie(succ, zpass) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        zpass = WebGLRenderingContextBase::KEEP;
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
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (!CheckGLenum(env, funcArg.GetThisVar(), face, {
        WebGLRenderingContextBase::FRONT,
        WebGLRenderingContextBase::BACK,
        WebGLRenderingContextBase::FRONT_AND_BACK
    }, {})) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    LOGI("WebGL stencilOpSeparate face %{public}u", face);
    GLenum sfail;
    tie(succ, sfail) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        sfail = WebGLRenderingContextBase::KEEP;
    }
    LOGI("WebGL stencilOpSeparate sfail %{public}u", sfail);
    GLenum dpfail;
    tie(succ, dpfail) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        dpfail = WebGLRenderingContextBase::KEEP;
    }
    LOGI("WebGL stencilOpSeparate dpfail %{public}u", dpfail);
    GLenum dppass;
    tie(succ, dppass) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLenum();
    if (!succ) {
        dppass = WebGLRenderingContextBase::KEEP;
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

    bool succ;
    GLfloat data[4] = {}; // max len 4
    tie(succ, data[0]) = NVal(env, funcArg[NARG_POS::SECOND]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().UniformXf(env, funcArg[NARG_POS::FIRST], 1, data);
}

napi_value WebGLRenderingContextBase::Uniform2f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }

    bool succ;
    const uint32_t count = 2;
    GLfloat data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToFloat();
        if (!succ) {
            LOGI("WebGL uniform2f invalid arg");
            return nullptr;
        }
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().UniformXf(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform3f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }

    bool succ;
    const uint32_t count = 3;
    GLfloat data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToFloat();
        if (!succ) {
            LOGI("WebGL uniform3f invalid arg");
            return nullptr;
        }
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().UniformXf(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform4f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }

    bool succ;
    const uint32_t count = 4;
    GLfloat data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToFloat();
        if (!succ) {
            LOGI("WebGL uniform4f invalid arg");
            return nullptr;
        }
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().UniformXf(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform1i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    bool succ;
    const uint32_t count = 1;
    GLint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return nullptr;
        }
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().UniformXi(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform2i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }

    bool succ;
    const uint32_t count = 2;
    GLint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return nullptr;
        }
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().UniformXi(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform3i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }

    bool succ;
    const uint32_t count = 3;
    GLint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return nullptr;
        }
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().UniformXi(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform4i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }

    bool succ;
    const uint32_t count = 4;
    GLint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return nullptr;
        }
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().UniformXi(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::UseProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().UseProgram(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::ValidateProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        return nullptr;
    }
    uint32_t program = webGLProgram->GetProgramId();
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
    GLuint index = 0;
    bool succ = GetVertexAttribIndex(env, funcArg[NARG_POS::FIRST], index);
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGI("WebGL vertexAttrib1f index %{public}u", index);
    GLfloat v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib1f v0 %{public}f", v0);
    glVertexAttrib1f(index, v0);
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib2f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    GLuint index = 0;
    bool succ = GetVertexAttribIndex(env, funcArg[NARG_POS::FIRST], index);
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGI("WebGL vertexAttrib2f index %{public}u", index);
    GLfloat x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib2f x %{public}f", x);
    GLfloat y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib2f y %{public}f", y);
    glVertexAttrib2f(index, x, y);
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib3f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    GLuint index = 0;
    bool succ = GetVertexAttribIndex(env, funcArg[NARG_POS::FIRST], index);
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    LOGI("WebGL vertexAttrib3f index %{public}u", index);
    GLfloat x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib3f x %{public}lf", x);
    GLfloat y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib3f y %{public}lf", y);
    GLfloat z;
    tie(succ, z) = NVal(env, funcArg[NARG_POS::FOURTH]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib3f z %{public}lf", z);
    glVertexAttrib3f(index, x, y, z);
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib4f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    GLuint index = 0;
    bool succ = GetVertexAttribIndex(env, funcArg[NARG_POS::FIRST], index);
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4f index %{public}u", index);
    GLfloat x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4f x %{public}f", x);
    GLfloat y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4f y %{public}f", y);
    GLfloat z;
    tie(succ, z) = NVal(env, funcArg[NARG_POS::FOURTH]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4f z %{public}f", z);
    GLfloat w;
    tie(succ, w) = NVal(env, funcArg[NARG_POS::FIFTH]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttrib4f w %{public}f", w);
    glVertexAttrib4f(index, x, y, z, w);
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttribPointer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SIX)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    bool succ;
    Impl::VertexAttribArg vertexInfo = {0};
    tie(succ, vertexInfo.index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, vertexInfo.size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL vertexAttribPointer size %{public}u", vertexInfo.size);
    tie(succ, vertexInfo.type) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    tie(succ, vertexInfo.normalized) = NVal(env, funcArg[NARG_POS::FOURTH]).ToBool();
    if (!succ) {
        return nullptr;
    }
    int64_t data;
    tie(succ, data) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    vertexInfo.stride = static_cast<GLsizei>(data);
    tie(succ, data) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    vertexInfo.offset = static_cast<GLintptr>(data);
    return context->GetWebGLRenderingContextImpl().VertexAttribPointer(env, &vertexInfo);
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
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }

    WebGLFramebuffer *webGlFramebuffer = WebGLFramebuffer::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGlFramebuffer == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    GLuint frameBufferId = webGlFramebuffer->GetFramebuffer();
    res = static_cast<bool>(glIsFramebuffer(frameBufferId));
    LOGI("WebGL isFramebuffer framebuffer %{public}u res %{public}u", frameBufferId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }

    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
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
    LOGI("WebGL isRenderbuffer start");
    WebGLRenderbuffer *webGLRenderbuffer = WebGLRenderbuffer::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLRenderbuffer == nullptr) {
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

    WebGLShader *webGLShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLShader == nullptr) {
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
    WebGLTexture *webGLTexture = WebGLTexture::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLTexture == nullptr) {
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
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bool succ = false;
    double linewidth;
    tie(succ, linewidth) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
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

    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
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

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGI("WebGL pixelStorei fail to get context");
        return nullptr;
    }

    bool succ = false;
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLint param = 0;
    bool isbool = NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_boolean);
    if (isbool) {
        bool parambool = false;
        tie(succ, parambool) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
        if (!succ) {
            context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
            return nullptr;
        }
        param = parambool ? 1 : 0;
    } else {
        tie(succ, param) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
            return nullptr;
        }
    }
    context->GetWebGLRenderingContextImpl().PixelStorei(env, pname, param);
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

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGI("WebGL pixelStorei fail to get context");
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().GenerateMipmap(env, target);
}

static napi_value CreateWebGLActiveInfo(napi_env env, napi_callback_info info,
    void (*getActiveInfo)(uint32_t programId, int64_t index, GLint &size, GLenum &type, GLchar *name))
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        LOGE("WebGL getActiveAttrib invalid param");
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t programId = webGLProgram->GetProgramId();
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
    return objActiveInfo;
}

napi_value WebGLRenderingContextBase::GetActiveAttrib(napi_env env, napi_callback_info info)
{
    return CreateWebGLActiveInfo(env, info,
        [](uint32_t programId, int64_t index, GLint &size, GLenum &type, GLchar *name) {
        GLsizei bufSize = WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH;
        GLsizei length;
        glGetActiveAttrib(static_cast<GLuint>(programId),
            static_cast<GLuint>(index), bufSize, &length, &size, &type, name);
        if (length > WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH) {
            LOGE("WebGL: getActiveAttrib: [error] bufSize exceed!");
        }
        LOGI("WebGL getActiveAttrib %{public}s %{public}u %{public}d %{public}u", name, type, size, length);
    });
}

napi_value WebGLRenderingContextBase::GetActiveUniform(napi_env env, napi_callback_info info)
{
    return CreateWebGLActiveInfo(env, info, [](uint32_t programId, int64_t index,
        GLint &size, GLenum &type, GLchar*name) {
        GLsizei bufSize = WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH;
        GLsizei length;
        glGetActiveUniform(static_cast<GLuint>(programId),
            static_cast<GLuint>(index), bufSize, &length, &size, &type, name);
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
    } else {
        LOGI("WebGL getBufferParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetError(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    funcArg.InitArgs(NARG_CNT::ZERO);
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateInt64(env, WebGLRenderingContextBase::INVALID_OPERATION).val_;
    }
    return context->GetWebGLRenderingContextImpl().GetError(env);
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
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().GetFrameBufferAttachmentParameter(env, target, attachment, pname);
}

napi_value WebGLRenderingContextBase::GetProgramParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    bool succ = false;
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().GetProgramParameter(env, funcArg[NARG_POS::FIRST], pname);
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
    WebGLShader *webGlShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGlShader == nullptr) {
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
        return NVal::CreateInt64(env, -1).val_;
    }

    bool succ = false;
    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        return NVal::CreateInt64(env, -1).val_;
    }
    uint32_t programId = webGLProgram->GetProgramId();
    LOGI("WebGL getAttribLocation programId %{public}u", programId);
    unique_ptr<char[]> name;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return NVal::CreateInt64(env, -1).val_; // -1 is error
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
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(funcArg[NARG_POS::SECOND], BUFFER_DATA_FLOAT_32);
    if (status != 0) {
        LOGE("WebGL vertexAttrib1fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
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
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(funcArg[NARG_POS::SECOND], BUFFER_DATA_FLOAT_32);
    if (status != 0) {
        LOGE("WebGL vertexAttrib2fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
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
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(funcArg[NARG_POS::SECOND], BUFFER_DATA_FLOAT_32);
    if (status != 0) {
        LOGE("WebGL vertexAttrib3fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
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
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(funcArg[NARG_POS::SECOND], BUFFER_DATA_FLOAT_32);
    if (status != 0) {
        LOGE("WebGL vertexAttrib4fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
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
    GLuint index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().GetVertexAttrib(env, pname, index);
}

napi_value WebGLRenderingContextBase::GetParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().GetParameter(env, pname);
}

napi_value WebGLRenderingContextBase::GetAttachedShaders(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().GetAttachedShaders(env, funcArg[NARG_POS::FIRST]);
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
    napi_value objShaderPrecisionFormat = WebGLShaderPrecisionFormat::CreateObjectInstance(env,
        &webGLShaderPrecisionFormat).val_;
    if (objShaderPrecisionFormat == nullptr) {
        LOGI("WebGL getShaderPrecisionFormat fail oshaderType %{public}u", shaderType);
        return NVal::CreateNull(env).val_;
    }

    GLint range[2] { };
    GLint precision = 0;
    glGetShaderPrecisionFormat(static_cast<GLenum>(shaderType), static_cast<GLenum>(precisionType),
        range, &precision);
    LOGI("WebGL getShaderPrecisionFormat [%{public}u %{public}u] precision %{public}u",
        range[0], range[1], precision);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatRangeMin(range[0]);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatRangeMax(range[1]);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatPrecision(precision);
    return objShaderPrecisionFormat;
}

napi_value WebGLRenderingContextBase::GetShaderInfoLog(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    LOGI("WebGL getShaderInfoLog start");
    WebGLShader *webGlShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGlShader == nullptr) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    GLint length = 0;
    glGetShaderiv(static_cast<GLuint>(shaderId), GL_INFO_LOG_LENGTH, &length);
    LOGI("WebGL getShaderInfoLog shaderId %{public}u length %{public}u", shaderId, length);
    if (length == 0) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    GLsizei size = 0;
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length);
    if (buf == nullptr) {
        return NVal::CreateUTF8String(env, "").val_;
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
        return NVal::CreateUTF8String(env, "").val_;
    }
    LOGI("WebGL getProgramInfoLog start");
    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        return NVal::CreateUTF8String(env, "").val_;
    }

    uint32_t programId = webGLProgram->GetProgramId();
    LOGI("WebGL getProgramInfoLog programId %{public}u", programId);
    GLint length = 0;
    GLsizei size = 0;
    glGetProgramiv(static_cast<GLuint>(programId), GL_INFO_LOG_LENGTH, &length);
    LOGI("WebGL getProgramInfoLog bufSize %{public}u", length);
    if (length == 0) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length);
    if (buf == nullptr) {
        return NVal::CreateUTF8String(env, "").val_;
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

    WebGLShader *webGlShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGlShader == nullptr) {
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGI("WebGL getShaderSource shaderId %{public}u", shaderId);
    return NVal::CreateUTF8String(env, webGlShader->GetShaderRes()).val_;
}

static napi_value GetUniformInActive(napi_env env, GLuint programId, GLint locationId, GLenum type)
{
    static const int TYPE_MAPS[][4] = {
        {GL_FLOAT_VEC2, 2, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32}, // 2 is GL_FLOAT_VEC2 size
        {GL_FLOAT_VEC3, 3, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32}, // 3 is GL_FLOAT_VEC3 size
        {GL_FLOAT_VEC4, 4, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32}, // 4 is GL_FLOAT_VEC4 size
        {GL_FLOAT_MAT2, 4, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32}, // 4 is GL_FLOAT_MAT2 size
        {GL_FLOAT_MAT3, 9, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32}, // 9 is GL_FLOAT_MAT3 size
        {GL_FLOAT_MAT4, 16, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32}, // 16 is GL_FLOAT_MAT4 size

        {GL_INT_VEC2, 2, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32}, // 2 is GL_INT_VEC2 size
        {GL_INT_VEC3, 3, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32}, // 3 is GL_INT_VEC3 size
        {GL_INT_VEC4, 4, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32}, // 4 is GL_INT_VEC4 size

        {GL_BOOL_VEC2, 2, BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN}, // 2 is GL_BOOL_VEC2 size
        {GL_BOOL_VEC4, 4, BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN}, // 4 is GL_BOOL_VEC4 size
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
    for (; index < sizeof(TYPE_MAPS) / sizeof(TYPE_MAPS[0]); index++) {
        if (type == static_cast<GLenum>(TYPE_MAPS[index][0])) {
            break;
        }
    }
    if (index >= sizeof(TYPE_MAPS) / sizeof(TYPE_MAPS[0])) {
        LOGI("WebGL getUniform end");
        return nullptr;
    }

    WebGLWriteBufferArg writeBuffer(env);
    if (TYPE_MAPS[index][3] == BUFFER_DATA_FLOAT_32) { // 3 is type
        GLfloat *params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(TYPE_MAPS[index][1] * sizeof(GLfloat)));
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetUniformfv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), params);
        writeBuffer.DumpBuffer(BUFFER_DATA_FLOAT_32);
        return writeBuffer.ToNormalArray(BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32);
    } else if (TYPE_MAPS[index][3] == BUFFER_DATA_INT_32) { // 3 is type
        // 1 is type
        GLint *params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(TYPE_MAPS[index][1] * sizeof(GLint)));
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), params);
        writeBuffer.DumpBuffer(BUFFER_DATA_INT_32);
        return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_INT_32);
    } else if (TYPE_MAPS[index][3] == BUFFER_DATA_BOOLEAN) { // 3 is type
        GLint *params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(TYPE_MAPS[index][1] * sizeof(GLint)));
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), params);
        writeBuffer.DumpBuffer(BUFFER_DATA_INT_32);
        return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBase::GetUniform(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLProgram *webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLuint programId = webGLProgram->GetProgramId();
    LOGI("WebGL getUniform programId %{public}d", programId);

    WebGLUniformLocation *webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env,
        funcArg[NARG_POS::SECOND]);
    if (webGLUniformLocation == nullptr) {
        SET_CONTENT_ERROR(env, funcArg.GetThisVar(), WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLint locationId = webGLUniformLocation->GetUniformLocationId();

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
        return GetUniformInActive(env, programId, locationId, type);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBase::GetDrawingBufferWidth(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateInt64(env, 0).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateInt64(env, 0).val_;
    }
    return NVal::CreateInt64(env, context->mBitMapWidth).val_;
}

napi_value WebGLRenderingContextBase::GetDrawingBufferHeight(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateInt64(env, 0).val_;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateInt64(env, 0).val_;
    }
    return NVal::CreateInt64(env, context->mBitMapHeight).val_;
}

void WebGLRenderingContextBase::SetError(napi_env env, napi_value thisVar, GLenum error, std::string func, int line)
{
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, thisVar);
    if (context == nullptr) {
        return;
    }
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("SetError last error %{public}u", err);
    }
    LOGE("Set error %{public}u for %{public}s:%{public}d ", error, func.c_str(), line);
    context->GetWebGLRenderingContextImpl().SetError(error);
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
