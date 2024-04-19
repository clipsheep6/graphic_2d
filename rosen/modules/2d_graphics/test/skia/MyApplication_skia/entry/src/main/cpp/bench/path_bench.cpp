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
#include "common/log_common.h"

void PathBench::OnTestFunction(SkCanvas* canvas)
{

    // 创建一个画刷pen对象
    SkPaint *paint = new SkPaint();
    paint->setStyle(SkPaint::kStroke_Style);
    paint->setColor(SkColorSetARGB(0xFF, 0xFF, 0x00, 0x00));
    paint->setStrokeWidth(10);

    // 创建一个path对象
    SkPath* path = new SkPath();
    MakePath(path);
    canvas->drawPath(*path, *paint);
    DRAWING_LOGI("OnTestFunction path = %{public}s",__FILE_NAME__);
    delete path;
    delete canvas;
    delete paint;
    paint = nullptr;
}

//RectPathBench_AddRect
//path_stroke_smallrect
void PathBench::OnTestPerformance(SkCanvas* canvas)
{
    // 创建一个画刷pen对象
    SkPaint *paint = new SkPaint();
    paint->setStyle(SkPaint::kStroke_Style);
    paint->setAntiAlias(true);
    paint->setColor(SkColorSetARGB(0xFF, 0xFF, 0x00, 0x00));
    paint->setStrokeWidth(10);

    SkPath* path = new SkPath();
    MakePath(path);

    for (int i = 0; i < testCount_; i++) {
        //        OH_Drawing_PathClose(path);
        canvas->drawPath(*path, *paint);
    }

    delete path;
    delete canvas;
    delete paint;
    paint = nullptr;
}

void RectPathBench::MakePath(SkPath* path) {
    // 画一个矩形框
    SkRect r = {0, 0, 200, 200};
    path->addRect(r, SkPathDirection::kCCW);
}

void TrianglePathBench::MakePath(SkPath* path) {
    path->moveTo(100, 100);
    path->lineTo(150, 50);
    path->lineTo(200, 200);
    path->close();
}

void RotatedRectBench::MakePath(SkPath* path) {
    //当前用例名 drawpathrotate 测试 OH_Drawing_MatrixRotate  迁移基于skia pathBench.cpp->RotatedRectBench
    // skia case name : rotated_rect_aa_45
    SkRect r = {0, 0, 200, 200};
    path->addRect(r, SkPathDirection::kCCW);
    SkMatrix* matrix = new SkMatrix();
    matrix->setAll(1, 0, 0, 0, 1, 0, 0, 0, 1);
    matrix->setRotate(fDegrees, 0, 0);
    path->transform(*matrix);
    delete matrix;
}

void LongCurvedPathBench::MakePath(SkPath* path) {
    // 当前用例 drawpathquadto 迁移基于skia pathBench.cpp ->LongCurvedPathBench
    // skia case name : path_fill_small_long_curved

    TestRend rand = TestRend(12);
    int i;
    for (i = 0; i < 100; i++) {
        float base1 = rand.nextUScalar1() * 640;
        float base2 = rand.nextUScalar1() * 480;
        float base3 = rand.nextUScalar1() * 640;
        float base4 = rand.nextUScalar1() * 480;
        path->quadTo(base1, base2, base3, base4);
    }
    
    path->close();    
}