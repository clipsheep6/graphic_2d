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
#include "../../common/napi/n_func_arg.h"
#include "../include/webgl/webgl_buffer.h"
#include "../include/webgl/webgl_framebuffer.h"
#include "../include/webgl/webgl_program.h"
#include "../include/webgl/webgl_renderbuffer.h"
#include "../include/webgl/webgl_shader.h"
#include "../include/webgl/webgl_texture.h"
#include "../include/webgl/webgl_image_source.h"
#include "../include/util/log.h"
#include "../include/util/util.h"
#include "../include/util/buffer_data.h"
#include "../include/webgl/webgl_uniform_location.h"

#include<string>
#include<vector>

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;
static const GLsizei MATRIX_2X2_REQUIRE_MIN_SIZE = 4;
static const GLsizei MATRIX_3X3_REQUIRE_MIN_SIZE = 9;
static const GLsizei MATRIX_4X4_REQUIRE_MIN_SIZE = 16;

static const GLsizei UNIFORM_2IV_REQUIRE_MIN_SIZE = 2;
static const GLsizei UNIFORM_3IV_REQUIRE_MIN_SIZE = 3;
static const GLsizei UNIFORM_4IV_REQUIRE_MIN_SIZE = 4;
static const GLsizei UNIFORM_2FV_REQUIRE_MIN_SIZE = 2;
static const GLsizei UNIFORM_3FV_REQUIRE_MIN_SIZE = 3;
static const GLsizei UNIFORM_4FV_REQUIRE_MIN_SIZE = 4;

napi_value WebGLRenderingContextOverloads::BufferData(napi_env env, napi_callback_info info)
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
    LOGI("WebGL bufferData target = %{public}u", target);
    bool usagesucc = NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_number);
    LOGI("WebGL bufferData usagesucc = %{public}u", usagesucc);
    ReadBufferData bufferData(env);
    uint64_t size;
    void *data = nullptr;
    if (usagesucc) {
        tie(succ, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
    } else {
        napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND]);
        if (status != 0) {
            LOGE("WebGL BufferData failed to getbuffer data");
            return nullptr;
        }
        bufferData.DumpBuffer(bufferData.GetBufferDataType());
        data = bufferData.GetBuffer();
        size = bufferData.GetBufferLength();
    }
    GLenum usage;
    tie(succ, usage) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL bufferData usage 0x%40{public}x", usage);
    LOGI("WebGL bufferData size %{public}u", size);
    glBufferData(static_cast<GLenum>(target), static_cast<GLsizeiptr>(size),
        static_cast<uint8_t*>(data), static_cast<GLenum>(usage));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::BufferSubData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bufferSubData start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    LOGI("WebGL bufferSubData succ = %{public}u", succ);
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL bufferSubData target = %{public}u", target);
    int64_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL bufferSubData offset = %{public}u", offset);

    ReadBufferData bufferData(env);
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD]);
        if (status != 0) {
            LOGE("WebGL BufferSubData failed to getbuffer data");
            return nullptr;
        }
        bufferData.DumpBuffer(bufferData.GetBufferDataType());
        void *data = bufferData.GetBuffer();
        size_t size = bufferData.GetBufferLength();
        LOGI("WebGL bufferSubData size = %{public}u", size);
        LOGI("WebGL bufferSubData data = %{public}u", data);
        glBufferSubData(static_cast<GLenum>(target), static_cast<GLintptr>(offset),
            static_cast<GLsizeiptr>(size), static_cast<uint8_t*>(data));
    } else {
        glBufferSubData(static_cast<GLenum>(target), static_cast<GLintptr>(offset),
            static_cast<GLsizeiptr>(0), nullptr);
    }
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::CompressedTexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL compressedTexImage2D start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexImage2D target = %{public}u", target);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexImage2D index = %{public}u", level);
    int64_t internalformat;
    tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexImage2D level = %{public}u", internalformat);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexImage2D width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexImage2D height = %{public}u", height);
    int64_t border;
    tie(succ, border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexImage2D border = %{public}u", border);

    if (funcArg[NARG_POS::SEVENTH] != nullptr) {
        ReadBufferData bufferData(env);
        napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SEVENTH]);
        if (status != 0) {
            LOGE("WebGL compressedTexImage2D failed to getbuffer data");
            return nullptr;
        }
        bufferData.DumpBuffer(bufferData.GetBufferDataType());
        if (bufferData.GetBufferType() != buffer_dataview || bufferData.GetBufferType() != buffer_typedarray) {
            LOGE("WebGL compressedTexImage2D invalid buffer type %{public}u", bufferData.GetBufferType());
            return nullptr;
        }
        void *data = bufferData.GetBuffer();
        size_t imageSize = bufferData.GetBufferLength();
        LOGI("WebGL compressedTexImage2D imageSize = %{public}u", imageSize);
        LOGI("WebGL compressedTexImage2D data = %{public}u", data);
        glCompressedTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
            static_cast<GLenum>(internalformat), static_cast<GLsizei>(width),
            static_cast<GLsizei>(height), static_cast<GLint>(border),
            static_cast<GLsizei>(imageSize), static_cast<void*>(data));
    } else {
        // todo
        glCompressedTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
            static_cast<GLenum>(internalformat), static_cast<GLsizei>(width),
            static_cast<GLsizei>(height), static_cast<GLint>(border),
            static_cast<GLsizei>(0), static_cast<void*>(0));
    }
    LOGI("WebGL compressedTexImage2D end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::CompressedTexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::EIGHT)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL compressedTexSubImage2D start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexSubImage2D target = %{public}u", target);
    int32_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexSubImage2D level = %{public}u", level);
    int64_t xoffset;
    tie(succ, xoffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexSubImage2D xoffset = %{public}u", xoffset);
    int64_t yoffset;
    tie(succ, yoffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexSubImage2D yoffset = %{public}u", yoffset);
    int32_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexSubImage2D width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexSubImage2D height = %{public}u", height);
    int64_t format;
    tie(succ, format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL compressedTexSubImage2D format = %{public}u", format);
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::EIGHTH]);
    if (status != 0) {
        LOGE("WebGL compressedTexSubImage2D failed to get buffer");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_dataview) {
        LOGE("WebGL compressedTexSubImage2D invalid buffer type %{public}u", bufferData.GetBufferType());
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    size_t imageSize = bufferData.GetBufferLength();

    LOGI("WebGL compressedTexSubImage2D imageSize = %{public}u", imageSize);
    LOGI("WebGL compressedTexSubImage2D data = %{public}u", data);
    glCompressedTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(xoffset),
        static_cast<GLint>(yoffset), static_cast<GLsizei>(width), static_cast<GLsizei>(height),
        static_cast<GLenum>(format), static_cast<GLsizei>(imageSize),
        static_cast<void*>(data));
    LOGI("WebGL compressedTexSubImage2D end");
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::ReadPixels(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL readPixels start");
    int64_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL readPixels  = %{public}u", x);
    int64_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL readPixels y = %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL readPixels width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL readPixels height = %{public}u", height);
    int64_t format;
    tie(succ, format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL readPixels format = %{public}u", format);
    int64_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL readPixels type = %{public}u", type);
    if (funcArg[NARG_POS::SEVENTH] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL readPixels imageSize start");
    size_t imageSize;
    void *pixels = nullptr;
    tie(succ, pixels, imageSize) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToDataview();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL readPixels pixels = %{public}u", pixels);
    glReadPixels(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
        static_cast<GLsizei>(height), static_cast<GLenum>(format),
        static_cast<GLenum>(type), static_cast<void*>(pixels));
    LOGI("WebGL readPixels end");
    return nullptr;
}

static bool GetImage2DArg(napi_env env, const NFuncArg &funcArg, WebGLImage2DArg &imgArg)
{
    bool succ = false;
    tie(succ, imgArg.border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return false;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt32();
    if (!succ) {
        return false;
    }
    tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
    if (!succ) {
        return false;
    }
    if (funcArg[NARG_POS::NINTH] == nullptr) {
        LOGE("WebGl TexImage2D last param is null");
        return false;
    }
    if (NVal(env, funcArg[NARG_POS::NINTH]).IsDataView()) {
        LOGE("WebGl TexImage2D not support other date view");
        return false;
    } else if (NVal(env, funcArg[NARG_POS::NINTH]).TypeIs(napi_number)) {
        LOGE("WebGl TexImage2D not support other number");
        return false;
    } else {
        unique_ptr<WebGLImageSource> imageSource = WebGLImageSource::GetImageSource(env, funcArg[NARG_POS::NINTH]);
        if (imageSource != nullptr) {
            imgArg.imageSource = imageSource.get();
            imageSource.reset();
        } else {
            return false;
        }
    }

    return true;
}

napi_value WebGLRenderingContextOverloads::TexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!(funcArg.InitArgs(NARG_CNT::NINE) || funcArg.InitArgs(NARG_CNT::SIX))) {
        return nullptr;
    }

    WebGLImage2DArg imgArg = {};
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        LOGE("WebGl TexImage2D parse target failed");
        return nullptr;
    }
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        LOGE("WebGl TexImage2D parse level failed");
        return nullptr;
    }
    tie(succ, imgArg.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        LOGE("WebGl TexImage2D parse internalFormat failed");
        return nullptr;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        LOGE("WebGl TexImage2D parse format failed");
        return nullptr;
    }
    tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        LOGE("WebGl TexImage2D parse type failed");
        return nullptr;
    }
    bool usagesucc = NVal(env, funcArg[NARG_POS::SIXTH]).TypeIs(napi_number);
    LOGI("WebGl TexImage2D usagesucc: %{public}d", usagesucc);
    if (usagesucc) {
        succ = GetImage2DArg(env, funcArg, imgArg);
        if (!succ) {
            LOGE("WebGl TexImage2D get arg fail");
            return nullptr;
        }
    } else {
        unique_ptr<WebGLImageSource> imageSource = WebGLImageSource::GetImageSource(env, funcArg[NARG_POS::SIXTH]);
        if (imageSource != nullptr) {
            imgArg.imageSource = imageSource.get();
            imageSource.reset();
        } else {
            return nullptr;
        }
    }

    if (imgArg.imageSource != nullptr) {
        glTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat,
            imgArg.imageSource->width, imgArg.imageSource->height, imgArg.border,
            imgArg.format, imgArg.type, static_cast<void*>(imgArg.imageSource->source.get()));
    } else {
        glTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat,
            imgArg.width, imgArg.height, imgArg.border,
            imgArg.format, imgArg.type, static_cast<void*>(imgArg.source));
    }
    return nullptr;
}

static bool GetSubImage2DArg(napi_env env, const NFuncArg &funcArg, WebGLSubImage2DArg &imgArg)
{
    bool succ = false;
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return false;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return false;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
    if (!succ) {
        return false;
    }
    tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
    if (!succ) {
        return false;
    }
    if (funcArg[NARG_POS::NINTH] == nullptr) {
        return false;
    }
    if (NVal(env, funcArg[NARG_POS::NINTH]).IsDataView()) {
        LOGE("WebGl GetSubImage2DArg not support other date view");
        return false;
    } else if (NVal(env, funcArg[NARG_POS::NINTH]).TypeIs(napi_number)) {
        LOGE("WebGl GetSubImage2DArg not support other number");
        return false;
    } else {
        unique_ptr<WebGLImageSource> imageSource = WebGLImageSource::GetImageSource(env, funcArg[NARG_POS::NINTH]);
        if (imageSource != nullptr) {
            imgArg.imageSource = imageSource.get();
            imageSource.reset();
        } else {
            return false;
        }
    }
    return true;
}

static bool GetSubImage2DArgFromImageSource(napi_env env, const NFuncArg &funcArg, WebGLSubImage2DArg &imgArg)
{
    bool succ = false;
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        LOGE("WebGl TexSubImage2D parse format failed");
        return false;
    }
    tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        LOGE("WebGl TexSubImage2D parse type failed");
        return false;
    }
    unique_ptr<WebGLImageSource> imageSource = WebGLImageSource::GetImageSource(env, funcArg[NARG_POS::SEVENTH]);
    if (imageSource != nullptr) {
        imgArg.imageSource = imageSource.get();
        imageSource.reset();
    } else {
        return false;
    }
    return true;
}

napi_value WebGLRenderingContextOverloads::TexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!(funcArg.InitArgs(NARG_CNT::NINE) || funcArg.InitArgs(NARG_CNT::SEVEN))) {
        return nullptr;
    }
    bool succ = false;
    WebGLSubImage2DArg imgArg = {};
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        LOGE("WebGl TexSubImage2D parse target failed");
        return nullptr;
    }
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        LOGE("WebGl TexSubImage2D parse level failed");
        return nullptr;
    }
    tie(succ, imgArg.xoffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        LOGE("WebGl TexSubImage2D parse xoffset failed");
        return nullptr;
    }
    tie(succ, imgArg.yoffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        LOGE("WebGl TexSubImage2D parse yoffset failed");
        return nullptr;
    }

    bool usageNumber = NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number);
    LOGI("WebGl TexSubImage2D usagesucc: %{public}d", usageNumber);
    if (usageNumber) {
        succ = GetSubImage2DArg(env, funcArg, imgArg);
    } else {
        succ = GetSubImage2DArgFromImageSource(env, funcArg, imgArg);
    }
    if (!succ) {
        LOGE("WebGl TexSubImage2D get arg fail");
        return nullptr;
    }
    if (imgArg.imageSource != nullptr) {
        glTexSubImage2D(imgArg.target, imgArg.level, imgArg.xoffset, imgArg.yoffset,
            imgArg.imageSource->width, imgArg.imageSource->height,
            imgArg.format, imgArg.type, static_cast<void*>(imgArg.imageSource->source.get()));
    } else {
        glTexSubImage2D(imgArg.target, imgArg.level, imgArg.xoffset, imgArg.yoffset,
            imgArg.width, imgArg.height,
            imgArg.format, imgArg.type, static_cast<void*>(imgArg.source));
    }
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform1fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLUniformLocation *webGLUniformlocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformlocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    GLint location = webGLUniformlocation->GetUniformLocationId();
    LOGI("WebGL uniform1fv location = %{public}u", location);

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL uniform1fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGE("WebGL :uniform1fv invalid buffer type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL :uniform1fv invalid buffer data type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    GLsizei count = bufferData.GetBufferLength() / sizeof(float);

    LOGI("WebGL uniform1fv count = %{public}u", count);
    glUniform1fv(location, count, static_cast<GLfloat*>(data));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL uniform2fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform2fv location = %{public}u", location);

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL uniform2fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGI("WebGL uniform2fv invalid buffer type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGI("WebGL uniform2fv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    GLsizei count = bufferData.GetBufferLength() / sizeof(float);
    if (count < UNIFORM_2FV_REQUIRE_MIN_SIZE || (count % UNIFORM_2FV_REQUIRE_MIN_SIZE)) {
        LOGE("WebGL uniform2fv invalid value, count %{public}d", count);
        return nullptr;
    }
    LOGI("WebGL uniform2fv length %{public}u", bufferData.GetBufferLength());
    glUniform2fv(location, count >> 1, static_cast<GLfloat*>(data));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL uniform3fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderingContextOverloads::uniform3fv location = %{public}u", location);

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL uniform3fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGE("WebGL uniform3fv invalid buffer type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL uniform3fv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    size_t count = bufferData.GetBufferLength() / sizeof(float);
    if (count < UNIFORM_3FV_REQUIRE_MIN_SIZE || (count % UNIFORM_3FV_REQUIRE_MIN_SIZE)) {
        LOGE("WebGL uniform3fv invalid value, count %{public}d", count);
        return nullptr;
    }
    LOGI("WebGL uniform3fv count %{public}u length %{public}u", count, bufferData.GetBufferLength());
    glUniform3fv(static_cast<GLint>(location), count / 3, static_cast<GLfloat*>(data));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
            return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL uniform4fv location = %{public}u", location);

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL uniform4fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGE("WebGL uniform4fv invalid buffer type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL uniform4fv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    GLsizei count = bufferData.GetBufferLength() / sizeof(float);
    if (count < UNIFORM_4FV_REQUIRE_MIN_SIZE || (count % UNIFORM_4FV_REQUIRE_MIN_SIZE)) {
        LOGE("WebGL uniform4fv invalid value, count %{public}d", count);
        return nullptr;
    }
    LOGI("WebGL uniform4fv length %{public}u", bufferData.GetBufferLength());
    glUniform4fv(location, count >> 2, static_cast<GLfloat*>(data));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform1iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL uniform1iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL uniform1iv location = %{public}u", location);

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_int32);
    if (status != 0) {
        LOGE("WebGL uniform1iv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGE("WebGL uniform1iv invalid buffer type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_int32) {
        LOGE("WebGL uniform1iv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    GLsizei count = bufferData.GetBufferLength() / sizeof(uint32_t);
    LOGI("WebGL uniform1iv count %{public}u length %{public}u", count, bufferData.GetBufferLength());
    glUniform1iv(location, count, static_cast<GLint*>(data));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform2iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    LOGI("WebGL uniform2iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL uniform2iv location = %{public}u", location);

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_int32);
    if (status != 0) {
        LOGE("WebGL uniform2iv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGI("WebGL uniform2iv invalid buffer type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_int32) {
        LOGI("WebGL uniform2iv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    GLsizei count = bufferData.GetBufferLength() / sizeof(uint32_t);
    if (count < UNIFORM_2IV_REQUIRE_MIN_SIZE || (count % UNIFORM_2IV_REQUIRE_MIN_SIZE)) {
        LOGE("WebGL uniform2iv invalid value, count %{public}d", count);
        return nullptr;
    }
    LOGI("WebGL uniform2iv length %{public}u", bufferData.GetBufferLength());
    glUniform2iv(location, count >> 1, static_cast<GLint*>(data));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform3iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL uniform3iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL uniform3iv location = %{public}u", location);

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_int32);
    if (status != 0) {
        LOGE("WebGL uniform3iv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGE("WebGL uniform3iv invalid buffer type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_int32) {
        LOGE("WebGL uniform3iv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    GLsizei count = bufferData.GetBufferLength() / sizeof(uint32_t);
    if (count < UNIFORM_3IV_REQUIRE_MIN_SIZE || (count % UNIFORM_3IV_REQUIRE_MIN_SIZE)) {
        LOGE("WebGL uniform3iv invalid value, count %{public}d", count);
        return nullptr;
    }
    LOGI("WebGL uniform3iv length %{public}u", bufferData.GetBufferLength());
    glUniform3iv(location, count / 3, static_cast<GLint*>(data));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::Uniform4iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL uniform4iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL uniform4iv location = %{public}u", location);

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_int32);
    if (status != 0) {
        LOGE("WebGL uniform4iv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGE("WebGL uniform4iv invalid buffer type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_int32) {
        LOGE("WebGL uniform4iv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    GLsizei count = bufferData.GetBufferLength() / sizeof(uint32_t);
    if (count < UNIFORM_4IV_REQUIRE_MIN_SIZE || (count % UNIFORM_4IV_REQUIRE_MIN_SIZE)) {
        LOGE("WebGL uniform4iv invalid value, count %{public}d", count);
        return nullptr;
    }
    LOGI("WebGL uniform4iv length %{public}u", bufferData.GetBufferLength());
    glUniform4iv(static_cast<GLint>(location), count >> 2, static_cast<GLint*>(data));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::UniformMatrix2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
    return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniformMatrix2fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL uniformMatrix2fv location = %{public}d", location);
    GLboolean transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    LOGI("WebGL uniformMatrix2fv succ = %{public}u", succ);
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniformMatrix2fv transpose = %{public}u", transpose);

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL uniformMatrix2fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGE("WebGL uniformMatrix2fv invalid buffer type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL uniformMatrix2fv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    size_t count = bufferData.GetBufferLength() / sizeof(float);
    if (count < MATRIX_2X2_REQUIRE_MIN_SIZE || (count % MATRIX_2X2_REQUIRE_MIN_SIZE)) {
        LOGE("WebGL uniformMatrix2fv invalid value, count %{public}d", count);
        return nullptr;
    }
    LOGI("WebGL uniformMatrix2fv length %{public}u", bufferData.GetBufferLength());
    glUniformMatrix2fv(static_cast<GLint>(location), count >> 2, transpose, static_cast<GLfloat*>(data));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::UniformMatrix3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniformMatrix3fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL uniformMatrix3fv location = %{public}d", location);
    GLboolean transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniformMatrix3fv transpose = %{public}u", transpose);
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL uniformMatrix3fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGE("WebGL uniformMatrix3fv invalid buffer type = %{public}u", static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL uniformMatrix3fv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    GLsizei count = bufferData.GetBufferLength() / sizeof(float);
    if (count < MATRIX_3X3_REQUIRE_MIN_SIZE || (count % MATRIX_3X3_REQUIRE_MIN_SIZE)) {
        LOGE("WebGL uniformMatrix3fv invalid value, count %{public}d", count);
        return nullptr;
    }
    LOGI("WebGL uniformMatrix3fv length %{public}u", bufferData.GetBufferLength());
    glUniformMatrix3fv(location, count / 9, transpose, static_cast<GLfloat*>(data));
    return nullptr;
}

napi_value WebGLRenderingContextOverloads::UniformMatrix4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniformMatrix4fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status uniformlocationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (uniformlocationStatus != napi_ok) {
        return nullptr;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL uniformMatrix4fv location = %{public}u", location);
    GLboolean transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL uniformMatrix4fv transpose = %{public}u", transpose);
        ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL uniformMatrix4fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() != buffer_typedarray && bufferData.GetBufferType() != buffer_array) {
        LOGE("WebGL uniformMatrix4fv invalid buffer type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferType()));
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL uniformMatrix4fv invalid buffer data type = %{public}u",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return nullptr;
    }
    void *data = bufferData.GetBuffer();
    GLsizei count = bufferData.GetBufferLength() / sizeof(float);
    if (count < MATRIX_4X4_REQUIRE_MIN_SIZE || (count % MATRIX_4X4_REQUIRE_MIN_SIZE)) {
        LOGE("WebGL uniformMatrix4fv invalid value, count %{public}d", count);
        return nullptr;
    }
    LOGI("WebGL uniformMatrix4fv length %{public}u", bufferData.GetBufferLength());
    glUniformMatrix4fv(location, count >> 4, transpose, static_cast<GLfloat*>(data));
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif