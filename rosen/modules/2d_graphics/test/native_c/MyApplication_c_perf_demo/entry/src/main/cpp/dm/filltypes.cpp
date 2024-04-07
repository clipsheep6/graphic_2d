#include "filltypes.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_text_blob.h>
#include "test_common.h"
#include "common/log_common.h"
#include <array>

FillTypeGM::FillTypeGM()
{
    bitmapWidth_ = 835;
    bitmapHeight_ = 840;
    fileName_ = "filltypes";
}
// 创建路径path对象
OH_Drawing_Path *path = OH_Drawing_PathCreate();

// 用例名: filltypes
// 测试OH_Drawing_PathSetFillType迁移基于skia gm\filltypes.cpp->dm\filltypes.cpp
void FillTypeGM::showPath(OH_Drawing_Canvas *canvas, int x, int y, SkPathFillType ft, SkScalar scale,
    OH_Drawing_Brush *brush)
{
    DRAWING_LOGI("FillTypeGM::showPath start");
    // 创建一个矩形对象为图像画布
    float right = 150, bottom = 150;
    OH_Drawing_Rect *rectAngLe = OH_Drawing_RectCreate(0, 0, right, bottom);
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_CanvasTranslate(canvas, x, y);
    OH_Drawing_CanvasClipRect(canvas, rectAngLe, OH_Drawing_CanvasClipOp::INTERSECT, true);
    DRAWING_LOGI("FillTypeGM::showPath rectAngLe=%{public}p", rectAngLe);
    // 使用指定白色清空画布底色
    OH_Drawing_CanvasClear(canvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));

    if (ft == kWinding) {
        DRAWING_LOGI("FillTypeGM::showPath kWinding");
        OH_Drawing_PathSetFillType(path, OH_Drawing_PathFillType::PATH_FILL_TYPE_WINDING);
    } else if (ft == kEvenOdd) {
        DRAWING_LOGI("FillTypeGM::showPath kEvenOdd");
        OH_Drawing_PathSetFillType(path, OH_Drawing_PathFillType::PATH_FILL_TYPE_EVEN_ODD);
    } else if (ft == kInverseWinding) {
        DRAWING_LOGI("FillTypeGM::showPath kInverseWinding");
        OH_Drawing_PathSetFillType(path, OH_Drawing_PathFillType::PATH_FILL_TYPE_INVERSE_WINDING);
    } else if (ft == kInverseEvenOdd) {
        DRAWING_LOGI("FillTypeGM::showPath kInverseEvenOdd");
        OH_Drawing_PathSetFillType(path, OH_Drawing_PathFillType::PATH_FILL_TYPE_INVERSE_EVEN_ODD);
    } else {
        OH_Drawing_PathSetFillType(path, OH_Drawing_PathFillType::PATH_FILL_TYPE_WINDING);
        DRAWING_LOGI("FillTypeGM::showPath a default rule");
    }
    // 其中这里平移画布的x=rectAngLe.left + rectAngLe.right.y=rectAngLe.top + rectAngLe.bottom
    OH_Drawing_CanvasTranslate(canvas, right * SK_ScalarHalf, bottom * SK_ScalarHalf);
    DRAWING_LOGI("FillTypeGM::showPath right * SK_ScalarHalf=%{public}f bottom * SK_ScalarHalf=%{public}f",
        right * SK_ScalarHalf, bottom * SK_ScalarHalf);
    OH_Drawing_CanvasScale(canvas, scale, scale);
    DRAWING_LOGI("FillTypeGM::showPath scale=%{public}f", scale);
    OH_Drawing_CanvasTranslate(canvas, -(right * SK_ScalarHalf), -(bottom * SK_ScalarHalf));
    DRAWING_LOGI("FillTypeGM::showPath -(right * SK_ScalarHalf)=%{public}f -(bottom * SK_ScalarHalf)=%{public}f",
        -(right * SK_ScalarHalf), -(bottom * SK_ScalarHalf));
    // 上画布
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasRestore(canvas);
    DRAWING_LOGI("FillTypeGM::showPath end");
}

// FillTypeGM::showFour 左上角、右上角、左下角、右下角四个为一组选择枚举类型进行变换
void FillTypeGM::showFour(OH_Drawing_Canvas *canvas, SkScalar scale, OH_Drawing_Brush *brush)
{
    DRAWING_LOGI("FillTypeGM::showFour start");
    showPath(canvas, 0, 0, kWinding, scale, brush);
    showPath(canvas, 200, 0, kEvenOdd, scale, brush);
    showPath(canvas, 00, 200, kInverseWinding, scale, brush);
    showPath(canvas, 200, 200, kInverseEvenOdd, scale, brush);
    DRAWING_LOGI("FillTypeGM::showFour end");
}


void FillTypeGM::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    DRAWING_LOGI("FillTypeGM::OnTestFunction start");
    // 创建画刷brush对象
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    // 使用指定颜色设置清空画布底色
    OH_Drawing_CanvasClear(canvas, 0xFFDDDDDD);
    // 创建矩形对象并将弧添加到路径中
    float x = 50;
    float y = 50;
    float r = 45;
    float rects_x = 100;
    float rects_y = 100;
    float rects_r = 45;
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(x - r, y - r, x + r, y + r);
    OH_Drawing_Rect *rects =
        OH_Drawing_RectCreate(rects_x - rects_r, rects_y - rects_r, rects_x + rects_r, rects_y + rects_r);
    OH_Drawing_PathAddArc(path, rect, 0, 360);
    OH_Drawing_PathAddArc(path, rects, 0, 360);
    DRAWING_LOGI("FillTypeGM::OnTestFunction rect=%{public}p rects =%{public}p", rect, rects);

    // 上述前置动作已做完
    OH_Drawing_CanvasTranslate(canvas, 20, 20);
    const SkScalar scale = 1.25;
    // scale：左侧两个与右侧两个的图像放大缩小
    OH_Drawing_BrushSetAntiAlias(brush, false);
    showFour(canvas, SK_Scalar1, brush);
    OH_Drawing_CanvasTranslate(canvas, 450, 0);
    showFour(canvas, scale, brush);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasTranslate(canvas, -450, 450);
    showFour(canvas, SK_Scalar1, brush);
    OH_Drawing_CanvasTranslate(canvas, 450, 0);
    showFour(canvas, scale, brush);

    DRAWING_LOGI("FillTypeGM::OnTestFunction end");

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rects);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
}
