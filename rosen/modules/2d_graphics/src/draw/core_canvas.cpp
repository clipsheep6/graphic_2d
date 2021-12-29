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

#include <iostream>
#include <string>

#include "core_canvas.h"
#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
CoreCanvas::CoreCanvas() : impl_(CoreCanvasImplFactory::CreateCoreCanvasImpl()) {}

void CoreCanvas::Bind(const Bitmap& bitmap)
{
    impl_->Bind(bitmap.bmpImplPtr);
}

void CoreCanvas::DrawPoint(const Point& point)
{
    impl_->DrawPoint(point);
}

void CoreCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    impl_->DrawLine(startPt, endPt);
}

void CoreCanvas::DrawRect(const Rect& rect)
{
    impl_->DrawRect(rect);
}

void CoreCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    impl_->DrawRoundRect(roundRect);
}

void CoreCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    impl_->DrawNestedRoundRect(outer, inner);
}

void CoreCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    impl_->DrawArc(oval, startAngle, sweepAngle);
}

void CoreCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    impl_->DrawPie(oval, startAngle, sweepAngle);
}

void CoreCanvas::DrawOval(const Rect& oval)
{
    impl_->DrawOval(oval);
}

void CoreCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    impl_->DrawCircle(centerPt, radius);
}

void CoreCanvas::DrawPath(const Path& path)
{
    impl_->DrawPath(path);
}

void CoreCanvas::DrawBackground()
{
    impl_->DrawBackground();
}

void CoreCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
                scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    impl_->DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
}

void CoreCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    impl_->DrawBitmap(bitmap.bmpImplPtr, px, py);
}

void CoreCanvas::DrawImage()
{
    impl_->DrawImage();
}

void CoreCanvas::DrawText(const Text& text)
{
    impl_->DrawText(text);
}

void CoreCanvas::ClipRect(const Rect& rect, ClipOp op)
{
    impl_->ClipRect(rect, op);
}

void CoreCanvas::ClipRoundRect(const RoundRect& roundRect, ClipOp op)
{
    impl_->ClipRoundRect(roundRect, op);
}

void CoreCanvas::ClipPath(const Path& path, ClipOp op)
{
    impl_->ClipPath(path, op);
}

void CoreCanvas::SetMatrix(const Matrix& matrix)
{
    impl_->SetMatrix(matrix);
}

void CoreCanvas::ResetMatrix()
{
    impl_->ResetMatrix();
}

void CoreCanvas::ConcatMatrix(const Matrix& matrix)
{
    impl_->ConcatMatrix(matrix);
}

void CoreCanvas::Translate(scalar dx, scalar dy)
{
    impl_->Translate(dx, dy);
}

void CoreCanvas::Scale(scalar sx, scalar sy)
{
    impl_->Scale(sx, sy);
}

void CoreCanvas::Rotate(scalar deg)
{
    impl_->Rotate(deg);
}

void CoreCanvas::Shear(scalar sx, scalar sy)
{
    impl_->Shear(sx, sy);
}

void CoreCanvas::Flush()
{
    impl_->Flush();
}

void CoreCanvas::Clear(ColorQuad color)
{
    impl_->Clear(color);
}

void CoreCanvas::Save()
{
    impl_->Save();
}

void CoreCanvas::SaveLayer(const Rect& rect)
{
    impl_->SaveLayer(rect);
}

void CoreCanvas::Restore()
{
    impl_->Restore();
}

void CoreCanvas::AttachPen(const Pen& pen)
{
    impl_->AttachPen(pen);
}

void CoreCanvas::AttachBrush(const Brush& brush)
{
    impl_->AttachBrush(brush);
}

void CoreCanvas::DetachPen()
{
    impl_->DetachPen();
}

void CoreCanvas::DetachBrush()
{
    impl_->DetachBrush();
}
}
}
