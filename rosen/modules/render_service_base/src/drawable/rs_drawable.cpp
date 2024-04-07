/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_drawable.h"

#include <limits>
#include "drawable/rs_misc_drawable.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {
namespace {
using namespace DrawableV2;

// NOTE: This LUT should always the same size and order as RSModifierType
// key = RSModifierType, value = RSDrawableSlot
constexpr int DIRTY_LUT_SIZE = static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE);
static const std::array<RSDrawableSlot, DIRTY_LUT_SIZE> g_propertyToDrawableLut = {
    RSDrawableSlot::INVALID,                       // INVALID
    RSDrawableSlot::CLIP_TO_BOUNDS,                // BOUNDS
    RSDrawableSlot::FRAME_OFFSET,                  // FRAME
    RSDrawableSlot::INVALID,                       // POSITION_Z
    RSDrawableSlot::INVALID,                       // PIVOT
    RSDrawableSlot::INVALID,                       // PIVOT_Z
    RSDrawableSlot::INVALID,                       // QUATERNION
    RSDrawableSlot::INVALID,                       // ROTATION
    RSDrawableSlot::INVALID,                       // ROTATION_X
    RSDrawableSlot::INVALID,                       // ROTATION_Y
    RSDrawableSlot::INVALID,                       // CAMERA_DISTANCE
    RSDrawableSlot::INVALID,                       // SCALE
    RSDrawableSlot::INVALID,                       // SKEW
    RSDrawableSlot::INVALID,                       // TRANSLATE
    RSDrawableSlot::INVALID,                       // TRANSLATE_Z
    RSDrawableSlot::INVALID,                       // SUBLAYER_TRANSFORM
    RSDrawableSlot::CLIP_TO_BOUNDS,                // CORNER_RADIUS
    RSDrawableSlot::INVALID,                       // ALPHA
    RSDrawableSlot::INVALID,                       // ALPHA_OFFSCREEN
    RSDrawableSlot::FOREGROUND_COLOR,              // FOREGROUND_COLOR
    RSDrawableSlot::BACKGROUND_COLOR,              // BACKGROUND_COLOR
    RSDrawableSlot::BACKGROUND_SHADER,             // BACKGROUND_SHADER
    RSDrawableSlot::BACKGROUND_IMAGE,              // BG_IMAGE
    RSDrawableSlot::BACKGROUND_IMAGE,              // BG_IMAGE_WIDTH
    RSDrawableSlot::BACKGROUND_IMAGE,              // BG_IMAGE_HEIGHT
    RSDrawableSlot::BACKGROUND_IMAGE,              // BG_IMAGE_POSITION_X
    RSDrawableSlot::BACKGROUND_IMAGE,              // BG_IMAGE_POSITION_Y
    RSDrawableSlot::INVALID,                       // SURFACE_BG_COLOR
    RSDrawableSlot::BORDER,                        // BORDER_COLOR
    RSDrawableSlot::BORDER,                        // BORDER_WIDTH
    RSDrawableSlot::BORDER,                        // BORDER_STYLE
    RSDrawableSlot::FOREGROUND_FILTER,             // FILTER
    RSDrawableSlot::BACKGROUND_FILTER,             // BACKGROUND_FILTER
    RSDrawableSlot::FOREGROUND_FILTER,             // LINEAR_GRADIENT_BLUR_PARA
    RSDrawableSlot::DYNAMIC_LIGHT_UP,              // DYNAMIC_LIGHT_UP_RATE
    RSDrawableSlot::DYNAMIC_LIGHT_UP,              // DYNAMIC_LIGHT_UP_DEGREE
    RSDrawableSlot::FRAME_OFFSET,                  // FRAME_GRAVITY
    RSDrawableSlot::CLIP_TO_BOUNDS,                // CLIP_RRECT
    RSDrawableSlot::CLIP_TO_BOUNDS,                // CLIP_BOUNDS
    RSDrawableSlot::CLIP_TO_BOUNDS,                // CLIP_TO_BOUNDS
    RSDrawableSlot::CLIP_TO_FRAME,                 // CLIP_TO_FRAME
    RSDrawableSlot::INVALID,                       // VISIBLE
    RSDrawableSlot::SHADOW,                        // SHADOW_COLOR
    RSDrawableSlot::SHADOW,                        // SHADOW_OFFSET_X
    RSDrawableSlot::SHADOW,                        // SHADOW_OFFSET_Y
    RSDrawableSlot::SHADOW,                        // SHADOW_ALPHA
    RSDrawableSlot::SHADOW,                        // SHADOW_ELEVATION
    RSDrawableSlot::SHADOW,                        // SHADOW_RADIUS
    RSDrawableSlot::SHADOW,                        // SHADOW_PATH
    RSDrawableSlot::SHADOW,                        // SHADOW_MASK
    RSDrawableSlot::SHADOW,                        // SHADOW_COLOR_STRATEGY
    RSDrawableSlot::MASK,                          // MASK
    RSDrawableSlot::INVALID,                       // SPHERIZE
    RSDrawableSlot::LIGHT_UP_EFFECT,               // LIGHT_UP_EFFECT
    RSDrawableSlot::PIXEL_STRETCH,                 // PIXEL_STRETCH
    RSDrawableSlot::PIXEL_STRETCH,                 // PIXEL_STRETCH_PERCENT
    RSDrawableSlot::USE_EFFECT,                    // USE_EFFECT
    RSDrawableSlot::BLEND_MODE,                    // COLOR_BLEND_MODE
    RSDrawableSlot::BLEND_MODE,                    // COLOR_BLEND_APPLY_TYPE
    RSDrawableSlot::INVALID,                       // SANDBOX
    RSDrawableSlot::COLOR_FILTER,                  // GRAY_SCALE
    RSDrawableSlot::COLOR_FILTER,                  // BRIGHTNESS
    RSDrawableSlot::COLOR_FILTER,                  // CONTRAST
    RSDrawableSlot::COLOR_FILTER,                  // SATURATE
    RSDrawableSlot::COLOR_FILTER,                  // SEPIA
    RSDrawableSlot::COLOR_FILTER,                  // INVERT
    RSDrawableSlot::BINARIZATION,                  // AIINVERT
    RSDrawableSlot::BACKGROUND_FILTER,             // SYSTEMBAREFFECT
    RSDrawableSlot::COLOR_FILTER,                  // HUE_ROTATE
    RSDrawableSlot::COLOR_FILTER,                  // COLOR_BLEND
    RSDrawableSlot::PARTICLE_EFFECT,               // PARTICLE
    RSDrawableSlot::INVALID,                       // SHADOW_IS_FILLED
    RSDrawableSlot::OUTLINE,                       // OUTLINE_COLOR
    RSDrawableSlot::OUTLINE,                       // OUTLINE_WIDTH
    RSDrawableSlot::OUTLINE,                       // OUTLINE_STYLE
    RSDrawableSlot::OUTLINE,                       // OUTLINE_RADIUS
    RSDrawableSlot::CHILDREN,                      // USE_SHADOW_BATCHING
    RSDrawableSlot::INVALID,                       // GREY_COEF
    RSDrawableSlot::POINT_LIGHT,                   // LIGHT_INTENSITY
    RSDrawableSlot::POINT_LIGHT,                   // LIGHT_POSITION
    RSDrawableSlot::POINT_LIGHT,                   // ILLUMINATED_BORDER_WIDTH
    RSDrawableSlot::POINT_LIGHT,                   // ILLUMINATED_TYPE
    RSDrawableSlot::POINT_LIGHT,                   // BLOOM
    RSDrawableSlot::INVALID,                       // DYNAMIC_DIM_DEGREE,
    RSDrawableSlot::INVALID,                       // CUSTOM
    RSDrawableSlot::INVALID,                       // EXTENDED
    RSDrawableSlot::TRANSITION,                    // TRANSITION
    RSDrawableSlot::BACKGROUND_STYLE,              // BACKGROUND_STYLE
    RSDrawableSlot::CONTENT_STYLE,                 // CONTENT_STYLE
    RSDrawableSlot::FOREGROUND_STYLE,              // FOREGROUND_STYLE
    RSDrawableSlot::OVERLAY,                       // OVERLAY_STYLE
    RSDrawableSlot::INVALID,                       // NODE_MODIFIER
    RSDrawableSlot::ENV_FOREGROUND_COLOR,          // ENV_FOREGROUND_COLOR
    RSDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY, // ENV_FOREGROUND_COLOR_STRATEGY
    RSDrawableSlot::INVALID,                       // GEOMETRYTRANS
    RSDrawableSlot::CHILDREN,                      // CHILDREN
};

template<RSModifierType type>
static inline RSDrawable::Ptr ModifierGenerator(const RSRenderNode& node)
{
    return RSCustomModifierDrawable::OnGenerate(node, type);
}

// NOTE: This LUT should always the same size as RSDrawableSlot
// index = RSDrawableSlotType, value = DrawableGenerator
constexpr int GEN_LUT_SIZE = static_cast<int>(RSDrawableSlot::MAX);
static const std::array<RSDrawable::Generator, GEN_LUT_SIZE> g_drawableGeneratorLut = {
    nullptr, // SAVE_ALL

    // Bounds Geometry
    RSMaskDrawable::OnGenerate,                    // MASK,
    ModifierGenerator<RSModifierType::TRANSITION>, // TRANSITION,
    RSEnvFGColorDrawable::OnGenerate,              // ENV_FOREGROUND_COLOR,
    RSShadowDrawable::OnGenerate,                  // SHADOW,
    RSOutlineDrawable::OnGenerate,                 // OUTLINE,

    // BG properties in Bounds Clip
    nullptr,                                             // BG_SAVE_BOUNDS,
    nullptr,                                             // CLIP_TO_BOUNDS,
    RSBeginBlendModeDrawable::OnGenerate,                // BLEND_MODE,
    RSBackgroundColorDrawable::OnGenerate,               // BACKGROUND_COLOR,
    RSBackgroundShaderDrawable::OnGenerate,              // BACKGROUND_SHADER,
    RSBackgroundImageDrawable::OnGenerate,               // BACKGROUND_IMAGE,
    RSBackgroundFilterDrawable::OnGenerate,              // BACKGROUND_FILTER,
    RSUseEffectDrawable::OnGenerate,                     // USE_EFFECT,
    ModifierGenerator<RSModifierType::BACKGROUND_STYLE>, // BACKGROUND_STYLE,
    RSDynamicLightUpDrawable::OnGenerate,                // DYNAMIC_LIGHT_UP,
    RSEnvFGColorStrategyDrawable::OnGenerate,            // ENV_FOREGROUND_COLOR_STRATEGY,
    nullptr,                                             // BG_RESTORE_BOUNDS,

    // Frame Geometry
    nullptr,                                             // SAVE_FRAME,
    RSFrameOffsetDrawable::OnGenerate,                   // FRAME_OFFSET,
    RSClipToFrameDrawable::OnGenerate,                   // CLIP_TO_FRAME,
    ModifierGenerator<RSModifierType::CONTENT_STYLE>,    // CONTENT_STYLE,
    RSChildrenDrawable::OnGenerate,                      // CHILDREN,
    ModifierGenerator<RSModifierType::FOREGROUND_STYLE>, // FOREGROUND_STYLE,
    nullptr,                                             // RESTORE_FRAME,

    // FG properties in Bounds clip
    nullptr,                                // FG_SAVE_BOUNDS,
    nullptr,                                // FG_CLIP_TO_BOUNDS,
    RSBinarizationDrawable::OnGenerate,     // BINARIZATION,
    RSColorFilterDrawable::OnGenerate,      // COLOR_FILTER,
    RSLightUpEffectDrawable::OnGenerate,    // LIGHT_UP_EFFECT,
    RSForegroundFilterDrawable::OnGenerate, // FOREGROUND_FILTER,
    RSForegroundColorDrawable::OnGenerate,  // FOREGROUND_COLOR,
    nullptr,                                // FG_RESTORE_BOUNDS,

    // No clip (unless ClipToBounds is set)
    RSPointLightDrawable::OnGenerate,                 // POINT_LIGHT,
    RSBorderDrawable::OnGenerate,                     // BORDER,
    ModifierGenerator<RSModifierType::OVERLAY_STYLE>, // OVERLAY,
    RSParticleDrawable::OnGenerate,                   // PARTICLE_EFFECT,
    RSPixelStretchDrawable::OnGenerate,               // PIXEL_STRETCH,

    // Restore state
    RSEndBlendModeDrawable::OnGenerate, // RESTORE_BLEND_MODE,
    nullptr,                            // RESTORE_ALL,
};

enum DrawableVecStatus : uint8_t {
    CLIP_TO_BOUNDS     = 1 << 0,
    BG_BOUNDS_PROPERTY = 1 << 1,
    FG_BOUNDS_PROPERTY = 1 << 2,
    CLIP_TO_FRAME      = 1 << 3,
    FRAME_PROPERTY     = 1 << 4,
    HAVE_ENV_CHANGE    = 1 << 5,
    BOUNDS_MASK        = CLIP_TO_BOUNDS | BG_BOUNDS_PROPERTY | FG_BOUNDS_PROPERTY,
    FRAME_MASK         = CLIP_TO_FRAME | FRAME_PROPERTY,
    OTHER_MASK         = HAVE_ENV_CHANGE,
};

inline static bool HasPropertyDrawableInRange(
    const RSDrawable::Vec& drawableVec, RSDrawableSlot begin, RSDrawableSlot end)
{
    return std::any_of(drawableVec.begin() + static_cast<size_t>(begin), drawableVec.begin() + static_cast<size_t>(end),
        [](const auto& Ptr) { return Ptr != nullptr; });
}

static uint8_t CalculateDrawableVecStatus(RSRenderNode& node, const RSDrawable::Vec& drawableVec)
{
    uint8_t result = 0;
    auto& properties = node.GetRenderProperties();

    // color blend mode has implicit dependency on clipToBounds
    if (properties.GetClipToBounds() || properties.GetClipToRRect() || properties.GetClipBounds() != nullptr ||
        properties.GetColorBlendMode() != static_cast<int>(RSColorBlendMode::NONE)) {
        result |= DrawableVecStatus::CLIP_TO_BOUNDS;
    }
    if (properties.GetClipToFrame()) {
        result |= DrawableVecStatus::CLIP_TO_FRAME;
    }

    if (HasPropertyDrawableInRange(
        drawableVec, RSDrawableSlot::BG_PROPERTIES_BEGIN, RSDrawableSlot::BG_PROPERTIES_END)) {
        result |= DrawableVecStatus::BG_BOUNDS_PROPERTY;
    }
    if (HasPropertyDrawableInRange(
        drawableVec, RSDrawableSlot::FG_PROPERTIES_BEGIN, RSDrawableSlot::FG_PROPERTIES_END)) {
        result |= DrawableVecStatus::FG_BOUNDS_PROPERTY;
    }

    // If we have any frame properties EXCEPT children
    if (HasPropertyDrawableInRange(drawableVec, RSDrawableSlot::CONTENT_PROPERTIES_BEGIN, RSDrawableSlot::CHILDREN) ||
        drawableVec[static_cast<size_t>(RSDrawableSlot::FOREGROUND_STYLE)]) {
        result |= DrawableVecStatus::FRAME_PROPERTY;
    }

    // Foreground color & Background Effect & Blend Mode should be processed here
    if (drawableVec[static_cast<size_t>(RSDrawableSlot::ENV_FOREGROUND_COLOR)] ||
        drawableVec[static_cast<size_t>(RSDrawableSlot::ENV_FOREGROUND_COLOR_STRATEGY)] ||
        drawableVec[static_cast<size_t>(RSDrawableSlot::BLEND_MODE)] ||
        (node.GetType() == RSRenderNodeType::EFFECT_NODE &&
            drawableVec[static_cast<size_t>(RSDrawableSlot::BACKGROUND_FILTER)])) {
        result |= DrawableVecStatus::HAVE_ENV_CHANGE;
    }

    return result;
}

inline static void SaveRestoreHelper(RSDrawable::Vec& drawableVec, RSDrawableSlot slot1, RSDrawableSlot slot2,
    RSPaintFilterCanvas::SaveType type = RSPaintFilterCanvas::kCanvas)
{
    if (type == RSPaintFilterCanvas::kNone) {
        return;
    }
    if (type == RSPaintFilterCanvas::kCanvas) {
        auto count = std::make_shared<uint32_t>(UINT_MAX);
        drawableVec[static_cast<size_t>(slot1)] = std::make_unique<RSSaveDrawable>(count);
        drawableVec[static_cast<size_t>(slot2)] = std::make_unique<RSRestoreDrawable>(count);
    } else {
        auto status = std::make_shared<RSPaintFilterCanvas::SaveStatus>();
        drawableVec[static_cast<size_t>(slot1)] = std::make_unique<RSCustomSaveDrawable>(status, type);
        drawableVec[static_cast<size_t>(slot2)] = std::make_unique<RSCustomRestoreDrawable>(status);
    }
}

static void OptimizeBoundsSaveRestore(RSRenderNode& node, RSDrawable::Vec& drawableVec, uint8_t flags)
{
    constexpr static std::array boundsSlotsToErase = {
        RSDrawableSlot::BG_SAVE_BOUNDS,
        RSDrawableSlot::CLIP_TO_BOUNDS,
        RSDrawableSlot::BG_RESTORE_BOUNDS,
        RSDrawableSlot::FG_SAVE_BOUNDS,
        RSDrawableSlot::FG_CLIP_TO_BOUNDS,
        RSDrawableSlot::FG_RESTORE_BOUNDS,
    };

    // Erase existing save/clip/restore before re-generating
    for (auto& slot : boundsSlotsToErase) {
        drawableVec[static_cast<size_t>(slot)] = nullptr;
    }

    if (flags & DrawableVecStatus::CLIP_TO_BOUNDS) {
        // case 1: ClipToBounds set.
        // add one clip, and reuse SAVE_ALL and RESTORE_ALL.
        drawableVec[static_cast<size_t>(RSDrawableSlot::CLIP_TO_BOUNDS)] = RSClipToBoundsDrawable::OnGenerate(node);
        return;
    }

    if ((flags & DrawableVecStatus::BG_BOUNDS_PROPERTY) && (flags & DrawableVecStatus::FG_BOUNDS_PROPERTY)) {
        // case 2: ClipToBounds not set and we have bounds properties both BG and FG.
        // add two sets of save/clip/restore before & after content.

        // part 1: before children
        SaveRestoreHelper(drawableVec, RSDrawableSlot::BG_SAVE_BOUNDS, RSDrawableSlot::BG_RESTORE_BOUNDS,
            RSPaintFilterCanvas::kCanvas);
        drawableVec[static_cast<size_t>(RSDrawableSlot::CLIP_TO_BOUNDS)] = RSClipToBoundsDrawable::OnGenerate(node);

        // part 2: after children, add aliases
        drawableVec[static_cast<size_t>(RSDrawableSlot::FG_SAVE_BOUNDS)] =
            drawableVec[static_cast<size_t>(RSDrawableSlot::BG_SAVE_BOUNDS)];
        drawableVec[static_cast<size_t>(RSDrawableSlot::FG_CLIP_TO_BOUNDS)] =
            drawableVec[static_cast<size_t>(RSDrawableSlot::CLIP_TO_BOUNDS)];
        drawableVec[static_cast<size_t>(RSDrawableSlot::FG_RESTORE_BOUNDS)] =
            drawableVec[static_cast<size_t>(RSDrawableSlot::BG_RESTORE_BOUNDS)];
        return;
    }

    if (flags & DrawableVecStatus::BG_BOUNDS_PROPERTY) {
        // case 3: ClipToBounds not set and we have background bounds properties.
        SaveRestoreHelper(drawableVec, RSDrawableSlot::BG_SAVE_BOUNDS, RSDrawableSlot::BG_RESTORE_BOUNDS,
            RSPaintFilterCanvas::kCanvas);

        drawableVec[static_cast<size_t>(RSDrawableSlot::CLIP_TO_BOUNDS)] = RSClipToBoundsDrawable::OnGenerate(node);
        return;
    }

    if (flags & DrawableVecStatus::FG_BOUNDS_PROPERTY) {
        // case 4: ClipToBounds not set and we have foreground bounds properties.
        SaveRestoreHelper(drawableVec, RSDrawableSlot::FG_SAVE_BOUNDS, RSDrawableSlot::FG_RESTORE_BOUNDS,
            RSPaintFilterCanvas::kCanvas);

        drawableVec[static_cast<size_t>(RSDrawableSlot::FG_CLIP_TO_BOUNDS)] = RSClipToBoundsDrawable::OnGenerate(node);
        return;
    }
    // case 5: ClipToBounds not set and no bounds properties, no need to save/clip/restore.
    // nothing to do
}

static void OptimizeFrameSaveRestore(RSRenderNode& node, RSDrawable::Vec& drawableVec, uint8_t flags)
{
    constexpr static std::array frameSlotsToErase = {
        RSDrawableSlot::SAVE_FRAME,
        RSDrawableSlot::RESTORE_FRAME,
    };
    // Erase existing save/clip/restore before re-generating
    for (auto& slot : frameSlotsToErase) {
        drawableVec[static_cast<size_t>(slot)] = nullptr;
    }

    // PLANNING: if both clipToFrame and clipToBounds are set, and frame == bounds, we don't need an extra clip
    if (flags & DrawableVecStatus::FRAME_PROPERTY) {
        // if we
        SaveRestoreHelper(
            drawableVec, RSDrawableSlot::SAVE_FRAME, RSDrawableSlot::RESTORE_FRAME, RSPaintFilterCanvas::kCanvas);
    }
}

static void OptimizeGlobalSaveRestore(RSRenderNode& node, RSDrawable::Vec& drawableVec, uint8_t flags)
{
    constexpr static std::array globalSlotsToErase = {
        RSDrawableSlot::SAVE_ALL,
        RSDrawableSlot::RESTORE_ALL,
    };
    // Erase existing save/clip/restore before re-generating
    for (auto& slot : globalSlotsToErase) {
        drawableVec[static_cast<size_t>(slot)] = nullptr;
    }

    // Parent will do canvas save/restore, we don't need to do it again
    uint8_t saveType = RSPaintFilterCanvas::SaveType::kNone;
    if (flags & DrawableVecStatus::HAVE_ENV_CHANGE) {
        // If we change env(fg color, effect, blendMode etc), we need to save env
        saveType |= RSPaintFilterCanvas::SaveType::kEnv;
    }

    if (saveType == RSPaintFilterCanvas::SaveType::kNone) {
        return;
    }
    // add save/restore with needed type
    SaveRestoreHelper(drawableVec, RSDrawableSlot::SAVE_ALL, RSDrawableSlot::RESTORE_ALL,
        static_cast<RSPaintFilterCanvas::SaveType>(saveType));
}
} // namespace

std::unordered_set<RSDrawableSlot> RSDrawable::CalculateDirtySlots(
    const ModifierDirtyTypes& dirtyTypes, const Vec& drawableVec)
{
    // Step 1.1: calculate dirty slots by looking up g_propertyToDrawableLut
    std::unordered_set<RSDrawableSlot> dirtySlots;
    for (size_t type = 0; type < static_cast<size_t>(RSModifierType::MAX_RS_MODIFIER_TYPE); type++) {
        if (!dirtyTypes.test(type)) {
            continue;
        }
        auto dirtySlot = g_propertyToDrawableLut[type];
        if (dirtySlot != RSDrawableSlot::INVALID) {
            dirtySlots.emplace(dirtySlot);
        }
    }

    // Step 1.2: expand dirty slots by rules
    // TODO: border etc. should be updated when border radius changed
    if (dirtySlots.count(RSDrawableSlot::FRAME_OFFSET)) {
        if (drawableVec[static_cast<size_t>(RSDrawableSlot::CLIP_TO_FRAME)]) {
            dirtySlots.emplace(RSDrawableSlot::CLIP_TO_FRAME);
        }
    }

    // Step 1.3: if bounds changed, every existing drawable needs to be updated
    if (dirtyTypes.test(static_cast<size_t>(RSModifierType::BOUNDS))) {
        for (size_t i = 0; i < drawableVec.size(); i++) {
            if (drawableVec[i]) {
                dirtySlots.emplace(static_cast<RSDrawableSlot>(i));
            }
        }
    }

    return dirtySlots;
}

bool RSDrawable::UpdateDirtySlots(
    const RSRenderNode& node, Vec& drawableVec, std::unordered_set<RSDrawableSlot>& dirtySlots)
{
    // Step 2: Update or generate all dirty slots
    bool drawableAddedOrRemoved = false;
    for (const auto& slot : dirtySlots) {
        if (auto& drawable = drawableVec[static_cast<size_t>(slot)]) {
            // If the slot is already created, call OnUpdate
            if (!drawable->OnUpdate(node)) {
                // If the slot is no longer needed, destroy it
                drawable.reset();
                drawableAddedOrRemoved = true;
            }
        } else if (auto& generator = g_drawableGeneratorLut[static_cast<int>(slot)]) {
            // If the slot is not created, call OnGenerate
            if (auto drawable = generator(node)) {
                drawableVec[static_cast<size_t>(slot)] = std::move(drawable);
                drawableAddedOrRemoved = true;
            }
        }
    }

    return drawableAddedOrRemoved;
}

void RSDrawable::UpdateSaveRestore(RSRenderNode& node, Vec& drawableVec, uint8_t& drawableVecStatus)
{
    // ====================================================================
    // Step 3: Universal save/clip/restore optimization

    // Step 3.1: calculate new drawable map status
    auto drawableVecStatusNew = CalculateDrawableVecStatus(node, drawableVec);

    uint8_t changedBits = drawableVecStatus ^ drawableVecStatusNew;
    if (changedBits == 0) {
        // nothing to do
        return;
    }

    // Step 3.2: update save/clip/restore for changed types
    if (changedBits & BOUNDS_MASK) {
        // update bounds save/clip if need
        OptimizeBoundsSaveRestore(node, drawableVec, drawableVecStatusNew);
    }
    if (changedBits & FRAME_MASK) {
        // update frame save/clip if need
        OptimizeFrameSaveRestore(node, drawableVec, drawableVecStatusNew);
    }
    if (changedBits & OTHER_MASK) {
        // update global save/clip if need
        OptimizeGlobalSaveRestore(node, drawableVec, drawableVecStatusNew);
    }

    drawableVecStatus = drawableVecStatusNew;
}
} // namespace OHOS::Rosen
