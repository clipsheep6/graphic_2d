#include "draw_oval.h"
#include "test_common.h"

void DrawOval::onDraw(OH_Drawing_Canvas* canvas, uint32_t index) {
    // ovals_stroke_0_aa
    OH_Drawing_CanvasDrawOval(canvas, rects[index]);
}
