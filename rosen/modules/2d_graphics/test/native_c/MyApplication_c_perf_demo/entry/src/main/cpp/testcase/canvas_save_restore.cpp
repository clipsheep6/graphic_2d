#include "canvas_save_restore.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_rect.h>
//#include <native_drawing/drawing_sampling_options.h>
#include "common/log_common.h"

void CanvasSaveRestore::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreateTranslation(17.5, 17.5);
    OH_Drawing_CanvasConcatMatrix(canvas, matrix);
    OH_Drawing_CanvasClear(canvas, OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0xff));
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_MatrixDestroy(matrix);
}

void CanvasSaveRestore::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // canvas_save_restore_8
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreateTranslation(17.5, 17.5);
    for (int i = 0; i < testCount_; i++) {
        for (int j = 0; j < depth_; j++) {
            OH_Drawing_CanvasSave(canvas);
            OH_Drawing_CanvasConcatMatrix(canvas, matrix);
        }
        OH_Drawing_CanvasClear(canvas, OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0xff));
        for (int k = 0; k < depth_; k++) {
            OH_Drawing_CanvasRestore(canvas);
        }
    }
    OH_Drawing_MatrixDestroy(matrix);
}
