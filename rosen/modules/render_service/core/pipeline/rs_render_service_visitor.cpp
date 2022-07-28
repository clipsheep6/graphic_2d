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

#include "pipeline/rs_render_service_visitor.h"

#include "common/rs_obj_abs_geometry.h"
#include "display_type.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_divided_render_util.h"
#include "rs_trace.h"
#include "platform/drawing/rs_surface.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

RSRenderServiceVisitor::RSRenderServiceVisitor(bool parallel) : mParallelEnable(parallel) {}

RSRenderServiceVisitor::~RSRenderServiceVisitor() {}

void RSRenderServiceVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSRenderServiceVisitor::ProcessBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    if (!mParallelEnable) {
        // clear SortedChildren, it will be generated again in next frame
        node.ResetSortedChildren();
    }
}

void RSRenderServiceVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    node.ApplyModifiers();
    if (node.IsMirrorDisplay()) {
        auto mirrorSource = node.GetMirrorSource();
        auto existingSource = mirrorSource.lock();
        if (!existingSource) {
            RS_LOGI("RSRenderServiceVisitor::PrepareDisplayRenderNode mirrorSource haven't existed");
            return;
        }
        PrepareBaseRenderNode(*existingSource);
    } else {
        PrepareBaseRenderNode(node);
    }
}

void RSRenderServiceVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    isSecurityDisplay_ = node.GetSecurityDisplay();
    RS_LOGD("RsDebug RSRenderServiceVisitor::ProcessDisplayRenderNode: nodeid:[%llu] screenid:[%llu] \
        isSecurityDisplay:[%s] child size:[%d] total size:[%d]", node.GetId(), node.GetScreenId(),
        isSecurityDisplay_ ? "true" : "false", node.GetChildrenCount(), node.GetSortedChildren().size());
    globalZOrder_ = 0.0f;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();
    ScreenInfo currScreenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    // skip frame according to skipFrameInterval value of SetScreenSkipFrameInterval interface
    if (node.SkipFrame(currScreenInfo.skipFrameInterval)) {
        return;
    }
    RS_TRACE_NAME("ProcessDisplayRenderNode[" + std::to_string(node.GetScreenId()) + "]");
    ScreenState state = currScreenInfo.state;
    switch (state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE:
                RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
            break;
        default:
            RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode State is unusual");
            return;
    }
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }
    auto mirrorNode = node.GetMirrorSource().lock();
    if (!processor_->Init(node.GetScreenId(), node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID)) {
        RS_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode: processor init failed!");
        return;
    }

    if (node.IsMirrorDisplay()) {
        auto mirrorSource = node.GetMirrorSource();
        auto existingSource = mirrorSource.lock();
        if (!existingSource) {
            RS_LOGI("RSRenderServiceVisitor::ProcessDisplayRenderNode mirrorSource haven't existed");
            return;
        }
        if (mParallelEnable) {
            ScreenRotation rotation = screenManager->GetRotation(node.GetScreenId());
            uint32_t boundWidth = currScreenInfo.width;
            uint32_t boundHeight = currScreenInfo.height;
            if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
                std::swap(boundWidth, boundHeight);
            }
            skCanvas_ = std::make_unique<SkCanvas>(boundWidth, boundHeight);
            canvas_ = std::make_shared<RSPaintFilterCanvas>(skCanvas_.get());
            canvas_->clipRect(SkRect::MakeWH(boundWidth, boundHeight));
        }
        ProcessBaseRenderNode(*existingSource);
    } else {
        ScreenRotation rotation = screenManager->GetRotation(node.GetScreenId());
        uint32_t boundWidth = currScreenInfo.width;
        uint32_t boundHeight = currScreenInfo.height;
        if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
            std::swap(boundWidth, boundHeight);
        }
        skCanvas_ = std::make_unique<SkCanvas>(boundWidth, boundHeight);
        canvas_ = std::make_shared<RSPaintFilterCanvas>(skCanvas_.get());
        canvas_->clipRect(SkRect::MakeWH(boundWidth, boundHeight));
        auto boundsGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
        if (boundsGeoPtr && boundsGeoPtr->IsNeedClientCompose()) {
            RSDividedRenderUtil::SetNeedClient(true);
            boundsGeoPtr->SetSize(boundWidth, boundHeight);
        } else {
            RSDividedRenderUtil::SetNeedClient(false);
        }
        processor_->SetBoundsGeometry(boundsGeoPtr);
        ProcessBaseRenderNode(node);
    }
    processor_->PostProcess();
}

void RSRenderServiceVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_LOGI("RSRenderServiceVisitor::PrepareSurfaceRenderNode node[%llu] prepare paused because of \
            security DisplayNode.", node.GetId());
        return;
    }
    node.ApplyModifiers();
    if (!node.GetRenderProperties().GetVisible()) {
        RS_LOGI("RSRenderServiceVisitor::PrepareSurfaceRenderNode node : %llu is invisible", node.GetId());
        return;
    }
    PrepareBaseRenderNode(node);
}

void RSRenderServiceVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!processor_) {
        RS_LOGE("RSRenderServiceVisitor::ProcessSurfaceRenderNode processor is nullptr");
        return;
    }
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_LOGI("RSRenderServiceVisitor::ProcessSurfaceRenderNode node[%llu] process paused because of \
            security DisplayNode.", node.GetId());
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        RS_LOGI("RSRenderServiceVisitor::ProcessSurfaceRenderNode node : %llu is invisible", node.GetId());
        return;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && RSSystemProperties::GetOcclusionEnabled()) {
        return;
    }
    if (mParallelEnable) {
        node.ParallelVisitLock();
    }
    node.SetOffset(offsetX_, offsetY_);
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.SetGlobalZOrder(globalZOrder_);
    globalZOrder_ = globalZOrder_ + 1;
    processor_->ProcessSurface(node);
    node.ProcessRenderAfterChildren(*canvas_);
    if (mParallelEnable) {
        node.ParallelVisitUnlock();
    }
}
} // namespace Rosen
} // namespace OHOS
