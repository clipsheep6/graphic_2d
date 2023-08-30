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

#ifndef WEBGL_BUFFER_DATA_H_OBJECT_SOURCE_H
#define WEBGL_BUFFER_DATA_H_OBJECT_SOURCE_H

#include <vector>

#include "securec.h"
#include "../../../common/napi/n_exporter.h"
#include "../../../common/napi/n_val.h"
#include "../../../common/napi/n_func_arg.h"    // for NFuncArg, NARG_CNT, ZERO
namespace OHOS {
namespace Rosen {
using BufferDataType = enum {
    buffer_data_int8 = napi_int8_array,
    buffer_data_uint8 = napi_uint8_array,
    buffer_data_uint8_clamped = napi_uint8_clamped_array,
    buffer_data_int16 = napi_int16_array,
    buffer_data_uint16 = napi_uint16_array,
    buffer_data_int32 = napi_int32_array,
    buffer_data_uint32 = napi_uint32_array,
    buffer_data_float32 = napi_float32_array,
    buffer_data_float64 = napi_float64_array,
    buffer_data_bigint64 = napi_bigint64_array,
    buffer_data_biguint64 = napi_biguint64_array,
    buffer_data_boolean,
    buffer_data_glenum,

};

using BufferType = enum {
    buffer_dataview,
    buffer_arraybuffer,
    buffer_typedarray,
    buffer_array,
};

class CommBufferData {
public:
    CommBufferData(napi_env env) : env_(env) {}
    ~CommBufferData() {
    }

    void DumpBuffer(BufferDataType destDataType);
    BufferDataType GetBufferDataType() { return type_; }
    BufferType GetBufferType() { return bufferType_; }
    size_t GetBufferLength() { return dataLen_; }

protected:
    CommBufferData(const CommBufferData&) = delete;
    CommBufferData& operator=(const CommBufferData&) = delete;

    template<class T> void DumpDataBuffer(const char *format);
    napi_env env_;
    uint8_t *data_ = { nullptr };
    BufferDataType type_ { buffer_data_uint8 };
    size_t dataLen_ { 0 };
    BufferType bufferType_ { buffer_array };
    std::vector<uint8_t> buffer_ { };
};

class ReadBufferData : public CommBufferData {
public:
    ReadBufferData(napi_env env) : CommBufferData(env) {}
    ~ReadBufferData() {
    }

    napi_status GetBufferData(napi_value data, BufferDataType destDataType = buffer_data_float32);
    uint8_t *GetBuffer() { return data_; }

private:
    ReadBufferData(const ReadBufferData&) = delete;
    ReadBufferData& operator=(const ReadBufferData&) = delete;

    template<class srcT, class dstT>
    napi_status GetArrayElement(napi_value data,
        napi_status (*getElementValue)(napi_env env, napi_value element, srcT *value));
    napi_status GetArrayData(napi_value data, BufferDataType destDataType);
};

class WriteBufferData : public CommBufferData {
public:
    WriteBufferData(napi_env env) : CommBufferData(env) {}
    ~WriteBufferData() {
        if (data_ != nullptr) {
            delete []data_;
        }
    }

    uint8_t *AllocBuffer(uint32_t size) {
        if (data_ != nullptr) {
            delete []data_;
            data_ = nullptr;
        }
        //buffer_.resize(size);
        dataLen_ = size;
        //data_ = buffer_.data();
        data_ = new uint8_t[dataLen_];
        return data_;
    }

    int SetBufferData(uint8_t *data, uint32_t dataLen)
    {
        if (data_ != nullptr) {
            delete []data_;
            data_ = nullptr;
        }
        //buffer_.resize(dataLen);
        dataLen_ = dataLen;
        //data_ = buffer_.data();
        data_ = new uint8_t[dataLen_];
        return memcpy_s(data_, dataLen, data, dataLen);
    }
    napi_value ToExternalArray(BufferDataType dstDataType);
    napi_value ToNormalArray(BufferDataType srcDataType, BufferDataType dstDataType);
private:
    template<class dstT>
    napi_value genExternalArray();
    template<class srcT, class dstT>
    napi_value genNormalArray(napi_status (*getNApiValue)(napi_env env, dstT value, napi_value *element));

    WriteBufferData(const WriteBufferData&) = delete;
    WriteBufferData& operator=(const WriteBufferData&) = delete;
};

union WebGLArgValue
{
    double argDouble;
    bool argBool;
    uint8_t argUint8;
    uint16_t argUint16;
    uint32_t argUint32;
    float argFloat;
    uint64_t argUint64;
    GLenum argEnum;
};

struct WebGLArgFunc {
    BufferDataType type;
    uint32_t index;
};

class WebGLArg {
public:
    WebGLArg(napi_env env) : env_(env) {}
    ~WebGLArg() {}

    std::tuple<bool, uint32_t> GetWebGLArgs(
        const NFuncArg &funcArg, WebGLArgValue args[], std::vector<WebGLArgFunc> funcs);
    bool GetWebGLArg(napi_value data, WebGLArgValue &args, const WebGLArgFunc &func);
private:
    napi_env env_;
    WebGLArg(const WebGLArg&) = delete;
    WebGLArg& operator=(const WebGLArg&) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_BUFFER_DATA_H_OBJECT_SOURCE_H
