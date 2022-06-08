/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "surfacebuffer_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <display_type.h>
#include "surface_buffer.h"
#include "surface_buffer_impl.h"
#include "buffer_extra_data.h"
#include "buffer_extra_data_impl.h"

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10; // define string size is 10
        constexpr char STR_END_CHAR = '\0';
        const uint8_t* data_ = nullptr; // used to store outside untrusted data
        size_t size_ = 0; // outside untrusted data size
        size_t pos;

        // calculate the min size of the outside untrusted data
        constexpr size_t MIN_SIZE = sizeof(int)+sizeof(ColorGamut)+sizeof(TransformType)+
                                    sizeof(int32_t)+sizeof(int32_t)+sizeof(BufferRequestConfig)+
                                    sizeof(BufferHandle)+STR_LEN+sizeof(int32_t)+STR_LEN+
                                    sizeof(int64_t)+STR_LEN+sizeof(double)+STR_LEN+STR_LEN;
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        std::memcpy(&object, data_ + pos, objectSize);
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen-1] = STR_END_CHAR;
        for (int i = 0; i < strlen-1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < MIN_SIZE) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        int seqNum = GetData<int>();
        ColorGamut colorGamut = GetData<ColorGamut>();
        TransformType transform = GetData<TransformType>();
        int32_t width = GetData<int32_t>();
        int32_t height = GetData<int32_t>();
        BufferRequestConfig config = GetData<BufferRequestConfig>();
        BufferHandle handle = GetData<BufferHandle>();
        std::string key_Int32 = GetStringFromData(STR_LEN);
        int32_t value_Int32 = GetData<int32_t>();
        std::string key_Int64 = GetStringFromData(STR_LEN);
        int64_t value_Int64 = GetData<int64_t>();
        std::string key_Double = GetStringFromData(STR_LEN);
        double value_Double = GetData<double>();
        std::string key_Str = GetStringFromData(STR_LEN);
        std::string value_Str = GetStringFromData(STR_LEN);

        // test
        SurfaceBuffer* surfaceBuffer = new SurfaceBufferImpl(seqNum);
        surfaceBuffer->SetSurfaceBufferColorGamut(colorGamut);
        surfaceBuffer->SetSurfaceBufferTransform(transform);
        surfaceBuffer->SetSurfaceBufferWidth(width);
        surfaceBuffer->SetSurfaceBufferHeight(height);
        surfaceBuffer->Alloc(config);
        surfaceBuffer->SetBufferHandle(&handle);
        sptr<BufferExtraData> bedata = new BufferExtraDataImpl();
        bedata->ExtraSet(key_Int32, value_Int32);
        bedata->ExtraSet(key_Int64, value_Int64);
        bedata->ExtraSet(key_Double, value_Double);
        bedata->ExtraSet(key_Str, value_Str);
        surfaceBuffer->SetExtraData(bedata);

        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

