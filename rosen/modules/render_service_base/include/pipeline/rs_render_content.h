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
#ifndef RENDER_SERVICE_BASE_CORE_PIPELINE_RS_RENDER_CONTENT_H
#define RENDER_SERVICE_BASE_CORE_PIPELINE_RS_RENDER_CONTENT_H

#include <sys/types.h>

#include "common/rs_macros.h"
#include "memory/rs_dfx_string.h"
#include "modifier/rs_render_modifier.h"
#include "property/rs_properties.h"
namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSRenderContent {
public:
    RSRenderContent() = default;
    RSProperties& GetMutableRenderProperties();
    const RSProperties& GetRenderProperties() const;
    std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>>& GetMutableDrawCmdModifiers();
    const std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>>& GetDrawCmdModifiers() const;
    void AddDrawCmdModifier(RSModifierType modifierType, const std::shared_ptr<RSRenderModifier> modifier);
    void RemoveDrawCmdModifier(const PropertyId& id);
    void RemoveDrawCmdModifier(const pid_t pid);
    std::shared_ptr<RSRenderModifier> GetDrawCmdModifier(const PropertyId& id);
    void ApplyModifiers();
    void ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type);

    void ResizeDrawableVec();
    void UpdateDrawableVec(RSPropertyDrawableGenerateContext drawableContext, Slot::RSPropertyDrawableSlot dirtySlots);
    void IterateOnDrawableRange(RSPropertyDrawableSlot begin, RSPropertyDrawableSlot end,
        RSPaintFilterCanvas& canvas)

    void DumpNodeInfo(DfxString& log);
private:
    uint8_t drawableVecStatus_ = 0;
    RSProperties renderProperties_;
    std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>> drawCmdModifiers_;
    std::vector<std::unique_ptr<RSPropertyDrawable>> propertyDrawablesVec_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_CORE_PIPELINE_RS_RENDER_CONTENT_H