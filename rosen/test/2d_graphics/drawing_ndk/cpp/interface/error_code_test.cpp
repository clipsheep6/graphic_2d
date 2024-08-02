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

#include "error_code_test.h"

#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_error_code.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_text_typography.h>

#include "common/log_common.h"

void LogErrorCodeResult(std::string& result, OH_Drawing_ErrorCode expectedValue)
{
    std::string functionName = result;
    if (OH_Drawing_ErrorCodeGet() == expectedValue) {
        result += " : success";
        DRAWING_LOGI("%{public}s: Expected value matches actual value", functionName.c_str());
    } else {
        result += ": fail";
        DRAWING_LOGE("%{public}s: Actual error code: %{public}d does not match expected value: %{public}d",
            functionName.c_str(), OH_Drawing_ErrorCodeGet(), expectedValue);
    }
}

void CreateBitmapFromPixels(
    std::string& result, OH_Drawing_Image_Info* cImageInfo, void* pixels, OH_Drawing_ErrorCode expectedValue)
{
    OH_Drawing_BitmapCreateFromPixels(cImageInfo, pixels, 0);
    result = std::string(__func__);
    LogErrorCodeResult(result, expectedValue);
}

void CreateBitmapBuild(std::string& result, OH_Drawing_Bitmap* cBitmap, OH_Drawing_BitmapFormat* cBitmapFormat,
    OH_Drawing_ErrorCode expectedValue)
{
    OH_Drawing_BitmapBuild(cBitmap, 10, 10, cBitmapFormat);
    result = std::string(__func__);
    LogErrorCodeResult(result, expectedValue);
}

void GetValueFromMatrix1(std::string& result, OH_Drawing_Matrix* matrix, OH_Drawing_ErrorCode expectedValue)
{
    OH_Drawing_MatrixGetValue(matrix, 0);
    result = std::string(__func__);
    LogErrorCodeResult(result, expectedValue);
}

void GetValueFromMatrix2(std::string& result, OH_Drawing_Matrix* matrix, OH_Drawing_ErrorCode expectedValue)
{
    OH_Drawing_MatrixGetValue(matrix, 9);
    result = std::string(__func__);
    LogErrorCodeResult(result, expectedValue);
}

void ThreadErrorCodeTest::DrawTextOnCanvas(OH_Drawing_Canvas* canvas, const char* text)
{
    constexpr float penWidth = 3.0f;
    // 创建一个画笔Pen对象，Pen对象用于形状的边框线绘制
    auto cPen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(cPen, true);                                        // 开启了画笔的抗锯齿功能
    OH_Drawing_PenSetColor(cPen, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00)); // 设置了画笔的颜色为黑色
    OH_Drawing_PenSetWidth(cPen, penWidth);                                        // 设置了画笔的宽度
    OH_Drawing_PenSetJoin(cPen, LINE_ROUND_JOIN); // 设置了画笔的连接方式为圆角连接
    OH_Drawing_CanvasAttachPen(canvas, cPen);     // 将Pen画笔设置到canvas中

    // 选择从左到右/左对齐等排版属性
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextDirection(typoStyle, TEXT_DIRECTION_LTR); // 设置了文本的方向为从左到右
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_JUSTIFY); // 设置了文本的对齐方式为两端对齐

    // 设置文字颜色，例如黑色
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x66, 0x00, 0x00)); // 设置文字颜色
    double width = 200;
    double fontSize = width / 6;                                  // 计算文字的大小
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);          // 设置文字的大小
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400); // 设置文字的字重
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    OH_Drawing_SetTextStyleFontHeight(txtStyle, 1);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies); // 设置文字的字体类型
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);  // 设置文字的字体风格
    OH_Drawing_SetTextStyleLocale(txtStyle, "en");                  // 设置文字的语言环境 英文
    // 将排版样式和文字样式添加到其中
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);

    // 设置文字内容
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    // 将排版处理器 handler 传入其中
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    // 设置页面最大宽度
    double maxWidth = width * 4;
    OH_Drawing_TypographyLayout(typography, maxWidth);

    // 绘制文字到画布指定位置
    OH_Drawing_TypographyPaint(typography, canvas, 0, 0);
}

void ThreadErrorCodeTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    std::vector<std::string> threadResults(20);
    for (int i = 0; i < 5; i++) {
        CreateThread([&threadResults, i]() {
            OH_Drawing_Image_Info* cImageInfo = nullptr;
            void* pixels = nullptr;
            // 401
            CreateBitmapFromPixels(threadResults[i * 4], cImageInfo, pixels, OH_DRAWING_ERROR_INVALID_PARAMETER);
        });

        CreateThread([&threadResults, i]() {
            OH_Drawing_Matrix* matrixTest1 = nullptr;
            // 401
            GetValueFromMatrix1(threadResults[i * 4 + 1], matrixTest1, OH_DRAWING_ERROR_INVALID_PARAMETER);
        });

        CreateThread([&threadResults, i]() {
            OH_Drawing_Matrix* matrixTest2 = OH_Drawing_MatrixCreate();
            // 26200001
            GetValueFromMatrix2(threadResults[i * 4 + 2], matrixTest2, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
        });

        CreateThread([&threadResults, i]() {
            OH_Drawing_Bitmap* cBitmap = nullptr;
            OH_Drawing_BitmapFormat* cBitmapFormat = nullptr;
            // 401
            CreateBitmapBuild(threadResults[i * 4 + 3], cBitmap, cBitmapFormat, OH_DRAWING_ERROR_INVALID_PARAMETER);
        });
    }
    // 阻塞等待所有线程执行完成
    BlockThread();
    std::string resultsText;
    for (int i = 0; i < threadResults.size(); i++) {
        resultsText += "thread" + std::to_string(i) + " : " + threadResults[i] + "\n";
    }
    DrawTextOnCanvas(canvas, resultsText.c_str());
}

void ThreadErrorCodeTest::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    return;
}