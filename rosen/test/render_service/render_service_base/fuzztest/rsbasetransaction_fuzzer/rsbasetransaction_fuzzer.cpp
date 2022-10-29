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

#include "rsbasetransaction_fuzzer.h"

#include <securec.h>

#include "transaction/rs_ashmem_helper.h"

namespace OHOS {
namespace Rosen {
namespace {
    const uint8_t* g_data = nullptr;
    size_t g_size = 0;
    size_t g_pos;
} // namespace
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    bool RSAshmemAllocator(const uint8_t* data, size_t size)
    {
        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocatorWithFd(GetData<int>(), GetData<size_t>(), GetData<int>());
        ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(GetData<size_t>(), GetData<int>());
        if (ashmemAllocator != nullptr) {
            (void)ashmemAllocator->GetFd();
            (void)ashmemAllocator->GetSize();
            (void)ashmemAllocator->GetData();
            (void)ashmemAllocator->CopyFromAshmem(GetData<int>());
            (void)ashmemAllocator->Realloc(ashmemAllocator->GetData(), GetData<int>());
            (void)ashmemAllocator->Dealloc(ashmemAllocator->GetData());
        }
        return true;
    }
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSAshmemAllocator(data, size);
    return 0;
}

