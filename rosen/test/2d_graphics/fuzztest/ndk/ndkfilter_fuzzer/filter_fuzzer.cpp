/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "filter_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"

#include "drawing_bitmap.h"
#include "drawing_brush.h"
#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_color_filter.h"
#include "drawing_font_collection.h"
#include "drawing_filter.h"
#include "drawing_image_filter.h"
#include "drawing_mask_filter.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_shadow_layer.h"
#include "drawing_text_declaration.h"
#include "drawing_text_typography.h"
#include "drawing_types.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
} // namespace

namespace Drawing {

void FilterTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    float sigmaX = GetObject<float>();
    float sigmaY = GetObject<float>();
    uint32_t tileMode = GetObject<uint32_t>();
    OH_Drawing_ImageFilter* imageFilter = OH_Drawing_ImageFilterCreateBlur(sigmaX, sigmaY, static_cast<OH_Drawing_TileMode>(tileMode % 4), nullptr);
    OH_Drawing_FilterSetImageFilter(filter, imageFilter);
    uint32_t blurType = GetObject<uint32_t>();
    float sigma = GetObject<float>();
    bool respectCTM = GetObject<bool>();
    OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(static_cast<OH_Drawing_BlurType>(blurType % 4), sigma, respectCTM);
    OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_FilterGetColorFilter(filter, colorFilter);

    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_ImageFilterDestroy(imageFilter);
    OH_Drawing_MaskFilterDestroy(maskFilter);
    OH_Drawing_ColorFilterDestroy(colorFilter);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::FilterTest(data, size);

    return 0;
}
