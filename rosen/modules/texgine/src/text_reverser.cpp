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

#include "text_reverser.h"

#include "texgine/utils/exlog.h"
#include "texgine/utils/trace.h"

namespace Texgine {
void TextReverser::ReverseRTLText(std::vector<VariantSpan> &lineSpans)
{
    ScopedTrace scope("Shaper::ReverseRTLText");
    LOGSCOPED(sl, LOG2EX_DEBUG(), "ReverseRTLText");
    const auto &endit = lineSpans.end();
    auto rtlSpansBeginIt = endit;
    auto rtlSpansEndIt = endit;
    bool rtl = false;
    for (auto it = lineSpans.begin(); it !=lineSpans.end(); it++) {
        it->Dump(DumpType::DontReturn);
        auto ts = it->TryToTextSpan();
        if (ts != nullptr) {
            rtl = ts->IsRTL();
        }

        if (rtl) {
            rtlSpansBeginIt = rtlSpansBeginIt == endit ? it : rtlSpansBeginIt;
            rtlSpansEndIt = it;
            LOGCEX_DEBUG() << "is rtl";
            continue;
        } else {
            LOGCEX_DEBUG() << "no rtl";
        }

        if (rtlSpansBeginIt == endit) {
            continue;
        }

        rtlSpansEndIt++;
        LOGSCOPED(sl, LOG2EX_DEBUG(), "reverse");
        for (auto it = rtlSpansBeginIt; it != rtlSpansEndIt; it++) {
            it->Dump();
        }
        std::reverse(rtlSpansBeginIt, rtlSpansEndIt);
        rtlSpansBeginIt = endit;
        rtlSpansEndIt = endit;
    }

    if (rtlSpansBeginIt != endit) {
        rtlSpansEndIt++;
        LOGSCOPED(sl, LOG2EX_DEBUG(), "reverse");
        for (auto it = rtlSpansBeginIt; it != rtlSpansEndIt; it++) {
            it->Dump();
        }
        std::reverse(rtlSpansBeginIt, rtlSpansEndIt);
    }
}
} // namespace Texgine
