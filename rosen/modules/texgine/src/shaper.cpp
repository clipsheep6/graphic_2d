/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "shaper.h"

#include <queue>
#include <variant>

#include "bidi_processer.h"
#include "line_breaker.h"
#include "measurer.h"
#include "texgine/any_span.h"
#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif
#include "text_breaker.h"
#include "text_merger.h"
#include "text_reverser.h"
#include "text_shaper.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define MAXWIDTH 1e9
namespace {
void DumpLineMetrics(const std::vector<LineMetrics> &lineMetrics)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "DumpLineMetrics");
    for (const auto &metric : lineMetrics) {
        for (const auto &span : metric.lineSpans) {
            span.Dump();
        }
    }
}
} // namespace

bool Shaper::DidExceedMaxLines() const
{
    return didExceedMaxLines_;
}

double Shaper::GetMinIntrinsicWidth() const
{
    return minIntrinsicWidth_;
}

double Shaper::GetMaxIntrinsicWidth() const
{
    return maxIntrinsicWidth_;
}

std::vector<LineMetrics> Shaper::CreateEllipsisSpan(const TypographyStyle &ys,
    const std::shared_ptr<FontProviders> &fontProviders)
{
    if (ys.ellipsis.empty()) {
        return {};
    }

    TextStyle xs;
    xs.fontSize = ys.lineStyle.fontSize;
    xs.fontFamilies = ys.lineStyle.fontFamilies;

    std::vector<VariantSpan> spans = {TextSpan::MakeFromText(ys.ellipsis)};
    spans[0].SetTextStyle(xs);
    auto ys2 = ys;
    ys2.wordBreakType = WordBreakType::BREAK_ALL;
    ys2.breakStrategy = BreakStrategy::GREEDY;
    return DoShapeBeforeEllipsis(spans, ys2, fontProviders, MAXWIDTH);
}

void Shaper::ConsiderEllipsis(const TypographyStyle &tstyle,
    const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit)
{
    didExceedMaxLines_ = false;
    auto maxLines = tstyle.maxLines;
    if (maxLines < 0) {
        maxLines = 1;
    }

    if (lineMetrics_.size() <= maxLines) {
        return;
    }

    std::vector<LineMetrics> ellipsisMertics = CreateEllipsisSpan(tstyle, fontProviders);
    double ellipsisWidth = 0.0;
    std::vector<VariantSpan> ellipsisSpans;
    for (auto &metric : ellipsisMertics) {
        for (auto &es : metric.lineSpans) {
            ellipsisWidth += es.GetWidth();
            ellipsisSpans.push_back(es);
        }
    }

    switch (tstyle.ellipsisModal) {
        case EllipsisModal::HEAD: {
            if (maxLines == 1) { // single line
                ConsiderOneHeadEllipsis(ellipsisMertics, ellipsisSpans, tstyle, fontProviders);
            }
            break;
        }
        case EllipsisModal::MIDDLE: {
            if (maxLines == 1) { // single line
                ConsiderMidSpanEllipsis(ellipsisMertics, ellipsisSpans, tstyle, fontProviders);
            }
            break;
        }
        case EllipsisModal::TAIL:
        default:
            ConsiderTailEllipsis(ellipsisSpans, ellipsisWidth, maxLines, widthLimit);
            break;
    }
    didExceedMaxLines_ = true;
}

void Shaper::ComputeIntrinsicWidth(const size_t maxLines)
{
    maxIntrinsicWidth_ = 0.0;
    minIntrinsicWidth_ = 0.0;
    double lastInvisibleWidth = 0;
    for (const auto &line : lineMetrics_) {
        for (const auto &span : line.lineSpans) {
            if (span == nullptr) {
                continue;
            }

            auto width = span.GetWidth();
            auto visibleWidth = span.GetVisibleWidth();
            maxIntrinsicWidth_ += width;
            minIntrinsicWidth_ = std::max(visibleWidth, minIntrinsicWidth_);
            lastInvisibleWidth = width - visibleWidth;
        }
    }

    maxIntrinsicWidth_ -= lastInvisibleWidth;
    if (maxLines > 1) {
        minIntrinsicWidth_ = std::min(maxIntrinsicWidth_, minIntrinsicWidth_);
    } else {
        minIntrinsicWidth_ = maxIntrinsicWidth_;
    }
}

std::vector<LineMetrics> Shaper::DoShapeBeforeEllipsis(std::vector<VariantSpan> spans, const TypographyStyle &tstyle,
        const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit)
{
    TextBreaker tb;
    tb.SetWidthLimit(widthLimit);
    auto ret = tb.WordBreak(spans, tstyle, fontProviders);
    if (ret) {
        LOGEX_FUNC_LINE(ERROR) << "word break failed";
        return {};
    }

    BidiProcesser bp;
    auto newSpans = bp.ProcessBidiText(spans, tstyle.direction);
    if (newSpans.empty()) {
        LOGEX_FUNC_LINE(ERROR) << "Process BidiText failed";
        return {};
    }

    LineBreaker lb;
    return lb.BreakLines(newSpans, tstyle, widthLimit);
}

std::vector<LineMetrics> Shaper::DoShape(std::vector<VariantSpan> spans, const TypographyStyle &tstyle,
    const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("Shaper::DoShape");
#endif

    widthLimit_ = widthLimit;
    lineMetrics_ = DoShapeBeforeEllipsis(spans, tstyle, fontProviders, widthLimit_);
    ComputeIntrinsicWidth(tstyle.maxLines);
    ConsiderEllipsis(tstyle, fontProviders, widthLimit_);

    TextMerger tm;
    for (auto &metric : lineMetrics_) {
        auto res = tm.MergeSpans(metric.lineSpans);
        std::swap(res, metric.lineSpans);
    }

    TextReverser tr;
    for (auto &metric : lineMetrics_) {
        tr.ReverseRTLText(metric.lineSpans);
        tr.ProcessTypoDirection(metric.lineSpans, tstyle.direction);
    }

    TextShaper textShaper;
    for (const auto &metric : lineMetrics_) {
        for (const auto &span : metric.lineSpans) {
            textShaper.Shape(span, tstyle, fontProviders);
        }
    }
    DumpLineMetrics(lineMetrics_);
    return lineMetrics_;
}

void Shaper::ConsiderTailEllipsis(const std::vector<VariantSpan> &ellipsisSpans, const double ellipsisWidth,
    const size_t maxLines, const double widthLimit)
{
    bool isErase = false;
    bool isEexceedWidthLimit = false;
    if (maxLines < lineMetrics_.size()) {
        lineMetrics_.erase(lineMetrics_.begin() + maxLines, lineMetrics_.end());
        isErase = true;
    }
    double width = 0;
    auto &lastline = lineMetrics_[maxLines - 1];
    if (!isErase && lastline.width <= widthLimit) {
        return;
    }

    for (const auto &span : lastline.lineSpans) {
        width += span.GetWidth();
    }

    // protected the first span and ellipsis
    while (static_cast<int>(width) > static_cast<int>(widthLimit - ellipsisWidth) &&
        static_cast<int>(lastline.lineSpans.size()) > 1) {
        width -= lastline.lineSpans.back().GetWidth();
        lastline.lineSpans.pop_back();
        isEexceedWidthLimit = true;
    }

    auto ts = lastline.lineSpans.back().TryToTextSpan();
    if (ts == nullptr && (isEexceedWidthLimit || isErase)) {
        lastline.lineSpans.insert(lastline.lineSpans.end(), ellipsisSpans.begin(), ellipsisSpans.end());
        return;
    }

    bool hasEllipsis = ts && (static_cast<int>(ts->GetWidth()) > static_cast<int>(widthLimit - ellipsisWidth) ||
        isErase);
    if (!hasEllipsis && ts != nullptr) {
        return;
    }

    if (hasEllipsis) {
        ProcessEllipsis(static_cast<int>(widthLimit - ellipsisWidth), ts->cgs_, lastline);
        // Add ellipsisSpans
        lastline.lineSpans.insert(lastline.lineSpans.end(), ellipsisSpans.begin(), ellipsisSpans.end());
    }
}

void Shaper::ProcessEllipsis(int avalibleWidth, CharGroups &cgs, LineMetrics &lastLine)
{
    double cgsWidth = 0.0;
    CharGroups newCgs = CharGroups::CreateEmpty();
    for (auto &cg : cgs) {
        cgsWidth += cg.GetWidth();
        if (static_cast<int>(cgsWidth) < avalibleWidth) {
            newCgs.PushBack(cg);
        } else {
            break;
        }
    }

    // If there is only one span in the last line, protect the first chargroup
    if (newCgs.GetSize() == 0 && lastLine.lineSpans.size() == 1) {
        newCgs.PushBack(cgs.Get(0));
    }

    if (newCgs.GetSize() != 0) {
        VariantSpan span = TextSpan::MakeFromCharGroups(newCgs);
        auto style = lastLine.lineSpans.back().GetTextStyle();
        span.SetTextStyle(style);
        lastLine.lineSpans.pop_back();
        lastLine.lineSpans.push_back(span);
        auto nt = span.TryToTextSpan();
    } else {
        lastLine.lineSpans.pop_back();
    }
}

double Shaper::GetSpansWidth(const std::vector<VariantSpan> &spans) const
{
    double spansWidth = 0.0;
    for (auto &span : spans) {
        spansWidth += span.GetWidth();
    }
    return spansWidth;
}

bool Shaper::GetAllSpan(const std::vector<VariantSpan> &ellipsisSpans, std::vector<VariantSpan> &spans,
    const std::vector<LineMetrics> &ellipsisMertics)
{
    for (auto metric : lineMetrics_) {
        for (auto span : metric.lineSpans) {
            spans.push_back(span);
        }
    }
    double ellipsisWidth = GetSpansWidth(ellipsisSpans);
    avalibleWidth_ = static_cast<int>(widthLimit_ - ellipsisWidth);

    if (static_cast<int>(GetSpansWidth(spans)) <= avalibleWidth_) {
        return false;
    }

    if (avalibleWidth_ <= 0) {
        lineMetrics_ = ellipsisMertics;
        return false;
    }
    return true;
}

void Shaper::SplitJointLeftSpans(const std::vector<VariantSpan> &ellipsisSpans, const size_t leftIndex,
    const TypographyStyle &style, const std::shared_ptr<FontProviders> &fontProviders)
{
    if (textSpan_ == nullptr) {
        return;
    }

    std::vector<uint16_t> leftGroups = textSpan_->cgs_.GetCharsToU16(0, leftIndex, true);
    std::vector<VariantSpan> leftVariantSpans = {TextSpan::MakeFromText(leftGroups)};
    lineMetrics_ = DoShapeBeforeEllipsis(leftVariantSpans, style, fontProviders, widthLimit_);
    lineMetrics_.back().lineSpans.insert(lineMetrics_.back().lineSpans.end(),
        ellipsisSpans.begin(), ellipsisSpans.end());
}

void Shaper::SplitJointRightSpans(const std::vector<VariantSpan> &ellipsisSpans, const size_t rightIndex,
    const TypographyStyle &style, const std::shared_ptr<FontProviders> &fontProviders,
    const EllipsisModal ellipsisModal)
{
    if (textSpan_ == nullptr) {
        return;
    }
    size_t maxIndex = textSpan_->cgs_.GetSize();
    // maxIndex - 1 means the last index of the array
    std::vector<uint16_t> rightGroups = textSpan_->cgs_.GetCharsToU16(rightIndex, maxIndex - 1, false);
    std::vector<VariantSpan> rightVariantSpans = {TextSpan::MakeFromText(rightGroups)};
    std::vector<LineMetrics> rightLineMetrics = DoShapeBeforeEllipsis(rightVariantSpans,
        style, fontProviders, widthLimit_);

    if (ellipsisModal == EllipsisModal::HEAD) {
        lineMetrics_ = DoShapeBeforeEllipsis(rightVariantSpans, style, fontProviders, widthLimit_);
        lineMetrics_.front().lineSpans.insert(lineMetrics_.front().lineSpans.begin(),
            ellipsisSpans.begin(), ellipsisSpans.end());
        return;
    }

    lineMetrics_.back().lineSpans.insert(lineMetrics_.back().lineSpans.end(),
        rightLineMetrics.front().lineSpans.begin(), rightLineMetrics.front().lineSpans.end());
}

void Shaper::ConsiderOneHeadEllipsis(const std::vector<LineMetrics> &ellipsisMertics,
    const std::vector<VariantSpan> &ellipsisSpans, const TypographyStyle &style,
    const std::shared_ptr<FontProviders> &fontProviders)
{
    avalibleWidth_ = 0;
    std::vector<VariantSpan> spans;
    bool needProcess = GetAllSpan(ellipsisSpans, spans, ellipsisMertics);
    if (!needProcess) {
        return;
    }
    auto &filstSpan = spans.front();
    textSpan_ = filstSpan.TryToTextSpan();
    if (textSpan_ == nullptr) {
        return;
    }
    size_t rightIndex = textSpan_->cgs_.GetSize();
    size_t maxIndex = textSpan_->cgs_.GetSize();
    double rightCharsWidth = 0.0;
    while (static_cast<int>(rightCharsWidth) <= avalibleWidth_) {
        if (rightIndex == 0) {
            break;
        }
        rightIndex--;
        rightCharsWidth += textSpan_->cgs_.GetCharWidth(rightIndex);
    }
    rightIndex++;

    if (rightIndex < maxIndex) {
        SplitJointRightSpans(ellipsisSpans, rightIndex, style, fontProviders, EllipsisModal::HEAD);
    } else {
        lineMetrics_ = ellipsisMertics;
    }
}

bool Shaper::CalcCharsIndex(size_t &leftIndex, size_t &rightIndex) const
{
    if (textSpan_ == nullptr) {
        return false;
    }

    double leftCharsWidth = 0.0;
    double rightCharsWidth = 0.0;
    bool isLeft = true;
    bool isNormal = true;
    int charsWidth = static_cast<int>(leftCharsWidth + rightCharsWidth);
    while (charsWidth <= avalibleWidth_) {
        if (isLeft) {
            leftCharsWidth += textSpan_->cgs_.GetCharWidth(leftIndex);
            charsWidth = static_cast<int>(leftCharsWidth + rightCharsWidth);
            if (charsWidth > avalibleWidth_) {
                leftIndex--;
                break;
            }
            isLeft = false;
        } else {
            rightIndex--;
            rightCharsWidth += textSpan_->cgs_.GetCharWidth(rightIndex);
            charsWidth = static_cast<int>(leftCharsWidth + rightCharsWidth);
            if (charsWidth > avalibleWidth_) {
                rightIndex++;
                break;
            }
            leftIndex++;
            isLeft = true;
        }
        if (leftIndex > rightIndex) {
            isNormal = false;
            break;
        }
    }
    return isNormal;
}

void Shaper::ConsiderOneMidEllipsis(const std::vector<LineMetrics> &ellipsisMertics,
    const std::vector<VariantSpan> &ellipsisSpans, const TypographyStyle &style,
    const std::shared_ptr<FontProviders> &fontProviders)
{
    if (textSpan_ == nullptr) {
        return;
    }
    size_t leftIndex = 0;
    size_t maxIndex = textSpan_->cgs_.GetSize();
    size_t rightIndex = maxIndex;

    if (!CalcCharsIndex(leftIndex, rightIndex)) {
        return;
    }

    if (leftIndex < maxIndex) {
        SplitJointLeftSpans(ellipsisSpans, leftIndex, style, fontProviders);
    } else {
        lineMetrics_ = ellipsisMertics;
        textSpan_ = nullptr;
        avalibleWidth_ = 0;
        return;
    }

    if (rightIndex < maxIndex) {
        SplitJointRightSpans(ellipsisSpans, rightIndex, style, fontProviders);
    }
    textSpan_ = nullptr;
    avalibleWidth_ = 0;
}

bool Shaper::CalcMidSpanIndex(const std::vector<VariantSpan> &spans, size_t &leftIndex, size_t &rightIndex,
    struct SpansWidth &spansWidth)
{
    double leftSpansWidth = 0.0;
    double rightSpansWidth = 0.0;
    bool isLeft = true;
    bool isNormal = true;
    int curSpansWidth = static_cast<int>(leftSpansWidth + rightSpansWidth);
    while (curSpansWidth <= avalibleWidth_) {
        if (isLeft) {
            leftSpansWidth += spans.at(leftIndex).GetWidth();
            curSpansWidth = static_cast<int>(leftSpansWidth + rightSpansWidth);
            if (curSpansWidth > avalibleWidth_) {
                break;
            }
            isLeft = false;
        } else {
            rightIndex--;
            rightSpansWidth += spans.at(rightIndex).GetWidth();
            curSpansWidth = static_cast<int>(leftSpansWidth + rightSpansWidth);
            if (curSpansWidth > avalibleWidth_) {
                break;
            }
            leftIndex++;
            isLeft = true;
        }
        if (leftIndex >= rightIndex) {
            isNormal = false;
            break;
        }
    }
    spansWidth.leftWidth = leftSpansWidth;
    spansWidth.rightWidth = rightSpansWidth;
    return isNormal;
}

void Shaper::CalcLeftIndex(const std::vector<VariantSpan> &spans, const size_t maxIndex,
    size_t &leftIndex, size_t &rightIndex, struct SpansWidth &spansWidth)
{
    if (leftIndex >= spans.size()) {
        leftIndex = maxIndex;
        return;
    }
    if (rightIndex < spans.size()) {
        textSpan_ = spans.at(rightIndex).TryToTextSpan();
        if (textSpan_) {
            rightIndex = static_cast<size_t>(textSpan_->cgs_.GetRange().start);
        }
    } else {
        rightIndex = maxIndex;
    }
    double width = spansWidth.leftWidth + spansWidth.rightWidth - spans.at(leftIndex).GetWidth();
    textSpan_ = spans.at(leftIndex).TryToTextSpan();
    if (textSpan_) {
        leftIndex = static_cast<size_t>(textSpan_->cgs_.GetRange().start);
        double charsWidth = 0.0;
        for (; leftIndex < static_cast<size_t>(textSpan_->cgs_.GetRange().end); leftIndex++) {
            charsWidth += textSpan_->cgs_.GetCharWidth(leftIndex);
            if (static_cast<int>(charsWidth + width) > avalibleWidth_) {
                leftIndex--;
                break;
            }
        }
    }
}

void Shaper::CalcRightIndex(const std::vector<VariantSpan> &spans, const size_t maxIndex,
    size_t &leftIndex, size_t &rightIndex, struct SpansWidth &spansWidth)
{
    if (rightIndex >= spans.size()) {
        rightIndex = maxIndex;
        return;
    }
    if (leftIndex < spans.size()) {
        textSpan_ = spans.at(leftIndex).TryToTextSpan();
        if (textSpan_) {
            // end - 1 means the last index of the array
            leftIndex = static_cast<size_t>(textSpan_->cgs_.GetRange().end - 1);
        }
    } else {
        leftIndex = maxIndex;
    }
    double width = spansWidth.leftWidth + spansWidth.rightWidth - spans.at(rightIndex).GetWidth();
    textSpan_ = spans.at(rightIndex).TryToTextSpan();
    if (textSpan_) {
        // end - 1 means the last index of the array
        rightIndex = static_cast<size_t>(textSpan_->cgs_.GetRange().end - 1);
        double charsWidth = 0.0;
        for (; rightIndex >= static_cast<size_t>(textSpan_->cgs_.GetRange().start); rightIndex--) {
            charsWidth += textSpan_->cgs_.GetCharWidth(rightIndex);
            if (static_cast<int>(charsWidth + width) > avalibleWidth_) {
                rightIndex++;
                break;
            }
        }
    }
}

void Shaper::ConsiderMidSpanEllipsis(const std::vector<LineMetrics> &ellipsisMertics,
    const std::vector<VariantSpan> &ellipsisSpans, const TypographyStyle &style,
    const std::shared_ptr<FontProviders> &fontProviders)
{
    avalibleWidth_ = 0;
    std::vector<VariantSpan> spans;
    bool needProcess = GetAllSpan(ellipsisSpans, spans, ellipsisMertics);
    if (!needProcess) {
        return;
    }

    auto &filstSpan = spans.front();
    textSpan_ = filstSpan.TryToTextSpan();
    if (textSpan_ == nullptr) {
        return;
    }
    if (textSpan_->cgs_.IsSingleWord()) {
        ConsiderOneMidEllipsis(ellipsisMertics, ellipsisSpans, style, fontProviders);
        return;
    }

    size_t leftIndex = 0;
    size_t rightIndex = spans.size();
    size_t maxIndex = textSpan_->cgs_.GetSize();
    struct SpansWidth spansWidth;
    if (!CalcMidSpanIndex(spans, leftIndex, rightIndex, spansWidth)) {
        lineMetrics_ = ellipsisMertics;
        textSpan_ = nullptr;
        avalibleWidth_ = 0;
        return;
    }

    if (static_cast<int>(spansWidth.leftWidth) > static_cast<int>(spansWidth.rightWidth)) {
        CalcLeftIndex(spans, maxIndex, leftIndex, rightIndex, spansWidth);
    } else {
        CalcRightIndex(spans, maxIndex, leftIndex, rightIndex, spansWidth);
    }

    textSpan_ = filstSpan.TryToTextSpan();
    if (leftIndex < maxIndex) {
        SplitJointLeftSpans(ellipsisSpans, leftIndex, style, fontProviders);
    } else {
        lineMetrics_ = ellipsisMertics;
        textSpan_ = nullptr;
        avalibleWidth_ = 0;
        return;
    }

    if (rightIndex < maxIndex) {
        SplitJointRightSpans(ellipsisSpans, rightIndex, style, fontProviders);
    }
    textSpan_ = nullptr;
    avalibleWidth_ = 0;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
