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
    
    for (int i = 0; i < testCount_; i++) {
        int x = i % bitmapWidth_;
        int y = (i + 100) % bitmapHeight_;
        int w = (100 > (bitmapWidth_ - x)) ? (bitmapWidth_ - x) : 100;
        int h = (100 > (bitmapHeight_ - y)) ? (bitmapHeight_ - y) : 100;
        //针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(x, y, w, h);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    }
}
