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
#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_context_attributes.h"
#include "context/webgl_rendering_context.h"
#include "context/webgl_rendering_context_base.h"
#include "context/webgl_rendering_context_base_impl.h"
#include "context/webgl_rendering_context_basic_base.h"
#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "util/egl_manager.h"
#include "util/log.h"
#include "util/object_source.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;
#define SET_ERROR(error, ...) \
    do {                      \
        LOGE(__VA_ARGS__);    \
        SetError(error);      \
    } while (0)

void WebGLRenderingContextBaseImpl::TexImage2D_(
    const TexImageArg* imgArg, WebGLTexture* texture, const void* pixels, bool changeUnpackAlignment)
{
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    GLenum internalFormat = GetValidInternalFormat(imgArg->internalFormat, imgArg->type);
    glTexImage2D(imgArg->target, imgArg->level, internalFormat, imgArg->width, imgArg->height, imgArg->border,
        imgArg->format, imgArg->type, pixels);
    texture->SetTextureLevel(imgArg);

    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment_);
    }
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(
    napi_env env, const TexImageArg* imgArg, napi_value pixels, GLuint srcOffset)
{
    imgArg->Dump("WebGL texImage2D");
    WebGLReadBufferArg bufferData(env);
    GLvoid* data = nullptr;
    if (!NVal(env, pixels).IsNull()) {
        napi_status status = bufferData.GenBufferData(pixels);
        if (status != napi_ok) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texImage2D get buffer fail");
            return nullptr;
        }
        data = reinterpret_cast<void*>(bufferData.GetBuffer());
    }
    WebGLTexture* texture = nullptr;
    GLenum error;
    std::tie(error, texture) = CheckTextureInfo(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texImage2D error %{public}u", error);
        SetError(error);
        return nullptr;
    }
    if (texture->CheckImmutable()) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg->level && WebGLTexture::CheckNPOT(imgArg->width, imgArg->height)) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    bool changeUnpackAlignment = false;
    /*
    Vector<uint8_t> tempData;
    if (data && (unpackFlipY_ || unpackPremultiplyAlpha_)) {
        if (!WebGLImageConversion::extractTextureData(width, height, format, type, unpackAlignment_, unpackFlipY_, ,
    data, tempData)) return; data = tempData.data(); changeUnpackAlignment = true;
    }
    */

    TexImage2D_(imgArg, texture, data, changeUnpackAlignment);
    LOGE("WebGL texImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const TexImageArg* info, napi_value source)
{
    TexImageArg* imgArg = const_cast<TexImageArg*>(info);
    imgArg->Dump("WebGL texImage2D");
    if (NVal(env, source).IsNull()) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    WebGLTexture* texture = nullptr;
    GLenum error;
    std::tie(error, texture) = CheckTextureInfo(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texImage2D error %{public}u", error);
        SetError(error);
    }
    if (texture->CheckImmutable()) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg->level && WebGLTexture::CheckNPOT(imgArg->width, imgArg->height)) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (!CheckSettableTexFormat(imgArg->format)) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (imgArg->type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
        // The UNSIGNED_INT_10F_11F_11F_REV type pack/unpack isn't implemented.
        imgArg->type = GL_FLOAT;
    }
    WebGLImageSource imageSource;
    bool succ = imageSource.GenImageSource(env, source);
    if (!succ) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texImage2D Image source invalid");
        return nullptr;
    }

    GLvoid* data = imageSource.GetImageSourceData(info->format, info->type, unpackFlipY_, unpackPremultiplyAlpha_);
    imgArg->width = imageSource.GetWidth();
    imgArg->height = imageSource.GetHeight();
    TexImage2D_(imgArg, texture, data, unpackAlignment_ != 1);
    LOGE("WebGL texImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexImage2D(napi_env env, const TexImageArg* imgArg, GLintptr pbOffset)
{
    imgArg->Dump("WebGL texImage2D");
    WebGLTexture* texture = nullptr;
    GLenum error;
    std::tie(error, texture) = CheckTextureInfo(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texImage2D error %{public}u", error);
        SetError(error);
    }
    if (texture->CheckImmutable()) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!IsHighWebGL() && imgArg->level && WebGLTexture::CheckNPOT(imgArg->width, imgArg->height)) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (!CheckSettableTexFormat(imgArg->format)) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glTexImage2D(imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->width, imgArg->height, imgArg->border,
        imgArg->format, imgArg->type, reinterpret_cast<GLvoid*>(pbOffset));
    LOGE("WebGL texImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}

void WebGLRenderingContextBaseImpl::TexSubImage2D_(
    const TexSubImage2DArg* imgArg, WebGLTexture* texture, const void* pixels, bool changeUnpackAlignment)
{
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    glTexSubImage2D(imgArg->target, imgArg->level, imgArg->xOffset, imgArg->yOffset, imgArg->width, imgArg->height,
        imgArg->format, imgArg->type, pixels);
    texture->SetTextureLevel(imgArg);

    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment_);
    }
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(napi_env env, const TexSubImage2DArg* imgArg, GLintptr pbOffset)
{
    // TODO for WebGL 2
    imgArg->Dump("WebGL texSubImage2D");
    WebGLTexture* texture = nullptr;
    GLenum error;
    std::tie(error, texture) = CheckTextureInfo(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SetError(error);
    }
    LOGE("WebGL texSubImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(
    napi_env env, const TexSubImage2DArg* info, napi_value pixels, GLuint srcOffset)
{
    TexSubImage2DArg *imgArg = const_cast<TexSubImage2DArg *>(info);
    imgArg->Dump("WebGL texSubImage2D");
    WebGLTexture* texture = nullptr;
    GLenum error;
    std::tie(error, texture) = CheckTextureInfo(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SetError(error);
    }

    if (!texture->CheckValid(imgArg->target, imgArg->level)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION, "texSubImage2D invalid texture level");
        return nullptr;
    }

    GLvoid* data = nullptr;
    WebGLImageSource imageSource;
    bool changeUnpackAlignment = false;
    if (!NVal(env, pixels).IsNull()) {
        bool succ = imageSource.GenImageSource(env, imgArg->width, imgArg->height, pixels);
        if (!succ) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texSubImage2D invalid pixels");
            return nullptr;
        }
        changeUnpackAlignment = unpackFlipY_ || unpackPremultiplyAlpha_;
        data = imageSource.GetImageSourceData(imgArg->format, imgArg->type, unpackFlipY_, unpackPremultiplyAlpha_);
        //imgArg->width = imageSource.GetWidth();
        //imgArg->height = imageSource.GetHeight();
    }

    error = CheckTexSubImage2D(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SetError(error);
        return nullptr;
    }

    if (imgArg->type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
        // The UNSIGNED_INT_10F_11F_11F_REV type pack/unpack isn't implemented.
        imgArg->type = GL_FLOAT;
    }
    TexSubImage2D_(imgArg, texture, data, changeUnpackAlignment);
    LOGE("WebGL texSubImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::TexSubImage2D(
    napi_env env, const TexSubImage2DArg* info, napi_value imageData)
{
    TexSubImage2DArg* imgArg = const_cast<TexSubImage2DArg*>(info);
    imgArg->Dump("WebGL texSubImage2D");

    WebGLTexture* texture = nullptr;
    GLenum error;
    std::tie(error, texture) = CheckTextureInfo(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SetError(error);
        return nullptr;
    }
    error = CheckTexSubImage2D(env, imgArg, texture);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL texSubImage2D error %{public}u", error);
        SetError(error);
        return nullptr;
    }

    GLvoid* data = nullptr;
    WebGLImageSource imageSource;
    if (!NVal(env, imageData).IsNull()) {
        bool succ = imageSource.GenImageSource(env, imageData);
        if (!succ) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texSubImage2D Image source invalid");
            return nullptr;
        }
        data = imageSource.GetImageSourceData(info->format, info->type, unpackFlipY_, unpackPremultiplyAlpha_);
        imgArg->width = imageSource.GetWidth();
        imgArg->height = imageSource.GetHeight();
    }
    TexSubImage2D_(imgArg, texture, data, unpackAlignment_);
    LOGE("WebGL texSubImage2D target %{public}u result %{public}u", imgArg->target, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DrawElements(
    napi_env env, GLenum mode, GLsizei count, GLenum type, GLintptr offset)
{
    LOGI("WebGL drawElements mode %{public}u %{public}u %{public}u %{public}u", mode, count, type, offset);
    if (!CheckDrawMode(env, mode)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckStencil(env)) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
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
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    if (count < 0 || offset < 0) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if ((offset % size) != 0) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    // TODO count == 0

    WebGLBuffer* webGLBuffer =
        GetObjectInstance<WebGLBuffer>(env, boundBufferIds[BoundBufferType::ELEMENT_ARRAY_BUFFER]);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferData() == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION, "drawElements Can not find bound buffer");
        return nullptr;
    }
    LOGI("WebGL drawElements buffer length %{public}zu", webGLBuffer->GetBufferData()->GetBufferLength());

    // check count
    if (size * count > webGLBuffer->GetBufferData()->GetBufferLength()) {
        LOGE("WebGL drawElements Insufficient buffer size %{public}u", count);
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
    }
    if (static_cast<size_t>(offset) >= webGLBuffer->GetBufferData()->GetBufferLength()) {
        LOGE("WebGL drawElements invalid offset %{public}u", offset);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    if (!currentProgramId) {
        LOGE("WebGL drawArrays no valid shader program in use");
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glDrawElements(mode, static_cast<GLsizei>(count), type, reinterpret_cast<GLvoid*>(offset));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DrawArrays(napi_env env, GLenum mode, GLint first, GLsizei count)
{
    LOGI("WebGL drawArrays mode %{public}u %{public}u %{public}u", mode, first, count);
    if (!CheckDrawMode(env, mode)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckStencil(env)) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
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
    if (!CheckFrameBufferBoundComplete(env)) {
        SetError(WebGLRenderingContextBase::INVALID_FRAMEBUFFER_OPERATION);
        return nullptr;
    }
    LOGI("WebGL drawArrays %{public}u", GetError_());
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
    if (buffer == nullptr || buffer->GetBufferData() == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    uint64_t size = static_cast<uint64_t>(buffer->GetBufferData()->GetRealBufferLength());
    if (size < offset) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    size = size - offset;

    GLenum result = CheckReadPixelsArg(arg, size);
    if (!result) {
        SetError(result);
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
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (!CheckPixelsFormat(env, arg->format)) {
        LOGE("WebGL readPixels invalid format %{public}u", arg->format);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!CheckPixelsType(env, arg->type)) {
        LOGE("WebGL readPixels invalid type %{public}u", arg->type);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum result = CheckReadPixelsArg(arg, static_cast<int64_t>(bufferData.GetBufferLength()));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL readPixels result %{public}u", result);
        SetError(result);
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

napi_value WebGLRenderingContextBaseImpl::BufferData(
    napi_env env, GLenum target, napi_value data, GLenum usage, GLuint srcOffset, GLuint length)
{
    LOGI("WebGL bufferData target %{public}u, usage %{public}u ", target, usage);
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
    LOGI("WebGL bufferData buffer size %{public}zu", bufferData->GetBufferLength());

    // change
    srcOffset = static_cast<GLuint>(srcOffset * bufferData->GetBufferDataSize());
    length = (length == 0) ? static_cast<GLuint>(bufferData->GetBufferLength())
                           : static_cast<GLuint>(length * bufferData->GetBufferDataSize());
    bufferData->SetRealBuffer(length, srcOffset);

    BufferData_(env, target, length, usage, bufferData.get());
    bufferData.release();
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BufferSubData(
    napi_env env, GLenum target, GLintptr offset, napi_value buffer, GLuint srcOffset, GLuint length)
{
    LOGI("WebGL bufferSubData target %{public}u offset %{public}u %{public}u %{public}u", target, offset, srcOffset,
        length);

    WebGLBuffer* webGLBuffer = GetObjectInstance<WebGLBuffer>(env, boundBufferIds[BoundBufferType::ARRAY_BUFFER]);
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
    if ((static_cast<size_t>(offset) + bufferData.GetBufferLength()) >
        webGLBuffer->GetBufferData()->GetBufferLength()) {
        LOGE("WebGL bufferSubData invalid buffer size %{public}zu offset %{public}zu ", bufferData.GetBufferLength(),
            webGLBuffer->GetBufferData()->GetBufferLength());
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
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
                SetError(WebGLRenderingContextBase::INVALID_VALUE);
                return nullptr;
            }
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

bool WebGLRenderingContextBaseImpl::CheckCompressedTexImage2D(
    napi_env env, const TexImageArg* textureInfo, size_t imageSize)
{
    if (!CheckTextureLevel(textureInfo->target, textureInfo->level)) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return false;
    }

    if (!CheckTexImageInternalFormat(IMAGE_2D_FUNC_COMPRESSED_TEX_IMAGE_2D, textureInfo->internalFormat)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return false;
    }
    if (textureInfo->border) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return false;
    }

    GLenum result = CheckCompressedTexDimensions(textureInfo, imageSize);
    if (result) {
        SetError(result);
        return false;
    }

    WebGLTexture* texture = GetBoundTexture(env, textureInfo->target, true);
    if (!texture) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return false;
    }

    if (texture->CheckImmutable()) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return false;
    }
    if (!IsHighWebGL() && textureInfo->level && WebGLTexture::CheckNPOT(textureInfo->width, textureInfo->height)) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return false;
    }
    return true;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexImage2D(
    napi_env env, const TexImageArg* imgArg, GLsizei imageSize, GLintptr offset)
{
    // TODO
    imgArg->Dump("WebGL compressedTexSubImage2D");
    bool succ = CheckCompressedTexImage2D(env, imgArg, static_cast<size_t>(imageSize));
    if (!succ) {
        return nullptr;
    }

    glCompressedTexImage2D(imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->width, imgArg->height,
        imgArg->border, imageSize, reinterpret_cast<void*>(offset));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexImage2D(
    napi_env env, const TexImageArg* imgArg, napi_value srcData, GLuint srcOffset, GLuint srcLengthOverride)
{
    imgArg->Dump("WebGL compressedTexSubImage2D");

    WebGLReadBufferArg bufferData(env);
    bool succ = bufferData.GenBufferData(srcData, BUFFER_DATA_FLOAT_32) == napi_ok;
    if (!succ) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
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

    succ = CheckCompressedTexImage2D(env, imgArg, length);
    if (!succ) {
        return nullptr;
    }

    glCompressedTexImage2D(imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->width, imgArg->height,
        imgArg->border, static_cast<GLsizei>(length), data);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexSubImage2D(
    napi_env env, const TexSubImage2DArg* imgArg, napi_value srcData, GLuint srcOffset, GLuint srcLengthOverride)
{
    LOGI("WebGL compressedTexSubImage2D target %{public}u %{public}d %{public}u ", imgArg->target, imgArg->level,
        imgArg->format);
    LOGI("WebGL compressedTexSubImage2D target %{public}d %{public}d %{public}d %{public}d", imgArg->xOffset,
        imgArg->yOffset, imgArg->width, imgArg->height);

    WebGLTexture* texture = nullptr;
    GLenum error;
    std::tie(error, texture) = CheckTextureInfo(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL compressedTexSubImage2D error %{public}u", error);
        SetError(error);
    }

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
        data = reinterpret_cast<void*>(bufferData.GetBuffer());
        length = static_cast<GLsizei>(bufferData.GetBufferLength());
    }
    glCompressedTexSubImage2D(imgArg->target, imgArg->level, imgArg->xOffset, imgArg->yOffset, imgArg->width,
        imgArg->height, imgArg->format, length, data);
    LOGE("WebGL compressedTexSubImage2D error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompressedTexSubImage2D(
    napi_env env, const TexSubImage2DArg* imgArg, GLsizei imageSize, GLintptr offset)
{
    LOGI("WebGL compressedTexSubImage2D target %{public}u %{public}d %{public}u ", imgArg->target, imgArg->level,
        imgArg->format);
    LOGI("WebGL compressedTexSubImage2D target %{public}d %{public}d %{public}d %{public}d", imgArg->xOffset,
        imgArg->yOffset, imgArg->width, imgArg->height);

    WebGLTexture* texture = nullptr;
    GLenum error;
    std::tie(error, texture) = CheckTextureInfo(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL compressedTexSubImage2D error %{public}u", error);
        SetError(error);
    }

    glCompressedTexSubImage2D(imgArg->target, imgArg->level, imgArg->xOffset, imgArg->yOffset, imgArg->width,
        imgArg->height, imgArg->format, imageSize, reinterpret_cast<void*>(offset));
    LOGE("WebGL compressedTexSubImage2D error %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CopyTexImage2D(napi_env env, const CopyTexImage2DArg* imgArg)
{
    LOGI("WebGL copyTexImage2D target %{public}u %{public}d %{public}u ", imgArg->target, imgArg->level,
        imgArg->internalFormat);
    LOGI("WebGL copyTexImage2D target %{public}d %{public}d %{public}d %{public}d %{public}d", imgArg->x, imgArg->y,
        imgArg->width, imgArg->height, imgArg->border);

    WebGLTexture* texture = nullptr;
    GLenum error;
    std::tie(error, texture) = CheckTextureInfo(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL copyTexImage2D error %{public}u", error);
        SetError(error);
    }

    glCopyTexImage2D(imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->x, imgArg->y, imgArg->width,
        imgArg->height, imgArg->border);
    LOGE("WebGL copyTexImage2D error %{public}u", GetError_());
    return nullptr;
}
napi_value WebGLRenderingContextBaseImpl::CopyTexSubImage2D(napi_env env, const CopyTexSubImage2DArg* imgArg)
{
    LOGI("WebGL copyTexSubImage2D target %{public}u %{public}d %{public}d %{public}d", imgArg->target, imgArg->level,
        imgArg->xOffset, imgArg->yOffset);
    LOGI("WebGL copyTexSubImage2D target %{public}d %{public}d %{public}d %{public}d ", imgArg->x, imgArg->y,
        imgArg->width, imgArg->height);

    WebGLTexture* texture = nullptr;
    GLenum error;
    std::tie(error, texture) = CheckTextureInfo(env, imgArg);
    if (error != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL copyTexSubImage2D error %{public}u", error);
        SetError(error);
    }

    glCopyTexSubImage2D(imgArg->target, imgArg->level, imgArg->xOffset, imgArg->yOffset, imgArg->x, imgArg->y,
        imgArg->width, imgArg->height);
    LOGE("WebGL copyTexSubImage2D error %{public}u", GetError_());
    return nullptr;
}

bool WebGLRenderingContextBaseImpl::CheckTextureLevel(GLenum target, GLint level)
{
    if ((level < 0) || level > WebGLTexture::GetMaxTextureLevelForTarget(target)) {
        return false;
    }
    return true;
}

std::tuple<GLenum, WebGLTexture*> WebGLRenderingContextBaseImpl::CheckTextureInfo(
    napi_env env, const TexImageArg* textureInfo)
{
    if (!CheckTextureLevel(textureInfo->target, textureInfo->level)) {
        return std::make_tuple(WebGLRenderingContextBase::INVALID_VALUE, nullptr);
    }

    if (textureInfo->border) {
        return std::make_tuple(WebGLRenderingContextBase::INVALID_VALUE, nullptr);
    }

    WebGLTexture* texture = GetBoundTexture(env, textureInfo->target, true);
    if (!texture) {
        LOGE("Can not find texture ");
        return std::make_tuple(WebGLRenderingContextBase::INVALID_ENUM, nullptr);
    }

    GLenum internalFormat = 0;
    if (textureInfo->internalFormat == 0) {
        internalFormat = texture->GetInternalFormat(textureInfo->target, textureInfo->level);
    }

    GLenum result = WebGLTexture::CheckTextureFormatAndType(textureInfo, IsHighWebGL());
    if (result) {
        LOGE("Invalid format or type %{public}u", result);
        return std::make_tuple(result, nullptr);
    }

    result = CheckTexFuncDimensions(textureInfo);
    if (result) {
        LOGE("Invalid texture dimension or type %{public}u", result);
        return std::make_tuple(result, nullptr);
    }
    return std::make_tuple(WebGLRenderingContextBase::NO_ERROR, texture);
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
    napi_env env, const TexSubImage2DArg* textureInfo, WebGLTexture* texture)
{
    if (!CheckSettableTexFormat(textureInfo->format)) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    if (textureInfo->xOffset < 0 || textureInfo->yOffset < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    // Before checking if it is in the range, check if overflow happens first.
    if (textureInfo->xOffset + textureInfo->width < 0 || textureInfo->yOffset + textureInfo->height < 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (textureInfo->xOffset + textureInfo->width > texture->GetWidth(textureInfo->target, textureInfo->level) ||
        textureInfo->yOffset + textureInfo->height > texture->GetHeight(textureInfo->target, textureInfo->level)) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    if (!IsHighWebGL() && texture->GetType(textureInfo->target, textureInfo->level) != textureInfo->type) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGLRenderingContextBaseImpl::CheckTexImageInternalFormat(int func, GLenum internalFormat)
{
    switch (func) {
        case IMAGE_2D_FUNC_COMPRESSED_TEX_IMAGE_2D:
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

GLenum WebGLRenderingContextBaseImpl::CheckTexFuncDimensions(const TexImageArg* textureInfo)
{
    if (textureInfo->width < 0 || textureInfo->height < 0) {
        LOGE("Invalid offset or size ");
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    switch (textureInfo->target) {
        case GL_TEXTURE_2D:
            if (textureInfo->width > (maxTextureSize >> textureInfo->level) ||
                textureInfo->height > (maxTextureSize >> textureInfo->level)) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            if (textureInfo->func != IMAGE_2D_FUNC_SUB_IMAGE_2D && textureInfo->width != textureInfo->height) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            if (textureInfo->width > (maxTextureSize >> textureInfo->level)) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            break;
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

static bool CheckTexDimension(GLint level, GLsizei width, GLsizei height)
{
    bool widthValid = (level && width == 1) || (level && width == 2) || !(width % 4);     // 4 kBlockWidth
    bool heightValid = (level && height == 1) || (level && height == 2) || !(height % 4); // 4 kBlockWidth
    return widthValid && heightValid;
}

GLenum WebGLRenderingContextBaseImpl::CheckCompressedTexDimensions(const TexImageArg* textureInfo, size_t dataLen)
{
    bool widthValid = true;
    bool heightValid = true;
    size_t bytesRequired = 0;

    switch (textureInfo->format) {
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

            const int index = (textureInfo->format < GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR)
                                  ? (int)textureInfo->format - GL_COMPRESSED_RGBA_ASTC_4x4_KHR
                                  : (int)textureInfo->format - GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;

            const int kBlockSize = 16;
            const int kBlockWidth = WebGLCompressedTextureASTC::kBlockSizeCompressASTC[index].blockWidth;
            const int kBlockHeight = WebGLCompressedTextureASTC::kBlockSizeCompressASTC[index].blockHeight;

            int numBlocksAcross = (textureInfo->width + kBlockWidth - 1) / kBlockWidth;
            int numBlocksDown = (textureInfo->height + kBlockHeight - 1) / kBlockHeight;
            int numBlocks = numBlocksAcross * numBlocksDown;
            bytesRequired = numBlocks * kBlockSize;
            break;
        }
#endif
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: {
            widthValid = CheckTexDimension(textureInfo->level, textureInfo->width, textureInfo->height);

            const int kBlockWidth = 4;
            const int kBlockHeight = 4;
            const int kBlockSize = 8;
            int numBlocksAcross = (textureInfo->width + kBlockWidth - 1) / kBlockWidth;
            int numBlocksDown = (textureInfo->height + kBlockHeight - 1) / kBlockHeight;
            int numBlocks = numBlocksAcross * numBlocksDown;
            bytesRequired = numBlocks * kBlockSize;
            break;
        }
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: {
            widthValid = CheckTexDimension(textureInfo->level, textureInfo->width, textureInfo->height);
            const int kBlockWidth = 4;
            const int kBlockHeight = 4;
            const int kBlockSize = 16;
            int numBlocksAcross = (textureInfo->width + kBlockWidth - 1) / kBlockWidth;
            int numBlocksDown = (textureInfo->height + kBlockHeight - 1) / kBlockHeight;
            int numBlocks = numBlocksAcross * numBlocksDown;
            bytesRequired = numBlocks * kBlockSize;
            break;
        }
        // case GC3D_COMPRESSED_ATC_RGB_AMD:
        case GL_ETC1_RGB8_OES: {
            widthValid = CheckTexDimension(textureInfo->level, textureInfo->width, textureInfo->height);
            bytesRequired = floor(static_cast<double>((textureInfo->width + 3) / 4)) *
                            floor(static_cast<double>((textureInfo->height + 3) / 4)) * 8;
            break;
        }
        /*
        case GC3D_COMPRESSED_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        case GC3D_COMPRESSED_ATC_RGBA_INTERPOLATED_ALPHA_AMD: {
            widthValid = CheckTexDimension(textureInfo->level, textureInfo->width, textureInfo->height);
            bytesRequired = floor(static_cast<double>((textureInfo->width + 3) / 4)) *
                            floor(static_cast<double>((textureInfo->height + 3) / 4)) * 16;
            break;
        }
        */
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG: {
            widthValid = (textureInfo->width & (textureInfo->width - 1)) == 0;
            heightValid = (textureInfo->height & (textureInfo->height - 1)) == 0;
            bytesRequired = (max(textureInfo->width, 8) * max(textureInfo->height, 8) * 4 + 7) / 8;
            break;
        }
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: {
            widthValid = (textureInfo->width & (textureInfo->width - 1)) == 0;
            heightValid = (textureInfo->height & (textureInfo->height - 1)) == 0;
            bytesRequired = (max(textureInfo->width, 16) * max(textureInfo->height, 8) * 2 + 7) / 8;
            break;
        }
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }

    if (!widthValid || !heightValid) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    if (dataLen != bytesRequired) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

} // namespace Impl
} // namespace Rosen
} // namespace OHOS
