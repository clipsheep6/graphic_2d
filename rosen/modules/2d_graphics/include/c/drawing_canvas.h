/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_DRAWING_H
#define C_INCLUDE_DRAWING_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides functions such as 2D graphics rendering, text drawing, and image display.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 8
 * @version 1.0
 */

/**
 * @file drawing_canvas.h
 *
 * @brief 文件中定义了与画布相关的功能函数。
 *
 * 引用文件"native_drawing/drawing_canvas.h"
 * @library libnative_drawing.so
 * @since 8
 * @version 1.0
 */

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 用于创建一个画布对象。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return 函数会返回一个指针，指针指向创建的画布对象。
 * @since 8
 * @version 1.0
 */
OH_Drawing_Canvas* OH_Drawing_CanvasCreate(void);

/**
 * @brief 用于销毁画布对象并回收该对象占有的内存。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasDestroy(OH_Drawing_Canvas*);

/**
 * @brief Binds a bitmap to a canvas so that the content drawn on the canvas
 * is output to the bitmap (this process is called CPU rendering).
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Bitmap 指向位图对象的指针。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasBind(OH_Drawing_Canvas*, OH_Drawing_Bitmap*);

/**
 * @brief 用于设置画笔给画布，画布将会使用设置画笔的样式和颜色去绘制图形形状的轮廓。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Pen 指向画笔对象的指针。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasAttachPen(OH_Drawing_Canvas*, const OH_Drawing_Pen*);

/**
 * @brief Detaches the pen from a canvas so that the canvas will not use the style
 * and color of the pen to outline a shape.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasDetachPen(OH_Drawing_Canvas*);

/**
 * @brief 用于设置画刷给画布，画布将会使用设置的画刷样式和颜色去填充绘制的图形形状。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Brush 指向画刷对象的指针。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasAttachBrush(OH_Drawing_Canvas*, const OH_Drawing_Brush*);

/**
 * @brief 用于去除掉画布中的画刷，使用后画布将不去填充图形形状。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasDetachBrush(OH_Drawing_Canvas*);

/**
 * @brief 用于保存当前画布的状态（画布矩阵）到一个栈顶。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasSave(OH_Drawing_Canvas*);

/**
 * @brief Saves matrix and clip, and allocates a bitmap for subsequent drawing.
 * Calling restore discards changes to matrix and clip, and draws the bitmap.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @param OH_Drawing_Rect Indicates the pointer to an <b>OH_Drawing_Rect</b> object.
 * @param OH_Drawing_Brush Indicates the pointer to an <b>OH_Drawing_Brush</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_CanvasSaveLayer(OH_Drawing_Canvas*, const OH_Drawing_Rect*, const OH_Drawing_Brush*);

/**
 * @brief Restores the canvas status (canvas matrix) saved on the top of the stack.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasRestore(OH_Drawing_Canvas*);

/**
 * @brief 用于获取栈中保存的画布状态（画布矩阵）的数量。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @return 函数会返回一个32位的值描述画布状态（画布矩阵）的数量。
 * @since 11
 * @version 1.0
 */
uint32_t OH_Drawing_CanvasGetSaveCount(OH_Drawing_Canvas*);

/**
 * @brief 用于恢复到指定数量的画布状态（画布矩阵）。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param saveCount 指定的画布状态（画布矩阵）数量。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasRestoreToCount(OH_Drawing_Canvas*, uint32_t saveCount);

/**
 * @brief 用于画一条直线段。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param x1 线段起始点的横坐标。
 * @param y1 线段起始点的纵坐标。
 * @param x2 线段结束点的横坐标。
 * @param y2 线段结束点的纵坐标。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasDrawLine(OH_Drawing_Canvas*, float x1, float y1, float x2, float y2);

/**
 * @brief 用于画一个自定义路径。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Path 指向路径对象的指针。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasDrawPath(OH_Drawing_Canvas*, const OH_Drawing_Path*);

/**
 * @brief 用于画一个位图，位图又称为点阵图像、像素图或栅格图像，是由像素（图片元素）的单个点组成。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Bitmap 指向位图对象的指针。
 * @param left 位图对象左上角的横坐标。
 * @param top 位图对象左上角的纵坐标。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasDrawBitmap(OH_Drawing_Canvas*, const OH_Drawing_Bitmap*, float left, float top);

/**
 * @brief 用于画一个矩形。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Rect 指向矩形对象的指针。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasDrawRect(OH_Drawing_Canvas*, const OH_Drawing_Rect*);

/**
 * @brief 用于画一个圆形。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Point 指向坐标点对象的指针，表示圆心。
 * @param radius 圆形的半径。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasDrawCircle(OH_Drawing_Canvas*, const OH_Drawing_Point*, float radius);

/**
 * @brief 用于画一个椭圆。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Rect 指向矩形对象的指针。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasDrawOval(OH_Drawing_Canvas*, const OH_Drawing_Rect*);

/**
 * @brief 用于画一个弧。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Rect 指向矩形对象的指针。
 * @param startAngle 弧的起始角度。
 * @param sweepAngle 弧的扫描角度。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasDrawArc(OH_Drawing_Canvas*, const OH_Drawing_Rect*, float startAngle, float sweepAngle);

/**
 * @brief 用于画一个圆角矩形。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_RoundRect 指向圆角矩形对象的指针。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasDrawRoundRect(OH_Drawing_Canvas*, const OH_Drawing_RoundRect*);

/**
 * @brief 用于画一段文字。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_TextBlob 指向文本对象的指针。
 * @param x 文本对象左下角的横坐标。
 * @param y 文本对象左下角的纵坐标。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasDrawTextBlob(OH_Drawing_Canvas*, const OH_Drawing_TextBlob*, float x, float y);

/**
 * @brief Enumerates clip op.
 *
 * @since 11
 * @version 1.0
 */
typedef enum {
    /**
     * 将指定区域裁剪（取差集）。
     */
    DIFFERENCE,
    /**
     * 将指定区域保留（取交集）。
     */
    INTERSECT,
} OH_Drawing_CanvasClipOp;

/**
 * @brief 用于裁剪一个矩形。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Rect 指向矩形对象的指针。
 * @param clipOp 裁剪方式。支持可选的具体裁剪方式可见@{link OH_Drawing_CanvasClipOp}枚举。
 * @param doAntiAlias 值为true则做抗锯齿处理，反之不做。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasClipRect(OH_Drawing_Canvas*, const OH_Drawing_Rect*,
    OH_Drawing_CanvasClipOp clipOp, bool doAntiAlias);

/**
 * @brief 用于裁剪一个自定义路径。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param OH_Drawing_Path 指向路径对象的指针。
 * @param clipOp 裁剪方式。支持可选的具体裁剪方式可见@{link OH_Drawing_CanvasClipOp}枚举。
 * @param doAntiAlias 真为抗锯齿，假则不做抗锯齿处理。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasClipPath(OH_Drawing_Canvas*, const OH_Drawing_Path*,
    OH_Drawing_CanvasClipOp clipOp, bool doAntiAlias);

/**
 * @brief 用于画布旋转一定的角度，正数表示顺时针旋转，负数反之。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param degrees 旋转角度。
 * @param px 旋转中心的横坐标。
 * @param py 旋转中心的纵坐标。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasRotate(OH_Drawing_Canvas*, float degrees, float px, float py);

/**
 * @brief 用于平移画布一段距离。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param dx 水平方向移动的距离。
 * @param dy 垂直方向移动的距离。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasTranslate(OH_Drawing_Canvas*, float dx, float dy);

/**
 * @brief 用于画布缩放。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param sx 水平方向缩放的比例。
 * @param sy 垂直方向缩放的比例。
 * @since 11
 * @version 1.0
 */
void OH_Drawing_CanvasScale(OH_Drawing_Canvas*, float sx, float sy);

/**
 * @brief 用于使用指定颜色去清空画布。
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas 指向画布对象的指针。
 * @param color 描述颜色的32位（ARGB）变量。
 * @since 8
 * @version 1.0
 */
void OH_Drawing_CanvasClear(OH_Drawing_Canvas*, uint32_t color);

/**
 * @brief Get the width of a canvas.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @since 12
 * @version 1.0
 */
int32_t OH_Drawing_CanvasGetWidth(OH_Drawing_Canvas*);

/**
 * @brief Get the height of a canvas.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @since 12
 * @version 1.0
 */
int32_t OH_Drawing_CanvasGetHeight(OH_Drawing_Canvas*);

/**
 * @brief Get the bounds of clip of a canvas.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @return An <b>OH_Drawing_Rect</b> object, represents the boundar of clip,
 * transformed by inverse of matrix.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Rect OH_Drawing_CanvasGetLocalClipBounds(OH_Drawing_Canvas*);

/**
 * @brief Get a 3x3 matrix of the transform from local coordinates to 'device'
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @param OH_Drawing_Matrix Indicates the pointer to an <b>OH_Drawing_Matrix</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_CanvasGetTotalMatrix(OH_Drawing_Canvas*, OH_Drawing_Matrix*);

/**
 * @brief Use the passed matrix to transforming the geometry, then use existing matrix.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @param OH_Drawing_Matrix Indicates the pointer to an <b>OH_Drawing_Matrix</b> object,
 * represents the matrix which is passed.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_CanvasConcatMatrix(OH_Drawing_Canvas*, OH_Drawing_Matrix*);

/**
 * @brief Enumerates of shadow flags.
 *
 * @since 12
 * @version 1.0
 */
typedef enum {
    /**
     * Use no shadow flags.
     */
    SHADOW_FLAGS_NONE,
    /**
     * The occluding object is transparent.
     */
    SHADOW_FLAGS_TRANSPARENT_OCCLUDER,
    /**
     * No need to analyze shadows.
     */
    SHADOW_FLAGS_GEOMETRIC_ONLY,
    /**
     * Use all shadow falgs.
     */
    SHADOW_FLAGS_ALL,
} OH_Drawing_CanvasShadowFlags;

/**
 * @brief Use circular light to draw an offset spot shadow and outlining ambient shadow for the given path.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @param OH_Drawing_Path Indicates the pointer to an <b>OH_Drawing_Path</b> object, use to generate shadows.
 * @param OH_Drawing_Point3 Indicates the pointer to an <b>OH_Drawing_Point3</b> object.
 * represents the value of the function which returns Z offset of the occluder from the canvas based on x and y.
 * @param OH_Drawing_Point3 Indicates the pointer to an <b>OH_Drawing_Point3</b> object.
 * represents the position of the light relative to the canvas.
 * @param lightRadius The radius of the circular light.
 * @param ambientColor Ambient shadow's color.
 * @param spotColor Spot shadow's color.
 * @param flag Indicates the flag to control opaque occluder, shadow, and light position.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_CanvasDrawShadow(OH_Drawing_Canvas*, OH_Drawing_Path*, OH_Drawing_Point3*,
    OH_Drawing_Point3*, float lightRadius, uint32_t ambientColor, uint32_t spotColor,
    OH_Drawing_CanvasShadowFlags flag);

/**
 * @brief Sets matrix of canvas.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @param OH_Drawing_Matrix Indicates the pointer to an <b>OH_Drawing_Matrix</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_CanvasSetMatrix(OH_Drawing_Canvas*, OH_Drawing_Matrix*);

/**
 * @brief Draws the specified source rectangle of the image onto the canvas,
 * scaled and translated to the destination rectangle.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @param OH_Drawing_Image Indicates the pointer to an <b>OH_Drawing_Image</b> object.
 * @param OH_Drawing_Rect Indicates the pointer to an <b>OH_Drawing_Rect</b> object.
 * @param OH_Drawing_SamplingOptions Indicates the pointer to an <b>OH_Drawing_SamplingOptions</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_CanvasDrawImageRect(OH_Drawing_Canvas*, OH_Drawing_Image*,
    OH_Drawing_Rect* dst, OH_Drawing_SamplingOptions*);

/**
 * @brief Read pixels data from canvas.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @param OH_Drawing_Image_Info width, height, colorType, and alphaType of dstPixels.
 * @param dstPixels destination pixel storage.
 * @param dstRowBytes size of one row of pixels.
 * @param srcX offset into canvas writable pixels on x-axis.
 * @param srcY offset into canvas writable pixels on y-axis.
 * @return true if pixels are copied to dstPixels.
 * @since 12
 * @version 1.0
 */
bool OH_Drawing_CanvasReadPixels(OH_Drawing_Canvas*, OH_Drawing_Image_Info*,
    void* dstPixels, uint32_t dstRowBytes, int32_t srcX, int32_t srcY);

/**
 * @brief Read pixels data to a bitmap from canvas.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @param OH_Drawing_Bitmap Indicates the pointer to an <b>OH_Drawing_Bitmap</b> object.
 * @param srcX offset into canvas writable pixels on x-axis.
 * @param srcY offset into canvas writable pixels on y-axis.
 * @return true if pixels are copied to dstBitmap.
 * @since 12
 * @version 1.0
 */
bool OH_Drawing_CanvasReadPixelsToBitmap(OH_Drawing_Canvas*, OH_Drawing_Bitmap*, int32_t srcX, int32_t srcY);

#ifdef __cplusplus
}
#endif
/** @} */
#endif