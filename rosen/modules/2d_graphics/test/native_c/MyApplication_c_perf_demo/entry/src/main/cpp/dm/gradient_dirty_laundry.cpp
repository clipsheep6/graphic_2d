#include "gradient_dirty_laundry.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <time.h>
#include <array>
#include "common/log_common.h"
#include "test_common.h"


enum {
    WIDTH = 640,
    HEIGHT = 615
};

struct GradData {
    int fCount = 40;
    const uint32_t *fColors;
    const float *fPos;
};

static const uint32_t gColors[] = {
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000, // 10 lines, 50 colors
};
// static const float_t fPosa[] = {4.00f, 4.00f};
// constexpr GradData gGradData[] = {
//    {40, gColors, fPosa},
// };
constexpr SkScalar gPos[] = {0.25f, 0.75f};
// constexpr SkScalar gPos[] = {0.00f, 0.00f};
// static const uint32_t gShallowColors[] = {0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000};
// constexpr SkScalar gPos[] = {SK_Scalar1 * 999 / 2000, SK_Scalar1 * 999 / 2000};
constexpr GradData gGradData[] = {
    {40, gColors, gPos},
    //    {2, gShallowColors, gPos},
    //      { 2, gColors, gPos },
};
GradData data = gGradData[0];
// 圆心和圆半径坐标
OH_Drawing_Point2D pts[] = {{0, 0}, {SkIntToScalar(100), SkIntToScalar(100)}};
OH_Drawing_Point2D pt[] = {SkScalarAve(pts[0].x, pts[1].x), SkScalarAve(pts[0].y, pts[1].y)};
OH_Drawing_Point2D Point = pt[0];
float x = Point.x;
float y = Point.y;
OH_Drawing_Point *centerPt = OH_Drawing_PointCreate(Point.x, Point.y);
//  渐变的起点和终点坐标
OH_Drawing_Point *firstPoint = OH_Drawing_PointCreate(0, 0);
OH_Drawing_Point *secondPoint = OH_Drawing_PointCreate(SkIntToScalar(100), SkIntToScalar(100));
int fCount = 40;
// 定义三种着色器
OH_Drawing_ShaderEffect *MakeLinear(const OH_Drawing_Point *firstPoint, const OH_Drawing_Point *secondPoint,
    const GradData &data, OH_Drawing_TileMode tileMode)
{
    DRAWING_LOGI("GradientsGM MakeLinear");
    OH_Drawing_ShaderEffect *MakeLinear = OH_Drawing_ShaderEffectCreateLinearGradient(firstPoint, secondPoint,
        data.fColors, data.fPos, fCount, OH_Drawing_TileMode::CLAMP);
    DRAWING_LOGI("GradientsGM firstPoint = %{public}p,secondPoint = %{public}p,fColors = "
                 "%{public}u,fPos=%{public}f,fCount=%{public}d",
        firstPoint, secondPoint, data.fColors, data.fPos, fCount);
    DRAWING_LOGI("GradientsGM MakeLinear = %{public}p", MakeLinear);
    return MakeLinear;
}
OH_Drawing_ShaderEffect *MakeRadial(const OH_Drawing_Point *firstPoint, const OH_Drawing_Point *secondPoint,
    const GradData &data, OH_Drawing_TileMode tileMode)
{
    DRAWING_LOGI("GradientsGM MakeRadial");
    OH_Drawing_ShaderEffect *MakeRadial = OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, pt[0].x, data.fColors,
        data.fPos, fCount, OH_Drawing_TileMode::REPEAT);
    DRAWING_LOGI(
        "GradientsGM centerPt = %{public}p,pt[0].x = %{public}f,fColors = %{public}u,fPos=%{public}f,fCount=%{public}d",
        centerPt, pt[0].x, data.fColors, data.fPos, fCount);
    DRAWING_LOGI("GradientsGM MakeRadial = %{public}p", MakeRadial);
    return MakeRadial;
}
OH_Drawing_ShaderEffect *MakeSweep(const OH_Drawing_Point *firstPoint, const OH_Drawing_Point *secondPoint,
    const GradData &data, OH_Drawing_TileMode tileMode)
{
    DRAWING_LOGI("GradientsGM MakeSweep");
    OH_Drawing_ShaderEffect *MakeSweep = OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, data.fColors, data.fPos,
        fCount, OH_Drawing_TileMode::DECAL);
    DRAWING_LOGI("GradientsGM centerPt = %{public}p, fColors = %{public}u,fPos=%{public}f,fCount=%{public}d", centerPt,
        data.fColors, data.fPos, fCount);
    DRAWING_LOGI("GradientsGM MakeSweep = %{public}p", MakeSweep);
    return MakeSweep;
}

typedef OH_Drawing_ShaderEffect *(*GradMaker)(const OH_Drawing_Point *firstPoint, const OH_Drawing_Point *secondPoint,
    const GradData &, OH_Drawing_TileMode);
constexpr GradMaker gGradMakers[] = {
    MakeLinear,
    MakeRadial,
    MakeSweep,
};

GradientsGM::GradientsGM()
{
    // skia dm file gm/gradient_dirty_laundry.cpp
    bitmapWidth_ = WIDTH;
    bitmapHeight_ = HEIGHT;
    fileName_ = "gradient_dirty_laundry";
}

void GradientsGM::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    DRAWING_LOGI("GradientsGM::OnTestFunction start");
    DRAWING_LOGI("GradientsGM centerPt = %{public}p,pt[0].x = %{public}f,firstPoint = %{public}p,secondPoint = "
                 "%{public}p,fColors = %{public}u,fPos=%{public}f,fCount=%{public}d",
        centerPt, pt[0].x, firstPoint, secondPoint, data.fColors, data.fPos, data.fCount);
    // 使用指定颜色设置清空画布底色
    OH_Drawing_CanvasClear(canvas, 0xFFDDDDDD);
    // 创建一个画刷brush对象设置抗锯齿属性
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    //        OH_Drawing_CanvasAttachBrush(canvas, brush);
    // //     创建一个画刷pen对象设置抗锯齿属性
    //        OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    //        OH_Drawing_PenSetWidth(pen, 2);
    //        OH_Drawing_PenSetAntiAlias(pen, true);

    // 创建一个矩形对象
    OH_Drawing_Rect *rectAngLe = OH_Drawing_RectCreate(0, 0, SkIntToScalar(100), SkIntToScalar(100));
    // 平移画布
    OH_Drawing_CanvasTranslate(canvas, SkIntToScalar(20), SkIntToScalar(20));

    for (size_t i = 0; i < SK_ARRAY_COUNT(gGradData); i++) {
        DRAWING_LOGI("GradientsGM::OnTestFunction i = %{public}d", i);
        OH_Drawing_CanvasSave(canvas);
        for (size_t j = 0; j < SK_ARRAY_COUNT(gGradMakers); j++) {
            DRAWING_LOGI("GradientsGM::OnTestFunction j = %{public}d", j);
            OH_Drawing_BrushSetShaderEffect(brush,
                gGradMakers[j](firstPoint, secondPoint, gGradData[i], OH_Drawing_TileMode::CLAMP));
            // OH_Drawing_PenSetShaderEffect(pen, gGradMakers[j](firstPoint, secondPoint, gGradData[i],
            // OH_Drawing_TileMode::CLAMP));

            OH_Drawing_CanvasAttachBrush(canvas, brush);
            // OH_Drawing_CanvasAttachPen(canvas, pen);
            OH_Drawing_CanvasDrawRect(canvas, rectAngLe);

            OH_Drawing_CanvasTranslate(canvas, 0, SkIntToScalar(120));


            DRAWING_LOGI("GradientsGM::OnTestFunction gGradMakers_j = %{public}p",
                gGradMakers[j](firstPoint, secondPoint, gGradData[i], OH_Drawing_TileMode::CLAMP));
        }
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, SkIntToScalar(120), 0);
    }
    DRAWING_LOGI("GradientsGM::OnTestFunction end");

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_RectDestroy(rectAngLe);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
    //        OH_Drawing_CanvasDetachPen(canvas);
    //        OH_Drawing_PenDestroy(pen);
    //        pen = nullptr;
}
