#include "draw_rect_test.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_rect.h>

void DrawRectTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    // 创建一个画刷Brush对象，Brush对象用于形状的填充
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    //绘制矩形
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
}


void DrawRectTest::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 创建一个画刷Brush对象，Brush对象用于形状的填充
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    //绘制矩形
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawRect(canvas, rect);
    }
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
}
