#include "bugfix7792.h"
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


BugFix7792::BugFix7792()
{
    // skia dm file gm/pathfill.cpp
    bitmapWidth_ = 800;
    bitmapHeight_ = 800;
    fileName_ = "bug7792";
}

BugFix7792::~BugFix7792() {}

void BugFix7792::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    // 用例名: bug7792 测试 OH_Drawing_PathReset
    // 迁移基于skia gm\pathfill.cpp->dm\bugfix7792.cpp
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    // 创建一个brush对象
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    //  图型坐标点： y
    //   x    0 1 2 3
    //        1 2 3 4
    //        2 3 4 5
    //        3 4 5 6
    // 第一个图型坐标点:（0.0）
    OH_Drawing_PathMoveTo(path, 10, 10);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第二个图型坐标点:（0.1）
    OH_Drawing_CanvasTranslate(canvas, 200, 0);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 50);
    OH_Drawing_PathMoveTo(path, 100, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathLineTo(path, 75, 50);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第三个图型坐标点:（0.2）
    OH_Drawing_CanvasTranslate(canvas, 200, 0);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 10, 10);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第四个图型坐标点:（0.4）
    OH_Drawing_CanvasTranslate(canvas, -200 * 2, 200);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 150);
    OH_Drawing_PathLineTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathMoveTo(path, 75, 150);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第五个图型坐标点:（1.1）
    OH_Drawing_CanvasTranslate(canvas, 200, 0);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 250, 75);
    OH_Drawing_PathMoveTo(path, 250, 75);
    OH_Drawing_PathMoveTo(path, 250, 75);
    OH_Drawing_PathMoveTo(path, 100, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathLineTo(path, 75, 75);
    OH_Drawing_PathClose(path);
    OH_Drawing_PathLineTo(path, 0, 0);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第六个图型坐标点:（1.2）
    OH_Drawing_CanvasTranslate(canvas, 200, 0);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathMoveTo(path, 250, 75);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第七个图型坐标点:（1.3）
    OH_Drawing_CanvasTranslate(canvas, -200 * 2, 200);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 10);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathLineTo(path, 75, 10);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第八个图型坐标点:（1.4）
    OH_Drawing_CanvasTranslate(canvas, 200, 0);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathClose(path);
    OH_Drawing_PathMoveTo(path, 10, 10);
    OH_Drawing_PathLineTo(path, 30, 10);
    OH_Drawing_PathLineTo(path, 10, 30);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第九个图型坐标点:（2.2）
    OH_Drawing_CanvasTranslate(canvas, 200, 0);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 75, 75);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十个图型坐标点:（2.3）
    OH_Drawing_CanvasTranslate(canvas, -200 * 2, 200);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathLineTo(path, 75, 250);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十一个图型坐标点:（2.4）
    OH_Drawing_CanvasTranslate(canvas, 200, 0);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathLineTo(path, 75, 10);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十二个图型坐标点:（2.5）
    OH_Drawing_CanvasTranslate(canvas, 200, 0);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 10, 150);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 75, 75);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十三个图型坐标点:（3.3）
    OH_Drawing_CanvasTranslate(canvas, 200, -200 * 3);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathLineTo(path, 75, 100);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十四个图型坐标点:（3.4）
    OH_Drawing_CanvasTranslate(canvas, 0, 200);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 150, 100);
    OH_Drawing_PathLineTo(path, 150, 100);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 75, 150);
    OH_Drawing_PathLineTo(path, 75, 100);
    OH_Drawing_PathLineTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十五个图型坐标点:（3.5）
    OH_Drawing_CanvasTranslate(canvas, 0, 200);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 140, 150);
    OH_Drawing_PathLineTo(path, 140, 75);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 第十六个图型坐标点:（3.6）
    OH_Drawing_CanvasTranslate(canvas, 0, 200);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathLineTo(path, 150, 75);
    OH_Drawing_PathLineTo(path, 150, 150);
    OH_Drawing_PathLineTo(path, 140, 150);
    OH_Drawing_PathLineTo(path, 140, 75);
    OH_Drawing_PathMoveTo(path, 75, 75);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // 释放内存
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(nullptr);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
}
