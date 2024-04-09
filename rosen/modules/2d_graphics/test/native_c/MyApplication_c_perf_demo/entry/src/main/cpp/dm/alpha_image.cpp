#include "alpha_image.h"
#include <multimedia/image_framework/image_mdk_common.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_sampling_options.h>
#include "test_common.h"
#include "common/log_common.h"

enum{
    kW=152,
    kH=80,
};

AlphaImage::AlphaImage() {
    //skia dm file gm/fontregen.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "alpha_image_alpha_tint";
}


void AlphaImage::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFF888888);//gray
    OH_Drawing_Bitmap* bm= OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = {OH_Drawing_ColorFormat::COLOR_FORMAT_BGRA_8888,OH_Drawing_AlphaFormat::ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(bm, 64, 64, &format);
    uint32_t w =OH_Drawing_BitmapGetWidth(bm);
    uint32_t h = OH_Drawing_BitmapGetHeight(bm);
    void* pixel = OH_Drawing_BitmapGetPixels(bm);
    for(int y = 0;y<h;++y){
        for(int x = 0; x<w;++x){
            
//            void* ptr = (static_cast<type>())pixel + (y*w +x)*sizeof(void*);
//            *ptr = y*4;

        }
    }
    
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bm);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, 0x8000FF00);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasTranslate(canvas, 8, 8);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, kW, kH);
    OH_Drawing_SamplingOptions* option = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST,OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);
    OH_Drawing_CanvasDrawImageRect(canvas, image, rect, option);
    
//    OH_Drawing_CanvasDrawBitmap(canvas, bm, 0, 0);
    OH_Drawing_CanvasTranslate(canvas, 72, 0);
    OH_Drawing_Rect* rc = OH_Drawing_RectCreate(0, 0, 64, 64);
    OH_Drawing_Point* start = OH_Drawing_PointCreate(0, 0);
    OH_Drawing_Point* end = OH_Drawing_PointCreate(0, 64);
    
    static const uint32_t gColors[] = { 0x0000FF00, 0xFF00FF00 };
    OH_Drawing_ShaderEffect * effect = OH_Drawing_ShaderEffectCreateLinearGradient(start, end, gColors, nullptr, 2, OH_Drawing_TileMode::REPEAT);
    OH_Drawing_BrushSetShaderEffect(brush, effect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rc);
 
}
