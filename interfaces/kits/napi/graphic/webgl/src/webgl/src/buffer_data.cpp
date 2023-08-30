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

#include "../include/util/buffer_data.h"
#include "../include/util/log.h"

namespace OHOS {
namespace Rosen {
using namespace std;

template<class T>
void CommBufferData::DumpDataBuffer(const char *format)
{
    LOGI("WebGL CommBufferData dataLen_ %{public}u ", dataLen_);
    for (size_t i = 0; i < dataLen_ / sizeof(T); i++) {
        LOGI(format, *reinterpret_cast<T *>(data_ + i * sizeof(T)));
    }
}

void CommBufferData::DumpBuffer(BufferDataType destDataType)
{
    switch(destDataType) {
        case buffer_data_boolean: {
            DumpDataBuffer<bool>("WebGL CommBufferData elm %{public}d");
            break;
        }
        case buffer_data_int8:
        case buffer_data_uint8:
        case buffer_data_uint8_clamped: {
            DumpDataBuffer<uint8_t>("WebGL CommBufferData elm %{public}d");
            break;
        }
        case buffer_data_int16:
        case buffer_data_uint16: {
            DumpDataBuffer<uint16_t>("WebGL CommBufferData elm %{public}u");
            break;
        }
        case buffer_data_int32:
        case buffer_data_uint32:{
            DumpDataBuffer<uint32_t>("WebGL CommBufferData elm %{public}u");
            break;
        }
        case buffer_data_float32: {
            DumpDataBuffer<float>("WebGL CommBufferData elm %{public}f");
            break;
        }
        case buffer_data_float64: {
            DumpDataBuffer<double>("WebGL CommBufferData elm %{public}lf");
            break;
        }
        case buffer_data_bigint64:
        case buffer_data_biguint64: {
            DumpDataBuffer<uint64_t>("WebGL CommBufferData elm %{public}u");
            break;
        }
        default:
            break;
    }
}

template<class srcT, class dstT>
napi_status ReadBufferData::GetArrayElement(napi_value data,
    napi_status (*getElementValue)(napi_env env, napi_value element, srcT *value))
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
    uint8_t *buffer = buffer_.data();
    for (uint32_t i = 0; i < length; i++) {
        napi_value element;
        status = napi_get_element(env_, data, i, &element);
        if (status != napi_ok) {
            return status;
        }

        srcT elm;
        if ((status = getElementValue(env_, element, &elm)) != napi_ok) {
            return status;
        }
        *reinterpret_cast<dstT *>(buffer) = elm;
        buffer += sizeof(dstT);
    }

    dataLen_ = buffer - data_;
    return napi_ok;
}

napi_status ReadBufferData::GetArrayData(napi_value data, BufferDataType destDataType)
{
    napi_status status = napi_invalid_arg;
    switch(destDataType) {
        case buffer_data_boolean: {
            status = GetArrayElement<bool, bool>(data, napi_get_value_bool);
            break;
        }
        case buffer_data_int8:
        case buffer_data_uint8:
        case buffer_data_uint8_clamped: {
            status = GetArrayElement<uint32_t, uint8_t>(data, napi_get_value_uint32);
            break;
        }
        case buffer_data_int16:
        case buffer_data_uint16: {
            status = GetArrayElement<uint32_t, uint16_t>(data, napi_get_value_uint32);
            break;
        }
        case buffer_data_int32:
        case buffer_data_uint32:{
            status = GetArrayElement<uint32_t, uint32_t>(data, napi_get_value_uint32);
            break;
        }
        case buffer_data_float32: {
            status = GetArrayElement<double, float>(data, napi_get_value_double);
            break;
        }
        case buffer_data_float64: {
            status = GetArrayElement<double, double>(data, napi_get_value_double);
            break;
        }
        case buffer_data_bigint64:
        case buffer_data_biguint64: {
            status = GetArrayElement<int64_t, uint64_t>(data, napi_get_value_int64);
            break;
        }
        case buffer_data_glenum:{
            status = GetArrayElement<int64_t, GLenum>(data, napi_get_value_int64);
            break;
        }
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

// destDataType only for buffer_array
napi_status ReadBufferData::GetBufferData(napi_value data, BufferDataType destDataType)
{
    if (data == nullptr) {
        return napi_invalid_arg;
    }
    bool type = false;
    napi_status status = napi_ok;
    napi_is_dataview(env_, data, &type);
    LOGI("WebGL ReadBufferData isDataView %{public}u", type);
    if (type) {
        bufferType_ = buffer_dataview;
        napi_value arraybuffer = nullptr;
        size_t byte_offset = 0;
        return napi_get_dataview_info(env_, data, &dataLen_, reinterpret_cast<void **>(&data_), &arraybuffer, &byte_offset);
    }
    napi_is_arraybuffer(env_, data, &type);
    LOGI("WebGL ReadBufferData isArrayBuffer %{public}u", type);
    if (type) {
        bufferType_ = buffer_arraybuffer;
        type_ = buffer_data_float32; // default float
        return napi_get_arraybuffer_info(env_, data, reinterpret_cast<void **>(&data_), &dataLen_);
    }
    napi_is_typedarray(env_, data, &type);
    LOGI("WebGL ReadBufferData typedarray %{public}u", type);
    if (type) {
        bufferType_ = buffer_typedarray;
        napi_value arraybuffer = nullptr;
        size_t byte_offset;
        napi_typedarray_type typedarray;
        status = napi_get_typedarray_info(env_, data, &typedarray, &dataLen_,
            (void **)(&data_), &arraybuffer, &byte_offset);
        type_ = (BufferDataType)typedarray;
        LOGI("WebGL ReadBufferData data type_ %{public}u", type_);
    } else {
        napi_is_array(env_, data, &type);
        LOGI("WebGL ReadBufferData array %{public}u destDataType %{public}u", type, destDataType);
        if (type) {
            bufferType_ = buffer_array;
            status = GetArrayData(data, destDataType);
        } else {
            return napi_invalid_arg;
        }
    }
    return status;
}

template<class dstT>
napi_value WriteBufferData::genExternalArray()
{
    LOGI("genExternalArray data_ %{public}p dataLen_[%{public}u  %{public}u]", data_, dataLen_, type_);
    if (data_ == nullptr) {
        return nullptr;
    }
    napi_value outputBuffer = nullptr;
    size_t count = dataLen_ / sizeof(dstT);
    napi_value outputArray = nullptr;
    napi_status status = napi_create_external_arraybuffer(env_, data_, dataLen_,
        [](napi_env env, void *finalize_data, void *finalize_hint) {
            LOGI("genExternalArray free %{public}p", finalize_data);
            delete []reinterpret_cast<uint8_t *>(finalize_data);
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

napi_value WriteBufferData::ToExternalArray(BufferDataType dstDataType)
{
    type_ = dstDataType;
    switch(dstDataType) {
        case buffer_data_boolean: {
            return genExternalArray<bool>();
        }
        case buffer_data_int8:
        case buffer_data_uint8:
        case buffer_data_uint8_clamped: {
            return genExternalArray<uint8_t>();
        }
        case buffer_data_int16:
        case buffer_data_uint16: {
            return genExternalArray<uint16_t>();
        }
        case buffer_data_int32:
        case buffer_data_uint32:{
            return genExternalArray<uint32_t>();
        }
        case buffer_data_float32: {
            return genExternalArray<float>();
        }
        case buffer_data_float64: {
            return genExternalArray<double>();
        }
        case buffer_data_bigint64:
        case buffer_data_biguint64: {
            return genExternalArray<uint64_t>();
        }
        default:
            break;
    }
    return NVal::CreateNull(env_).val_;

}

template<class srcT, class dstT>
napi_value WriteBufferData::genNormalArray(napi_status (*getNApiValue)(napi_env env, dstT value, napi_value *element))
{
    napi_value res = nullptr;
    size_t count = dataLen_ / sizeof(srcT);
    napi_create_array_with_length(env_, count, &res);
    for (size_t i = 0; i < count; i++) {
        srcT data = *reinterpret_cast<srcT *>(data_ + i * sizeof(srcT));
        napi_value element;
        getNApiValue(env_, static_cast<dstT>(data), &element);
        napi_set_element(env_, res, i, element);
    }
    return res;
}

napi_value WriteBufferData::ToNormalArray(BufferDataType srcDataType, BufferDataType dstDataType)
{
    type_ = dstDataType;
    if (dstDataType == buffer_data_boolean) {
        if (srcDataType == buffer_data_boolean) {
            return genNormalArray<bool, bool>(napi_get_boolean);
        } else if (srcDataType == buffer_data_int32 || srcDataType == buffer_data_uint32) {
            return genNormalArray<uint32_t, bool>(napi_get_boolean);
        }
        return NVal::CreateNull(env_).val_;
    }

    if (dstDataType == buffer_data_int32 || dstDataType == buffer_data_uint32) {
        if (srcDataType == buffer_data_int32 || srcDataType == buffer_data_uint32) {
            return genNormalArray<uint32_t, uint32_t>(napi_create_uint32);
        }
        return NVal::CreateNull(env_).val_;
    }

    if (srcDataType == buffer_data_float32) {
        if (srcDataType == buffer_data_float32) {
            return genNormalArray<float, double>(napi_create_double);
        }
    }
    return NVal::CreateNull(env_).val_;
}

bool WebGLArg::GetWebGLArg(napi_value data, WebGLArgValue &arg, const WebGLArgFunc &func)
{
    bool succ = true;
    switch(func.type) {
        case buffer_data_boolean: {
            tie(succ, arg.argBool) = NVal(env_, data).ToBool();
            break;
        }
        case buffer_data_int8:
        case buffer_data_uint8:
        case buffer_data_uint8_clamped:{
            tie(succ, arg.argUint8) = NVal(env_, data).ToInt32();
            break;
        }
        case buffer_data_int16:
        case buffer_data_uint16: {
            tie(succ, arg.argUint16) = NVal(env_, data).ToInt32();
            break;
        }
        case buffer_data_int32:
        case buffer_data_uint32:{
            tie(succ, arg.argUint32) = NVal(env_, data).ToInt32();
            break;
        }
        case buffer_data_float32: {
            tie(succ, arg.argFloat) = NVal(env_, data).ToInt32();
            break;
        }
        case buffer_data_float64: {
            tie(succ, arg.argDouble) = NVal(env_, data).ToInt32();
            break;
        }
        case buffer_data_bigint64:
        case buffer_data_biguint64: {
            tie(succ, arg.argUint64) = NVal(env_, data).ToInt64();
            break;
        }
        case buffer_data_glenum:{
            tie(succ, arg.argEnum) = NVal(env_, data).ToGLenum();
            break;
        }
        default:
            break;
    }
    return succ;
}

std::tuple<bool, uint32_t> WebGLArg::GetWebGLArgs(
    const NFuncArg &funcArg, WebGLArgValue args[], std::vector<WebGLArgFunc> funcs)
{
    bool succ = true;
    uint32_t index = 0;
    for (size_t i = 0; i < funcs.size(); i++) {
        if (funcArg[funcs[i].index] == nullptr) {
            args[funcs[i].index].argUint64 = 0;
            continue;
        }
        succ = GetWebGLArg(funcArg[funcs[i].index], args[funcs[i].index], funcs[i]);
        if (!succ) {
            LOGE("Failed to get arg index %{public}u", funcs[i].index);
            index = funcs[i].index;
            break;
        }
    }
    return make_tuple(succ, index);
}

} // namespace Rosen
} // namespace OHOS
