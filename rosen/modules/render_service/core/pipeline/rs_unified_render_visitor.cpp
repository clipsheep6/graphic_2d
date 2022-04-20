/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_unified_render_visitor.h"

#include <surface.h>
#include <window.h>
#include <window_manager.h>
#include <window_option.h>
#include "render_context/render_context.h"

#include "common/rs_obj_abs_geometry.h"
#include "display_type.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_service_listener_impl.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "screen_manager/screen_types.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {

RSUnifiedRenderVisitor::RSUnifiedRenderVisitor() {}

RSUnifiedRenderVisitor::~RSUnifiedRenderVisitor() {}

void RSUnifiedRenderVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetChildren()) {
        if (auto c = child.lock()) {
            c->Prepare(shared_from_this());
        }
    }
    for (auto& child : node.GetDisappearingChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSUnifiedRenderVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    SortZOrder(node);
    PrepareBaseRenderNode(node);
}

void RSUnifiedRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (IsChildOfDisplayNode(node)) {
        auto currentGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
        if (currentGeoPtr != nullptr) {
            currentGeoPtr->UpdateByMatrixFromParent(nullptr);
            currentGeoPtr->UpdateByMatrixFromRenderThread(node.GetMatrix());
            currentGeoPtr->UpdateByMatrixFromSelf();
        }
        PrepareBaseRenderNode(node);
    } else {
        bool dirtyFlag = dirtyFlag_;
        dirtyFlag_ = node.Update(dirtyManager_, parent_ ? &(parent_->GetRenderProperties()) : nullptr, dirtyFlag_);
        PrepareBaseRenderNode(node);
        dirtyFlag_ = dirtyFlag;
    }
}

void RSUnifiedRenderVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    parent_ = nullptr;
    dirtyFlag_ = false;
    PrepareCanvasRenderNode(node);
}

void RSUnifiedRenderVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode &node)
{
    bool dirtyFlag = dirtyFlag_;
    dirtyFlag_ = node.Update(dirtyManager_, parent_ ? &(parent_->GetRenderProperties()) : nullptr, dirtyFlag_);
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSUnifiedRenderVisitor::ProcessBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetChildren()) {
        if (auto c = child.lock()) {
            c->Process(shared_from_this());
        }
    }
    for (auto& child : node.GetDisappearingChildren()) {
        child->Process(shared_from_this());
    }
}

void RSUnifiedRenderVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    ROSEN_LOGI("RsDebug RSUnifiedRenderVisitor::ProcessDisplayRenderNode child size:%d", node.GetChildren().size());
    globalZOrder_ = 0.0f;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        ROSEN_LOGE("RSUnifiedRenderVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    switch (screenInfo_.state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ? RSDisplayRenderNode::CompositeType::COMPATIBLE_COMPOSITE
                : RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
            break;
        default:
            ROSEN_LOGE("RSUnifiedRenderVisitor::ProcessDisplayRenderNode State is unusual");
            return;
    }
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        ROSEN_LOGE("RSUnifiedRenderVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }
    processor_->Init(node.GetScreenId());

    std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
    auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNodePtr) {
        ROSEN_LOGE("RSDisplayRenderNode::CreateSurface ReinterpretCastTo fail");
        return;
    }
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListenerImpl(displayNodePtr);
    if (!node.CreateSurface(listener)) {
        ROSEN_LOGE("RSUnifiedRenderVisitor::ProcessDisplayRenderNode CreateSurface failed");
        return;
    }
    auto rsSurface = node.GetRSSurface();
    if (rsSurface == nullptr) {
        ROSEN_LOGE("RSUnifiedRenderVisitor::ProcessDisplayRenderNode No RSSurface found");
        return;
    }
#ifdef ACE_ENABLE_GL
    RenderContext* rc = RSRenderThread::Instance().GetRenderContext();
    rsSurface->SetRenderContext(rc);
#endif
    auto surfaceFrame = rsSurface->RequestFrame(screenInfo_.width, screenInfo_.height);
    if (surfaceFrame == nullptr) {
        ROSEN_LOGE("RSUnifiedRenderVisitor Request Frame Failed");
        return;
    }
    canvas_ = new RSPaintFilterCanvas(surfaceFrame->GetCanvas());
    canvas_->clear(SK_ColorTRANSPARENT);

    ProcessBaseRenderNode(node);

    rsSurface->FlushFrame(surfaceFrame);
    delete canvas_;
    canvas_ = nullptr;

    node.SetGlobalZOrder(globalZOrder_++);
    processor_->ProcessSurface(node);
    processor_->PostProcess();
}

void RSUnifiedRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    // for window surface node - whose parent is display node
    if (IsChildOfDisplayNode(node)) {
        if (node.GetRenderProperties().GetBoundsPositionX() >= screenInfo_.width ||
            node.GetRenderProperties().GetBoundsPositionY() >= screenInfo_.height) {
            ROSEN_LOGI("RsDebug RSUnifiedRenderVisitor::ProcessSurfaceRenderNode this node:%llu no need to composite",
                node.GetId());
            return;
        }
        if (!canvas_) {
            ROSEN_LOGE("RSUnifiedRenderVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
            return;
        }
        auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
        if (!geoPtr) {
            ROSEN_LOGI("RsDebug RSUnifiedRenderVisitor::ProcessSurfaceRenderNode node:%llu, get geoPtr failed",
                node.GetId());
            return;
        }
        canvas_->save();
        canvas_->setMatrix(geoPtr->GetAbsMatrix());
        ProcessBaseRenderNode(node);
        canvas_->restore();
    } else {
        canvas_->save();
        canvas_->clipRect(SkRect::MakeXYWH(
            node.GetRenderProperties().GetBoundsPositionX(), node.GetRenderProperties().GetBoundsPositionY(),
            node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight()));
        if (node.GetConsumer() != nullptr && node.GetBuffer() == nullptr) {
            ROSEN_LOGI("RSRenderThreadVisitor::ProcessSurfaceRenderNode buffer is not available, set black");
            canvas_->clear(SK_ColorBLACK);
        } else {
            OHOS::sptr<SurfaceBuffer> buffer;
            RSProcessor::SpecialTask task;
            if (!processor_->ConsumeAndUpdateBuffer(node, task, buffer)) {
                ROSEN_LOGI("RSRenderThreadVisitor::ProcessSurfaceRenderNode consume buffer fail");
                return;
            }
            ROSEN_LOGI("RSRenderThreadVisitor::ProcessSurfaceRenderNode draw buffer on canvas");
            DrawBufferOnCanvas(node);
        }
        canvas_->restore();
    }
}

void RSUnifiedRenderVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
        return;
    }
    if (!canvas_) {
        ROSEN_LOGE("RSUnifiedRenderVisitor::ProcessRootRenderNode, canvas is nullptr");
        return;
    }
    auto parent = node.GetParent().lock();
    if (!parent) {
        ROSEN_LOGE("RSUnifiedRenderVisitor::ProcessRootRenderNode parent is nullptr");
        return;
    }
    auto rootParent = parent->ReinterpretCastTo<RSSurfaceRenderNode>();
    if (!rootParent) {
        ROSEN_LOGI("RSUnifiedRenderVisitor::ProcessRootRenderNode this parent is not SurfaceNode");
        return;
    }
    canvas_->save();
    ProcessCanvasRenderNode(node);
    canvas_->restore();
}

void RSUnifiedRenderVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
        return;
    }
    if (!canvas_) {
        ROSEN_LOGE("RSUnifiedRenderVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSUnifiedRenderVisitor::SortZOrder(RSBaseRenderNode& node)
{
    auto& children = node.GetChildren();
    static auto compare = [](std::weak_ptr<RSBaseRenderNode> first, std::weak_ptr<RSBaseRenderNode> second) -> bool {
        auto node1 = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(first.lock());
        auto node2 = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(second.lock());
        if (node1 == nullptr || node2 == nullptr) {
            return false;
        }
        return node1->GetRenderProperties().GetPositionZ() < node2->GetRenderProperties().GetPositionZ();
    };
    std::stable_sort(children.begin(), children.end(), compare);
}

bool RSUnifiedRenderVisitor::IsChildOfDisplayNode(RSBaseRenderNode& node)
{
    auto parent = node.GetParent().lock();
    if (!parent) {
        ROSEN_LOGE("RSUnifiedRenderVisitor::IsChildOfDisplayNode parent is nullptr");
        return false;
    }
    auto parentPtr = parent->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!parentPtr) {
        ROSEN_LOGI("RSUnifiedRenderVisitor::IsChildOfDisplayNode this parent is not DisplayNode");
        return false;
    }
    return true;
}

void RSUnifiedRenderVisitor::DrawBufferOnCanvas(RSSurfaceRenderNode& node)
{
    auto buffer = node.GetBuffer();
    SkColorType colorType = (buffer->GetFormat() == PIXEL_FMT_BGRA_8888) ?
        kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
    SkImageInfo imageInfo = SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(),
        colorType, kPremul_SkAlphaType);
    auto pixmap = SkPixmap(imageInfo, buffer->GetVirAddr(), buffer->GetStride());
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setAlphaf(node.GetAlpha() * node.GetRenderProperties().GetAlpha()); // ??
    SkBitmap bitmap;
    if (!bitmap.installPixels(pixmap)) {
        ROSEN_LOGE("RSUnifiedRenderVisitor::DrawBufferOnCanvas installPixels failed");
    }
    canvas_->save();
    const RSProperties& property = node.GetRenderProperties();
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(node.GetRenderProperties().GetBoundsPositionX(), node.GetRenderProperties().GetBoundsPositionY(),
            node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight());
        RSPropertiesPainter::SaveLayerForFilter(property, *canvas_, filter, skRectPtr);
        RSPropertiesPainter::RestoreForFilter(*canvas_);
    }
    canvas_->drawBitmapRect(bitmap,
        SkRect::MakeXYWH(0, 0,buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight()),
        SkRect::MakeXYWH(node.GetRenderProperties().GetBoundsPositionX(), node.GetRenderProperties().GetBoundsPositionY(),
            node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight()), &paint);
    canvas_->restore();
}
} // namespace Rosen
} // namespace OHOS


