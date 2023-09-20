/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "../include/context/webgl_rendering_context_overloads.h"

#include "../include/context/webgl_rendering_context_base.h"
#include "../include/context/webgl_rendering_context_base_impl.h"
#include "../../common/napi/n_func_arg.h"
#include "../include/webgl/webgl_buffer.h"
#include "../include/webgl/webgl_framebuffer.h"
#include "../include/webgl/webgl_program.h"
#include "../include/webgl/webgl_renderbuffer.h"
#include "../include/webgl/webgl_shader.h"
#include "../include/webgl/webgl_texture.h"
#include "../include/webgl/webgl_arg.h"
#include "../include/webgl/webgl_uniform_location.h"
#include "../include/util/log.h"
#include "../include/util/util.h"

#include<string>
#include<vector>

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;
napi_value WebGLRenderingContextOverloads::BufferData(napi_env env, napi_callback_info info)
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
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    GLenum usage;
    tie(succ, usage) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }

    bool usageSucc = NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_number);
    if (usageSucc) {
        int64_t size;
        tie(succ, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        return context->GetWebGLRenderingContextImpl().BufferData(env, target, static_cast<GLsizeiptr>(size), usage);
    }

    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
    }
    int64_t length = 0;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
    }
    return context->GetWebGLRenderingContextImpl().BufferData(env, target,
        funcArg[NARG_POS::SECOND], usage, srcOffset, length);
}

napi_value WebGLRenderingContextOverloads::BufferSubData(napi_env env, napi_callback_info info)
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
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    int64_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().BufferSubData(env, target,
        static_cast<GLintptr>(offset), funcArg[NARG_POS::THIRD], 0, 0);
}

napi_value WebGLRenderingContextOverloads::CompressedTexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN)) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    Impl::Image2DArg imgArg = { 0 };
    imgArg.func = Impl::IMAGE_2D_FUNC_COMPRESSED_TEX_IMAGE_2D;
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    tie(succ, imgArg.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    tie(succ, imgArg.border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    bool usageNumber = NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number);
    if (context->GetWebGLRenderingContextImpl().IsHighWebGL() && usageNumber) {
        int64_t imageSize = 0;
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        int64_t offset = 0;
        tie(succ, offset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        return context->GetWebGLRenderingContextImpl().CompressedTexImage2D(env,
            &imgArg, static_cast<GLsizei>(imageSize), static_cast<GLintptr>(offset));
    }

    GLuint srcOffset = 0;
    if (funcArg[NARG_POS::EIGHTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToUint32();
    }
    GLuint srcLengthOverride = 0;
    if (funcArg[NARG_POS::NINTH] != nullptr) {
        tie(succ, srcLengthOverride) = NVal(env, funcArg[NARG_POS::NINTH]).ToUint32();
    }
    return context->GetWebGLRenderingContextImpl().CompressedTexImage2D(env,
        &imgArg, funcArg[NARG_POS::SEVENTH], srcOffset, srcLengthOverride);
}

static bool CheckTieResult(Impl::Image2DArg &imgArg, napi_env env, NFuncArg &funcArg)
{
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        imgArg.target = 0;
    }
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return false;
    }
    tie(succ, imgArg.xoffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return false;
    }
    tie(succ, imgArg.yoffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return false;
    }
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return false;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return false;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToGLenum();
    if (!succ) {
        return false;
    }
    return true;
}

napi_value WebGLRenderingContextOverloads::CompressedTexSubImage2D(napi_env env, napi_callback_info info)
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
    imgArg.func = Impl::IMAGE_2D_FUNC_COMPRESSED_TEX_SUB_IMAGE_2D;
    if (!CheckTieResult(imgArg, env, funcArg)) {
        return nullptr;
    }

    bool usageNumber = NVal(env, funcArg[NARG_POS::EIGHTH]).TypeIs(napi_number);
    bool succ = false;
    if (context->GetWebGLRenderingContextImpl().IsHighWebGL() && usageNumber) {
        int64_t imageSize = 0;
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        int64_t offset = 0;
        tie(succ, offset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        return context->GetWebGLRenderingContextImpl().CompressedTexSubImage2D(env,
            &imgArg, static_cast<GLsizei>(imageSize), static_cast<GLintptr>(offset));
    }

    GLuint srcOffset = 0;
    if (funcArg[NARG_POS::NINTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToUint32();
        if (!succ) {
            return nullptr;
        }
    }
    GLuint srcLengthOverride = 0;
    if (funcArg[NARG_POS::TENTH] != nullptr) {
        tie(succ, srcLengthOverride) = NVal(env, funcArg[NARG_POS::NINTH]).ToUint32();
        if (!succ) {
            return nullptr;
        }
    }
    return context->GetWebGLRenderingContextImpl().CompressedTexImage2D(env,
        &imgArg, funcArg[NARG_POS::EIGHTH], srcOffset, srcLengthOverride);
}

napi_value WebGLRenderingContextOverloads::ReadPixels(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    bool succ;
    Impl::PixelsArg arg = {0};
    tie(succ, arg.x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, arg.y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    int64_t data = 0;
    tie(succ, data) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    arg.width = static_cast<GLsizei>(data);
    tie(succ, data) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    arg.height = static_cast<GLsizei>(data);
    tie(succ, arg.format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLenum();
    if (!succ) {
        arg.format = 0;
    }
    tie(succ, arg.type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLenum();
    if (!succ) {
        arg.type = 0;
    }
    GLintptr offset = 0;
    if (NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number)) {
        tie(succ, data) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        offset = static_cast<GLintptr>(data);
        return context->GetWebGLRenderingContextImpl().ReadPixels(env, &arg, offset);
    }
    GLuint dstOffset = 0;
    if (!NVal(env, funcArg[NARG_POS::EIGHTH]).IsNull()) {
        tie(succ, dstOffset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToUint32();
        if (!succ) {
            return nullptr;
        }
    }
    return context->GetWebGLRenderingContextImpl().ReadPixels(env, &arg, funcArg[NARG_POS::SEVENTH], dstOffset);
}

napi_value WebGLRenderingContextOverloads::TexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!(funcArg.InitArgs(NARG_CNT::NINE) || funcArg.InitArgs(NARG_CNT::SIX))) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    Impl::Image2DArg imgArg = { 0 };
    imgArg.func = Impl::IMAGE_2D_FUNC_TEXT_IMAGE_2D;
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    bool usageNumber = NVal(env, funcArg[NARG_POS::SIXTH]).TypeIs(napi_number);
    LOGI("WebGl TexImage2D usageNumber: %{public}d", usageNumber);
    if (!usageNumber) { // for source: TexImageSource
        tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLenum();
        if (!succ) {
            return nullptr;
        }
        tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLenum();
        if (!succ) {
            return nullptr;
        }
        return context->GetWebGLRenderingContextImpl().TexImage2D(env, &imgArg, funcArg[NARG_POS::SIXTH]);
    }
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    usageNumber = NVal(env, funcArg[NARG_POS::NINTH]).TypeIs(napi_number);
    LOGI("WebGl TexImage2D usageNumber: %{public}d", usageNumber);
    if (context->GetWebGLRenderingContextImpl().IsHighWebGL() && usageNumber) { //support pboOffset
        int64_t pboOffset = 0;
        tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        return context->GetWebGLRenderingContextImpl().TexImage2D(env, &imgArg, static_cast<GLintptr>(pboOffset));
    } else if (NVal(env, funcArg[NARG_POS::NINTH]).IsBufferArray()) {
        return context->GetWebGLRenderingContextImpl().TexImage2D(env, &imgArg, funcArg[NARG_POS::NINTH], 0);
    } else {
        return context->GetWebGLRenderingContextImpl().TexImage2D(env, &imgArg, funcArg[NARG_POS::NINTH]);
    }
}

napi_value WebGLRenderingContextOverloads::TexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!(funcArg.InitArgs(NARG_CNT::NINE) || funcArg.InitArgs(NARG_CNT::SEVEN))) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    Impl::Image2DArg imgArg = { 0 };
    imgArg.func = Impl::IMAGE_2D_FUNC_SUB_IMAGE_2D;
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
    bool usageNumber = NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number);
    if (!usageNumber) {
        tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        return context->GetWebGLRenderingContextImpl().TexSubImage2D(env, &imgArg, funcArg[NARG_POS::SEVENTH]);
    }

    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    usageNumber = NVal(env, funcArg[NARG_POS::NINTH]).TypeIs(napi_number);
    if (context->GetWebGLRenderingContextImpl().IsHighWebGL() && usageNumber) { //support pboOffset
        int64_t pboOffset = 0;
        tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        return context->GetWebGLRenderingContextImpl().TexSubImage2D(env, &imgArg, static_cast<GLintptr>(pboOffset));
    } else if (NVal(env, funcArg[NARG_POS::NINTH]).IsBufferArray()) {
        return context->GetWebGLRenderingContextImpl().TexSubImage2D(env, &imgArg, funcArg[NARG_POS::NINTH], 0);
    } else {
        return context->GetWebGLRenderingContextImpl().TexSubImage2D(env, &imgArg, funcArg[NARG_POS::NINTH]);
    }
}

static bool GetUniformExtInfo(napi_env env, const NFuncArg &funcArg, Impl::UniformExtInfo *info)
{
    bool succ;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, info->srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToUint32();
        if (!succ) {
            return false;
        }
        LOGI("WebGL UniformMatrixInfo srcOffset = %{public}u", info->srcOffset);
    }
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, info->srcLength) = NVal(env, funcArg[NARG_POS::FIFTH]).ToUint32();
        if (!succ) {
            return false;
        }
        LOGI("WebGL UniformMatrixInfo srcLength = %{public}u", info->srcLength);
    }
    return true;
}

napi_value WebGLRenderingContextOverloads::Uniform1fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = 1;
    bool succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Uniformfv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = WebGLArg::UNIFORM_2V_REQUIRE_MIN_SIZE;
    bool succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Uniformfv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = WebGLArg::UNIFORM_3V_REQUIRE_MIN_SIZE;
    bool succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Uniformfv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = WebGLArg::UNIFORM_4V_REQUIRE_MIN_SIZE;
    bool succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Uniformfv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform1iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = 1;
    bool succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Uniformiv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform2iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = WebGLArg::UNIFORM_2V_REQUIRE_MIN_SIZE;
    bool succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Uniformiv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform3iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = WebGLArg::UNIFORM_3V_REQUIRE_MIN_SIZE;
    bool succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Uniformiv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform4iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = WebGLArg::UNIFORM_4V_REQUIRE_MIN_SIZE;
    bool succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Uniformiv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::UniformMatrix2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = WebGLArg::MATRIX_2X2_REQUIRE_MIN_SIZE;
    bool succ;
    bool transpose = false;
    tie(succ, transpose)  = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    return context->GetWebGLRenderingContextImpl().UniformMatrixfv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}

napi_value WebGLRenderingContextOverloads::UniformMatrix3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = WebGLArg::MATRIX_3X3_REQUIRE_MIN_SIZE;
    bool succ;
    bool transpose = false;
    tie(succ, transpose)  = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    return context->GetWebGLRenderingContextImpl().UniformMatrixfv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}

napi_value WebGLRenderingContextOverloads::UniformMatrix4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }

    Impl::UniformExtInfo extInfo = {};
    extInfo.dimension = WebGLArg::MATRIX_4X4_REQUIRE_MIN_SIZE;
    bool succ;
    bool transpose = false;
    tie(succ, transpose)  = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    succ = GetUniformExtInfo(env, funcArg, &extInfo);
    if (!succ) {
        return nullptr;
    }
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    return context->GetWebGLRenderingContextImpl().UniformMatrixfv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif