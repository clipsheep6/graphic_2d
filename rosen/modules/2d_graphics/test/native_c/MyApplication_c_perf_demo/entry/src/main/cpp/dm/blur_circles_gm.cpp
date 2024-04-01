#include "blur_circles_gm.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>

#include "common/log_common.h"
#include "test_common.h"

BlurCirclesGM::BlurCirclesGM()
{
    bitmapWidth_ = 950;
    bitmapHeight_ = 950;
    fileName_ = "blurcircles";
}

float BlurCirclesGM::ConvertRadiusToSigma(float radius)
{
    const float r = radius > 0 ? 0.57735f * radius + 0.5f : 0.0f;
    return r;
}

void BlurCirclesGM::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    const float blurRadii[4] = { 1.f, 5.f, 10.f, 20.f };
    const float circleRadii[4] = { 5.f, 10.f, 25.f, 50.f };
    const int loops = 4;
    // 画布保存状态数量
    uint32_t fSaveCount = 0;
    OH_Drawing_CanvasScale(canvas, 1.5f, 1.5f);
    OH_Drawing_CanvasTranslate(canvas, 50.f, 50.f);
    for (int i = 0; i < loops; ++i) {
        // 创建一个模板滤波器
        OH_Drawing_MaskFilter* mask = OH_Drawing_MaskFilterCreateBlur(NORMAL, ConvertRadiusToSigma(blurRadii[i]), true);
        // 创建一个滤波器对象
        OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
        // 为滤波器设置模板
        OH_Drawing_FilterSetMaskFilter(filter, mask);
        fSaveCount = OH_Drawing_CanvasGetSaveCount(canvas);
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasTranslate(canvas, 0, 150.f * i);

        for (int j = 0; j < loops; ++j) {
            OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
            OH_Drawing_BrushSetColor(brush, 0xFF000000);
            // 设置画笔滤波器
            OH_Drawing_BrushSetFilter(brush, filter);
            OH_Drawing_CanvasAttachBrush(canvas, brush);
            OH_Drawing_Point* point = OH_Drawing_PointCreate(50.f, 50.f);
            OH_Drawing_CanvasSave(canvas);
            // 旋转画布
            OH_Drawing_CanvasRotate(canvas, j * 22.f, 50.f, 50.f);
            OH_Drawing_CanvasDrawCircle(canvas, point, circleRadii[j]);
            OH_Drawing_CanvasRestore(canvas);
            OH_Drawing_CanvasTranslate(canvas, 150.f, 0);
            // 释放内存
            OH_Drawing_CanvasDetachBrush(canvas);
            OH_Drawing_BrushDestroy(brush);
            OH_Drawing_PointDestroy(point);
        }
        // 恢复画布状态
        int n = OH_Drawing_CanvasGetSaveCount(canvas) - fSaveCount;
        for (int j = 0; j < n; ++j) {
            OH_Drawing_CanvasRestore(canvas);
        }
        // 释放内存
        OH_Drawing_MaskFilterDestroy(mask);
        OH_Drawing_FilterDestroy(filter);
    }
}
