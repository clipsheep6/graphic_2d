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

#ifndef SKIA_CANVAS_ENFORCER_H
#define SKIA_CANVAS_ENFORCER_H

#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkRect.h"
#include "include/utils/SkNoDrawCanvas.h"
#include "include/core/SkCanvas.h"

#include "draw/core_canvas.h"
#include "skia_canvas.h"
#include "skia_paint.h"

namespace OHOS{
namespace Rosen {
namespace Drawing {
class SkiaCanvasEnforcer : public SkNoDrawCanvas {
public:
    SkiaCanvasEnforcer(CoreCanvas& canvas) ;
    ~SkiaCanvasEnforcer() override;

protected:
    void willSave() override;
    void willRestore() override;

    void onFlush() override;

    SaveLayerStrategy getSaveLayerStrategy(const SaveLayerRec& rec) override;

    void onDrawOval(const SkRect& rect, const SkPaint& paint) override;
    void onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[],
                      const SkPaint& paint) override;
    void onDrawPath(const SkPath& path, const SkPaint& paint) override;
    void onDrawRRect(const SkRRect& rrect, const SkPaint& paint) override;

    void didConcat(const SkMatrix& matrix) override;
    void didSetMatrix(const SkMatrix& matrix) override;
    void didTranslate(SkScalar dx, SkScalar dy) override;

private:
    void AttachPaint(CoreCanvas* canvas, const SkPaint& paint);
    void DetachPaint(CoreCanvas* canvas, const SkPaint& paint);

    CoreCanvas* coreCanvas_;
    SkiaPaint skiaPaint_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS



#endif //  SKIA_CANVAS_ENFORCER_H
