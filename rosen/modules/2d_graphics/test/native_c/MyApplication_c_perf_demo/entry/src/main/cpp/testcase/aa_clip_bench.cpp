#include "aa_clip_bench.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include "test_common.h"
#include "common/log_common.h"

AAClipBench::AAClipBench(bool isPath, bool isAa): isPath_(isPath), isAa_(isAa) {
    clipRect[0] = OH_Drawing_RectCreate(200.5, 100.5, 600.5, 500.5);
    clipRect[1] = OH_Drawing_RectCreate(0.5, 100.5, 400.5, 500.5);
    clipPath = OH_Drawing_PathCreate();
    OH_Drawing_RoundRect* rrect = OH_Drawing_RoundRectCreate(clipRect[0], 10, 10);
    OH_Drawing_PathAddRoundRect(clipPath, rrect, PATH_DIRECTION_CW);
    OH_Drawing_RoundRectDestroy(rrect);
}

AAClipBench::~AAClipBench() {
    OH_Drawing_RectDestroy(clipRect[0]);
    OH_Drawing_RectDestroy(clipRect[1]);
    OH_Drawing_PathCopy(clipPath);
    }

void AAClipBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGE("AAClipBench::OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* drawRect = OH_Drawing_RectCreate(0, 0, 700, 700);

    OH_Drawing_PathReset(clipPath);
    OH_Drawing_RoundRect *rrect = OH_Drawing_RoundRectCreate(clipRect[0], 50, 50);
    OH_Drawing_PathAddRoundRect(clipPath, rrect, PATH_DIRECTION_CW);
    OH_Drawing_CanvasSave(canvas);
    if (isPath_) {
        OH_Drawing_CanvasClipPath(canvas, clipPath, INTERSECT, isAa_);
    } else {
        OH_Drawing_CanvasClipRect(canvas, clipRect[0], INTERSECT, isAa_);
    }
    
    OH_Drawing_CanvasDrawRect(canvas, drawRect);
    OH_Drawing_CanvasRestore(canvas);

    OH_Drawing_RectDestroy(drawRect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

void AAClipBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // aaclip_path_AA
    // aaclip_rect_AA
    DRAWING_LOGE("AAClipBench::OnTestPerformance");
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect *drawRect = OH_Drawing_RectCreate(0, 0, 700, 700);
    
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect* rect = clipRect[i % 2];
        OH_Drawing_PathReset(clipPath);
        OH_Drawing_RoundRect *rrect = OH_Drawing_RoundRectCreate(rect, 50, 50);
        OH_Drawing_PathAddRoundRect(clipPath, rrect, PATH_DIRECTION_CW);
        OH_Drawing_CanvasSave(canvas);
        if (isPath_) {
            OH_Drawing_CanvasClipPath(canvas, clipPath, INTERSECT, isAa_);
        } else {
            OH_Drawing_CanvasClipRect(canvas, rect, INTERSECT, isAa_);
        }

        OH_Drawing_CanvasDrawRect(canvas, drawRect);
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_RoundRectDestroy(rrect);
    }
    
    OH_Drawing_RectDestroy(drawRect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}
