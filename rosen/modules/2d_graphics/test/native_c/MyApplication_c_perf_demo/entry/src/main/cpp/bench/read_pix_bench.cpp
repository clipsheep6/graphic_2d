#include "read_pix_bench.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <time.h>
#include "common/log_common.h"
#include "test_common.h"

void ReadPixBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0x80FF0000);
    int32_t w = OH_Drawing_CanvasGetWidth(canvas);
    int32_t h = OH_Drawing_CanvasGetHeight(canvas);
    OH_Drawing_Image_Info imageInfo {w,h,COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL} ;
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    
    if(bToBitmap_ == false){
    OH_Drawing_CanvasReadPixels(canvas,&imageInfo,OH_Drawing_BitmapGetPixels(bitmap),w,0,0);
    }
    else{
        OH_Drawing_CanvasReadPixelsToBitmap(canvas, bitmap, 100, 100);
    }
    
    OH_Drawing_BitmapDestroy(bitmap);
}

void ReadPixBench::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    //当前用例名 drawcanvasreadpixels 测试 OH_Drawing_CanvasReadPixels  迁移基于skia ReadPixBench.cpp->ReadPixBench
    // skia case name : readpix_pm_rgba_null

    OH_Drawing_CanvasClear(canvas, 0x80FF0000);
    int32_t w = OH_Drawing_CanvasGetWidth(canvas);
    int32_t h = OH_Drawing_CanvasGetHeight(canvas);
    OH_Drawing_Image_Info imageInfo {w,h,COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL} ;
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL};
    OH_Drawing_BitmapBuild(bitmap,w,h,&cFormat);
//    DRAWING_LOGI("DrawCanvasReadPixels %{public}d,%{public}d",w,h);
    
    if(bToBitmap_ == false){
        for (int i = 0; i < testCount_; i++) {
            OH_Drawing_CanvasReadPixels(canvas,&imageInfo,OH_Drawing_BitmapGetPixels(bitmap),w,0,0);
        }
    }
    else{
        int32_t toBitmapW = 100;
        int32_t toBitmapH = 100;
         for (int i = 0; i < testCount_; i++) {
            OH_Drawing_CanvasReadPixelsToBitmap(canvas, bitmap, toBitmapW, toBitmapH);
         }       
    }

    OH_Drawing_BitmapDestroy(bitmap);
}
