/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "property/rs_property_drawable_utilities.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_single_frame_composer.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen {
// ============================================================================
// alias (reference or soft link) of another drawable
RSAliasDrawable::RSAliasDrawable(RSPropertyDrawableSlot slot) : slot_(slot) {}
void RSAliasDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    content.DrawPropertyDrawable(slot_, canvas);
}

// ============================================================================
// Save and Restore
RSSaveDrawable::RSSaveDrawable(std::shared_ptr<int> content) : content_(std::move(content)) {}
void RSSaveDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
#ifndef USE_ROSEN_DRAWING
    *content_ = canvas.save();
#else
    *content_ = canvas.GetSaveCount();
    canvas.Save();
#endif
}

RSRestoreDrawable::RSRestoreDrawable(std::shared_ptr<int> content) : content_(std::move(content)) {}
void RSRestoreDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
#ifndef USE_ROSEN_DRAWING
    canvas.restoreToCount(*content_);
#else
    canvas.RestoreToCount(*content_);
#endif
}

RSCustomSaveDrawable::RSCustomSaveDrawable(
    std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content, RSPaintFilterCanvas::SaveType type)
    : content_(std::move(content)), type_(type)
{}
void RSCustomSaveDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
#ifndef USE_ROSEN_DRAWING
    *content_ = canvas.Save(type_);
#else
    *content_ = canvas.SaveAllStatus(type_);
#endif
}

RSCustomRestoreDrawable::RSCustomRestoreDrawable(std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content)
    : content_(std::move(content))
{}
void RSCustomRestoreDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    canvas.RestoreStatus(*content_);
}

// ============================================================================
// Adapter for RSRenderModifier
RSModifierDrawable::RSModifierDrawable(RSModifierType type) : type_(type) {}
void RSModifierDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    auto itr = content.drawCmdModifiers_.find(type_);
    if (itr == content.drawCmdModifiers_.end() || itr->second.empty()) {
        return;
    }
    // temporary fix, will refactor RSRenderModifier::Apply to workaround this issue
    RSModifierContext context = { const_cast<RSRenderContent&>(content).renderProperties_, &canvas };
    // PLANNING: add back single-frame compose
    for (const auto& modifier : itr->second) {
        modifier->Apply(context);
    }
}

// ============================================================================
// Alpha
RSAlphaDrawable::RSAlphaDrawable(float alpha) : RSPropertyDrawable(), alpha_(alpha) {}
void RSAlphaDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
    canvas.MultiplyAlpha(alpha_);
}
RSPropertyDrawable::DrawablePtr RSAlphaDrawable::Generate(const RSRenderContent& content)
{
    auto alpha = content.GetRenderProperties().GetAlpha();
    if (alpha == 1) {
        return nullptr;
    }
    return content.GetRenderProperties().GetAlphaOffscreen() ? std::make_unique<RSAlphaOffscreenDrawable>(alpha)
                                                   : std::make_unique<RSAlphaDrawable>(alpha);
}

RSAlphaOffscreenDrawable::RSAlphaOffscreenDrawable(float alpha) : RSAlphaDrawable(alpha) {}
void RSAlphaOffscreenDrawable::Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const
{
#ifndef USE_ROSEN_DRAWING
    auto rect = RSPropertiesPainter::Rect2SkRect(content.GetRenderProperties().GetBoundsRect());
    canvas.saveLayerAlpha(&rect, std::clamp(alpha_, 0.f, 1.f) * UINT8_MAX);
#else
    auto rect = RSPropertiesPainter::Rect2DrawingRect(content.GetRenderProperties().GetBoundsRect());
    Drawing::Brush brush;
    brush.SetAlphaF(std::clamp(alpha_, 0.f, 1.f) * UINT8_MAX);
    Drawing::SaveLayerOps slr(&rect, &brush);
    canvas.SaveLayer(slr);
#endif
}
} // namespace OHOS::Rosen
