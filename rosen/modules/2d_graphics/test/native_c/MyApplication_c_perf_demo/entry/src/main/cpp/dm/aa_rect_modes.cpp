#include "aa_rect_modes.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include "test_common.h"
#include "common/log_common.h"

AARectModes::AARectModes() {
    bitmapWidth_ = 640;
    bitmapHeight_ = 480;
    fileName_ = "aarectmodes";
}

AARectModes::~AARectModes() {

}

void AARectModes::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    //bala bala bala
    //TODO
    //SUCH AS
    OH_Drawing_CanvasClear(canvas, 0xFF00ff00);
}
