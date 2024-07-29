/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_CANVAS_H
#define RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_CANVAS_H

#include "./rs_parallel_recorder.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSParallelStatusCanvas : public RSPaintFilterCanvas {
public:
   RSParallelStatusCanvas(Drawing::Canvas* canvas);
   ~RSParallelStatusCanvas() override;
public:
    //Draw Command Hook
    void DrawPoint(const Drawing::Point& point) override {}
    void DrawPoints(Drawing::PointMode mode, size_t count, const Drawing::Point pts[]) override {}
    void DrawLine(const Drawing::Point& startPt, const Drawing::Point& endPt) override {}
    void DrawRect(const Drawing::Rect& rect) override {}
    void DrawRoundRect(const Drawing::RoundRect& roundRect) override {}
    void DrawNestedRoundRect(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner) override {}
    void DrawArc(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) override {}
    void DrawPie(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) override {}
    void DrawOval(const Drawing::Rect& oval) override {}
    void DrawCircle(const Drawing::Point& centerPt, Drawing::scalar radius) override {}
    void DrawPath(const Drawing::Path& path) override {}
    void DrawBackground(const Drawing::Brush& brush) override {}
    void DrawShadow(const Drawing::Path& path, const Drawing::Point3& planeParams,
        const Drawing::Point3& devLightPos, Drawing::scalar lightRadius,
        Drawing::Color ambientColor, Drawing::Color spotColor, Drawing::ShadowFlags flag) override {}
    void DrawColor(Drawing::ColorQuad color, Drawing::BlendMode mode = Drawing::BlendMode::SRC_OVER) override {}
    void DrawRegion(const Drawing::Region& region) override {}
    void DrawPatch(const Drawing::Point cubics[12], const Drawing::ColorQuad colors[4],
        const Drawing::Point texCoords[4], Drawing::BlendMode mode) override {}
    void DrawVertices(const Drawing::Vertices& vertices, Drawing::BlendMode mode) override {}
    void DrawImageNine(const Drawing::Image* image, const Drawing::RectI& center, const Drawing::Rect& dst,
        Drawing::FilterMode filter, const Drawing::Brush* brush = nullptr) override {}
    void DrawImageLattice(const Drawing::Image* image, const Drawing::Lattice& lattice, const Drawing::Rect& dst,
        Drawing::FilterMode filter, const Drawing::Brush* brush = nullptr) override {}
    void DrawBitmap(const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py) override {}
    void DrawImage(const Drawing::Image& image,
        const Drawing::scalar px, const Drawing::scalar py, const Drawing::SamplingOptions& sampling) override {}
    void DrawImageRect(const Drawing::Image& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint) override {}
    void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling) override {}
    void DrawPicture(const Drawing::Picture& picture) override {}
    void DrawTextBlob(const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y) override {}
    //ABORT!!!
    void SaveLayer(const Drawing::SaveLayerOps& saveLayerOps) override;
};

class RSB_EXPORT RSParallelDrawCanvas : public RSPaintFilterCanvas {
public:
    RSParallelDrawCanvas(Drawing::Canvas* canvas);
    RSParallelDrawCanvas(Drawing::Surface* surface);
    ~RSParallelDrawCanvas() override;
    void InheritStatus(RSParallelStatusCanvas* statusCanvas);
    //Shared Draw
    void DrawImage(const Drawing::Image& image,
        const Drawing::scalar px, const Drawing::scalar py, const Drawing::SamplingOptions& sampling) override;
    void DrawImageRect(const Drawing::Image& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint) override;
    void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling) override;
    void SetIsSubtreeParallel(bool canSharedDraw) override;
private:
    bool canSharedDraw_{ true };
};

class RSB_EXPORT RSParallelCanvas : public RSPaintFilterCanvas {
public:
    RSParallelCanvas(Drawing::Surface* surface);
    size_t AddParallelStatusCanvas(Drawing::Canvas* canvas);
    void RemoveAllStatusCanvas();
    void RemoveStatusCanvas(size_t idx);

private:
    std::unordered_map<size_t, std::shared_ptr<RSParallelStatusCanvas>> statusCanvasMap_{};
};

}
#endif