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

#include "word_breaker.h"

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"

namespace Texgine {
void WordBreaker::SetLocale(const icu::Locale &locale)
{
    locale_ = locale;
}

void WordBreaker::SetRange(size_t start, size_t end)
{
    startIndex_ = start;
    endIndex_ = end;
}

std::vector<Boundary> WordBreaker::GetBoundary(const std::vector<uint16_t> &u16str, bool isWordInstance)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::BreakIterator *wbi = nullptr;
    if (isWordInstance) {
        wbi = icu::BreakIterator::createWordInstance(locale_, status);
    } else {
        wbi = icu::BreakIterator::createLineInstance(locale_, status);
    }

    if (wbi == nullptr) {
        LOG2EX(ERROR) << "create BreakIterator failed";
        throw TEXGINE_EXCEPTION(APIFailed);
    }

    // > U_ZERO_ERROR: error, < U_ZERO_ERROR: warning
    if (status > U_ZERO_ERROR) {
        LOG2EX(ERROR) << "status is error";
        throw TEXGINE_EXCEPTION(APIFailed);
    }

    if (endIndex_ <= startIndex_) {
        LOG2EX(ERROR) << "endIndex_ <= startIndex_";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    if (endIndex_ > u16str.size()) {
        LOG2EX(ERROR) << "endIndex_ > u16str.size()";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    wbi->setText({u16str.data() + startIndex_, static_cast<int32_t>(endIndex_ - startIndex_)});

    std::vector<Boundary> boundaries;
    auto beg = wbi->first();
    for (auto end = wbi->next(); end != wbi->DONE; end = wbi->next()) {
        boundaries.emplace_back(beg, end);
        beg = end;
    }

    return boundaries;
}
} // namespace Texgine
