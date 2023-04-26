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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXT_MERGER_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXT_MERGER_H

#include <vector>

#include "variant_span.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
enum class MergeResult {
    ACCEPTED,
    REJECTED,
    BREAKED,
    IGNORE,
};

class TextMerger {
public:
    std::vector<VariantSpan> MergeSpans(const std::vector<VariantSpan> &spans);
    MergeResult MergeSpan(const VariantSpan &span, std::optional<bool> &currentRTL, CharGroups &cgs);
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXT_MERGER_H
