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

#include "drawable/rs_surface_render_node_drawable.h"

#include <memory>
#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "impl_interface/region_impl.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "utils/rect.h"
#include "utils/region.h"

namespace OHOS::Rosen::DrawableV2 {
RSSurfaceRenderNodeDrawable::Registrar RSSurfaceRenderNodeDrawable::instance_;

RSSurfaceRenderNodeDrawable::RSSurfaceRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSSurfaceRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSSurfaceRenderNodeDrawable(std::move(node));
}

Drawing::Region RSSurfaceRenderNodeDrawable::CalculateVisibleRegion(RSSurfaceRenderParams* surfaceParams,
   std::shared_ptr<RSSurfaceRenderNode> surfaceNode) const
{
    Drawing::Region resultRegion;
    if (!surfaceParams->IsMainWindowType()) {
        return resultRegion;
    }

    // The region is dirty region of this SurfaceNode.
    Occlusion::Region surfaceNodeDirtyRegion(surfaceNode->GetDirtyManager()->GetDirtyRegion());
    // The region is the result of global dirty region AND occlusion region.
    Occlusion::Region globalDirtyRegion = surfaceNode->GetGlobalDirtyRegion();
    // This include dirty region and occlusion region when surfaceNode is mainWindow.
    auto visibleDirtyRegion = globalDirtyRegion.Or(surfaceNodeDirtyRegion);
    if (visibleDirtyRegion.IsEmpty()) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::OnDraw occlusion skip SurfaceName:%s NodeId:%" PRIu64 "",
            surfaceNode->GetName().c_str(), surfaceParams->GetId());
        return resultRegion;
    }

    for (auto& rect : visibleDirtyRegion.GetRegionRects()) {
        Drawing::Region tempRegion;
        tempRegion.SetRect(Drawing::RectI(rect.left_, rect.top_, rect.right_, rect.bottom_));
        resultRegion.Op(tempRegion, Drawing::RegionOp::UNION);
    }
    return resultRegion;
}

void RSSurfaceRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    if (!renderNode_) {
        RS_LOGE("There is no CanvasNode in RSSurfaceRenderNodeDrawable");
        return;
    }

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto surfaceNode = std::static_pointer_cast<RSSurfaceRenderNode>(nodeSp);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw params is nullptr");
        return;
    }
    Drawing::Region resultRegion = CalculateVisibleRegion(surfaceParams, surfaceNode);
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if ((!uniParam || uniParam->IsOpDropped()) && surfaceParams->IsMainWindowType() && resultRegion.IsEmpty()) {
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw occlusion skip SurfaceName:%s NodeId:%" PRIu64 "",
            surfaceNode->GetName().c_str(), surfaceParams->GetId());
        return;
    }

    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw, rscanvas us nullptr");
        return;
    }

    // TO-DO [UI First] Check UpdateCacheSurface
    // TO-DO [DFX] Draw Context ClipRect

    RS_TRACE_NAME("RSSurfaceRenderNodeDrawable::OnDraw:[" + surfaceNode->GetName() + "] " +
                  surfaceParams->GetAbsDrawRect().ToString() + "Alpha: " + std::to_string(surfaceNode->GetGlobalAlpha()));

    RS_LOGD("RSSurfaceRenderNodeDrawable::OnDraw node:%{public}" PRIu64 ",child size:%{public}u,"
            "name:%{public}s,OcclusionVisible:%{public}d",
        surfaceParams->GetId(), surfaceNode->GetChildrenCount(), surfaceNode->GetName().c_str(),
        surfaceParams->GetOcclusionVisible());

    auto renderEngine_ = RSUniRenderThread::Instance().GetRenderEngine();

    Drawing::GPUContext* gpuContext = renderEngine_->GetRenderContext()->GetDrGPUContext();
    surfaceNode->SetDrawingGPUContext(gpuContext); // TO-DO
    RSTagTracker tagTracker(gpuContext, surfaceParams->GetId(), RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE);

    surfaceNode->UpdateFilterCacheStatusWithVisible(true);

    // TO-DO [Sub Thread] CheckFilterCache

    RSAutoCanvasRestore acr(rscanvas);

    rscanvas->MultiplyAlpha(surfaceParams->GetAlpha());

    bool isSelfDrawingSurface = surfaceParams->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    if (isSelfDrawingSurface && !surfaceParams->IsSpherizeValid()) {
        rscanvas->Save();
    }

    if (surfaceParams->IsMainWindowType()) {
        rscanvas->UpdateDirtyRegion(resultRegion);
        rscanvas->SetDirtyFlag(true);
    }

    rscanvas->ConcatMatrix(surfaceParams->GetMatrix());

    if (isSelfDrawingSurface) {
        RSUniRenderUtil::FloorTransXYInCanvasMatrix(*rscanvas);
    }

    nodeSp->ProcessRenderBeforeChildren(*rscanvas);

    if (surfaceParams->GetBuffer() != nullptr) {
        DealWithSelfDrawingNodeBuffer(*surfaceNode, *rscanvas, *surfaceParams);
    }

    if (isSelfDrawingSurface) {
        rscanvas->Restore();
    }

    RSRenderNodeDrawable::OnDraw(canvas);
    rscanvas->SetDirtyFlag(false);
}

void RSSurfaceRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    if (!renderNode_) {
        RS_LOGE("There is no SurfaceRender in RSSurfaceRenderNodeDrawable");
        return;
    }

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto surfaceNode = std::static_pointer_cast<RSSurfaceRenderNode>(nodeSp);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw params is nullptr");
        return;
    }

    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw, rscanvas us nullptr");
        return;
    }
    rscanvas->MultiplyAlpha(surfaceParams->GetAlpha());

    if (surfaceParams->IsSecurityLayer() || surfaceParams->IsSkipLayer()) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CaptureSingleSurfaceNode: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) clear white since it is security layer.",
            surfaceParams->GetId());
        return;
    }
    
    if (RSUniRenderThread::GetCaptureParam().isCaptureDisplay_) {
        CaptureSurfaceInDisplay(*surfaceNode, *rscanvas, *surfaceParams);
    } else {
        CaptureSingleSurfaceNode(*surfaceNode, *rscanvas, *surfaceParams);
    }
}

void RSSurfaceRenderNodeDrawable::CaptureSingleSurfaceNode(RSSurfaceRenderNode& surfaceNode,
    RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams)
{
    uint32_t saveCount = canvas.GetSaveCount();
    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto nodeType = surfaceParams.GetSurfaceNodeType();
    bool isSelfDrawingSurface = (nodeType == RSSurfaceNodeType::SELF_DRAWING_NODE);
    if (isSelfDrawingSurface) {
        canvas.Save();
    }

    Drawing::Matrix captureMatrix = Drawing::Matrix();
    if (nodeType == RSSurfaceNodeType::APP_WINDOW_NODE) {
        // When CaptureSingleSurfaceNodeWithUni, we should consider scale factor of canvas_ and
        // child nodes (self-drawing surfaceNode) of AppWindow should use relative coordinates
        // which is the node relative to the upper-left corner of the window.
        // So we have to get the invert matrix of AppWindow here and apply it to canvas_
        // when we calculate the position of self-drawing surfaceNode.
        auto scaleX = RSUniRenderThread::GetCaptureParam().scaleX_;
        auto scaleY = RSUniRenderThread::GetCaptureParam().scaleY_;
        
        captureMatrix.Set(Drawing::Matrix::Index::SCALE_X, scaleX);
        captureMatrix.Set(Drawing::Matrix::Index::SCALE_Y, scaleY);
        Drawing::Matrix invertMatrix;
        if (surfaceParams.GetMatrix().Invert(invertMatrix)) {
            captureMatrix.PreConcat(invertMatrix);
        }
    } else if (nodeType == RSSurfaceNodeType::STARTING_WINDOW_NODE) {
        canvas.SetMatrix(surfaceParams.GetMatrix());
    }

    if (isSelfDrawingSurface) {
        RSUniRenderUtil::FloorTransXYInCanvasMatrix(canvas);
    }

    nodeSp->ProcessRenderBeforeChildren(canvas);

    if (surfaceParams.IsSecurityLayer() || surfaceParams.IsSkipLayer()) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CaptureSingleSurfaceNode: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) clear white since it is security layer.",
            surfaceParams.GetId());
        canvas.Clear(Drawing::Color::COLOR_WHITE);
        canvas.RestoreToCount(saveCount);
        return;
    }

    if (surfaceParams.GetBuffer() != nullptr) {
        DealWithSelfDrawingNodeBuffer(surfaceNode, canvas, surfaceParams);
    }

    if (isSelfDrawingSurface) {
        canvas.Restore();
    }

    // TODO
    // if (!surfaceNode.GetHasSecurityLayer() && !surfaceNode.GetHasSkipLayer() &&
    //     isUIFirst_ && RSUniRenderUtil::HandleCaptureNode(surfaceNode, canvas)) {
    //     RS_LOGD("RSSurfaceRenderNodeDrawable::CaptureSingleSurfaceNode: \
    //         process RSSurfaceRenderNode [%{public}s, %{public}" PRIu64 "] use cache texture.",
    //         surfaceNode.GetName().c_str(), surfaceParams.GetId());
    //     return;
    // }
    RSRenderNodeDrawable::OnCapture(canvas);
}

void RSSurfaceRenderNodeDrawable::CaptureSurfaceInDisplay(RSSurfaceRenderNode& surfaceNode,
    RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams)
{
    if (surfaceParams.IsSecurityLayer() || surfaceParams.IsSkipLayer()) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CaptureSurfaceInDisplay: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) paused since it is security layer.",
            surfaceParams.GetId());
        return;
    }

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    canvas.ConcatMatrix(surfaceParams.GetMatrix());

    // TODO
    // if (!node.GetHasSecurityLayer() && !node.GetHasSkipLayer() &&
    //     isUIFirst_ && RSUniRenderUtil::HandleSubThreadNode(node, *canvas_)) {
    //     RS_LOGD("RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni: \
    //         process RSSurfaceRenderNode [%{public}s, %{public}" PRIu64 "] use cache texture.",
    //         node.GetName().c_str(), node.GetId());
    //     return;
    // }

    auto nodeType = surfaceParams.GetSurfaceNodeType();
    bool isSelfDrawingSurface = (nodeType == RSSurfaceNodeType::SELF_DRAWING_NODE);
    if (isSelfDrawingSurface) {
        canvas.Save();
    }

    if (isSelfDrawingSurface) {
        RSUniRenderUtil::FloorTransXYInCanvasMatrix(canvas);
    }

    nodeSp->ProcessRenderBeforeChildren(canvas);

    if (isSelfDrawingSurface) {
        canvas.Restore();
    }

    if (surfaceParams.GetBuffer() != nullptr) {
        DealWithSelfDrawingNodeBuffer(surfaceNode, canvas, surfaceParams);
    }

    if (isSelfDrawingSurface) {
        canvas.Restore();
    }

    RSRenderNodeDrawable::OnCapture(canvas);
}

void RSSurfaceRenderNodeDrawable::DealWithSelfDrawingNodeBuffer(RSSurfaceRenderNode& surfaceNode,
    RSPaintFilterCanvas& canvas, const RSSurfaceRenderParams& surfaceParams)
{
    if (surfaceParams.GetHardwareEnabled()) {
        if (!surfaceNode.IsHardwareEnabledTopSurface()) {
            canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
        }
        return;
    }
    surfaceNode.SetGlobalAlpha(1.0f); // TO-DO
    int threadIndex = 0;
    auto params = RSUniRenderUtil::CreateBufferDrawParam(surfaceNode, false, threadIndex, true);
    params.targetColorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    auto screenManager = CreateOrGetScreenManager();
    auto ancestor = surfaceParams.GetAncestorDisplayNode().lock()->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!ancestor) {
        RS_LOGE("surfaceNode GetAncestorDisplayNode() return nullptr");
        return;
    }
    auto ancestorParam = static_cast<RSDisplayRenderParams*>(ancestor->GetRenderParams().get());
    params.screenBrightnessNits =
        screenManager->GetScreenBrightnessNits(ancestorParam ? ancestorParam->GetScreenId() : 0);
#endif
    auto bgColor = surfaceParams.GetBackgroundColor();
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if ((surfaceParams.GetSelfDrawingNodeType() != SelfDrawingNodeType::VIDEO) &&
        (bgColor != RgbPalette::Transparent())) {
        auto bounds = RSPropertiesPainter::Rect2DrawingRect(
            { 0, 0, surfaceParams.GetBounds().GetWidth(), surfaceParams.GetBounds().GetHeight() });
        Drawing::SaveLayerOps layerOps(&bounds, nullptr);
        canvas.SaveLayer(layerOps);
        canvas.SetAlpha(1.0f);
        Drawing::Brush brush;
        brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(RSPropertiesPainter::RRect2DrawingRRect(surfaceParams.GetRRect()));
        canvas.DetachBrush();
        renderEngine->DrawSurfaceNodeWithParams(canvas, surfaceNode, params);
        canvas.Restore();
    } else {
        renderEngine->DrawSurfaceNodeWithParams(canvas, surfaceNode, params);
    }
}
} // namespace OHOS::Rosen::DrawableV2
