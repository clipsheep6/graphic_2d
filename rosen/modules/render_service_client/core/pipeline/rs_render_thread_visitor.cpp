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

#include "pipeline/rs_render_thread_visitor.h"

#include <include/core/SkColor.h>
#include <include/core/SkFont.h>
#include <include/core/SkMatrix.h>
#include <include/core/SkPaint.h>

#include "rs_trace.h"

#include "command/rs_base_node_command.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "include/core/SkRect.h"
#include "rs_trace.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"

namespace {
bool CompareVector(const std::vector<OHOS::Rosen::NodeId>& a, const std::vector<OHOS::Rosen::NodeId>& b)
{
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}
} // namespace

namespace OHOS {
namespace Rosen {
RSRenderThreadVisitor::RSRenderThreadVisitor() : canvas_(nullptr) {}

RSRenderThreadVisitor::~RSRenderThreadVisitor() {}

void RSRenderThreadVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSRenderThreadVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    if (isIdle_) {
        dirtyManager_.Clear();
        dirtyFlag_ = false;
        isIdle_ = false;
        PrepareCanvasRenderNode(node);
        isIdle_ = true;
    } else {
        PrepareCanvasRenderNode(node);
    }
}

void RSRenderThreadVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    bool dirtyFlag = dirtyFlag_;
    dirtyFlag_ = node.Update(dirtyManager_, nullptr, dirtyFlag_);
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSRenderThreadVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    bool dirtyFlag = dirtyFlag_;
    dirtyFlag_ = node.Update(dirtyManager_, nullptr, dirtyFlag_);
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSRenderThreadVisitor::ProcessBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSRenderThreadVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!isIdle_) {
        ProcessCanvasRenderNode(node);
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGE("No valid RSRootRenderNode");
        return;
    }
    if (node.GetSurfaceWidth() <= 0 || node.GetSurfaceHeight() <= 0) {
        ROSEN_LOGE("RSRootRenderNode have negative width or height [%d %d]", node.GetSurfaceWidth(),
            node.GetSurfaceHeight());
        return;
    }
    auto ptr = RSNodeMap::Instance().GetNode<RSSurfaceNode>(node.GetRSSurfaceNodeId());
    if (ptr == nullptr) {
        ROSEN_LOGE("No valid RSSurfaceNode");
        return;
    }

    auto surfaceNodeColorSpace = ptr->GetColorSpace();

    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(ptr);
    if (rsSurface == nullptr) {
        ROSEN_LOGE("No RSSurface found");
        return;
    }

    auto rsSurfaceColorSpace = rsSurface->GetColorSpace();

    if (surfaceNodeColorSpace != rsSurfaceColorSpace) {
        ROSEN_LOGD("Set new colorspace %d to rsSurface", surfaceNodeColorSpace);
        rsSurface->SetColorSpace(surfaceNodeColorSpace);
    }

#ifdef ACE_ENABLE_GL
    RenderContext* rc = RSRenderThread::Instance().GetRenderContext();
    rsSurface->SetRenderContext(rc);
#endif
    RS_TRACE_BEGIN("rsSurface->RequestFrame");
    auto surfaceFrame = rsSurface->RequestFrame(node.GetSurfaceWidth(), node.GetSurfaceHeight());
    RS_TRACE_END();
    if (surfaceFrame == nullptr) {
        ROSEN_LOGE("Request Frame Failed");
        return;
    }

    sk_sp<SkSurface> skSurface = nullptr;
    auto iter = forceRasterNodes.find(node.GetId());
    if (iter != forceRasterNodes.end()) {
        forceRasterNodes.erase(iter);
        SkImageInfo imageInfo = SkImageInfo::Make(node.GetSurfaceWidth(), node.GetSurfaceHeight(),
            kRGBA_8888_SkColorType, kOpaque_SkAlphaType, SkColorSpace::MakeSRGB());
        skSurface = SkSurface::MakeRaster(imageInfo);
        canvas_ = new RSPaintFilterCanvas(skSurface.get());
    } else {
        canvas_->clipRect(SkRect::MakeWH(node.GetSurfaceWidth(), node.GetSurfaceHeight()));
        auto skSurface = surfaceFrame->GetSurface();
        canvas_ = new RSPaintFilterCanvas(skSurface.get());
    }
    canvas_->clear(SK_ColorTRANSPARENT);
    isIdle_ = false;

    // clear current children before traversal, we will re-add them again during traversal
    childSurfaceNodeIds_.clear();

    // reset matrix
    parentSurfaceNodeMatrix_ = SkMatrix::I();

    ProcessCanvasRenderNode(node);

    if (!CompareVector(childSurfaceNodeIds_, node.childSurfaceNodeIds_)) {
        auto thisSurfaceNodeId = node.GetRSSurfaceNodeId();
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(thisSurfaceNodeId);
        SendCommandFromRT(command);
        for (const auto& childSurfaceNodeId : childSurfaceNodeIds_) {
            command = std::make_unique<RSBaseNodeAddChild>(thisSurfaceNodeId, childSurfaceNodeId, -1);
            SendCommandFromRT(command);
        }
        node.childSurfaceNodeIds_ = std::move(childSurfaceNodeIds_);
    }

    if (skSurface) {
        canvas_->flush();
        surfaceFrame->GetCanvas()->clear(SK_ColorTRANSPARENT);
        skSurface->draw(surfaceFrame->GetCanvas(), 0.f, 0.f, nullptr);
    }
    if (RSRootRenderNode::NeedForceRaster()) {
        RSRootRenderNode::MarkForceRaster(false);
        forceRasterNodes.insert(node.GetId());
        if (!skSurface) {
            RSRenderThread::Instance().RequestNextVSync();
        }
    }

    RS_TRACE_BEGIN("rsSurface->FlushFrame");
    rsSurface->FlushFrame(surfaceFrame);
    RS_TRACE_END();

    delete canvas_;
    canvas_ = nullptr;

    isIdle_ = true;
}

void RSRenderThreadVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
        return;
    }
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSRenderThreadVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGI("RSRenderThreadVisitor::ProcessSurfaceRenderNode node : %llu is invisible", node.GetId());
        return;
    }
    // RSSurfaceRenderNode in RSRenderThreadVisitor do not have information of property.
    // We only get parent's matrix and send it to RenderService
#ifdef ROSEN_OHOS
    SkMatrix invertMatrix;
    SkMatrix contextMatrix = canvas_->getTotalMatrix();

    if (parentSurfaceNodeMatrix_.invert(&invertMatrix)) {
        contextMatrix.preConcat(parentSurfaceNodeMatrix_);
    } else {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessSurfaceRenderNode, invertMatrix failed");
    }
    node.SetContextMatrix(contextMatrix);
    node.SetContextAlpha(canvas_->GetAlpha());
    // for proxied nodes (i.e. remote window components), we only set matrix & alpha, do not change its hierarchy and
    // clip status.
    if (node.IsProxy()) {
        ProcessBaseRenderNode(node);
        return;
    }
    node.SetContextClipRegion(canvas_->getLocalClipBounds());

    // clip hole
    ClipHoleForSurfaceNode(node);
#endif
    // 1. add this node to parent's children list
    childSurfaceNodeIds_.emplace_back(node.GetId());

    // 2. backup and reset environment variables before traversal children
    std::vector<NodeId> siblingSurfaceNodeIds(std::move(childSurfaceNodeIds_));
    childSurfaceNodeIds_.clear();
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    parentSurfaceNodeMatrix_ = canvas_->getTotalMatrix();

    // 3. traversal children, child surface node will be added to childSurfaceNodeIds_
    // note: apply current node properties onto canvas if there is any child node
    ProcessBaseRenderNode(node);

    // 4. if children changed, sync children to RenderService
    if (!CompareVector(childSurfaceNodeIds_, node.childSurfaceNodeIds_)) {
        auto thisSurfaceNodeId = node.GetId();
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(thisSurfaceNodeId);
        SendCommandFromRT(command);
        for (const auto& childSurfaceNodeId : childSurfaceNodeIds_) {
            command = std::make_unique<RSBaseNodeAddChild>(thisSurfaceNodeId, childSurfaceNodeId, -1);
            SendCommandFromRT(command);
        }
        node.childSurfaceNodeIds_ = std::move(childSurfaceNodeIds_);
    }

    // 5. restore environments variables before continue traversal siblings
    childSurfaceNodeIds_ = std::move(siblingSurfaceNodeIds);
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
}

const Vector4f CalSrcRectRatio(const SkRect& originRect, const SkRect& resRect)
{
    float x = std::max((resRect.left() - originRect.left()) / originRect.width(), 0.0f);
    float y = std::max((resRect.top() - originRect.top()) / originRect.height(), 0.0f);
    float width = std::min(resRect.width() / originRect.width(), 1.0f);
    float height = std::min(resRect.height() / originRect.height(), 1.0f);
    return Vector4f(x, y, width, height);
}

void RSRenderThreadVisitor::ClipHoleForSurfaceNode(RSSurfaceRenderNode& node)
{
#ifdef ROSEN_OHOS
    auto x = node.GetRenderProperties().GetBoundsPositionX();
    auto y = node.GetRenderProperties().GetBoundsPositionY();
    auto width = node.GetRenderProperties().GetBoundsWidth();
    auto height = node.GetRenderProperties().GetBoundsHeight();
    canvas_->save();
    SkRect originRect = SkRect::MakeXYWH(x, y, width, height);
    canvas_->clipRect(originRect);
    SkRect resRect = canvas_->getLocalClipBounds();
    auto ratio = CalSrcRectRatio(originRect, resRect);
    node.SetSrcRatio(ratio);
    if (node.IsNotifyRTBufferAvailable() == true) {
        canvas_->clear(SK_ColorTRANSPARENT);
    } else {
        ROSEN_LOGI("RSRenderThreadVisitor::ProcessSurfaceRenderNode node : %llu, not clip [%f, %f, %f, %f]",
            node.GetId(), x, y, width, height);
        auto backgroundColor = node.GetRenderProperties().GetBackgroundColor();
        if (backgroundColor != RgbPalette::Transparent()) {
            canvas_->clear(backgroundColor.AsArgbInt());
        } else {
            canvas_->clear(SK_ColorBLACK);
        }
    }
    canvas_->restore();
#endif
}

void RSRenderThreadVisitor::SendCommandFromRT(std::unique_ptr<RSCommand>& command)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommandFromRT(command);
    }
}
} // namespace Rosen
} // namespace OHOS
