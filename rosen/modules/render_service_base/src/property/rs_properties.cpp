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

#include "property/rs_properties.h"

#include <algorithm>
#include <securec.h>

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_point_light_manager.h"
#include "property/rs_properties_def.h"
#include "render/rs_filter.h"
#include "render/rs_material_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t INDEX_2 = 2;
constexpr int32_t INDEX_4 = 4;
constexpr int32_t INDEX_5 = 5;
constexpr int32_t INDEX_6 = 6;
constexpr int32_t INDEX_7 = 7;
constexpr int32_t INDEX_9 = 9;
constexpr int32_t INDEX_10 = 10;
constexpr int32_t INDEX_11 = 11;
constexpr int32_t INDEX_12 = 12;
constexpr int32_t INDEX_14 = 14;
constexpr int32_t INDEX_18 = 18;

const Vector4f Vector4fZero { 0.f, 0.f, 0.f, 0.f };
const auto EMPTY_RECT = RectF();
constexpr float SPHERIZE_VALID_EPSILON = 0.001f; // used to judge if spherize valid
constexpr uint8_t BORDER_TYPE_NONE = (uint32_t)BorderStyle::NONE;

using ResetPropertyFunc = void (*)(RSProperties* prop);
// Every modifier before RSModifierType::CUSTOM is property modifier, and it should have a ResetPropertyFunc
// NOTE: alway add new resetter when adding new property modifier
const std::array<ResetPropertyFunc, static_cast<int>(RSModifierType::CUSTOM)> g_propertyResetterLUT = {
    nullptr,                                                             // INVALID,                  0
    nullptr,                                                             // BOUNDS,                   1
    nullptr,                                                             // FRAME,                    2
    [](RSProperties* prop) { prop->SetPositionZ(0.f); },                 // POSITION_Z,               3
    [](RSProperties* prop) { prop->SetPivot(Vector2f(0.5f, 0.5f)); },    // PIVOT,                    4
    [](RSProperties* prop) { prop->SetPivotZ(0.f); },                    // PIVOT_Z,                  5
    [](RSProperties* prop) { prop->SetQuaternion(Quaternion()); },       // QUATERNION,               6
    [](RSProperties* prop) { prop->SetRotation(0.f); },                  // ROTATION,                 7
    [](RSProperties* prop) { prop->SetRotationX(0.f); },                 // ROTATION_X,               8
    [](RSProperties* prop) { prop->SetRotationY(0.f); },                 // ROTATION_Y,               9
    [](RSProperties* prop) { prop->SetCameraDistance(0.f); },            // CAMERA_DISTANCE,          10
    [](RSProperties* prop) { prop->SetScale(Vector2f(1.f, 1.f)); },      // SCALE,                    11
    [](RSProperties* prop) { prop->SetTranslate(Vector2f(0.f, 0.f)); },  // TRANSLATE,                12
    [](RSProperties* prop) { prop->SetTranslateZ(0.f); },                // TRANSLATE_Z,              13
    [](RSProperties* prop) { prop->SetSublayerTransform({}); },          // SUBLAYER_TRANSFORM,       14
    [](RSProperties* prop) { prop->SetCornerRadius(0.f); },              // CORNER_RADIUS,            15
    [](RSProperties* prop) { prop->SetAlpha(1.f); },                     // ALPHA,                    16
    [](RSProperties* prop) { prop->SetAlphaOffscreen(false); },          // ALPHA_OFFSCREEN,          17
    [](RSProperties* prop) { prop->SetForegroundColor({}); },            // FOREGROUND_COLOR,         18
    [](RSProperties* prop) { prop->SetBackgroundColor({}); },            // BACKGROUND_COLOR,         19
    [](RSProperties* prop) { prop->SetBackgroundShader({}); },           // BACKGROUND_SHADER,        20
    [](RSProperties* prop) { prop->SetBgImage({}); },                    // BG_IMAGE,                 21
    [](RSProperties* prop) { prop->SetBgImageWidth(0.f); },              // BG_IMAGE_WIDTH,           22
    [](RSProperties* prop) { prop->SetBgImageHeight(0.f); },             // BG_IMAGE_HEIGHT,          23
    [](RSProperties* prop) { prop->SetBgImagePositionX(0.f); },          // BG_IMAGE_POSITION_X,      24
    [](RSProperties* prop) { prop->SetBgImagePositionY(0.f); },          // BG_IMAGE_POSITION_Y,      25
    nullptr,                                                             // SURFACE_BG_COLOR,         26
    [](RSProperties* prop) { prop->SetBorderColor(RSColor()); },         // BORDER_COLOR,             27
    [](RSProperties* prop) { prop->SetBorderWidth(0.f); },               // BORDER_WIDTH,             28
    [](RSProperties* prop) { prop->SetBorderStyle(BORDER_TYPE_NONE); },  // BORDER_STYLE,             29
    [](RSProperties* prop) { prop->SetFilter({}); },                     // FILTER,                   30
    [](RSProperties* prop) { prop->SetBackgroundFilter({}); },           // BACKGROUND_FILTER,        31
    [](RSProperties* prop) { prop->SetLinearGradientBlurPara({}); },     // LINEAR_GRADIENT_BLUR_PARA,32
    [](RSProperties* prop) { prop->SetDynamicLightUpRate({}); },         // DYNAMIC_LIGHT_UP_RATE,    33
    [](RSProperties* prop) { prop->SetDynamicLightUpDegree({}); },       // DYNAMIC_LIGHT_UP_DEGREE,  34
    [](RSProperties* prop) { prop->SetFrameGravity(Gravity::DEFAULT); }, // FRAME_GRAVITY,            35
    [](RSProperties* prop) { prop->SetClipRRect({}); },                  // CLIP_RRECT,               36
    [](RSProperties* prop) { prop->SetClipBounds({}); },                 // CLIP_BOUNDS,              37
    [](RSProperties* prop) { prop->SetClipToBounds(false); },            // CLIP_TO_BOUNDS,           38
    [](RSProperties* prop) { prop->SetClipToFrame(false); },             // CLIP_TO_FRAME,            39
    [](RSProperties* prop) { prop->SetVisible(true); },                  // VISIBLE,                  40
    [](RSProperties* prop) { prop->SetShadowColor({}); },                // SHADOW_COLOR,             41
    [](RSProperties* prop) { prop->SetShadowOffsetX(0.f); },             // SHADOW_OFFSET_X,          42
    [](RSProperties* prop) { prop->SetShadowOffsetY(0.f); },             // SHADOW_OFFSET_Y,          43
    [](RSProperties* prop) { prop->SetShadowAlpha(0.f); },               // SHADOW_ALPHA,             44
    [](RSProperties* prop) { prop->SetShadowElevation(0.f); },           // SHADOW_ELEVATION,         45
    [](RSProperties* prop) { prop->SetShadowRadius(0.f); },              // SHADOW_RADIUS,            46
    [](RSProperties* prop) { prop->SetShadowPath({}); },                 // SHADOW_PATH,              47
    [](RSProperties* prop) { prop->SetShadowMask(false); },              // SHADOW_MASK,              48
    [](RSProperties* prop) { prop->SetShadowColorStrategy(               // ShadowColorStrategy,      49
        SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE); },
    [](RSProperties* prop) { prop->SetMask({}); },                       // MASK,                     50
    [](RSProperties* prop) { prop->SetSpherize(0.f); },                  // SPHERIZE,                 51
    [](RSProperties* prop) { prop->SetLightUpEffect(1.f); },             // LIGHT_UP_EFFECT,          52
    [](RSProperties* prop) { prop->SetPixelStretch({}); },               // PIXEL_STRETCH,            53
    [](RSProperties* prop) { prop->SetPixelStretchPercent({}); },        // PIXEL_STRETCH_PERCENT,    54
    [](RSProperties* prop) { prop->SetUseEffect(false); },               // USE_EFFECT,               55
    [](RSProperties* prop) { prop->SetColorBlendMode(
        static_cast<int>(RSColorBlendModeType::NONE)); },                // COLOR_BLENDMODE,          56
    [](RSProperties* prop) { prop->ResetSandBox(); },                    // SANDBOX,                  57
    [](RSProperties* prop) { prop->SetGrayScale({}); },                  // GRAY_SCALE,               58
    [](RSProperties* prop) { prop->SetBrightness({}); },                 // BRIGHTNESS,               59
    [](RSProperties* prop) { prop->SetContrast({}); },                   // CONTRAST,                 60
    [](RSProperties* prop) { prop->SetSaturate({}); },                   // SATURATE,                 61
    [](RSProperties* prop) { prop->SetSepia({}); },                      // SEPIA,                    62
    [](RSProperties* prop) { prop->SetInvert({}); },                     // INVERT,                   63
    [](RSProperties* prop) { prop->SetAiInvert({}); },                   // AIINVERT,                 64
    [](RSProperties* prop) { prop->SetHueRotate({}); },                  // HUE_ROTATE,               65
    [](RSProperties* prop) { prop->SetColorBlend({}); },                 // COLOR_BLEND,              66
    [](RSProperties* prop) { prop->SetParticles({}); },                  // PARTICLE,                 67
    [](RSProperties* prop) { prop->SetShadowIsFilled(false); },          // SHADOW_IS_FILLED,         68
    [](RSProperties* prop) { prop->SetOuterBorderColor(RSColor()); },    // OUTER_BORDER_COLOR,       69
    [](RSProperties* prop) { prop->SetOuterBorderWidth(0.f); },          // OUTER_BORDER_WIDTH,       70
    [](RSProperties* prop) {
        prop->SetOuterBorderStyle(BORDER_TYPE_NONE);
    },                                                                   // OUTER_BORDER_STYLE,       71
    [](RSProperties* prop) { prop->SetOuterBorderRadius(0.f); },         // OUTER_BORDER_RADIUS,      72
    [](RSProperties* prop) { prop->SetUseShadowBatching(false); },       // USE_SHADOW_BATCHING,      73
    [](RSProperties* prop) { prop->SetGreyCoef1(0.f); },               // GREY_COEF1,                 74
    [](RSProperties* prop) { prop->SetGreyCoef2(0.f); },               // GREY_COEF2,                 75
    [](RSProperties* prop) { prop->SetLightIntensity(-1.f); },            // LIGHT_INTENSITY           76
    [](RSProperties* prop) { prop->SetLightPosition({}); },               // LIGHT_POSITION            77
    [](RSProperties* prop) { prop->SetIlluminatedType(-1); },             // ILLUMINATED_TYPE          78
    [](RSProperties* prop) { prop->SetBloom({}); },                       // BLOOM                     79
};
} // namespace

// Only enable filter cache when uni-render is enabled and filter cache is enabled

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
const bool RSProperties::FilterCacheEnabled =
    RSSystemProperties::GetFilterCacheEnabled() && RSUniRenderJudgement::IsUniRender();
#endif

RSProperties::RSProperties()
{
    boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    frameGeo_ = std::make_shared<RSObjGeometry>();
}

RSProperties::~RSProperties() = default;

#ifndef USE_ROSEN_DRAWING
void RSProperties::ResetProperty(const std::unordered_set<RSModifierType>& dirtyTypes)
{
    for (const auto& type : dirtyTypes) {
        if (type >= RSModifierType::CUSTOM) {
            continue;
        }
        if (auto& resetFunc = g_propertyResetterLUT[static_cast<uint8_t>(type)]) {
            resetFunc(this);
        }
    }
}
#else
void RSProperties::ResetProperty(const std::bitset<static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE)>& dirtyTypes)
{
    if (dirtyTypes.none()) {
        return;
    }
    for (uint8_t type = 0; type < static_cast<size_t>(RSModifierType::CUSTOM); type++) {
        if (dirtyTypes[type]) {
            if (auto& resetFunc = g_propertyResetterLUT[type]) {
                resetFunc(this);
            }
        }
    }
}
#endif

void RSProperties::SetBounds(Vector4f bounds)
{
    if (bounds.z_ != boundsGeo_->GetWidth() || bounds.w_ != boundsGeo_->GetHeight()) {
        contentDirty_ = true;
    }
    boundsGeo_->SetRect(bounds.x_, bounds.y_, bounds.z_, bounds.w_);
    hasBounds_ = true;
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsSize(Vector2f size)
{
    boundsGeo_->SetSize(size.x_, size.y_);
    hasBounds_ = true;
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsWidth(float width)
{
    boundsGeo_->SetWidth(width);
    hasBounds_ = true;
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsHeight(float height)
{
    boundsGeo_->SetHeight(height);
    hasBounds_ = true;
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsPosition(Vector2f position)
{
    boundsGeo_->SetPosition(position.x_, position.y_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsPositionX(float positionX)
{
    boundsGeo_->SetX(positionX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsPositionY(float positionY)
{
    boundsGeo_->SetY(positionY);
    geoDirty_ = true;
    SetDirty();
}

Vector4f RSProperties::GetBounds() const
{
    return { boundsGeo_->GetX(), boundsGeo_->GetY(), boundsGeo_->GetWidth(), boundsGeo_->GetHeight() };
}

Vector2f RSProperties::GetBoundsSize() const
{
    return { boundsGeo_->GetWidth(), boundsGeo_->GetHeight() };
}

float RSProperties::GetBoundsWidth() const
{
    return boundsGeo_->GetWidth();
}

float RSProperties::GetBoundsHeight() const
{
    return boundsGeo_->GetHeight();
}

float RSProperties::GetBoundsPositionX() const
{
    return boundsGeo_->GetX();
}

float RSProperties::GetBoundsPositionY() const
{
    return boundsGeo_->GetY();
}

Vector2f RSProperties::GetBoundsPosition() const
{
    return { GetBoundsPositionX(), GetBoundsPositionY() };
}

void RSProperties::SetFrame(Vector4f frame)
{
    if (frame.z_ != frameGeo_->GetWidth() || frame.w_ != frameGeo_->GetHeight()) {
        contentDirty_ = true;
    }
    frameGeo_->SetRect(frame.x_, frame.y_, frame.z_, frame.w_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetFrameSize(Vector2f size)
{
    frameGeo_->SetSize(size.x_, size.y_);
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetFrameWidth(float width)
{
    frameGeo_->SetWidth(width);
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetFrameHeight(float height)
{
    frameGeo_->SetHeight(height);
    geoDirty_ = true;
    contentDirty_ = true;
    SetDirty();
}

void RSProperties::SetFramePosition(Vector2f position)
{
    frameGeo_->SetPosition(position.x_, position.y_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetFramePositionX(float positionX)
{
    frameGeo_->SetX(positionX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetFramePositionY(float positionY)
{
    frameGeo_->SetY(positionY);
    geoDirty_ = true;
    SetDirty();
}

Vector4f RSProperties::GetFrame() const
{
    return { frameGeo_->GetX(), frameGeo_->GetY(), frameGeo_->GetWidth(), frameGeo_->GetHeight() };
}

Vector2f RSProperties::GetFrameSize() const
{
    return { frameGeo_->GetWidth(), frameGeo_->GetHeight() };
}

float RSProperties::GetFrameWidth() const
{
    return frameGeo_->GetWidth();
}

float RSProperties::GetFrameHeight() const
{
    return frameGeo_->GetHeight();
}

float RSProperties::GetFramePositionX() const
{
    return frameGeo_->GetX();
}

float RSProperties::GetFramePositionY() const
{
    return frameGeo_->GetY();
}

Vector2f RSProperties::GetFramePosition() const
{
    return { GetFramePositionX(), GetFramePositionY() };
}

float RSProperties::GetFrameOffsetX() const
{
    return frameOffsetX_;
}

float RSProperties::GetFrameOffsetY() const
{
    return frameOffsetY_;
}

const std::shared_ptr<RSObjAbsGeometry>& RSProperties::GetBoundsGeometry() const
{
    return boundsGeo_;
}

const std::shared_ptr<RSObjGeometry>& RSProperties::GetFrameGeometry() const
{
    return frameGeo_;
}

#ifndef USE_ROSEN_DRAWING
bool RSProperties::UpdateGeometry(const RSProperties* parent, bool dirtyFlag, const std::optional<SkPoint>& offset,
    const std::optional<SkRect>& clipRect)
#else
bool RSProperties::UpdateGeometry(const RSProperties* parent, bool dirtyFlag,
    const std::optional<Drawing::Point>& offset, const std::optional<Drawing::Rect>& clipRect)
#endif
{
    if (boundsGeo_ == nullptr) {
        return false;
    }
    auto boundsGeoPtr = (boundsGeo_);

    if (!dirtyFlag && !geoDirty_) {
        return false;
    }
    auto parentGeo = parent == nullptr ? nullptr : (parent->boundsGeo_);
    if (parentGeo && sandbox_ && sandbox_->matrix_) {
        parentGeo = std::make_shared<RSObjAbsGeometry>();
        parentGeo->ConcatMatrix(*(sandbox_->matrix_));
    }
    CheckEmptyBounds();
    boundsGeoPtr->UpdateMatrix(parentGeo, offset, clipRect);
    if (lightSourcePtr_ && lightSourcePtr_->IsLightSourceValid()) {
        CalculateAbsLightPosition();
        RSPointLightManager::Instance()->AddDirtyLightSource(backref_);
    }
    if (illuminatedPtr_ && illuminatedPtr_->IsIlluminatedValid()) {
        RSPointLightManager::Instance()->AddDirtyIlluminated(backref_);
    }
    if (RSSystemProperties::GetSkipGeometryNotChangeEnabled()) {
        auto rect = boundsGeoPtr->GetAbsRect();
        if (!lastRect_.has_value()) {
            lastRect_ = rect;
            return true;
        }
        dirtyFlag = dirtyFlag || rect != lastRect_.value();
        lastRect_ = rect;
        return dirtyFlag;
    } else {
        return true;
    }
}

void RSProperties::SetSandBox(const std::optional<Vector2f>& parentPosition)
{
    if (!sandbox_) {
        sandbox_ = std::make_unique<Sandbox>();
    }
    sandbox_->position_ = parentPosition;
    geoDirty_ = true;
    SetDirty();
}

std::optional<Vector2f> RSProperties::GetSandBox() const
{
    return sandbox_ ? sandbox_->position_ : std::nullopt;
}

void RSProperties::ResetSandBox()
{
    sandbox_ = nullptr;
}

#ifndef USE_ROSEN_DRAWING
void RSProperties::UpdateSandBoxMatrix(const std::optional<SkMatrix>& rootMatrix)
#else
void RSProperties::UpdateSandBoxMatrix(const std::optional<Drawing::Matrix>& rootMatrix)
#endif
{
    if (!sandbox_) {
        return;
    }
    if (!rootMatrix || !sandbox_->position_) {
        sandbox_->matrix_ = std::nullopt;
        return;
    }
    auto rootMat = rootMatrix.value();
#ifndef USE_ROSEN_DRAWING
    bool hasScale = rootMat.getScaleX() != 1.0f || rootMat.getScaleY() != 1.0f;
#else
    bool hasScale = rootMat.Get(Drawing::Matrix::SCALE_X) != 1.0f || rootMat.Get(Drawing::Matrix::SCALE_Y) != 1.0f;
#endif
    if (hasScale) {
        sandbox_->matrix_ = std::nullopt;
        return;
    }
#ifndef USE_ROSEN_DRAWING
    sandbox_->matrix_ = rootMat.preTranslate(sandbox_->position_->x_, sandbox_->position_->y_);
#else
    Drawing::Matrix matrix = rootMatrix.value();
    matrix.PreTranslate(sandbox_->position_->x_, sandbox_->position_->y_);
    sandbox_->matrix_ = matrix;
#endif
}

#ifndef USE_ROSEN_DRAWING
std::optional<SkMatrix> RSProperties::GetSandBoxMatrix() const
#else
std::optional<Drawing::Matrix> RSProperties::GetSandBoxMatrix() const
#endif
{
    return sandbox_ ? sandbox_->matrix_ : std::nullopt;
}

void RSProperties::SetPositionZ(float positionZ)
{
    boundsGeo_->SetZ(positionZ);
    frameGeo_->SetZ(positionZ);
    geoDirty_ = true;
    SetDirty();
}

float RSProperties::GetPositionZ() const
{
    return boundsGeo_->GetZ();
}

void RSProperties::SetPivot(Vector2f pivot)
{
    boundsGeo_->SetPivot(pivot.x_, pivot.y_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPivotX(float pivotX)
{
    boundsGeo_->SetPivotX(pivotX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPivotY(float pivotY)
{
    boundsGeo_->SetPivotY(pivotY);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPivotZ(float pivotZ)
{
    boundsGeo_->SetPivotZ(pivotZ);
    geoDirty_ = true;
    SetDirty();
}

Vector2f RSProperties::GetPivot() const
{
    return { boundsGeo_->GetPivotX(), boundsGeo_->GetPivotY() };
}

float RSProperties::GetPivotX() const
{
    return boundsGeo_->GetPivotX();
}

float RSProperties::GetPivotY() const
{
    return boundsGeo_->GetPivotY();
}

float RSProperties::GetPivotZ() const
{
    return boundsGeo_->GetPivotZ();
}

void RSProperties::SetCornerRadius(const Vector4f& cornerRadius)
{
    cornerRadius_ = cornerRadius;
    SetDirty();
}

const Vector4f& RSProperties::GetCornerRadius() const
{
    return cornerRadius_ ? cornerRadius_.value() : Vector4fZero;
}

void RSProperties::SetQuaternion(Quaternion quaternion)
{
    boundsGeo_->SetQuaternion(quaternion);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetRotation(float degree)
{
    boundsGeo_->SetRotation(degree);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetRotationX(float degree)
{
    boundsGeo_->SetRotationX(degree);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetRotationY(float degree)
{
    boundsGeo_->SetRotationY(degree);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetCameraDistance(float cameraDistance)
{
    boundsGeo_->SetCameraDistance(cameraDistance);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetScale(Vector2f scale)
{
    boundsGeo_->SetScale(scale.x_, scale.y_);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetScaleX(float sx)
{
    boundsGeo_->SetScaleX(sx);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetScaleY(float sy)
{
    boundsGeo_->SetScaleY(sy);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslate(Vector2f translate)
{
    boundsGeo_->SetTranslateX(translate[0]);
    boundsGeo_->SetTranslateY(translate[1]);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslateX(float translate)
{
    boundsGeo_->SetTranslateX(translate);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslateY(float translate)
{
    boundsGeo_->SetTranslateY(translate);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslateZ(float translate)
{
    boundsGeo_->SetTranslateZ(translate);
    geoDirty_ = true;
    SetDirty();
}

Quaternion RSProperties::GetQuaternion() const
{
    return boundsGeo_->GetQuaternion();
}

float RSProperties::GetRotation() const
{
    return boundsGeo_->GetRotation();
}

float RSProperties::GetRotationX() const
{
    return boundsGeo_->GetRotationX();
}

float RSProperties::GetRotationY() const
{
    return boundsGeo_->GetRotationY();
}

float RSProperties::GetCameraDistance() const
{
    return boundsGeo_->GetCameraDistance();
}

float RSProperties::GetScaleX() const
{
    return boundsGeo_->GetScaleX();
}

float RSProperties::GetScaleY() const
{
    return boundsGeo_->GetScaleY();
}

Vector2f RSProperties::GetScale() const
{
    return { boundsGeo_->GetScaleX(), boundsGeo_->GetScaleY() };
}

Vector2f RSProperties::GetTranslate() const
{
    return Vector2f(GetTranslateX(), GetTranslateY());
}

float RSProperties::GetTranslateX() const
{
    return boundsGeo_->GetTranslateX();
}

float RSProperties::GetTranslateY() const
{
    return boundsGeo_->GetTranslateY();
}

float RSProperties::GetTranslateZ() const
{
    return boundsGeo_->GetTranslateZ();
}

void RSProperties::SetParticles(const RSRenderParticleVector& particles)
{
    particles_ = particles;
    if (particles_.GetParticleSize() > 0) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

const RSRenderParticleVector& RSProperties::GetParticles() const
{
    return particles_;
}

void RSProperties::SetAlpha(float alpha)
{
    alpha_ = alpha;
    if (alpha_ < 1.f) {
        alphaNeedApply_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetAlpha() const
{
    return alpha_;
}
void RSProperties::SetAlphaOffscreen(bool alphaOffscreen)
{
    alphaOffscreen_ = alphaOffscreen;
    SetDirty();
    contentDirty_ = true;
}

bool RSProperties::GetAlphaOffscreen() const
{
    return alphaOffscreen_;
}

void RSProperties::SetSublayerTransform(const std::optional<Matrix3f>& sublayerTransform)
{
    sublayerTransform_ = sublayerTransform;
    SetDirty();
}

const std::optional<Matrix3f>& RSProperties::GetSublayerTransform() const
{
    return sublayerTransform_;
}

// foreground properties
void RSProperties::SetForegroundColor(Color color)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->foregroundColor_ = color;
    SetDirty();
    contentDirty_ = true;
}

Color RSProperties::GetForegroundColor() const
{
    return decoration_ ? decoration_->foregroundColor_ : RgbPalette::Transparent();
}

// background properties
void RSProperties::SetBackgroundColor(Color color)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    if (color.GetAlpha() > 0) {
        isDrawn_ = true;
    }
    decoration_->backgroundColor_ = color;
    SetDirty();
    contentDirty_ = true;
}

const Color& RSProperties::GetBackgroundColor() const
{
    return decoration_ ? decoration_->backgroundColor_ : RgbPalette::Transparent();
}

void RSProperties::SetBackgroundShader(const std::shared_ptr<RSShader>& shader)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    if (shader) {
        isDrawn_ = true;
    }
    decoration_->bgShader_ = shader;
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSShader> RSProperties::GetBackgroundShader() const
{
    return decoration_ ? decoration_->bgShader_ : nullptr;
}

void RSProperties::SetBgImage(const std::shared_ptr<RSImage>& image)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    if (image) {
        isDrawn_ = true;
    }
    decoration_->bgImage_ = image;
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSImage> RSProperties::GetBgImage() const
{
    return decoration_ ? decoration_->bgImage_ : nullptr;
}

void RSProperties::SetBgImageWidth(float width)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->bgImageRect_.width_ = width;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBgImageHeight(float height)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->bgImageRect_.height_ = height;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBgImagePositionX(float positionX)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->bgImageRect_.left_ = positionX;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBgImagePositionY(float positionY)
{
    if (!decoration_) {
        decoration_ = std::make_optional<Decoration>();
    }
    decoration_->bgImageRect_.top_ = positionY;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetBgImageWidth() const
{
    return decoration_ ? decoration_->bgImageRect_.width_ : 0.f;
}

float RSProperties::GetBgImageHeight() const
{
    return decoration_ ? decoration_->bgImageRect_.height_ : 0.f;
}

float RSProperties::GetBgImagePositionX() const
{
    return decoration_ ? decoration_->bgImageRect_.left_ : 0.f;
}

float RSProperties::GetBgImagePositionY() const
{
    return decoration_ ? decoration_->bgImageRect_.top_ : 0.f;
}

// border properties
void RSProperties::SetBorderColor(Vector4<Color> color)
{
    if (!border_) {
        border_ = std::make_shared<RSBorder>();
    }
    border_->SetColorFour(color);
    if (border_->GetColor().GetAlpha() > 0) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBorderWidth(Vector4f width)
{
    if (!border_) {
        border_ = std::make_shared<RSBorder>();
    }
    border_->SetWidthFour(width);
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetBorderStyle(Vector4<uint32_t> style)
{
    if (!border_) {
        border_ = std::make_shared<RSBorder>();
    }
    border_->SetStyleFour(style);
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

Vector4<Color> RSProperties::GetBorderColor() const
{
    return border_ ? border_->GetColorFour() : Vector4<Color>(RgbPalette::Transparent());
}

Vector4f RSProperties::GetBorderWidth() const
{
    return border_ ? border_->GetWidthFour() : Vector4f(0.f);
}

Vector4<uint32_t> RSProperties::GetBorderStyle() const
{
    return border_ ? border_->GetStyleFour() : Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE));
}

const std::shared_ptr<RSBorder>& RSProperties::GetBorder() const
{
    return border_;
}

void RSProperties::SetOuterBorderColor(Vector4<Color> color)
{
    if (!outerBorder_) {
        outerBorder_ = std::make_shared<RSBorder>();
    }
    outerBorder_->SetColorFour(color);
    if (outerBorder_->GetColor().GetAlpha() > 0) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetOuterBorderWidth(Vector4f width)
{
    if (!outerBorder_) {
        outerBorder_ = std::make_shared<RSBorder>();
    }
    outerBorder_->SetWidthFour(width);
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetOuterBorderStyle(Vector4<uint32_t> style)
{
    if (!outerBorder_) {
        outerBorder_ = std::make_shared<RSBorder>();
    }
    outerBorder_->SetStyleFour(style);
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetOuterBorderRadius(Vector4f radius)
{
    if (!outerBorder_) {
        outerBorder_ = std::make_shared<RSBorder>();
    }
    outerBorder_->SetRadiusFour(radius);
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

Vector4<Color> RSProperties::GetOuterBorderColor() const
{
    return outerBorder_ ? outerBorder_->GetColorFour() : Vector4<Color>(RgbPalette::Transparent());
}

Vector4f RSProperties::GetOuterBorderWidth() const
{
    return outerBorder_ ? outerBorder_->GetWidthFour() : Vector4f(0.f);
}

Vector4<uint32_t> RSProperties::GetOuterBorderStyle() const
{
    return outerBorder_ ? outerBorder_->GetStyleFour() : Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE));
}

Vector4f RSProperties::GetOuterBorderRadius() const
{
    return outerBorder_ ? outerBorder_->GetRadiusFour() : Vector4fZero;
}

const std::shared_ptr<RSBorder>& RSProperties::GetOuterBorder() const
{
    return outerBorder_;
}

void RSProperties::SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter)
{
    backgroundFilter_ = backgroundFilter;
    if (backgroundFilter_) {
        isDrawn_ = true;
    }
    SetDirty();
    filterNeedUpdate_ = true;
    contentDirty_ = true;
}

void RSProperties::SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para)
{
    linearGradientBlurPara_ = para;
    if (para && para->blurRadius_ > 0.f) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetDynamicLightUpRate(const std::optional<float>& rate)
{
    dynamicLightUpRate_ = rate;
    if (rate.has_value()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetDynamicLightUpDegree(const std::optional<float>& lightUpDegree)
{
    dynamicLightUpDegree_ = lightUpDegree;
    if (lightUpDegree.has_value()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetGreyCoef1(float greyCoef1)
{
    greyCoef1_ = greyCoef1;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetGreyCoef2(float greyCoef2)
{
    greyCoef2_ = greyCoef2;
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetFilter(const std::shared_ptr<RSFilter>& filter)
{
    filter_ = filter;
    if (filter) {
        isDrawn_ = true;
    }
    SetDirty();
    filterNeedUpdate_ = true;
    contentDirty_ = true;
}

const std::shared_ptr<RSFilter>& RSProperties::GetBackgroundFilter() const
{
    return backgroundFilter_;
}

const std::shared_ptr<RSLinearGradientBlurPara>& RSProperties::GetLinearGradientBlurPara() const
{
    return linearGradientBlurPara_;
}

const std::optional<float>& RSProperties::GetDynamicLightUpRate() const
{
    return dynamicLightUpRate_;
}

const std::optional<float>& RSProperties::GetDynamicLightUpDegree() const
{
    return dynamicLightUpDegree_;
}

float RSProperties::GetGreyCoef1() const
{
    return greyCoef1_;
}

float RSProperties::GetGreyCoef2() const
{
    return greyCoef2_;
}

bool RSProperties::IsGreyAdjustmenValid() const
{
    return ROSEN_GNE(greyCoef1_, 0.0) && ROSEN_LE(greyCoef1_, 127.0) &&   // 127.0 number
        ROSEN_GNE(greyCoef2_, 0.0) && ROSEN_LE(greyCoef2_, 127.0);        // 127.0 number
}

const std::shared_ptr<RSFilter>& RSProperties::GetFilter() const
{
    return filter_;
}

bool RSProperties::IsDynamicLightUpValid() const
{
    return dynamicLightUpRate_.has_value() && dynamicLightUpDegree_.has_value() &&
           ROSEN_GNE(*dynamicLightUpRate_, 0.0) && ROSEN_GE(*dynamicLightUpDegree_, -1.0) &&
           ROSEN_LE(*dynamicLightUpDegree_, 1.0);
}

// shadow properties
void RSProperties::SetShadowColor(Color color)
{
    if (!shadow_.has_value()) {
        shadow_ = std::make_optional<RSShadow>();
    }
    shadow_->SetColor(color);
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowOffsetX(float offsetX)
{
    if (!shadow_.has_value()) {
        shadow_ = std::make_optional<RSShadow>();
    }
    shadow_->SetOffsetX(offsetX);
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowOffsetY(float offsetY)
{
    if (!shadow_.has_value()) {
        shadow_ = std::make_optional<RSShadow>();
    }
    shadow_->SetOffsetY(offsetY);
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowAlpha(float alpha)
{
    if (!shadow_.has_value()) {
        shadow_ = std::make_optional<RSShadow>();
    }
    shadow_->SetAlpha(alpha);
    if (shadow_->IsValid()) {
        isDrawn_ = true;
    }
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowElevation(float elevation)
{
    if (!shadow_.has_value()) {
        shadow_ = std::make_optional<RSShadow>();
    }
    shadow_->SetElevation(elevation);
    if (shadow_->IsValid()) {
        isDrawn_ = true;
    }
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowRadius(float radius)
{
    if (!shadow_.has_value()) {
        shadow_ = std::make_optional<RSShadow>();
    }
    shadow_->SetRadius(radius);
    if (shadow_->IsValid()) {
        isDrawn_ = true;
    }
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowPath(std::shared_ptr<RSPath> shadowPath)
{
    if (!shadow_.has_value()) {
        shadow_ = std::make_optional<RSShadow>();
    }
    shadow_->SetPath(shadowPath);
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowMask(bool shadowMask)
{
    if (!shadow_.has_value()) {
        shadow_ = std::make_optional<RSShadow>();
    }
    shadow_->SetMask(shadowMask);
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowIsFilled(bool shadowIsFilled)
{
    if (!shadow_.has_value()) {
        shadow_ = std::make_optional<RSShadow>();
    }
    shadow_->SetIsFilled(shadowIsFilled);
    SetDirty();
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
}

void RSProperties::SetShadowColorStrategy(int shadowColorStrategy)
{
    if (!shadow_.has_value()) {
        shadow_ = std::make_optional<RSShadow>();
    }
    shadow_->SetColorStrategy(shadowColorStrategy);
    SetDirty();
    filterNeedUpdate_ = true;
    // [planning] if shadow stores as texture and out of node
    // node content would not be affected
    contentDirty_ = true;
    if (shadowColorStrategy != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE && colorPickerTaskShadow_ == nullptr) {
        CreateColorPickerTaskForShadow();
        colorPickerTaskShadow_->SetShadowColorStrategy(shadowColorStrategy);
    }
}


const Color& RSProperties::GetShadowColor() const
{
    static const auto DEFAULT_SPOT_COLOR_VALUE = Color::FromArgbInt(DEFAULT_SPOT_COLOR);
    return shadow_ ? shadow_->GetColor() : DEFAULT_SPOT_COLOR_VALUE;
}

float RSProperties::GetShadowOffsetX() const
{
    return shadow_ ? shadow_->GetOffsetX() : DEFAULT_SHADOW_OFFSET_X;
}

float RSProperties::GetShadowOffsetY() const
{
    return shadow_ ? shadow_->GetOffsetY() : DEFAULT_SHADOW_OFFSET_Y;
}

float RSProperties::GetShadowAlpha() const
{
    return shadow_ ? shadow_->GetAlpha() : 0.f;
}

float RSProperties::GetShadowElevation() const
{
    return shadow_ ? shadow_->GetElevation() : 0.f;
}

float RSProperties::GetShadowRadius() const
{
    return shadow_ ? shadow_->GetRadius() : DEFAULT_SHADOW_RADIUS;
}

std::shared_ptr<RSPath> RSProperties::GetShadowPath() const
{
    return shadow_ ? shadow_->GetPath() : nullptr;
}

bool RSProperties::GetShadowMask() const
{
    return shadow_ ? shadow_->GetMask() : false;
}

bool RSProperties::GetShadowIsFilled() const
{
    return shadow_ ? shadow_->GetIsFilled() : false;
}

int RSProperties::GetShadowColorStrategy() const
{
    return shadow_ ? shadow_->GetColorStrategy() : SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
}

const std::optional<RSShadow>& RSProperties::GetShadow() const
{
    return shadow_;
}

bool RSProperties::IsShadowValid() const
{
    return shadow_ && shadow_->IsValid();
}

void RSProperties::SetFrameGravity(Gravity gravity)
{
    if (frameGravity_ != gravity) {
        frameGravity_ = gravity;
        SetDirty();
        contentDirty_ = true;
    }
}

Gravity RSProperties::GetFrameGravity() const
{
    return frameGravity_;
}

void RSProperties::SetDrawRegion(const std::shared_ptr<RectF>& rect)
{
    drawRegion_ = rect;
    SetDirty();
    geoDirty_ = true;  // since drawRegion affect dirtyRegion, mark it as geoDirty
}

std::shared_ptr<RectF> RSProperties::GetDrawRegion() const
{
    return drawRegion_;
}

void RSProperties::SetClipRRect(RRect clipRRect)
{
    clipRRect_ = clipRRect;
    if (GetClipToRRect()) {
        isDrawn_ = true;
    }
    SetDirty();
    geoDirty_ = true;  // [planning] all clip ops should be checked
}

RRect RSProperties::GetClipRRect() const
{
    return clipRRect_ ? *clipRRect_ : RRect();
}

bool RSProperties::GetClipToRRect() const
{
    return clipRRect_.has_value();
}

void RSProperties::SetClipBounds(const std::shared_ptr<RSPath>& path)
{
    if (path) {
        isDrawn_ = true;
    }
    if (clipPath_ != path) {
        clipPath_ = path;
        SetDirty();
        geoDirty_ = true;  // [planning] all clip ops should be checked
    }
}

const std::shared_ptr<RSPath>& RSProperties::GetClipBounds() const
{
    return clipPath_;
}

void RSProperties::SetClipToBounds(bool clipToBounds)
{
    if (clipToBounds) {
        isDrawn_ = true;
    }
    if (clipToBounds_ != clipToBounds) {
        clipToBounds_ = clipToBounds;
        SetDirty();
        geoDirty_ = true;  // [planning] all clip ops should be checked
    }
}

bool RSProperties::GetClipToBounds() const
{
    return clipToBounds_;
}

void RSProperties::SetClipToFrame(bool clipToFrame)
{
    if (clipToFrame) {
        isDrawn_ = true;
    }
    if (clipToFrame_ != clipToFrame) {
        clipToFrame_ = clipToFrame;
        SetDirty();
        geoDirty_ = true;  // [planning] all clip ops should be checked
    }
}

bool RSProperties::GetClipToFrame() const
{
    return clipToFrame_;
}

RectF RSProperties::GetBoundsRect() const
{
    if (boundsGeo_->IsEmpty()) {
        return {0, 0, GetFrameWidth(), GetFrameHeight()};
    } else {
        return {0, 0, GetBoundsWidth(), GetBoundsHeight()};
    }
}

RectF RSProperties::GetFrameRect() const
{
    return {0, 0, GetFrameWidth(), GetFrameHeight()};
}

const RectF& RSProperties::GetBgImageRect() const
{
    return decoration_ ? decoration_->bgImageRect_ : EMPTY_RECT;
}

void RSProperties::SetVisible(bool visible)
{
    if (visible_ != visible) {
        visible_ = visible;
        SetDirty();
        contentDirty_ = true;
    }
}

bool RSProperties::GetVisible() const
{
    return visible_;
}

const RRect& RSProperties::GetRRect() const
{
    return rrect_;
}

void RSProperties::GenerateRRect()
{
    RectF rect = GetBoundsRect();
    rrect_ = RRect(rect, GetCornerRadius());
}

RRect RSProperties::GetInnerRRect() const
{
    auto rect = GetBoundsRect();
    Vector4f cornerRadius = GetCornerRadius();
    if (border_) {
        rect.left_ += border_->GetWidth(RSBorder::LEFT);
        rect.top_ += border_->GetWidth(RSBorder::TOP);
        rect.width_ -= border_->GetWidth(RSBorder::LEFT) + border_->GetWidth(RSBorder::RIGHT);
        rect.height_ -= border_->GetWidth(RSBorder::TOP) + border_->GetWidth(RSBorder::BOTTOM);
        cornerRadius = cornerRadius - GetBorderWidth();
    }
    RRect rrect = RRect(rect, cornerRadius);
    return rrect;
}

bool RSProperties::NeedFilter() const
{
    return needFilter_;
}

bool RSProperties::NeedClip() const
{
    return clipToBounds_ || clipToFrame_;
}

void RSProperties::SetDirty()
{
    isDirty_ = true;
}

void RSProperties::ResetDirty()
{
    isDirty_ = false;
    geoDirty_ = false;
    contentDirty_ = false;
}

bool RSProperties::IsDirty() const
{
    return isDirty_;
}

bool RSProperties::IsGeoDirty() const
{
    return geoDirty_;
}

bool RSProperties::IsContentDirty() const
{
    return contentDirty_;
}

RectI RSProperties::GetDirtyRect() const
{
    RectI dirtyRect;
    auto boundsGeometry = (boundsGeo_);
    if (clipToBounds_ || std::isinf(GetFrameWidth()) || std::isinf(GetFrameHeight())) {
        dirtyRect = boundsGeometry->GetAbsRect();
    } else {
        auto frameRect =
            boundsGeometry->MapAbsRect(RectF(GetFrameOffsetX(), GetFrameOffsetY(), GetFrameWidth(), GetFrameHeight()));
        dirtyRect = boundsGeometry->GetAbsRect().JoinRect(frameRect);
    }
    if (drawRegion_ == nullptr || drawRegion_->IsEmpty()) {
        return dirtyRect;
    } else {
        auto drawRegion = boundsGeometry->MapAbsRect(*drawRegion_);
        // this is used to fix the scene with drawRegion problem, which is need to be optimized
        drawRegion.SetRight(drawRegion.GetRight() + 1);
        drawRegion.SetBottom(drawRegion.GetBottom() + 1);
        drawRegion.SetAll(drawRegion.left_ - 1, drawRegion.top_ - 1,
            drawRegion.width_ + 1, drawRegion.height_ + 1);
        return dirtyRect.JoinRect(drawRegion);
    }
}

RectI RSProperties::GetDirtyRect(RectI& drawRegion) const
{
    RectI dirtyRect;
    auto boundsGeometry = (boundsGeo_);
    if (clipToBounds_ || std::isinf(GetFrameWidth()) || std::isinf(GetFrameHeight())) {
        dirtyRect = boundsGeometry->GetAbsRect();
    } else {
        auto frameRect =
            boundsGeometry->MapAbsRect(RectF(GetFrameOffsetX(), GetFrameOffsetY(), GetFrameWidth(), GetFrameHeight()));
        dirtyRect = boundsGeometry->GetAbsRect().JoinRect(frameRect);
    }
    if (drawRegion_ == nullptr || drawRegion_->IsEmpty()) {
        drawRegion = RectI();
        return dirtyRect;
    } else {
        drawRegion = boundsGeometry->MapAbsRect(*drawRegion_);
        // this is used to fix the scene with drawRegion problem, which is need to be optimized
        drawRegion.SetRight(drawRegion.GetRight() + 1);
        drawRegion.SetBottom(drawRegion.GetBottom() + 1);
        drawRegion.SetAll(drawRegion.left_ - 1, drawRegion.top_ - 1,
            drawRegion.width_ + 1, drawRegion.height_ + 1);
        return dirtyRect.JoinRect(drawRegion);
    }
}

void RSProperties::CheckEmptyBounds()
{
    // [planning] remove this func and fallback to framerect after surfacenode using frame
    if (!hasBounds_) {
        boundsGeo_->SetRect(frameGeo_->GetX(), frameGeo_->GetY(), frameGeo_->GetWidth(), frameGeo_->GetHeight());
    }
}

// mask properties
void RSProperties::SetMask(const std::shared_ptr<RSMask>& mask)
{
    mask_ = mask;
    if (mask_) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

std::shared_ptr<RSMask> RSProperties::GetMask() const
{
    return mask_;
}

void RSProperties::SetSpherize(float spherizeDegree)
{
    spherizeDegree_ = spherizeDegree;
    isSpherizeValid_ = spherizeDegree_ > SPHERIZE_VALID_EPSILON;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetSpherize() const
{
    return spherizeDegree_;
}

bool RSProperties::IsSpherizeValid() const
{
    return isSpherizeValid_;
}

void RSProperties::SetLightUpEffect(float lightUpEffectDegree)
{
    lightUpEffectDegree_ = lightUpEffectDegree;
    if (IsLightUpEffectValid()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetLightUpEffect() const
{
    return lightUpEffectDegree_;
}

bool RSProperties::IsLightUpEffectValid() const
{
    return ROSEN_GE(GetLightUpEffect(), 0.0) && ROSEN_LNE(GetLightUpEffect(), 1.0);
}

void RSProperties::SetUseEffect(bool useEffect)
{
    useEffect_ = useEffect;
    if (GetUseEffect()) {
        isDrawn_ = true;
    }
    filterNeedUpdate_ = true;
    SetDirty();
}

bool RSProperties::GetUseEffect() const
{
    return useEffect_;
}

void RSProperties::SetUseShadowBatching(bool useShadowBatching)
{
    if (useShadowBatching) {
        isDrawn_ = true;
    }
    useShadowBatching_ = useShadowBatching;
    SetDirty();
}

bool RSProperties::GetUseShadowBatching() const
{
    return useShadowBatching_;
}

void RSProperties::SetPixelStretch(const std::optional<Vector4f>& stretchSize)
{
    pixelStretch_ = stretchSize;
    SetDirty();
    pixelStretchNeedUpdate_ = true;
    contentDirty_ = true;
}

const std::optional<Vector4f>& RSProperties::GetPixelStretch() const
{
    return pixelStretch_;
}

RectI RSProperties::GetPixelStretchDirtyRect() const
{
    auto dirtyRect = GetDirtyRect();

    auto scaledBounds = RectF(dirtyRect.left_ - pixelStretch_->x_, dirtyRect.top_ - pixelStretch_->y_,
        dirtyRect.width_ + pixelStretch_->x_ + pixelStretch_->z_,
        dirtyRect.height_ + pixelStretch_->y_ + pixelStretch_->w_);

    auto scaledIBounds = RectI(std::floor(scaledBounds.left_), std::floor(scaledBounds.top_),
        std::ceil(scaledBounds.width_) + 1, std::ceil(scaledBounds.height_) + 1);
    return dirtyRect.JoinRect(scaledIBounds);
}

void RSProperties::SetPixelStretchPercent(const std::optional<Vector4f>& stretchPercent)
{
    pixelStretchPercent_ = stretchPercent;
    SetDirty();
    pixelStretchNeedUpdate_ = true;
    contentDirty_ = true;
}

const std::optional<Vector4f>& RSProperties::GetPixelStretchPercent() const
{
    return pixelStretchPercent_;
}

// Image effect properties
void RSProperties::SetGrayScale(const std::optional<float>& grayScale)
{
    grayScale_ = grayScale;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetGrayScale() const
{
    return grayScale_;
}

void RSProperties::SetLightIntensity(float lightIntensity)
{
    if (!lightSourcePtr_) {
        lightSourcePtr_ = std::make_shared<RSLightSource>();
    }
    lightSourcePtr_->SetLightIntensity(lightIntensity);
    SetDirty();
    contentDirty_ = true;

    if (ROSEN_EQ(lightIntensity, INVALID_INTENSITY)) { // skip when resetFunc call
        return;
    }
    auto preIntensity = lightSourcePtr_->GetPreLigthIntensity();
    auto renderNode = backref_.lock();
    bool preIntensityIsZero = ROSEN_EQ(preIntensity, 0.f);
    bool curIntensityIsZero = ROSEN_EQ(lightIntensity, 0.f);
    if (preIntensityIsZero && !curIntensityIsZero) { // 0 --> non-zero
        RSPointLightManager::Instance()->RegisterLightSource(renderNode);
    } else if (!preIntensityIsZero && curIntensityIsZero) { // non-zero --> 0
        RSPointLightManager::Instance()->UnRegisterLightSource(renderNode);
    }
}

void RSProperties::SetLightPosition(const Vector4f& lightPosition)
{
    if (!lightSourcePtr_) {
        lightSourcePtr_ = std::make_shared<RSLightSource>();
    }
    lightSourcePtr_->SetLightPosition(lightPosition);
    SetDirty();
    contentDirty_ = true;
}

void RSProperties::SetIlluminatedType(int illuminatedType)
{
    if (!illuminatedPtr_) {
        illuminatedPtr_ = std::make_shared<RSIlluminated>();
    }
    auto curIlluminateType = IlluminatedType(illuminatedType);
    illuminatedPtr_->SetIlluminatedType(curIlluminateType);
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;

    if (curIlluminateType == IlluminatedType::INVALID) { // skip when resetFunc call
        return;
    }
    auto renderNode = backref_.lock();
    auto preIlluminatedType = illuminatedPtr_->GetPreIlluminatedType();
    bool preTypeIsNone = preIlluminatedType == IlluminatedType::NONE;
    bool curTypeIsNone = curIlluminateType == IlluminatedType::NONE;
    if (preTypeIsNone && !curTypeIsNone) {
        RSPointLightManager::Instance()->RegisterIlluminated(renderNode);
    } else if (!preTypeIsNone && curTypeIsNone) {
        RSPointLightManager::Instance()->UnRegisterIlluminated(renderNode);
    }
}

void RSProperties::SetBloom(float bloomIntensity)
{
    if (!illuminatedPtr_) {
        illuminatedPtr_ = std::make_shared<RSIlluminated>();
    }
    illuminatedPtr_->SetBloomIntensity(bloomIntensity);
    isDrawn_ = true;
    SetDirty();
    contentDirty_ = true;
}

float RSProperties::GetLightIntensity() const
{
    return lightSourcePtr_ ? lightSourcePtr_->GetLightIntensity() : 0.f;
}

Vector4f RSProperties::GetLightPosition() const
{
    return lightSourcePtr_ ? lightSourcePtr_->GetLightPosition() : Vector4f(0.f);
}

int RSProperties::GetIlluminatedType() const
{
    return illuminatedPtr_ ? static_cast<int>(illuminatedPtr_->GetIlluminatedType()) : 0;
}

float RSProperties::GetBloom() const
{
    return illuminatedPtr_ ? illuminatedPtr_->GetBloomIntensity() : 0.f;
}

void RSProperties::CalculateAbsLightPosition()
{
    auto absRect = boundsGeo_->GetAbsRect();
    auto lightPosition = lightSourcePtr_->GetLightPosition();
    lightSourcePtr_->SetAbsLightPosition(Vector4f(
        lightPosition.x_ + absRect.left_, lightPosition.y_ + absRect.top_, lightPosition.z_, lightPosition.w_));
}

const std::shared_ptr<RSLightSource>& RSProperties::GetLightSource() const
{
    return lightSourcePtr_;
}

const std::shared_ptr<RSIlluminated>& RSProperties::GetIlluminated() const
{
    return illuminatedPtr_;
}

void RSProperties::SetBrightness(const std::optional<float>& brightness)
{
    brightness_ = brightness;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetBrightness() const
{
    return brightness_;
}

void RSProperties::SetContrast(const std::optional<float>& contrast)
{
    contrast_ = contrast;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetContrast() const
{
    return contrast_;
}

void RSProperties::SetSaturate(const std::optional<float>& saturate)
{
    saturate_ = saturate;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetSaturate() const
{
    return saturate_;
}

void RSProperties::SetSepia(const std::optional<float>& sepia)
{
    sepia_ = sepia;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetSepia() const
{
    return sepia_;
}

void RSProperties::SetInvert(const std::optional<float>& invert)
{
    invert_ = invert;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetInvert() const
{
    return invert_;
}


void RSProperties::SetAiInvert(const std::optional<Vector4f>& aiInvert)
{
    aiInvert_ = aiInvert;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
    isDrawn_ = true;
}

const std::optional<Vector4f>& RSProperties::GetAiInvert() const
{
    return aiInvert_;
}

void RSProperties::SetHueRotate(const std::optional<float>& hueRotate)
{
    hueRotate_ = hueRotate;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<float>& RSProperties::GetHueRotate() const
{
    return hueRotate_;
}

void RSProperties::SetColorBlend(const std::optional<Color>& colorBlend)
{
    colorBlend_ = colorBlend;
    colorFilterNeedUpdate_ = true;
    SetDirty();
    contentDirty_ = true;
}

const std::optional<Color>& RSProperties::GetColorBlend() const
{
    return colorBlend_;
}

static bool GreatNotEqual(double left, double right)
{
    constexpr double epsilon = 0.001f;
    return (left - right) > epsilon;
}

static bool NearEqual(const double left, const double right)
{
    constexpr double epsilon = 0.001f;
    return (std::abs(left - right) <= epsilon);
}

static bool GreatOrEqual(double left, double right)
{
    constexpr double epsilon = -0.001f;
    return (left - right) > epsilon;
}

#ifndef USE_ROSEN_DRAWING
const sk_sp<SkColorFilter>& RSProperties::GetColorFilter() const
#else
const std::shared_ptr<Drawing::ColorFilter>& RSProperties::GetColorFilter() const
#endif
{
    return colorFilter_;
}

void RSProperties::GenerateColorFilter()
{
    // No update needed if color filter is valid
    if (!colorFilterNeedUpdate_) {
        return;
    }

    colorFilterNeedUpdate_ = false;
    colorFilter_ = nullptr;
    if (!grayScale_ && !brightness_ && !contrast_ && !saturate_ && !sepia_ && !invert_ && !hueRotate_ && !colorBlend_) {
        return;
    }

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkColorFilter> filter = nullptr;
#else
    std::shared_ptr<Drawing::ColorFilter> filter = nullptr;
#endif

    if (grayScale_.has_value() && GreatNotEqual(*grayScale_, 0.f)) {
        auto grayScale = grayScale_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = matrix[INDEX_5] = matrix[INDEX_10] = 0.2126f * grayScale; // 0.2126 : gray scale coefficient
        matrix[1] = matrix[INDEX_6] = matrix[INDEX_11] = 0.7152f * grayScale; // 0.7152 : gray scale coefficient
        matrix[INDEX_2] = matrix[INDEX_7] = matrix[INDEX_12] = 0.0722f * grayScale; // 0.0722 : gray scale coefficient
        matrix[INDEX_18] = 1.0 * grayScale;
#ifndef USE_ROSEN_DRAWING
        filter = SkColorFilters::Matrix(matrix);
        colorFilter_ = filter->makeComposed(colorFilter_);
#else
        Drawing::ColorMatrix colorMatrix;
        colorMatrix.SetArray(matrix);
        filter = Drawing::ColorFilter::CreateMatrixColorFilter(colorMatrix);
        if (colorFilter_) {
            filter->Compose(*colorFilter_);
        }
        colorFilter_ = filter;
#endif
    }
    if (brightness_.has_value() && !NearEqual(*brightness_, 1.0)) {
        auto brightness = brightness_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        // shift brightness to (-1, 1)
        brightness = brightness - 1;
        matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = matrix[INDEX_18] = 1.0f;
        matrix[INDEX_4] = matrix[INDEX_9] = matrix[INDEX_14] = brightness;
#ifndef USE_ROSEN_DRAWING
        filter = SkColorFilters::Matrix(matrix);
        colorFilter_ = filter->makeComposed(colorFilter_);
#else
        Drawing::ColorMatrix colorMatrix;
        colorMatrix.SetArray(matrix);
        filter = Drawing::ColorFilter::CreateMatrixColorFilter(colorMatrix);
        if (colorFilter_) {
            filter->Compose(*colorFilter_);
        }
        colorFilter_ = filter;
#endif
    }
    if (contrast_.has_value() && !NearEqual(*contrast_, 1.0)) {
        auto contrast = contrast_.value();
        uint32_t contrastValue128 = 128;
        uint32_t contrastValue255 = 255;
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = contrast;
        matrix[INDEX_4] = matrix[INDEX_9] = matrix[INDEX_14] = contrastValue128 * (1 - contrast) / contrastValue255;
        matrix[INDEX_18] = 1.0f;
#ifndef USE_ROSEN_DRAWING
        filter = SkColorFilters::Matrix(matrix);
        colorFilter_ = filter->makeComposed(colorFilter_);
#else
        Drawing::ColorMatrix colorMatrix;
        colorMatrix.SetArray(matrix);
        filter = Drawing::ColorFilter::CreateMatrixColorFilter(colorMatrix);
        if (colorFilter_) {
            filter->Compose(*colorFilter_);
        }
        colorFilter_ = filter;
#endif
    }
    if (saturate_.has_value() && !NearEqual(*saturate_, 1.0) && GreatOrEqual(*saturate_, 0.0)) {
        auto saturate = saturate_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = 0.3086f * (1 - saturate) + saturate; // 0.3086 : saturate coefficient
        matrix[1] = matrix[INDEX_11] = 0.6094f * (1 - saturate); // 0.6094 : saturate coefficient
        matrix[INDEX_2] = matrix[INDEX_7] = 0.0820f * (1 - saturate); // 0.0820 : saturate coefficient
        matrix[INDEX_5] = matrix[INDEX_10] = 0.3086f * (1 - saturate); // 0.3086 : saturate coefficient
        matrix[INDEX_6] = 0.6094f * (1 - saturate) + saturate; // 0.6094 : saturate coefficient
        matrix[INDEX_12] = 0.0820f * (1 - saturate) + saturate; // 0.0820 : saturate coefficient
        matrix[INDEX_18] = 1.0f;
#ifndef USE_ROSEN_DRAWING
        filter = SkColorFilters::Matrix(matrix);
        colorFilter_ = filter->makeComposed(colorFilter_);
#else
        Drawing::ColorMatrix colorMatrix;
        colorMatrix.SetArray(matrix);
        filter = Drawing::ColorFilter::CreateMatrixColorFilter(colorMatrix);
        if (colorFilter_) {
            filter->Compose(*colorFilter_);
        }
        colorFilter_ = filter;
#endif
    }
    if (sepia_.has_value() && GreatNotEqual(*sepia_, 0.0)) {
        auto sepia = sepia_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        matrix[0] = 0.393f * sepia;
        matrix[1] = 0.769f * sepia;
        matrix[INDEX_2] = 0.189f * sepia;

        matrix[INDEX_5] = 0.349f * sepia;
        matrix[INDEX_6] = 0.686f * sepia;
        matrix[INDEX_7] = 0.168f * sepia;

        matrix[INDEX_10] = 0.272f * sepia;
        matrix[INDEX_11] = 0.534f * sepia;
        matrix[INDEX_12] = 0.131f * sepia;
        matrix[INDEX_18] = 1.0f * sepia;
#ifndef USE_ROSEN_DRAWING
        filter = SkColorFilters::Matrix(matrix);
        colorFilter_ = filter->makeComposed(colorFilter_);
#else
        Drawing::ColorMatrix colorMatrix;
        colorMatrix.SetArray(matrix);
        filter = Drawing::ColorFilter::CreateMatrixColorFilter(colorMatrix);
        if (colorFilter_) {
            filter->Compose(*colorFilter_);
        }
        colorFilter_ = filter;
#endif
    }
    if (invert_.has_value() && GreatNotEqual(*invert_, 0.0)) {
        auto invert = invert_.value();
        float matrix[20] = { 0.0f }; // 20 : matrix size
        if (invert > 1.0) {
            invert = 1.0;
        }
        // complete color invert when dstRGB = 1 - srcRGB
        // map (0, 1) to (1, -1)
        matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = 1.0 - 2.0 * invert; // 2.0: invert
        matrix[INDEX_18] = 1.0f;
        // invert = 0.5 -> RGB = (0.5, 0.5, 0.5) -> image completely gray
        matrix[INDEX_4] = matrix[INDEX_9] = matrix[INDEX_14] = invert;
#ifndef USE_ROSEN_DRAWING
        filter = SkColorFilters::Matrix(matrix);
        colorFilter_ = filter->makeComposed(colorFilter_);
#else
        Drawing::ColorMatrix colorMatrix;
        colorMatrix.SetArray(matrix);
        filter = Drawing::ColorFilter::CreateMatrixColorFilter(colorMatrix);
        if (colorFilter_) {
            filter->Compose(*colorFilter_);
        }
        colorFilter_ = filter;
#endif
    }
    if (hueRotate_.has_value() && GreatNotEqual(*hueRotate_, 0.0)) {
        auto hueRotate = hueRotate_.value();
        while (GreatOrEqual(hueRotate, 360)) { // 360 : degree
            hueRotate -= 360; // 360 : degree
        }
        float matrix[20] = { 0.0f }; // 20 : matrix size
        int32_t type = hueRotate / 120; // 120 : degree
        float N = (hueRotate - 120 * type) / 120; // 120 : degree
        switch (type) {
            case 0:
                // color change = R->G, G->B, B->R
                matrix[INDEX_2] = matrix[INDEX_5] = matrix[INDEX_11] = N;
                matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = 1 - N;
                matrix[INDEX_18] = 1.0f;
                break;
            case 1:
                // compare to original: R->B, G->R, B->G
                matrix[1] = matrix[INDEX_7] = matrix[INDEX_10] = N;
                matrix[INDEX_2] = matrix[INDEX_5] = matrix[INDEX_11] = 1 - N;
                matrix[INDEX_18] = 1.0f;
                break;
            case 2: // 2: back to normal color
                matrix[0] = matrix[INDEX_6] = matrix[INDEX_12] = N;
                matrix[1] = matrix[INDEX_7] = matrix[INDEX_10] = 1 - N;
                matrix[INDEX_18] = 1.0f;
                break;
            default:
                break;
        }
#ifndef USE_ROSEN_DRAWING
        filter = SkColorFilters::Matrix(matrix);
        colorFilter_ = filter->makeComposed(colorFilter_);
#else
        Drawing::ColorMatrix colorMatrix;
        colorMatrix.SetArray(matrix);
        filter = Drawing::ColorFilter::CreateMatrixColorFilter(colorMatrix);
        if (colorFilter_) {
            filter->Compose(*colorFilter_);
        }
        colorFilter_ = filter;
#endif
    }
    if (colorBlend_.has_value() && *colorBlend_ != RgbPalette::Transparent()) {
        auto colorBlend = colorBlend_.value();
#ifndef USE_ROSEN_DRAWING
        filter = SkColorFilters::Blend(
            SkColorSetARGB(colorBlend.GetAlpha(), colorBlend.GetRed(), colorBlend.GetGreen(), colorBlend.GetBlue()),
            SkBlendMode::kPlus);
        colorFilter_ = filter->makeComposed(colorFilter_);
#else
        filter = Drawing::ColorFilter::CreateBlendModeColorFilter(Drawing::Color::ColorQuadSetARGB(
            colorBlend.GetRed(), colorBlend.GetGreen(), colorBlend.GetBlue(), colorBlend.GetAlpha()),
            Drawing::BlendMode::PLUS);
        if (colorFilter_) {
            filter->Compose(*colorFilter_);
        }
        colorFilter_ = filter;
#endif
    }
    isDrawn_ = true;
}

std::string RSProperties::Dump() const
{
    std::string dumpInfo;
    char buffer[UINT8_MAX] = { 0 };
    if (sprintf_s(buffer, UINT8_MAX, "Bounds[%.1f %.1f %.1f %.1f] Frame[%.1f %.1f %.1f %.1f]",
        GetBoundsPositionX(), GetBoundsPositionY(), GetBoundsWidth(), GetBoundsHeight(),
        GetFramePositionX(), GetFramePositionY(), GetFrameWidth(), GetFrameHeight()) != -1) {
        dumpInfo.append(buffer);
    }

    // PositionZ
    auto ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for PositionZ, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetPositionZ(), 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", PositionZ[%.1f]", GetPositionZ()) != -1) {
        dumpInfo.append(buffer);
    }

    // Pivot
    std::unique_ptr<Transform> defaultTrans = std::make_unique<Transform>();
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Pivot, ret=" + std::to_string(ret);
    }
    Vector2f pivot = GetPivot();
    if ((!ROSEN_EQ(pivot[0], defaultTrans->pivotX_) || !ROSEN_EQ(pivot[1], defaultTrans->pivotY_)) &&
        sprintf_s(buffer, UINT8_MAX, ", Pivot[%.1f,%.1f]", pivot[0], pivot[1]) != -1) {
        dumpInfo.append(buffer);
    }

    // CornerRadius
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for CornerRadius, ret=" + std::to_string(ret);
    }
    if (!GetCornerRadius().IsZero() &&
        sprintf_s(buffer, UINT8_MAX, ", CornerRadius[%.1f %.1f %.1f %.1f]",
            GetCornerRadius().x_, GetCornerRadius().y_, GetCornerRadius().z_, GetCornerRadius().w_) != -1) {
        dumpInfo.append(buffer);
    }

    // PixelStretch PixelStretchPercent
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for PixelStretch, ret=" + std::to_string(ret);
    }
    if (pixelStretch_.has_value() &&
        sprintf_s(buffer, UINT8_MAX, ", PixelStretch[left:%.1f top:%.1f right:%.1f bottom:%.1f]",
            pixelStretch_->x_, pixelStretch_->y_, pixelStretch_->z_, pixelStretch_->w_) != -1) {
        dumpInfo.append(buffer);
    }

    // Rotation
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Rotation, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetRotation(), defaultTrans->rotation_) &&
        sprintf_s(buffer, UINT8_MAX, ", Rotation[%.1f]", GetRotation()) != -1) {
        dumpInfo.append(buffer);
    }
    // RotationX
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for RotationX, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetRotationX(), defaultTrans->rotationX_) &&
        sprintf_s(buffer, UINT8_MAX, ", RotationX[%.1f]", GetRotationX()) != -1) {
        dumpInfo.append(buffer);
    }
    // RotationY
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for RotationY, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetRotationY(), defaultTrans->rotationY_) &&
        sprintf_s(buffer, UINT8_MAX, ", RotationY[%.1f]", GetRotationY()) != -1) {
        dumpInfo.append(buffer);
    }

    // TranslateX
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for TranslateX, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetTranslateX(), defaultTrans->translateX_) &&
        sprintf_s(buffer, UINT8_MAX, ", TranslateX[%.1f]", GetTranslateX()) != -1) {
        dumpInfo.append(buffer);
    }

    // TranslateY
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for TranslateY, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetTranslateY(), defaultTrans->translateY_) &&
        sprintf_s(buffer, UINT8_MAX, ", TranslateY[%.1f]", GetTranslateY()) != -1) {
        dumpInfo.append(buffer);
    }

    // TranslateZ
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for TranslateZ, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetTranslateZ(), defaultTrans->translateZ_) &&
        sprintf_s(buffer, UINT8_MAX, ", TranslateZ[%.1f]", GetTranslateZ()) != -1) {
        dumpInfo.append(buffer);
    }

    // ScaleX
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ScaleX, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetScaleX(), defaultTrans->scaleX_) &&
        sprintf_s(buffer, UINT8_MAX, ", ScaleX[%.1f]", GetScaleX()) != -1) {
        dumpInfo.append(buffer);
    }

    // ScaleY
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ScaleY, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetScaleY(), defaultTrans->scaleY_) &&
        sprintf_s(buffer, UINT8_MAX, ", ScaleY[%.1f]", GetScaleY()) != -1) {
        dumpInfo.append(buffer);
    }

    // Alpha
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Alpha, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetAlpha(), 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Alpha[%.1f]", GetAlpha()) != -1) {
        dumpInfo.append(buffer);
    }

    // Spherize
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Spherize, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetSpherize(), 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Spherize[%.1f]", GetSpherize()) != -1) {
        dumpInfo.append(buffer);
    }

    // LightUpEffect
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for LightUpEffect, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetLightUpEffect(), 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", LightUpEffect[%.1f]", GetLightUpEffect()) != -1) {
        dumpInfo.append(buffer);
    }

    // ForegroundColor
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ForegroundColor, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetForegroundColor(), RgbPalette::Transparent()) &&
        sprintf_s(buffer, UINT8_MAX, ", ForegroundColor[#%08X]", GetForegroundColor().AsArgbInt()) != -1) {
        dumpInfo.append(buffer);
    }

    // BackgroundColor
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for BackgroundColor, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetBackgroundColor(), RgbPalette::Transparent()) &&
        sprintf_s(buffer, UINT8_MAX, ", BackgroundColor[#%08X]", GetBackgroundColor().AsArgbInt()) != -1) {
        dumpInfo.append(buffer);
    }

    // BgImage
    std::unique_ptr<Decoration> defaultDecoration = std::make_unique<Decoration>();
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for BgImage, ret=" + std::to_string(ret);
    }
    if ((!ROSEN_EQ(GetBgImagePositionX(), defaultDecoration->bgImageRect_.left_) ||
        !ROSEN_EQ(GetBgImagePositionY(), defaultDecoration->bgImageRect_.top_) ||
        !ROSEN_EQ(GetBgImageWidth(), defaultDecoration->bgImageRect_.width_) ||
        !ROSEN_EQ(GetBgImageHeight(), defaultDecoration->bgImageRect_.height_)) &&
        sprintf_s(buffer, UINT8_MAX, ", BgImage[%.1f %.1f %.1f %.1f]", GetBgImagePositionX(),
            GetBgImagePositionY(), GetBgImageWidth(), GetBgImageHeight()) != -1) {
        dumpInfo.append(buffer);
    }

    // Border
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Border, ret=" + std::to_string(ret);
    }
    if (border_ && border_->HasBorder() &&
        sprintf_s(buffer, UINT8_MAX, ", Border[%s]", border_->ToString().c_str()) != -1) {
        dumpInfo.append(buffer);
    }

    // Filter
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Filter, ret=" + std::to_string(ret);
    }
    auto filter_ = GetFilter();
    if (filter_ && filter_->IsValid() &&
        sprintf_s(buffer, UINT8_MAX, ", Filter[%s]", filter_->GetDescription().c_str()) != -1) {
        dumpInfo.append(buffer);
    }

    // BackgroundFilter
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for BackgroundFilter, ret=" + std::to_string(ret);
    }
    auto backgroundFilter_ = GetBackgroundFilter();
    if (backgroundFilter_ && backgroundFilter_->IsValid() &&
        sprintf_s(buffer, UINT8_MAX, ", BackgroundFilter[%s]", backgroundFilter_->GetDescription().c_str()) != -1) {
        dumpInfo.append(buffer);
    }

    // OuterBorder
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for OuterBorder, ret=" + std::to_string(ret);
    }
    if (outerBorder_ && outerBorder_->HasBorder() &&
        sprintf_s(buffer, UINT8_MAX, ", OuterBorder[%s]", outerBorder_->ToString().c_str()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowColor
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowColor, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowColor(), Color(DEFAULT_SPOT_COLOR)) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowColor[#%08X]", GetShadowColor().AsArgbInt()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowOffsetX
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowOffsetX, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowOffsetX(), DEFAULT_SHADOW_OFFSET_X) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowOffsetX[%.1f]", GetShadowOffsetX()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowOffsetY
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowOffsetY, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowOffsetY(), DEFAULT_SHADOW_OFFSET_Y) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowOffsetY[%.1f]", GetShadowOffsetY()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowAlpha
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowAlpha, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowAlpha(), 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowAlpha[%.1f]", GetShadowAlpha()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowElevation
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowElevation, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowElevation(), 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowElevation[%.1f]", GetShadowElevation()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowRadius
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowRadius, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowRadius(), 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowRadius[%.1f]", GetShadowRadius()) != -1) {
        dumpInfo.append(buffer);
    }

    // ShadowIsFilled
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ShadowIsFilled, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetShadowIsFilled(), false) &&
        sprintf_s(buffer, UINT8_MAX, ", ShadowIsFilled[%d]", GetShadowIsFilled()) != -1) {
        dumpInfo.append(buffer);
    }

    // FrameGravity
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for FrameGravity, ret=" + std::to_string(ret);
    }
    if (!ROSEN_EQ(GetFrameGravity(), Gravity::DEFAULT) &&
        sprintf_s(buffer, UINT8_MAX, ", FrameGravity[%d]", GetFrameGravity()) != -1) {
        dumpInfo.append(buffer);
    }

    // IsVisible
    if (!GetVisible()) {
        dumpInfo.append(", IsVisible[false]");
    }

    // Gray Scale
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for GrayScale, ret=" + std::to_string(ret);
    }
    auto grayScale = GetGrayScale();
    if (grayScale.has_value() && !ROSEN_EQ(*grayScale, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", GrayScale[%.1f]", *grayScale) != -1) {
        dumpInfo.append(buffer);
    }

    // DynamicLightUpRate
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for DynamicLightUpRate, ret=" + std::to_string(ret);
    }
    auto dynamicLightUpRate = GetDynamicLightUpRate();
    if (dynamicLightUpRate.has_value() && !ROSEN_EQ(*dynamicLightUpRate, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", DynamicLightUpRate[%.1f]", *dynamicLightUpRate) != -1) {
        dumpInfo.append(buffer);
    }

    // DynamicLightUpDegree
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for DynamicLightUpDegree, ret=" + std::to_string(ret);
    }
    auto dynamicLightUpDegree = GetDynamicLightUpDegree();
    if (dynamicLightUpDegree.has_value() && !ROSEN_EQ(*dynamicLightUpDegree, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", DynamicLightUpDegree[%.1f]", *dynamicLightUpDegree) != -1) {
        dumpInfo.append(buffer);
    }

    // Brightness
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Brightness, ret=" + std::to_string(ret);
    }
    auto brightness = GetBrightness();
    if (brightness.has_value() && !ROSEN_EQ(*brightness, 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Brightness[%.1f]", *brightness) != -1) {
        dumpInfo.append(buffer);
    }

    // Contrast
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Contrast, ret=" + std::to_string(ret);
    }
    auto contrast = GetContrast();
    if (contrast.has_value() && !ROSEN_EQ(*contrast, 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Contrast[%.1f]", *contrast) != -1) {
        dumpInfo.append(buffer);
    }

    // Saturate
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Saturate, ret=" + std::to_string(ret);
    }
    auto saturate = GetSaturate();
    if (saturate.has_value() && !ROSEN_EQ(*saturate, 1.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Saturate[%.1f]", *saturate) != -1) {
        dumpInfo.append(buffer);
    }

    // Sepia
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Sepia, ret=" + std::to_string(ret);
    }
    auto sepia = GetSepia();
    if (sepia.has_value() && !ROSEN_EQ(*sepia, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Sepia[%.1f]", *sepia) != -1) {
        dumpInfo.append(buffer);
    }

    // Invert
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for Invert, ret=" + std::to_string(ret);
    }
    auto invert = GetInvert();
    if (invert.has_value() && !ROSEN_EQ(*invert, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", Invert[%.1f]", *invert) != -1) {
        dumpInfo.append(buffer);
    }

    // Hue Rotate
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for HueRotate, ret=" + std::to_string(ret);
    }
    auto hueRotate = GetHueRotate();
    if (hueRotate.has_value() && !ROSEN_EQ(*hueRotate, 0.f) &&
        sprintf_s(buffer, UINT8_MAX, ", HueRotate[%.1f]", *hueRotate) != -1) {
        dumpInfo.append(buffer);
    }

    // Color Blend
    ret = memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (ret != EOK) {
        return "Failed to memset_s for ColorBlend, ret=" + std::to_string(ret);
    }
    auto colorBlend = GetColorBlend();
    if (colorBlend.has_value() && !ROSEN_EQ(*colorBlend, RgbPalette::Transparent()) &&
        sprintf_s(buffer, UINT8_MAX, ", ColorBlend[#%08X]", colorBlend->AsArgbInt()) != -1) {
        dumpInfo.append(buffer);
    }

    return dumpInfo;
}

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
void RSProperties::CreateFilterCacheManagerIfNeed()
{
    if (!FilterCacheEnabled) {
        return;
    }
    if (auto& filter = GetBackgroundFilter()) {
        auto& cacheManager = backgroundFilterCacheManager_;
        if (cacheManager == nullptr) {
            cacheManager = std::make_unique<RSFilterCacheManager>();
        }
        cacheManager->UpdateCacheStateWithFilterHash(filter);
    } else {
        if (backgroundFilterCacheManager_ != nullptr) {
            backgroundFilterCacheManager_->ReleaseCacheOffTree();
        }
        backgroundFilterCacheManager_.reset();
    }
    if (auto& filter = GetFilter()) {
        auto& cacheManager = foregroundFilterCacheManager_;
        if (cacheManager == nullptr) {
            cacheManager = std::make_unique<RSFilterCacheManager>();
        }
        cacheManager->UpdateCacheStateWithFilterHash(filter);
    } else {
        if (foregroundFilterCacheManager_ != nullptr) {
            foregroundFilterCacheManager_->ReleaseCacheOffTree();
        }
        foregroundFilterCacheManager_.reset();
    }
}

const std::unique_ptr<RSFilterCacheManager>& RSProperties::GetFilterCacheManager(bool isForeground) const
{
    return isForeground ? foregroundFilterCacheManager_ : backgroundFilterCacheManager_;
}

void RSProperties::ClearFilterCache()
{
    if (foregroundFilterCacheManager_ != nullptr) {
        foregroundFilterCacheManager_->ReleaseCacheOffTree();
    }
    if (backgroundFilterCacheManager_ != nullptr) {
        backgroundFilterCacheManager_->ReleaseCacheOffTree();
    }
    if (backgroundFilter_ != nullptr && (backgroundFilter_->GetFilterType() == RSFilter::MATERIAL)) {
        auto filter = std::static_pointer_cast<RSMaterialFilter>(backgroundFilter_);
        filter->ReleaseColorPicker();
    }
    if (filter_ != nullptr && (filter_->GetFilterType() == RSFilter::MATERIAL)) {
        auto filter = std::static_pointer_cast<RSMaterialFilter>(filter_);
        filter->ReleaseColorPicker();
    }
}

void RSProperties::CreateColorPickerTaskForShadow()
{
    if (colorPickerTaskShadow_ == nullptr) {
        colorPickerTaskShadow_ = std::make_shared<RSColorPickerCacheTask>();
    }
}

#endif

void RSProperties::OnApplyModifiers()
{
    if (geoDirty_) {
        if (!hasBounds_) {
            CheckEmptyBounds();
        } else {
            CalculateFrameOffset();
        }
        // frame and bounds are the same, no need to clip twice
        if (clipToFrame_ && clipToBounds_ && frameOffsetX_ == 0 && frameOffsetY_ == 0) {
            clipToFrame_ = false;
        }
    }
    if (colorFilterNeedUpdate_) {
        GenerateColorFilter();
    }
    if (pixelStretchNeedUpdate_ || geoDirty_) {
        CalculatePixelStretch();
    }
    if (filterNeedUpdate_) {
        filterNeedUpdate_ = false;
        if (IsShadowValid()) {
            filterNeedUpdate_ = true;
        }
        if (backgroundFilter_ != nullptr && !backgroundFilter_->IsValid()) {
            backgroundFilter_.reset();
        }
        if (filter_ != nullptr && !filter_->IsValid()) {
            filter_.reset();
        }
        needFilter_ = backgroundFilter_ != nullptr || filter_ != nullptr || useEffect_ || IsLightUpEffectValid() ||
                        IsDynamicLightUpValid() || IsShadowValid() || IsGreyAdjustmenValid();
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
        CreateFilterCacheManagerIfNeed();
#endif
    }
    GenerateRRect();
}

void RSProperties::CalculatePixelStretch()
{
    pixelStretchNeedUpdate_ = false;
    // no pixel stretch
    if (!pixelStretch_.has_value() && !pixelStretchPercent_.has_value()) {
        return;
    }
    // convert pixel stretch percent to pixel stretch
    if (pixelStretchPercent_) {
        auto width = GetBoundsWidth();
        auto height = GetBoundsHeight();
        if (isinf(width) || isinf(height)) {
            return;
        }
        pixelStretch_ = *pixelStretchPercent_ * Vector4f(width, height, width, height);
    }
    constexpr static float EPS = 1e-5f;
    // parameter check: near zero
    if (abs(pixelStretch_->x_) < EPS && abs(pixelStretch_->y_) < EPS && abs(pixelStretch_->z_) < EPS &&
        abs(pixelStretch_->w_) < EPS) {
        pixelStretch_ = std::nullopt;
        return;
    }
    // parameter check: all >= 0 or all <= 0
    if ((pixelStretch_->x_ < EPS && pixelStretch_->y_ < EPS && pixelStretch_->z_ < EPS && pixelStretch_->w_ < EPS) ||
        (pixelStretch_->x_ > -EPS && pixelStretch_->y_ > -EPS && pixelStretch_->z_ > -EPS &&
            pixelStretch_->w_ > -EPS)) {
        isDrawn_ = true;
        return;
    }
    pixelStretch_ = std::nullopt;
}

void RSProperties::CalculateFrameOffset()
{
    frameOffsetX_ = frameGeo_->GetX() - boundsGeo_->GetX();
    frameOffsetY_ = frameGeo_->GetY() - boundsGeo_->GetY();
    if (isinf(frameOffsetX_)) {
        frameOffsetX_ = 0.;
    }
    if (isinf(frameOffsetY_)) {
        frameOffsetY_ = 0.;
    }
    if (frameOffsetX_ != 0. || frameOffsetY_ != 0.) {
        isDrawn_ = true;
    }
}

// blend with background
void RSProperties::SetColorBlendMode(int colorBlendMode)
{
    colorBlendMode_ = colorBlendMode;
    if (colorBlendMode_ != static_cast<int>(RSColorBlendModeType::NONE)) {
        isDrawn_ = true;
    }
    SetDirty();
    contentDirty_ = true;
}

int RSProperties::GetColorBlendMode() const
{
    return colorBlendMode_;
}

const std::shared_ptr<RSColorPickerCacheTask>& RSProperties::GetColorPickerCacheTaskShadow() const
{
    return colorPickerTaskShadow_;
}

} // namespace Rosen
} // namespace OHOS
