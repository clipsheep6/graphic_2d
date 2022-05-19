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

#include "setargb_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>

#include "c/drawing_color.h"

const uint32_t CONSTANTS_GREEN = 10;
const int CONSTANTS_NUMBER_FIVE = 5;

namespace OHOS {
namespace Rosen {
namespace Drawing {
template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    std::memcpy(&object, data, objectSize);
    return objectSize;
}

bool DrawingColorSetArgbFuzzTest(const uint8_t* data, size_t size)
{
    uint32_t alpha;
    if (data == nullptr || size < sizeof(alpha)) {
        return false;
    }
    uint32_t blue = static_cast<uint32_t>(size % CONSTANTS_NUMBER_FIVE);
    size_t startPos = 0;
    GetObject<uint32_t>(alpha, data + startPos, size - startPos);
    OH_Drawing_ColorSetArgb(alpha, 0, CONSTANTS_GREEN, blue);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::DrawingColorSetArgbFuzzTest(data, size);
    return 0;
}
