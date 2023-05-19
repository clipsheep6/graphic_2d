/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_canvas_render_node.h"

#include <algorithm>
#include "modifier/rs_modifier_type.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_common_def.h"
#include "include/core/SkCanvas.h"
#include "memory/rs_memory_track.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "render/rs_blur_filter.h"
#include "render/rs_light_up_effect_filter.h"
#include "platform/common/rs_log.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr PropertyId ANONYMOUS_MODIFIER_ID = 0;
}

RSCanvasRenderNode::RSCanvasRenderNode(NodeId id, std::weak_ptr<RSContext> context) : RSRenderNode(id, context)
{
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
}

RSCanvasRenderNode::~RSCanvasRenderNode()
{
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
}

void RSCanvasRenderNode::UpdateRecording(std::shared_ptr<DrawCmdList> drawCmds, RSModifierType type)
{
    if (!drawCmds || drawCmds->GetSize() == 0) {
        return;
    }
    auto renderProperty = std::make_shared<RSRenderProperty<DrawCmdListPtr>>(drawCmds, ANONYMOUS_MODIFIER_ID);
    auto renderModifier = std::make_shared<RSDrawCmdListRenderModifier>(renderProperty);
    renderModifier->SetType(type);
    AddModifier(renderModifier);
}

void RSCanvasRenderNode::ClearRecording()
{
    RemoveModifier(ANONYMOUS_MODIFIER_ID);
}

void RSCanvasRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareCanvasRenderNode(*this);
}

void RSCanvasRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    std::unique_lock<std::mutex> lock(canvasNodeProcessMutex_);
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessCanvasRenderNode(*this);
}

void RSCanvasRenderNode::ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas)
{
    RSRenderNode::ProcessTransitionBeforeChildren(canvas);
}

void RSCanvasRenderNode::ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas)
{
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::TRANSITION);
    ApplyDrawCmdModifier(context, RSModifierType::ENV_FOREGROUND_COLOR);

    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(GetRenderProperties().GetBackgroundFilter());
    if (filter != nullptr) {
#ifndef NEW_SKIA
        RSTagTracker tagTracker(canvas.getGrContext(), GetId(), RSTagTracker::TAGTYPE::TAG_FILTER);
#endif
        RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, filter, nullptr, canvas.GetSurface());
    }
    ApplyDrawCmdModifier(context, RSModifierType::BACKGROUND_STYLE);

    canvasNodeSaveCount_ = canvas.Save();
    ApplyDrawCmdModifier(context, RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY);
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());

    if (GetRenderProperties().GetClipToFrame()) {
        RSPropertiesPainter::Clip(canvas, GetRenderProperties().GetFrameRect());
    }
}

void RSCanvasRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);
}

void RSCanvasRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    ProcessTransitionBeforeChildren(canvas);
    ProcessAnimatePropertyBeforeChildren(canvas);
}

void RSCanvasRenderNode::ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas)
{
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::FOREGROUND_STYLE);

    canvas.RestoreStatus(canvasNodeSaveCount_);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(GetRenderProperties().GetFilter());
    if (GetRenderProperties().IsLightUpEffectValid()) {
        std::shared_ptr<RSSkiaFilter> lightUpFilter =
            std::make_shared<RSLightUpEffectFilter>(GetRenderProperties().GetLightUpEffect());
        filter = filter ? filter->Compose(lightUpFilter) : lightUpFilter;
    }
    if (filter != nullptr) {
#ifndef NEW_SKIA
        RSTagTracker tagTracker(canvas.getGrContext(), GetId(), RSTagTracker::TAGTYPE::TAG_FILTER);
#endif
        RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, filter, nullptr, canvas.GetSurface());
    }
    RSPropertiesPainter::DrawBorder(GetRenderProperties(), canvas);
    ApplyDrawCmdModifier(context, RSModifierType::OVERLAY_STYLE);
    RSPropertiesPainter::DrawForegroundColor(GetRenderProperties(), canvas);
}

void RSCanvasRenderNode::ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas)
{
    RSPropertiesPainter::DrawPixelStretch(GetRenderProperties(), canvas);
    RSRenderNode::ProcessRenderAfterChildren(canvas);
}

void RSCanvasRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    ProcessAnimatePropertyAfterChildren(canvas);
    ProcessTransitionAfterChildren(canvas);
}

void RSCanvasRenderNode::ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type) const
{
    auto itr = drawCmdModifiers_.find(type);
    if (itr == drawCmdModifiers_.end() || itr->second.empty()) {
        return;
    }
    for (const auto& modifier : itr->second) {
        modifier->Apply(context);
    }
}

void RSCanvasRenderNode::InternalDrawContent(RSPaintFilterCanvas& canvas)
{
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());

    if (GetRenderProperties().GetClipToFrame()) {
        RSPropertiesPainter::Clip(canvas, GetRenderProperties().GetFrameRect());
    }
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);

    // temporary solution for drawing children
    for (auto& child : GetSortedChildren()) {
        if (auto canvasChild = ReinterpretCast<RSCanvasRenderNode>(child)) {
            canvasChild->InternalDrawContent(canvas);
        }
    }
}

void RSCanvasRenderNode::OnApplyModifiers()
{
    GetMutableRenderProperties().backref_ = ReinterpretCastTo<RSRenderNode>();
}

void RSCanvasRenderNode::ProcessDrivenBackgroundRender(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    RSRenderNode::ProcessRenderBeforeChildren(canvas);
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::TRANSITION);
    ApplyDrawCmdModifier(context, RSModifierType::ENV_FOREGROUND_COLOR);

    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(GetRenderProperties().GetBackgroundFilter());
    if (filter != nullptr) {
#ifndef NEW_SKIA
        RSTagTracker tagTracker(canvas.getGrContext(), GetId(), RSTagTracker::TAGTYPE::TAG_FILTER);
#endif
        RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, filter, nullptr, canvas.GetSurface());
    }
    ApplyDrawCmdModifier(context, RSModifierType::BACKGROUND_STYLE);
    RSRenderNode::ProcessRenderAfterChildren(canvas);
#endif
}

void RSCanvasRenderNode::ProcessDrivenContentRender(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
    DrawDrivenContent(canvas);
#endif
}

void RSCanvasRenderNode::ProcessDrivenContentRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    // Unresolvable bug: Driven render do not support DrawFilter/DrawBorder/FOREGROUND_STYLE/OVERLAY_STYLE
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::FOREGROUND_STYLE);

    GetMutableRenderProperties().ResetBounds();
#endif
}

RectF RSCanvasRenderNode::GetDrivenContentClipFrameRect() const
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    // temporary solution for driven content clip
    RectF rect;
    auto itr = drawCmdModifiers_.find(RSModifierType::CONTENT_STYLE);
    if (itr == drawCmdModifiers_.end() || itr->second.empty()) {
        return rect;
    }
    if (!itr->second.empty()) {
        auto drawCmdModifier = std::static_pointer_cast<RSDrawCmdListRenderModifier>(itr->second.front());
        if (drawCmdModifier != nullptr) {
            rect = drawCmdModifier->GetCmdsClipRect();
        }
    }
    return rect;
#else
    return RectF { 0.0f, 0.0f, 0.0f, 0.0f };
#endif
}

void RSCanvasRenderNode::DrawDrivenContent(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    auto itr = drawCmdModifiers_.find(RSModifierType::CONTENT_STYLE);
    if (itr == drawCmdModifiers_.end() || itr->second.empty()) {
        return;
    }
    int32_t index = 0;
    for (const auto& modifier : itr->second) {
        if (index == 0) {
            // temporary solution for driven content clip
            auto drawCmdModifier = std::static_pointer_cast<RSDrawCmdListRenderModifier>(modifier);
            if (drawCmdModifier != nullptr) {
                drawCmdModifier->ApplyForDrivenContent(context);
                index++;
                continue;
            }
        }
        if (modifier != nullptr) {
            modifier->Apply(context);
        }
        index++;
    }
#endif
}
} // namespace Rosen
} // namespace OHOS
