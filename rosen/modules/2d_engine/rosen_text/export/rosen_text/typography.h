/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "draw/canvas.h"
#include "include/core/SkCanvas.h" // SKIA
#include "utils/rect.h"

#include "typography_types.h"

namespace OHOS {
namespace Rosen {
enum class TextRectWidthStyle {
    TIGHT,
    MAX,
};

enum class TextRectHeightStyle {
    TIGHT,
    COVER_TOP_AND_BOTTOM,
    COVER_HALF_TOP_AND_BOTTOM,
    COVER_TOP,
    COVER_BOTTOM,
    FOLLOW_BY_STRUT,
};

struct TextRect {
    Drawing::RectF rect_;
    TextDirection direction_;
    TextRect(Drawing::RectF rect, TextDirection direction);
};

enum class Affinity {
    PREV,
    NEXT,
};

struct IndexAndAffinity {
    size_t index_;
    Affinity affinity_;
    IndexAndAffinity(size_t index, Affinity affinity);
};

struct Boundary {
    size_t leftIndex_; // include leftIndex_
    size_t rightIndex_; // not include rightIndex_

    Boundary(size_t left, size_t right);
    bool operator ==(const Boundary& rhs) const;
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
    virtual void Paint(SkCanvas *canvas, double x, double y) = 0; // SKIA
    virtual void Paint(Drawing::Canvas *canvas, double x, double y) = 0; // DRAWING

    virtual std::vector<TextRect> GetTextRectsByBoundary(size_t left, size_t right,
                                                         TextRectHeightStyle heightStyle,
                                                         TextRectWidthStyle widthStyle) = 0;
    virtual std::vector<TextRect> GetTextRectsOfPlaceholders() = 0;
    virtual IndexAndAffinity GetGlyphIndexByCoordinate(double x, double y) = 0;
    virtual Boundary GetWordBoundaryByIndex(size_t index) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_H
