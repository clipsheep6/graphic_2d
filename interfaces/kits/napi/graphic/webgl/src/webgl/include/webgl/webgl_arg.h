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

#ifndef ROSENRENDER_ROSEN_WEBGL_ARG
#define ROSENRENDER_ROSEN_WEBGL_ARG

#include "napi/n_class.h"      // for NClass
#include "napi/n_func_arg.h"   // for NFuncArg, NARG_CNT, ZERO
#include "napi/n_val.h"              // for NVal
#include "util/log.h"
#include "util/util.h"
#include "context/webgl_rendering_context_basic_base.h"
#include "image_source.h"
#include "securec.h"

namespace OHOS {
namespace Rosen {
enum : int {
    WEBGL_V_1_X = 0,
    WEBGL_V_2_0
};

using WebGLArgType = enum : int {
    ARG_FLOAT,
    ARG_UINT,
    ARG_INT,
    ARG_ENUM,
    ARG_INTPTR,
    ARG_SIZE,
    ARG_BOOL,
    ARG_SIZEPTR,
};

union WebGLArgValue {
    GLboolean   gl_bool;
    GLfloat     gl_float;
    GLuint      gl_uint;
    GLint       gl_int;
    GLenum      gl_enum;
    GLintptr    gl_intptr;
    GLsizei     gl_sizei;
    GLsizeiptr  gl_sizeptr;
};

struct WebGLArgInfo {
    WebGLArgType argType;
    uint32_t argIndex;
    const char *argName;
    WebGLArgInfo(uint32_t index, WebGLArgType type, const char *name) : argType(type), argIndex(index), argName(name) {}
    ~WebGLArgInfo() {}
};

class WebGLArg {
public:
    static const GLsizei MATRIX_2X2_REQUIRE_MIN_SIZE = 4;
    static const GLsizei MATRIX_3X3_REQUIRE_MIN_SIZE = 9;
    static const GLsizei MATRIX_4X4_REQUIRE_MIN_SIZE = 16;

    static const GLsizei UNIFORM_2V_REQUIRE_MIN_SIZE = 2;
    static const GLsizei UNIFORM_3V_REQUIRE_MIN_SIZE = 3;
    static const GLsizei UNIFORM_4V_REQUIRE_MIN_SIZE = 4;

    WebGLArg(napi_env env, napi_value thisVar) : env_(env)
    {
        renderingContext_ = Util::GetContextObject(env, thisVar);
    }

    virtual ~WebGLArg()
    {
        values.clear();
    }

    bool IsHighWebGL()
    {
        return false;
    }

    // for normal arg
    bool GetWebGLArgs(const NFuncArg &funcArg, uint32_t maxIndex, const std::vector<WebGLArgInfo> &funcs);
    GLboolean  ToGLboolean(uint32_t index) { return values[index].gl_bool; };
    GLfloat    ToGLfloat(uint32_t index) { return values[index].gl_float; };
    GLuint     ToGLuint(uint32_t index) { return values[index].gl_uint; };
    GLint      ToGLint(uint32_t index) { return values[index].gl_int; };
    GLenum     ToGLenum(uint32_t index) { return values[index].gl_enum; };
    GLintptr   ToGLintptr(uint32_t index) { return values[index].gl_intptr; };
    GLsizei    ToGLsizei(uint32_t index) { return values[index].gl_sizei; };
    GLsizeiptr ToGLsizeptr(uint32_t index) { return values[index].gl_sizeptr; };
protected:
    napi_env env_;
    WebGLRenderingContextBasicBase *renderingContext_ { nullptr };
private:
    bool GetWebGLArg(napi_value data, WebGLArgValue &args, const WebGLArgInfo &func);

    std::vector<WebGLArgValue> values = {};
    WebGLArg(const WebGLArg&) = delete;
    WebGLArg& operator=(const WebGLArg&) = delete;
};

class WebGLImageSource {
public:
    WebGLImageSource() {}
    ~WebGLImageSource() {};

    GLsizei GetWidth() { return width_; }
    GLsizei GetHeight() { return height_; }
    uint8_t *GetImageSourceData(bool unpackFlipY);
    bool GenImageSource(napi_env env, napi_value sourceImage);
private:
    template <class T>
    std::tuple<bool, T> GetObjectIntField(napi_env env, napi_value resultObject, const std::string &name);
    bool HandleImageSourceData(napi_env env, napi_value resultData, GLuint offset);
    bool HandleImageSource();

    std::unique_ptr<OHOS::Media::ImageSource> imageSource_ { nullptr };
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap_ { nullptr };
    GLsizei width_ { 0 };
    GLsizei height_{ 0 };

    WebGLImageSource(const WebGLImageSource&) = delete;
    WebGLImageSource& operator=(const WebGLImageSource&) = delete;
};

using BufferDataType = enum {
    BUFFER_DATA_INT_8 = napi_int8_array,
    BUFFER_DATA_UINT_8 = napi_uint8_array,
    BUFFER_DATA_UINT_8_CLAMPED = napi_uint8_clamped_array,
    BUFFER_DATA_INT_16 = napi_int16_array,
    BUFFER_DATA_UINT_16 = napi_uint16_array,
    BUFFER_DATA_INT_32 = napi_int32_array,
    BUFFER_DATA_UINT_32 = napi_uint32_array, // 6
    BUFFER_DATA_FLOAT_32 = napi_float32_array,
    BUFFER_DATA_FLOAT_64 = napi_float64_array,
    BUFFER_DATA_BIGINT_64 = napi_bigint64_array,
    BUFFER_DATA_BIGUINT_64 = napi_biguint64_array,
    BUFFER_DATA_BOOLEAN,
    BUFFER_DATA_GLENUM,
    BUFFER_DATA_INVALID
};

using BufferType = enum {
    BUFFER_DATA_VIEW,
    BUFFER_ARRAY_BUFFER,
    BUFFER_TYPED_ARRAY,
    BUFFER_ARRAY,
};

class WebGLCommBuffer {
public:
    static const int MAX_DUMP = 12;
    explicit WebGLCommBuffer(napi_env env) : env_(env) {}
    ~WebGLCommBuffer() {}

    uint32_t GetBufferDataSize();
    void DumpBuffer(BufferDataType destDataType);
    BufferDataType GetBufferDataType() { return type_; }
    BufferType GetBufferType() { return bufferType_; }
    uint32_t GetBufferLength() { return dataLen_; }
protected:
    WebGLCommBuffer(const WebGLCommBuffer&) = delete;
    WebGLCommBuffer& operator=(const WebGLCommBuffer&) = delete;

    template<class T> void DumpDataBuffer(const char *format);
    napi_env env_;
    uint8_t *data_ = { nullptr };
    BufferDataType type_ { BUFFER_DATA_UINT_8 };
    size_t dataLen_ { 0 };
    BufferType bufferType_ { BUFFER_ARRAY };
    std::vector<uint8_t> buffer_ { };
};

class WebGLReadBufferArg : public WebGLCommBuffer {
public:
    explicit WebGLReadBufferArg(napi_env env) : WebGLCommBuffer(env) {}
    ~WebGLReadBufferArg() {}

    napi_status GenBufferData(napi_value data, BufferDataType destDataType = BUFFER_DATA_FLOAT_32);
    uint8_t *GetBuffer() { return data_ + offset_; }
    uint32_t GetRealBufferLength() { return (realLength_ == 0) ? GetBufferLength() : realLength_; }
    void SetRealBuffer(uint32_t realLength, uint32_t offset)
    {
        LOGI("SetRealBuffer old length %{public}u, new length %{public}u %{public}u",
            GetBufferLength(), offset, realLength);
        realLength_ = realLength;
        offset_ = offset;
    }
private:
    uint32_t realLength_ { 0 };
    uint32_t offset_ { 0 };

    WebGLReadBufferArg(const WebGLReadBufferArg&) = delete;
    WebGLReadBufferArg& operator=(const WebGLReadBufferArg&) = delete;

    template<class srcT, class dstT>
    napi_status GetArrayElement(napi_value data, dstT defaultValue,
        napi_status (*getElementValue)(napi_env env, napi_value element, srcT *value));
    napi_status GetArrayData(napi_value data, BufferDataType destDataType);
};

class WebGLWriteBufferArg : public WebGLCommBuffer {
public:
    explicit WebGLWriteBufferArg(napi_env env) : WebGLCommBuffer(env) {}
    ~WebGLWriteBufferArg()
    {
        if (data_ != nullptr) {
            delete []data_;
        }
    }

    uint8_t *AllocBuffer(uint32_t size)
    {
        if (data_ != nullptr) {
            delete []data_;
            data_ = nullptr;
        }
        dataLen_ = size;
        data_ = new uint8_t[dataLen_];
        return data_;
    }

    int SetBufferData(uint8_t *data, uint32_t dataLen)
    {
        if (data_ != nullptr) {
            delete []data_;
            data_ = nullptr;
        }
        dataLen_ = dataLen;
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

    WebGLWriteBufferArg(const WebGLWriteBufferArg&) = delete;
    WebGLWriteBufferArg& operator=(const WebGLWriteBufferArg&) = delete;
};

} // namespace Rosen
} // namespace OHOS
#endif // ROSENRENDER_ROSEN_WEBGL_ARG
