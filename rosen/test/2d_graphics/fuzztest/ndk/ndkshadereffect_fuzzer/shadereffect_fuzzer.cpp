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

#include "shadereffect_fuzzer.h"

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
#include "drawing_image.h"
#include "drawing_image_filter.h"
#include "drawing_matrix.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_point.h"
#include "drawing_sampling_options.h"
#include "drawing_shader_effect.h"
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
void NativeShaderEffectTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetObject<uint32_t>();
    float x = GetObject<float>();
    float y = GetObject<float>();
    float radius = GetObject<float>();
    float startRadius = GetObject<float>();
    float endRadius = GetObject<float>();
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(color);

    OH_Drawing_Point* startPt = OH_Drawing_PointCreate(x, y);
    OH_Drawing_Point* endPt = OH_Drawing_PointCreate(x, y);
    uint32_t format = GetObject<uint32_t>();
    uint32_t* colors = new uint32_t[format];
    for (size_t i = 0; i < format; i++){
        colors[i] = GetObject<uint32_t>();
    }
    float* pos = new float[format];
    for (size_t i = 0; i < format; i++){
        pos[i] = GetObject<float>();
    }
    uint32_t tileMode = GetObject<uint32_t>();
    OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, pos, format, 
        static_cast<OH_Drawing_TileMode>(tileMode % 4));
    uint32_t arraySize = GetObject<uint32_t>();
    OH_Drawing_Point2D* startPoint = new OH_Drawing_Point2D[arraySize];
    for (size_t i = 0; i < arraySize; i++) {
        startPoint[i] = {GetObject<float>(), GetObject<float>()};
    }
    OH_Drawing_Point2D* endPoint = new OH_Drawing_Point2D[arraySize];
    for (size_t i = 0; i < arraySize; i++) {
        endPoint[i] = {GetObject<float>(), GetObject<float>()};
    }
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(startPoint, endPoint, colors, pos, format,
        static_cast<OH_Drawing_TileMode>(tileMode % 4), matrix);
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(x, y);
    OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, radius, colors, pos, format,
        static_cast<OH_Drawing_TileMode>(tileMode % 4));
    OH_Drawing_Point2D* centerPoint = new OH_Drawing_Point2D[arraySize];
    for (size_t i = 0; i < arraySize; i++) {
        centerPoint[i] = {GetObject<float>(), GetObject<float>()};
    }
    OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(centerPoint, radius, colors, pos, format,
        static_cast<OH_Drawing_TileMode>(tileMode % 4), matrix);
    OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, colors, pos, format,
        static_cast<OH_Drawing_TileMode>(tileMode % 4));
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    uint32_t filterMode = GetObject<uint32_t>();
    uint32_t mipmapMode = GetObject<uint32_t>();
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        static_cast<OH_Drawing_FilterMode>(filterMode % 2), static_cast<OH_Drawing_MipmapMode>(mipmapMode % 3));
    OH_Drawing_ShaderEffectCreateImageShader(image, static_cast<OH_Drawing_TileMode>(tileMode % 4),
        static_cast<OH_Drawing_TileMode>(tileMode % 4), samplingOptions, matrix);
    OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(startPt, startRadius, endPt, endRadius, colors, pos,
        format, static_cast<OH_Drawing_TileMode>(tileMode % 4), matrix);

    if (colors != nullptr) {
        delete[] colors;
        colors = nullptr;
    }
    if (pos != nullptr) {
        delete[] pos;
        pos = nullptr;
    }
    if (startPoint != nullptr) {
        delete[] startPoint;
        startPoint = nullptr;
    }
    if (endPoint != nullptr) {
        delete[] endPoint;
        endPoint = nullptr;
    }
    if (centerPoint != nullptr) {
        delete[] centerPoint;
        centerPoint = nullptr;
    }
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
    OH_Drawing_PointDestroy(centerPt);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);

}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeShaderEffectTest(data, size);
    return 0;
}
