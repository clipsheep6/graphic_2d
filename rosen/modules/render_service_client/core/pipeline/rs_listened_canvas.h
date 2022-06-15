/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {
class RSCanvasListener;

class RS_EXPORT RSListenedCanvas : public RSPaintFilterCanvas {
public:
    RSListenedCanvas(RSPaintFilterCanvas *canvas);
    ~RSListenedCanvas();

    void SetListener(const std::shared_ptr<RSCanvasListener> &listener);

    void onDrawPaint(const SkPaint& paint) override;
    void onDrawRect(const SkRect& rect, const SkPaint& paint) override;
    void onDrawRRect(const SkRRect& rrect, const SkPaint& paint) override;
    void onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                      const SkPaint& paint) override;
    void onDrawOval(const SkRect& rect, const SkPaint& paint) override;
    void onDrawArc(const SkRect& rect, SkScalar startAngle, SkScalar sweepAngle, bool useCenter,
                   const SkPaint& paint) override;
    void onDrawPath(const SkPath& path, const SkPaint& paint) override;
    void onDrawRegion(const SkRegion& region, const SkPaint& paint) override;
    void onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                        const SkPaint& paint) override;
    void onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                     const SkPoint texCoords[4], SkBlendMode mode,
                     const SkPaint& paint) override;
    void onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[],
                      const SkPaint& paint) override;
    void onDrawAnnotation(const SkRect& rect, const char key[], SkData* value) override;
    void onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rect) override;
    void onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix) override;
    void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix,
                       const SkPaint* paint) override;
    void willSave() override;
    void willRestore() override;
    void onFlush() override;
    void didTranslate(SkScalar dx, SkScalar dy) override;
    void onClipRect(const SkRect& rect, SkClipOp clipOp, ClipEdgeStyle style) override;
    void onClipRRect(const SkRRect& rect, SkClipOp clipOp, ClipEdgeStyle style) override;
    void onClipPath(const SkPath& path, SkClipOp clipOp, ClipEdgeStyle style) override;
    void onClipRegion(const SkRegion& region, SkClipOp clipop) override;

private:
    OHOS::Rosen::RSPaintFilterCanvas *canvas_ = nullptr;
    std::shared_ptr<RSCanvasListener> listener_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
