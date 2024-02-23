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
#ifndef ROSEN_MODULES_SPTEXT_LINE_METRICS_H
#define ROSEN_MODULES_SPTEXT_LINE_METRICS_H

#include <map>
#include <vector>

#include "include/core/SkFontMetrics.h"
#include "text/font_metrics.h"
#include "text_style.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class RunMetrics {
public:
    explicit RunMetrics(const TextStyle* style) : textStyle(style) {}

#ifndef USE_ROSEN_DRAWING
    RunMetrics(const TextStyle* style, const SkFontMetrics& metrics)
        : textStyle(style), fontMetrics(metrics) {}
#else
    RunMetrics(const TextStyle* style, const Drawing::FontMetrics& metrics)
        : textStyle(style), fontMetrics(metrics) {}
#endif

    const TextStyle* textStyle;
#ifndef USE_ROSEN_DRAWING
    SkFontMetrics fontMetrics;
#else
    Drawing::FontMetrics fontMetrics;
#endif
};

class LineMetrics {
public:
    size_t startIndex = 0;
    size_t endIndex = 0;
    size_t endExcludingWhitespace = 0;
    size_t endIncludingNewline = 0;
    bool hardBreak = false;

    double ascent = 0.0;
    double descent = 0.0;
    double unscaledAscent = 0.0;

    double height = 0.0;
    double width = 0.0;
    double left = 0.0;
    double baseline = 0.0;

    size_t lineNumber = 0;

    std::map<size_t, RunMetrics> runMetrics;
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_LINE_METRICS_H
