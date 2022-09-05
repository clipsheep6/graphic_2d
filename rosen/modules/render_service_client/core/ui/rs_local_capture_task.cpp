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

 #include "ui/rs_local_capture_task.h"
#include <cstddef>
 #include "platform/common/rs_log.h"
 #include "pipeline/rs_node_map.h"
 #include "pipeline/rs_render_thread_visitor.h"
 #include "pipeline/rs_render_node_map.h"
 #include "pipeline/rs_render_thread.h"
 #include "pipeline/rs_canvas_render_node.h"
 #include "pipeline/rs_root_render_node.h"

namespace OHOS {

namespace Rosen {

std::shared_ptr<Media::PixelMap> RSLocalCaptureTask::GetLocalCapture(NodeId nodeId, float scaleX, float scaleY)
{
    nodeId_ = nodeId;
    scaleX_ = scaleX;
    scaleY_ = scaleY;
    if (scaleX_ <= 0.f || scaleY_ <= 0.f) {
        ROSEN_LOGE("RSLocalCaptureTask::GetLocalCapture: scale is invalid.");
        return nullptr;
    }
    // create rs_render_node, because it maybe canvasNode、surfaceNode、rootNode
    auto node = RSRenderThread::Instance().GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(nodeId_);
    if (node == nullptr) {
        ROSEN_LOGE("RSLocalCaptureTask::GetLocalCapture: node is nullptr");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelmap;
    std::shared_ptr<RSLocalCaptureVisitor> visitor = std::make_shared<RSLocalCaptureVisitor>(scaleX_, scaleY_);  // create a visitor
    // creat pixelMap
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    pixelmap = Media::PixelMap::Create(opts);
    // Get sksurface according to pixelmap
    if (pixelmap == nullptr) {
        ROSEN_LOGE("RSLocalCaptureTask::GetLocalCapture: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        ROSEN_LOGE("RSLocalCaptureTask::GetLocalCapture: address == nullptr");
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
            kRGBA_8888_SkColorType, kPremul_SkAlphaType);
#ifdef ACE_ENABLE_GL
    RenderContext* rc = RSRenderThread::Instance().GetRenderContext();
    if(rc == nullptr) {
        ROSEN_LOGE("RSLocalCaptureTask::createSurface: RenderContext == nullptr");
        return nullptr;
    }
    auto skSurface = SkSurface::MakeRenderTarget(rc->GetGrContext(), SkBudgeted::kNo, info);
#else
    auto skSurface = SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
#endif
    visitor->SetSurface(skSurface.get());
    // Draw
    if (node->GetType() == RSRenderNodeType::RS_NODE) { // if node is surfacenode, it must be processed in rs, so here directly return
        return nullptr;
    }
    node->Process(visitor);
    return pixelmap;
}

void RSLocalCaptureTask::RSLocalCaptureVisitor::SetSurface(SkSurface* surface)
{
    if (surface == nullptr) {
        ROSEN_LOGE("surface == nullptr");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface);
    canvas_->scale(scaleX_, scaleY_);
}

void RSLocalCaptureTask::RSLocalCaptureVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGD("ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        ROSEN_LOGE("ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSLocalCaptureTask::RSLocalCaptureVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGD("ProcessRootRenderNode, no need process");
        return;
    }

    if (!canvas_) {
        ROSEN_LOGE("ProcessRootRenderNode, canvas is nullptr");
        return;
    }

    canvas_->save();
    canvas_->clipRect(SkRect::MakeWH(node.GetSurfaceWidth(), node.GetSurfaceHeight()));
    canvas_->clear(SK_ColorTRANSPARENT);
    ProcessCanvasRenderNode(node);
    canvas_->restore();
}

void RSLocalCaptureTask::RSLocalCaptureVisitor::ProcessBaseRenderNode(RSBaseRenderNode &node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSLocalCaptureTask::RSLocalCaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGI("ProcessSurfaceRenderNode node : %" PRIu64 " is invisible", node.GetId());
        return;
    }
    // in renderthread we can't process surfacenode, so here we directly return
    return;
}

} // namespace Rosen
} // namespace OHOS