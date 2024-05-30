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

#include "matrix_fuzzer.h"

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
#include "drawing_matrix.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_rect.h"
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
void NativeMatrixTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    float degree = GetObject<float>();
    float x = GetObject<float>();
    float y = GetObject<float>();
    float px = GetObject<float>();
    float py = GetObject<float>();
    float scaleX = GetObject<float>();
    float skewX = GetObject<float>();
    float transX = GetObject<float>();
    float skewY = GetObject<float>();
    float scaleY = GetObject<float>();
    float transY = GetObject<float>();
    float persp0 = GetObject<float>();
    float persp1 = GetObject<float>();
    float persp2 = GetObject<float>();
    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    float index = GetObject<float>();
    uint32_t count = GetObject<uint32_t>();
    uint32_t fitMode = GetObject<uint32_t>();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixCreateRotation(degree, x, y);
    OH_Drawing_MatrixCreateScale(x, y, px, py);
    OH_Drawing_MatrixCreateTranslation(x, y);
    OH_Drawing_Rect* rectOne = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_Rect* rectTwo = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_MatrixSetMatrix(matrix, scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
    OH_Drawing_MatrixSetRectToRect(matrix, rectOne, rectTwo, static_cast<OH_Drawing_ScaleToFit>(fitMode % 4));
    OH_Drawing_MatrixPreRotate(matrix, degree, px, py);
    OH_Drawing_MatrixPreScale(matrix, transX, transY, px, py);
    OH_Drawing_MatrixPreTranslate(matrix, x, y);
    OH_Drawing_MatrixPostRotate(matrix, degree, px, py);
    OH_Drawing_MatrixPostScale(matrix, transX, transY, px, py);
    OH_Drawing_MatrixPostTranslate(matrix, x, y);
    OH_Drawing_MatrixReset(matrix);
    OH_Drawing_Matrix* matrixTwo = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrixThree = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixConcat(matrix, matrixTwo, matrixThree);
    OH_Drawing_MatrixGetValue(matrix, index);
    OH_Drawing_MatrixRotate(matrix, degree, px, py);
    OH_Drawing_MatrixTranslate(matrix, x, y);
    OH_Drawing_MatrixScale(matrix, transX, transY, px, py);
    OH_Drawing_MatrixInvert(matrix, matrixTwo);
    uint32_t arraySize = GetObject<uint32_t>();
    OH_Drawing_Point2D* pointOne = new OH_Drawing_Point2D[arraySize];
    for (size_t i = 0; i < arraySize; i++) {
        pointOne[i] = {GetObject<float>(), GetObject<float>()};
    }
    OH_Drawing_Point2D* pointTwo = new OH_Drawing_Point2D[arraySize];
    for (size_t i = 0; i < arraySize; i++) {
        pointTwo[i] = {GetObject<float>(), GetObject<float>()};
    }
    OH_Drawing_MatrixSetPolyToPoly(matrix, pointOne, pointTwo, count);
    OH_Drawing_MatrixMapRect(matrix, rectOne, rectTwo);
    OH_Drawing_MatrixIsEqual(matrixTwo, matrixThree);
    OH_Drawing_MatrixIsIdentity(matrix);
    if (pointOne != nullptr) {
        delete[] pointOne;
        pointOne = nullptr;
    }
    if (pointTwo != nullptr) {
        delete[] pointTwo;
        pointTwo = nullptr;
    }
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrixTwo);
    OH_Drawing_MatrixDestroy(matrixThree);
    OH_Drawing_RectDestroy(rectOne);
    OH_Drawing_RectDestroy(rectTwo);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeMatrixTest(data, size);
    return 0;
}
