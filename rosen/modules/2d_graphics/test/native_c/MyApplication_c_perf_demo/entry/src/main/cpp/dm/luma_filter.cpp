#include "luma_filter.h"
#include "bench/random_path_bench.h"
#include "test_common.h"
#include <native_drawing/drawing_font_mgr.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_typeface.h>
#include "common/log_common.h"

enum {
    kW = 600,
    kH = 420,
};

LumaFilter::LumaFilter() {
    // skia dm file gm/lumafilter.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "lumafilter";
}

uint32_t ColorSetA(uint32_t c, uint8_t a) {
    return (c & 0x00FFFFFF) | (a << 24);
}

static float kSize = 80;
static float kInset = 10;

uint32_t kColor1 = 0xFFFFFF00;
uint32_t kColor2 = 0xFF82FF00;

const char *gModeStrings[] = {
    "Clear", "Src", "Dst", "SrcOver", "DstOver", "SrcIn", "DstIn",
    "SrcOut", "DstOut", "SrcATop", "DstATop", "Xor", "Plus",
    "Modulate", "Screen", "Overlay", "Darken", "Lighten", "ColorDodge",
    "ColorBurn", "HardLight", "SoftLight", "Difference", "Exclusion",
    "Multiply", "Hue", "Saturation", "Color", "Luminosity"};

void draw_label(OH_Drawing_Canvas *canvas, const char *label, OH_Drawing_Point2D offset) {
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    OH_Drawing_Typeface *type = OH_Drawing_TypefaceCreateDefault();
    // skia默认字体和OH默认字体不一致
    OH_Drawing_FontSetTypeface(font, type);
    size_t len = strlen(label);

    // 缺乏计算文本宽度的接口，OH_Drawing_FontCountText临时替代
    int width = OH_Drawing_FontCountText(font, label, len, TEXT_ENCODING_UTF8);
    OH_Drawing_FontGetTypeface(font);
    OH_Drawing_TextBlob *blob = OH_Drawing_TextBlobCreateFromText(label, len, font, TEXT_ENCODING_UTF8);

    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawTextBlob(canvas, blob, offset.x - width / 2, offset.y);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_TypefaceDestroy(type);
    OH_Drawing_TextBlobDestroy(blob);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

void draw_scene(OH_Drawing_Canvas *canvas, OH_Drawing_ColorFilter *cFilter, OH_Drawing_BlendMode mode, OH_Drawing_ShaderEffect *s1, OH_Drawing_ShaderEffect *s2) {
    DrawRect r, c, bounds;
    bounds = {0, 0, kSize, kSize};
    r = bounds;
    c = bounds;
    c.fRight = bounds.centerX();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(r.fLeft, r.fTop, r.fRight, r.fBottom);
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetColor(brush, 0x200000FF);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasSaveLayer(canvas, rect, nullptr);
    OH_Drawing_RectDestroy(rect);

    r = bounds;
    r.inset(kInset, 0);
    rect = OH_Drawing_RectCreate(r.fLeft, r.fTop, r.fRight, r.fBottom);
    OH_Drawing_BrushSetShaderEffect(brush, s1);
    OH_Drawing_BrushSetColor(brush, s1 ? 0xFF000000 : ColorSetA(kColor1, 0x80));
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawOval(canvas, rect);

    if (!s1) {
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_Rect *cRect = OH_Drawing_RectCreate(c.fLeft, c.fTop, c.fRight, c.fBottom);
        OH_Drawing_CanvasClipRect(canvas, cRect, OH_Drawing_CanvasClipOp::INTERSECT, false);
        OH_Drawing_BrushSetColor(brush, kColor1);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDrawOval(canvas, rect);
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_RectDestroy(cRect);
    }
    OH_Drawing_RectDestroy(rect);

    OH_Drawing_Brush *xferBrush = OH_Drawing_BrushCreate();
    r = bounds;
    rect = OH_Drawing_RectCreate(r.fLeft, r.fTop, r.fRight, r.fBottom);
    OH_Drawing_BrushSetBlendMode(xferBrush, mode);
    OH_Drawing_CanvasSaveLayer(canvas, rect, xferBrush);

    r = bounds;
    r.inset(0, kInset);
    rect = OH_Drawing_RectCreate(r.fLeft, r.fTop, r.fRight, r.fBottom);
    OH_Drawing_BrushSetShaderEffect(brush, s2);
    OH_Drawing_BrushSetColor(brush, s2 ? 0xFF000000 : ColorSetA(kColor2, 0x80));
    OH_Drawing_Filter *filter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetColorFilter(filter, cFilter);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawOval(canvas, rect);
    if (!s2) {
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_Rect *cRect = OH_Drawing_RectCreate(c.fLeft, c.fTop, c.fRight, c.fBottom);
        OH_Drawing_CanvasClipRect(canvas, cRect, OH_Drawing_CanvasClipOp::INTERSECT, false);
        OH_Drawing_BrushSetColor(brush, kColor2);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDrawOval(canvas, rect);
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_RectDestroy(cRect);
    }

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_BrushDestroy(xferBrush);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_FilterDestroy(filter);

    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasRestore(canvas);
}

void LumaFilter::OnTestFunction(OH_Drawing_Canvas *canvas) {
    SkColor g1Colors[] = {kColor1, ColorSetA(kColor1, 0x20)};
    SkColor g2Colors[] = {kColor2, ColorSetA(kColor2, 0x20)};
    OH_Drawing_Point *g1Points[] = {OH_Drawing_PointCreate(0, 0), OH_Drawing_PointCreate(0, 100)};
    OH_Drawing_Point *g2Points[] = {OH_Drawing_PointCreate(0, 0), OH_Drawing_PointCreate(kSize, 0)};

    float pos[] = {0.2f, 1.0f};

    OH_Drawing_ColorFilter *fFilter = OH_Drawing_ColorFilterCreateLuma();
    OH_Drawing_ShaderEffect *fGr1 = OH_Drawing_ShaderEffectCreateLinearGradient(g1Points[0], g1Points[1], g1Colors, pos, 2, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_ShaderEffect *fGr2 = OH_Drawing_ShaderEffectCreateLinearGradient(g2Points[0], g2Points[1], g2Colors, pos, 2, OH_Drawing_TileMode::CLAMP);

    //////////////////////////////////////
    OH_Drawing_BlendMode modes[] = {
        OH_Drawing_BlendMode::BLEND_MODE_SRC_OVER,
        OH_Drawing_BlendMode::BLEND_MODE_DST_OVER,
        OH_Drawing_BlendMode::BLEND_MODE_SRC_ATOP,
        OH_Drawing_BlendMode::BLEND_MODE_DST_ATOP,
        OH_Drawing_BlendMode::BLEND_MODE_SRC_IN,
        OH_Drawing_BlendMode::BLEND_MODE_DST_IN,
    };

    struct {
        OH_Drawing_ShaderEffect *fShader1;
        OH_Drawing_ShaderEffect *fShader2;
    } shaders[] = {
        {nullptr, nullptr},
        {nullptr, fGr2},
        {fGr1, nullptr},
        {fGr1, fGr2},
    };
    float gridStep = kSize + 2 * kInset;
    DRAWING_LOGI("OnTestFunction path = %{public}s", __FILE_NAME__);

    size_t modes_size = 6;
    for (size_t i = 0; i < modes_size; ++i) {
        OH_Drawing_Point2D offset = {gridStep * (0.5f + i), 20};
        draw_label(canvas, gModeStrings[modes[i]], offset);
    }

    size_t shaders_size = 4;
    for (size_t i = 0; i < shaders_size; ++i) {
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasTranslate(canvas, kInset, gridStep * i + 30);
        for (size_t m = 0; m < modes_size; ++m) {
            draw_scene(canvas, fFilter, modes[m], shaders[i].fShader1, shaders[i].fShader2);
            OH_Drawing_CanvasTranslate(canvas, gridStep, 0);
        }
        OH_Drawing_CanvasRestore(canvas);
    }

    OH_Drawing_ColorFilterDestroy(fFilter);
    OH_Drawing_ShaderEffectDestroy(fGr1);
    OH_Drawing_ShaderEffectDestroy(fGr2);
}
