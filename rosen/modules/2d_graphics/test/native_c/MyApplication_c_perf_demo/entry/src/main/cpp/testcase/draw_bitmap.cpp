#include "draw_bitmap.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_matrix.h>
#include "common/log_common.h"


void DrawBitmap::OnTestFunctionCpu(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    // 定义bitmap的像素格式
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
    // 构造对应格式的bitmap
    OH_Drawing_BitmapBuild(bitmap, 200, 200, &cFormat);
    OH_Drawing_Canvas* bitmapCanvas = OH_Drawing_CanvasCreate();
    // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
    OH_Drawing_CanvasBind(bitmapCanvas, bitmap);
    // 0xFF00FF00
    OH_Drawing_CanvasClear(bitmapCanvas,
                           OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0x00));

    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreateTranslation(17.5, 17.5);
    OH_Drawing_CanvasConcatMatrix(canvas, matrix);
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap, 0, 0);
    }

void DrawBitmap::OnTestPerformanceCpu(OH_Drawing_Canvas *canvas) {

    OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
    // 定义bitmap的像素格式
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
    // 构造对应格式的bitmap
    OH_Drawing_BitmapBuild(bitmap, 200, 200, &cFormat);
    OH_Drawing_Canvas *bitmapCanvas = OH_Drawing_CanvasCreate();
    // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
    OH_Drawing_CanvasBind(bitmapCanvas, bitmap);
    // 0xFF00FF00
    OH_Drawing_CanvasClear(bitmapCanvas, OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0x00));
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreateTranslation(17.5, 17.5);
    OH_Drawing_CanvasConcatMatrix(canvas, matrix);
    // 将文本绘制到画布上
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawBitmap(canvas, bitmap, 0, 0);
    }
}
