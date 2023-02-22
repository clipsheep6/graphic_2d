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

#include <include/core/SkCanvas.h>

#include "texgine/any_span.h"

namespace Texgine {
class MyAnySpan : public AnySpan {
public:
    MyAnySpan(double width, double height,
              AnySpanAlignment align = AnySpanAlignment::AboveBaseline,
              TextBaseline baseline = TextBaseline::Alphabetic,
              double offset = 0.0,
              SkColor color = SK_ColorGRAY)
    {
        width_ = width;
        height_ = height;
        align_ = align;
        baseline_ = baseline;
        offset_ = offset;
        color_ = color;
    }

    ~MyAnySpan() = default;

    double GetWidth() const override
    {
        return width_;
    }

    double GetHeight() const override
    {
        return height_;
    }

    AnySpanAlignment GetAlignment() const override
    {
        return align_;
    }

    TextBaseline GetBaseline() const override
    {
        return baseline_;
    }

    double GetLineOffset() const override
    {
        return offset_;
    }

    void Paint(SkCanvas &canvas, double offsetx, double offsety) override
    {
        SkPaint paint;
        paint.setColor(color_);
        paint.setStyle(paint.kFill_Style);
        canvas.drawRect(SkRect::MakeXYWH(offsetx, offsety, width_, height_), paint);
    }

private:
    double width_ = 0.0;
    double height_ = 0.0;
    AnySpanAlignment align_ = AnySpanAlignment::AboveBaseline;
    TextBaseline baseline_ = TextBaseline::Alphabetic;
    double offset_ = 0.0;
    SkColor color_ = SK_ColorGRAY;
};
} // namespace Texgine
