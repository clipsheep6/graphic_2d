#include "draw_rect.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include "test_common.h"


DrawRect::DrawRect(int shift) {
    TestRend rand;
    float offset = 1.0 / 3;
    for (int i = 0; i < RAND_SIZE; i++) {
        int x = rand.nextU() % W;
        int y = rand.nextU() % H;
        int w = rand.nextU() % W;
        int h = rand.nextU() % H;
        w >>= shift;
        h >>= shift;
        x = x - w / 2 + offset;
        y = y - h / 2 + offset;
        rects[i] = OH_Drawing_RectCreate(x, y, x + w, y + h);
        colors[i] = rand.nextU() | 0xFF808080;
    }
}

DrawRect::~DrawRect() {
    for (int i = 0; i < RAND_SIZE; i++) {
        OH_Drawing_RectDestroy(rects[i]);
    }
}

void DrawRect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    // 创建一个画刷Brush对象，Brush对象用于形状的填充
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 1, 0.1, 0, 0, 1, 0, 0.0001, 0.001, 1);
    OH_Drawing_CanvasConcatMatrix(canvas, matrix);

    OH_Drawing_BrushSetColor(brush, colors[0]);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rects[0]);
    
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
}

void DrawRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // rects_stroke_0_aa
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 1, 0.1, 0, 0, 1, 0, 0.0001, 0.001, 1);
    OH_Drawing_CanvasConcatMatrix(canvas, matrix);

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_BrushSetColor(brush, colors[i % RAND_SIZE]);
        OH_Drawing_BrushSetAntiAlias(brush, true);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDrawRect(canvas, rects[i % RAND_SIZE]);
        OH_Drawing_CanvasDetachBrush(canvas);
    }
    
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
}
