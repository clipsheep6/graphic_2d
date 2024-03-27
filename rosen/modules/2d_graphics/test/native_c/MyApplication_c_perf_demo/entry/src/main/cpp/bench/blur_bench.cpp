#include "bench/blur_bench.h"
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_canvas.h>
#include "..//test_common.h"

void BlurBench::OnTestFunction(OH_Drawing_Canvas *canvas) {
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    const float BLUR_SIGMA_SCALE = 0.57735f;
    const float r = fRadius > 0 ? BLUR_SIGMA_SCALE * fRadius + 0.5f : 0.0f;
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(fRadius, fRadius, fRandom.nextUScalar1() * 400 + fRadius, fRandom.nextUScalar1() * 400 + fRadius);
    // 创建一个模板滤波器
    OH_Drawing_MaskFilter *mask = OH_Drawing_MaskFilterCreateBlur(type_, r, true);
    // 创建一个滤波器对象
    OH_Drawing_Filter *filter = OH_Drawing_FilterCreate();
    // 为滤波器设置模板
    OH_Drawing_FilterSetMaskFilter(filter, mask);
    // 设置画笔滤波器
    OH_Drawing_PenSetFilter(pen, filter);
    // 上画布
    OH_Drawing_CanvasDrawOval(canvas, rect);
    // 释放内存
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_MaskFilterDestroy(mask);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
}

void BlurBench::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // blur_0.50_normal_high_quality
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    const float BLUR_SIGMA_SCALE = 0.57735f;
    const float r = fRadius > 0 ? BLUR_SIGMA_SCALE * fRadius + 0.5f : 0.0f;
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect *rect = OH_Drawing_RectCreate(fRadius, fRadius, fRandom.nextUScalar1() * 400 + fRadius, fRandom.nextUScalar1() * 400 + fRadius);
        // 创建一个模板滤波器
        OH_Drawing_MaskFilter *mask = OH_Drawing_MaskFilterCreateBlur(type_, r, true);
        // 创建一个滤波器对象
        OH_Drawing_Filter *filter = OH_Drawing_FilterCreate();
        // 为滤波器设置模板
        OH_Drawing_FilterSetMaskFilter(filter, mask);
        // 设置画笔滤波器
        OH_Drawing_PenSetFilter(pen, filter);
        // 上画布
        OH_Drawing_CanvasDrawOval(canvas, rect);
        // 释放内存
        OH_Drawing_MaskFilterDestroy(mask);
        OH_Drawing_FilterDestroy(filter);
        OH_Drawing_RectDestroy(rect);
    }
    // 释放内存
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}