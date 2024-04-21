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

#include "render/rs_drawing_filter.h"
 
#include <memory>
#include "draw/blend_mode.h"
#include "ge_render.h"
#include "ge_visual_effect.h"
#include "src/core/SkOpts.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
 
namespace OHOS {
namespace Rosen {
RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter, uint32_t hash)
    : RSFilter(), imageFilter_(imageFilter)
{
    imageFilterHash_ = hash;
    shaderFilters_.reserve(10);
}
 
RSDrawingFilter::RSDrawingFilter(std::shared_ptr<RSShaderFilter> shaderFilter)
{
    hash_ = shaderFilter->Hash();
    shaderFilters_.reserve(10);
    shaderFilters_.emplace_back(shaderFilter);
}
 
RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
    std::shared_ptr<RSShaderFilter> shaderFilter, uint32_t hash) : RSFilter(), imageFilter_(imageFilter)
{
    hash_ = shaderFilter->Hash();
    imageFilterHash_ = hash;
    shaderFilters_.reserve(10);
    shaderFilters_.emplace_back(shaderFilter);
}
 
RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter,
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters, uint32_t hash)
    : RSFilter(), imageFilter_(imageFilter)
{
    shaderFilters_ = shaderFilters;
    for (const auto& shaderFilter : shaderFilters_) {
        uint32_t hash = shaderFilter->Hash();
        hash_ = SkOpts::hash(&hash, sizeof(hash), hash_);
    }
    imageFilterHash_ = hash;
}
 
RSDrawingFilter::~RSDrawingFilter() {};
 
Drawing::Brush RSDrawingFilter::GetBrush() const
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetImageFilter(imageFilter_);
    brush.SetFilter(filter);
    return brush;
}
 
bool RSDrawingFilter::CanSkipFrame(float radius)
{
    constexpr float HEAVY_BLUR_THRESHOLD = 25.0f;
    return radius > HEAVY_BLUR_THRESHOLD;
}

uint32_t RSDrawingFilter::Hash() const
{
    auto hash = SkOpts::hash(&imageFilterHash_, sizeof(imageFilterHash_), hash_);
    return hash;
}

uint32_t RSDrawingFilter::ShaderHash() const
{
    return hash_;
}

uint32_t RSDrawingFilter::ImageHash() const
{
    return imageFilterHash_;
}

std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(const std::shared_ptr<RSDrawingFilter> other) const
{
    std::shared_ptr<RSDrawingFilter> result = std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_, imageFilterHash_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
    for (auto item : other->GetShaderFilters()) {
        result->InsertShaderFilter(item);
    }
    auto otherShaderHash = other->ShaderHash();
    auto otherImageHash = other->ImageHash();
    result->hash_ = SkOpts::hash(&otherShaderHash, sizeof(otherShaderHash), hash_);
    result->imageFilterHash_ = SkOpts::hash(&otherImageHash, sizeof(otherImageHash), imageFilterHash_);
    return result;
}
 
std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(const std::shared_ptr<RSShaderFilter> other) const
{
    std::shared_ptr<RSDrawingFilter> result = std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_, imageFilterHash_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->InsertShaderFilter(other);
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}
 
std::shared_ptr<RSDrawingFilter> RSDrawingFilter::Compose(const std::shared_ptr<Drawing::ImageFilter> other, uint32_t hash) const
{
    std::shared_ptr<RSDrawingFilter> result = std::make_shared<RSDrawingFilter>(imageFilter_, shaderFilters_, imageFilterHash_);
    result->hash_ = hash_;
    if (other == nullptr) {
        return result;
    }
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other);
    result->imageFilterHash_ = SkOpts::hash(&hash, sizeof(hash), imageFilterHash_);
    return result;
}
 
std::shared_ptr<Drawing::ImageFilter> RSDrawingFilter::GetImageFilter() const
{
    return imageFilter_;
}
 
void RSDrawingFilter::SetImageFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter)
{
    imageFilter_ = imageFilter;
}

std::shared_ptr<RSShaderFilter> RSDrawingFilter::GetShaderFilterWithType(RSShaderFilter::ShaderFilterType type)
{
    for (const auto& shaderFilter : shaderFilters_) {
        if (shaderFilter->GetShaderFilterType() == type) {
            return shaderFilter;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<RSShaderFilter>> RSDrawingFilter::GetShaderFilters() const
{
    return shaderFilters_; 
}
 
void RSDrawingFilter::InsertShaderFilter(std::shared_ptr<RSShaderFilter> shaderFilter)
{
    shaderFilters_.emplace_back(shaderFilter); 
}
 
void RSDrawingFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    std::for_each(shaderFilters_.begin(), shaderFilters_.end(), [&](auto& filter) {
        filter->GenerateGEVisualEffect(visualEffectContainer);
    });
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    auto outImage = geRender->ApplyImageEffect(canvas, *visualEffectContainer, image, src, src, Drawing::SamplingOptions());
    auto brush = GetBrush();
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*outImage, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
}
 
void RSDrawingFilter::PreProcess(std::shared_ptr<Drawing::Image>& image) {
    std::for_each(shaderFilters_.begin(), shaderFilters_.end(), [&](auto& filter) {
        filter->PreProcess(image);
    });
}
 
void RSDrawingFilter::PostProcess(Drawing::Canvas& canvas) {
    std::for_each(shaderFilters_.begin(), shaderFilters_.end(), [&](auto& filter) {
        filter->PostProcess(canvas);
    });
}
} // namespace Rosen
} // namespace OHOS