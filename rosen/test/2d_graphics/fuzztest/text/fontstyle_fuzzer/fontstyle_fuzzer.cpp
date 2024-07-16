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

#include "fontstyle_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "text/font_style.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_SIZE = 5000;
constexpr size_t SLANT_SIZE = 3;
} // namespace
namespace Drawing {
bool FontStyleFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    FontStyle fontStyleOne;
    int weight = GetObject<int>() % MAX_SIZE;
    int width = GetObject<int>() % MAX_SIZE;
    uint32_t slant = GetObject<uint32_t>();
    FontStyle fontStyleTwo = FontStyle(weight, width, static_cast<FontStyle::Slant>(slant % SLANT_SIZE));
    FontStyle fontStyleThree = FontStyle(weight, width, static_cast<FontStyle::Slant>(slant % SLANT_SIZE));
    if (fontStyleTwo == fontStyleThree) {};
    fontStyleOne.GetWeight();
    fontStyleTwo.GetWeight();
    fontStyleTwo.GetWidth();
    fontStyleTwo.GetSlant();
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::FontStyleFuzzTest(data, size);
    return 0;
}