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

#include "ui/rs_offscreen_render.h"
#include <cstddef>
#include "platform/common/rs_log.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread_visitor.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "include/core/SkMatrix.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_capture_callback.h"
#include "transaction/rs_render_service_client.h"
#include "include/core/SkImage.h"
#include "pixel_map_rosen_utils.h"
#include "pixel_map.h"

namespace OHOS {

namespace Rosen {

std::shared_ptr<Media::PixelMap> RSOffscreenRender::GetLocalCapture(NodeId nodeId, float scaleX, float scaleY)
{
    nodeId_ = nodeId;
    scaleX_ = scaleX;
    scaleY_ = scaleY;
    if (scaleX_ <= 0.f || scaleY_ <= 0.f) {
        ROSEN_LOGE("RSOffscreenRender::GetLocalCapture: scale is invalid.");
        return nullptr;
    }
    // create rs_render_node,because it maybe canvasNode or surfaceNode or rootNode
    auto node = RSRenderThread::Instance().GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(nodeId_);
    std::shared_ptr<Media::PixelMap> pixelmap;
    pixelmap = CreatePixelMapByNode(node);
    auto skSurface = CreateSurface(pixelmap);
    std::shared_ptr<RSOffscreenRenderVisitor> visitor = std::make_shared<RSOffscreenRenderVisitor>(scaleX_, scaleY_, nodeId_);
    visitor->SetSurface(skSurface.get());
    // Draw
    node->Process(visitor);
#ifdef ACE_ENABLE_GL
    sk_sp<SkImage> img(skSurface.get()->makeImageSnapshot());
    if (!img) {
        ROSEN_LOGE("RSOffscreenRender::GetLocalCapture: img is nullptr");
        return nullptr;
    }

    auto data = (uint8_t *)malloc(pixelmap->GetRowBytes() * pixelmap->GetHeight());
    if (data == nullptr) {
        ROSEN_LOGE("RSOffscreenRender::GetLocalCapture: data is nullptr");
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    if (!img->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
        ROSEN_LOGE("RSOffscreenRender::GetLocalCapture: readPixels failed");
        free(data);
        data = nullptr;
        return nullptr;
    }
    pixelmap->SetPixelsAddr(data, nullptr, pixelmap->GetRowBytes() * pixelmap->GetHeight(),
        Media::AllocatorType::HEAP_ALLOC, nullptr);
    return pixelmap;
#endif
    return pixelmap;
}

std::shared_ptr<Media::PixelMap> RSOffscreenRender::CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node)
{
    if (node == nullptr) {
        ROSEN_LOGE("RSOffscreenRender::GetLocalCapture: node is nullptr");
        return nullptr;
    }
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    return Media::PixelMap::Create(opts);
}

sk_sp<SkSurface> RSOffscreenRender::CreateSurface(const std::shared_ptr<Media::PixelMap> pixelmap)
{
    if (pixelmap == nullptr) {
        ROSEN_LOGE("RSOffscreenRender::GetLocalCapture: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        ROSEN_LOGE("RSOffscreenRender::GetLocalCapture: address == nullptr");
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
#ifdef ACE_ENABLE_GL
    RenderContext* rc = RSRenderThread::Instance().GetRenderContext();
    if (rc == nullptr) {
        ROSEN_LOGE("RSOffscreenRender::createSurface: RenderContext == nullptr");
        return nullptr;
    }
    rc->SetUpGrContext();
    return SkSurface::MakeRenderTarget(rc->GetGrContext(), SkBudgeted::kNo, info);
#endif
    return SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

RSOffscreenRender::RSOffscreenRenderVisitor::RSOffscreenRenderVisitor(float scaleX, float scaleY, NodeId nodeId)
    : scaleX_(scaleX), scaleY_(scaleY), nodeId_(nodeId)
{

}

void RSOffscreenRender::RSOffscreenRenderVisitor::SetSurface(SkSurface* surface)
{
    if (surface == nullptr) {
        ROSEN_LOGE("surface == nullptr");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface);
    canvas_->scale(scaleX_, scaleY_);
}

void RSOffscreenRender::RSOffscreenRenderVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGD("ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        ROSEN_LOGE("ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    if(node.GetId() == nodeId_) {
        // When drawing nodes, canvas will offset the bounds value, so we will move in reverse here first
        canvas_->translate(-node.GetRenderProperties().GetBoundsPositionX(), -node.GetRenderProperties().GetBoundsPositionY());
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSOffscreenRender::RSOffscreenRenderVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
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

void RSOffscreenRender::RSOffscreenRenderVisitor::ProcessBaseRenderNode(RSBaseRenderNode &node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSOffscreenRender::RSOffscreenRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("RSOffscreenRender::RSOffscreenRenderVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGI("RSOffscreenRender::RSOffscreenRenderVisitor::ProcessSurfaceRenderNode node : %" PRIu64 " is invisible", node.GetId());
        return;
    }
    if (node.GetSecurityLayer()) {
        return;
    }
    if(node.GetId() == nodeId_) {
        // When drawing nodes, canvas will offset the bounds value, so we will move in reverse here first
        canvas_->translate(-node.GetRenderProperties().GetBoundsPositionX(), -node.GetRenderProperties().GetBoundsPositionY());
    }
    // we can't get buffer in renderThread, so use renderService surface capture to process surfaceNode
    std::shared_ptr<RSOffscreenRenderCallback> callback = std::make_shared<RSOffscreenRenderCallback>();
    auto renderServiceClient = std::make_unique<RSRenderServiceClient>();
    renderServiceClient->TakeSurfaceCapture(node.GetId(), callback, scaleX_, scaleY_);
    std::shared_ptr<Media::PixelMap> pixelMap = callback->GetResult(2000); // wait for <= 2000ms
    if (pixelMap == nullptr){
        ROSEN_LOGE("RSUIDirector::LocalCapture failed to get pixelmap, return nullptr!");
    }
    // draw pixelmap in canvas
    auto image = Media::PixelMapRosenUtils::ExtractSkImage(pixelMap);
    canvas_->drawImage(image, node.GetRenderProperties().GetBoundsPositionX(), node.GetRenderProperties().GetBoundsPositionY());
}

} // namespace Rosen
} // namespace OHOS