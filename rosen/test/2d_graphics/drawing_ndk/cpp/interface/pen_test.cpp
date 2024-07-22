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

#include "pen_test.h"

#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_pen.h>

#include "test_common.h"

#include "common/log_common.h"

void PenReset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PenReset(pen);
    }
    OH_Drawing_PenSetWidth(pen, 10); // 10 width
    OH_Drawing_PenSetColor(pen, DRAW_COLORGREEN);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawLine(canvas, 0, 0, 100, 100); // 100 宽高
    OH_Drawing_PenDestroy(pen);
}

void PenGetShaderEffect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFF0000);
    OH_Drawing_ErrorCode code;
    for (int i = 0; i < testCount_; i++) {
        code = OH_Drawing_PenGetShaderEffect(pen, shaderEffect);
    }
    DRAWING_LOGI("PenGetShaderEffect::OnTestFunction code = %{public}d", code);
    DRAWING_LOGI("PenGetShaderEffect::OnTestFunction ret =%{public}p", shaderEffect);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 5.f);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);
    OH_Drawing_PenGetShaderEffect(pen, shaderEffect);
    DRAWING_LOGI("PenGetShaderEffect::OnTestFunction ret =%{public}p", shaderEffect);
    OH_Drawing_ShaderEffect* retrievedShaderEffect = nullptr;
    OH_Drawing_PenGetShaderEffect(pen, retrievedShaderEffect);
    if (retrievedShaderEffect == nullptr) {
        DRAWING_LOGI("PenGetShaderEffect::OnTestFunction ret =%{public}p", retrievedShaderEffect);
    } else {
        DRAWING_LOGE("PenGetShaderEffect::OnTestFunction failed to get shaderEffect");
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 400, 400);
    OH_Drawing_CanvasDrawRect(canvas, rect);
}