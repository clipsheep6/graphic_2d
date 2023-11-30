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

#include "pipeline/rs_render_content.h"

namespace OHOS {
namespace Rosen {
RSProperties& RSRenderContent::GetMutableRenderProperties()
{
    return renderProperties_;
}

const RSProperties& RSRenderContent::GetRenderProperties() const
{
    return renderProperties_;
}

const std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>>& RSRenderContent::GetDrawCmdModifiers() const
{
    return drawCmdModifiers_;
}

std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>>& RSRenderContent::GetMutableDrawCmdModifiers()
{
    return drawCmdModifiers_;
}

void RSRenderContent::AddDrawCmdModifier(RSModifierType modifierType, const std::shared_ptr<RSRenderModifier> modifier)
{
    drawCmdModifiers_[modifierType].emplace_back(modifier);
}

void RSRenderContent::RemoveDrawCmdModifier(const PropertyId& id)
{
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        modifiers.remove_if([id](const auto& modifier) -> bool {
            return modifier ? modifier->GetPropertyId() == id : true;
        });
    }
}

std::shared_ptr<RSRenderModifier> RSRenderContent::GetDrawCmdModifier(const PropertyId& id)
{
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        auto it = std::find_if(modifiers.begin(), modifiers.end(),
            [id](const auto& modifier) -> bool { return modifier->GetPropertyId() == id; });
        if (it != modifiers.end()) {
            return *it;
        }
    }
    return nullptr;
}

void RSRenderContent::ApplyModifiers()
{
    RSModifierContext context = { renderProperties_ };
    for (auto& modifier : drawCmdModifiers_[RSModifierType::GEOMETRYTRANS]) {
        modifier->Apply(context);
    }
}

void RSRenderContent::RemoveDrawCmdModifier(const pid_t pid)
{
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        modifiers.remove_if(
            [pid](const auto& it) -> bool { return ExtractPid(it->GetPropertyId()) == pid; });
    }
}

void RSRenderContent::ResizeDrawableVec()
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        propertyDrawablesVec_.resize(RSPropertyDrawableSlot::MAX);
    }
}

void RSRenderContent::UpdateDrawableVec(RSPropertyDrawableGenerateContext drawableContext,
    Slot::RSPropertyDrawableSlot dirtySlots)
{
    // initialize necessary save/clip/restore
    if (drawableVecStatus_ == 0) {
        RSPropertyDrawable::InitializeSaveRestore(drawableContext, propertyDrawablesVec_);
    }
    // Update or regenerate drawable
    bool drawableChanged = RSPropertyDrawable::UpdateDrawableVec(drawableContext, propertyDrawablesVec_, dirtySlots);
    // if 1. first initialized or 2. any drawables changed, update save/clip/restore
    if (drawableChanged || drawableVecStatus_ == 0) {
        RSPropertyDrawable::UpdateSaveRestore(drawableContext, propertyDrawablesVec_, drawableVecStatus_);
    }
}

void RSRenderContent::IterateOnDrawableRange(RSPropertyDrawableSlot begin, RSPropertyDrawableSlot end,
        RSPaintFilterCanvas& canvas)
{
    for (uint16_t index = begin; index <= end; index++) {
        auto& drawablePtr = propertyDrawablesVec_[index];
        if (drawablePtr) {
            drawablePtr->Draw(*this, canvas);
        }
    }
}

void RSRenderContent::DumpNodeInfo(DfxString& log)
{
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        for (auto modifier : modifiers) {
            modifier->DumpPicture(log);
        }
    }
}

} // namespace Rosen
} // namespace OHOS
