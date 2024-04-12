#include "fill_types_persp.h"
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
    kW=835,
    kH=840,
};
FillTypesPersp::FillTypesPersp() {
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "filltypespersp";
}

void FillTypesPersp::makePath()
{
    fPath = OH_Drawing_PathCreate();
    DrawPathAddCircle(fPath, 50, 50, 45);
    DrawPathAddCircle(fPath, 100, 100, 45);
}

void FillTypesPersp::showPath(OH_Drawing_Canvas* canvas, int x, int y, OH_Drawing_PathFillType ft, float scale, OH_Drawing_Brush* brush)
{
    DrawRect r={0,0,150,150};
    OH_Drawing_Rect* rc = DrawCreateRect(r);
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_CanvasTranslate(canvas, x, y);
    OH_Drawing_CanvasClipRect(canvas, rc, OH_Drawing_CanvasClipOp::INTERSECT, true);
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    OH_Drawing_PathSetFillType(fPath, ft);
    OH_Drawing_CanvasTranslate(canvas, r.centerX(), r.centerY());
    OH_Drawing_CanvasScale(canvas, scale, scale);
    OH_Drawing_CanvasTranslate(canvas, -r.centerX(), -r.centerY());
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawPath(canvas, fPath);
    OH_Drawing_RectDestroy(rc);
    OH_Drawing_CanvasRestore(canvas);
}

void FillTypesPersp::showFour(OH_Drawing_Canvas* canvas, float scale, bool aa)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Point* center = OH_Drawing_PointCreate(100, 100);
    uint32_t colors[] = {DRAW_ColorBLUE, DRAW_ColorRED,DRAW_ColorGREEN};
    float pos[] = {0,0.5,1.0};
    OH_Drawing_ShaderEffect* effect = OH_Drawing_ShaderEffectCreateRadialGradient(center, 100, colors, pos, 3, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_BrushSetShaderEffect(brush, effect);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    showPath(canvas, 0, 0, OH_Drawing_PathFillType::PATH_FILL_TYPE_WINDING, scale, brush);
    showPath(canvas, 200, 0, OH_Drawing_PathFillType::PATH_FILL_TYPE_EVEN_ODD, scale, brush);
    showPath(canvas, 0, 200, OH_Drawing_PathFillType::PATH_FILL_TYPE_INVERSE_WINDING, scale, brush);
    showPath(canvas, 200, 200, OH_Drawing_PathFillType::PATH_FILL_TYPE_INVERSE_EVEN_ODD, scale, brush);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_ShaderEffectDestroy(effect);
    OH_Drawing_BrushDestroy(brush);
 }

void FillTypesPersp::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    makePath();
    OH_Drawing_Brush* bkgnrd = OH_Drawing_BrushCreate();;
    OH_Drawing_Point* center = OH_Drawing_PointCreate(100, 100);
    uint32_t colors[] = {DRAW_ColorBLACK, DRAW_ColorCYAN, DRAW_ColorYELLOW,DRAW_ColorWHITE};
    float pos[] = {0,0.25,0.75,1.0};
    OH_Drawing_ShaderEffect* effect = OH_Drawing_ShaderEffectCreateRadialGradient(center, 1000, colors, pos, 4, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_BrushSetShaderEffect(bkgnrd, effect);
    OH_Drawing_CanvasAttachBrush(canvas, bkgnrd);
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_CanvasTranslate(canvas, 100, 100);

    OH_Drawing_Matrix *mat = OH_Drawing_MatrixCreate();
    //            | scaleX  skewX transX |
    //            |  skewY scaleY transY |
    //            | persp0 persp1 persp2 |
    OH_Drawing_MatrixSetMatrix(mat, 1, 0, 0,
                               0, 1, 0,
                               0, 1.0 / 1000, 1);
    OH_Drawing_CanvasConcatMatrix(canvas, mat);
    OH_Drawing_CanvasDrawBackground(canvas, bkgnrd);
    OH_Drawing_CanvasRestore(canvas);
    
    OH_Drawing_Matrix* persp = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(persp, 1, 0, 0, 
                                    0, 1, 0, 
                                    -1.0/1800, 1.0/500, 1);    
    OH_Drawing_CanvasConcatMatrix(canvas, persp);
    OH_Drawing_CanvasTranslate(canvas, 20, 20);
    
    float scale  = 5.0/4;
    showFour(canvas, 1.0, false);
    OH_Drawing_CanvasTranslate(canvas, 450, 0);
    showFour(canvas, scale, false);    
 
    OH_Drawing_CanvasTranslate(canvas, -450, 450);
    showFour(canvas, 1, true);    
    OH_Drawing_CanvasTranslate(canvas, 450, 0);
    showFour(canvas, scale, true);      
    
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_ShaderEffectDestroy(effect);
    OH_Drawing_BrushDestroy(bkgnrd);
}
