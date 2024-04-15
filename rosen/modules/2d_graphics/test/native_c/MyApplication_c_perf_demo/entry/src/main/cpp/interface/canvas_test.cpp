#include "canvas_test.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_sampling_options.h>

#include "test_common.h"
#include "common/log_common.h"

void CanvasDrawRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    StyleSettings(canvas, styleType_);
    
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, rectWidth_, rectHeight_);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawRect(canvas, rect);
    }
    OH_Drawing_RectDestroy(rect);
}
