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

#include "draw_picture_recorder_test.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_text_typography.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_picture_recorder.h>
#include <native_drawing/drawing_picture.h>
#include <cmath>
#include <native_drawing/drawing_region.h>

void DrawPictureRecorderTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_PictureRecorder *ohPictureRecorder = OH_Drawing_PictureRecorderCreate();
    OH_Drawing_Canvas *ohRecorderCanvas= OH_Drawing_BeginRecording(ohPictureRecorder, 300, 300);
    // 创建一个path对象，然后使用接口连接成一个五角星形状
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    int len = bitmapHeight_ / 4;
    float aX = bitmapWidth_ / 2;
    float aY = bitmapHeight_ / 4;
    float dX = aX - len * std::sin(18.0f);
    float dY = aY + len * std::cos(18.0f);
    float cX = aX + len * std::sin(18.0f);
    float cY = dY;
    float bX = aX + (len / 2.0);
    float bY = aY + std::sqrt((cX - dX) * (cX - dX) + (len / 2.0) * (len / 2.0));
    float eX = aX - (len / 2.0);
    float eY = bY;
    // 创建一个path对象，然后使用接口连接成一个五角星形状
    path = OH_Drawing_PathCreate();
    // 指定path的起始位置
    OH_Drawing_PathMoveTo(path, aX, aY);
    // 用直线连接到目标点
    OH_Drawing_PathLineTo(path, bX, bY);
    OH_Drawing_PathLineTo(path, cX, cY);
    OH_Drawing_PathLineTo(path, dX, dY);
    OH_Drawing_PathLineTo(path, eX, eY);
    // 闭合形状，path绘制完毕
    OH_Drawing_PathClose(path);

    constexpr float penWidth = 10.0f; // pen width 10
    // 创建一个画笔Pen对象，Pen对象用于形状的边框线绘制
    OH_Drawing_Pen* cPen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(cPen, true);
    OH_Drawing_PenSetColor(cPen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetWidth(cPen, penWidth);
    OH_Drawing_PenSetJoin(cPen, LINE_ROUND_JOIN);
    // 将Pen画笔设置到canvas中
    OH_Drawing_CanvasAttachPen(ohRecorderCanvas, cPen);
    // 创建一个画刷Brush对象，Brush对象用于形状的填充
    OH_Drawing_Brush *cBrush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(cBrush, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0x00));
    // 将Brush画刷设置到canvas中
    OH_Drawing_CanvasAttachBrush(ohRecorderCanvas, cBrush);
    // 在画布上画path的形状，五角星的边框样式为pen设置，颜色填充为Brush设置
    OH_Drawing_CanvasDrawPath(ohRecorderCanvas, path);
    OH_Drawing_Picture *picture =OH_Drawing_FinishingRecording(ohPictureRecorder);
    OH_Drawing_CanvasDrawPicture(canvas, picture);
    OH_Drawing_PenDestroy(cPen);
    cPen = nullptr;
}

void DrawPictureRecorderTest::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_PictureRecorder *ohPictureRecorder = OH_Drawing_PictureRecorderCreate();
    OH_Drawing_Canvas *ohRecorderCanvas = OH_Drawing_BeginRecording(ohPictureRecorder, 300, 300);
    // 创建一个path对象，然后使用接口连接成一个五角星形状
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    int len = bitmapHeight_ / 4;
    float aX = bitmapWidth_ / 2;
    float aY = bitmapHeight_ / 4;
    float dX = aX - len * std::sin(18.0f);
    float dY = aY + len * std::cos(18.0f);
    float cX = aX + len * std::sin(18.0f);
    float cY = dY;
    float bX = aX + (len / 2.0);
    float bY = aY + std::sqrt((cX - dX) * (cX - dX) + (len / 2.0) * (len / 2.0));
    float eX = aX - (len / 2.0);
    float eY = bY;
    // 创建一个path对象，然后使用接口连接成一个五角星形状
    path = OH_Drawing_PathCreate();
    // 指定path的起始位置
    OH_Drawing_PathMoveTo(path, aX, aY);
    // 用直线连接到目标点
    OH_Drawing_PathLineTo(path, bX, bY);
    OH_Drawing_PathLineTo(path, cX, cY);
    OH_Drawing_PathLineTo(path, dX, dY);
    OH_Drawing_PathLineTo(path, eX, eY);
    // 闭合形状，path绘制完毕
    OH_Drawing_PathClose(path);

    constexpr float penWidth = 10.0f; // pen width 10
    // 创建一个画笔Pen对象，Pen对象用于形状的边框线绘制
    OH_Drawing_Pen *cPen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(cPen, true);
    OH_Drawing_PenSetColor(cPen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetWidth(cPen, penWidth);
    OH_Drawing_PenSetJoin(cPen, LINE_ROUND_JOIN);
    // 将Pen画笔设置到canvas中
    OH_Drawing_CanvasAttachPen(ohRecorderCanvas, cPen);
    // 创建一个画刷Brush对象，Brush对象用于形状的填充
    OH_Drawing_Brush *cBrush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(cBrush, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0x00));
    // 将Brush画刷设置到canvas中
    OH_Drawing_CanvasAttachBrush(ohRecorderCanvas, cBrush);
    // 在画布上画path的形状，五角星的边框样式为pen设置，颜色填充为Brush设置
    OH_Drawing_CanvasDrawPath(ohRecorderCanvas, path);
    OH_Drawing_Picture *picture = OH_Drawing_FinishingRecording(ohPictureRecorder);
    OH_Drawing_CanvasDrawPicture(canvas, picture);
    OH_Drawing_PenDestroy(cPen);
    cPen = nullptr;
}

