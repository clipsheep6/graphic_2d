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
#include "../include/context/webgl2_rendering_context_overloads.h"
#include "../include/webgl/webgl_uniform_location.h"
#include "../include/webgl/webgl_image_source.h"
#include "../../common/napi/n_func_arg.h"
#include "../include/webgl/webgl_buffer.h"
#include "../include/webgl/webgl_framebuffer.h"
#include "../include/webgl/webgl_program.h"
#include "../include/webgl/webgl_renderbuffer.h"
#include "../include/webgl/webgl_shader.h"
#include "../include/webgl/webgl_texture.h"
#include "../include/util/log.h"
#include "../include/util/buffer_data.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;

napi_value WebGL2RenderingContextOverloads::BufferData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    int64_t usage;
    tie(succ, usage) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 bufferData target = %{public}u usage %{public}u ", target, usage);
    ReadBufferData bufferData(env);
    GLvoid *data = nullptr;
    int64_t size = 0;
    bool usageSucc = NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_number);
    if (usageSucc) {
        tie(succ, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return nullptr;
        }
    } else {
        int64_t srcOffset = 0;
        if (funcArg[NARG_POS::FOURTH] != nullptr) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 bufferData srcOffset = %{public}u", srcOffset);
        }
        int64_t length = 0;
        if (funcArg[NARG_POS::FIFTH] != nullptr) {
            tie(succ, length) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 bufferData length = %{public}u", length);
        }
        napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND]);
        if (status != 0) {
            LOGE("WebGL BufferData failed to getbuffer data");
            return nullptr;
        }
        bufferData.DumpBuffer(bufferData.GetBufferDataType());
        data = reinterpret_cast<GLvoid *>(bufferData.GetBuffer() + srcOffset);
        size = length != 0 ? length : bufferData.GetBufferLength();
    }
    LOGI("WebGL2 bufferData size = %{public}u", size);
    LOGI("WebGL2 bufferData usage = %{public}u", usage);
    glBufferData(static_cast<GLenum>(target), static_cast<GLsizeiptr>(size),
                 static_cast<GLvoid *>(data), static_cast<GLenum>(usage));
    LOGI("WebGL2 bufferData end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::BufferSubData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 bufferSubData target = %{public}u", target);
    int64_t dstByteOffset;
    tie(succ, dstByteOffset) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 bufferSubData dstByteOffset = %{public}u", dstByteOffset);

    // srcOffset
    int64_t srcOffset = 0;
    bool usageSucc = NVal(env, funcArg[NARG_POS::THIRD]).TypeIs(napi_number);
    if (usageSucc) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 bufferSubData srcOffset = %{public}u", srcOffset);
        // TODO
        //glBufferSubData(static_cast<GLenum>(target), static_cast<GLintptr>(dstByteOffset),
        //                static_cast<GLsizeiptr>(0), reinterpret_cast<GLvoid *>(srcOffset));
    } else {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 bufferSubData srcOffset = %{public}u", srcOffset);
        int64_t length = 0;
        if (funcArg[NARG_POS::FIFTH] != nullptr) {
            tie(succ, length) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 bufferSubData length = %{public}u", length);
        }
        ReadBufferData bufferData(env);
        napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD]);
        if (status != 0) {
            LOGE("WebGL BufferData failed to getbuffer data");
            return nullptr;
        }
        bufferData.DumpBuffer(bufferData.GetBufferDataType());
        GLvoid *srcData = reinterpret_cast<GLvoid *>(bufferData.GetBuffer() + srcOffset);
        length = length != 0 ? length : bufferData.GetBufferLength();
        glBufferSubData(static_cast<GLenum>(target), static_cast<GLintptr>(dstByteOffset),
                        static_cast<GLsizeiptr>(length), reinterpret_cast<GLvoid *>(srcData));
    }
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
    size_t imageSize;
    void *pixels = nullptr;
    if (NVal(env, funcArg[NARG_POS::NINTH]).IsDataView()) {
        tie(succ, pixels, imageSize) = NVal(env, funcArg[NARG_POS::NINTH]).ToDataview();
        if (!succ) {
            return false;
        }
        int64_t srcOffset = 0;
        if (funcArg[NARG_POS::TENTH] != nullptr) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
            if (!succ) {
                return false;
            }
        }
        imgArg.source = reinterpret_cast<uint8_t *>((reinterpret_cast<uint8_t *>(pixels) + srcOffset));
    } else if (NVal(env, funcArg[NARG_POS::NINTH]).TypeIs(napi_number)) {
        int64_t pboOffset = 0;
        tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        if (!succ) {
            return false;
        }
        LOGE("WebGl TexImage2D parse pboOffset %{public}x", pboOffset);
        imgArg.source  = reinterpret_cast<uint8_t *>(pboOffset);
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

napi_value WebGL2RenderingContextOverloads::TexImage2D(napi_env env, napi_callback_info info)
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
        usagesucc = GetImage2DArg(env, funcArg, imgArg);
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
        size_t imageSize;
        void *pixels = nullptr;
        tie(succ, pixels, imageSize) = NVal(env, funcArg[NARG_POS::NINTH]).ToDataview();
        if (!succ) {
            return false;
        }
        int64_t srcOffset = 0;
        if (funcArg[NARG_POS::TENTH] != nullptr) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
            if (!succ) {
                return false;
            }
        }
        imgArg.source = reinterpret_cast<uint8_t *>((reinterpret_cast<uint8_t *>(pixels) + srcOffset));
    } else if (NVal(env, funcArg[NARG_POS::NINTH]).TypeIs(napi_number)) {
        int64_t pboOffset = 0;
        tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        if (!succ) {
            return false;
        }
        LOGE("WebGl TexImage2D parse pboOffset %{public}x", pboOffset);
        imgArg.source  = reinterpret_cast<uint8_t *>(pboOffset);
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

napi_value WebGL2RenderingContextOverloads::TexSubImage2D(napi_env env, napi_callback_info info)
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

    if (imgArg.imageSource != nullptr) {
        glTexSubImage2D(imgArg.target, imgArg.level, imgArg.xoffset, imgArg.yoffset,
            imgArg.imageSource->width, imgArg.imageSource->height,
            imgArg.format, imgArg.type, static_cast<void*>(imgArg.imageSource->source.get()));
    } else {
        glTexImage2D(imgArg.target, imgArg.level, imgArg.xoffset, imgArg.yoffset,
            imgArg.width, imgArg.height,
            imgArg.format, imgArg.type, static_cast<void*>(imgArg.source));
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::CompressedTexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN, NARG_CNT::NINE)) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexImage2D start");
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexImage2D target = %{public}u", target);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexImage2D level = %{public}u", level);
    int64_t internalFormat;
    tie(succ, internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexImage2D internalFormat = %{public}u", internalFormat);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexImage2D width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexImage2D height = %{public}u", height);
    int64_t border;
    tie(succ, border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexImage2D border = %{public}u", border);
    GLvoid *srcData = nullptr;
    int64_t imageSize = 0;
    bool usageSucc = NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number);
    if (usageSucc) {
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 compressedTexImage2D imageSize = %{public}u", imageSize);
        int64_t offset;
        tie(succ, offset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 compressedTexImage2D offset = %{public}u", offset);
        char *srcDataBase = static_cast<char *>(srcData);
        srcData = reinterpret_cast<GLvoid *>((srcDataBase + offset));
    } else {
        int64_t srcOffset = 0;
        if (funcArg[NARG_POS::NINTH] != nullptr) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderingContextOverloads::compressedTexSubImage2D srcOffset = %{public}u", srcOffset);
        }
        int64_t srcLengthOverride;
        if (funcArg[NARG_POS::TENTH] != nullptr) {
            tie(succ, srcLengthOverride) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("compressedTexSubImage2D srcLengthOverride = %{public}u", srcLengthOverride);
        }
        tie(succ, srcData, srcLengthOverride) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToDataview();
        if (!succ) {
            return nullptr;
        }
        char *srcDataBase = static_cast<char *>(srcData);
        srcData = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
        LOGI("WebGL2 compressedTexImage2D srcData = %{public}u", srcData);
    }
    glCompressedTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
                           static_cast<GLenum>(internalFormat), static_cast<GLsizei>(width),
                           static_cast<GLsizei>(height), static_cast<GLint>(border),
                           static_cast<GLsizei>(imageSize), reinterpret_cast<GLvoid *>(srcData));
    LOGI("WebGL2 compressedTexImage2D end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::CompressedTexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::EIGHT, NARG_CNT::TEN)) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexSubImage2D start");
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexSubImage2D target = %{public}u", target);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexSubImage2D level = %{public}u", level);
    int64_t xOffset;
    tie(succ, xOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexSubImage2D xOffset = %{public}u", xOffset);
    int64_t yOffset;
    tie(succ, yOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexSubImage2D yOffset = %{public}u", yOffset);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexSubImage2D width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexSubImage2D height = %{public}u", height);
    int64_t format;
    tie(succ, format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 compressedTexSubImage2D format = %{public}u", format);
    int64_t imageSize = 0;
    GLvoid *srcData = nullptr;
    bool usageSucc = NVal(env, funcArg[NARG_POS::EIGHTH]).TypeIs(napi_number);
    if (usageSucc) {
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 compressedTexSubImage2D imageSize = %{public}u", imageSize);
        int64_t offset;
        tie(succ, offset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 compressedTexSubImage2D offset = %{public}u", offset);
        char *srcDataBase = static_cast<char *>(srcData);
        srcData = reinterpret_cast<GLvoid *>((srcDataBase + offset));
        LOGI("WebGL2 compressedTexSubImage2D srcData = %{public}u", srcData);
    } else {
        int64_t srcOffset = 0;
        if (funcArg[NARG_POS::NINTH] != nullptr) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderingContextOverloads::compressedTexSubImage2D srcOffset = %{public}u", srcOffset);
        }
        int64_t srcLength = 0;
        if (funcArg[NARG_POS::TENTH] != nullptr) {
            tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 WebGL2RenderingContextOverloads::compressedTexSubImage2D srcLength = %{public}u", srcLength);
        }
        tie(succ, srcData, srcLength) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToDataview();
        if (!succ) {
            return nullptr;
        }
        char *srcDataBase = static_cast<char *>(srcData);
        srcData = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
        LOGI("WebGL2 compressedTexSubImage2D srcData = %{public}u", srcData);
    }
    glCompressedTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
                              static_cast<GLint>(xOffset), static_cast<GLint>(yOffset),
                              static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                              static_cast<GLenum>(format), static_cast<GLsizei>(imageSize),
                              reinterpret_cast<GLvoid *>(srcData));
    LOGI("WebGL2 compressedTexSubImage2D end");
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform1fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform1fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform1fv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform1fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform1fv length = %{public}u", length);
    }

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL glUniform1fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL glUniform1fv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    if (bufferData.GetBufferType() == buffer_array) {
        length = bufferData.GetBufferLength();
        glUniform1fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
            reinterpret_cast<GLfloat*>(bufferData.GetBuffer() + srcOffset));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniform1fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
            reinterpret_cast<GLfloat *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL glUniform1fv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform2fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 uniform2fv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform2fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 glUniform2fv length = %{public}u", length);
    }

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL glUniform2fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL glUniform2fv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    if (bufferData.GetBufferType() == buffer_array) {
        length = bufferData.GetBufferLength();
        glUniform2fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
            reinterpret_cast<GLfloat*>(bufferData.GetBuffer() + srcOffset));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniform2fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
            reinterpret_cast<GLfloat *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL glUniform2fv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform3fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGL2RenderingContextOverloads::uniform3fv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform3fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform3fv length = %{public}u", length);
    }

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL glUniform3fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL glUniform3fv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    if (bufferData.GetBufferType() == buffer_array) {
        length = bufferData.GetBufferLength();
        glUniform3fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
            reinterpret_cast<GLfloat*>(bufferData.GetBuffer() + srcOffset));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniform3fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
            reinterpret_cast<GLfloat *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL glUniform3fv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform4fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 uniform4fv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 glUniform4fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform4fv length = %{public}u", length);
    }

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL glUniform4fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL glUniform4fv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    if (bufferData.GetBufferType() == buffer_array) {
        length = bufferData.GetBufferLength();
        glUniform4fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
            reinterpret_cast<GLfloat*>(bufferData.GetBuffer() + srcOffset));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniform4fv(static_cast<GLint>(locationId), static_cast<GLsizei>(length),
            reinterpret_cast<GLfloat *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL glUniform4fv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform1iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform1iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 uniform1iv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform1iv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform1iv length = %{public}u", length);
    }
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_int32);
    if (status != 0) {
        LOGE("WebGL glUniform1iv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_int32) {
        LOGE("WebGL glUniform1iv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    // TODO
    if (bufferData.GetBufferType() == buffer_array) {
        glUniform1iv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
                     reinterpret_cast<GLint *>(bufferData.GetBuffer()));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniform1iv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
                     reinterpret_cast<GLint *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL glUniform1iv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform2iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform2iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 uniform2iv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform2iv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform2iv length = %{public}u", length);
    }

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_int32);
    if (status != 0) {
        LOGE("WebGL glUniform2iv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_int32) {
        LOGE("WebGL glUniform2iv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    // TODO
    if (bufferData.GetBufferType() == buffer_array) {
        glUniform2iv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
                     reinterpret_cast<GLint *>(bufferData.GetBuffer()));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniform2iv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
                     reinterpret_cast<GLint *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL glUniform2iv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform3iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform3iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 uniform3iv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform3iv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform3iv length = %{public}u", length);
    }

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_int32);
    if (status != 0) {
        LOGE("WebGL uniform4iv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_int32) {
        LOGE("WebGL uniform4iv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    // TODO
    if (bufferData.GetBufferType() == buffer_array) {
        glUniform3iv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
                     reinterpret_cast<GLint *>(bufferData.GetBuffer()));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniform3iv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
                     reinterpret_cast<GLint *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL uniform4iv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::Uniform4iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform4iv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 uniform4iv locationId = %{public}u", locationId);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform4iv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform4iv length = %{public}u", length);
    }
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::SECOND], buffer_data_int32);
    if (status != 0) {
        LOGE("WebGL uniform4iv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_int32) {
        LOGE("WebGL uniform4iv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    // TODO
    if (bufferData.GetBufferType() == buffer_array) {
        glUniform4iv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
                     reinterpret_cast<GLint *>(bufferData.GetBuffer()));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniform4iv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
                     reinterpret_cast<GLint *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL uniform4iv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::UniformMatrix2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniformMatrix2fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 uniformMatrix2fv locationId = %{public}u", locationId);
    bool transpose = false;
    tie(succ, transpose)  = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix2fv transpose = %{public}u", transpose);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix2fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix2fv length = %{public}u", length);
    }
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL uniformMatrix2fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL uniformMatrix2fv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    if (bufferData.GetBufferType() == buffer_array) {
        glUniformMatrix2fv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
            static_cast<GLboolean>(transpose),
            reinterpret_cast<GLfloat *>(bufferData.GetBuffer()));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniformMatrix2fv(static_cast<GLint>(locationId),
            static_cast<GLsizei>(length), static_cast<GLboolean>(transpose),
            reinterpret_cast<GLfloat *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL uniformMatrix2fv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::UniformMatrix3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniformMatrix3fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 uniformMatrix3fv locationId = %{public}u", locationId);
    bool transpose = false;
    tie(succ, transpose)  = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix3fv transpose = %{public}u", transpose);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix3fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix3fv length = %{public}u", length);
    }
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL uniformMatrix3fv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL uniformMatrix3fv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    if (bufferData.GetBufferType() == buffer_array) {
        glUniformMatrix3fv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
            static_cast<GLboolean>(transpose),
            reinterpret_cast<GLfloat *>(bufferData.GetBuffer()));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniformMatrix3fv(static_cast<GLint>(locationId),
            static_cast<GLsizei>(length), static_cast<GLboolean>(transpose),
            reinterpret_cast<GLfloat *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL uniformMatrix3fv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::UniformMatrix4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniformMatrix4fv start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 uniformMatrix4fv locationId = %{public}u", locationId);
    bool transpose = false;
    tie(succ, transpose)  = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix4fv transpose = %{public}u", transpose);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix4fv srcOffset = %{public}u", srcOffset);
    }
    uint32_t length = 0;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, length) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix4fv length = %{public}u", length);
    }
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL BufferData failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL uniformMatrix4fv not support data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    if (bufferData.GetBufferType() == buffer_array) {
        glUniformMatrix4fv(static_cast<GLint>(locationId), static_cast<GLsizei>(bufferData.GetBufferLength()),
            static_cast<GLboolean>(transpose),
            reinterpret_cast<GLfloat *>(bufferData.GetBuffer()));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        length = length != 0 ? length : bufferData.GetBufferLength();
        glUniformMatrix4fv(static_cast<GLint>(locationId),
            static_cast<GLsizei>(length), static_cast<GLboolean>(transpose),
            reinterpret_cast<GLfloat *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL uniformMatrix4fv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

static uint8_t *GetPixelsData(napi_env env, napi_value dataArg, napi_value offsetArg)
{
    bool succ = false;
    void *data = nullptr;
    bool usageSucc = NVal(env, dataArg).TypeIs(napi_number);
    if (usageSucc) {
        int64_t offset;
        tie(succ, offset) = NVal(env, dataArg).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 readPixels offset = %{public}u", offset);
        return reinterpret_cast<uint8_t *>((offset));
    } else {
        size_t size;
        tie(succ, data, size) = NVal(env, dataArg).ToDataview();
        if (!succ) {
            return nullptr;
        }
        int64_t dstOffset = 0;
        if (offsetArg != nullptr) {
            tie(succ, dstOffset) = NVal(env, offsetArg).ToInt64();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 readPixels dstOffset = %{public}u", dstOffset);
        }
        LOGI("WebGL2 readPixels data = %{public}u dstOffset %{public}u", data, dstOffset);
        return reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + dstOffset);
    }
    return nullptr;
}

napi_value WebGL2RenderingContextOverloads::ReadPixels(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN, NARG_CNT::EIGHT)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 readPixels start");
    int64_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 readPixels x = %{public}u", x);
    int64_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 readPixels y = %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 readPixels width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 readPixels height = %{public}u", height);
    int64_t format;
    tie(succ, format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 readPixels format = %{public}u", format);
    int64_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 readPixels type = %{public}u", type);
    GLvoid *data = GetPixelsData(env, funcArg[NARG_POS::SEVENTH], funcArg[NARG_POS::EIGHTH]);
    if (data == nullptr) {
        return nullptr;
    }
    glReadPixels(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height), static_cast<GLenum>(format),
                 static_cast<GLenum>(type), reinterpret_cast<GLvoid *>(data));
    LOGI("WebGL2 readPixels end");
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif