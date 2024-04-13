#include "clip_cubic_gm.h"
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include "test_common.h"
#include "common/log_common.h"

typedef float SkScalar;
const SkScalar W = 100;
const SkScalar H = 240;

ClipCubicGm::ClipCubicGm() {
    //skia dm file gm/clip_cubic_gm.cpp
    bitmapWidth_ = 400;
    bitmapHeight_ = 410;
    fileName_ = "clipcubicgm";
}
uint32_t fSaveCount = 0;

ClipCubicGm::~ClipCubicGm() {

}

void doDraw(OH_Drawing_Canvas *canvas, const OH_Drawing_Path *path) {
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();

    OH_Drawing_BrushSetAntiAlias(brush, true);  
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xCC, 0xCC, 0xCC));
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
}

void drawAndClip(OH_Drawing_Canvas *canvas, const OH_Drawing_Path* path, SkScalar dx, SkScalar dy) {
    fSaveCount = OH_Drawing_CanvasGetSaveCount(canvas);
    DRAWING_LOGI("get fSaveCount=%{public}d ", fSaveCount);
    OH_Drawing_CanvasSave(canvas);

    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, H / 4, W, H / 4 + H / 2);
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_PenSetColor(pen, color_to_565(0xFF8888FF));
    OH_Drawing_BrushSetColor(brush, color_to_565(0xFF8888FF));
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    OH_Drawing_CanvasDrawRect(canvas, rect);
    doDraw(canvas, path);

    OH_Drawing_CanvasTranslate(canvas, dx, dy);

    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasClipRect(canvas, rect, OH_Drawing_CanvasClipOp::INTERSECT, true);
    doDraw(canvas, path);

    OH_Drawing_CanvasRestoreToCount(canvas, fSaveCount);
    DRAWING_LOGI("ClipCubicGm::GetSaveCount count=%{public}d", fSaveCount);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
}

void ClipCubicGm::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path *fVPath = OH_Drawing_PathCreate();
    OH_Drawing_Path *fHPath = OH_Drawing_PathCreate();

    OH_Drawing_PathMoveTo(fVPath, W, 0);
    OH_Drawing_PathCubicTo(fVPath, W, H - 10, 0, 10, 0, H);
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixRotate(matrix, 90, W / 2, H / 2);

    OH_Drawing_PathTransformWithPerspectiveClip(fVPath, matrix, fHPath, true);

    OH_Drawing_CanvasTranslate(canvas, 80, 10);
    drawAndClip(canvas, fVPath, 200, 0);
    OH_Drawing_CanvasTranslate(canvas, 0, 200);
    drawAndClip(canvas, fHPath, 200, 0);

    OH_Drawing_PathDestroy(fVPath);
    OH_Drawing_PathDestroy(fHPath);
    OH_Drawing_MatrixDestroy(matrix);
}

