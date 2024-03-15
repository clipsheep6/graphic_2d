#include "draw_bitmaprect.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include "common/log_common.h"


void DrawBitmapRect::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    // 构造对应格式的bitmap
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(bitmap, 256, 256, &cFormat);
    OH_Drawing_Canvas* bitmapCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitmapCanvas, bitmap);
    OH_Drawing_CanvasClear(bitmapCanvas, OH_Drawing_ColorSetArgb(0xff, 0x12, 0x34, 0xAA));

    // 设置aa
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    
    //DRAW    
    OH_Drawing_Rect *srcRect = OH_Drawing_RectCreate(0, 0, 256, 256);
    OH_Drawing_Rect *dstRect = OH_Drawing_RectCreate(100, 100, 256, 256);
    OH_Drawing_Rect *dstRect1 = OH_Drawing_RectCreate(400, 400, 656, 656);
    OH_Drawing_SamplingOptions * sampling = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_LINEAR, MIPMAP_MODE_LINEAR);
    OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, srcRect, dstRect, sampling);
    OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, srcRect, dstRect1, sampling);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(srcRect);
    OH_Drawing_RectDestroy(dstRect);
    OH_Drawing_RectDestroy(dstRect1);
    OH_Drawing_SamplingOptionsDestroy(sampling);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_CanvasDestroy(bitmapCanvas);
    OH_Drawing_BitmapDestroy(bitmap);
}

void DrawBitmapRect::OnTestPerformance(OH_Drawing_Canvas *canvas)
{
    // bulkrect_grid_sharedimage_ref
    // 构造对应格式的bitmap
    OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(bitmap, 256, 256, &cFormat);
    OH_Drawing_Canvas *bitmapCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitmapCanvas, bitmap);
    OH_Drawing_CanvasClear(bitmapCanvas, OH_Drawing_ColorSetArgb(0xff, 0x12, 0x34, 0xAA));

    // 设置aa
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    int num = testCount_ > 10000? 10000: testCount_;
    int sqr = sqrt(num - 1);
    DRAWING_LOGE("xyj DrawBitmapRect size: [%{public}u]", sqr);
    OH_Drawing_Rect *rectList[num];
    int dstWidth = (bitmapWidth_ - 1) / sqr;
    int dstHeight = (bitmapHeight_ - 1) / sqr;
    for (int j = 0; j < num; j++) {
        float x = (j % sqr) * dstWidth + 0.5; // Offset to ensure AA doesn't get disabled
        float y = (j / sqr) * dstWidth + 0.5;
        rectList[j] = OH_Drawing_RectCreate(x, y, x+dstWidth, y+dstHeight);
    }

    // DRAW
    OH_Drawing_Rect *srcRect = OH_Drawing_RectCreate(0, 0, 256, 256);
    OH_Drawing_SamplingOptions *sampling = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_LINEAR, MIPMAP_MODE_LINEAR);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, srcRect, rectList[i % num], sampling);
    }

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    
    for (int j = 0; j < num; j++) {
        OH_Drawing_RectDestroy(rectList[j]);
    }
    OH_Drawing_RectDestroy(srcRect);
    OH_Drawing_SamplingOptionsDestroy(sampling);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_CanvasDestroy(bitmapCanvas);
    OH_Drawing_BitmapDestroy(bitmap);
}