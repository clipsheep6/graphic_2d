#include "alpha_image.h"
#include <multimedia/image_framework/image_mdk_common.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color_filter.h>
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

OH_Drawing_ColorFilter* make_color_filter() {
    float colorMatrix[20] = {
        1, 0, 0,   0,   0,
        0, 1, 0,   0,   0,
        0, 0, 0.5, 0.5, 0,
        0, 0, 0.5, 0.5, 0}; // mix G and A.
    return OH_Drawing_ColorFilterCreateMatrix(colorMatrix);
}

AlphaImage::AlphaImage() {
    //skia dm file gm/fontregen.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "alpha_image_alpha_tint";
}

void AlphaImage::OnTestFunction(OH_Drawing_Canvas* canvas)
{
//    DRAWING_LOGI("OnTestFunction path = %{public}s",__FILE_NAME__);
    uint32_t w = 64;
    uint32_t h = 64;
    OH_Drawing_CanvasClear(canvas, DRAW_ColorGRAY);
    OH_Drawing_Bitmap* bm= OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = {OH_Drawing_ColorFormat::COLOR_FORMAT_ALPHA_8,OH_Drawing_AlphaFormat::ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(bm, w, h, &format);
    for(int y = 0;y<h;++y){
        for(int x = 0; x<w;++x){
            *DrawBitmapGetAddr8(bm,x,y) = y*4;
        }
    }
    
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bm);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    
    OH_Drawing_BrushSetColor(brush, 0x8000FF00);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasTranslate(canvas, 8, 8);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    OH_Drawing_SamplingOptions* option = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST,OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);
    OH_Drawing_CanvasDrawImageRect(canvas, image, rect, option);
    
    OH_Drawing_CanvasTranslate(canvas, 72, 0);
    OH_Drawing_ShaderEffect * effect = OH_Drawing_ShaderEffectCreateImageShader(image,OH_Drawing_TileMode::CLAMP,OH_Drawing_TileMode::CLAMP,option,nullptr);
    OH_Drawing_BrushSetShaderEffect(brush, effect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_SamplingOptionsDestroy(option);
    OH_Drawing_ShaderEffectDestroy(effect);
}
