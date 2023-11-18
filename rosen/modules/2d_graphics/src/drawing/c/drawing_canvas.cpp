/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

void OH_Drawing_CanvasClipRect(OH_Drawing_Canvas* cCanvas, float left, float top, float right, float bottom,
    OH_Drawing_CanvasClipOp cClipOp, bool doAntiAlias)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    Rect rect{left, top, right, bottom};
    canvas->ClipRect(rect, CClipOpCastToClipOp(cClipOp), doAntiAlias);
}

void OH_Drawing_CanvasTranslate(OH_Drawing_Canvas* cCanvas, float dx, float dy)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Translate(dx, dy);
}

void OH_Drawing_CanvasDrawScale(OH_Drawing_Canvas* cCanvas, float sx, float sy)
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
