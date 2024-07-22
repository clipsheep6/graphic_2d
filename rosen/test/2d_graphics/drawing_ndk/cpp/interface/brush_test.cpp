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
#include "brush_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_rect.h>

#include "common/log_common.h"

void BrushReset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* DemoRef = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(DemoRef, 0xff00ff00);
    OH_Drawing_CanvasAttachBrush(canvas, DemoRef);
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数
    OH_Drawing_CanvasDrawRect(canvas, rect1);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_BrushReset(DemoRef);
    }
    OH_Drawing_CanvasAttachBrush(canvas, DemoRef);
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(200, 200, 300, 300); // 200, 200, 300, 300 矩形参数
    OH_Drawing_CanvasDrawRect(canvas, rect2);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(DemoRef);
};

void BrushGetShaderEffect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFF0000);
    OH_Drawing_BrushGetShaderEffect(brush, shaderEffect);
    DRAWING_LOGI("BrushGetShaderEffect::OnTestPerformance ret =%{public}p", shaderEffect);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 5.f);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    OH_Drawing_ErrorCode code = OH_DRAWING_SUCCESS;
    for (int i = 0; i < testCount_; i++) {
        code = OH_Drawing_BrushGetShaderEffect(brush, shaderEffect);
    }
    DRAWING_LOGI("BrushGetShaderEffect::OnTestPerformance code =%{public}d", code);
    DRAWING_LOGI("BrushGetShaderEffect::OnTestPerformance ret =%{public}p", shaderEffect);
    OH_Drawing_ShaderEffect* retrievedShaderEffect = nullptr;
    OH_Drawing_BrushGetShaderEffect(brush, retrievedShaderEffect);
    if (retrievedShaderEffect == nullptr) {
        DRAWING_LOGI("BrushGetShaderEffect::OnTestFunction ret =%{public}p", retrievedShaderEffect);
    } else {
        DRAWING_LOGE("BrushGetShaderEffect::OnTestFunction failed to get shaderEffect");
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 400, 400);
    OH_Drawing_CanvasDrawRect(canvas, rect);
};