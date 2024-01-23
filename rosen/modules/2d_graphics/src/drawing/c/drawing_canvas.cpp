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

#include "c/drawing_canvas.h"

#include "draw/canvas.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Canvas* CastToCanvas(OH_Drawing_Canvas* cCanvas)
{
    return reinterpret_cast<Canvas*>(cCanvas);
}

static const Path& CastToPath(const OH_Drawing_Path& cPath)
{
    return reinterpret_cast<const Path&>(cPath);
}

static const Brush& CastToBrush(const OH_Drawing_Brush& cBrush)
{
    return reinterpret_cast<const Brush&>(cBrush);
}

static const Pen& CastToPen(const OH_Drawing_Pen& cPen)
{
    return reinterpret_cast<const Pen&>(cPen);
}

static const Bitmap& CastToBitmap(const OH_Drawing_Bitmap& cBitmap)
{
    return reinterpret_cast<const Bitmap&>(cBitmap);
}

static const Point& CastToPoint(const OH_Drawing_Point& cPoint)
{
    return reinterpret_cast<const Point&>(cPoint);
}

static const RoundRect& CastToRoundRect(const OH_Drawing_RoundRect& cRoundRect)
{
    return reinterpret_cast<const RoundRect&>(cRoundRect);
}

static const TextBlob* CastToTextBlob(const OH_Drawing_TextBlob* cTextBlob)
{
    return reinterpret_cast<const TextBlob*>(cTextBlob);
}

OH_Drawing_Canvas* OH_Drawing_CanvasCreate()
{
    return (OH_Drawing_Canvas*)new Canvas;
}

void OH_Drawing_CanvasDestroy(OH_Drawing_Canvas* cCanvas)
{
    delete CastToCanvas(cCanvas);
}

void OH_Drawing_CanvasBind(OH_Drawing_Canvas* cCanvas, OH_Drawing_Bitmap* cBitmap)
{
    if (cBitmap == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Bind(CastToBitmap(*cBitmap));
}

void OH_Drawing_CanvasAttachPen(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Pen* cPen)
{
    if (cPen == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->AttachPen(CastToPen(*cPen));
}

void OH_Drawing_CanvasDetachPen(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DetachPen();
}

void OH_Drawing_CanvasAttachBrush(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->AttachBrush(CastToBrush(*cBrush));
}

void OH_Drawing_CanvasDetachBrush(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DetachBrush();
}

void OH_Drawing_CanvasSave(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Save();
}

void OH_Drawing_CanvasRestore(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Restore();
}

uint32_t OH_Drawing_CanvasGetSaveCount(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return 0;
    }
    return canvas->GetSaveCount();
}

void OH_Drawing_CanvasRestoreToCount(OH_Drawing_Canvas* cCanvas, uint32_t saveCount)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->RestoreToCount(saveCount);
}

void OH_Drawing_CanvasDrawLine(OH_Drawing_Canvas* cCanvas, float x1, float y1, float x2, float y2)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    Point startPt(x1, y1);
    Point endPt(x2, y2);
    canvas->DrawLine(startPt, endPt);
}

void OH_Drawing_CanvasDrawPath(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Path* cPath)
{
    if (cPath == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawPath(CastToPath(*cPath));
}

void OH_Drawing_CanvasDrawBitmap(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Bitmap* cBitmap, float left, float top)
{
    if (cBitmap == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawBitmap(CastToBitmap(*cBitmap), left, top);
}

void OH_Drawing_CanvasDrawRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect)
{
    if (cRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawRect(Rect(cRect->left, cRect->top, cRect->right, cRect->bottom));
}

void OH_Drawing_CanvasDrawCircle(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Point* cPoint, float radius)
{
    if (cPoint == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawCircle(CastToPoint(*cPoint), radius);
}

void OH_Drawing_CanvasDrawOval(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect)
{
    if (cRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawOval(Rect(cRect->left, cRect->top, cRect->right, cRect->bottom));
}

void OH_Drawing_CanvasDrawArc(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect,
    float startAngle, float sweepAngle)
{
    if (cRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawArc(Rect(cRect->left, cRect->top, cRect->right, cRect->bottom), startAngle, sweepAngle);
}

void OH_Drawing_CanvasDrawRoundRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_RoundRect* cRoundRect)
{
    if (cRoundRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawRoundRect(CastToRoundRect(*cRoundRect));
}

void OH_Drawing_CanvasDrawTextBlob(OH_Drawing_Canvas* cCanvas, const OH_Drawing_TextBlob* cTextBlob, float x, float y)
{
    if (cTextBlob == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawTextBlob(CastToTextBlob(cTextBlob), x, y);
}

static ClipOp CClipOpCastToClipOp(OH_Drawing_CanvasClipOp cClipOp)
{
    ClipOp clipOp = ClipOp::INTERSECT;
    switch (cClipOp) {
        case DIFFERENCE:
            clipOp = ClipOp::DIFFERENCE;
            break;
        case INTERSECT:
            clipOp = ClipOp::INTERSECT;
            break;
        default:
            break;
    }
    return clipOp;
}

void OH_Drawing_CanvasClipRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect,
    OH_Drawing_CanvasClipOp cClipOp, bool doAntiAlias)
{
    if (cRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->ClipRect(Rect(cRect->left, cRect->top, cRect->right, cRect->bottom),
        CClipOpCastToClipOp(cClipOp), doAntiAlias);
}

void OH_Drawing_CanvasClipPath(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Path* cPath,
    OH_Drawing_CanvasClipOp cClipOp, bool doAntiAlias)
{
    if (cPath == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->ClipPath(CastToPath(*cPath), CClipOpCastToClipOp(cClipOp), doAntiAlias);
}

void OH_Drawing_CanvasRotate(OH_Drawing_Canvas* cCanvas, float degrees, float px, float py)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Rotate(degrees, px, py);
}

void OH_Drawing_CanvasTranslate(OH_Drawing_Canvas* cCanvas, float dx, float dy)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Translate(dx, dy);
}

void OH_Drawing_CanvasScale(OH_Drawing_Canvas* cCanvas, float sx, float sy)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Scale(sx, sy);
}

void OH_Drawing_CanvasClear(OH_Drawing_Canvas* cCanvas, uint32_t color)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Clear(color);
}

int32_t OH_Drawing_CanvasGetWidth(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return 0;
    }
    return canvas->GetWidth();
}

int32_t OH_Drawing_CanvasGetHeight(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return 0;
    }
    return canvas->GetHeight();
}

OH_Drawing_Rect OH_Drawing_CanvasGetLocalClipBounds(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        OH_Drawing_Rect dRect;
        return dRect;
    }
    Rect rect = canvas->GetLocalClipBounds();
    OH_Drawing_Rect cRect = { rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom()};
    return cRect;
}

void OH_Drawing_CanvasGetTotalMatrix(OH_Drawing_Canvas* cCanvas, OH_Drawing_Matrix* cMatrix)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    Matrix matrix = canvas->GetTotalMatrix();
    cMatrix = (OH_Drawing_Matrix*)new Matrix(matrix);
}

void OH_Drawing_CanvasConcatMatrix(OH_Drawing_Canvas* cCanvas, OH_Drawing_Matrix* cMatrix)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->ConcatMatrix(*reinterpret_cast<Matrix*>(cMatrix));
}

static ShadowFlags CClipOpCastToClipOp(OH_Drawing_CanvasShadowFlags cFlag)
{
    ShadowFlags shadowFlags = ShadowFlags::NONE;
    switch (cFlag) {
        case OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_NONE:
            shadowFlags = ShadowFlags::NONE;
            break;
        case OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_TRANSPARENT_OCCLUDER:
            shadowFlags = ShadowFlags::TRANSPARENT_OCCLUDER;
            break;
        case OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_GEOMETRIC_ONLY:
            shadowFlags = ShadowFlags::GEOMETRIC_ONLY;
            break;
        case OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_ALL:
            shadowFlags = ShadowFlags::ALL;
            break;
        default:
            break;
    }
    return shadowFlags;
}

void OH_Drawing_CanvasDrawShadow(OH_Drawing_Canvas* cCanvas, OH_Drawing_Path* cPath, OH_Drawing_Point3* planeParams,
    OH_Drawing_Point3* devLightPos, float lightRadius, uint32_t ambientColor, uint32_t spotColor,
    OH_Drawing_CanvasShadowFlags flag)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawShadow(*reinterpret_cast<Path*>(cPath), *reinterpret_cast<Point3*>(planeParams),
        *reinterpret_cast<Point3*>(devLightPos), lightRadius, Color(ambientColor), Color(spotColor),
        CClipOpCastToClipOp(flag));
}