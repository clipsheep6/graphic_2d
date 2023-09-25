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

#include "webgl/webgl_arg.h"

#include "context/webgl_rendering_context_base.h"
#include "util/util.h"
#include "image_source.h"

namespace OHOS {
namespace Rosen {
using namespace std;
using namespace OHOS::Media;

bool WebGLArg::GetWebGLArg(napi_value data, WebGLArgValue& arg, const WebGLArgInfo& func)
{
    bool succ = true;
    switch (func.argType) {
        case ARG_BOOL: {
            tie(succ, arg.gl_bool) = NVal(env_, data).ToBool();
            break;
        }
        case ARG_UINT: {
            tie(succ, arg.gl_uint) = NVal(env_, data).ToUint32();
            break;
        }
        case ARG_INT: {
            tie(succ, arg.gl_int) = NVal(env_, data).ToInt32();
            break;
        }
        case ARG_FLOAT: {
            double v;
            tie(succ, v) = NVal(env_, data).ToDouble();
            arg.gl_float = v;
            break;
        }
        case ARG_ENUM: {
            tie(succ, arg.gl_enum) = NVal(env_, data).ToGLenum();
            break;
        }
        case ARG_INTPTR: {
            int64_t v = 0;
            tie(succ, v) = NVal(env_, data).ToInt64();
            arg.gl_intptr = static_cast<GLintptr>(v);
            break;
        }
        case ARG_SIZE: {
            int64_t v = 0;
            tie(succ, v) = NVal(env_, data).ToInt64();
            arg.gl_sizei = static_cast<GLsizei>(v);
            break;
        }
        case ARG_SIZEPTR: {
            int64_t v = 0;
            tie(succ, v) = NVal(env_, data).ToInt64();
            arg.gl_sizeptr = static_cast<GLsizei>(v);
            break;
        }
        default:
            break;
    }
    return succ;
}

bool WebGLArg::GetWebGLArgs(const NFuncArg& funcArg, uint32_t maxIndex, const std::vector<WebGLArgInfo>& funcs)
{
    values.resize(maxIndex + 1);
    bool succ = true;
    for (size_t i = 0; i < funcs.size(); i++) {
        if (funcs[i].argIndex > values.size()) {
            LOGE("Invalid arg %{public}s %{public}u", funcs[i].argName, funcs[i].argIndex);
            continue;
        }
        if (funcArg[funcs[i].argIndex] == nullptr) {
            values[funcs[i].argIndex].gl_sizei = 0;
            continue;
        }
        succ = GetWebGLArg(funcArg[funcs[i].argIndex], values[funcs[i].argIndex], funcs[i]);
        if (!succ) {
            LOGE("Failed to get arg %{public}s", funcs[i].argName);
            break;
        }
    }
    return succ;
}

std::tuple<GLenum, GLintptr> WebGLArg::ToGLintptr(napi_env env, napi_value dataObj)
{
    int64_t data = 0;
    bool succ;
    tie(succ, data) = NVal(env, dataObj).ToInt64();
    if (!succ) {
        return make_tuple(WebGLRenderingContextBase::INVALID_VALUE, 0);
    }
    if (data < 0) {
        return make_tuple(WebGLRenderingContextBase::INVALID_VALUE, 0);
    }
    if (data > static_cast<int64_t>(std::numeric_limits<int>::max())) {
        return make_tuple(WebGLRenderingContextBase::INVALID_OPERATION, 0);
    }
    return make_tuple(WebGLRenderingContextBase::NO_ERROR, static_cast<GLintptr>(data));
}

template<class T>
void WebGLCommBuffer::DumpDataBuffer(const char* format) const
{
    LOGI("WebGL WebGLCommBuffer dataLen_ %{public}u ", dataLen_);
    size_t count = dataLen_ / sizeof(T);
    size_t i = 0;
    size_t max = count > MAX_DUMP ? MAX_DUMP : count;
    for (; i < max; i++) {
        LOGI(format, i, *reinterpret_cast<T*>(data_ + i * sizeof(T)));
    }

    if (count <= MAX_DUMP) {
        return;
    }
    if (count > MAX_DUMP) {
        i = count - MAX_DUMP;
    }
    for (; i < count; i++) {
        LOGI(format, i, *reinterpret_cast<T*>(data_ + i * sizeof(T)));
    }
}

void WebGLCommBuffer::DumpBuffer(BufferDataType destDataType) const
{
    switch (destDataType) {
        case BUFFER_DATA_BOOLEAN: {
            DumpDataBuffer<bool>("WebGL WebGLCommBuffer bool elm %{public}u %{public}d");
            break;
        }
        case BUFFER_DATA_INT_8:
        case BUFFER_DATA_UINT_8:
        case BUFFER_DATA_UINT_8_CLAMPED: {
            DumpDataBuffer<uint8_t>("WebGL WebGLCommBuffer int8 elm %{public}u %{public}d");
            break;
        }
        case BUFFER_DATA_INT_16:
        case BUFFER_DATA_UINT_16: {
            DumpDataBuffer<uint16_t>("WebGL WebGLCommBuffer int16 elm %{public}u %{public}u");
            break;
        }
        case BUFFER_DATA_INT_32:
        case BUFFER_DATA_UINT_32: {
            DumpDataBuffer<uint32_t>("WebGL WebGLCommBuffer int32 elm %{public}u %{public}u");
            break;
        }
        case BUFFER_DATA_FLOAT_32: {
            DumpDataBuffer<float>("WebGL WebGLCommBuffer float elm %{public}u %{public}f");
            break;
        }
        case BUFFER_DATA_FLOAT_64: {
            DumpDataBuffer<double>("WebGL WebGLCommBuffer float64 elm %{public}u %{public}lf");
            break;
        }
        case BUFFER_DATA_BIGINT_64:
        case BUFFER_DATA_BIGUINT_64: {
            DumpDataBuffer<uint64_t>("WebGL WebGLCommBuffer int64 elm %{public}u %{public}u");
            break;
        }
        case BUFFER_DATA_GLENUM: {
            DumpDataBuffer<GLenum>("WebGL WebGLCommBuffer glenum elm %{public}u %{public}x");
            break;
        }
        default:
            break;
    }
}

size_t WebGLCommBuffer::GetBufferDataSize() const
{
    switch (type_) {
        case BUFFER_DATA_BOOLEAN: {
            return sizeof(bool);
        }
        case BUFFER_DATA_INT_8:
        case BUFFER_DATA_UINT_8:
        case BUFFER_DATA_UINT_8_CLAMPED: {
            return sizeof(uint8_t);
        }
        case BUFFER_DATA_INT_16:
        case BUFFER_DATA_UINT_16: {
            return sizeof(uint16_t);
        }
        case BUFFER_DATA_INT_32:
        case BUFFER_DATA_FLOAT_32:
        case BUFFER_DATA_UINT_32: {
            return sizeof(uint32_t);
        }
        case BUFFER_DATA_FLOAT_64: {
            return sizeof(double);
        }
        case BUFFER_DATA_BIGINT_64:
        case BUFFER_DATA_BIGUINT_64: {
            return sizeof(uint64_t);
        }
        case BUFFER_DATA_GLENUM: {
            return sizeof(GLenum);
        }
        default:
            break;
    }
    return sizeof(uint32_t);
}

template<class srcT, class dstT>
napi_status WebGLReadBufferArg::GetArrayElement(
    napi_value data, dstT defaultValue, napi_status (*getElementValue)(napi_env env_, napi_value element, srcT* value))
{
    uint32_t length;
    napi_status status = napi_get_array_length(env_, data, &length);
    if (status != napi_ok) {
        return status;
    }
    LOGI("WebGL GetArrayData array len %{public}u ", length);
    if (length == 0) {
        return napi_ok;
    }
    buffer_.resize(length * sizeof(dstT));
    data_ = buffer_.data();
    uint8_t* buffer = buffer_.data();
    for (uint32_t i = 0; i < length; i++) {
        napi_value element;
        status = napi_get_element(env_, data, i, &element);
        if (status != napi_ok) {
            return status;
        }
        srcT elm = 0;
        if (NVal(env_, element).IsUndefined()) {
            *reinterpret_cast<dstT*>(buffer) = defaultValue;
            buffer += sizeof(dstT);
            continue;
        }

        if (NVal(env_, element).IsNull()) {
            *reinterpret_cast<dstT*>(buffer) = elm;
            buffer += sizeof(dstT);
            continue;
        }
        if ((status = getElementValue(env_, element, &elm)) != napi_ok) {
            return status;
        }
        *reinterpret_cast<dstT*>(buffer) = elm;
        buffer += sizeof(dstT);
    }
    LOGI("WebGL GetArrayData array %{public}p data_  %{public}p ", buffer, data_);
    dataLen_ = length * sizeof(dstT);
    return napi_ok;
}

napi_status WebGLReadBufferArg::GetArrayData(napi_value data, BufferDataType destDataType)
{
    napi_status status = napi_invalid_arg;
    switch (destDataType) {
        case BufferDataType::BUFFER_DATA_BOOLEAN:
            status = GetArrayElement<bool, bool>(data, false, napi_get_value_bool);
            break;
        case BufferDataType::BUFFER_DATA_INT_8:
        case BufferDataType::BUFFER_DATA_UINT_8:
        case BufferDataType::BUFFER_DATA_UINT_8_CLAMPED:
            status = GetArrayElement<uint32_t, uint8_t>(data, 0, napi_get_value_uint32);
            break;
        case BufferDataType::BUFFER_DATA_INT_16:
        case BufferDataType::BUFFER_DATA_UINT_16:
            status = GetArrayElement<uint32_t, uint16_t>(data, 0, napi_get_value_uint32);
            break;
        case BufferDataType::BUFFER_DATA_INT_32:
        case BufferDataType::BUFFER_DATA_UINT_32:
            status = GetArrayElement<uint32_t, uint32_t>(data, 0, napi_get_value_uint32);
            break;
        case BufferDataType::BUFFER_DATA_FLOAT_32: {
            float def = 0;
            (void)memset_s(&def, sizeof(def), 0xff, sizeof(def));
            status = GetArrayElement<double, float>(data, def, napi_get_value_double);
            break;
        }
        case BufferDataType::BUFFER_DATA_FLOAT_64:
            status = GetArrayElement<double, double>(data, 0, napi_get_value_double);
            break;
        case BufferDataType::BUFFER_DATA_BIGINT_64:
        case BufferDataType::BUFFER_DATA_BIGUINT_64:
            status = GetArrayElement<int64_t, uint64_t>(data, 0, napi_get_value_int64);
            break;
        case BufferDataType::BUFFER_DATA_GLENUM:
            status = GetArrayElement<int64_t, GLenum>(data, 0, napi_get_value_int64);
            break;
        default:
            break;
    }
    if (status != napi_ok) {
        return status;
    }
    type_ = destDataType;
    data_ = buffer_.data();
    return napi_ok;
}

// destDataType only for BUFFER_ARRAY
napi_status WebGLReadBufferArg::GenBufferData(napi_value data, BufferDataType destDataType)
{
    if (data == nullptr) {
        return napi_invalid_arg;
    }
    bool type = false;
    napi_status status = napi_ok;
    napi_is_dataview(env_, data, &type);
    LOGI("WebGL WebGLReadBufferArg isDataView %{public}u", type);
    if (type) {
        bufferType_ = BUFFER_DATA_VIEW;
        napi_value arraybuffer = nullptr;
        size_t byteOffset = 0;
        type_ = BUFFER_DATA_UINT_8;
        return napi_get_dataview_info(
            env_, data, &dataLen_, reinterpret_cast<void**>(&data_), &arraybuffer, &byteOffset);
    }
    napi_is_arraybuffer(env_, data, &type);
    LOGI("WebGL WebGLReadBufferArg isArrayBuffer %{public}u", type);
    if (type) {
        bufferType_ = BUFFER_ARRAY_BUFFER;
        type_ = BUFFER_DATA_UINT_8;
        return napi_get_arraybuffer_info(env_, data, reinterpret_cast<void**>(&data_), &dataLen_);
    }
    napi_is_typedarray(env_, data, &type);
    LOGI("WebGL WebGLReadBufferArg typedarray %{public}u", type);
    if (type) {
        bufferType_ = BUFFER_TYPED_ARRAY;
        napi_value arraybuffer = nullptr;
        size_t byteOffset;
        napi_typedarray_type typedarray;
        status =
            napi_get_typedarray_info(env_, data, &typedarray, &dataLen_, (void**)(&data_), &arraybuffer, &byteOffset);
        type_ = (BufferDataType)typedarray;
        LOGI("WebGL WebGLReadBufferArg data type_ %{public}u", type_);
    } else {
        napi_is_array(env_, data, &type);
        LOGI("WebGL WebGLReadBufferArg array %{public}u destDataType %{public}u", type, destDataType);
        if (type) {
            bufferType_ = BUFFER_ARRAY;
            status = GetArrayData(data, destDataType);
        } else {
            return napi_invalid_arg;
        }
    }
    return status;
}

template<class dstT>
napi_value WebGLWriteBufferArg::genExternalArray()
{
    LOGI("genExternalArray dataLen_[%{public}u  %{public}u]", dataLen_, type_);
    if (data_ == nullptr) {
        return nullptr;
    }
    napi_value outputBuffer = nullptr;
    size_t count = dataLen_ / sizeof(dstT);
    napi_value outputArray = nullptr;
    napi_status status = napi_create_external_arraybuffer(
        env_, data_, dataLen_,
        [](napi_env env_, void* finalize_data, void* finalize_hint) {
            LOGI("genExternalArray free %{public}p", finalize_data);
            delete[] reinterpret_cast<uint8_t*>(finalize_data);
        },
        nullptr, &outputBuffer);
    if (status == napi_ok) {
        status = napi_create_typedarray(env_, (napi_typedarray_type)type_, count, outputBuffer, 0, &outputArray);
    }
    if (status != napi_ok) {
        LOGI("genExternalArray error %{public}d", status);
    }
    data_ = nullptr;
    return outputArray;
}

napi_value WebGLWriteBufferArg::ToExternalArray(BufferDataType dstDataType)
{
    type_ = dstDataType;
    switch (dstDataType) {
        case BUFFER_DATA_BOOLEAN: {
            return genExternalArray<bool>();
        }
        case BUFFER_DATA_INT_8:
        case BUFFER_DATA_UINT_8:
        case BUFFER_DATA_UINT_8_CLAMPED: {
            return genExternalArray<uint8_t>();
        }
        case BUFFER_DATA_INT_16:
        case BUFFER_DATA_UINT_16: {
            return genExternalArray<uint16_t>();
        }
        case BUFFER_DATA_INT_32:
        case BUFFER_DATA_UINT_32: {
            return genExternalArray<uint32_t>();
        }
        case BUFFER_DATA_FLOAT_32: {
            return genExternalArray<float>();
        }
        case BUFFER_DATA_FLOAT_64: {
            return genExternalArray<double>();
        }
        case BUFFER_DATA_BIGINT_64:
        case BUFFER_DATA_BIGUINT_64: {
            return genExternalArray<uint64_t>();
        }
        default:
            break;
    }
    return NVal::CreateNull(env_).val_;
}

template<class srcT, class dstT>
napi_value WebGLWriteBufferArg::genNormalArray(
    napi_status (*getNApiValue)(napi_env env_, dstT value, napi_value* element))
{
    napi_value res = nullptr;
    size_t count = dataLen_ / sizeof(srcT);
    napi_create_array_with_length(env_, count, &res);
    for (size_t i = 0; i < count; i++) {
        srcT data = *reinterpret_cast<srcT*>(data_ + i * sizeof(srcT));
        napi_value element;
        getNApiValue(env_, static_cast<dstT>(data), &element);
        napi_set_element(env_, res, i, element);
    }
    return res;
}

napi_value WebGLWriteBufferArg::ToNormalArray(BufferDataType srcDataType, BufferDataType dstDataType)
{
    type_ = dstDataType;
    if (dstDataType == BUFFER_DATA_BOOLEAN) {
        if (srcDataType == BUFFER_DATA_BOOLEAN) {
            return genNormalArray<bool, bool>(napi_get_boolean);
        } else if (srcDataType == BUFFER_DATA_INT_32 || srcDataType == BUFFER_DATA_UINT_32) {
            return genNormalArray<uint32_t, bool>(napi_get_boolean);
        }
        return NVal::CreateNull(env_).val_;
    }

    if (dstDataType == BUFFER_DATA_INT_32 || dstDataType == BUFFER_DATA_UINT_32) {
        if (srcDataType == BUFFER_DATA_INT_32 || srcDataType == BUFFER_DATA_UINT_32) {
            return genNormalArray<uint32_t, uint32_t>(napi_create_uint32);
        }
        return NVal::CreateNull(env_).val_;
    }

    if (dstDataType == BUFFER_DATA_BIGINT_64 || dstDataType == BUFFER_DATA_BIGUINT_64) {
        if (srcDataType == BUFFER_DATA_INT_32 || srcDataType == BUFFER_DATA_UINT_32) {
            return genNormalArray<uint32_t, int64_t>(napi_create_int64);
        }
        return NVal::CreateNull(env_).val_;
    }

    if (srcDataType == BUFFER_DATA_FLOAT_32) {
        if (srcDataType == BUFFER_DATA_FLOAT_32) {
            return genNormalArray<float, double>(napi_create_double);
        }
    }
    return NVal::CreateNull(env_).val_;
}

bool WebGLImageSource::HandleImageSource()
{
    ImageInfo imageInfo;
    uint32_t errorCode = imageSource_->GetImageInfo(0, imageInfo);
    if (errorCode == 0) {
        errorCode = imageSource_->GetImageInfo(imageInfo);
        LOGE("WebGl ImageSource [%{public}u %{public}u]", imageInfo.size.width, imageInfo.size.height);
        width_ = imageInfo.size.width;
        height_ = imageInfo.size.height;
    }

    DecodeOptions decodeOpts;
    pixelMap_ = imageSource_->CreatePixelMap(decodeOpts, errorCode);
    if (pixelMap_ == nullptr) {
        LOGE("WebGl ImageSource fail to create pixel map");
        return false;
    }
    LOGE("WebGl ImageSource Width [%{public}u %{public}u] ", pixelMap_->GetWidth(), pixelMap_->GetHeight());
    LOGE("WebGl ImageSource GetByteCount %{public}u ", pixelMap_->GetByteCount());
    LOGE("WebGl ImageSource GetPixelBytes %{public}u ", pixelMap_->GetPixelBytes());
    LOGE("WebGl ImageSource GetRowBytes %{public}u ", pixelMap_->GetRowBytes());
    return true;
}

bool WebGLImageSource::HandleImageSourceData(napi_env env, napi_value resultData, GLuint offset)
{
    uint32_t errorCode = 0;
    bool succ = false;
    bool userBuff = false;
    SourceOptions opts;
    WebGLReadBufferArg readBuffer(env);
    napi_valuetype valueType;
    napi_typeof(env, resultData, &valueType);
    if (valueType == napi_string) { // File Path
        size_t ignore = 0;
        std::unique_ptr<char[]> source = nullptr;
        tie(succ, source, ignore) = NVal(env, resultData).ToUTF8String();
        if (!succ) {
            return false;
        }
        LOGE("WebGl ImageSource src: %{public}s", source.get());
        imageSource_ = ImageSource::CreateImageSource(source.get(), opts, errorCode);
    } else if (valueType == napi_number) { // Fd
        int fd = 0;
        napi_get_value_int32(env, resultData, &fd);
        LOGI("WebGl ImageSource fdIndex is [%{public}d]", fd);
        imageSource_ = ImageSource::CreateImageSource(fd, opts, errorCode);
    } else { // Input Buffer
        napi_status status = readBuffer.GenBufferData(resultData);
        if (status != napi_ok) {
            return false;
        }
        LOGI("WebGl ImageSource sourceBufferSize is [%{public}zu]", readBuffer.GetBufferLength());
        imageSource_ = ImageSource::CreateImageSource(readBuffer.GetBuffer() + offset,
            static_cast<GLuint>(readBuffer.GetBufferLength()) - offset, opts, errorCode);
        userBuff = true;
    }
    if (imageSource_ == nullptr) {
        LOGE("WebGl ImageSource fail to get errorCode %{public}u", errorCode);
        return false;
    }
    return HandleImageSource();
}

template<class T>
tuple<bool, T> WebGLImageSource::GetObjectIntField(napi_env env, napi_value resultObject, const std::string& name)
{
    napi_value result = nullptr;
    napi_status status = napi_get_named_property(env, resultObject, name.c_str(), &result);
    if (status != napi_ok) {
        return make_tuple(false, 0);
    }
    int64_t res = 0;
    status = napi_get_value_int64(env, result, &res);
    return make_tuple(status == napi_ok, static_cast<T>(res));
}

bool WebGLImageSource::GenImageSource(napi_env env, GLsizei width, GLsizei height, napi_value pixels)
{
    width_ = width;
    height_ = height;
    bufferDataArg_ = std::make_unique<WebGLReadBufferArg>(env);
    if (bufferDataArg_ == nullptr) {
        return false;
    }
    napi_status status = bufferDataArg_->GenBufferData(pixels);
    if (status != napi_ok) {
        return false;
    }
    return true;
}

bool WebGLImageSource::GenImageSource(napi_env env, napi_value sourceImage)
{
    bool succ = false;
    napi_value resultObject = nullptr;
    napi_status status = napi_coerce_to_object(env, sourceImage, &resultObject);
    if (status != napi_ok) {
        return false;
    }

    // image maybe no width
    tie(succ, width_) = GetObjectIntField<GLsizei>(env, resultObject, "width");
    tie(succ, height_) = GetObjectIntField<GLsizei>(env, resultObject, "height");
    LOGI("WebGl GenImageSource size [%{public}d %{public}d]", width_, height_);

    napi_value resultData = nullptr;
    status = napi_get_named_property(env, resultObject, "data", &resultData);
    if (status != napi_ok || NVal(env, resultData).IsNull()) {
        status = napi_get_named_property(env, resultObject, "src", &resultData);
    }
    if (status != napi_ok || NVal(env, resultData).IsNull()) {
        LOGE("WebGl GenImageSource status %{public}u", status);
        return false;
    }
    return HandleImageSourceData(env, resultData, 0);
}

GLvoid* WebGLImageSource::GetImageSourceData(GLenum format, GLenum type, bool flipY, bool premultiplyAlpha)
{
    if (bufferDataArg_) {
        return reinterpret_cast<GLvoid *>(bufferDataArg_->GetBuffer());
    }
    if (pixelMap_ == nullptr) {
        return nullptr;
    }
    if (!(format == GL_RGBA && type == GL_UNSIGNED_BYTE && !flipY && !premultiplyAlpha)) {
        pixelMap_->flip(flipY, premultiplyAlpha);
    }
    return reinterpret_cast<GLvoid *>(const_cast<uint8_t*>(pixelMap_->GetPixels()));
}

uint32_t WebGLArg::GetWebGLdataSize(GLenum type)
{
    switch (type) {
        case GL_BYTE:
            return sizeof(GLbyte);
        case GL_UNSIGNED_BYTE:
            return sizeof(GLubyte);
        case GL_SHORT:
            return sizeof(GLshort);
        case GL_UNSIGNED_SHORT:
            return sizeof(GLushort);
        case GL_INT:
            return sizeof(GLint);
        case GL_UNSIGNED_INT:
            return sizeof(GLuint);
        case GL_FLOAT:
            return sizeof(GLfloat);
        default:
            break;
    }
    return sizeof(GLuint);
}
} // namespace Rosen
} // namespace OHOS
