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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_BOUNDS_GEOMETRY_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_BOUNDS_GEOMETRY_H

#include <utility>

#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkRRect.h"

#include "common/rs_color.h"
#include "common/rs_vector4.h"
#include "property/rs_property_drawable.h"
#include "render/rs_mask.h"

namespace OHOS::Rosen {
class RSSkiaFilter;
struct RSCachedEffectData;
class RSRenderParticle;

class RSBoundsGeometryDrawable : public RSPropertyDrawable {
public:
    explicit RSBoundsGeometryDrawable() = default;
    ~RSBoundsGeometryDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
    bool Update(RSPropertyDrawableGenerateContext& context) override { return true; }

    static RSPropertyDrawable::DrawablePtr Generate(const RSPropertyDrawableGenerateContext& context);
};

class RSClipBoundsDrawable : public RSPropertyDrawable {
public:
    explicit RSClipBoundsDrawable() = default;
    ~RSClipBoundsDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
    bool Update(RSPropertyDrawableGenerateContext& context) override;

    static RSPropertyDrawable::DrawablePtr Generate(const RSPropertyDrawableGenerateContext& context);
};

// ============================================================================
// Border
class RSBorderDrawable : public RSPropertyDrawable {
public:
    explicit RSBorderDrawable(SkPaint&& paint) : RSPropertyDrawable(), paint_(std::move(paint)) {}
    ~RSBorderDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSPropertyDrawableGenerateContext& context);

protected:
    SkPaint paint_;
};

class RSBorderDRRectDrawable : public RSBorderDrawable {
public:
    explicit RSBorderDRRectDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderDRRectDrawable() override = default;
    void OnBoundsChange(const RSProperties& properties);
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

private:
    SkRRect inner_;
    SkRRect outer_;
};

class RSBorderFourLineDrawable : public RSBorderDrawable {
public:
    explicit RSBorderFourLineDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderFourLineDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
};

class RSBorderPathDrawable : public RSBorderDrawable {
public:
    explicit RSBorderPathDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderPathDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
    void OnBoundsChange(const RSProperties& properties);

private:
    SkPath borderPath_;
};

class RSBorderFourLineRoundCornerDrawable : public RSBorderDrawable {
public:
    explicit RSBorderFourLineRoundCornerDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderFourLineRoundCornerDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
    void OnBoundsChange(const RSProperties& properties);

private:
    SkRRect innerRrect_;
    SkRRect rrect_;
};

// ============================================================================
// Mask
class RSMaskDrawable : public RSPropertyDrawable {
public:
    explicit RSMaskDrawable(std::shared_ptr<RSMask> mask);
    ~RSMaskDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

protected:
    std::shared_ptr<RSMask> mask_;
    SkPaint maskFilter_;
    SkPaint maskPaint_;
};

class RSSvgDomMaskDrawable : public RSMaskDrawable {
public:
    explicit RSSvgDomMaskDrawable(std::shared_ptr<RSMask> mask);
    ~RSSvgDomMaskDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
};

class RSSvgPictureMaskDrawable : public RSMaskDrawable {
public:
    explicit RSSvgPictureMaskDrawable(std::shared_ptr<RSMask> mask);
    ~RSSvgPictureMaskDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
};

class RSGradientMaskDrawable : public RSMaskDrawable {
public:
    explicit RSGradientMaskDrawable(std::shared_ptr<RSMask> mask);
    ~RSGradientMaskDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
};

class RSPathMaskDrawable : public RSMaskDrawable {
public:
    explicit RSPathMaskDrawable(std::shared_ptr<RSMask> mask);
    ~RSPathMaskDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
};

// ============================================================================
// Shadow
class RSShadowBaseDrawable : public RSPropertyDrawable {
public:
    explicit RSShadowBaseDrawable(SkPath skPath, const RSProperties& properties);
    ~RSShadowBaseDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

protected:
    SkPath skPath_;
    float offsetX_;
    float offsetY_;
    Color color_;
};

class RSShadowDrawable : public RSShadowBaseDrawable {
public:
    explicit RSShadowDrawable(SkPath skPath, const RSProperties& properties);
    ~RSShadowDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

protected:
    float radius_;
};

class RSColorfulShadowDrawable : public RSShadowBaseDrawable {
public:
    explicit RSColorfulShadowDrawable(SkPath skPath, const RSProperties& properties);
    ~RSColorfulShadowDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

protected:
    SkPaint blurPaint_;
    std::weak_ptr<RSRenderNode> node_;
};

class RSHardwareAccelerationShadowDrawable : public RSShadowBaseDrawable {
public:
    explicit RSHardwareAccelerationShadowDrawable(SkPath skPath, const RSProperties& properties);
    ~RSHardwareAccelerationShadowDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

protected:
    float shadowElevation_;
};

// ============================================================================
// DynamicLightUp
class RSDynamicLightUpDrawable : public RSPropertyDrawable {
public:
    explicit RSDynamicLightUpDrawable(float rate, float degree);
    ~RSDynamicLightUpDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

protected:
    float rate_;
    float degree_;
};

// ============================================================================
// RSEffectDataApplyDrawable
class RSEffectDataApplyDrawable : public RSPropertyDrawable {
public:
    explicit RSEffectDataApplyDrawable() = default;
    ~RSEffectDataApplyDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
};

// ============================================================================
// LightUpEffect
class RSLightUpEffectDrawable : public RSPropertyDrawable {
public:
    explicit RSLightUpEffectDrawable() = default;
    ~RSLightUpEffectDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
};

class RSFilterDrawable : public RSPropertyDrawable {
public:
    enum FilterType {
        BACKGROUND_FILTER,
        FOREGROUND_FILTER,
    };
    explicit RSFilterDrawable(const std::shared_ptr<RSFilter>& filter) : filter_(filter) {}
    ~RSFilterDrawable() override = default;
    void DrawFilter(RSRenderNode& node, RSPaintFilterCanvas& canvas, FilterType filterType);
    const std::shared_ptr<RSCachedEffectData> GeneratedCachedEffectData(
        RSRenderNode& node, RSPaintFilterCanvas& canvas);
    static bool GetBlurEnabled();

protected:
    const std::shared_ptr<RSFilter>& filter_;
};

// ============================================================================
// BackgroundFilter
class RSBackgroundFilterDrawable : public RSFilterDrawable {
public:
    explicit RSBackgroundFilterDrawable(const std::shared_ptr<RSFilter>& filter) : RSFilterDrawable(filter) {}
    ~RSBackgroundFilterDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
};

// ============================================================================
// ForegroundFilter
class RSForegroundFilterDrawable : public RSFilterDrawable {
public:
    explicit RSForegroundFilterDrawable(const std::shared_ptr<RSFilter>& filter) : RSFilterDrawable(filter) {}
    ~RSForegroundFilterDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
};

// ============================================================================
// BackgroundEffect
class RSEffectDataGenerateDrawable : public RSFilterDrawable {
public:
    explicit RSEffectDataGenerateDrawable(const std::shared_ptr<RSFilter>& filter) : RSFilterDrawable(filter) {}
    ~RSEffectDataGenerateDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
};

// ============================================================================
// LinearGradientBlurFilter
class RSLinearGradientBlurFilterDrawable : public RSPropertyDrawable {
public:
    explicit RSLinearGradientBlurFilterDrawable(const std::shared_ptr<RSLinearGradientBlurPara>& para) : para_(para) {}
    ~RSLinearGradientBlurFilterDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

private:
    const std::shared_ptr<RSLinearGradientBlurPara>& para_;
};

// ============================================================================
// ForegroundColor
class RSForegroundColorDrawable : public RSPropertyDrawable {
public:
    explicit RSForegroundColorDrawable(SkPaint&& paint) : paint_(std::move(paint)) {}
    ~RSForegroundColorDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

private:
    SkPaint paint_;
};

// ============================================================================
// Particle
// PLANNING:
// 1. move bounds to RSParticleDrawable, and use OnBoundsChange to update it.
// 2. more refactor
class RSParticleDrawable : public RSPropertyDrawable {
public:
    explicit RSParticleDrawable() = default;
    ~RSParticleDrawable() override = default;
    void AddPropertyDrawable(std::shared_ptr<RSPropertyDrawable> drawable);
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

private:
    std::vector<std::shared_ptr<RSPropertyDrawable>> propertyDrawableVec_ = {};
};

class RSPointParticleDrawable : public RSPropertyDrawable {
public:
    explicit RSPointParticleDrawable(SkPaint&& paint, std::shared_ptr<RSRenderParticle> particles)
        : paint_(std::move(paint)), particles_(std::move(particles))
    {}
    ~RSPointParticleDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

private:
    SkPaint paint_;
    std::shared_ptr<RSRenderParticle> particles_;
};

class RSImageParticleDrawable : public RSPropertyDrawable {
public:
    explicit RSImageParticleDrawable(SkPaint&& paint, std::shared_ptr<RSRenderParticle> particles)
        : paint_(std::move(paint)), particles_(std::move(particles))
    {}
    ~RSImageParticleDrawable() override = default;
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

private:
    SkPaint paint_;
    std::shared_ptr<RSRenderParticle> particles_;
};

// ============================================================================
// PixelStretch
class RSPixelStretchDrawable : public RSPropertyDrawable {
public:
    explicit RSPixelStretchDrawable(const Vector4f& pixelStretch) : pixelStretch_(pixelStretch) {}
    ~RSPixelStretchDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

private:
    Vector4f pixelStretch_;
};

// ============================================================================
// Background
class RSBackgroundDrawable : public RSPropertyDrawable {
public:
    explicit RSBackgroundDrawable(bool hasRoundedCorners)
    {
        paint_.setAntiAlias(forceBgAntiAlias_ || hasRoundedCorners);
    }
    ~RSBackgroundDrawable() override = default;
    static void setForceBgAntiAlias(bool antiAlias);
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

protected:
    SkPaint paint_;
    static bool forceBgAntiAlias_;
};

class RSBackgroundColorDrawable : public RSBackgroundDrawable {
public:
    explicit RSBackgroundColorDrawable(bool hasRoundedCorners, SkColor color) : RSBackgroundDrawable(hasRoundedCorners)
    {
        paint_.setColor(color);
    }
    ~RSBackgroundColorDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
};

class RSBackgroundShaderDrawable : public RSBackgroundDrawable {
public:
    explicit RSBackgroundShaderDrawable(bool hasRoundedCorners, sk_sp<SkShader> filter)
        : RSBackgroundDrawable(hasRoundedCorners)
    {
        paint_.setShader(std::move(filter));
    }
    ~RSBackgroundShaderDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
};

class RSBackgroundImageDrawable : public RSBackgroundDrawable {
public:
    explicit RSBackgroundImageDrawable(bool hasRoundedCorners, std::shared_ptr<RSImage> image)
        : RSBackgroundDrawable(hasRoundedCorners), image_(std::move(image))
    {}
    ~RSBackgroundImageDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas) override;

private:
    std::shared_ptr<RSImage> image_;
};
};     // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_BOUNDS_GEOMETRY_H
