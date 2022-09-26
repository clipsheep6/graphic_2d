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

#include "typography.h"

#include "engine_adapter/skia_adapter/skia_canvas.h"
#include "txt/paragraph_txt.h"

#include "convert.h"

namespace OHOS {
namespace Rosen {
TextBox::TextBox(Drawing::RectF rect, TextDirection direction)
{
    rect_ = rect;
    direction_ = direction;
}

PositionAndAffinity::PositionAndAffinity(size_t pos, Affinity affinity)
{
    pos_ = pos;
    affinity_ = affinity;
}

Range::Range(size_t start, size_t end)
{
    start_ = start;
    end_ = end;
}

bool Range::operator ==(const Range &rhs) const
{
    return start_ == rhs.start_ && end_ == rhs.end_;
}

namespace AdapterTxt {
Typography::Typography(std::unique_ptr<txt::Paragraph> paragraph)
{
    paragraph_ = std::move(paragraph);
}

double Typography::GetMaxWidth() const
{
    return paragraph_->GetMaxWidth();
}

double Typography::GetHeight() const
{
    return paragraph_->GetHeight();
}

double Typography::GetActualWidth() const
{
    return paragraph_->GetLongestLine();
}

double Typography::GetMinIntrinsicWidth()
{
    return paragraph_->GetMinIntrinsicWidth();
}

double Typography::GetMaxIntrinsicWidth()
{
    return paragraph_->GetMaxIntrinsicWidth();
}

double Typography::GetAlphabeticBaseline()
{
    return paragraph_->GetAlphabeticBaseline();
}

double Typography::GetIdeographicBaseline()
{
    return paragraph_->GetIdeographicBaseline();
}

bool Typography::DidExceedMaxLines() const
{
    return paragraph_->DidExceedMaxLines();
}

int Typography::GetLineCount() const
{
    auto paragraphTxt = reinterpret_cast<txt::ParagraphTxt *>(paragraph_.get());
    if (paragraphTxt) {
        return paragraphTxt->GetLineCount();
    }
    return 0;
}

void Typography::SetIndents(const std::vector<float> &indents)
{
    auto paragraphTxt = reinterpret_cast<txt::ParagraphTxt *>(paragraph_.get());
    if (paragraphTxt) {
        paragraphTxt->SetIndents(indents);
    }
}

void Typography::Layout(double width)
{
    return paragraph_->Layout(width);
}

void Typography::Paint(SkCanvas *canvas, double x, double y)
{
    return paragraph_->Paint(canvas, x, y);
}

void Typography::Paint(Drawing::Canvas *drawCanvas, double x, double y)
{
    std::shared_ptr<Drawing::CoreCanvasImpl> coreCanvas = drawCanvas->GetCanvasData();
    auto skiacavas = static_cast<Drawing::SkiaCanvas *>(coreCanvas.get());
    auto canvas = skiacavas->ExportSkCanvas();
    paragraph_->Paint(canvas, x, y);
}

std::vector<TextBox> Typography::GetRectsForRange(size_t start, size_t end,
                                                  RectHeightStyle heightStyle,
                                                  RectWidthStyle widthStyle)
{
    auto txtRectHeightStyle = Convert(heightStyle);
    auto txtRectWidthStyle = Convert(widthStyle);
    auto rects = paragraph_->GetRectsForRange(start, end, txtRectHeightStyle, txtRectWidthStyle);

    std::vector<TextBox> boxes;
    for (const auto &rect : rects) {
        boxes.push_back(Convert(rect));
    }
    return boxes;
}

std::vector<TextBox> Typography::GetRectsForPlaceholders()
{
    auto rects = paragraph_->GetRectsForPlaceholders();

    std::vector<TextBox> boxes;
    for (const auto &rect : rects) {
        boxes.push_back(Convert(rect));
    }
    return boxes;
}

PositionAndAffinity Typography::GetGlyphPositionAtCoordinate(double dx, double dy)
{
    auto pos = paragraph_->GetGlyphPositionAtCoordinate(dx, dy);
    return Convert(pos);
}

Range Typography::GetWordBoundary(size_t offset)
{
    auto range = paragraph_->GetWordBoundary(offset);
    return Convert(range);
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
