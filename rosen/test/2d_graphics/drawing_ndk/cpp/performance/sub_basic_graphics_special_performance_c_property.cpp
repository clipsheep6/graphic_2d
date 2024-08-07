/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sub_basic_graphics_special_performance_c_property.h"

#include <cstdint>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>
#include <rawfile/raw_file.h>

#include "hilog/log.h"
#include "interface/sample_option_test.h"
#include "native_drawing/drawing_error_code.h"
#include "native_drawing/drawing_pixel_map.h"
#include "native_drawing/drawing_shadow_layer.h"
#include "performance.h"
#include "test_common.h"

#include "common/drawing_type.h"
#include "common/log_common.h"

void PerformanceCCanvasDrawVerticesOnTestPerformance(
    OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    DRAWING_LOGI("SubBasicGraphicsSpecialPerformanceCProperty OnTestPerformance OH_Drawing_CanvasDrawVertices");
    const int32_t itemWidth = 100;  // 100 绘图表面的宽度
    const int32_t itemHeight = 100; // 100 绘图表面的高度
    if (!canvas) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformanceCOpsWitch OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();

    SET_PEN_AND_BRUSH_EFFECT;

    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12); //for test
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    static int func = 0;
    std::string funcTitles[] = {
        "OH_Drawing_CanvasDrawVertices",
    };
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();

    int32_t imageWidth = 100 * 4;
    int32_t imageHeight = 100 * 4;
    OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888,
        ALPHA_FORMAT_PREMUL };                                                  // 100 用于宽高
    char* srcPixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4)); // 100 4 用于像素计算
    if (srcPixels == nullptr) {
        return;
    }
    for (int i = 0; i < imageHeight * imageHeight; i++) {
        ((uint32_t*)srcPixels)[i] = 0xFF5500FF;
    }
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, srcPixels, imageWidth * 4);
    ErrorCheck();
    if (!bitmap) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformance create bitmap error");
    }
    ErrorCheck();
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_ShaderEffect* shaderEffect1 = OH_Drawing_ShaderEffectCreateImageShader(
        image, OH_Drawing_TileMode::CLAMP, OH_Drawing_TileMode::MIRROR, samplingOptions, matrix);

    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect1);
    std::string str;
    str.append("brush");
    str.append(funcTitles[func]);
    OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
    OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 600); //for test
    OH_Drawing_TextBlobDestroy(textBlob);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    OH_Drawing_FontDestroy(titleFont);
    OH_Drawing_VertexMode mode = VERTEX_MODE_TRIANGLES;
    const int32_t vertexCount = 1000; //for test
    OH_Drawing_Point2D positions[vertexCount];

    OH_Drawing_Point2D textures[vertexCount];

    uint32_t colors[vertexCount];

    //
    uint16_t indexes[] = { 0, 1, 2, 3, 4 }; //for test
    // 10 testCount for test
    for (int i = 0; i < 10; i++) {
        for (int i = 0; i < vertexCount; i++) {
            positions[i] = { itemWidth + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 500)),
                itemHeight + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 500)) }; //for test
        }
        for (int i = 0; i < vertexCount; i++) {
            textures[i] = { itemWidth + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 500)),
                itemHeight + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 500)) }; //for test
        }
        for (int i = 0; i < vertexCount; i++) {
            colors[i] = 0xFF000000 + ((rand() % 256) << 4) + ((rand() % 256) << 2) + (rand() % 256); //for test
        }
        OH_Drawing_CanvasDrawVertices(
            canvas, mode, vertexCount, positions, textures, colors, 0, indexes, OH_Drawing_BlendMode::BLEND_MODE_SRC);
    }
    DESTROY_EFFECT;
    OH_Drawing_ShaderEffectDestroy(shaderEffect1);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_BitmapDestroy(bitmap);
    free(srcPixels);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PenDestroy(pen);

    OH_Drawing_BrushDestroy(brush);
}

void PerformanceCCanvasDrawVerticesBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCCanvasDrawVerticesOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BRUSH, testCount_);
}

void PerformanceCCanvasReadPixelsOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int testCount)
{
    DRAWING_LOGI("SubBasicGraphicsSpecialPerformanceCOpsWitch OnTestPerformance OH_Drawing_CanvasReadPixels");
    const int32_t itemWidth = 100;  // 100 绘图表面的宽度
    const int32_t itemHeight = 100; // 100 绘图表面的高度
    if (!canvas) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformanceCOpsWitch OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    SET_PEN_AND_BRUSH_EFFECT;
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    // draw bitmap

    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    int32_t imageWidth = 1024 * 4; //for test
    int32_t imageHeight = 1080 * 2; //for test
    OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888,
        ALPHA_FORMAT_PREMUL };                                                  // 100 用于宽高
    char* srcPixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4)); // 100 4 用于像素计算
    if (srcPixels == nullptr) {
        return;
    }
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        //for test
        if (i % 3 == 0) {
            ((uint32_t*)srcPixels)[i] = 0xFF5500FF;
        //for test    
        } else if (i % 3 == 1) {
            ((uint32_t*)srcPixels)[i] = 0xFF0055FF;
        } else {
            ((uint32_t*)srcPixels)[i] = 0xFFFF5500;
        }
    }
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, srcPixels, imageWidth * 4);
    ErrorCheck();
    if (!bitmap) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformance create bitmap error");
    }
    uint32_t bitmapX = 100; //for test
    uint32_t bitmapY = 100; //for test
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap, bitmapX, bitmapY);
    ErrorCheck();
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    ErrorCheck();
    OH_Drawing_Rect* imageRectSrc = OH_Drawing_RectCreate(0, 0, 100, 100); //for test

    OH_Drawing_Rect* imageRect =
        OH_Drawing_RectCreate(itemWidth * 3, itemHeight * 3, itemWidth * 3 + 100, itemHeight * 3 + 100); //for test

    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12); //for test
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    static int func = 0;
    std::string funcTitles[] = { "OH_Drawing_CanvasReadPixels" };
    if (drawingType == DRAWING_TYPE_ATTACH_BOTH) {
        std::string str;
        str.append("pen&&brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 600); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);

    } else if (drawingType == DRAWING_TYPE_ATTACH_PEN) {
        std::string str;
        str.append("pen");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 600); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DRAWING_TYPE_ATTACH_BRUSH) {
        std::string str;
        str.append("brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 600); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_FontDestroy(titleFont);

    bool res;

    char* dstPixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4)); // 100 4 用于像素计算
    if (dstPixels == nullptr) {
        return;
    }
    for (int i = 0; i < testCount; i++) {
        res = OH_Drawing_CanvasReadPixels(
            canvas, &imageInfo, dstPixels, imageWidth * 4, bitmapX, bitmapY); // 100, 4 用于像素计算
    }
    if (!res) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformanceCOpsWitch OH_Drawing_CanvasReadPixels failed");
    }
    OH_Drawing_Bitmap* bitmap1 = OH_Drawing_BitmapCreateFromPixels(&imageInfo, dstPixels, imageWidth * 4);
    ErrorCheck();
    //500,500 bitmap position (x,y)
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap1, 500, 500);
    ErrorCheck();
    OH_Drawing_BitmapDestroy(bitmap1);
    ErrorCheck();
    DESTROY_EFFECT;
    free(dstPixels);
    OH_Drawing_RectDestroy(imageRect);
    OH_Drawing_RectDestroy(imageRectSrc);
    free(srcPixels);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PenDestroy(pen);

    OH_Drawing_BrushDestroy(brush);
}

void PerformanceCCanvasReadPixels::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCCanvasReadPixelsOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BOTH, testCount_);
}

void PerformanceCCanvasReadPixelsPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCCanvasReadPixelsOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_PEN, testCount_);
}

void PerformanceCCanvasReadPixelsBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCCanvasReadPixelsOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BRUSH, testCount_);
}

void PerformanceCCanvasReadPixelsToBitmapOnTestPerformance(
    OH_Drawing_Canvas* canvas, DrawingType drawingType, int testCount)
{
    DRAWING_LOGI("SubBasicGraphicsSpecialPerformanceCProperty OnTestPerformance OH_Drawing_CanvasReadPixelsToBitmap");
    const int32_t itemWidth = 100;  // 256 绘图表面的宽度
    const int32_t itemHeight = 100; // 256 绘图表面的高度
    if (!canvas) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformanceCOpsWitch OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    SET_PEN_AND_BRUSH_EFFECT

    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    //
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    // 画bitmap
    ErrorCheck();
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    int32_t imageWidth = 1024 * 4; //for test
    int32_t imageHeight = 1080 * 2; //for test
    OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888,
        ALPHA_FORMAT_PREMUL };                                                  // 100 用于宽高
    char* srcPixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4)); // 100 4 用于像素计算
    if (srcPixels == nullptr) {
        return;
    }
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        //for test
        if (i < imageWidth * 100) {
            ((uint32_t*)srcPixels)[i] = 0xFF5500FF;
        //for test
        } else if (i < imageWidth * 300) {
            ((uint32_t*)srcPixels)[i] = 0xFF0055FF;
        } else {
            ((uint32_t*)srcPixels)[i] = 0xFFFF5500;
        }
    }
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, srcPixels, imageWidth * 4); //for test
    if (!bitmap) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformance create bitmap error");
    }

    void* data = OH_Drawing_BitmapGetPixels(bitmap);

    OH_Drawing_ImageBuildFromBitmap(image, bitmap);

    OH_Drawing_Rect* imageRect =
        OH_Drawing_RectCreate(itemWidth * 3, itemHeight * 3, itemWidth * 3 + imageWidth, itemHeight * 3 + imageHeight); //for test

    ErrorCheck();
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12); //for test
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    static int func = 0;
    std::string funcTitles[] = {
        "OH_Drawing_CanvasReadPixelsToBitmap",
    };
    if (drawingType == DRAWING_TYPE_ATTACH_BOTH) {
        std::string str;
        str.append("pen &&brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 100); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DRAWING_TYPE_ATTACH_PEN) {
        std::string str;
        str.append("pen");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 100); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DRAWING_TYPE_ATTACH_BRUSH) {
        std::string str;
        str.append("brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 100); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_FontDestroy(titleFont);

    bool res;

    OH_Drawing_CanvasDrawImageRect(canvas, image, imageRect, samplingOptions);
    OH_Drawing_Bitmap* bitmap1 = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapBuild(bitmap1, imageWidth, imageHeight, &bitmapFormat);
    ErrorCheck();

    for (int i = 0; i < testCount; i++) {
        res = OH_Drawing_CanvasReadPixelsToBitmap(
            canvas, bitmap1, OH_Drawing_RectGetLeft(imageRect), OH_Drawing_RectGetTop(imageRect));
    }
    if (!res) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformanceCOpsWitch OH_Drawing_CanvasReadPixelsToBitmap faild");
    }
    DESTROY_EFFECT;
    OH_Drawing_BitmapDestroy(bitmap1);
    ErrorCheck();
    free(srcPixels);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_RectDestroy(imageRect);

    OH_Drawing_SamplingOptionsDestroy(samplingOptions);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PenDestroy(pen);

    OH_Drawing_BrushDestroy(brush);
}

void PerformanceCCanvasReadPixelsToBitmap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCCanvasReadPixelsToBitmapOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BOTH, testCount_);
}

void PerformanceCCanvasReadPixelsToBitmapPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCCanvasReadPixelsToBitmapOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_PEN, testCount_);
}

void PerformanceCCanvasReadPixelsToBitmapBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCCanvasReadPixelsToBitmapOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BRUSH, testCount_);
}

void PerformanceCLargeImageLoadOnTestPerformance(
    OH_Drawing_Canvas* canvas, DrawingType drawingType, int testCount, int func)
{
    DRAWING_LOGI("PerformanceCLargeImageLoad OnTestPerformance");
    const int32_t itemWidth = 100;  // 256 绘图表面的宽度
    const int32_t itemHeight = 100; // 256 绘图表面的高度
    if (!canvas) {
        DRAWING_LOGE("PerformanceCLargeImageLoad OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    SET_PEN_AND_BRUSH_EFFECT

    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    //
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    // 画bitmap
    int32_t imageWidth = 1024 * 4; //for test
    int32_t imageHeight = 1080 * 2; //for test
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888,
        ALPHA_FORMAT_OPAQUE };                                                  // 100 用于宽高
    char* srcPixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4)); // 100 4 用于像素计算
    if (srcPixels == nullptr) {
        return;
    }
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        //for test
        if (i < imageWidth * 100) {
            ((uint32_t*)srcPixels)[i] = 0xFF5500FF;
        //for test
        } else if (i < imageWidth * 300) {
            ((uint32_t*)srcPixels)[i] = 0xFF0055FF;
        } else {
            ((uint32_t*)srcPixels)[i] = 0xFFFF5500;
        }
    }
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, srcPixels, imageWidth * 4);
    if (!bitmap) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformance create bitmap error");
    }
    uint32_t bitmapX = 100; //for test
    uint32_t bitmapY = 100; //for test
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap, bitmapX, bitmapY);

    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    OH_Drawing_Rect* imageRectSrc = OH_Drawing_RectCreate(0, 0, imageWidth, imageHeight);
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12); //for test
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    std::string funcTitles[] = { "OH_Drawing_CanvasDrawImageRectWithSrc", "OH_Drawing_CanvasDrawImageRect",
        "OH_Drawing_CanvasReadPixels" };
    if (drawingType == DRAWING_TYPE_ATTACH_BOTH) {
        std::string str;
        str.append("pen &&brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 100); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);

    } else if (drawingType == DRAWING_TYPE_ATTACH_PEN) {
        std::string str;
        str.append("pen");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 100); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DRAWING_TYPE_ATTACH_BRUSH) {
        std::string str;
        str.append("brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 100); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_FontDestroy(titleFont);

    bool res;

    char* dstPixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4)); // 100 4 用于像素计算
    uint32_t pixelX = 0;
    uint32_t pixelY = 0;
    if (dstPixels == nullptr) {
        return;
    }
    for (int i = 0; i < testCount; i++) {
        if (func == 0) {
            //for test
            OH_Drawing_Rect* imageRectDst = OH_Drawing_RectCreate(
                itemWidth + i * 0.5, itemHeight * 3, itemWidth + imageWidth + i * 0.5, itemHeight * 3 + imageHeight);
            OH_Drawing_CanvasDrawImageRectWithSrc(
                canvas, image, imageRectSrc, imageRectDst, samplingOptions, STRICT_SRC_RECT_CONSTRAINT);
            OH_Drawing_RectDestroy(imageRectDst);
        } else if (func == 1) {
            //for test
            OH_Drawing_Rect* imageRect = OH_Drawing_RectCreate(
                itemWidth + i * 0.5, itemHeight * 3, itemWidth + imageWidth + i * 0.5, itemHeight * 3 + imageHeight);
            OH_Drawing_CanvasDrawImageRect(canvas, image, imageRect, samplingOptions);
            OH_Drawing_RectDestroy(imageRect);
        } else {
            pixelX = (bitmapX + i) % imageWidth;
            pixelY = (bitmapY + i) % imageHeight;
            res = OH_Drawing_CanvasReadPixels(canvas, &imageInfo, dstPixels, imageWidth * 4, pixelX, pixelY); //imageWidth * 4 argb buffer width
        }
    }
    OH_Drawing_Bitmap* bitmap1 = OH_Drawing_BitmapCreateFromPixels(&imageInfo, dstPixels, imageWidth * 4); //imageWidth * 4 argb buffer width
    //100,100 position(x,y)
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap1, 100, 100);

    OH_Drawing_BitmapDestroy(bitmap1);
    DESTROY_EFFECT;
    ErrorCheck();
    free(dstPixels);
    free(srcPixels);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_RectDestroy(imageRectSrc);

    OH_Drawing_SamplingOptionsDestroy(samplingOptions);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PenDestroy(pen);

    OH_Drawing_BrushDestroy(brush);
}

void PerformanceCCanvasLargeImageLoadDrawImageRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCLargeImageLoadOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BOTH, testCount_, 1);
}

void PerformanceCCanvasLargeImageLoadDrawImageRectPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCLargeImageLoadOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_PEN, testCount_, 1);
}

void PerformanceCCanvasLargeImageLoadDrawImageRectBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCLargeImageLoadOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BRUSH, testCount_, 1);
}

void PerformanceCCanvasLargeImageLoadDrawImageRectWithSrc::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCLargeImageLoadOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BOTH, testCount_, 0);
}

void PerformanceCCanvasLargeImageLoadDrawImageRectWithSrcPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCLargeImageLoadOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_PEN, testCount_, 0);
}

void PerformanceCCanvasLargeImageLoadDrawImageRectWithSrcBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCLargeImageLoadOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BRUSH, testCount_, 0);
}

void PerformanceCCanvasLargeImageLoadReadPixels::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    //2 func index
    PerformanceCLargeImageLoadOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BOTH, testCount_, 2);
}

void PerformanceCCanvasLargeImageLoadReadPixelsPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    //2 func index
    PerformanceCLargeImageLoadOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_PEN, testCount_, 2);
}

void PerformanceCCanvasLargeImageLoadReadPixelsBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    //2 func index
    PerformanceCLargeImageLoadOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BRUSH, testCount_, 2);
}