#include "rect_bench.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_round_rect.h>
#include "test_common.h"
#include "common/log_common.h"

RectBench::RectBench(int shift,bool bBursh, int stroke ,bool aa ,bool perspective) {
    TestRend rand;
    float offset = 1.0 / 3;
    for (int i = 0; i < RAND_SIZE; i++) {
        uint32_t x = rand.nextU() % W;
        uint32_t y = rand.nextU() % H;
        uint32_t w = rand.nextU() % W;
        uint32_t h = rand.nextU() % H;
        w >>= shift;
        h >>= shift;
        x = x - w / 2 + offset;
        y = y - h / 2 + offset;
        rects[i] = OH_Drawing_RectCreate(x, y, x + w, y + h);
        widths[i] = w;
        heights[i] = h;
        colors[i] = rand.nextU() | 0xFF808080;
    }
    fStroke = stroke;
    fAA = aa;
    fPerspective = perspective;
    fBrush = bBursh;
}

//it will cause crash when drawing
//RectBench::~RectBench() {
//    for (int i = 0; i < RAND_SIZE; i++) {
//        OH_Drawing_RectDestroy(rects[i]);
//    }
//}

void RectBench::onCleanUp()
{
    for (int i = 0; i < RAND_SIZE; i++) {
        OH_Drawing_RectDestroy(rects[i]);
        rects[i] = nullptr;
    }
}

void RectBench::onDraw(OH_Drawing_Canvas* canvas, uint32_t index) {
    // rects_stroke_0_aa
//    DRAWING_LOGI("RectBench::onDraw %{public}d", index);
    OH_Drawing_CanvasDrawRect(canvas, rects[index]);
}

void RectBench::OnTestFunction(OH_Drawing_Canvas *canvas) {
    DRAWING_LOGE("RectBench::OnTestFunction");
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    
    if (fPerspective) {
        OH_Drawing_MatrixSetMatrix(matrix, 1, 0.1, 0, 0, 1, 0, 0.0001, 0.001, 1);
        OH_Drawing_CanvasConcatMatrix(canvas, matrix);
    }
    
    if (fBrush) {
        OH_Drawing_BrushSetColor(brush, colors[0]);
        OH_Drawing_BrushSetAntiAlias(brush, fAA);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    } else {
        if(fStroke > 0)
        {
            OH_Drawing_PenSetWidth(pen, fStroke);
        }
        OH_Drawing_PenSetColor(pen, colors[0]);
        OH_Drawing_PenSetAntiAlias(pen, fAA);
        setupPaint(canvas,pen);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        
    }
    onDraw(canvas, 0);
    if (fBrush) {
        OH_Drawing_CanvasDetachBrush(canvas);
    } else {
        OH_Drawing_CanvasDetachPen(canvas);
    }
    
    onCleanUp();
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
}

void RectBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // rects_stroke_0_aa
    DRAWING_LOGE("RectBench::OnTestPerformance");
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    if(fBrush){
    }
    else{
        if(fStroke > 0){
            OH_Drawing_PenSetWidth(pen, fStroke);
        }
    }

    if (fPerspective){
        OH_Drawing_MatrixSetMatrix(matrix, 1, 0.1, 0, 0, 1, 0, 0.0001, 0.001, 1);
        OH_Drawing_CanvasConcatMatrix(canvas, matrix);
    }

    for (int i = 0; i < testCount_; i++) {
        if (fBrush) {
            OH_Drawing_BrushSetColor(brush, colors[i % RAND_SIZE]);
            OH_Drawing_BrushSetAntiAlias(brush, fAA);
            OH_Drawing_CanvasAttachBrush(canvas, brush);
        } else {
            OH_Drawing_PenSetColor(pen, colors[i % RAND_SIZE]);
            OH_Drawing_PenSetAntiAlias(pen, fAA);
            setupPaint(canvas,pen);
            OH_Drawing_CanvasAttachPen(canvas, pen);
        }
        onDraw(canvas, i % RAND_SIZE);
        if (fBrush) {
            OH_Drawing_CanvasDetachBrush(canvas);
        } else {
            OH_Drawing_CanvasDetachPen(canvas);
        }        
    }
    
    onCleanUp();
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
}

void RRectBench::onDraw(OH_Drawing_Canvas* canvas, uint32_t index) {
    // rrects_stroke_0_aa
    OH_Drawing_RoundRect *rrect = OH_Drawing_RoundRectCreate(rects[index], widths[index]/4, heights[index]/4);
    OH_Drawing_CanvasDrawRoundRect(canvas, rrect);
    OH_Drawing_RoundRectDestroy(rrect);
}

void OvalBench::onDraw(OH_Drawing_Canvas* canvas, uint32_t index) {
    // ovals_stroke_0_aa
    OH_Drawing_CanvasDrawOval(canvas, rects[index]);
}

void SrcModeRectBench::setupPaint(OH_Drawing_Canvas* canvas,OH_Drawing_Pen* pen) {
    //当前用例名 drawpenrectsrcmode 测试 OH_Drawing_PenSetAlpha  迁移基于skia RectBench.cpp->SrcModeRectBench
    // skia case name : srcmode_rects_1_stroke_0_aa
    OH_Drawing_PenSetAlpha(pen, 0x80);
    OH_Drawing_PenSetBlendMode(pen, OH_Drawing_BlendMode::BLEND_MODE_SRC);
}

void BlitMaskBench::OnTestFunction(OH_Drawing_Canvas *canvas) {
    float gSizes[] = {float(13), float(24)};
    int sizes = sizeof(gSizes) / sizeof(gSizes[0]);

    if (FLAGS_strokeWidth >= 0) {
        gSizes[0] = (float)FLAGS_strokeWidth;
        sizes = 1;
    }

    TestRend rand = TestRend();
    uint32_t color = 0xFF000000;
    unsigned int alpha = 0xFF;

    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);

    if (_type == KMaskShader) {
        // 位图
        OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
        OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
        OH_Drawing_BitmapBuild(bitmap, 10, 1, &cFormat);
        OH_Drawing_CanvasBind(canvas, bitmap);

        // 着色器
        OH_Drawing_Point *startPt = OH_Drawing_PointCreate(0, 0);
        OH_Drawing_Point *endPt = OH_Drawing_PointCreate(10, 10);
        const uint32_t Color[] = {
            0xFF000000,
            0xFF00FF00,
        };
        OH_Drawing_ShaderEffect *shaderEffect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, Color, nullptr, 2, OH_Drawing_TileMode::CLAMP);
        OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);

        OH_Drawing_BitmapDestroy(bitmap);
        OH_Drawing_ShaderEffectDestroy(shaderEffect);
        bitmap = nullptr;
        startPt = nullptr;
        endPt = nullptr;
        shaderEffect = nullptr;
    }
    switch (_type) {
    case kMaskOpaque:
        color = colors[0];
        alpha = 0xFF;
        break;
    case kMaskBlack:
        alpha = 0xFF;
        color = 0xFF000000;
        break;
    case kMaskColor:
        color = colors[0];
        alpha = rand.nextU() & 255;
        break;
    case KMaskShader:
        break;
    }

    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetColor(brush, color);
    OH_Drawing_BrushSetAlpha(brush, alpha);
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    OH_Drawing_CanvasDrawRect(canvas, rects[0]);

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
}

void BlitMaskBench::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // maskcolor

    float gSizes[] = {float(13), float(24)};
    int sizes = sizeof(gSizes) / sizeof(gSizes[0]);

    if (FLAGS_strokeWidth >= 0) {
        gSizes[0] = (float)FLAGS_strokeWidth;
        sizes = 1;
    }

    TestRend rand = TestRend();
    uint32_t color = 0xFF000000;
    unsigned int alpha = 0xFF;

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
        OH_Drawing_BrushSetAntiAlias(brush, true);

        if (_type == KMaskShader) {
            // 位图
            OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
            OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
            OH_Drawing_BitmapBuild(bitmap, 10, 1, &cFormat);
            OH_Drawing_CanvasBind(canvas, bitmap);

            // 着色器
            OH_Drawing_Point *startPt = OH_Drawing_PointCreate(0, 0);
            OH_Drawing_Point *endPt = OH_Drawing_PointCreate(10, 10);
            const uint32_t Color[] = {
                0xFF000000,
                0xFF00FF00,
            };
            OH_Drawing_ShaderEffect *shaderEffect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, Color, nullptr, 2, OH_Drawing_TileMode::CLAMP);
            OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);

            OH_Drawing_BitmapDestroy(bitmap);
            OH_Drawing_ShaderEffectDestroy(shaderEffect);
            bitmap = nullptr;
            startPt = nullptr;
            endPt = nullptr;
            shaderEffect = nullptr;
        }
        switch (_type) {
        case kMaskOpaque:
            color = colors[i % RAND_SIZE];
            alpha = 0xFF;
            break;
        case kMaskBlack:
            alpha = 0xFF;
            color = 0xFF000000;
            break;
        case kMaskColor:
            color = colors[i % RAND_SIZE];
            alpha = rand.nextU() & 255;
            break;
        case KMaskShader:
            break;
        }

        OH_Drawing_BrushSetAntiAlias(brush, true);
        OH_Drawing_BrushSetColor(brush, color);
        OH_Drawing_BrushSetAlpha(brush, alpha);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDrawRect(canvas, rects[i % RAND_SIZE]);

        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_BrushDestroy(brush);
        brush = nullptr;
    }
}
