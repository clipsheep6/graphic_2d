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

#include "sub_basic_graphics_special_performance_c_ops_witch.h"

#include <cstdint>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
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

#include "hilog/log.h"
#include "native_drawing/drawing_error_code.h"
#include "native_drawing/drawing_pixel_map.h"
#include "native_drawing/drawing_shadow_layer.h"
#include "performance.h"
#include "test_common.h"

#include "common/drawing_type.h"
#include "common/log_common.h"

void SubBasicGraphicsSpecialPerformanceCOpsWitchOnTestPerformance(
    OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    DRAWING_LOGI("SubBasicGraphicsSpecialPerformanceCOpsWitch OnTestPerformance");
    const int32_t itemWidth = 100;  // 100
    const int32_t itemHeight = 100; // 100
    if (!canvas) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformanceCOpsWitch OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    SET_PEN_AND_BRUSH_EFFECT;
    // 画线
    ErrorCheck();
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12); //for test
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    if (drawingType == DRAWING_TYPE_ATTACH_BOTH) {
        OH_Drawing_TextBlob* textBlob =
            OH_Drawing_TextBlobCreateFromString("pen &&brush", titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 200, 100); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DRAWING_TYPE_ATTACH_PEN) {
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString("pen", titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 200, 100); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DRAWING_TYPE_ATTACH_BRUSH) {
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString("brush", titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 200, 100); //for test
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_FontDestroy(titleFont);

    ErrorCheck();
    // 创建pixmap
    OH_Pixelmap_InitializationOptions* createOps = nullptr;
    auto ret = OH_PixelmapInitializationOptions_Create(&createOps);
    if (ret != IMAGE_SUCCESS || !createOps) {
        DRAWING_LOGE(
            "SubBasicGraphicsSpecialPerformanceCOpsWitch:: OH_PixelmapInitializationOptions_Create failed %{public}d",
            ret);
        return;
    }
    int32_t imageWidth = 1024 * 4; // 1024 * 4 for test,4k
    int32_t imageHeight = 1080 * 2; // 1024 * 4 for test,4k
    OH_PixelmapInitializationOptions_SetWidth(createOps, imageWidth);
    OH_PixelmapInitializationOptions_SetHeight(createOps, imageHeight);
    OH_PixelmapInitializationOptions_SetPixelFormat(createOps, 3);    // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetSrcPixelFormat(createOps, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetAlphaType(createOps, 2);      // 2 is ALPHA_FORMAT_PREMUL

    size_t bufferSize = imageWidth * imageHeight * 4;

    void* bitmapAddr = malloc(bufferSize);
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        // 100 for test
        if (i < imageWidth * 100) {
            ((uint32_t*)bitmapAddr)[i] = 0xFF5500FF;
        // 300 for test    
        } else if (i < imageWidth * 300) {
            ((uint32_t*)bitmapAddr)[i] = 0xFF0055FF;
        } else {
            ((uint32_t*)bitmapAddr)[i] = 0xFFFF5500;
        }
    }
    ErrorCheck();
    OH_PixelmapNative* pixelMapNative = nullptr;

    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    ret = OH_PixelmapNative_CreatePixelmap((uint8_t*)bitmapAddr, bufferSize, createOps, &pixelMapNative);
    OH_Drawing_PixelMap* pixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMapNative);

    ErrorCheck();
    // 画region，region目前不支持左上点大约右下点的情况
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    ErrorCheck();
    // 画点
    const int32_t pointCount = 100; //for test
    OH_Drawing_Point2D point2D[pointCount];

    OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888,
        ALPHA_FORMAT_PREMUL }; // 100 用于宽高
    // 画bitmap
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, bitmapAddr, imageWidth * 4);  //for test

    // 画bitmaprect 第二行1
    OH_Drawing_Rect* bitMapSrc = OH_Drawing_RectCreate(0, 0, itemWidth, itemHeight);

    // 画圆
    OH_Drawing_Point* point = OH_Drawing_PointCreate(itemWidth * 3 + 10, itemHeight * 2 + 30);  //for test

    // 画弧
    OH_Drawing_Rect* arcRect =
        OH_Drawing_RectCreate(itemWidth * 5, itemHeight * 2 + 10, itemWidth * 5 + 50, itemHeight * 2 + 100);  //for test

    // 画带边角的rect
    OH_Drawing_Rect* roundRectRect =
        OH_Drawing_RectCreate(itemWidth * 6, itemHeight * 2 + 10, itemWidth * 6 + 50, itemHeight * 2 + 60);  //for test

    // 画textblob 第三行
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 18); //for test
    OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString("font test你好 @", font, TEXT_ENCODING_UTF8);

    // 画阴影
    OH_Drawing_Path* shadowPath = OH_Drawing_PathCreate();
    OH_Drawing_PathArcTo(shadowPath, 0, 0, 10, 10, 0, 1.8);  //for test

    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    OH_Drawing_Rect* imageRectSrc = OH_Drawing_RectCreate(0, 0, 100, 100);  //for test
    //

    int32_t xOffset = 0;
    //10 testCount
    for (int i = 0; i < 10; i++) {
        xOffset = i % itemWidth;
        OH_Drawing_CanvasDrawLine(canvas, itemWidth + xOffset, itemHeight, itemWidth + 30, itemHeight + 70); //for test
        // draw path
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathMoveTo(path, itemWidth * 2 + xOffset, itemHeight); //for test
        OH_Drawing_PathLineTo(path, itemWidth * 2 + 50, itemHeight + 50); //for test
        OH_Drawing_PathAddRect(
            path, itemWidth * 2 + 50, itemHeight + 50, itemWidth * 2 + 80, itemHeight + 90, PATH_DIRECTION_CCW); //for test
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathDestroy(path);
        OH_Drawing_Rect* dst = OH_Drawing_RectCreate(
            itemWidth * 3 + xOffset, itemHeight, itemWidth * 4 + xOffset, itemHeight + imageHeight); //for test
        OH_Drawing_CanvasDrawPixelMapRect(canvas, pixelMap, nullptr, dst, samplingOptions);
        OH_Drawing_RectDestroy(dst);
        OH_Drawing_Rect* regionRect =
            OH_Drawing_RectCreate(itemWidth * 4 + xOffset, itemHeight, itemWidth * 4 + 70 + xOffset, itemHeight + 70); //for test
        ErrorCheck();
        OH_Drawing_RegionSetRect(region, regionRect);
        OH_Drawing_CanvasDrawRegion(canvas, region);
        OH_Drawing_RectDestroy(regionRect);
        for (int i = 0; i < pointCount; i++) {
            point2D[i] = { itemWidth + i + xOffset * 1.f, itemHeight + i * 1.f };
        }
        OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_POINTS, pointCount, point2D);
        OH_Drawing_CanvasDrawBitmap(canvas, bitmap, itemWidth * 6 + xOffset, itemHeight); //for test position x
        OH_Drawing_Rect* bitMapDst =
            OH_Drawing_RectCreate(itemWidth + xOffset, itemHeight * 2, itemWidth * 2 + xOffset, itemHeight * 2 + 100); //for test
        OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, bitMapSrc, bitMapDst, samplingOptions);
        OH_Drawing_RectDestroy(bitMapDst);
        // 画rect
        OH_Drawing_Rect* canvasRect = OH_Drawing_RectCreate(
            itemWidth * 2 + xOffset, itemHeight * 2 + 20, itemWidth * 2 + xOffset + 30, itemHeight * 2 + 50); //for test
        OH_Drawing_CanvasDrawRect(canvas, canvasRect);
        OH_Drawing_RectDestroy(canvasRect);
        OH_Drawing_CanvasDrawCircle(canvas, point, 10 + xOffset); //for test
        OH_Drawing_Rect* ovalRect = OH_Drawing_RectCreate(
            itemWidth * 4 + xOffset, itemHeight * 2 + 10, itemWidth * 4 + 50 + xOffset, itemHeight * 2 + 80); //for test
        OH_Drawing_CanvasDrawOval(canvas, ovalRect);
        OH_Drawing_RectDestroy(ovalRect);
        OH_Drawing_CanvasDrawArc(canvas, arcRect, 0, 1.8 + xOffset); //for test
        OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(roundRectRect, 5 + xOffset, 10); //for test
        OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
        OH_Drawing_RoundRectDestroy(roundRect);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, itemWidth + xOffset, itemHeight * 3 + 50); // 3,50 for test,mean right
        OH_Drawing_CanvasDrawShadow(
            canvas, shadowPath, { 5 + xOffset * 1.f, 6, 7 }, { 1, 2, 3 }, 5, 0xFF0000FF, 0xFFFF00FF, SHADOW_FLAGS_NONE); //for test
        OH_Drawing_Rect* imageRectDst = OH_Drawing_RectCreate(
            itemWidth * 2 + xOffset, itemHeight * 3, itemWidth * 2 + xOffset + 100, itemHeight * 3 + 100); //for test
        OH_Drawing_CanvasDrawImageRectWithSrc(
            canvas, image, imageRectSrc, imageRectDst, samplingOptions, STRICT_SRC_RECT_CONSTRAINT);
        OH_Drawing_RectDestroy(imageRectDst);
        OH_Drawing_Rect* imageRect = OH_Drawing_RectCreate(
            itemWidth * 3 + xOffset, itemHeight * 3, itemWidth * 3 + xOffset + 100, itemHeight * 3 + 100); //for test
        OH_Drawing_CanvasDrawImageRect(canvas, image, imageRect, samplingOptions);
        OH_Drawing_RectDestroy(imageRect);
    }
    DESTROY_EFFECT;

    OH_Drawing_RectDestroy(imageRectSrc);
    OH_Drawing_PathDestroy(shadowPath);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TextBlobDestroy(textBlob);

    OH_Drawing_RectDestroy(roundRectRect);
    OH_Drawing_RectDestroy(arcRect);

    OH_Drawing_PointDestroy(point);

    OH_Drawing_RectDestroy(bitMapSrc);
    OH_Drawing_BitmapDestroy(bitmap);

    OH_Drawing_RegionDestroy(region);

    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_PixelmapNative_Release(pixelMapNative);
    free(bitmapAddr);
    OH_PixelmapInitializationOptions_Release(createOps);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
}

void SubBasicGraphicsSpecialPerformanceCOpsWitch::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    SubBasicGraphicsSpecialPerformanceCOpsWitchOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BOTH, testCount_);
}

void SubBasicGraphicsSpecialPerformanceCOpsWitchPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    SubBasicGraphicsSpecialPerformanceCOpsWitchOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_PEN, testCount_);
}

void SubBasicGraphicsSpecialPerformanceCOpsWitchBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    SubBasicGraphicsSpecialPerformanceCOpsWitchOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BRUSH, testCount_);
}

void SubBasicGraphicsSpecialPerformanceCLineAndPathOnTestPerformance(
    OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    DRAWING_LOGI("SubBasicGraphicsSpecialPerformanceCOpsWitch OnTestPerformance");
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

    OH_Drawing_BrushSetFilter(brush, filter);

    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12); //for test
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    if (drawingType == DRAWING_TYPE_ATTACH_BOTH) {
        OH_Drawing_TextBlob* textBlob =
            OH_Drawing_TextBlobCreateFromString("pen &&brush", titleFont, TEXT_ENCODING_UTF8);
        // 300,600 position(x,y)    
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DRAWING_TYPE_ATTACH_PEN) {
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString("pen", titleFont, TEXT_ENCODING_UTF8);
        // 300,600 position(x,y) 
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DRAWING_TYPE_ATTACH_BRUSH) {
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString("brush", titleFont, TEXT_ENCODING_UTF8);
        // 300,600 position(x,y) 
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300, 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_FontDestroy(titleFont);
    ErrorCheck();
    // drawing path
    // 100 init x,move position x
    int32_t x = 100;
    // 100 init y,move position y
    int32_t y = 100;
    //100, testCount
    for (int i = 0; i < 100; i++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        // 10 for test
        OH_Drawing_CanvasDrawLine(canvas, x, y, x + 10, y + 10);
        OH_Drawing_PathMoveTo(path, x, y);
        // 60 for test
        OH_Drawing_PathLineTo(path, x + 60, y);
        OH_Drawing_CanvasDrawPath(canvas, path);
        // 30,10,60 for test
        OH_Drawing_PathQuadTo(path, x + 30, y + 10, x + 60, y);
        OH_Drawing_CanvasDrawPath(canvas, path);
        // 30,40,50,120 for test
        OH_Drawing_PathCubicTo(path, x + 30, y + 3, x + 40, y + 50, x + 120, y);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathDestroy(path);
        // 15 for test,move position x
        x += 15;
        if (x > 600) {
            // 100 init x,move position x
            x = 100;
            // 15 for test,move position y
            y += 15;
        }
    }
    DESTROY_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
}

void SubBasicGraphicsSpecialPerformanceCLineAndPath::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    SubBasicGraphicsSpecialPerformanceCLineAndPathOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BOTH, testCount_);
}

void SubBasicGraphicsSpecialPerformanceCLineAndPathPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    SubBasicGraphicsSpecialPerformanceCLineAndPathOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_PEN, testCount_);
}

void SubBasicGraphicsSpecialPerformanceCLineAndPathBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    SubBasicGraphicsSpecialPerformanceCLineAndPathOnTestPerformance(canvas, DRAWING_TYPE_ATTACH_BRUSH, testCount_);
}
