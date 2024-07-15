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

#include "color_filter_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "effect/color_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t BLENDMODE_SIZE = 29;
constexpr size_t FILTERTYPE_SIZE = 8;
constexpr size_t MATRIX_SIZE = 20;
constexpr size_t OVER_DRAW_COLOR_NUM = 6;
} // namespace
namespace Drawing {
bool ColorFilterFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    colorFilter->Deserialize(nullptr);
    colorFilter->Serialize();
    colorFilter->GetType();
    colorFilter->GetDrawingType();
    const size_t MATRIX_SIZE = 20;
    float matrix[MATRIX_SIZE];
    colorFilter->AsAColorMatrix(matrix);
    float f1[MATRIX_SIZE];
    float f2[MATRIX_SIZE];
    colorFilter->InitWithCompose(f1, f2);

    return true;
}

bool ColorFilterFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    ColorQuad colorQuad = GetObject<ColorQuad>();
    uint32_t mode = GetObject<uint32_t>();
    uint32_t type = GetObject<uint32_t>();
    ColorFilter colorFilter = ColorFilter(static_cast<ColorFilter::FilterType>(type % FILTERTYPE_SIZE), colorQuad,
        static_cast<BlendMode>(mode % BLENDMODE_SIZE));
    ColorMatrix matrix;
    ColorFilter colorFilterOne = ColorFilter(static_cast<ColorFilter::FilterType>(type % FILTERTYPE_SIZE), matrix);
    std::shared_ptr<ColorFilter> colorFilterTwo = ColorFilter::CreateBlendModeColorFilter(colorQuad,
        static_cast<BlendMode>(mode % BLENDMODE_SIZE));
    std::shared_ptr<ColorFilter> colorFilterThree = ColorFilter::CreateComposeColorFilter(colorFilter, colorFilterOne);
    float fOne[MATRIX_SIZE];
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        fOne[i] = GetObject<float>();
    }
    float fTwo[MATRIX_SIZE];
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        fTwo[i] = GetObject<float>();
    }
    std::shared_ptr<ColorFilter> colorFilterFour = ColorFilter::CreateComposeColorFilter(fOne, fTwo);
    std::shared_ptr<ColorFilter> colorFilterFive = ColorFilter::CreateMatrixColorFilter(matrix);
    std::shared_ptr<ColorFilter> colorFilterSix = ColorFilter::CreateFloatColorFilter(fOne);
    std::shared_ptr<ColorFilter> colorFilterSeven = ColorFilter::CreateSrgbGammaToLinear();
    ColorQuad colors[OVER_DRAW_COLOR_NUM];
    for (size_t i = 0; i < OVER_DRAW_COLOR_NUM; i++) {
        colors[i] = GetObject<ColorQuad>();
    }
    std::shared_ptr<ColorFilter> colorFilterEight = ColorFilter::CreateOverDrawColorFilter(colors);
    std::shared_ptr<ColorFilter> colorFilterNine = ColorFilter::CreateLumaColorFilter();
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::ColorFilterFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ColorFilterFuzzTest002(data, size);
    return 0;
}
