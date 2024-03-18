#include "draw_path_add_rect.h"
#include <string>
#include <time.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>

typedef float SkScalar;
struct SkRect {
    SkScalar fLeft;   //!< smaller x-axis bounds
    SkScalar fTop;    //!< smaller y-axis bounds
    SkScalar fRight;  //!< larger x-axis bounds
    SkScalar fBottom; //!< larger y-axis bounds

    bool contains(SkScalar x, SkScalar y) const { return x >= fLeft && x < fRight && y >= fTop && y < fBottom; }
};

//RectPathBench_AddRect
void DrawPathAddRect::OnTestFunction(OH_Drawing_Canvas *canvas) {

    // 创建一个画刷pen对象
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 创建一个path对象
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    // 画一个矩形框
    SkRect r = {0, 0, 200, 200};
    OH_Drawing_PathAddRect(path, r.fLeft, r.fTop, r.fRight, r.fBottom, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_PathDestroy(path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

//RectPathBench_AddRect
//path_stroke_smallrect
void DrawPathAddRect::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // 创建一个画刷pen对象
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 创建一个path对象
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    // 画一个矩形框
    SkRect r = {0, 0, 200, 200};
    OH_Drawing_PathAddRect(path, r.fLeft, r.fTop, r.fRight, r.fBottom,
                               OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    for (int i = 0; i < testCount_; i++) {
        //        OH_Drawing_PathClose(path);
        OH_Drawing_CanvasDrawPath(canvas, path);
    }

    OH_Drawing_PathDestroy(path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}