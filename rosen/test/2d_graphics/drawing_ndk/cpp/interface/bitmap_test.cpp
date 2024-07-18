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

#include "bitmap_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_space.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>

#include "test_common.h"

#include "common/log_common.h"

void ImageBuildFromBitmap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    uint32_t width = rand.nextULessThan(bitmapWidth_);
    uint32_t height = rand.nextULessThan(bitmapHeight_);
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = rand.nextULessThan(bitmapWidth_);
    float b = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_BitmapFormat format = { COLOR_FORMAT_ALPHA_8, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bm, width, height, &format);
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_LINEAR, MIPMAP_MODE_NEAREST);

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ImageBuildFromBitmap(image, bm);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_CanvasDrawImageRect(canvas, image, rect, samplingOptions);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_BitmapDestroy(bm);
}

void BitmapReadPixels::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 性能测试:100000次66ms
    const unsigned int width = 100;  // 100 宽度
    const unsigned int height = 100; // 100 高度
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat bitmapFormat { fCT, fAT };
    OH_Drawing_BitmapBuild(cBitmap, width, height, &bitmapFormat);
    OH_Drawing_Image_Info imageInfo { width, height, fCT, fAT };
    void* pixels = new uint32_t[width * height];
    bool resTrue = false;
    // 将位图中的矩形区域像素数据读取到指定的内存缓冲区中,循环攻击此接口
    for (int i = 0; i < testCount_; i++) {
        resTrue =
            OH_Drawing_BitmapReadPixels(cBitmap, &imageInfo, pixels, width * 4, 0, 0); // 4目标像素数据每行的字节数
    }
    if (resTrue == true) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, width, height);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    }
    // 释放
    if (pixels != nullptr) {
        delete[] reinterpret_cast<uint32_t*>(pixels);
        pixels = nullptr;
    }
    OH_Drawing_BitmapDestroy(cBitmap);
}

void BitmapBuild::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 128;
    int32_t h = 128;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    void* pixels = OH_Drawing_BitmapGetPixels(bm);
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_ARGB_4444, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_ARGB_4444, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_BitmapBuild(bm, w, h, &cFormat);
    }
    OH_Drawing_Bitmap* cPixels = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, w);
    OH_Drawing_CanvasBind(canvas, cPixels);  // 将画布与bitmap绑定
    OH_Drawing_CanvasDrawRect(canvas, rect); // 画一个矩形
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(cPixels);
    OH_Drawing_BitmapDestroy(bm);
}

void BitmapCreateFromPixels::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 256;
    int32_t h = 256;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    void* pixels = OH_Drawing_BitmapGetPixels(bm);
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    OH_Drawing_BitmapBuild(bm, w, h, &cFormat);
    OH_Drawing_Bitmap* cPixels = 0;
    for (int i = 0; i < testCount_; i++) {
        cPixels = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, w);
    }
    OH_Drawing_CanvasBind(canvas, cPixels);  // 将画布与bitmap绑定
    OH_Drawing_CanvasDrawOval(canvas, rect); // 画一个椭圆
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(cPixels);
    OH_Drawing_BitmapDestroy(bm);
}

void BitmapCreateFromImageInfo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 256;
    int32_t h = 256;
    int32_t rowBytes = w * h * 4;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    OH_Drawing_Bitmap* cInfo = 0;
    for (int i = 0; i < testCount_; i++) {
        cInfo = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, rowBytes);
    }
    OH_Drawing_CanvasDrawOval(canvas, rect); // 画一个椭圆
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(cInfo);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

void BitmapGetRowBytes::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 256;
    int32_t h = 256;
    int32_t rowBytes = w * h * 4;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    OH_Drawing_Bitmap* cInfo = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, rowBytes);
    uint32_t byte;
    OH_Drawing_ErrorCode code;
    for (int i = 0; i < testCount_; i++) {
        code = OH_Drawing_BitmapGetRowBytes(cInfo, &byte);
    }
    DRAWING_LOGI("BitmapGetRowBytes::OnTestPerformance OH_Drawing_ErrorCode = %{public}d", code);
    DRAWING_LOGI("BitmapGetRowBytes::OnTestPerformance byte == rowBytes is%{public}s",
        (byte == rowBytes) ? "true" : "false");
    OH_Drawing_CanvasDrawOval(canvas, rect); // 画一个椭圆
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(cInfo);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

void BitmapGetColorSpaceType::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 256;
    int32_t h = 256;
    int32_t rowBytes = w * h * 4;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    OH_Drawing_ColorSpace* colorSpaceSrgbLinear = OH_Drawing_ColorSpaceCreateSrgbLinear();
    OH_Drawing_Bitmap* Srgb = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, rowBytes);
    OH_Drawing_Bitmap* SrgbLinear = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpaceSrgbLinear, rowBytes);
    OH_Drawing_ErrorCode code1;
    OH_Drawing_ErrorCode code2;
    OH_Drawing_ColorSpaceType type1;
    OH_Drawing_ColorSpaceType type2;
    for (int i = 0; i < testCount_; i++) {
        code1 = OH_Drawing_BitmapGetColorSpaceType(Srgb, &type1);
        code2 = OH_Drawing_BitmapGetColorSpaceType(SrgbLinear, &type2);
    }
    DRAWING_LOGI("BitmapGetColorSpaceType::OnTestPerformance type1 = Srgb is%{public}s",
        (type1 == OH_Drawing_ColorSpaceType::SRGB) ? "true" : "false");
    DRAWING_LOGI("BitmapGetColorSpaceType::OnTestPerformance type2 = SrgbLinear is%{public}s",
        (type2 == OH_Drawing_ColorSpaceType::SRGB_LINEAR) ? "true" : "false");
    DRAWING_LOGI("BitmapGetColorSpaceType::OnTestPerformance OH_Drawing_ErrorCode1 = %{public}d", code1);
    DRAWING_LOGI("BitmapGetColorSpaceType::OnTestPerformance OH_Drawing_ErrorCode2 = %{public}d", code2);
    OH_Drawing_CanvasDrawOval(canvas, rect); // 画一个椭圆
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(Srgb);
    OH_Drawing_BitmapDestroy(SrgbLinear);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

void BitmapExtractSubset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 256;
    int32_t h = 256;
    int32_t rowBytes = w * h * 4;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    OH_Drawing_Bitmap* cInfo = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, rowBytes);
    OH_Drawing_Bitmap* dstBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_ErrorCode code;
    bool retrieved;
    for (int i = 0; i < testCount_; i++) {
        code = OH_Drawing_BitmapExtractSubset(cInfo, dstBitmap, rect, &retrieved);
    }
    DRAWING_LOGI("BitmapExtractSubset::OnTestPerformance dstBitmap width = w is%{public}s",
        (OH_Drawing_BitmapGetWidth(dstBitmap) == w) ? "true" : "false");
    DRAWING_LOGI("BitmapExtractSubset::OnTestPerformance dstBitmap height = h is%{public}s",
        (OH_Drawing_BitmapGetHeight(dstBitmap) == h) ? "true" : "false");
    DRAWING_LOGI("BitmapExtractSubset::OnTestPerformance OH_Drawing_ErrorCode = %{public}d", code);
    DRAWING_LOGI("BitmapExtractSubset::OnTestPerformance retrieved is%{public}s", retrieved ? "true" : "false");
    OH_Drawing_CanvasDrawOval(canvas, rect); // 画一个椭圆
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(cInfo);
    OH_Drawing_BitmapDestroy(dstBitmap);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

void BitmapIsImmutable::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 256;
    int32_t h = 256;
    int32_t rowBytes = w * h * 4;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    OH_Drawing_Bitmap* cInfo = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, rowBytes);
    OH_Drawing_ErrorCode code;
    bool immutable;
    for (int i = 0; i < testCount_; i++) {
        code = OH_Drawing_BitmapIsImmutable(cInfo, &immutable);
    }
    DRAWING_LOGI("BitmapIsImmutable::OnTestPerformance OH_Drawing_ErrorCode = %{public}d", code);
    DRAWING_LOGI("BitmapIsImmutable::OnTestPerformance immutable is%{public}s", immutable ? "true" : "false");
    OH_Drawing_CanvasDrawOval(canvas, rect); // 画一个椭圆
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(cInfo);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

void BitmapSetImmutable::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 256;
    int32_t h = 256;
    int32_t rowBytes = w * h * 4;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    OH_Drawing_Bitmap* cInfo = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, rowBytes);
    OH_Drawing_ErrorCode code;
    for (int i = 0; i < testCount_; i++) {
        code = OH_Drawing_BitmapSetImmutable(cInfo);
    }
    DRAWING_LOGI("BitmapSetImmutable::OnTestPerformance OH_Drawing_ErrorCode = %{public}d", code);
    bool immutable;
    OH_Drawing_BitmapIsImmutable(cInfo, &immutable);
    DRAWING_LOGI("BitmapSetImmutable::OnTestPerformance immutable is%{public}s", immutable ? "true" : "false");
    OH_Drawing_CanvasDrawOval(canvas, rect); // 画一个椭圆
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(cInfo);
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}