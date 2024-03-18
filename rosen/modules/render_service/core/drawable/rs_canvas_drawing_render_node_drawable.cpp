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

#include "drawable/rs_canvas_drawing_render_node_drawable.h"

#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSCanvasDrawingRenderNodeDrawable::Registrar RSCanvasDrawingRenderNodeDrawable::instance_;

RSCanvasDrawingRenderNodeDrawable::RSCanvasDrawingRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{
    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto canvasDrawingRenderNode = std::static_pointer_cast<RSCanvasDrawingRenderNode>(nodeSp);
    canvasDrawingRenderNode->InitRenderContent();
}

RSRenderNodeDrawable::Ptr RSCanvasDrawingRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return std::make_unique<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
}

void RSCanvasDrawingRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas) const
{
    auto& params = renderNode_->GetRenderParams();
    if (!params) {
        RS_LOGE("Failed to draw canvas drawing node, params is null!");
        return;
    }

    if (!params->GetShouldPaint()) {
        return;
    }

    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ConcatMatrix(params->GetMatrix());
    auto& bounds = params->GetBounds();
    bool quickRejected = canvas.QuickReject(bounds);
    if (quickRejected) {
        return;
    }

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto canvasDrawingRenderNode = std::static_pointer_cast<RSCanvasDrawingRenderNode>(nodeSp);
    auto canvasDrawingNodeRenderContent = canvasDrawingRenderNode->GetRenderContent();

    auto paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    if (!paintFilterCanvas) {
        RS_LOGE("Failed to draw canvas drawing node, paint filter canvas is null!");
        return;
    }

    if (!canvasDrawingNodeRenderContent->InitSurface(bounds.GetWidth(), bounds.GetHeight(), *paintFilterCanvas)) {
        RS_LOGE("Failed to init surface!");
        return;
    }

    // 1. Draw background of this drawing node by the main canvas.
    DrawBackground(canvas, bounds);

    // 2. Draw content of this drawing node by the content canvas.
    DrawRenderContent(*canvasDrawingNodeRenderContent, canvas, bounds);

    // 3. Draw children of this drawing node by the main canvas.
    void DrawChildren(Drawing::Canvas & canvas, const Drawing::Rect& rect);

    // 4. Draw foreground of this drawing node by the main canvas.
    void DrawForeground(Drawing::Canvas & canvas, const Drawing::Rect& rect);
}

void RSCanvasDrawingRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas) const {}

void RSCanvasDrawingRenderNodeDrawable::DrawRenderContent(
    RSCanvasDrawingRenderNodeContent& renderContent, Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    auto contentCanvas = renderContent.GetCanvas();
    DrawContent(*contentCanvas, rect);

    auto image = renderContent.Flush();
    if (image == nullptr) {
        RS_LOGE("Failed to draw gpu image!");
        return;
    }

    SKResourceManager::Instance().HoldResource(image);
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    Drawing::Paint paint;
    paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
    canvas.AttachPaint(paint);
    if (canvas.GetRecordingState()) {
        auto cpuImage = image->MakeRasterImage();
        canvas.DrawImage(*cpuImage, 0.f, 0.f, samplingOptions);
    } else {
        canvas.DrawImage(*image, 0.f, 0.f, samplingOptions);
    }
    canvas.DetachPaint();
}

} // namespace OHOS::Rosen
