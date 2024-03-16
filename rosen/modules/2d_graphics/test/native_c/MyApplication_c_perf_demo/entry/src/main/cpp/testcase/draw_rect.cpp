#include "draw_rect.h"
#include "test_common.h"

void DrawRect::onDraw(OH_Drawing_Canvas* canvas, uint32_t index) {
    // rects_stroke_0_aa
    OH_Drawing_CanvasDrawRect(canvas, rects[index]);
}
