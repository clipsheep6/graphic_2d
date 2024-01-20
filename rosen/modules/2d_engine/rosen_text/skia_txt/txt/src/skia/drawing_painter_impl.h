/*
 * Copyright 2019 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DrawingPainterImpl_DEFINED
#define DrawingPainterImpl_DEFINED

#include "draw/canvas.h"
#include "modules/skparagraph/include/ParagraphPainter.h"
#include "txt/paint_record.h"

namespace skia {
namespace textlayout {

using RSCanvas = OHOS::Rosen::Drawing::Canvas;

class RSCanvasParagraphPainter : public ParagraphPainter {
public:
    RSCanvasParagraphPainter(RSCanvas* canvas, const std::vector<txt::PaintRecord>& paints);

    void drawTextBlob(const sk_sp<SkTextBlob>& blob, SkScalar x, SkScalar y, const SkPaintOrID& paint) override;
    void drawTextShadow(const sk_sp<SkTextBlob>& blob, SkScalar x, SkScalar y, SkColor color, SkScalar blurSigma) override;
    void drawRect(const SkRect& rect, const SkPaintOrID& paint) override;
    void drawFilledRect(const SkRect& rect, const DecorationStyle& decorStyle) override;
    void drawPath(const SkPath& path, const DecorationStyle& decorStyle) override;
    void drawLine(SkScalar x0, SkScalar y0, SkScalar x1, SkScalar y1, const DecorationStyle& decorStyle) override;

    void clipRect(const SkRect& rect) override;
    void translate(SkScalar dx, SkScalar dy) override;

    void save() override;
    void restore() override;

private:
    RSCanvas* fCanvas;
    const std::vector<txt::PaintRecord>& paints;
};

}  // namespace textlayout
}  // namespace skia

#endif  // DrawingPainterImpl_DEFINED
