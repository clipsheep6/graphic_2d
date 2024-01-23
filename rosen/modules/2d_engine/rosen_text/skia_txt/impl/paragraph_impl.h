/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_PARAGRAPH_IMPL_H
#define ROSEN_MODULES_SPTEXT_PARAGRAPH_IMPL_H

#include <optional>

#include "modules/skparagraph/include/Paragraph.h"
#include "txt/paint_record.h"
#include "txt/paragraph.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class ParagraphImpl : public Paragraph {
public:
    ParagraphImpl(std::unique_ptr<skia::textlayout::Paragraph> paragraph,
        std::vector<PaintRecord>&& paints);

    virtual ~ParagraphImpl() = default;

    double GetMaxWidth() override;

    double GetHeight() override;

    double GetLongestLine() override;

    double GetMinIntrinsicWidth() override;

    double GetMaxIntrinsicWidth() override;

    double GetAlphabeticBaseline() override;

    double GetIdeographicBaseline() override;

    bool DidExceedMaxLines() override;

    size_t GetLineCount() const override;

    void SetIndents(const std::vector<float>& indents) override;

    void Layout(double width) override;

    void Paint(SkCanvas* canvas, double x, double y) override;

    void Paint(Drawing::Canvas* canvas, double x, double y) override;

    std::vector<TextBox> GetRectsForRange(size_t start, size_t end,
        RectHeightStyle rectHeightStyle, RectWidthStyle rectWidthStyle) override;

    std::vector<TextBox> GetRectsForPlaceholders() override;

    PositionWithAffinity GetGlyphPositionAtCoordinate(double dx, double dy) override;

    Range<size_t> GetWordBoundary(size_t offset) override;

    Range<size_t> GetActualTextRange(int lineNumber, bool includeSpaces) override;

    std::vector<LineMetrics>& GetLineMetrics() override;

    bool GetLineMetricsAt(int lineNumber, skia::textlayout::LineMetrics* lineMetrics) const override;

private:
    TextStyle SkStyleToTextStyle(const skia::textlayout::TextStyle& skia);

    std::unique_ptr<skia::textlayout::Paragraph> paragraph_;
    std::vector<PaintRecord> paints_;
    std::optional<std::vector<LineMetrics>> lineMetrics_;
    std::vector<TextStyle> lineMetricsStyles_;
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_PARAGRAPH_IMPL_H
