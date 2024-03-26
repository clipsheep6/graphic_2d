#include "rect_bench.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_round_rect.h>
#include "test_common.h"
#include "common/log_common.h"

RectBench::RectBench(int shift) {
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
}

RectBench::~RectBench() {
    for (int i = 0; i < RAND_SIZE; i++) {
        OH_Drawing_RectDestroy(rects[i]);
    }
}

void RectBench::onDraw(OH_Drawing_Canvas* canvas, uint32_t index) {
    // rects_stroke_0_aa
    OH_Drawing_CanvasDrawRect(canvas, rects[index]);
}

void RectBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGE("RectBench::OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 1, 0.1, 0, 0, 1, 0, 0.0001, 0.001, 1);
    OH_Drawing_CanvasConcatMatrix(canvas, matrix);

    OH_Drawing_BrushSetColor(brush, colors[0]);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    onDraw(canvas, 0);
    OH_Drawing_CanvasDetachBrush(canvas);

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_BrushDestroy(brush);
}

void RectBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // rects_stroke_0_aa
    DRAWING_LOGE("RectBench::OnTestPerformance");
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 1, 0.1, 0, 0, 1, 0, 0.0001, 0.001, 1);
    OH_Drawing_CanvasConcatMatrix(canvas, matrix);

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_BrushSetColor(brush, colors[i % RAND_SIZE]);
        OH_Drawing_BrushSetAntiAlias(brush, true);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        onDraw(canvas, i % RAND_SIZE);
        OH_Drawing_CanvasDetachBrush(canvas);
    }

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_BrushDestroy(brush);
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