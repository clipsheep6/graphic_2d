//
// Created on 2024/7/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "canvas_draw_image_rect_with_src.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_mask_filter.h>
#include "test_common.h"

// OH_Drawing_Image *precondItions():创建了一个位图（bitmap），并在其上绘制了一个圆形，然后生成了一个图像（image）对象
OH_Drawing_Image *precondItions()
{
    // 初始化位图对象
    uint32_t w = 64;
    uint32_t h = 64;
    OH_Drawing_Bitmap *bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = { OH_Drawing_ColorFormat::COLOR_FORMAT_RGBA_8888,
                                       OH_Drawing_AlphaFormat::ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapBuild(bm, w, h, &format);
    OH_Drawing_Canvas *bitcanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitcanvas, bm);
    OH_Drawing_CanvasClear(bitcanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    // 创建一个画刷brush对象设置抗锯齿属性
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    // 着色器
    OH_Drawing_Point *startPt = OH_Drawing_PointCreate(0.f, 0.f);
    OH_Drawing_Point *endPt = OH_Drawing_PointCreate(64.f, 64.f);
    const uint32_t Color[] = {
        0xFFFFFFFF,
        0xFF0000FF,
    };
    OH_Drawing_ShaderEffect *shaderEffect =
        OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, Color, nullptr, 2, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    OH_Drawing_CanvasAttachBrush(bitcanvas, brush);
    // 画圆形
    OH_Drawing_Point *center = OH_Drawing_PointCreate(32.f, 32.f);
    float radius = 32;
    OH_Drawing_CanvasDrawCircle(bitcanvas, center, radius);
    OH_Drawing_Image *image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bm);
    // 释放内存
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_CanvasDetachBrush(bitcanvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_CanvasDestroy(bitcanvas);
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
    OH_Drawing_PointDestroy(center);
    return image;
}

void OhCanvasDrawImageRectWithSrc::OnTestPerformance(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFCCCCCC);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDetachBrush(canvas);
    // 创建采样选项对象
    OH_Drawing_SamplingOptions *option = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST,
        OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);
    // 调用PrecondItions()内返回的（image）对象
    auto image = precondItions();
    //创建四个截屏的区域
    OH_Drawing_Rect *arrayRect[4];
    arrayRect[0] = OH_Drawing_RectCreate(0, 0, 32, 32);     // 0, 0, 32, 32指针数组
    arrayRect[1] = OH_Drawing_RectCreate(0, 0, 80, 80);     // 0, 0, 80, 80指针数组
    arrayRect[2] = OH_Drawing_RectCreate(32, 32, 96, 96);   // 32, 32, 96, 96指针数组
    arrayRect[3] = OH_Drawing_RectCreate(-32, -32, 32, 32); // -32, -32, 32, 32指针数组
    const size_t ARRAY_SIZE = sizeof(arrayRect) / sizeof(arrayRect[0]);
    // 开始创建矩形对象并平移
    OH_Drawing_Rect *dstR = OH_Drawing_RectCreate(0, 200, 128, 380); // 200, 128, 380坐标
    //OH_Drawing_CanvasTranslate(canvas, 1, 40);                      // 16, 40平移距离
    OH_Drawing_Rect *dst = OH_Drawing_RectCreate(0, 0, 64, 64);      // 64正方形
    for (size_t i = 0; i < testCount_; ++i) {
        if((i+1)%100 == 0){
            OH_Drawing_CanvasTranslate(canvas, -500, 30);
        }        
        OH_Drawing_Rect *srcR = arrayRect[i%4];
        OH_Drawing_CanvasDrawImageRect(canvas, image, dst, option);
        OH_Drawing_CanvasDrawImageRectWithSrc(canvas, image, srcR, dstR, option,
            OH_Drawing_SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        // 画矩形
        OH_Drawing_CanvasDrawRect(canvas, dstR);
        OH_Drawing_CanvasDrawRect(canvas, srcR);
        OH_Drawing_CanvasTranslate(canvas, 5, 0); // 160, 0平移距离
    }    
    // 释放内存
    OH_Drawing_RectDestroy(arrayRect[0]);
    OH_Drawing_RectDestroy(arrayRect[1]);
    OH_Drawing_RectDestroy(arrayRect[2]);
    OH_Drawing_RectDestroy(arrayRect[3]);
    OH_Drawing_SamplingOptionsDestroy(option);
    OH_Drawing_RectDestroy(dst);
    OH_Drawing_RectDestroy(dstR);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_PenDestroy(pen);
}