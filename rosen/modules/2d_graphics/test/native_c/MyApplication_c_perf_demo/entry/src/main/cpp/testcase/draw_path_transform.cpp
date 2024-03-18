#include "draw_path_transform.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_matrix.h>
#include <time.h>
#include "common/log_common.h"
#include "test_common.h"

static const int points[] = {
    10, 10, 15, 5, 20, 20,
    30, 5, 25, 20, 15, 12,
    21, 21, 30, 30, 12, 4,
    32, 28, 20, 18, 12, 10
};

static const int kMaxPathSize = 10;

void DrawPathTransform::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetWidth(pen, 0.0);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    // 在画布上画path的形状
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Matrix* m = OH_Drawing_MatrixCreateScale(3,3,300,300);
    OH_Drawing_PathTransform(path, m);
    
    for(int j=0;j<100;++j)
        OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_MatrixDestroy(m);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void DrawPathTransform::OnTestPerformance(OH_Drawing_Canvas* canvas)
{   
    //当前用例名 drawpathtransform OH_Drawing_PathTransform  迁移基于skia HairlinePathBench.cpp->HairlinePathBench
    // skia case name : path_hairline_big_AA_Cubic
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetWidth(pen, 0.0);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    // 在画布上画path的形状
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
//    makePathLine(path);
//    makePathQuad(path);
    makePathCubic(path);
    OH_Drawing_Matrix* m = OH_Drawing_MatrixCreateScale(3,3,0,0);
    OH_Drawing_PathTransform(path, m);
    
    for (int i = 0; i < testCount_; i++) {
        for(int j=0;j<100;++j)
            OH_Drawing_CanvasDrawPath(canvas, path);
    }
    OH_Drawing_MatrixDestroy(m);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void DrawPathTransform::makePathLine(OH_Drawing_Path *path)
{
    TestRend rand = TestRend();
    int size = sizeof(points)/sizeof(points[0]);
    int hSize = size / 2;
    for (int i = 0; i < kMaxPathSize; ++i) {
        int xTrans = 10 + 40 * (i%(kMaxPathSize/2));
        int yTrans = 0;
        if (i > kMaxPathSize/2 - 1) {
            yTrans = 40;
        }
        int base1 = 2 * rand.nextULessThan(hSize);
        int base2 = 2 * rand.nextULessThan(hSize);
        int base3 = 2 * rand.nextULessThan(hSize);
        OH_Drawing_PathMoveTo(path, float(points[base1] + xTrans), float(points[base1+1] + yTrans));
        OH_Drawing_PathLineTo(path, float(points[base2] + xTrans), float(points[base2+1] + yTrans));
        OH_Drawing_PathLineTo(path, float(points[base3] + xTrans), float(points[base3+1] + yTrans));
    }    
}

void DrawPathTransform::makePathQuad(OH_Drawing_Path *path)
{
    TestRend rand = TestRend();
    int size = sizeof(points)/sizeof(points[0]);
    int hSize = size / 2;
    for (int i = 0; i < kMaxPathSize; ++i) {
        int xTrans = 10 + 40 * (i%(kMaxPathSize/2));
        int yTrans = 0;
        if (i > kMaxPathSize/2 - 1) {
            yTrans = 40;
        }
        int base1 = 2 * rand.nextULessThan(hSize);
        int base2 = 2 * rand.nextULessThan(hSize);
        int base3 = 2 * rand.nextULessThan(hSize);
        OH_Drawing_PathMoveTo(path, float(points[base1] + xTrans), float(points[base1+1] + yTrans));
        OH_Drawing_PathQuadTo(path, float(points[base2] + xTrans), float(points[base2+1] + yTrans),
                                    float(points[base3] + xTrans), float(points[base3+1] + yTrans));
    }      
}

void DrawPathTransform::makePathConic(OH_Drawing_Path *path)
{
  
}

void DrawPathTransform::makePathCubic(OH_Drawing_Path *path)
{
    TestRend rand = TestRend();
    int size = sizeof(points)/sizeof(points[0]);
    int hSize = size / 2;
    for (int i = 0; i < kMaxPathSize; ++i) {
        int xTrans = 10 + 40 * (i%(kMaxPathSize/2));
        int yTrans = 0;
        if (i > kMaxPathSize/2 - 1) {
            yTrans = 40;
        }
        int base1 = 2 * rand.nextULessThan(hSize);
        int base2 = 2 * rand.nextULessThan(hSize);
        int base3 = 2 * rand.nextULessThan(hSize);
        int base4 = 2 * rand.nextULessThan(hSize);
        OH_Drawing_PathMoveTo(path, float(points[base1] + xTrans), float(points[base1+1] + yTrans));
        OH_Drawing_PathCubicTo(path, float(points[base2] + xTrans), float(points[base2+1] + yTrans),
                                    float(points[base3] + xTrans), float(points[base3+1] + yTrans),
                                    float(points[base4] + xTrans), float(points[base4+1] + yTrans));
    }       
}
