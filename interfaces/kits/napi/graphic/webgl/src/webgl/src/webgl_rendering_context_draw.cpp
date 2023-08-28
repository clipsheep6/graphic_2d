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

void WebGLRenderingContextBaseImpl::TexImage2D_(
    const TexImageArg& imgArg, WebGLTexture* texture, const void* pixels, bool changeUnpackAlignment)
{
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    GLenum internalFormat = GetValidInternalFormat(imgArg.internalFormat, imgArg.type);
    glTexImage2D(imgArg.target, imgArg.level, internalFormat, imgArg.width, imgArg.height, imgArg.border, imgArg.format,
        imgArg.type, pixels);
    texture->SetTextureLevel(imgArg);

    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment_);
    }
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(
    napi_env env, const TexImageArg& imgArg, napi_value pixels, GLuint srcOffset)
{
    imgArg.Dump("WebGL texImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        LOGE("Can not find texture ");
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texImage2D error %{public}u", error);
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
    WebGLImageSource imageSource(env, version_);
    bool changeUnpackAlignment = false;
    if (!NVal(env, pixels).IsNull()) {
        error = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, pixels, srcOffset);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D invalid pixels");
            return nullptr;
        }
        changeUnpackAlignment = unpackFlipY_ || unpackPremultiplyAlpha_;
        data = imageSource.GetImageSourceData(unpackFlipY_, unpackPremultiplyAlpha_);
    }
    TexImage2D_(imgArg, texture, data, changeUnpackAlignment);
    LOGE("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const TexImageArg& info, napi_value source)
{
    TexImageArg imgArg(info);
    imgArg.Dump("WebGL texImage2D");
    if (NVal(env, source).IsNull()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        LOGE("Can not find texture ");
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texImage2D error %{public}u", error);
        SET_ERROR(error);
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (!CheckSettableTexFormat(imgArg.format)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (imgArg.type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
        // The UNSIGNED_INT_10F_11F_11F_REV type pack/unpack isn't implemented.
        imgArg.type = GL_FLOAT;
    }
    WebGLImageSource imageSource(env, version_);
    error = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, source);
    if (error) {
        SET_ERROR_WITH_LOG(error, "texImage2D Image source invalid");
        return nullptr;
    }

    GLvoid* data = imageSource.GetImageSourceData(unpackFlipY_, unpackPremultiplyAlpha_);
    imgArg.width = imageSource.GetWidth();
    imgArg.height = imageSource.GetHeight();
    TexImage2D_(imgArg, texture, data, unpackAlignment_ != 1);
    LOGE("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const TexImageArg& imgArg, GLintptr pbOffset)
{
    imgArg.Dump("WebGL texImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        LOGE("Can not find texture ");
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texImage2D error %{public}u", error);
        SET_ERROR(error);
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (!CheckSettableTexFormat(imgArg.format)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.border,
        imgArg.format, imgArg.type, reinterpret_cast<GLvoid*>(pbOffset));
    LOGE("WebGL texImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
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
    texture->SetTextureLevel(imgArg);

    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment_);
    }
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(napi_env env, const TexSubImage2DArg& imgArg, GLintptr pbOffset)
{
    // TODO for WebGL 2
    imgArg.Dump("WebGL texSubImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        LOGE("Can not find texture ");
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SET_ERROR(error);
    }
    LOGE("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(
    napi_env env, const TexSubImage2DArg& info, napi_value pixels, GLuint srcOffset)
{
    TexSubImage2DArg& imgArg = const_cast<TexSubImage2DArg&>(info);
    imgArg.Dump("WebGL texSubImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        LOGE("Can not find texture ");
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SET_ERROR(error);
    }

    if (!texture->CheckValid(imgArg.target, imgArg.level)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "texSubImage2D invalid texture level");
        return nullptr;
    }

    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_);
    bool changeUnpackAlignment = false;
    if (!NVal(env, pixels).IsNull()) {
        error = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, pixels, srcOffset);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D invalid pixels");
            return nullptr;
        }
        changeUnpackAlignment = unpackFlipY_ || unpackPremultiplyAlpha_;
        data = imageSource.GetImageSourceData(unpackFlipY_, unpackPremultiplyAlpha_);
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }

    error = CheckTexSubImage2D(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SET_ERROR(error);
        return nullptr;
    }

    if (imgArg.type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
        // The UNSIGNED_INT_10F_11F_11F_REV type pack/unpack isn't implemented.
        imgArg.type = GL_FLOAT;
    }
    TexSubImage2D_(imgArg, texture, data, changeUnpackAlignment);
    LOGE("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(
    napi_env env, const TexSubImage2DArg& info, napi_value imageData)
{
    TexSubImage2DArg imgArg(info);
    imgArg.Dump("WebGL texSubImage2D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        LOGE("Can not find texture ");
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum error = CheckTexImage(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SET_ERROR(error);
        return nullptr;
    }
    error = CheckTexSubImage2D(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SET_ERROR(error);
        return nullptr;
    }

    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_);
    if (!NVal(env, imageData).IsNull()) {
        error = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, imageData);
        if (error) {
            SET_ERROR_WITH_LOG(error, "texSubImage2D Image source invalid");
            return nullptr;
        }
        data = imageSource.GetImageSourceData(unpackFlipY_, unpackPremultiplyAlpha_);
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }
    TexSubImage2D_(imgArg, texture, data, unpackAlignment_);
    LOGE("WebGL texSubImage2D target %{public}u result %{public}u", imgArg.target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DrawElements(
    napi_env env, GLenum mode, GLsizei count, GLenum type, GLintptr offset)
{
    LOGI("WebGL drawElements mode %{public}u %{public}u %{public}u %{public}u", mode, count, type, offset);
    GLenum result = CheckDrawElements(env, mode, count, type, static_cast<int64_t>(offset));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL drawElements error %{public}u", result);
        SET_ERROR(result);
        return nullptr;
    }
    glDrawElements(mode, count, type, reinterpret_cast<GLvoid*>(offset));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DrawArrays(napi_env env, GLenum mode, GLint first, GLsizei count)
{
    LOGI("WebGL drawArrays mode %{public}u %{public}u %{public}u", mode, first, count);
    GLenum result = CheckDrawArrays(env, mode, first, count);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }
    glDrawArrays(mode, first, count);
    LOGI("WebGL drawArrays %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ReadPixels(napi_env env, const PixelsArg* arg, GLintptr offset)
{
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

    GLenum result = CheckReadPixelsArg(arg, size);
    if (!result) {
        SET_ERROR(result);
        return nullptr;
    }
    glReadPixels(arg->x, arg->y, arg->width, arg->height, arg->format, arg->type, reinterpret_cast<void*>(offset));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ReadPixels(
    napi_env env, const PixelsArg* arg, napi_value buffer, GLuint dstOffset)
{
    LOGI("WebGL readPixels pixels [%{public}d %{public}d %{public}d %{public}d %{public}u %{public}u]", arg->x, arg->y,
        arg->width, arg->height, arg->format, arg->type);

    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(buffer);
    if (status != 0) {
        LOGE("WebGL readPixels failed to getbuffer data");
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (!CheckPixelsFormat(env, arg->format)) {
        LOGE("WebGL readPixels invalid format %{public}u", arg->format);
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckPixelsType(env, arg->type)) {
        LOGE("WebGL readPixels invalid type %{public}u", arg->type);
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum result = CheckReadPixelsArg(arg, static_cast<int64_t>(bufferData.GetBufferLength()));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL readPixels result %{public}u", result);
        SET_ERROR(result);
        return nullptr;
    }

    LOGI("WebGL readPixels pixels %{public}p", bufferData.GetBuffer());
    glReadPixels(arg->x, arg->y, arg->width, arg->height, arg->format, arg->type,
        static_cast<void*>(bufferData.GetBuffer() + dstOffset));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData_(
    napi_env env, GLenum target, GLsizeiptr size, GLenum usage, WebGLReadBufferArg* bufferData)
{
    LOGI("WebGL bufferData target %{public}u, usage %{public}u size %{public}u", target, usage, size);
    WebGLBuffer* webGLBuffer = CheckAndGetBoundBuffer(env, target);
    if (webGLBuffer == nullptr) {
        LOGE("WebGL bufferData can not get bound buffer");
        return nullptr;
    }
    if (!CheckBufferDataUsage(env, usage)) {
        LOGE("WebGL bufferData invalid usage %{public}u", usage);
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (webGLBuffer->GetTarget() != target) {
        LOGE("WebGL bufferData Invalid buffer target %{public}u %{public}u", webGLBuffer->GetTarget(), target);
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    webGLBuffer->SetBufferSize(size);
    if (bufferData != nullptr) {
        glBufferData(target, size, bufferData->GetBuffer(), usage);
    } else {
        glBufferData(target, size, nullptr, usage);
    }
    webGLBuffer->SetTarget(target);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData(napi_env env, GLenum target, int64_t size, GLenum usage)
{
    LOGI("WebGL bufferData target %{public}u, usage %{public}u size %{public}u", target, usage, size);
    BufferData_(env, target, static_cast<GLsizeiptr>(size), usage, nullptr);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferData(
    napi_env env, GLenum target, napi_value data, GLenum usage, GLuint srcOffset, GLuint length)
{
    LOGI("WebGL bufferData target %{public}u, usage %{public}u ", target, usage);
    WebGLReadBufferArg bufferData(env);
    if (NVal(env, data).IsNull()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bool succ = bufferData.GenBufferData(data, BUFFER_DATA_FLOAT_32) == napi_ok;
    if (!succ) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGI("WebGL bufferData buffer size %{public}zu", bufferData.GetBufferLength());
    // change
    srcOffset = static_cast<GLuint>(srcOffset * bufferData.GetBufferDataSize());
    length = (length == 0) ? static_cast<GLuint>(bufferData.GetBufferLength())
                        : static_cast<GLuint>(length * bufferData.GetBufferDataSize());
    BufferData_(env, target, length, usage, &bufferData);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferSubData(
    napi_env env, GLenum target, GLintptr offset, napi_value buffer, GLuint srcOffset, GLuint length)
{
    LOGI("WebGL bufferSubData target %{public}u offset %{public}u %{public}u %{public}u", target, offset, srcOffset,
        length);

    WebGLBuffer* webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::ARRAY_BUFFER]);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferSize() == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "bufferSubData Can not find bound buffer");
        return nullptr;
    }

    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(buffer);
    if (status != 0) {
        LOGE("WebGL bufferSubData invalid buffer data ");
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    // check sub buffer
    if ((static_cast<size_t>(offset) + bufferData.GetBufferLength()) > webGLBuffer->GetBufferSize()) {
        LOGE("WebGL bufferSubData invalid buffer size %{public}zu offset %{public}zu ", bufferData.GetBufferLength(),
            webGLBuffer->GetBufferSize());
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    LOGI("WebGL bufferSubData size %{public}zu", bufferData.GetBufferLength());

    glBufferSubData(target, offset, static_cast<GLsizeiptr>(bufferData.GetBufferLength()),
        static_cast<uint8_t*>(bufferData.GetBuffer()));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::PixelStorei(napi_env env, GLenum pname, GLint param)
{
    LOGI("WebGL pixelStorei pname %{public}u param %{public}u", pname, param);
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
                LOGE("WebGL pixelStorei invalid parameter for UNPACK_COLORSPACE_CONVERSION_WEBGL %{public}u", pname);
                SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
                return nullptr;
            }
            break;
        default:
            LOGE("WebGL pixelStorei invalid pname  %{public}u", pname);
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    glPixelStorei(pname, param);
    return nullptr;
}

std::tuple<bool, WebGLTexture*> WebGLRenderingContextBaseImpl::CheckCompressedTexImage2D(
    napi_env env, const TexImageArg& imgArg, size_t imageSize)
{
    if (!CheckTextureLevel(imgArg.target, imgArg.level)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return std::make_tuple(false, nullptr);
    }

    if (!CheckTexImageInternalFormat(IMAGE_COMPRESSED_TEX_IMAGE_2D, imgArg.internalFormat)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return std::make_tuple(false, nullptr);
    }
    if (imgArg.border) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return std::make_tuple(false, nullptr);
    }

    GLenum result = CheckCompressedTexDimensions(imgArg);
    if (result) {
        SET_ERROR(result);
        return std::make_tuple(false, nullptr);
    }
    result = CheckCompressedTexData(imgArg, imageSize);
    if (result) {
        SET_ERROR(result);
        return std::make_tuple(false, nullptr);
    }

    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return std::make_tuple(false, nullptr);
    }

    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return std::make_tuple(false, nullptr);
    }
    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return std::make_tuple(false, nullptr);
    }
    return std::make_tuple(true, texture);
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexImage2D(
    napi_env env, const TexImageArg& imgArg, GLsizei imageSize, GLintptr offset)
{
    // TODO
    imgArg.Dump("WebGL compressedTexImage2D");
    bool succ;
    WebGLTexture* texture = nullptr;
    tie(succ, texture) = CheckCompressedTexImage2D(env, imgArg, static_cast<size_t>(imageSize));
    if (!succ) {
        return nullptr;
    }

    glCompressedTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height,
        imgArg.border, imageSize, reinterpret_cast<void*>(offset));

    texture->SetTextureLevel(
        { imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, 1, GL_UNSIGNED_BYTE });
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

    WebGLTexture* texture = nullptr;
    tie(succ, texture) = CheckCompressedTexImage2D(env, imgArg, length);
    if (!succ) {
        return nullptr;
    }
    glCompressedTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height,
        imgArg.border, static_cast<GLsizei>(length), data);

    texture->SetTextureLevel(
        { imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, 1, GL_UNSIGNED_BYTE });
    return nullptr;
}

bool WebGLRenderingContextBaseImpl::CheckCompressedTexSubImage2D(
    napi_env env, const TexSubImage2DArg& imgArg, size_t imageSize)
{
    if (!CheckTextureLevel(imgArg.target, imgArg.level)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return false;
    }

    if (!CheckTexImageInternalFormat(IMAGE_COMPRESSED_TEX_SUB_IMAGE_2D, imgArg.format)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return false;
    }
    if (imgArg.border) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return false;
    }

    GLenum result = CheckCompressedTexData(imgArg, imageSize);
    if (result) {
        SET_ERROR(result);
        return false;
    }

    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
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
    if (NVal(env, srcData).IsNull()) {
        // support ArrayBufferView
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
    LOGE("WebGL compressedTexSubImage2D error %{public}u", GetError_());
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
    LOGE("WebGL compressedTexSubImage2D error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CopyTexImage2D(napi_env env, const CopyTexImage2DArg& imgArg)
{
    imgArg.Dump("WebGL copyTexImage2D");

    if (!CheckTextureLevel(imgArg.target, imgArg.level)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    if (!WebGLTexture::CheckTextureFormatAndType(
            imgArg.internalFormat, imgArg.format, imgArg.type, imgArg.level, IsHighWebGL())) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    if (imgArg.border) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLenum result = CheckTexFuncDimensions(imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    if (!CheckSettableTexFormat(imgArg.format)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (!texture) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    if (!CheckTexInternalFormatColorBufferCombination(imgArg.internalFormat, GetBoundFrameBufferColorFormat(env))) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    if (!IsHighWebGL() && imgArg.level && WebGLTexture::CheckNPOT(imgArg.width, imgArg.height)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLuint frameBufferId = 0;
    if (!CheckReadBufferAndGetInfo(env, frameBufferId, nullptr, nullptr)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    glCopyTexImage2D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.x, imgArg.y, imgArg.width,
        imgArg.height, imgArg.border);

    texture->SetTextureLevel({ imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, 1, 1 });
    LOGE("WebGL copyTexImage2D error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CopyTexSubImage2D(napi_env env, const CopyTexSubImageArg& imgArg)
{
    imgArg.Dump("WebGL copyTexImage2D");
    GLenum result = CheckCopyTexSubImage(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    GLuint frameBufferId = 0;
    if (!CheckReadBufferAndGetInfo(env, frameBufferId, nullptr, nullptr)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    // TODO clearIfComposited();
    // ScopedDrawingBufferBinder binder(drawingBuffer(), readFramebufferBinding);
    glCopyTexSubImage2D(
        imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.x, imgArg.y, imgArg.width, imgArg.height);
    LOGE("WebGL copyTexSubImage2D error %{public}u", GetError_());
    return nullptr;
}

bool WebGLRenderingContextBaseImpl::CheckTextureLevel(GLenum target, GLint level)
{
    if ((level < 0) || level > WebGLTexture::GetMaxTextureLevelForTarget(target, IsHighWebGL())) {
        return false;
    }
    return true;
}

GLenum WebGLRenderingContextBaseImpl::CheckTexImage(napi_env env, const TexImageArg& imgArg, WebGLTexture* texture)
{
    if (!CheckTextureLevel(imgArg.target, imgArg.level)) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (imgArg.border) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    GLenum internalFormat = 0;
    if (imgArg.internalFormat == 0) {
        internalFormat = texture->GetInternalFormat(imgArg.target, imgArg.level);
    }

    if (!WebGLTexture::CheckTextureFormatAndType(
            imgArg.internalFormat, imgArg.format, imgArg.type, imgArg.level, IsHighWebGL())) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    GLenum result = CheckTexFuncDimensions(imgArg);
    if (result) {
        LOGE("Invalid texture dimension or type %{public}u", result);
        return result;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGLRenderingContextBaseImpl::CheckSettableTexFormat(GLenum format)
{
    if (IsHighWebGL()) {
        return true;
    }

    /*
    if (WebGLImageConversion::getChannelBitsByFormat(format) & WebGLImageConversion::ChannelDepthStencil) {
        synthesizeGLError(GL_INVALID_OPERATION, functionName, "format can not be set, only rendered to");
        return false;
    }
    */
    return true;
}

GLenum WebGLRenderingContextBaseImpl::CheckTexSubImage2D(
    napi_env env, const TexSubImage2DArg& imgArg, WebGLTexture* texture)
{
    if (!CheckSettableTexFormat(imgArg.format)) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    if (imgArg.xOffset < 0 || imgArg.yOffset < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    // Before checking if it is in the range, check if overflow happens first.
    if (imgArg.xOffset + imgArg.width < 0 || imgArg.yOffset + imgArg.height < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (imgArg.xOffset + imgArg.width > texture->GetWidth(imgArg.target, imgArg.level) ||
        imgArg.yOffset + imgArg.height > texture->GetHeight(imgArg.target, imgArg.level)) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (!IsHighWebGL() && texture->GetType(imgArg.target, imgArg.level) != imgArg.type) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGLRenderingContextBaseImpl::CheckTexImageInternalFormat(int func, GLenum internalFormat)
{
    switch (func) {
        case IMAGE_COMPRESSED_TEX_IMAGE_2D:
            // m_compressedTextureFormats.contains(format);
            break;

        default:
            break;
    }
    return true;
}

GLenum WebGLRenderingContextBaseImpl::GetValidInternalFormat(GLenum internalFormat, GLenum type)
{
    // Convert to sized internal formats that are renderable with GL_CHROMIUM_color_buffer_float_rgb(a).
#if 0
    if (type == GL_FLOAT && internalformat == GL_RGBA
        && extensionsUtil()->isExtensionEnabled("GL_CHROMIUM_color_buffer_float_rgba"))
        return GL_RGBA32F_EXT;
    if (type == GL_FLOAT && internalformat == GL_RGB
        && extensionsUtil()->isExtensionEnabled("GL_CHROMIUM_color_buffer_float_rgb"))
        return GL_RGB32F_EXT;
#endif
    return internalFormat;
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
            if (imgArg.width > (maxTextureSize_ >> imgArg.level)) {
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
    bool widthValid = true;
    bool heightValid = true;
    switch (imgArg.format) {
#ifdef COMPRESSED_RGBA_ASTC
        case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
        case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
        case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
        case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
        case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
        case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
        case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR: {
            widthValid = true;
            heightValid = true;
            break;
        }
        case GC3D_COMPRESSED_ATC_RGB_AMD:
        case GC3D_COMPRESSED_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        case GC3D_COMPRESSED_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
#endif
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: {
            widthValid = (imgArg.level && imgArg.width == 1) || (imgArg.level && imgArg.width == 2) ||
                         !(imgArg.width % 4); // 4 kBlockWidth
            heightValid = (imgArg.level && imgArg.height == 1) || (imgArg.level && imgArg.height == 2) ||
                          !(imgArg.height % 4); // 4 kBlockWidth
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
            return WebGLRenderingContextBase::INVALID_ENUM;
    }

    if (!widthValid || !heightValid) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckCompressedTexData(const TexImageArg& imgArg, size_t dataLen)
{
    size_t bytesRequired = 0;

    switch (imgArg.format) {
#ifdef COMPRESSED_RGBA_ASTC
        case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
        case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
        case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
        case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
        case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
        case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
        case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
        case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR: {
            const int index = (imgArg.format < GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR)
                                  ? (int)imgArg.format - GL_COMPRESSED_RGBA_ASTC_4x4_KHR
                                  : (int)imgArg.format - GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;

            const int kBlockSize = 16;
            const int kBlockWidth = WebGLCompressedTextureASTC::kBlockSizeCompressASTC[index].blockWidth;
            const int kBlockHeight = WebGLCompressedTextureASTC::kBlockSizeCompressASTC[index].blockHeight;

            int numBlocksAcross = (imgArg.width + kBlockWidth - 1) / kBlockWidth;
            int numBlocksDown = (imgArg.height + kBlockHeight - 1) / kBlockHeight;
            int numBlocks = numBlocksAcross * numBlocksDown;
            bytesRequired = numBlocks * kBlockSize;
            break;
        }
#endif
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
        // case GC3D_COMPRESSED_ATC_RGB_AMD:
        case GL_ETC1_RGB8_OES: {
            bytesRequired = floor(static_cast<double>((imgArg.width + 3) / 4)) *
                            floor(static_cast<double>((imgArg.height + 3) / 4)) * 8;
            break;
        }
        /*
        case GC3D_COMPRESSED_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        case GC3D_COMPRESSED_ATC_RGBA_INTERPOLATED_ALPHA_AMD: {
            bytesRequired = floor(static_cast<double>((imgArg.width + 3) / 4)) *
                            floor(static_cast<double>((imgArg.height + 3) / 4)) * 16;
            break;
        }
        */
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG: {
            bytesRequired = (max(imgArg.width, 8) * max(imgArg.height, 8) * 4 + 7) / 8;
            break;
        }
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: {
            bytesRequired = (max(imgArg.width, 16) * max(imgArg.height, 8) * 2 + 7) / 8;
            break;
        }
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
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
        /*
        case GC3D_COMPRESSED_ATC_RGB_AMD:
        case GC3D_COMPRESSED_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        case GC3D_COMPRESSED_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
        */
        case GL_ETC1_RGB8_OES: {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGLRenderingContextBaseImpl::CheckTexInternalFormatColorBufferCombination(
    GLenum texInternalFormat, GLenum colorBufferFormat)
{
    // unsigned need = WebGLImageConversion::getChannelBitsByFormat(texInternalFormat);
    // unsigned have = WebGLImageConversion::getChannelBitsByFormat(colorBufferFormat);
    //  return (need & have) == need;
    return true;
}

// validateTexFuncData
GLenum WebGLRenderingContextBaseImpl::CheckTextureDataBuffer(
    const TexImageArg& arg, const WebGLReadBufferArg* bufferData)
{
    if (!bufferData) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (!CheckSettableTexFormat(arg.format)) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    switch (arg.type) {
    case GL_BYTE:
        if (bufferData->GetBufferDataType() != BUFFER_DATA_INT_8) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        break;
    case GL_UNSIGNED_BYTE:
        if (bufferData->GetBufferDataType() != BUFFER_DATA_UINT_8) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        break;
    case GL_SHORT:
        if (bufferData->GetBufferDataType() != BUFFER_DATA_INT_16) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        break;
    case GL_UNSIGNED_SHORT:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
        if (bufferData->GetBufferDataType() != BUFFER_DATA_UINT_16) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        break;
    case GL_INT:
        if (bufferData->GetBufferDataType() != BUFFER_DATA_INT_32) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        break;
    case GL_UNSIGNED_INT:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
    case GL_UNSIGNED_INT_5_9_9_9_REV:
    case GL_UNSIGNED_INT_24_8:
        if (bufferData->GetBufferDataType() != BUFFER_DATA_UINT_32) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        break;
    case GL_FLOAT: // OES_texture_float
        if (bufferData->GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        break;
    case GL_HALF_FLOAT:
    case GL_HALF_FLOAT_OES:
        if (bufferData->GetBufferDataType() != BUFFER_DATA_UINT_16) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
        break;
    default:
        break;
    }

    /*
    unsigned totalBytesRequired;
    GLenum error = WebGLImageConversion::computeImageSizeInBytes(format, type, width, height, m_unpackAlignment,
    &totalBytesRequired, 0); if (error != GL_NO_ERROR) { synthesizeGLError(error, functionName, "invalid texture
    dimensions"); return false;
    }
    if (pixels->byteLength() < totalBytesRequired) {
        if (m_unpackAlignment != 1) {
            error = WebGLImageConversion::computeImageSizeInBytes(format, type, width, height, 1, &totalBytesRequired,
    0); if (pixels->byteLength() == totalBytesRequired) { synthesizeGLError(GL_INVALID_OPERATION, functionName,
    "ArrayBufferView not big enough for request with UNPACK_ALIGNMENT > 1"); return false;
            }
        }
        synthesizeGLError(GL_INVALID_OPERATION, functionName, "ArrayBufferView not big enough for request");
        return false;
    }
    */
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckCopyTexSubImage(napi_env env, const CopyTexSubImageArg& arg)
{
    if (!CheckTextureLevel(arg.target, arg.level)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }

    WebGLTexture* texture = GetBoundTexture(env, arg.target, false);
    if (texture == nullptr) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (!WebGLTexture::CheckTextureSize(arg.xOffset, arg.width, texture->GetWidth(arg.target, arg.level)) ||
        !WebGLTexture::CheckTextureSize(arg.yOffset, arg.height, texture->GetHeight(arg.target, arg.level))) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (arg.func == IMAGE_COPY_TEX_SUB_IMAGE_3D) {
        const CopyTexSubImage3DArg *img3D = reinterpret_cast<const CopyTexSubImage3DArg *>(&arg);
        if (!WebGLTexture::CheckTextureSize(img3D->zOffset, img3D->depth, texture->GetDepth(arg.target, arg.level))) {
            return WebGLRenderingContextBase::INVALID_VALUE;
        }
    }

    GLenum internalFormat = texture->GetInternalFormat(arg.target, arg.level);
    if (!CheckSettableTexFormat(internalFormat)) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (!CheckTexInternalFormatColorBufferCombination(internalFormat, GetBoundFrameBufferColorFormat(env))) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
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
    if (!CheckFrameBufferBoundComplete(env)) {
        return WebGLRenderingContextBase::INVALID_FRAMEBUFFER_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckDrawElements(napi_env env, GLenum mode, GLsizei count, GLenum type, int64_t offset)
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
        case WebGLRenderingContextBase::UNSIGNED_INT:
            size = sizeof(int);
            if (IsHighWebGL()) {
                break;
            }
            [[fallthrough]];
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (count < 0 || offset < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if ((offset % size) != 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    // TODO count == 0

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
    if (!CheckFrameBufferBoundComplete(env)) {
        return WebGLRenderingContextBase::INVALID_FRAMEBUFFER_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}
} // namespace Impl
} // namespace Rosen
} // namespace OHOS
