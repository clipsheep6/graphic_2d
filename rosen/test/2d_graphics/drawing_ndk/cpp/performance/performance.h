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

#ifndef DRAWINGNDKTEST5_0_PERFORMANCE_H
#define DRAWINGNDKTEST5_0_PERFORMANCE_H
#include <cstdint>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>

#include "hilog/log.h"
#include "native_drawing/drawing_error_code.h"
#include "native_drawing/drawing_pixel_map.h"
#include "native_drawing/drawing_shadow_layer.h"
#include "test_common.h"

#include "common/log_common.h"
#define ErrorCheck()                                                                                              \
    do {                                                                                                          \
        if (OH_Drawing_ErrorCodeGet() != OH_DRAWING_SUCCESS) {                                                    \
            DRAWING_LOGE(                                                                                         \
                "SubBasicGraphicsSpecialPerformance %{public}d %{public}d", __LINE__, OH_Drawing_ErrorCodeGet()); \
        }                                                                                                         \
    } while (0)

#define SET_PEN_AND_BRUSH_EFFECT                                                                                \
    OH_Drawing_PenSetAlpha(pen, 0xF0);                                                                          \
    OH_Drawing_PenSetWidth(pen, 5);                                                                             \
    ErrorCheck();                                                                                               \
    OH_Drawing_PenSetAntiAlias(pen, true);                                                                      \
    ErrorCheck();                                                                                               \
    OH_Drawing_BlendMode model = BLEND_MODE_SRC;                                                                \
    OH_Drawing_PenSetBlendMode(pen, model);                                                                     \
    ErrorCheck();                                                                                               \
    OH_Drawing_PenLineCapStyle lineCapStyle = LINE_FLAT_CAP;                                                    \
    OH_Drawing_PenSetCap(pen, lineCapStyle);                                                                    \
    ErrorCheck();                                                                                               \
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);                                                                    \
    ErrorCheck();                                                                                               \
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();                                                      \
    ErrorCheck();                                                                                               \
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xFFFF00FF, BLEND_MODE_DST_IN); \
    ErrorCheck();                                                                                               \
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);                                                       \
    OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(NORMAL, 10.0, true);                    \
    OH_Drawing_FilterSetMaskFilter(filter, maskFilter);                                                         \
    ErrorCheck();                                                                                               \
    OH_Drawing_PenSetFilter(pen, filter);                                                                       \
    ErrorCheck();                                                                                               \
    OH_Drawing_PenLineJoinStyle lineJoinStyle = LINE_ROUND_JOIN;                                                \
    OH_Drawing_PenSetJoin(pen, lineJoinStyle);                                                                  \
    ErrorCheck();                                                                                               \
    OH_Drawing_PenSetMiterLimit(pen, 10.0);                                                                     \
    ErrorCheck();                                                                                               \
    float intervals[] = { 1.0, 1.0 };                                                                           \
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 2, 0);                       \
    OH_Drawing_PenSetPathEffect(pen, pathEffect);                                                               \
    ErrorCheck();                                                                                               \
    OH_Drawing_Point* styleCenter = OH_Drawing_PointCreate(100, 100);                                           \
    uint32_t styleColors[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };                                            \
    float pos[] = { 0, 0.5, 1.0 };                                                                              \
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateRadialGradient(                        \
        styleCenter, 100, styleColors, pos, 3, OH_Drawing_TileMode::CLAMP);                                     \
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);                                                           \
    ErrorCheck();                                                                                               \
    OH_Drawing_PenSetWidth(pen, 3);                                                                             \
    OH_Drawing_ShadowLayer* shadowLayer = OH_Drawing_ShadowLayerCreate(4.0, 3, 3, 0xFFFFFFFF);                  \
    OH_Drawing_PenSetShadowLayer(pen, shadowLayer);                                                             \
    ErrorCheck();                                                                                               \
    OH_Drawing_BrushSetAlpha(brush, 0xF0);                                                                      \
    ErrorCheck();                                                                                               \
    OH_Drawing_BrushSetAntiAlias(brush, true);                                                                  \
    ErrorCheck();                                                                                               \
    OH_Drawing_BrushSetColor(brush, 0xFFFF0000);                                                                \
    ErrorCheck();                                                                                               \
    OH_Drawing_BrushSetBlendMode(brush, model);                                                                 \
    ErrorCheck();                                                                                               \
    OH_Drawing_BrushSetFilter(brush, filter);                                                                   \
    ErrorCheck();                                                                                               \
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);                                                       \
    ErrorCheck();                                                                                               \
    OH_Drawing_BrushSetShadowLayer(brush, shadowLayer);                                                         \
    ErrorCheck();

#define DESTROY_EFFECT                            \
    OH_Drawing_MaskFilterDestroy(maskFilter);     \
    OH_Drawing_PointDestroy(styleCenter);         \
    OH_Drawing_ShadowLayerDestroy(shadowLayer);   \
    OH_Drawing_ShaderEffectDestroy(shaderEffect); \
    OH_Drawing_PathEffectDestroy(pathEffect);     \
    OH_Drawing_ColorFilterDestroy(colorFilter);   \
    OH_Drawing_FilterDestroy(filter);

#endif // DRAWINGNDKTEST5_0_PERFORMANCE_H
