#include "hardstopgradientbench_scalenumcolors.h"
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <time.h>
#include <array>
#include "common/log_common.h"
#include "test_common.h"
using namespace std;

int kNumPoints = 10;
constexpr int kSize = 400000;
constexpr int kNumColorChoices = 4;

SkColor color_choices[kNumColorChoices] = {
    SK_ColorRED,
    SK_ColorGREEN,
    SK_ColorBLUE,
    SK_ColorYELLOW,
};
SkColor colors[100];

void HardStopGradientBench_ScaleNumColors::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    // 用例名: hardstop_scale_num_colors_setShader 测试 OH_Drawing_PenSetShaderEffect
    // 迁移基于skia HardStopGradientBench_ScaleNumColors->hardstopgradientbench_scalenumcolors
    DRAWING_LOGI("HardStopGradientBench_ScaleNumColors::OnTestFunction start");
    // 创建一个画刷brush对象
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, 0xFF00FF00);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_ShaderEffect *effect;
    OH_Drawing_Point *startPt = OH_Drawing_PointCreate(0, kSize / 2);
    OH_Drawing_Point *endPt = OH_Drawing_PointCreate(kSize - 1, kSize / 2);
    OH_Drawing_Point *centerPt = OH_Drawing_PointCreate(kSize * 0.5, kSize * 0.5);
    for (int i = 0; i < fColorCount_; i++) {
        colors[i] = color_choices[i % kNumColorChoices];
    }
    int ArraySize = 100;
    float positions[ArraySize];
    // 初始化数组的前两个元素为0.0f
    positions[0] = 0.0f;
    positions[1] = 0.0f;
    // 用等间距的值填充数组的其余部分
    for (int i = 2; i < fColorCount_; ++i) {
        positions[i] = static_cast<float>(i) / (fColorCount_ - 1.0f);
        DRAWING_LOGI("HardStopGradientBench_ScaleNumColors = %{public}f", positions[i]);
    }

    switch (effectType_) {
        case kClamp:
            DRAWING_LOGI("HardStopGradientBench_ScaleNumColors:kClamp");
            effect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, positions, fColorCount_,
                OH_Drawing_TileMode::CLAMP);
            break;
        case kRepeat:
            DRAWING_LOGI("HardStopGradientBench_ScaleNumColors:kRepeat");

            effect = OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, kSize * 0.5, colors, positions, fColorCount_,
                OH_Drawing_TileMode::REPEAT);
            break;
        case kMirror:
            DRAWING_LOGI("HardStopGradientBench_ScaleNumColors:kMirror");
            effect = OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, colors, positions, fColorCount_,
                OH_Drawing_TileMode::DECAL);
            break;
        default:
            effect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, positions, fColorCount_,
                OH_Drawing_TileMode::CLAMP);
            break;
    }
    OH_Drawing_BrushSetShaderEffect(brush, effect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, kSize, kSize);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawOval(canvas, rect);
    }
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ShaderEffectDestroy(effect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
    DRAWING_LOGI("HardStopGradientBench_ScaleNumColors::OnTestFunction end");
}

void HardStopGradientBench_ScaleNumColors::OnTestPerformance(OH_Drawing_Canvas *canvas)
{
    // 用例名: hardstop_scale_num_colors_setShader 测试 OH_Drawing_PenSetShaderEffect
    // 迁移基于skia HardStopGradientBench_ScaleNumColors->hardstopgradientbench_scalenumcolors
    DRAWING_LOGI("HardStopGradientBench_ScaleNumColors::OnTestPerformance start");
    // 创建一个画刷brush对象
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, 0xFF00FF00);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_ShaderEffect *effect;
    OH_Drawing_Point *startPt = OH_Drawing_PointCreate(0, kSize / 2);
    OH_Drawing_Point *endPt = OH_Drawing_PointCreate(kSize - 1, kSize / 2);
    OH_Drawing_Point *centerPt = OH_Drawing_PointCreate(kSize * 0.5, kSize * 0.5);
    for (int i = 0; i < fColorCount_; i++) {
        colors[i] = color_choices[i % kNumColorChoices];
    }
    int ArraySize = 100;
    float positions[ArraySize];
    // 初始化数组的前两个元素为0.0f
    positions[0] = 0.0f;
    positions[1] = 0.0f;
    // 用等间距的值填充数组的其余部分
    for (int i = 2; i < fColorCount_; ++i) {
        positions[i] = static_cast<float>(i) / (fColorCount_ - 1.0f);
        DRAWING_LOGI("HardStopGradientBench_ScaleNumColors = %{public}f", positions[i]);
    }

    switch (effectType_) {
        case kClamp:
            DRAWING_LOGI("HardStopGradientBench_ScaleNumColors:kClamp");
            effect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, positions, fColorCount_,
                OH_Drawing_TileMode::CLAMP);
            break;
        case kRepeat:
            DRAWING_LOGI("HardStopGradientBench_ScaleNumColors:kRepeat");

            effect = OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, kSize * 0.5, colors, positions, fColorCount_,
                OH_Drawing_TileMode::REPEAT);
            break;
        case kMirror:
            DRAWING_LOGI("HardStopGradientBench_ScaleNumColors:kMirror");
            effect = OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, colors, positions, fColorCount_,
                OH_Drawing_TileMode::DECAL);
            break;
        default:
            effect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, positions, fColorCount_,
                OH_Drawing_TileMode::CLAMP);
            break;
    }
    OH_Drawing_BrushSetShaderEffect(brush, effect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, kSize, kSize);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawOval(canvas, rect);
    }
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ShaderEffectDestroy(effect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
    DRAWING_LOGI("HardStopGradientBench_ScaleNumColors::OnTestPerformance end");
}
