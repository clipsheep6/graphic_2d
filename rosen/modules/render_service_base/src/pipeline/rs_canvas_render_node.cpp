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
#include "include/core/SkCanvas.h"
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

RSCanvasRenderNode::RSCanvasRenderNode(NodeId id, std::weak_ptr<RSContext> context) : RSRenderNode(id, context) {}

RSCanvasRenderNode::~RSCanvasRenderNode() {}

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
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessCanvasRenderNode(*this);
}

void RSCanvasRenderNode::ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas)
{
    RSRenderNode::ProcessRenderBeforeChildren(canvas);
}

void RSCanvasRenderNode::ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas)
{
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::TRANSITION);

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    bool isDrivenVisitMode = IsMarkDriven() && IsDrivenVisitMode();
    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas, isDrivenVisitMode);
#else
    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas);
#endif
    auto filter = std::static_pointer_cast<RSSkiaFilter>(GetRenderProperties().GetBackgroundFilter());
    if (filter != nullptr) {
        RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, filter, nullptr, canvas.GetSurface());
    }
    ApplyDrawCmdModifier(context, RSModifierType::BACKGROUND_STYLE);

    canvasNodeSaveCount_ = canvas.SaveCanvasAndAlpha();
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (!isDrivenVisitMode) { // skip clip in driven render mode
        if (GetRenderProperties().GetClipToFrame()) {
            RSPropertiesPainter::Clip(canvas, GetRenderProperties().GetFrameRect());
        }
    }
#else
    if (GetRenderProperties().GetClipToFrame()) {
        RSPropertiesPainter::Clip(canvas, GetRenderProperties().GetFrameRect());
    }
#endif
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
    ProcessRenderContents(canvas);
}

void RSCanvasRenderNode::ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas)
{
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::FOREGROUND_STYLE);

    canvas.RestoreCanvasAndAlpha(canvasNodeSaveCount_);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(GetRenderProperties().GetFilter());
    std::shared_ptr<RSSkiaFilter> lightUpFilter = nullptr;
    if (GetRenderProperties().IsLightUpEffectValid()) {
        lightUpFilter = std::make_shared<RSLightUpEffectFilter>(GetRenderProperties().GetLightUpEffect());
    }
    auto composedFilter = RSSkiaFilter::Compose(filter, lightUpFilter);
    if (composedFilter != nullptr) {
        RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, composedFilter, nullptr, canvas.GetSurface());
    }
    RSPropertiesPainter::DrawBorder(GetRenderProperties(), canvas);
    ApplyDrawCmdModifier(context, RSModifierType::OVERLAY_STYLE);

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    bool isDrivenVisitMode = IsMarkDriven() && IsDrivenVisitMode();
    RSPropertiesPainter::DrawForegroundColor(GetRenderProperties(), canvas, isDrivenVisitMode);
#else
    RSPropertiesPainter::DrawForegroundColor(GetRenderProperties(), canvas);
#endif
}

void RSCanvasRenderNode::ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas)
{
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
}

void RSCanvasRenderNode::ApplyModifiers()
{
    RSRenderNode::ApplyModifiers();
    GetMutableRenderProperties().backref_ = ReinterpretCastTo<RSRenderNode>();
}

} // namespace Rosen
} // namespace OHOS
