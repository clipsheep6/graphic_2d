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
 
#include "testcase/canvas_draw_performance.h"
#include "testcase/drawing_type.h"
#include "dm/points.h"
#include "testcase/performance.h"
#include <cstdint>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pixel_map.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>

// 超多点绘制性能
void CanvasDrawPointsOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    
    OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    OH_Drawing_BrushSetColor(brush, DRAW_COLORGREEN);
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12);
    static int func = 0;
    std::string funcTitles[] = {
        "OH_Drawing_CanvasDrawPoints",
    };
    if (drawingType == DrawingTypeAttachBoth) {
        std::string str;
        str.append("pen && brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachPen) {
        std::string str;
        str.append("pen");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDetachBrush(canvas);
    } else if (drawingType == DrawingTypeAttachBrush) {
        std::string str;
        str.append("brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_PenSetAlpha(pen, 255);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_BlendMode model = BLEND_MODE_CLEAR;
    OH_Drawing_PenSetBlendMode(pen, model);
    OH_Drawing_PenLineCapStyle lineCapStyle = LINE_FLAT_CAP;
    OH_Drawing_PenSetCap(pen, lineCapStyle);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xFFFF00FF, BLEND_MODE_DST_IN);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_PenLineJoinStyle lineJoinStyle = LINE_MITER_JOIN;
    OH_Drawing_PenSetJoin(pen, lineJoinStyle);
    OH_Drawing_PenSetMiterLimit(pen, 3.0);
    float intervals[] = {1.0, 2,0, 3.0, 4.0, 5.0};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 20, 5.0);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFFFFFF);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);

    OH_Drawing_BrushSetAlpha(brush, 255);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetBlendMode(brush, model);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    
    // 绘制点
    OH_Drawing_Point2D pts[5000]; // 点个数
    // 绘制1000次
    for (int i = 0; i < testCount; i++) {
        for(int j = 0; j < 5000; j++){
            pts[j].x = 2 * j;
            pts[j].y = 2 * i;
        }
        OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_POINTS, 5000, pts);
    }
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_FilterDestroy(filter);
}

void CanvasDrawPointsPerformance::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawPointsOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawPointsPerformancePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawPointsOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}

void CanvasDrawPointsPerformanceBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
   CanvasDrawPointsOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}
// 超大折线绘制性能
void CanvasDrawBrokenLineOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    OH_Drawing_BrushSetColor(brush, DRAW_COLORGREEN);
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12);
    static int func = 0;
    std::string funcTitles[] = {
        "OH_Drawing_CanvasDrawPath",
    };
    if (drawingType == DrawingTypeAttachBoth) {
        std::string str;
        str.append("pen && brush ");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachPen) {
        std::string str;
        str.append("pen ");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDetachBrush(canvas);
    } else if (drawingType == DrawingTypeAttachBrush) {
        std::string str;
        str.append("brush ");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_PenSetAlpha(pen, 255);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_BlendMode model = BLEND_MODE_CLEAR;
    OH_Drawing_PenSetBlendMode(pen, model);
    OH_Drawing_PenLineCapStyle lineCapStyle = LINE_FLAT_CAP;
    OH_Drawing_PenSetCap(pen, lineCapStyle);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xFFFF00FF, BLEND_MODE_DST_IN);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_PenLineJoinStyle lineJoinStyle = LINE_MITER_JOIN;
    OH_Drawing_PenSetJoin(pen, lineJoinStyle);
    OH_Drawing_PenSetMiterLimit(pen, 3.0);
    float intervals[] = {1.0, 2,0, 3.0, 4.0, 5.0};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 20, 5.0);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFFFFFF);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);

    OH_Drawing_BrushSetAlpha(brush, 255);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetBlendMode(brush, model);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    int numLines = 200; // 折线点的数量
    int lineHeight = 10; // 折线点的垂直偏移
    int lineSpacing = 10; // 折线点之间的水平间距
    // 绘制首尾相连的上下折线
    int currentX = 0; // 初始 x 坐标
    int currentY = 0; // 初始 y 坐标
    int isUp = true; // 折线方向标志
    for(int i = 0; i < testCount; i++){
        OH_Drawing_PathMoveTo(path, 0, 2 * i);
        currentX = 0;
        currentY = 2 * i;
        for (int j = 0; j < numLines; j++) {
          if (isUp) {
            currentY += lineHeight;
          } else {
            currentY -= lineHeight;
          }
          isUp = !isUp; // 切换折线方向
          currentX += lineSpacing; // 向右移动
          OH_Drawing_PathLineTo(path, currentX, currentY);
        }
    }
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_FilterDestroy(filter);
}

void CanvasDrawBrokenLinePerformance::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawBrokenLineOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawBrokenLinePerformancePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawBrokenLineOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}

void CanvasDrawBrokenLinePerformanceBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
   CanvasDrawBrokenLineOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}
// 超大曲线绘制性能
void CanvasDrawCurveLineOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    OH_Drawing_BrushSetColor(brush, DRAW_COLORGREEN);
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12);
    static int func = 0;
    std::string funcTitles[] = {
        "OH_Drawing_CanvasDrawOval OH_Drawing_PathRQuadTo",
    };
    if (drawingType == DrawingTypeAttachBoth) {
        std::string str;
        str.append("pen && brush ");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachPen) {
        std::string str;
        str.append("pen ");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDetachBrush(canvas);
    } else if (drawingType == DrawingTypeAttachBrush) {
        std::string str;
        str.append("brush ");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_PenSetAlpha(pen, 255);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_BlendMode model = BLEND_MODE_CLEAR;
    OH_Drawing_PenSetBlendMode(pen, model);
    OH_Drawing_PenLineCapStyle lineCapStyle = LINE_FLAT_CAP;
    OH_Drawing_PenSetCap(pen, lineCapStyle);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xFFFF00FF, BLEND_MODE_DST_IN);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_PenLineJoinStyle lineJoinStyle = LINE_MITER_JOIN;
    OH_Drawing_PenSetJoin(pen, lineJoinStyle);
    OH_Drawing_PenSetMiterLimit(pen, 3.0);
    float intervals[] = {1.0, 2,0, 3.0, 4.0, 5.0};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 20, 5.0);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFFFFFF);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);

    OH_Drawing_BrushSetAlpha(brush, 255);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetBlendMode(brush, model);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    // 画椭圆和贝塞尔曲线
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // 画的多会无响应
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 20; j++){
            float left = 10 + 20 * j;
            float top = 10 + 30 * i;
            float right = 30 + 20 * j;
            float bottom = 40 + 30 * i;
            OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
            OH_Drawing_CanvasDrawOval(canvas, rect);
            OH_Drawing_PathMoveTo(path, 20 * j,  30 * i);
            OH_Drawing_PathRQuadTo(path, 20 * j,  30 * i, 20 * j + 20, 30 * i);
            OH_Drawing_CanvasDrawPath(canvas, path);
        }
        OH_Drawing_PathMoveTo(path, 0, 30 * i);
    }
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_FilterDestroy(filter);
}
void CanvasDrawCurveLinePerformance::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawCurveLineOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawCurveLinePerformancePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawCurveLineOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}

void CanvasDrawCurveLinePerformanceBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
   CanvasDrawCurveLineOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}
// 超大矩形组绘制性能
void CanvasDrawRegionOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    OH_Drawing_BrushSetColor(brush, DRAW_COLORGREEN);
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12);
    static int func = 0;
    std::string funcTitles[] = {
        "OH_Drawing_CanvasDrawOval OH_Drawing_PathRQuadTo",
    };
    if (drawingType == DrawingTypeAttachBoth) {
        std::string str;
        str.append("pen && brush ");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachPen) {
        std::string str;
        str.append("pen ");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDetachBrush(canvas);
    } else if (drawingType == DrawingTypeAttachBrush) {
        std::string str;
        str.append("brush ");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_PenSetAlpha(pen, 255);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_BlendMode model = BLEND_MODE_CLEAR;
    OH_Drawing_PenSetBlendMode(pen, model);
    OH_Drawing_PenLineCapStyle lineCapStyle = LINE_FLAT_CAP;
    OH_Drawing_PenSetCap(pen, lineCapStyle);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xFFFF00FF, BLEND_MODE_DST_IN);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_PenLineJoinStyle lineJoinStyle = LINE_MITER_JOIN;
    OH_Drawing_PenSetJoin(pen, lineJoinStyle);
    OH_Drawing_PenSetMiterLimit(pen, 3.0);
    float intervals[] = {1.0, 2,0, 3.0, 4.0, 5.0};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 20, 5.0);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFFFFFF);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);

    OH_Drawing_BrushSetAlpha(brush, 255);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetBlendMode(brush, model);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    // 矩形和圆角矩形
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    for(int i = 0; i < testCount; i++){
        for(int j = 0; j < 1000; j++){
            float left = 0 + 10 * j;
            float top = 0 + 15 * i;
            float right = 20 + 10 * j;
            float bottom = 30 + 15 * i;
            float xRad = 5;
            float yRad = 5;
            OH_Drawing_CanvasDrawRegion(canvas, region);
            OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
            OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, xRad, yRad);
            if(j % 2){
                OH_Drawing_CanvasDrawRect(canvas, rect);
                OH_Drawing_RectDestroy(rect);
            }else{
                OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
                OH_Drawing_RoundRectDestroy(roundRect);
            }
        }
    }
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_FilterDestroy(filter);
}
void CanvasDrawRegionPerformance::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawRegionOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawRegionPerformancePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawRegionOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}

void CanvasDrawRegionPerformanceBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
   CanvasDrawRegionOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}
// 超大位图bitMap
void CanvasDrawBitMapOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    OH_Drawing_BrushSetColor(brush, DRAW_COLORGREEN);
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12);
    static int func = 0;
    std::string funcTitles[] = {
        "OH_Drawing_CanvasDrawBitMap",
    };
    if (drawingType == DrawingTypeAttachBoth) {
        std::string str;
        str.append("pen && brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachPen) {
        std::string str;
        str.append("pen");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDetachBrush(canvas);
    } else if (drawingType == DrawingTypeAttachBrush) {
        std::string str;
        str.append("brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_PenSetAlpha(pen, 10);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_BlendMode model = BLEND_MODE_CLEAR;
    OH_Drawing_PenSetBlendMode(pen, model);
    OH_Drawing_PenLineCapStyle lineCapStyle = LINE_FLAT_CAP;
    OH_Drawing_PenSetCap(pen, lineCapStyle);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xFFFF00FF, BLEND_MODE_DST_IN);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_PenLineJoinStyle lineJoinStyle = LINE_MITER_JOIN;
    OH_Drawing_PenSetJoin(pen, lineJoinStyle);
    OH_Drawing_PenSetMiterLimit(pen, 3.0);
    float intervals[] = {1.0, 2,0, 3.0, 4.0, 5.0};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 20, 5.0);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFFFFFF);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);

    OH_Drawing_BrushSetAlpha(brush, 255);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetBlendMode(brush, model);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    
    //画bitmap
   int32_t imageWidth = 1920 * 4;
   int32_t imageHeight = 1080 * 2;
   OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE }; // 100 用于宽高
    char* pixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4));
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        ((uint32_t *)pixels)[i] = 0xFF0000FF;
    }
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, imageWidth * 4);
    for (int i = 0; i < 10; i++) {
        OH_Drawing_CanvasDrawBitmap(canvas, bitmap, i, i);
    }
    OH_Drawing_FontDestroy(titleFont);
    OH_Drawing_BitmapDestroy(bitmap);
    free(pixels);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_FilterDestroy(filter);
}
void  CanvasDrawBitMapPerformance::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawBitMapOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawBitMapPerformancePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawBitMapOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}

void  CanvasDrawBitMapPerformanceBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
   CanvasDrawBitMapOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}
// 超大位图bitMapRect
void CanvasDrawBitMapRectOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    const int32_t itemWidth = 100;  // 256 绘图表面的宽度
    const int32_t itemHeight = 100; // 256 绘图表面的高度
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    OH_Drawing_BrushSetColor(brush, DRAW_COLORGREEN);
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12);
    static int func = 0;
    std::string funcTitles[] = {
        "OH_Drawing_CanvasDrawBitMapRect",
    };
    if (drawingType == DrawingTypeAttachBoth) {
        std::string str;
        str.append("pen && brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachPen) {
        std::string str;
        str.append("pen");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDetachBrush(canvas);
    } else if (drawingType == DrawingTypeAttachBrush) {
        std::string str;
        str.append("brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 300 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_PenSetAlpha(pen, 10);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_BlendMode model = BLEND_MODE_CLEAR;
    OH_Drawing_PenSetBlendMode(pen, model);
    OH_Drawing_PenLineCapStyle lineCapStyle = LINE_FLAT_CAP;
    OH_Drawing_PenSetCap(pen, lineCapStyle);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xFFFF00FF, BLEND_MODE_DST_IN);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_PenLineJoinStyle lineJoinStyle = LINE_MITER_JOIN;
    OH_Drawing_PenSetJoin(pen, lineJoinStyle);
    OH_Drawing_PenSetMiterLimit(pen, 3.0);
    float intervals[] = {1.0, 2,0, 3.0, 4.0, 5.0};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 20, 5.0);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFFFFFF);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);

    OH_Drawing_BrushSetAlpha(brush, 255);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetBlendMode(brush, model);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    
   int32_t imageWidth = 1920*4;
   int32_t imageHeight = 1080*2;
   OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE }; // 100 用于宽高
    char* pixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4));
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        ((uint32_t *)pixels)[i] = 0xFF0000FF;
    }
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, imageWidth *4);
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    OH_Drawing_Rect* bitMapSrc = OH_Drawing_RectCreate(0, 0, itemWidth, itemHeight);
    for (int i = 0; i < testCount; i++) {
        OH_Drawing_Rect* bitMapDst = OH_Drawing_RectCreate(itemWidth, 0, itemWidth * 2, itemHeight + i);
        OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, bitMapSrc, bitMapDst, samplingOptions);
        OH_Drawing_RectDestroy(bitMapDst);
    }
    OH_Drawing_RectDestroy(bitMapSrc);
    OH_Drawing_FontDestroy(titleFont);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_FilterDestroy(filter);
}
void  CanvasDrawBitMapRectPerformance::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawBitMapRectOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawBitMapRectPerformancePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawBitMapRectOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}

void  CanvasDrawBitMapRectPerformanceBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
   CanvasDrawBitMapRectOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}
// 超大图片绘制性能
void CanvasDrawImageOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    const int32_t itemWidth = 100;
    const int32_t itemHeight = 100;
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    OH_Drawing_BrushSetColor(brush, DRAW_COLORGREEN);
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12);
    static int func = 0;
    std::string funcTitles[] = {
        "OH_Drawing_CanvasDrawImageRectWithSrc OH_Drawing_CanvasDrawImageRect",
    };
    if (drawingType == DrawingTypeAttachBoth) {
        std::string str;
        str.append("pen && brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 0 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachPen) {
        std::string str;
        str.append("pen");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 0 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDetachBrush(canvas);
    } else if (drawingType == DrawingTypeAttachBrush) {
        std::string str;
        str.append("brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 0 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_PenSetAlpha(pen, 10);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_BlendMode model = BLEND_MODE_CLEAR;
    OH_Drawing_PenSetBlendMode(pen, model);
    OH_Drawing_PenLineCapStyle lineCapStyle = LINE_FLAT_CAP;
    OH_Drawing_PenSetCap(pen, lineCapStyle);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xFFFF00FF, BLEND_MODE_DST_IN);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_PenLineJoinStyle lineJoinStyle = LINE_MITER_JOIN;
    OH_Drawing_PenSetJoin(pen, lineJoinStyle);
    OH_Drawing_PenSetMiterLimit(pen, 3.0);
    float intervals[] = {1.0, 2,0, 3.0, 4.0, 5.0};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 20, 5.0);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFFFFFF);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);

    OH_Drawing_BrushSetAlpha(brush, 255);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetBlendMode(brush, model);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);

    int32_t imageWidth = 1920*4;
    int32_t imageHeight = 1080*2;
    OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE }; // 100 用于宽高
    char* pixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4));
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        ((uint32_t *)pixels)[i] = 0xFF0000FF;
    }
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, imageWidth *4);
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);

    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    OH_Drawing_Rect *imageRectSrc = OH_Drawing_RectCreate(0, 0, itemWidth, itemHeight);
    for (int i = 0; i < testCount; i++) {
        OH_Drawing_Rect *imageRectDst = OH_Drawing_RectCreate(itemWidth, itemHeight, itemWidth + 100, itemHeight + i);
        OH_Drawing_Rect *imageRect = OH_Drawing_RectCreate(itemWidth * 3, itemHeight , itemWidth * 3 + 100, itemHeight + i);
        OH_Drawing_CanvasDrawImageRectWithSrc(canvas, image, imageRectSrc, imageRectDst,
        samplingOptions, STRICT_SRC_RECT_CONSTRAINT);
        OH_Drawing_CanvasDrawImageRect(canvas, image, imageRect, samplingOptions);
        OH_Drawing_RectDestroy(imageRect);
        OH_Drawing_RectDestroy(imageRectDst);
    }
    OH_Drawing_FontDestroy(titleFont);
    OH_Drawing_RectDestroy(imageRectSrc);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_FilterDestroy(filter);
}
void  CanvasDrawImagePerformance::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawImageOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawImagePerformancePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawImageOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}

void  CanvasDrawImagePerformanceBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
   CanvasDrawImageOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}
// 超大源绘制性能
void CanvasDrawPiexlMapOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    const int32_t itemWidth = 100;
    const int32_t itemHeight = 100;
    
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
         OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    OH_Drawing_BrushSetColor(brush, DRAW_COLORGREEN);
    OH_Drawing_Font* titleFont = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(titleFont, 12);
    static int func = 0;
    std::string funcTitles[] = {
        "OH_Drawing_CanvasDrawPixelMapRect",
    };
    if (drawingType == DrawingTypeAttachBoth) {
        std::string str;
        str.append("pen && brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 0 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachPen) {
        std::string str;
        str.append("pen");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 0 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDetachBrush(canvas);
    } else if (drawingType == DrawingTypeAttachBrush) {
        std::string str;
        str.append("brush");
        str.append(funcTitles[func]);
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(str.c_str(), titleFont, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 0 , 600);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }
    OH_Drawing_PenSetAlpha(pen, 10);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_BlendMode model = BLEND_MODE_CLEAR;
    OH_Drawing_PenSetBlendMode(pen, model);
    OH_Drawing_PenLineCapStyle lineCapStyle = LINE_FLAT_CAP;
    OH_Drawing_PenSetCap(pen, lineCapStyle);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xFFFF00FF, BLEND_MODE_DST_IN);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_PenLineJoinStyle lineJoinStyle = LINE_MITER_JOIN;
    OH_Drawing_PenSetJoin(pen, lineJoinStyle);
    OH_Drawing_PenSetMiterLimit(pen, 3.0);
    float intervals[] = {1.0, 2,0, 3.0, 4.0, 5.0};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 20, 5.0);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFFFFFF);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);

    OH_Drawing_BrushSetAlpha(brush, 255);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetBlendMode(brush, model);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    //创建pixmap
    OH_Pixelmap_InitializationOptions *createOps = nullptr;
    auto ret = OH_PixelmapInitializationOptions_Create(&createOps);
    int32_t imageWidth = 100;
    int32_t imageHeight = 100;
    OH_PixelmapInitializationOptions_SetWidth(createOps, imageWidth);
    OH_PixelmapInitializationOptions_SetHeight(createOps, imageHeight);
    OH_PixelmapInitializationOptions_SetPixelFormat(createOps, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetSrcPixelFormat(createOps, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetAlphaType(createOps, 2); // 2 is ALPHA_FORMAT_PREMUL

    size_t bufferSize = imageWidth * imageHeight * 4;
    void* bitmapAddr = malloc(bufferSize);
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        ((uint32_t *)bitmapAddr)[i] = DRAW_COLORBLUE;
    }

    OH_PixelmapNative *pixelMapNative = nullptr;
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    ret = OH_PixelmapNative_CreatePixelmap((uint8_t *)bitmapAddr, bufferSize, createOps, &pixelMapNative);
    OH_Drawing_PixelMap* pixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMapNative);
    for (int i = 0; i < testCount; i++) {
         OH_Drawing_Rect* dst = OH_Drawing_RectCreate(itemWidth * 3, 0, itemWidth * 8, itemHeight + i);
         OH_Drawing_CanvasDrawPixelMapRect(canvas, pixelMap, nullptr, dst, samplingOptions);
         OH_Drawing_RectDestroy(dst);
    }
    OH_Drawing_FontDestroy(titleFont);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_PixelmapNative_Release(pixelMapNative);
    free(bitmapAddr);
    OH_PixelmapInitializationOptions_Release(createOps);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_FilterDestroy(filter);
}
void CanvasDrawPiexlMapPerformance::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawPiexlMapOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawPiexlMapPerformancePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawPiexlMapOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}

void CanvasDrawPiexlMapPerformanceBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
   CanvasDrawPiexlMapOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}