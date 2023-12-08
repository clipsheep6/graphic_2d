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
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkCanvas.h"
#else
#include "recording/recording_canvas.h"
#endif
#include "memory/rs_memory_track.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "render/rs_blur_filter.h"
#include "render/rs_light_up_effect_filter.h"
#include "platform/common/rs_log.h"
#include "visitor/rs_node_visitor.h"
#include "property/rs_property_drawable.h"

namespace OHOS {
namespace Rosen {
using Slot::RSPropertyDrawableSlot;
namespace {
constexpr PropertyId ANONYMOUS_MODIFIER_ID = 0;
}

#ifndef USE_ROSEN_DRAWING
static inline SkBlendMode ConvertToSkBlendMode(int blendMode)
{
    static const std::unordered_map<int, SkBlendMode> skBlendModeLUT = {
        {static_cast<int>(RSColorBlendModeType::DST_IN), SkBlendMode::kDstIn},
        {static_cast<int>(RSColorBlendModeType::SRC_IN), SkBlendMode::kSrcIn}
    };

    auto iter = skBlendModeLUT.find(blendMode);
    if (iter == skBlendModeLUT.end()) {
        ROSEN_LOGE("The desired color_blend_mode is undefined, and the SkBlendMode::kSrc is used.");
        return SkBlendMode::kSrc;
    }

    return skBlendModeLUT.at(blendMode);
}
#else
static inline Drawing::BlendMode ConvertToBlendMode(int blendMode)
{
    static const std::unordered_map<int, Drawing::BlendMode> blendModeLUT = {
        {static_cast<int>(RSColorBlendModeType::DST_IN), Drawing::BlendMode::DST_IN},
        {static_cast<int>(RSColorBlendModeType::SRC_IN), Drawing::BlendMode::SRC_IN}
    };

    auto iter = blendModeLUT.find(blendMode);
    if (iter == blendModeLUT.end()) {
        ROSEN_LOGE("The desired color_blend_mode is undefined, and the Drawing::BlendMode::SRC is used.");
        return Drawing::BlendMode::SRC;
    }

    return blendModeLUT.at(blendMode);
}
#endif

static inline void EnableColorBlendModeFilterLayer(RSPaintFilterCanvas& canvas, int& colorBlendMode)
{
    if (colorBlendMode != static_cast<int>(RSColorBlendModeType::NONE)) {
#ifndef USE_ROSEN_DRAWING
        canvas.saveLayer(nullptr, nullptr);
#else
        canvas.SaveLayer({nullptr, nullptr});
#endif
        canvas.SaveAlpha();
        canvas.SetAlpha(1.0f);
    }
}

static inline void EnableColorBlendModeMaskLayer(RSPaintFilterCanvas& canvas, int& colorBlendMode)
{
    if (colorBlendMode != static_cast<int>(RSColorBlendModeType::NONE)) {
#ifndef USE_ROSEN_DRAWING
        SkBlendMode skBlendMode = ConvertToSkBlendMode(colorBlendMode);
        SkPaint maskPaint;
        maskPaint.setBlendMode(skBlendMode);
        SkCanvas::SaveLayerRec maskLayerRec(nullptr, &maskPaint, nullptr, 0);
        canvas.saveLayer(maskLayerRec);
#else
        Drawing::BlendMode blendMode = ConvertToBlendMode(colorBlendMode);
        Drawing::Brush brush;
        brush.SetBlendMode(blendMode);
        Drawing::SaveLayerOps maskLayerRec(nullptr, &brush, nullptr, 0);
        canvas.SaveLayer(maskLayerRec);
#endif
    }
}

RSCanvasRenderNode::RSCanvasRenderNode(NodeId id, const std::weak_ptr<RSContext>& context) : RSRenderNode(id, context)
{
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
}

RSCanvasRenderNode::~RSCanvasRenderNode()
{
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
}

#ifndef USE_ROSEN_DRAWING
void RSCanvasRenderNode::UpdateRecording(std::shared_ptr<DrawCmdList> drawCmds,
    RSModifierType type, bool isSingleFrameComposer)
{
    if (!drawCmds || drawCmds->GetSize() == 0) {
        return;
    }
    auto renderProperty = std::make_shared<RSRenderProperty<DrawCmdListPtr>>(drawCmds, ANONYMOUS_MODIFIER_ID);
    auto renderModifier = std::make_shared<RSDrawCmdListRenderModifier>(renderProperty);
    renderModifier->SetType(type);
    AddModifier(renderModifier, isSingleFrameComposer);
}
#else
void RSCanvasRenderNode::UpdateRecording(std::shared_ptr<Drawing::DrawCmdList> drawCmds,
    RSModifierType type, bool isSingleFrameComposer)
{
    if (!drawCmds || drawCmds->IsEmpty()) {
        return;
    }
    auto renderProperty = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(drawCmds, ANONYMOUS_MODIFIER_ID);
    auto renderModifier = std::make_shared<RSDrawCmdListRenderModifier>(renderProperty);
    renderModifier->SetType(type);
    AddModifier(renderModifier, isSingleFrameComposer);
}
#endif

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

void RSCanvasRenderNode::OnTreeStateChanged()
{
    if (!IsOnTheTree()) {
        // clear node groups cache when node is removed from tree
        if (GetCacheType() == CacheType::CONTENT) {
            SetCacheType(CacheType::NONE);
            ClearCacheSurfaceInThread();
            SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        }
    }
    RSRenderNode::OnTreeStateChanged();
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
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        IterateOnDrawableRange(RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::MASK, canvas);
        return;
    }
    RSRenderNode::ProcessTransitionBeforeChildren(canvas);
}

void RSCanvasRenderNode::ProcessShadowBatching(RSPaintFilterCanvas& canvas)
{
    RSAutoCanvasRestore acr(&canvas);
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        IterateOnDrawableRange(
            RSPropertyDrawableSlot::BOUNDS_MATRIX, RSPropertyDrawableSlot::TRANSITION, canvas);
        IterateOnDrawableRange(
            RSPropertyDrawableSlot::SHADOW, RSPropertyDrawableSlot::SHADOW, canvas);
        return;
    }
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyBoundsGeometry(canvas);
    ApplyAlpha(canvas);
    RSPropertiesPainter::DrawMask(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawShadow(GetRenderProperties(), canvas);
}

void RSCanvasRenderNode::ProcessAnimatePropertyBeforeChildrenWithDrawable(RSPaintFilterCanvas& canvas)
{
    auto parent = GetParent().lock();
    if (RSSystemProperties::GetUseShadowBatchingEnabled() &&
        parent && parent->GetRenderProperties().GetUseShadowBatching()) {
        IterateOnDrawableRange(
            RSPropertyDrawableSlot::TRANSITION, RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR, canvas);
        IterateOnDrawableRange(
            RSPropertyDrawableSlot::SAVE_LAYER_BACKGROUND, RSPropertyDrawableSlot::CLIP_TO_FRAME, canvas);
    } else {
        IterateOnDrawableRange(
            RSPropertyDrawableSlot::TRANSITION, RSPropertyDrawableSlot::CLIP_TO_FRAME, canvas);
    }
}

void RSCanvasRenderNode::ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        ProcessAnimatePropertyBeforeChildrenWithDrawable(canvas);
        return;
    }
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::TRANSITION);
    ApplyDrawCmdModifier(context, RSModifierType::ENV_FOREGROUND_COLOR);
    
    if (RSSystemProperties::GetUseShadowBatchingEnabled()) {
        auto parent = GetParent().lock();
        if (!(parent && parent->GetRenderProperties().GetUseShadowBatching())) {
            RSPropertiesPainter::DrawShadow(GetRenderProperties(), canvas);
        }
    } else {
        RSPropertiesPainter::DrawShadow(GetRenderProperties(), canvas);
    }
    // Inter-UI component blur & blending effect -- An empty layer
    int colorBlendMode = GetRenderProperties().GetColorBlendMode();
    EnableColorBlendModeFilterLayer(canvas, colorBlendMode);
    // In NEW_SKIA version, L96 code will cause dump if the 3rd parameter is true.
#ifdef NEW_SKIA
    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas, false);
#else
    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas);
#endif

    if (canvas.GetCacheType() != RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        if (GetRenderProperties().GetUseEffect()) {
            RSPropertiesPainter::ApplyBackgroundEffect(GetRenderProperties(), canvas);
        }
        RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, FilterType::BACKGROUND_FILTER);
    }

    ApplyDrawCmdModifier(context, RSModifierType::BACKGROUND_STYLE);

    if (GetRenderProperties().IsDynamicLightUpValid()) {
        RSPropertiesPainter::DrawDynamicLightUp(GetRenderProperties(), canvas);
    }

#ifndef USE_ROSEN_DRAWING
    canvasNodeSaveCount_ = canvas.Save();
#else
    canvasNodeSaveCount_ = canvas.SaveAllStatus();
#endif
    ApplyDrawCmdModifier(context, RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY);
#ifndef USE_ROSEN_DRAWING
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#else
    canvas.Translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#endif

    if (GetRenderProperties().GetClipToFrame()) {
    // In NEW_SKIA version, L116 code will cause dump if the 3rd parameter is true.
#ifdef NEW_SKIA
        RSPropertiesPainter::Clip(canvas, GetRenderProperties().GetFrameRect(), false);
#else
        RSPropertiesPainter::Clip(canvas, GetRenderProperties().GetFrameRect());
#endif
    }
    // Inter-UI component blur & blending effect -- A Mask layer
    EnableColorBlendModeMaskLayer(canvas, colorBlendMode);
}

void RSCanvasRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        IterateOnDrawableRange(RSPropertyDrawableSlot::CONTENT_STYLE, RSPropertyDrawableSlot::CONTENT_STYLE, canvas);
        return;
    }
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);
}

void RSCanvasRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        IterateOnDrawableRange(RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::CLIP_TO_FRAME, canvas);
        return;
    }
    ProcessTransitionBeforeChildren(canvas);
    ProcessAnimatePropertyBeforeChildren(canvas);
}

void RSCanvasRenderNode::ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        IterateOnDrawableRange(
            RSPropertyDrawableSlot::FOREGROUND_STYLE, RSPropertyDrawableSlot::PARTICLE_EFFECT, canvas);
        return;
    }
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::FOREGROUND_STYLE);

    auto& aiInvert = GetRenderProperties().GetAiInvert();
    if (aiInvert.has_value()) {
        RSPropertiesPainter::DrawBinarizationShader(GetRenderProperties(), canvas);
    }
    RSPropertiesPainter::DrawColorFilter(GetRenderProperties(), canvas);

    canvas.RestoreStatus(canvasNodeSaveCount_);
    int colorBlendMode = GetRenderProperties().GetColorBlendMode();
    if (colorBlendMode != static_cast<int>(RSColorBlendModeType::NONE)) {
        canvas.RestoreAlpha();
#ifndef USE_ROSEN_DRAWING
        canvas.restore();
#else
        canvas.Restore();
#endif
    }
    if (GetRenderProperties().IsLightUpEffectValid()) {
        RSPropertiesPainter::DrawLightUpEffect(GetRenderProperties(), canvas);
    }
    RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, FilterType::FOREGROUND_FILTER);
    auto para = GetRenderProperties().GetLinearGradientBlurPara();
    RSPropertiesPainter::DrawLinearGradientBlurFilter(GetRenderProperties(), canvas);

    auto illuminatedPtr_ = GetRenderProperties().GetIlluminated();
    if (illuminatedPtr_ && illuminatedPtr_->IsIlluminated() && ROSEN_EQ(GetRenderProperties().GetBloom(), 0.f)) {
#ifndef USE_ROSEN_DRAWING
        RSPropertiesPainter::DrawLight(GetRenderProperties(), canvas);
#else
// Drawing is not supported
#endif
    }
    RSPropertiesPainter::DrawBorder(GetRenderProperties(), canvas);
    ApplyDrawCmdModifier(context, RSModifierType::OVERLAY_STYLE);
    RSPropertiesPainter::DrawForegroundColor(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawParticle(GetRenderProperties(), canvas);
}

void RSCanvasRenderNode::ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        IterateOnDrawableRange(RSPropertyDrawableSlot::PIXEL_STRETCH, RSPropertyDrawableSlot::RESTORE_ALL, canvas);
        return;
    }
    RSPropertiesPainter::DrawPixelStretch(GetRenderProperties(), canvas);
    RSRenderNode::ProcessRenderAfterChildren(canvas);
}

void RSCanvasRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        IterateOnDrawableRange(RSPropertyDrawableSlot::FOREGROUND_STYLE, RSPropertyDrawableSlot::RESTORE_ALL, canvas);
        return;
    }
    ProcessAnimatePropertyAfterChildren(canvas);
    ProcessTransitionAfterChildren(canvas);
    canvas.RestoreEnv();
}

void RSCanvasRenderNode::ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type)
{
    auto itr = drawCmdModifiers_.find(type);
    if (itr == drawCmdModifiers_.end() || itr->second.empty()) {
        return;
    }

    if (RSSystemProperties::GetSingleFrameComposerEnabled()) {
        bool needSkip = false;
        if (GetNodeIsSingleFrameComposer() && singleFrameComposer_ != nullptr) {
            needSkip = singleFrameComposer_->SingleFrameModifierAddToList(type, itr->second);
        }
        for (const auto& modifier : itr->second) {
            if (singleFrameComposer_ != nullptr && singleFrameComposer_->SingleFrameIsNeedSkip(needSkip, modifier)) {
                continue;
            }
            modifier->Apply(context);
        }
    } else {
        for (const auto& modifier : itr->second) {
            modifier->Apply(context);
        }
    }
}

void RSCanvasRenderNode::InternalDrawContent(RSPaintFilterCanvas& canvas)
{
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
#ifndef USE_ROSEN_DRAWING
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#else
    canvas.Translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#endif

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

void RSCanvasRenderNode::ProcessDrivenBackgroundRender(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    RSRenderNode::ProcessRenderBeforeChildren(canvas);
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::TRANSITION);
    ApplyDrawCmdModifier(context, RSModifierType::ENV_FOREGROUND_COLOR);

    RSPropertiesPainter::DrawShadow(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, FilterType::BACKGROUND_FILTER);
    ApplyDrawCmdModifier(context, RSModifierType::BACKGROUND_STYLE);
    RSRenderNode::ProcessRenderAfterChildren(canvas);
#endif
}

void RSCanvasRenderNode::ProcessDrivenContentRender(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
#ifndef USE_ROSEN_DRAWING
    canvasNodeSaveCount_ = canvas.Save();
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#else
    canvasNodeSaveCount_ = canvas.SaveAllStatus();
    canvas.Translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#endif
    DrawDrivenContent(canvas);
#endif
}

void RSCanvasRenderNode::ProcessDrivenContentRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    // Unresolvable bug: Driven render do not support DrawFilter/DrawBorder/FOREGROUND_STYLE/OVERLAY_STYLE
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::FOREGROUND_STYLE);

    canvas.RestoreStatus(canvasNodeSaveCount_);
#endif
}

RectF RSCanvasRenderNode::GetDrivenContentClipFrameRect() const
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
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
#if defined(RS_ENABLE_DRIVEN_RENDER)
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
