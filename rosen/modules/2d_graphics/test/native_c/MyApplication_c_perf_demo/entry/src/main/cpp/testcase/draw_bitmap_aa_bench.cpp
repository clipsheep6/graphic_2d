#include "draw_bitmap_aa_bench.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_matrix.h>
//#include <native_drawing/drawing_sampling_options.h>
#include "common/log_common.h"


void DrawBitmapAABench::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(bitmap, 200, 200, &cFormat);
    OH_Drawing_Canvas* bitmapCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitmapCanvas, bitmap);
    OH_Drawing_CanvasClear(bitmapCanvas,
                           OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0x00));

    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreateTranslation(17.5, 17.5);
    OH_Drawing_CanvasConcatMatrix(canvas, matrix);
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap, 0, 0);

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_CanvasDestroy(bitmapCanvas);
    OH_Drawing_BitmapDestroy(bitmap);
}

void DrawBitmapAABench::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // draw_bitmap_noaa_translate
    OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(bitmap, 200, 200, &cFormat);
    OH_Drawing_Canvas *bitmapCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitmapCanvas, bitmap);
    OH_Drawing_CanvasClear(bitmapCanvas, OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0x00));
    
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreateTranslation(17.5, 17.5);
    OH_Drawing_CanvasConcatMatrix(canvas, matrix);
    
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawBitmap(canvas, bitmap, 0, 0);
    }
    
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_CanvasDestroy(bitmapCanvas);
    OH_Drawing_BitmapDestroy(bitmap);
}
