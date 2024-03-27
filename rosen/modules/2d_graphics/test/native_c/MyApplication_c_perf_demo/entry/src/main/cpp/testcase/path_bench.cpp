#include "path_bench.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <string>
#include <time.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include "test_common.h"

typedef float SkScalar;
struct SkRect {
    SkScalar fLeft;   //!< smaller x-axis bounds
    SkScalar fTop;    //!< smaller y-axis bounds
    SkScalar fRight;  //!< larger x-axis bounds
    SkScalar fBottom; //!< larger y-axis bounds

    bool contains(SkScalar x, SkScalar y) const { return x >= fLeft && x < fRight && y >= fTop && y < fBottom; }
};

void PathBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{

    // 创建一个画刷pen对象
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 创建一个path对象
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    MakePath(path);
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_PathDestroy(path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

//RectPathBench_AddRect
//path_stroke_smallrect
void PathBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 创建一个画刷pen对象
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    MakePath(path);
    
    for (int i = 0; i < testCount_; i++) {
        //        OH_Drawing_PathClose(path);
        OH_Drawing_CanvasDrawPath(canvas, path);
    }

    OH_Drawing_PathDestroy(path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void RectPathBench::MakePath(OH_Drawing_Path* path) {
    // 画一个矩形框
    SkRect r = {0, 0, 200, 200};
    OH_Drawing_PathAddRect(path, r.fLeft, r.fTop, r.fRight, r.fBottom,
                               OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
}

void TrianglePathBench::MakePath(OH_Drawing_Path* path) {
    OH_Drawing_PathMoveTo(path, 100, 100);
    OH_Drawing_PathLineTo(path, 150, 50);
    OH_Drawing_PathLineTo(path, 200, 200);
    OH_Drawing_PathClose(path);
}

void RotatedRectBench::MakePath(OH_Drawing_Path* path) {
    //当前用例名 drawpathrotate 测试 OH_Drawing_MatrixRotate  迁移基于skia pathBench.cpp->RotatedRectBench
    // skia case name : rotated_rect_aa_45
    SkRect r = {0, 0, 200, 200};
    OH_Drawing_PathAddRect(path, r.fLeft, r.fTop, r.fRight, r.fBottom,
                               OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 1, 0, 0, 0, 1, 0, 0, 0, 1);
    OH_Drawing_MatrixRotate(matrix, fDegrees, 0, 0);
    OH_Drawing_PathTransform(path, matrix);
}

void LongCurvedPathBench::MakePath(OH_Drawing_Path* path) {
    // 当前用例 drawpathquadto 迁移基于skia pathBench.cpp ->LongCurvedPathBench
    // skia case name : path_fill_small_long_curved

    TestRend rand = TestRend(12);
    float base1 = rand.nextUScalar1() * 640;
    float base2 = rand.nextUScalar1() * 480;
    float base3 = rand.nextUScalar1() * 640;
    float base4 = rand.nextUScalar1() * 480;
    OH_Drawing_PathQuadTo(path, base1, base2, base3, base4);
    OH_Drawing_PathClose(path);    
}