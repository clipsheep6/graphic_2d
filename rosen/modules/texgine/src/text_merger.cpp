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

#include "text_merger.h"

#include <unicode/ubidi.h>

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#include "text_span.h"
#include "texgine/utils/trace.h"

namespace Texgine {
std::ostream &operator <<(std::ostream &os, const MergeResult &result)
{
    switch (result) {
        case MergeResult::accepted:
            os << "accepted";
            break;
        case MergeResult::breaked:
            os << "breaked";
            break;
        case MergeResult::rejected:
            os << "rejected";
            break;
        case MergeResult::ignore:
            os << "ignore";
            break;
        default:
            break;
    }
    return os;
}

std::vector<VariantSpan> TextMerger::MergeSpans(const std::vector<VariantSpan> &spans)
{
    ScopedTrace scope("TextMerger::MergeSpans");
    std::vector<VariantSpan> vss;
    auto it = spans.begin();
    while (it != spans.end()) {
        CharGroups cgs;
        std::optional<bool> currentRTL = std::nullopt;
        for (; it != spans.end(); it++) {
            LOGSCOPED(sl, LOG2EX_DEBUG(), "MergeSpan");
            auto result = MergeSpan(*it, currentRTL, cgs);
            it->Dump(DumpType::DontReturn);
            LOGCEX_DEBUG() << result;
            if (result == MergeResult::breaked) {
                it++;
                break;
            } else if (result == MergeResult::rejected) {
                break;
            } else if (result == MergeResult::ignore) {
                vss.push_back(*it);
            }
        }

        if (it == spans.begin()) {
            continue;
        }

        auto &lastSpan = *(it - 1);
        if (auto ts = lastSpan.TryToTextSpan(); ts != nullptr) {
            lastSpan.Dump(DumpType::DontReturn);
            LOGCEX_DEBUG() << "generated";

            auto mergedSpan = ts->CloneWithCharGroups(cgs);
            VariantSpan vs(mergedSpan);
            vs.SetTextStyle(lastSpan.GetTextStyle());
            vss.push_back(vs);
        }
    }
    return vss;
}

MergeResult TextMerger::MergeSpan(const VariantSpan &span, std::optional<bool> &currentRTL, CharGroups &cgs)
{
    if (span == nullptr) {
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    auto ts = span.TryToTextSpan();
    if (ts == nullptr && cgs.IsValid()) {
        return MergeResult::rejected;
    }

    if (ts == nullptr && !cgs.IsValid()) {
        return MergeResult::ignore;
    }

    auto rtl = ts->IsRTL();
    if (rtl != currentRTL.value_or(rtl)) {
        return MergeResult::rejected;
    }

    currentRTL = rtl;
    if (!ts->cgs_.IsValid() || ts->cgs_.GetSize() == 0) {
        throw TEXGINE_EXCEPTION(ErrorStatus);
    }

    if (cgs.IsValid() == true && cgs.IsSameCharGroups(ts->cgs_) == false) {
        return MergeResult::rejected;
    }

    if (cgs.IsValid() == false) {
        cgs = ts->cgs_;
    } else if (cgs.Get(0).typeface_ != ts->typeface_) {
        return MergeResult::rejected;
    } else {
        cgs.Merge(ts->cgs_);
    }

    bool isWhitespace = (u_isWhitespace(ts->cgs_.GetBack().chars_[0]) == 1);
    if (isWhitespace) {
        return MergeResult::breaked;
    } else {
        return MergeResult::accepted;
    }
}
} // namespace Texgine
