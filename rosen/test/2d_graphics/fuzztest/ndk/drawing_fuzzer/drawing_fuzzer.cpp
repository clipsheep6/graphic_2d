/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "drawing_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"

#include "c/drawing_bitmap.h"
#include "c/drawing_brush.h"
#include "c/drawing_canvas.h"
#include "c/drawing_color.h"
#include "c/drawing_font_collection.h"
#include "c/drawing_path.h"
#include "c/drawing_pen.h"
#include "c/drawing_text_declaration.h"
#include "c/drawing_text_typography.h"
#include "c/drawing_types.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

void NativeDrawingBitmapTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap, width, height, &bitmapFormat);
    OH_Drawing_BitmapDestroy(bitmap);
}

void NativeDrawingBrushTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, false);
    OH_Drawing_BrushIsAntiAlias(brush);
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_BrushGetColor(brush);
    OH_Drawing_BrushDestroy(brush);
}

void NativeDrawingCanvasBitmapTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_CanvasBind(canvas, bitmap);
    OH_Drawing_CanvasDestroy(canvas);
}

void NativeDrawingCanvasPenTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDestroy(canvas);
}

void NativeDrawingCanvasBrushTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_CanvasDestroy(canvas);
}

void NativeDrawingCanvasDrawLineTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasDrawLine(canvas, 0, 0, 20, 20);
    OH_Drawing_CanvasDrawLine(canvas, -15.2f, -1, 0, 20);
    OH_Drawing_CanvasDestroy(canvas);
}

void NativeDrawingCanvasDrawPathTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasDestroy(canvas);
}

void NativeDrawingCanvasClearTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasClear(canvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_CanvasDestroy(canvas);
}

void NativeDrawingColorTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, false);
    OH_Drawing_BrushIsAntiAlias(brush);
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_BrushGetColor(brush);
    OH_Drawing_BrushDestroy(brush);
}

void OHDrawingFontCollectionTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    OH_Drawing_DestroyFontCollection(fontCollection);
}

void NativeDrawingPathMoveToTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 20, 20);
    OH_Drawing_PathMoveTo(path, -1, 20.6f);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathLineToTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathLineTo(path, 50, 40);
    OH_Drawing_PathLineTo(path, -50, 10.2f);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathResetTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 20, 20);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathArcToTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathArcTo(path, 10, 10, 20, 0, 0, 90);
    OH_Drawing_PathArcTo(path, -10, 10, 10, -10, 0, 90);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathQuadToTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathQuadTo(path, 0, 0, 30, 30);
    OH_Drawing_PathQuadTo(path, -20.5f, -20.5f, 30, 0);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathCubicToTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(path, 30, 40, 60, 0, 50, 20);
    OH_Drawing_PathCubicTo(path, -30, 40, 60, -30.6f, 50, -20);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathCloseTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathLineTo(path, 50, 40);
    OH_Drawing_PathClose(path);
    OH_Drawing_PathDestroy(path);
}

void OHDrawingTypographyTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    OH_Drawing_TypographyGetMaxWidth(typography);
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_BitmapGetWidth(cBitmap);
    OH_Drawing_BitmapGetHeight(cBitmap);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyGetHeight(typography);
    OH_Drawing_TypographyGetLongestLine(typography);
    OH_Drawing_TypographyGetMinIntrinsicWidth(typography);
    OH_Drawing_TypographyGetMaxIntrinsicWidth(typography);
    OH_Drawing_TypographyGetAlphabeticBaseline(typography);
    OH_Drawing_TypographyGetIdeographicBaseline(typography);
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
}

void NativeDrawingPenTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenIsAntiAlias(pen);
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenGetColor(pen);
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_PenGetWidth(pen);
    OH_Drawing_PenSetMiterLimit(pen, 5);
    OH_Drawing_PenGetMiterLimit(pen);
    OH_Drawing_PenSetCap(pen, OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP);
    OH_Drawing_PenGetCap(pen);
    OH_Drawing_PenSetJoin(pen, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    OH_Drawing_PenGetJoin(pen);
    OH_Drawing_PenDestroy(pen);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingBitmapTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingBrushTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasBitmapTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasPenTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasBrushTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasDrawLineTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasDrawPathTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasClearTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingColorTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingFontCollectionTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathMoveToTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathLineToTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathResetTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathArcToTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathQuadToTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathCubicToTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathCloseTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingTypographyTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPenTest(data, size);

    return 0;
}
