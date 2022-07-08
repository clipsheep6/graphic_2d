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

#include "pipeline/rs_render_node.h"

#include <algorithm>

#include "animation/rs_render_animation.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_context.h"
#include "platform/common/rs_log.h"
#ifdef ROSEN_OHOS
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "property/rs_transition_properties.h"
#endif

namespace OHOS {
namespace Rosen {
RSRenderNode::RSRenderNode(NodeId id, std::weak_ptr<RSContext> context)
    : RSBaseRenderNode(id, context), renderProperties_(true)
{}

RSRenderNode::~RSRenderNode()
{
    FallbackAnimationsToRoot();
}

void RSRenderNode::FallbackAnimationsToRoot()
{
    if (animationManager_.animations_.empty()) {
        return;
    }

    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("Invalid context");
        return;
    }
    auto target = context->GetNodeMap().GetAnimationFallbackNode();
    if (!target) {
        ROSEN_LOGE("Failed to move animation to root, root render node is null!");
        return;
    }

    if (context != nullptr) {
        context->RegisterAnimatingRenderNode(target);
    }
    for (const auto& [animationId, animation] : animationManager_.animations_) {
        animation->Detach();
        target->animationManager_.AddAnimation(animation);
    }
}

bool RSRenderNode::Animate(int64_t timestamp)
{
    return animationManager_.Animate(timestamp);
}

bool RSRenderNode::Update(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty)
{
    // if switch to invisible and sign as dirty
    // update dirty rect to refresh corresponding region
    if (!renderProperties_.GetVisible() && !renderProperties_.IsDirty()) {
        return false;
    }
    bool dirty = renderProperties_.UpdateGeometry(parent, parentDirty);
    isDirtyRegionUpdated_ = false;
    UpdateDirtyRegion(dirtyManager, parentDirty);
    renderProperties_.ResetDirty();
    return dirty;
}

RSProperties& RSRenderNode::GetMutableRenderProperties()
{
    return renderProperties_;
}

const RSProperties& RSRenderNode::GetRenderProperties() const
{
    return renderProperties_;
}

void RSRenderNode::UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager, bool parentDirty)
{
    if (!IsDirty() && !parentDirty) {
        return;
    }
    auto dirtyRect = renderProperties_.GetDirtyRect();
    // filter invalid dirtyrect
    if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
        ROSEN_LOGD("RSRenderNode::UpdateDirtyRegion invalid dirtyRect = [%d, %d, %d, %d]",
            dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
    } else {
        dirtyManager.MergeDirtyRect(renderProperties_.GetDirtyRect());
        if (!oldDirty_.IsEmpty()) {
            dirtyManager.MergeDirtyRect(oldDirty_);
        }
        isDirtyRegionUpdated_ = (oldDirty_ == renderProperties_.GetDirtyRect());
        oldDirty_ = renderProperties_.GetDirtyRect();
    }
    SetClean();
}

bool RSRenderNode::IsDirty() const
{
    return RSBaseRenderNode::IsDirty() || renderProperties_.IsDirty();
}

void RSRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    saveCount_ = canvas.getSaveCount();
    canvas.save();
    canvas.SaveAlpha();
    canvas.MultiplyAlpha(GetRenderProperties().GetAlpha());
    auto boundsGeo = std::static_pointer_cast<RSObjAbsGeometry>(GetRenderProperties().GetBoundsGeometry());
    if (boundsGeo && !boundsGeo->IsEmpty()) {
        canvas.concat(boundsGeo->GetMatrix());
    }
    auto transitionProperties = GetAnimationManager().GetTransitionProperties();
    RSPropertiesPainter::DrawTransitionProperties(transitionProperties, GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawMask(GetRenderProperties(), canvas);
#endif
}

void RSRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    GetMutableRenderProperties().ResetBounds();
    canvas.RestoreAlpha();
    canvas.restoreToCount(saveCount_);
#endif
}
} // namespace Rosen
} // namespace OHOS
