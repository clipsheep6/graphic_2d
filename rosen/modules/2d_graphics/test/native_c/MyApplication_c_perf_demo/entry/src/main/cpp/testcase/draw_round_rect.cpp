#include "draw_round_rect.h"
#include "test_common.h"
#include <native_drawing/drawing_round_rect.h>

void DrawRountRect::onDraw(OH_Drawing_Canvas* canvas, uint32_t index) {
    // rrects_stroke_0_aa
    OH_Drawing_RoundRect *rrect = OH_Drawing_RoundRectCreate(rects[index], widths[index]/4, heights[index]/4);
    OH_Drawing_CanvasDrawRoundRect(canvas, rrect);
    OH_Drawing_RoundRectDestroy(rrect);
}
