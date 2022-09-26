/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_H

#include <cstddef>
#include <vector>

#include "include/core/SkCanvas.h"

#include "typography_types.h"

namespace OHOS {
namespace Rosen {
enum class RectWidthStyle {
    Tight,
    Max,
};

enum class Affinity {
    Upstream,
    Downstream,
};

enum class RectHeightStyle {
    Tight,
    Max,
    IncludeLinespaceMiddle,
    IncludeLinespaceTop,
    IncludeLinespaceBottom,
    Strut,
};

struct TextBox {
    SkRect rect_;
    TextDirection direction_;
    TextBox(SkRect rect, TextDirection direction);
};

struct PositionAndAffinity {
    size_t pos_;
    Affinity affinity_;
    PositionAndAffinity(size_t pos, Affinity affinity);
};

struct Range {
    size_t start_;
    size_t end_;

    Range(size_t start, size_t end);
    bool operator ==(const Range& rhs) const;
};

class Typography {
public:
    virtual ~Typography() = default;

    virtual double GetMaxWidth() const = 0;
    virtual double GetHeight() const = 0;
    virtual double GetActualWidth() const = 0;
    virtual double GetMinIntrinsicWidth() = 0;
    virtual double GetMaxIntrinsicWidth() = 0;
    virtual double GetAlphabeticBaseline() = 0;
    virtual double GetIdeographicBaseline() = 0;
    virtual bool DidExceedMaxLines() const = 0;
    virtual int GetLineCount() const = 0;

    virtual void SetIndents(const std::vector<float> &indents) = 0;
    virtual void Layout(double width) = 0;
    virtual void Paint(SkCanvas *canvas, double x, double y) = 0;

    virtual std::vector<TextBox> GetRectsForRange(size_t start, size_t end,
                                                  RectHeightStyle heightStyle,
                                                  RectWidthStyle widthStyle) = 0;
    virtual std::vector<TextBox> GetRectsForPlaceholders() = 0;
    virtual PositionAndAffinity GetGlyphPositionAtCoordinate(double dx, double dy) = 0;
    virtual Range GetWordBoundary(size_t offset) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_H
