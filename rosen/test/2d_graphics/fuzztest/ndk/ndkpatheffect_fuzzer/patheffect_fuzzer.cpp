/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "patheffect_fuzzer.h"

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
#include "drawing_image_filter.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_path_effect.h"
#include "drawing_shadow_layer.h"
#include "drawing_text_declaration.h"
#include "drawing_text_typography.h"
#include "drawing_types.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
} // namespace

namespace Drawing {
void NativePathEffectTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t number = GetObject<uint32_t>();
    float* intervals = new float[number*2];
    for (size_t i = 0; i < number*2; i++){
        intervals[i] = GetObject<float>();
    }
    int count = GetObject<int>();
    float phase = GetObject<float>();
    OH_Drawing_PathEffect* PathEffect = OH_Drawing_CreateDashPathEffect(intervals, count, phase);
    if (intervals != nullptr) {
        delete[] intervals;
        intervals = nullptr;
    }
    OH_Drawing_PathEffectDestroy(PathEffect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativePathEffectTest(data, size);
    return 0;
}
