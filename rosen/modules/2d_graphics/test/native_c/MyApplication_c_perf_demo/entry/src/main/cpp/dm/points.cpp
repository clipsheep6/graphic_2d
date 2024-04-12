#include "points.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include "test_common.h"
#include "common/log_common.h"

enum{
    kW=640,
    kH=490,
};
Points::Points() {
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "points";
}

void Points::fill_pts(OH_Drawing_Point2D pts[], size_t n, TestRend* rand)
{
    for (size_t i = 0; i < n; i++) {
        // Compute these independently and store in variables, rather
        // than in the parameter-passing expression, to get consistent
        // evaluation order across compilers.
        float y = rand->nextUScalar1() * 480;
        float x = rand->nextUScalar1() * 640;
        pts[i].x = x;
        pts[i].y = y;
    }
}

void Points::OnTestFunction(OH_Drawing_Canvas* canvas)
{
// DRAWING_LOGI("OnTestFunction path = %{public}s",__FILE_NAME__);
    OH_Drawing_CanvasTranslate(canvas, 1, 1);
    TestRend rand;
    const size_t n = 99;
    OH_Drawing_Pen* pen0 = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush1 = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen2 = OH_Drawing_PenCreate();    
    OH_Drawing_Brush* brush3 = OH_Drawing_BrushCreate();
    
    OH_Drawing_PenSetColor(pen0, DRAW_ColorRED);
    OH_Drawing_BrushSetColor(brush1, DRAW_ColorGREEN);
    OH_Drawing_PenSetColor(pen2, DRAW_ColorBLUE);
    OH_Drawing_BrushSetColor(brush3, DRAW_ColorWHITE);
    
    OH_Drawing_PenSetWidth(pen0,4);
    OH_Drawing_PenSetCap(pen2,LINE_ROUND_CAP);
    OH_Drawing_PenSetWidth(pen2,6);
    
    OH_Drawing_Point2D pts[n];
    fill_pts(pts, n, &rand);
    
    OH_Drawing_CanvasAttachPen(canvas,pen0);
    OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_POLYGON,n,pts);
    OH_Drawing_CanvasDetachPen(canvas);
    
    OH_Drawing_CanvasAttachBrush(canvas,brush1);
    OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_LINES,n,pts);
    OH_Drawing_CanvasDetachBrush(canvas);
    
    OH_Drawing_CanvasAttachPen(canvas,pen2);
    OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_POINTS,n,pts);
    OH_Drawing_CanvasDetachPen(canvas);
    
    OH_Drawing_CanvasAttachBrush(canvas,brush3);
    OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_POINTS,n,pts);
    OH_Drawing_CanvasDetachBrush(canvas);
}
