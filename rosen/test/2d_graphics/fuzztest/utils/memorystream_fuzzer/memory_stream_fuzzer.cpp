/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "memory_stream_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "skia_adapter/skia_canvas.h"
#include "utils/memory_stream.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MATH_ONE = 1;
constexpr size_t MATH_TWO = 2;
constexpr size_t ARRAY_MAX_SIZE = 5000;
} // namespace

namespace Drawing {
void MemoryStreamFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t copyData = GetObject<uint32_t>() % MATH_TWO;
    uint32_t str_size = GetObject<uint32_t>() % ARRAY_MAX_SIZE + MATH_ONE;

    char* dataStr = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        dataStr[i] = GetObject<char>();
    }
    dataStr[str_size - MATH_ONE] = '\0';
    MemoryStream mem1 = MemoryStream(dataStr, str_size);
    MemoryStream mem2 = MemoryStream(dataStr, str_size, static_cast<bool>(copyData));
    if (dataStr != nullptr) {
        delete [] dataStr;
        dataStr = nullptr;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::MemoryStreamFuzzTest000(data, size);

    return 0;
}