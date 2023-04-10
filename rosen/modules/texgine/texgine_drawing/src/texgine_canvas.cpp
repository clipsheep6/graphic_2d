/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "texgine_canvas.h"

namespace Texgine {
void TexgineCanvas::DrawLine(double x0, double y0, double x1, double y1, TexginePaint &paint)
{
    canvas_->drawLine(x0, y0, x1, y1, paint.GetPaint());
}

void TexgineCanvas::DrawRect(TexgineRect &rect, const TexginePaint &paint) const
{
    canvas_->drawRect(*rect.GetRect(), paint.GetPaint());
}

void TexgineCanvas::DrawTextBlob(std::shared_ptr<TexgineTextBlob> &blob, float x, float y, TexginePaint &paint)
{
    canvas_->drawTextBlob(blob->GetTextBlob(), x, y, paint.GetPaint());
}

void TexgineCanvas::Clear(uint32_t clolor)
{
    canvas_->clear(clolor);
}

int TexgineCanvas::Save()
{
    return canvas_->save();
}

void TexgineCanvas::Translate(float dx, float dy)
{
    canvas_->translate(dx, dy);
}
SkCanvas *TexgineCanvas::GetCanvas()
{
    return canvas_;
}

void TexgineCanvas::Restore()
{
    canvas_->restore();
}

void TexgineCanvas::SetCanvas(SkCanvas *canvas)
{
    canvas_ = canvas;
}
} // namespace Texgine
