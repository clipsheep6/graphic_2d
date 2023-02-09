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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_ANY_SPAN_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_ANY_SPAN_H

#include "texgine/text_style.h"
#include "texgine/typography_types.h"
#include "texgine/utils/memory_object.h"

class SkCanvas;
namespace Texgine {
/** \enum AnySpanAlignment
 *  AnySpanAlignment is the alignment of the AnySpan in a Typography.
 */
enum class AnySpanAlignment {
    OffsetAtBaseline, //!< The bottom edge of the AnySpan is aligned with
                      //   the offset position below the baseline.
    AboveBaseline,    //!< The bottom edge of the AnySpan is aligned with baseline.
    BelowBaseline,    //!< The top edge of the AnySpan is aligned with baseline.
    TopOfRowBox,      //!< The top edge of the AnySpan is aligned with the
                      //   top edge of the row box.
    BottomOfRowBox,   //!< The bottom edge of the AnySpan is aligned with the
                      //   bottom edge of the row box.
    CenterOfRowBox,   //!< The vertical centerline of the AnySpan is aligned with the
                      //   vertical centerline of the row box.
};

/** \class AnySpan
 *  AnySpan is a span that can be any width, height and draw any content.
 */
class AnySpan : public MemoryObject {
public:
    // Is interface.
    virtual ~AnySpan() = default;

    // Returns the width of the span.
    virtual double GetWidth() const = 0;

    // Returns the height of the span.
    virtual double GetHeight() const = 0;

    // Returns the alignment of the span.
    virtual AnySpanAlignment GetAlignment() const = 0;

    // Returns the baseline of the span.
    virtual TextBaseline GetBaseline() const = 0;

    // Returns the offset of the span.
    virtual double GetLineOffset() const = 0;

    /** This method will be called when the Typography is drawn.
     *
     *  @param canvas  Canvas to be drawn.
     *  @param offsetx The Offset in x-asix of the starting point for drawing the AnySpan
     *  @param offsety The Offset in y-asix of the starting point for drawing the AnySpan
     */
    virtual void Paint(SkCanvas &canvas, double offsetx, double offsety) = 0;

private:
    friend void ReportMemoryUsage(const std::string &member, const AnySpan &that, bool needThis);
    void ReportMemoryUsage(const std::string &member, bool needThis) const override;
};
} // namespace Texgine

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_ANY_SPAN_H
