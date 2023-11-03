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
    do { \
        LOGE("WebGL set error code %{public}u", error); \
        SetError(error); \
    } while (0)

#define SET_ERROR_WITH_LOG(error, info, ...) \
    do {                      \
        LOGE("WebGL error code %{public}u " info, error, ##__VA_ARGS__); \
        SetError(error); \
    } while (0)

void WebGLRenderingContextBaseImpl::TexImage2D_(
    const TexImageArg& imgArg, WebGLTexture* texture, const void* pixels, bool changeUnpackAlignment)
{
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    LOGD("WebGL TexImage2D_ target %{public}u result %{public}u", imgArg.target, GetError_());
    glTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.border,
        imgArg.format, imgArg.type, pixels);
    texture->SetTextureLevel(imgArg);

    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment_);
    }
    LOGD("WebGL TexImage2D_ target %{public}u result %{public}u", imgArg.target, GetError_());
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(
    napi_env env, const TexImageArg& imgArg, napi_value pixels, GLuint srcOffset)
{
    imgArg.Dump("WebGL texImage2D");
    LOGD("WebGL texImage2D srcOffset %{public}u", srcOffset);
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(error);
        return nullptr;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    bool changeUnpackAlignment = false;
    if (!NVal(env, pixels).IsNull()) {
        error =imageSource.GenImageSource(
            { imgArg.format, imgArg.type, imgArg.width, imgArg.height }, pixels, srcOffset);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D invalid pixels");
            return nullptr;
        }
        changeUnpackAlignment = unpackFlipY_ || unpackPremultiplyAlpha_;
        data = imageSource.GetImageSourceData();
    }
    TexImage2D_(imgArg, texture, data, changeUnpackAlignment);
    LOGD("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const TexImageArg& info, napi_value source)
{
    TexImageArg imgArg(info);
    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    if (!NVal(env, source).IsNull()) {
        GLenum error = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, source);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texImage2D Image source invalid");
            return nullptr;
        }
        data = imageSource.GetImageSourceData();
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }
    imgArg.Dump("WebGL texImage2D");

    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "Can not find texture");
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texImage2D checkTexImage failed");
        return nullptr;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "WebGL texImage2D checkImmutable failed");
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL texImage2D checkNPOT failed");
        return nullptr;
    }
    if (imgArg.type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
        // The UNSIGNED_INT_10F_11F_11F_REV type pack/unpack isn't implemented.
        imgArg.type = GL_FLOAT;
    }

    TexImage2D_(imgArg, texture, data, unpackAlignment_ != 1);
    LOGD("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const TexImageArg& imgArg, GLintptr pbOffset)
{
    imgArg.Dump("WebGL texImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "Can not find texture");
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texImage2D error");
        return nullptr;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.border,
        imgArg.format, imgArg.type, reinterpret_cast<GLvoid*>(pbOffset));
    LOGD("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

void WebGLRenderingContextBaseImpl::TexSubImage2D_(
    const TexSubImage2DArg& imgArg, WebGLTexture* texture, const void* pixels, bool changeUnpackAlignment)
{
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    glTexSubImage2D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.width, imgArg.height,
        imgArg.format, imgArg.type, pixels);
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment_);
    }
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(napi_env env, const TexSubImage2DArg& imgArg, GLintptr pbOffset)
{
    imgArg.Dump("WebGL texSubImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(error);
        return nullptr;
    }
    glTexSubImage2D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.width, imgArg.height,
        imgArg.format, imgArg.type, reinterpret_cast<void *>(pbOffset));
    LOGD("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(
    napi_env env, const TexSubImage2DArg& info, napi_value pixels, GLuint srcOffset)
{
    TexSubImage2DArg& imgArg = const_cast<TexSubImage2DArg&>(info);
    imgArg.Dump("WebGL texSubImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D error");
        return nullptr;
    }

    if (!texture->CheckValid(imgArg.target, imgArg.level)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, " invalid texture level");
        return nullptr;
    }

    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    bool changeUnpackAlignment = false;
    if (!NVal(env, pixels).IsNull()) {
        error = imageSource.GenImageSource(
            { imgArg.format, imgArg.type, imgArg.width, imgArg.height }, pixels, srcOffset);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D invalid pixels");
            return nullptr;
        }
        changeUnpackAlignment = unpackFlipY_ || unpackPremultiplyAlpha_;
        data = imageSource.GetImageSourceData();
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }

    error = CheckTexSubImage2D(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D error");
        return nullptr;
    }

    if (imgArg.type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
        // The UNSIGNED_INT_10F_11F_11F_REV type pack/unpack isn't implemented.
        imgArg.type = GL_FLOAT;
    }
    TexSubImage2D_(imgArg, texture, data, changeUnpackAlignment);
    LOGD("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(
    napi_env env, const TexSubImage2DArg& info, napi_value imageData)
{
    TexSubImage2DArg imgArg(info);
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "Can not find texture");
        return nullptr;
    }
    GLvoid* data = nullptr;
    GLenum error = 0;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    if (!NVal(env, imageData).IsNull()) {
        error = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, imageData);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D Image source invalid");
            return nullptr;
        }
        data = imageSource.GetImageSourceData();
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }
    imgArg.Dump("WebGL texSubImage2D");
    error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D CheckTexImage failed");
        return nullptr;
    }
    error = CheckTexSubImage2D(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(error, "WebGL texSubImage2D CheckTexSubImage2D failed");
        return nullptr;
    }
    TexSubImage2D_(imgArg, texture, data, unpackAlignment_);
    LOGD("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DrawElements(
    napi_env env, GLenum mode, GLsizei count, GLenum type, GLintptr offset)
{
    LOGD("WebGL drawElements mode %{public}u %{public}u %{public}u %{public}u", mode, count, type, offset);
    GLenum result = CheckDrawElements(env, mode, count, type, static_cast<int64_t>(offset));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR_WITH_LOG(result, "WebGL drawElements failed");
        return nullptr;
    }
    glDrawElements(mode, count, type, reinterpret_cast<GLvoid*>(offset));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DrawArrays(napi_env env, GLenum mode, GLint first, GLsizei count)
{
    LOGD("WebGL drawArrays mode %{public}u %{public}u %{public}u error %{public}u", mode, first, count, GetError_());
    GLenum result = CheckDrawArrays(env, mode, first, count);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }
    glDrawArrays(mode, first, count);
    LOGD("WebGL drawArrays result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ReadPixels(napi_env env, const PixelsArg& arg, GLintptr offset)
{
    arg.Dump("WebGL readPixels");
    if (!IsHighWebGL()) {
        return nullptr;
    }

    WebGLBuffer* buffer = GetBoundBuffer(env, WebGL2RenderingContextBase::PIXEL_PACK_BUFFER);
    if (buffer == nullptr || buffer->GetBufferSize() == 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    uint64_t size = static_cast<uint64_t>(buffer->GetBufferSize());
    if (size < offset) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    size = size - offset;

    GLenum result = CheckReadPixelsArg(env, arg, size);
    if (!result) {
        SET_ERROR(result);
        return nullptr;
    }
    glReadPixels(arg.x, arg.y, arg.width, arg.height, arg.format, arg.type, reinterpret_cast<void*>(offset));
    LOGD("WebGL readPixels result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ReadPixels(
    napi_env env, const PixelsArg& arg, napi_value buffer, GLuint dstOffset)
{
    arg.Dump("WebGL readPixels");
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(buffer);
    if (status != 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLenum result = CheckReadPixelsArg(env, arg, static_cast<int64_t>(bufferData.GetBufferLength()));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }

    LOGD("WebGL readPixels pixels %{public}u", dstOffset);
    glReadPixels(arg.x, arg.y, arg.width, arg.height, arg.format, arg.type,
        static_cast<void*>(bufferData.GetBuffer() + dstOffset));
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    LOGD("WebGL readPixels result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData_(
    napi_env env, GLenum target, GLsizeiptr size, GLenum usage, const uint8_t* bufferData)
{
    LOGD("WebGL bufferData target %{public}u, usage %{public}u size %{public}u", target, usage, size);
    uint32_t index = 0;
    if (!CheckBufferTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLBuffer* webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[index]);
    if (webGLBuffer == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!CheckBufferDataUsage(env, usage)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (webGLBuffer->GetTarget() != target) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    webGLBuffer->SetBuffer(size, nullptr);
    if (bufferData != nullptr) {
        webGLBuffer->SetBuffer(size, bufferData);
        glBufferData(target, size, bufferData, usage);
    } else {
        glBufferData(target, size, nullptr, usage);
    }
    webGLBuffer->SetTarget(target);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData(napi_env env, GLenum target, int64_t size, GLenum usage)
{
    LOGD("WebGL bufferData target %{public}u, usage %{public}u size %{public}u", target, usage, size);
    BufferData_(env, target, static_cast<GLsizeiptr>(size), usage, nullptr);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData(
    napi_env env, GLenum target, napi_value data, GLenum usage, const BufferExt& ext)
{
    LOGD("WebGL bufferData target %{public}u, usage %{public}u ext: %{public}u %{public}u",
        target, usage, ext.offset, ext.length);
    WebGLReadBufferArg bufferData(env);
    if (NVal(env, data).IsNull()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bool succ = bufferData.GenBufferData(data, BUFFER_DATA_FLOAT_32) == napi_ok;
    if (!succ || bufferData.GetBufferType() == BUFFER_ARRAY) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGD("WebGL bufferData buffer size %{public}zu", bufferData.GetBufferLength());
    // change
    GLuint srcOffset = static_cast<GLuint>(ext.offset * bufferData.GetBufferDataSize());
    GLsizeiptr length = (ext.length == 0) ? static_cast<GLsizeiptr>(bufferData.GetBufferLength())
                        : static_cast<GLsizeiptr>(ext.length * bufferData.GetBufferDataSize());
    BufferData_(env, target, length, usage, bufferData.GetBuffer() + srcOffset);
    LOGD("WebGL bufferData target %{public}u, result %{public}u ", target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferSubData(
    napi_env env, GLenum target, GLintptr offset, napi_value buffer, const BufferExt& ext)
{
    LOGD("WebGL bufferSubData target %{public}u offset %{public}u %{public}u %{public}u",
        target, offset, ext.offset, ext.length);

    WebGLBuffer* webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::ARRAY_BUFFER]);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferSize() == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "bufferSubData Can not find bound buffer");
        return nullptr;
    }

    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(buffer);
    if (status != 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL bufferSubData invalid buffer data");
        return nullptr;
    }
    // check sub buffer
    if ((static_cast<size_t>(offset) + bufferData.GetBufferLength()) > webGLBuffer->GetBufferSize()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL bufferSubData invalid buffer size %{public}zu offset %{public}zu ",
            bufferData.GetBufferLength(), webGLBuffer->GetBufferSize());
        return nullptr;
    }

    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    LOGD("WebGL bufferSubData size %{public}zu", bufferData.GetBufferLength());

    glBufferSubData(target, offset, static_cast<GLsizeiptr>(bufferData.GetBufferLength()),
        static_cast<uint8_t*>(bufferData.GetBuffer()));
    LOGD("WebGL bufferSubData target %{public}u, result %{public}u ", target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::PixelStorei(napi_env env, GLenum pname, GLint param)
{
    LOGD("WebGL pixelStorei pname %{public}u param %{public}u", pname, param);
    switch (pname) {
        case WebGLRenderingContextBase::UNPACK_FLIP_Y_WEBGL:
            unpackFlipY_ = (param == 1);
            return nullptr;
        case WebGLRenderingContextBase::UNPACK_PREMULTIPLY_ALPHA_WEBGL:
            unpackPremultiplyAlpha_ = (param == 1);
            return nullptr;
        case WebGLRenderingContextBase::PACK_ALIGNMENT:
        case WebGLRenderingContextBase::UNPACK_ALIGNMENT:
            if (param == 1 || param == 2 || param == 4 || param == 8) { // 2,4,8 ALIGNMENT
                if (pname == WebGLRenderingContextBase::PACK_ALIGNMENT) {
                    packAlignment_ = param;
                    webGLRenderingContext_->SetPackAlignment(param);
                } else {
                    unpackAlignment_ = param;
                }
            } else {
                SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
                return nullptr;
            }
            break;
        case WebGLRenderingContextBase::UNPACK_COLORSPACE_CONVERSION_WEBGL:
            if (static_cast<GLenum>(param) == WebGLRenderingContextBase::BROWSER_DEFAULT_WEBGL || param == GL_NONE) {
                unpackColorspaceConversion_ = static_cast<GLenum>(param);
            } else {
                SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
                    "WebGL pixelStorei invalid parameter for UNPACK_COLORSPACE_CONVERSION_WEBGL %{public}u", pname);
                return nullptr;
            }
            break;
        default:
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM,
                "WebGL pixelStorei invalid pname  %{public}u", pname);
            return nullptr;
    }
    glPixelStorei(pname, param);
    LOGD("WebGL pixelStorei pname %{public}u, result %{public}u ", pname, GetError_());
    return nullptr;
}

GLenum WebGLRenderingContextBaseImpl::CheckCompressedTexImage2D(
    napi_env env, const TexImageArg& imgArg, size_t imageSize)
{
    GLenum result = CheckTextureLevel(imgArg.target, imgArg.level);
    if (result) {
        LOGE("Invalid target or level target %{public}u %{public}d", imgArg.target, imgArg.level);
        return result;
    }
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        LOGE("Invalid texture target %{public}u ", imgArg.target);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    if (!CheckTexImageInternalFormat(env, imgArg.func, imgArg.internalFormat)) {
        LOGE("Invalid internalFormat target %{public}u %{public}u", imgArg.target, imgArg.internalFormat);
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (imgArg.border) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    result = CheckCompressedTexDimensions(imgArg);
    if (result) {
        LOGE("Invalid internalFormat %{public}u ", imgArg.internalFormat);
        return result;
    }
    if (texture->CheckImmutable()) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    result = CheckCompressedTexData(imgArg, imageSize);
    if (result) {
        LOGE("Invalid tex data %{public}u ", result);
        return result;
    }

    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexImage2D(
    napi_env env, const TexImageArg& imgArg, GLsizei imageSize, GLintptr offset)
{
    imgArg.Dump("WebGL compressedTexImage2D");
    GLenum result = CheckCompressedTexImage2D(env, imgArg, static_cast<size_t>(imageSize));
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    glCompressedTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height,
        imgArg.border, imageSize, reinterpret_cast<void*>(offset));
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture != nullptr) {
        texture->SetTextureLevel(
            { imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, 1, GL_UNSIGNED_BYTE });
    }
    LOGD("WebGL compressedTexImage2D %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexImage2D(
    napi_env env, const TexImageArg& info, napi_value srcData, GLuint srcOffset, GLuint srcLengthOverride)
{
    TexImageArg& imgArg = const_cast<TexImageArg&>(info);
    imgArg.Dump("WebGL compressedTexImage2D");

    WebGLReadBufferArg bufferData(env);
    bool succ = bufferData.GenBufferData(srcData, BUFFER_DATA_FLOAT_32) == napi_ok;
    if (!succ) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());

    GLvoid* data = reinterpret_cast<GLvoid*>(bufferData.GetBuffer());
    size_t length = bufferData.GetBufferLength();
    if (srcOffset != 0) {
        data = reinterpret_cast<GLvoid*>(bufferData.GetBuffer() + srcOffset * bufferData.GetBufferDataSize());
    }
    if (srcLengthOverride != 0) {
        length = srcLengthOverride;
    }

    GLenum result = CheckCompressedTexImage2D(env, imgArg, length);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    glCompressedTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height,
        imgArg.border, static_cast<GLsizei>(length), data);

    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture != nullptr) {
        texture->SetTextureLevel(
            { imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, 1, GL_UNSIGNED_BYTE });
    }
    LOGD("WebGL compressedTexImage2D %{public}u", GetError_());
    return nullptr;
}

bool WebGLRenderingContextBaseImpl::CheckCompressedTexSubImage2D(
    napi_env env, const TexSubImage2DArg& imgArg, size_t imageSize)
{
    GLenum result = CheckTextureLevel(imgArg.target, imgArg.level);
    if (result) {
        SET_ERROR(result);
        return result;
    }
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return false;
    }

    if (!CheckTexImageInternalFormat(env, imgArg.func, imgArg.format)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return false;
    }
    if (imgArg.border) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return false;
    }
    result = CheckCompressedTexData(imgArg, imageSize);
    if (result) {
        SET_ERROR(result);
        return false;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return false;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return false;
    }
    result = CheckCompressedTexSubDimensions(imgArg, texture);
    if (result) {
        SET_ERROR(result);
        return false;
    }
    return true;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexSubImage2D(
    napi_env env, const TexSubImage2DArg& imgArg, napi_value srcData, GLuint srcOffset, GLuint srcLengthOverride)
{
    imgArg.Dump("WebGL compressedTexSubImage2D");
    WebGLReadBufferArg bufferData(env);
    GLvoid* data = nullptr;
    GLsizei length = 0;
    if (!NVal(env, srcData).IsNull()) {
        bool succ = bufferData.GenBufferData(srcData, BUFFER_DATA_FLOAT_32) == napi_ok;
        if (!succ) {
            return nullptr;
        }
        bufferData.DumpBuffer(bufferData.GetBufferDataType());
        data = reinterpret_cast<void*>(bufferData.GetBuffer() + srcOffset * bufferData.GetBufferDataSize());
        length = srcLengthOverride == 0 ? static_cast<GLsizei>(bufferData.GetBufferLength()) : srcLengthOverride;
    }
    bool succ = CheckCompressedTexSubImage2D(env, imgArg, length);
    if (!succ) {
        return nullptr;
    }
    glCompressedTexSubImage2D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.width, imgArg.height,
        imgArg.format, length, data);
    LOGD("WebGL compressedTexSubImage2D result: %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexSubImage2D(
    napi_env env, const TexSubImage2DArg& imgArg, GLsizei imageSize, GLintptr offset)
{
    imgArg.Dump("WebGL compressedTexSubImage2D");
    bool succ = CheckCompressedTexSubImage2D(env, imgArg, imageSize);
    if (!succ) {
        return nullptr;
    }
    glCompressedTexSubImage2D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.width, imgArg.height,
        imgArg.format, imageSize, reinterpret_cast<void*>(offset));
    LOGD("WebGL compressedTexSubImage2D result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CopyTexImage2D(napi_env env, const CopyTexImage2DArg& imgArg)
{
    imgArg.Dump("WebGL copyTexImage2D");
    GLenum result = CheckTextureLevel(imgArg.target, imgArg.level);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture || texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    result = CheckTextureFormatAndType(env,
        imgArg.internalFormat, imgArg.internalFormat, GL_UNSIGNED_BYTE, imgArg.level);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    if (imgArg.border) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    result = CheckTexFuncDimensions(imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLuint frameBufferId = 0;
    result = CheckReadBufferAndGetInfo(env, &frameBufferId, nullptr, nullptr);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    glCopyTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.x, imgArg.y, imgArg.width,
        imgArg.height, imgArg.border);

    texture->SetTextureLevel({ imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, 1, 1 });
   // LOGD("WebGL copyTexImage2D result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CopyTexSubImage2D(napi_env env, const CopyTexSubImageArg& imgArg)
{
    imgArg.Dump("WebGL copyTexSubImage2D");
    GLenum result = CheckCopyTexSubImage(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    GLuint frameBufferId = 0;
    result = CheckReadBufferAndGetInfo(env, &frameBufferId, nullptr, nullptr);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    glCopyTexSubImage2D(
        imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.x, imgArg.y, imgArg.width, imgArg.height);
   // LOGD("WebGL copyTexSubImage2D result %{public}u", GetError_());
    return nullptr;
}

GLenum WebGLRenderingContextBaseImpl::CheckTextureLevel(GLenum target, GLint level)
{
    GLint max = WebGLTexture::GetMaxTextureLevelForTarget(target, IsHighWebGL());
    if (max <= 0) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if ((level < 0) || level > max) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckTexImage(napi_env env, const TexImageArg& imgArg, WebGLTexture* texture)
{
    GLenum result = CheckTextureLevel(imgArg.target, imgArg.level);
    if (result) {
        return result;
    }
    if (imgArg.border) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    GLenum internalFormat = imgArg.internalFormat;
    if (imgArg.internalFormat == 0) {
        internalFormat = texture->GetInternalFormat(imgArg.target, imgArg.level);
    }
    if (internalFormat == 0) {
        LOGE("Invalid internalFormat %{public}u", internalFormat);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    result = CheckTextureFormatAndType(env, internalFormat, imgArg.format, imgArg.type, imgArg.level);
    if (result) {
        return result;
    }
    result = CheckTexFuncDimensions(imgArg);
    if (result) {
        LOGE("Invalid texture dimension or type %{public}u", result);
        return result;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckTexSubImage2D(
    napi_env env, const TexSubImage2DArg& imgArg, WebGLTexture* texture)
{
    if (imgArg.xOffset < 0 || imgArg.yOffset < 0) {
        LOGE("WebGL CheckTexSubImage2D invalid xOffset %{public}d", imgArg.xOffset);
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    // Before checking if it is in the range, check if overflow happens first.
    if (imgArg.xOffset + imgArg.width < 0 || imgArg.yOffset + imgArg.height < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if ((imgArg.xOffset + imgArg.width) > texture->GetWidth(imgArg.target, imgArg.level) ||
        (imgArg.yOffset + imgArg.height) > texture->GetHeight(imgArg.target, imgArg.level)) {
        LOGE("WebGL invalid CheckTexSubImage2D GetWidth %{public}d, GetHeight %{public}d",
            texture->GetWidth(imgArg.target, imgArg.level), texture->GetHeight(imgArg.target, imgArg.level));
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (!IsHighWebGL() && texture->GetType(imgArg.target, imgArg.level) != imgArg.type) {
        LOGE("WebGL invalid CheckTexSubImage2D type %{public}d", imgArg.type);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGLRenderingContextBaseImpl::CheckTexImageInternalFormat(napi_env env, int func, GLenum internalFormat)
{
    return CheckInList(internalFormat, WebGLRenderingContextBaseImpl::GetTexImageInternalFormat());
}

GLenum WebGLRenderingContextBaseImpl::CheckTexFuncDimensions(const TexImageArg& imgArg)
{
    if (imgArg.width < 0 || imgArg.height < 0) {
        LOGE("Invalid offset or size ");
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    switch (imgArg.target) {
        case GL_TEXTURE_2D:
            if (imgArg.width > (maxTextureSize_ >> imgArg.level) || imgArg.height > (maxTextureSize_ >> imgArg.level)) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            if (imgArg.func != IMAGE_TEX_SUB_IMAGE_2D && imgArg.width != imgArg.height) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            if (imgArg.width > (maxCubeMapTextureSize_ >> imgArg.level)) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            break;
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckCompressedTexDimensions(const TexImageArg& imgArg)
{
    GLenum result = CheckTexFuncDimensions(imgArg);
    if (result) {
        LOGE("Invalid tex dimensions %{public}u %{public}d", imgArg.target, imgArg.level);
        return result;
    }
    bool widthValid = true;
    bool heightValid = true;
    if (CheckInList(imgArg.internalFormat, GetExtentionAstcTexImageInternal())) {
        return WebGLRenderingContextBase::NO_ERROR;
    }
    switch (imgArg.internalFormat) {
#ifdef GC3D_COMPRESSED_ATC_RGB_AMD
        case GC3D_COMPRESSED_ATC_RGB_AMD:
        case GC3D_COMPRESSED_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        case GC3D_COMPRESSED_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
#endif
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: {
            widthValid = (imgArg.level && imgArg.width == 1) || (imgArg.level && imgArg.width == 2) ||
                !(imgArg.width % 4); // 1 2 4 ALIGNMENT
            heightValid = (imgArg.level && imgArg.height == 1) || (imgArg.level && imgArg.height == 2) ||
                !(imgArg.height % 4); // 1 2 4 ALIGNMENT
            break;
        }
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: {
            widthValid = (imgArg.width & (imgArg.width - 1)) == 0;
            heightValid = (imgArg.height & (imgArg.height - 1)) == 0;
            break;
        }
        default:
            return WebGLRenderingContextBase::NO_ERROR;
    }

    if (!widthValid || !heightValid) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckCompressedTexData(const TexImageArg& imgArg, size_t dataLen)
{
    size_t bytesRequired = 0;
    switch (imgArg.internalFormat) {
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: {
            const int kBlockWidth = 4;
            const int kBlockHeight = 4;
            const int kBlockSize = 8;
            int numBlocksAcross = (imgArg.width + kBlockWidth - 1) / kBlockWidth;
            int numBlocksDown = (imgArg.height + kBlockHeight - 1) / kBlockHeight;
            int numBlocks = numBlocksAcross * numBlocksDown;
            bytesRequired = numBlocks * kBlockSize;
            break;
        }
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: {
            const int kBlockWidth = 4;
            const int kBlockHeight = 4;
            const int kBlockSize = 16;
            int numBlocksAcross = (imgArg.width + kBlockWidth - 1) / kBlockWidth;
            int numBlocksDown = (imgArg.height + kBlockHeight - 1) / kBlockHeight;
            int numBlocks = numBlocksAcross * numBlocksDown;
            bytesRequired = numBlocks * kBlockSize;
            break;
        }
        case GL_ETC1_RGB8_OES: { // 3 kBlockWidth -1, 4 kBlockWidth, 8 kBlockSize
            bytesRequired = floor(static_cast<double>((imgArg.width + 3) / 4)) *
                floor(static_cast<double>((imgArg.height + 3) / 4)) * 8;
            break;
        }
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG: { // 4 4BPPV1, 7 kBlockWidth -1, 8 kBlockWidth, 8 kBlockSize
            bytesRequired = (max(imgArg.width, 8) * max(imgArg.height, 8) * 4 + 7) / 8;
            break;
        }
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: { // 2 2BPPV1, 7 kBlockWidth - 1, 16 kBlockWidth, 8 kBlockSize
            bytesRequired = (max(imgArg.width, 16) * max(imgArg.height, 8) * 2 + 7) / 8;
            break;
        }
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    LOGD("CheckCompressedTexData bytesRequired %{public}zu", bytesRequired);
    if (dataLen != bytesRequired) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGLRenderingContextBaseImpl::CheckCompressedTexSubDimensions(
    const TexSubImage2DArg& imgArg, WebGLTexture* texture)
{
    if (imgArg.xOffset < 0 || imgArg.yOffset < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    switch (imgArg.format) {
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: {
            const int kBlockWidth = 4;
            const int kBlockHeight = 4;
            if ((imgArg.xOffset % kBlockWidth) || (imgArg.yOffset % kBlockHeight)) {
                return WebGLRenderingContextBase::INVALID_OPERATION;
            }
            if (WebGLArg::CheckOverflow<GLint, GLint>(imgArg.xOffset, imgArg.width) ||
                WebGLArg::CheckOverflow<GLint, GLint>(imgArg.yOffset, imgArg.height)) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }

            if ((imgArg.xOffset + imgArg.width) > texture->GetWidth(imgArg.target, imgArg.level) ||
                (imgArg.yOffset + imgArg.height) > texture->GetHeight(imgArg.target, imgArg.level)) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            return CheckCompressedTexDimensions(imgArg);
        }
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: {
            if ((imgArg.xOffset != 0) || (imgArg.yOffset != 0)) {
                return WebGLRenderingContextBase::INVALID_OPERATION;
            }
            if (imgArg.width != texture->GetWidth(imgArg.target, imgArg.level) ||
                imgArg.height != texture->GetHeight(imgArg.target, imgArg.level)) {
                return WebGLRenderingContextBase::INVALID_OPERATION;
            }
            return CheckCompressedTexDimensions(imgArg);
        }
        case GL_ETC1_RGB8_OES: {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckTextureDataBuffer(
    const TexImageArg& arg, const WebGLReadBufferArg* bufferData)
{
    if (!bufferData) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (WebGLTexture::ChangeToBufferDataType(arg.type) != bufferData->GetBufferDataType()) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckCopyTexSubImage(napi_env env, const CopyTexSubImageArg& arg)
{
    GLenum result = CheckTextureLevel(arg.target, arg.level);
    if (result) {
        return result;
    }

    WebGLTexture* texture = GetBoundTexture(env, arg.target, false);
    if (texture == nullptr) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    if (!WebGLTexture::CheckTextureSize(arg.xOffset, arg.width, texture->GetWidth(arg.target, arg.level)) ||
        !WebGLTexture::CheckTextureSize(arg.yOffset, arg.height, texture->GetHeight(arg.target, arg.level))) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (arg.func == IMAGE_COPY_TEX_SUB_IMAGE_3D) {
        const CopyTexSubImage3DArg* img3D = reinterpret_cast<const CopyTexSubImage3DArg*>(&arg);
        if (!WebGLTexture::CheckTextureSize(img3D->zOffset, img3D->depth, texture->GetDepth(arg.target, arg.level))) {
            return WebGLRenderingContextBase::INVALID_VALUE;
        }
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckDrawArrays(napi_env env, GLenum mode, GLint first, GLsizei count)
{
    if (!CheckDrawMode(env, mode)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (!CheckStencil(env)) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    if (first < 0 || count < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (!currentProgramId_) {
        LOGE("WebGL drawArrays no valid shader program in use");
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    return CheckFrameBufferBoundComplete(env);
}

GLenum WebGLRenderingContextBaseImpl::CheckDrawElements(
    napi_env env, GLenum mode, GLsizei count, GLenum type, int64_t offset)
{
    if (!CheckDrawMode(env, mode)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (!CheckStencil(env)) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    uint32_t size = 1;
    switch (type) {
        case WebGLRenderingContextBase::UNSIGNED_BYTE:
            break;
        case WebGLRenderingContextBase::UNSIGNED_SHORT:
            size = sizeof(short);
            break;
        case WebGLRenderingContextBase::UNSIGNED_INT: {
            size = sizeof(int);
            if (IsHighWebGL()) {
                break;
            }
            [[fallthrough]];
        }
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (count < 0 || offset < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if ((offset % size) != 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    WebGLBuffer* webGLBuffer =
        GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::ELEMENT_ARRAY_BUFFER]);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferSize() == 0) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    // check count
    if (size * count > webGLBuffer->GetBufferSize()) {
        LOGE("WebGL drawElements Insufficient buffer size %{public}u", count);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    if (static_cast<size_t>(offset) >= webGLBuffer->GetBufferSize()) {
        LOGE("WebGL drawElements invalid offset %{public}u", offset);
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (!currentProgramId_) {
        LOGE("WebGL drawArrays no valid shader program in use");
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    return CheckFrameBufferBoundComplete(env);
}

GLenum WebGLRenderingContextBaseImpl::CheckReadBufferAndGetInfo(
    napi_env env, GLuint* frameBufferId, GLenum* format, GLenum* type)
{
    GLenum target = IsHighWebGL() ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER;
    WebGLFramebuffer* frameBuffer = GetBoundFrameBuffer(env, target);
    if (frameBuffer) {
        LOGD("CheckReadBufferAndGetInfo frameBuffer %{public}u", frameBuffer->GetFramebuffer());
        if (frameBuffer->CheckStatus(env, this) != WebGLRenderingContextBase::FRAMEBUFFER_COMPLETE) {
            LOGE("CheckStatus not FRAMEBUFFER_COMPLETE");
            return WebGLRenderingContextBase::INVALID_FRAMEBUFFER_OPERATION;
        }
        if (!GetReadBufferFormatAndType(env, frameBuffer, format, type)) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        *frameBufferId = frameBuffer->GetFramebuffer();
    } else {
        if (defaultReadBufferMode_ == GL_NONE) {
            LOGE("defaultReadBufferMode_ %{public}u", defaultReadBufferMode_);
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        if (format) {
            *format = GL_RGBA;
        }
        if (type) {
            *type = GL_UNSIGNED_BYTE;
        }
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGLRenderingContextBaseImpl::GetReadBufferFormatAndType(
    napi_env env, const WebGLFramebuffer* frameBuffer, GLenum* format, GLenum* type)
{
    GLenum mode = frameBuffer->GetReadBufferMode();
    LOGD("GetReadBufferFormatAndType mode %{public}u", mode);
    if (mode == GL_NONE) {
        return false;
    }
    WebGLAttachment* attachedObject = frameBuffer->GetAttachment(mode);
    if (!attachedObject) {
        LOGE("GetReadBufferFormatAndType no attachment %{public}u", mode);
        return false;
    }
    WebGLAttachmentInfo info = {};
    if (!frameBuffer->GetWebGLAttachmentInfo(env, this, attachedObject, info)) {
        LOGE("GetReadBufferFormatAndType no attachment info %{public}u", mode);
        return false;
    }
    if (format) {
        *format = info.format;
    }
    if (type) {
        *type = info.type;
    }
    return true;
}

GLenum WebGLRenderingContextBaseImpl::CheckTextureFormatAndType(
    napi_env env, GLenum internalFormat, GLenum format, GLenum type, GLint level)
{
    LOGD("internalFormat %{public}u format %{public}u type %{public}u  %{public}d",
        internalFormat, format, type, level);
    if (!CheckInList(internalFormat, WebGLTexture::GetSupportedInternalFormats())) {
        LOGE("Invalid internalFormat %{public}u ", internalFormat);
        return GL_INVALID_ENUM;
    }
    if (!CheckInList(format, WebGLTexture::GetSupportedFormats())) {
        LOGE("Invalid format %{public}u ", format);
        return GL_INVALID_ENUM;
    }
    if (!CheckInList(type, WebGLTexture::GetSupportedTypes())) {
        LOGE("Invalid type %{public}u ", type);
        return GL_INVALID_ENUM;
    }

    TextureFormatTypeMap map = { internalFormat, format, type };
    if (WebGLTexture::GetSupportedFormatTypeMaps().find(map) == WebGLTexture::GetSupportedFormatTypeMaps().end()) {
        LOGE("Invalid format type ");
        return GL_INVALID_OPERATION;
    }

    if (format == GL_DEPTH_COMPONENT && level > 0 && !IsHighWebGL()) {
        return GL_INVALID_OPERATION;
    }
    if (format == GL_DEPTH_STENCIL_OES && level > 0 && !IsHighWebGL()) {
        return GL_INVALID_OPERATION;
    }
    return 0;
}
} // namespace Impl
} // namespace Rosen
} // namespace OHOS
