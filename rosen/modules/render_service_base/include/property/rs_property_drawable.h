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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_H

#include <functional>
#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

#include "modifier/rs_modifier_type.h"

namespace OHOS::Rosen {
class RSPaintFilterCanvas;
class RSProperties;
class RSPropertyDrawableGenerateContext;
class RSRenderModifier;
class RSRenderNode;

namespace Slot {
enum RSPropertyDrawableSlot : uint8_t {
    INVALID = 0,

    SAVE_ALL,

    // Bounds Geometry
    BOUNDS_MATRIX,
    ALPHA,
    MASK,
    TRANSITION,
    ENV_FOREGROUND_COLOR,
    SHADOW,

    // In Bounds Clip
    SAVE_BOUNDS,
    CLIP_TO_BOUNDS,
    BACKGROUND_COLOR,
    BACKGROUND_SHADER,
    BACKGROUND_IMAGE,
    BACKGROUND_FILTER,
    USE_EFFECT,
    BACKGROUND_STYLE,
    DYNAMIC_LIGHT_UP,
    ENV_FOREGROUND_COLOR_STRATEGY,
    EXTRA_RESTORE_BOUNDS,

    // Frame Geometry
    SAVE_FRAME,
    FRAME_OFFSET,
    CLIP_TO_FRAME,
    CONTENT_STYLE,
    CHILDREN,
    FOREGROUND_STYLE,
    COLOR_FILTER,
    RESTORE_FRAME,

    // In Bounds clip (again)
    EXTRA_SAVE_BOUNDS,
    EXTRA_CLIP_TO_BOUNDS,
    LIGHT_UP_EFFECT,
    FOREGROUND_FILTER,
    LINEAR_GRADIENT_BLUR_FILTER,
    BORDER,
    OVERLAY,
    FOREGROUND_COLOR,
    PARTICLE_EFFECT,
    PIXEL_STRETCH,
    RESTORE_BOUNDS,

    RESTORE_ALL,
    MAX,
};
}

// Pure virtual base class
class RSPropertyDrawable {
public:
    RSPropertyDrawable() = default;
    virtual ~RSPropertyDrawable() = default;

    // not copyable and moveable
    RSPropertyDrawable(const RSPropertyDrawable&) = delete;
    RSPropertyDrawable(const RSPropertyDrawable&&) = delete;
    RSPropertyDrawable& operator=(const RSPropertyDrawable&) = delete;
    RSPropertyDrawable& operator=(const RSPropertyDrawable&&) = delete;

    using DrawablePtr = std::unique_ptr<RSPropertyDrawable>;

    virtual void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) = 0;
    // return true if the drawable is updated, else we will destroy and regenerate it
    virtual bool Update(RSPropertyDrawableGenerateContext& context) { return false; }

    // Generator
    using DrawableVec = std::vector<RSPropertyDrawable::DrawablePtr>;
    static std::unordered_set<uint8_t> GenerateDirtySlots(const std::unordered_set<RSModifierType>& dirtyTypes);
    static bool UpdateDrawableVec(
        RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, std::unordered_set<uint8_t>& dirtySlots);
    static void InitializeSaveRestore(RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec);
    static void UpdateSaveRestore(
        RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, uint8_t& drawableVecStatus);

    using DrawableGenerator = std::function<RSPropertyDrawable::DrawablePtr(const RSPropertyDrawableGenerateContext&)>;
private:

    static inline uint8_t CalculateDrawableVecStatus(
        RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec);
    static void OptimizeBoundsSaveRestore(
        RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, uint8_t flags);
    static void OptimizeFrameSaveRestore(
        RSPropertyDrawableGenerateContext& context, DrawableVec& drawableVec, uint8_t flags);
};

class RSPropertyDrawableGenerateContext {
public:
    explicit RSPropertyDrawableGenerateContext(RSRenderNode& node);
    virtual ~RSPropertyDrawableGenerateContext() = default;

    // disable copy and move
    RSPropertyDrawableGenerateContext(const RSPropertyDrawableGenerateContext&) = delete;
    RSPropertyDrawableGenerateContext(const RSPropertyDrawableGenerateContext&&) = delete;
    RSPropertyDrawableGenerateContext& operator=(const RSPropertyDrawableGenerateContext&) = delete;
    RSPropertyDrawableGenerateContext& operator=(const RSPropertyDrawableGenerateContext&&) = delete;

    // member variable
    const std::shared_ptr<RSRenderNode> node_;
    const RSProperties& properties_;
    bool hasChildren_;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_H
