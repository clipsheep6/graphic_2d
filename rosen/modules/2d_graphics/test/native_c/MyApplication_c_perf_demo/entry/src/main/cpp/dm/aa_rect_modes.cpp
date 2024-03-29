#include "aa_rect_modes.h"
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

static void test4(OH_Drawing_Canvas* canvas) {
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Point2D pts[] = {
        {10, 160}, {610, 160},
        {610, 160}, {10, 160},

        {610, 160}, {610, 160},
        {610, 199}, {610, 199},

        {10, 198}, {610, 198},
        {610, 199}, {10, 199},

        {10, 160}, {10, 160},
        {10, 199}, {10, 199}
    };
    char verbs[] = {
        0, 1, 1, 1, 4,
        0, 1, 1, 1, 4,
        0, 1, 1, 1, 4,
        0, 1, 1, 1, 4
    };
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Point2D* ptPtr = pts;
    for (size_t i = 0; i < sizeof(verbs); ++i) {
        
        switch (verbs[i]) {
            case 0://SkPath::kMove_Verb:
                OH_Drawing_PathMoveTo(path, ptPtr->x, ptPtr->y);
                ++ptPtr;
                break;
            case 1://SkPath::kLine_Verb:
                OH_Drawing_PathLineTo(path, ptPtr->x, ptPtr->y);
                ++ptPtr;
                break;
            case 4://SkPath::kClose_Verb:
                OH_Drawing_PathClose(path);
                break;
            default:
                break;
        }
    }
    OH_Drawing_Rect* clip = OH_Drawing_RectCreate(0, 130, 772, 531);
    OH_Drawing_CanvasClipRect(canvas, clip, OH_Drawing_CanvasClipOp::INTERSECT, true);
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(path);
}


constexpr OH_Drawing_BlendMode gModes[] = {
    OH_Drawing_BlendMode::BLEND_MODE_CLEAR,
    OH_Drawing_BlendMode::BLEND_MODE_SRC,
    OH_Drawing_BlendMode::BLEND_MODE_DST,
    OH_Drawing_BlendMode::BLEND_MODE_SRC_OVER,
    OH_Drawing_BlendMode::BLEND_MODE_DST_OVER,
    OH_Drawing_BlendMode::BLEND_MODE_SRC_IN,
    OH_Drawing_BlendMode::BLEND_MODE_DST_IN,
    OH_Drawing_BlendMode::BLEND_MODE_SRC_OUT,
    OH_Drawing_BlendMode::BLEND_MODE_DST_OUT,
    OH_Drawing_BlendMode::BLEND_MODE_SRC_ATOP,
    OH_Drawing_BlendMode::BLEND_MODE_DST_ATOP,
    OH_Drawing_BlendMode::BLEND_MODE_XOR,
};

const float W = 64;
const float H = 64;

static float drawCell(OH_Drawing_Canvas* canvas, OH_Drawing_BlendMode mode, uint8_t a0, uint8_t a1) {

    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 2.0);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Rect* r = OH_Drawing_RectCreate(W/10,H/10,W-W/10,H-H/10);
    
    OH_Drawing_PenSetColor(pen, 0xFF0000FF);
    OH_Drawing_PenSetAlpha(pen, a0);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawOval(canvas, r);

    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetAlpha(pen, a1);
    OH_Drawing_PenSetBlendMode(pen, mode);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    float offset = 1.0 / 3;
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(W / 4 + offset,
                                                  H / 4 + offset,
                                                  W / 4 + offset+W / 2, 
                                                  H / 4 + offset+H / 2);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(r);
    return H;
}

OH_Drawing_ShaderEffect *make_bg_shader()
{
//    OH_Drawing_BitmapFormat format;
//    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
//    OH_Drawing_BitmapBuild(bitmap, W, H, &format);
    //  OH没有 bitmap生成的shaderEffect 用OH_Drawing_ShaderEffectCreateLinearGradient 替代,但似乎没有效果
    
    OH_Drawing_Point *startPt = OH_Drawing_PointCreate(0, 0);
    OH_Drawing_Point *endPt = OH_Drawing_PointCreate(W, H);
    static const uint32_t gColors[] = { 0xFF000000, 0xFFFFFFFF };
    OH_Drawing_ShaderEffect *shaderEffect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, gColors, nullptr, 2, OH_Drawing_TileMode::REPEAT);
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
    return shaderEffect;
}

AARectModes::AARectModes() {
    bitmapWidth_ = 640;
    bitmapHeight_ = 480;
    fileName_ = "aarectmodes";
}

AARectModes::~AARectModes() {

}

void AARectModes::OnTestFunction(OH_Drawing_Canvas* canvas)
{

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush,true);
    OH_Drawing_ShaderEffect *shaderEffect = make_bg_shader();
    OH_Drawing_BrushSetColor(brush, 0xFFFFFFFF);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    if(false)
        test4(canvas);
    
    OH_Drawing_Rect* bounds = OH_Drawing_RectCreate(0, 0, W, H);
    uint8_t gAlphaValue[] = { 0xFF, 0x88, 0x88 };
    OH_Drawing_CanvasTranslate(canvas, 4.0, 4.0);
    
    for (int alpha = 0; alpha < 4; ++alpha) {
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasSave(canvas);

        for (size_t i = 0; i < sizeof(gModes)/sizeof(gModes[0]); ++i) {
            if (6 == i) {
                OH_Drawing_CanvasRestore(canvas);
                OH_Drawing_CanvasTranslate(canvas, W * 5.0, 0);
                OH_Drawing_CanvasSave(canvas);
            }
            
            OH_Drawing_CanvasDrawRect(canvas, bounds);
            OH_Drawing_CanvasSaveLayer(canvas, bounds, brush);
            float dy = drawCell(canvas, gModes[i],
                                   gAlphaValue[alpha & 1],
                                   gAlphaValue[alpha & 2]);
            OH_Drawing_CanvasRestore(canvas);
            OH_Drawing_CanvasTranslate(canvas, 0, dy * 5 / 4);
        }
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, W * 5 / 4, 0);
    }
    OH_Drawing_RectDestroy(bounds);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_BrushDestroy(brush);
    
}
