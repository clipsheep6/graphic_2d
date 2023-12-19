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
#include "render/rs_linear_gradient_blur_filter.h"

#include "src/core/SkOpts.h"

#include "platform/common/rs_log.h"
#ifdef USE_ROSEN_DRAWING
#include "effect/color_matrix.h"
#endif

namespace OHOS {
namespace Rosen {
RSLinearGradientBlurFilter::RSLinearGradientBlurFilter(RSLinearGradientBlurPara& rsLinearGradientBlurPara)
    : rsLinearGradientBlurPara_(rsLinearGradientBlurPara) {};

RSLinearGradientBlurFilter::~RSLinearGradientBlurFilter() = default;

std::shared_ptr<RSLinearGradientBlurPara> RSLinearGradientBlurFilter::GetRSLinearGradientBlurPara()
{
    return rsLinearGradientBlurPara_;
}

void RSLinearGradientBlurFilter::TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias)
{
    if (direction == static_cast<uint8_t>(GradientDirection::LEFT_BOTTOM)) {
        direction += 2; // 2 is used to transtorm diagnal direction.
    } else if (direction == static_cast<uint8_t>(GradientDirection::RIGHT_TOP) ||
                    direction == static_cast<uint8_t>(GradientDirection::RIGHT_BOTTOM)) {
        direction -= 1; // 1 is used to transtorm diagnal direction.
    }
    if (direction <= static_cast<uint8_t>(GradientDirection::BOTTOM)) {
        if (direction < directionBias) {
            direction += DIRECTION_NUM;
        }
        direction -= directionBias;
    } else {
        direction -= DIRECTION_NUM;
        if (direction < directionBias) {
            direction += DIRECTION_NUM;
        }
        direction -= directionBias;
        direction += DIRECTION_NUM;
    }
    if (direction == static_cast<uint8_t>(GradientDirection::RIGHT_BOTTOM)) {
        direction -= 2; // 2 is used to restore diagnal direction.
    } else if (direction == static_cast<uint8_t>(GradientDirection::LEFT_BOTTOM) ||
                        direction == static_cast<uint8_t>(GradientDirection::RIGHT_TOP)) {
        direction += 1; // 1 is used to restore diagnal direction.
    }
}

#ifndef USE_ROSEN_DRAWING
bool RSLinearGradientBlurFilter::GetGradientDirectionPoints(
    SkPoint (&pts)[2], const SkRect& clipBounds, GradientDirection direction)
#else
bool RSLinearGradientBlurFilter::GetGradientDirectionPoints(
    Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GradientDirection direction)
#endif
{
    switch (direction) {
        case GradientDirection::BOTTOM: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(clipBounds.width() / 2, 0); // 2 represents middle of width;
            pts[1].set(clipBounds.width() / 2, clipBounds.height()); // 2 represents middle of width;
#else
            pts[0].Set(clipBounds.GetWidth() / 2, 0); // 2 represents middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2, clipBounds.GetHeight()); // 2 represents middle of width;
#endif
            break;
        }
        case GradientDirection::TOP: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(clipBounds.width() / 2, clipBounds.height()); // 2 represents middle of width;
            pts[1].set(clipBounds.width() / 2, 0); // 2 represents middle of width;
#else
            pts[0].Set(clipBounds.GetWidth() / 2, clipBounds.GetHeight()); // 2 represents middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2, 0); // 2 represents middle of width;
#endif
            break;
        }
        case GradientDirection::RIGHT: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(0, clipBounds.height() / 2); // 2 represents middle of height;
            pts[1].set(clipBounds.width(), clipBounds.height() / 2); // 2 represents middle of height;
#else
            pts[0].Set(0, clipBounds.GetHeight() / 2); // 2 represents middle of height;
            pts[1].Set(clipBounds.GetWidth(), clipBounds.GetHeight() / 2); // 2 represents middle of height;
#endif
            break;
        }
        case GradientDirection::LEFT: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(clipBounds.width(), clipBounds.height() / 2); // 2 represents middle of height;
            pts[1].set(0, clipBounds.height() / 2); // 2 represents middle of height;
#else
            pts[0].Set(clipBounds.GetWidth(), clipBounds.GetHeight() / 2); // 2 represents middle of height;
            pts[1].Set(0, clipBounds.GetHeight() / 2); // 2 represents middle of height;
#endif
            break;
        }
        case GradientDirection::RIGHT_BOTTOM: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(0, 0);
            pts[1].set(clipBounds.width(), clipBounds.height());
#else
            pts[0].Set(0, 0);
            pts[1].Set(clipBounds.GetWidth(), clipBounds.GetHeight());
#endif
            break;
        }
        case GradientDirection::LEFT_TOP: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(clipBounds.width(), clipBounds.height());
            pts[1].set(0, 0);
#else
            pts[0].Set(clipBounds.GetWidth(), clipBounds.GetHeight());
            pts[1].Set(0, 0);
#endif
            break;
        }
        case GradientDirection::LEFT_BOTTOM: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(clipBounds.width(), 0);
            pts[1].set(0, clipBounds.height());
#else
            pts[0].Set(clipBounds.GetWidth(), 0);
            pts[1].Set(0, clipBounds.GetHeight());
#endif
            break;
        }
        case GradientDirection::RIGHT_TOP: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(0, clipBounds.height());
            pts[1].set(clipBounds.width(), 0);
#else
            pts[0].Set(0, clipBounds.GetHeight());
            pts[1].Set(clipBounds.GetWidth(), 0);
#endif
            break;
        }
        default: {
            return false;
        }
    }
    return true;
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSLinearGradientBlurFilter::MakeAlphaGradientShader(
    const SkRect& clipBounds, const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias)
{
    std::vector<SkColor> c;
    std::vector<SkScalar> p;
    SkPoint pts[2];
#else
std::shared_ptr<Drawing::ShaderEffect> RSLinearGradientBlurFilter::MakeAlphaGradientShader(
    const Drawing::Rect& clipBounds, const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias)
{
    std::vector<Drawing::ColorQuad> c;
    std::vector<Drawing::scalar> p;
    Drawing::Point pts[2];
#endif

    uint8_t direction = static_cast<uint8_t>(para->direction_);
    if (directionBias != 0) {
        TransformGradientBlurDirection(direction, directionBias);
    }
    bool result = GetGradientDirectionPoints(
        pts, clipBounds, static_cast<GradientDirection>(direction));
    if (!result) {
        return nullptr;
    }
    uint8_t ColorMax = 255;
    uint8_t ColorMin = 0;
    if (para->fractionStops_[0].second > 0.01) {  // 0.01 represents the fraction bias
#ifndef USE_ROSEN_DRAWING
        c.emplace_back(SkColorSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
#else
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
#endif
        p.emplace_back(para->fractionStops_[0].second - 0.01); // 0.01 represents the fraction bias
    }
    for (size_t i = 0; i < para->fractionStops_.size(); i++) {
#ifndef USE_ROSEN_DRAWING
        c.emplace_back(SkColorSetARGB(
            static_cast<uint8_t>(para->fractionStops_[i].first * ColorMax), ColorMax, ColorMax, ColorMax));
#else
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(
            static_cast<uint8_t>(para->fractionStops_[i].first * ColorMax), ColorMax, ColorMax, ColorMax));
#endif
        p.emplace_back(para->fractionStops_[i].second);
    }
    // 0.01 represents the fraction bias
    if (para->fractionStops_[para->fractionStops_.size() - 1].second < (1 - 0.01)) {
#ifndef USE_ROSEN_DRAWING
        c.emplace_back(SkColorSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
#else
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
#endif
        // 0.01 represents the fraction bias
        p.emplace_back(para->fractionStops_[para->fractionStops_.size() - 1].second + 0.01);
    }
#ifndef USE_ROSEN_DRAWING
    auto shader = SkGradientShader::MakeLinear(pts, &c[0], &p[0], p.size(), SkTileMode::kClamp);
    return shader;
#else
    return Drawing::ShaderEffect::CreateLinearGradient(pts[0], pts[1], c, p, Drawing::TileMode::CLAMP);
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSLinearGradientBlurFilter::MakeHorizontalMeanBlurShader(
    float radiusIn, sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader)
#else
std::shared_ptr<Drawing::ShaderEffect> RSLinearGradientBlurFilter::MakeHorizontalMeanBlurShader(float radiusIn,
    std::shared_ptr<Drawing::ShaderEffect> shader, std::shared_ptr<Drawing::ShaderEffect> gradientShader)
#endif
{
    const char* prog = R"(
        uniform half r;
        uniform shader imageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord, half radius)
        {
            half4 sum = vec4(0.0);
            half div = 0;
            for (half x = -30.0; x < 30.0; x += 1.0) {
                if (x > radius) {
                    break;
                }
                if (abs(x) < radius) {
                    div += 1;
                    sum += imageShader.eval(coord + float2(x, 0));
                }
            }
            return half4(sum.xyz / div, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a - 0) < 0.001) {
                return imageShader.eval(coord);
            }
            float val = clamp(r * gradientShader.eval(coord).a, 1.0, r);
            return meanFilter(coord, val);
        }
    )";
#ifndef USE_ROSEN_DRAWING
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    sk_sp<SkShader> children[] = {shader, gradientShader};
    size_t childCount = 2;
    return effect->makeShader(SkData::MakeWithCopy(
        &radiusIn, sizeof(radiusIn)), children, childCount, nullptr, false);
#else
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(prog);
    std::shared_ptr<Drawing::ShaderEffect> children[] = {shader, gradientShader};
    size_t childCount = 2;
    std::shared_ptr<Drawing::Data> data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&radiusIn, sizeof(radiusIn));
    return effect->MakeShader(data, children, childCount, nullptr, false);
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSLinearGradientBlurFilter::MakeVerticalMeanBlurShader(
    float radiusIn, sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader)
#else
std::shared_ptr<Drawing::ShaderEffect> RSLinearGradientBlurFilter::MakeVerticalMeanBlurShader(float radiusIn,
    std::shared_ptr<Drawing::ShaderEffect> shader, std::shared_ptr<Drawing::ShaderEffect> gradientShader)
#endif
{
    const char* prog = R"(
        uniform half r;
        uniform shader imageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord, half radius)
        {
            half4 sum = vec4(0.0);
            half div = 0;
            for (half y = -30.0; y < 30.0; y += 1.0) {
                if (y > radius) {
                    break;
                }
                if (abs(y) < radius) {
                    div += 1;
                    sum += imageShader.eval(coord + float2(0, y));
                }
            }
            return half4(sum.xyz / div, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a - 0) < 0.001) {
                return imageShader.eval(coord);
            }
            float val = clamp(r * gradientShader.eval(coord).a, 1.0, r);
            return meanFilter(coord, val);
        }
    )";
#ifndef USE_ROSEN_DRAWING
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    sk_sp<SkShader> children[] = {shader, gradientShader};
    size_t childCount = 2;
    return effect->makeShader(SkData::MakeWithCopy(
        &radiusIn, sizeof(radiusIn)), children, childCount, nullptr, false);
#else
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(prog);
    std::shared_ptr<Drawing::ShaderEffect> children[] = {shader, gradientShader};
    size_t childCount = 2;
    std::shared_ptr<Drawing::Data> data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&radiusIn, sizeof(radiusIn));
    return effect->MakeShader(data, children, childCount, nullptr, false);
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSLinearGradientBlurFilter::DrawHorizontalLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
    float radius, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding)
{
    auto image = skSurface->makeImageSnapshot(clipIPadding);
    if (image == nullptr) {
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = MakeHorizontalMeanBlurShader(radius, imageShader, alphaGradientShader);
    SkPaint paint;
    paint.setShader(shader);
    canvas.drawRect(SkRect::Make(clipIPadding.makeOffset(-clipIPadding.left(), -clipIPadding.top())), paint);
}
#else
void RSLinearGradientBlurFilter::DrawHorizontalLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
    float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::RectI& clipIPadding)
{
    auto image = surface->GetImageSnapshot(clipIPadding);
    if (image == nullptr) {
        return;
    }
    Drawing::Matrix m;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), m);
    auto shader = MakeHorizontalMeanBlurShader(radius, imageShader, alphaGradientShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    Drawing::Rect rect = clipIPadding;
    rect.Offset(-clipIPadding.GetLeft(), -clipIPadding.GetTop());
    canvas.DrawRect(rect);
    canvas.DetachBrush();
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSLinearGradientBlurFilter::DrawVerticalLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
    float radius, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding)
{
    auto image = skSurface->makeImageSnapshot(clipIPadding);
    if (image == nullptr) {
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = MakeVerticalMeanBlurShader(radius, imageShader, alphaGradientShader);
    SkPaint paint;
    paint.setShader(shader);
    canvas.drawRect(SkRect::Make(clipIPadding.makeOffset(-clipIPadding.left(), -clipIPadding.top())), paint);
}
#else
void RSLinearGradientBlurFilter::DrawVerticalLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
    float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::RectI& clipIPadding)
{
    auto image = surface->GetImageSnapshot(clipIPadding);
    if (image == nullptr) {
        return;
    }
    Drawing::Matrix m;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), m);
    auto shader = MakeVerticalMeanBlurShader(radius, imageShader, alphaGradientShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    Drawing::Rect rect = clipIPadding;
    rect.Offset(-clipIPadding.GetLeft(), -clipIPadding.GetTop());
    canvas.DrawRect(rect);
    canvas.DetachBrush();
}
#endif

#ifndef USE_ROSEN_DRAWING
uint8_t RSLinearGradientBlurFilter::CalcDirectionBias(const SkMatrix& mat)
{
    uint8_t directionBias = 0;
    // 1 and 3 represents rotate matrix's index
    if ((mat.get(1) > FLOAT_ZERO_THRESHOLD) && (mat.get(3) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 1; // 1 represents rotate 90 degree
    // 0 and 4 represents rotate matrix's index
    } else if ((mat.get(0) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.get(4) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 2; // 2 represents rotate 180 degree
    // 1 and 3 represents rotate matrix's index
    } else if ((mat.get(1) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.get(3) > FLOAT_ZERO_THRESHOLD)) {
        directionBias = 3; // 3 represents rotate 270 degree
    }
    return directionBias;
}
#else
uint8_t RSLinearGradientBlurFilter::CalcDirectionBias(const Drawing::Matrix& mat)
{
    uint8_t directionBias = 0;
    // 1 and 3 represents rotate matrix's index
    if ((mat.Get(1) > FLOAT_ZERO_THRESHOLD) && (mat.Get(3) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 1; // 1 represents rotate 90 degree
    // 0 and 4 represents rotate matrix's index
    } else if ((mat.Get(0) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(4) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 2; // 2 represents rotate 180 degree
    // 1 and 3 represents rotate matrix's index
    } else if ((mat.Get(1) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(3) > FLOAT_ZERO_THRESHOLD)) {
        directionBias = 3; // 3 represents rotate 270 degree
    }
    return directionBias;
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSLinearGradientBlurFilter::DrawLinearGradientBlur(SkSurface* surface, RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSLinearGradientBlurPara>& para, sk_sp<SkShader> alphaGradientShader,
    const SkIRect& clipIPadding)
#else
void RSLinearGradientBlurFilter::DrawLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSLinearGradientBlurPara>& para, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
    const Drawing::RectI& clipIPadding)
#endif
{
    if (para == nullptr) {
        ROSEN_LOGE("RSLinearGradientBlurFilter::DrawLinearGradientBlur para null");
        return;
    }

    if (RSSystemProperties::GetMaskLinearBlurEnabled()) {
        // use faster LinearGradientBlur if valid
        RS_OPTIONAL_TRACE_NAME("DrawLinearGradientBlur_mask");
        if (para->LinearGradientBlurFilter_ == nullptr) {
            ROSEN_LOGE("RSLinearGradientBlurFilter::DrawLinearGradientBlur blurFilter null");
            return;
        }
        auto& RSFilter = para->LinearGradientBlurFilter_;
#ifndef USE_ROSEN_DRAWING
        auto filter = std::static_pointer_cast<RSSkiaFilter>(RSFilter);
#else
        auto filter = std::static_pointer_cast<RSDrawingFilter>(RSFilter);
#endif
        DrawMaskLinearGradientBlur(surface, canvas, filter, alphaGradientShader, clipIPadding);
    } else {
        // use original LinearGradientBlur
        RS_OPTIONAL_TRACE_NAME("DrawLinearGradientBlur_original");
        float radius = para->blurRadius_ / 2;
        DrawHorizontalLinearGradientBlur(surface, canvas, radius, alphaGradientShader, clipIPadding);
        DrawVerticalLinearGradientBlur(surface, canvas, radius, alphaGradientShader, clipIPadding);
        DrawHorizontalLinearGradientBlur(surface, canvas, radius, alphaGradientShader, clipIPadding);
        DrawVerticalLinearGradientBlur(surface, canvas, radius, alphaGradientShader, clipIPadding);
    }
}

#ifndef USE_ROSEN_DRAWING
void RSLinearGradientBlurFilter::DrawMaskLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
    std::shared_ptr<RSSkiaFilter>& blurFilter, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding)
{
    auto image = skSurface->makeImageSnapshot(clipIPadding);
    if (image == nullptr) {
        ROSEN_LOGE("RSLinearGradientBlurFilter::DrawMaskLinearGradientBlur image is null");
        return;
    }

    auto offscreenRect = clipIPadding;
    auto offscreenSurface = skSurface->makeSurface(offscreenRect.width(), offscreenRect.height());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    blurFilter->DrawImageRect(offscreenCanvas, image, SkRect::Make(image->bounds().makeOutset(-1, -1)),
        SkRect::Make(image->bounds()));
    auto filteredSnapshot = offscreenSurface->makeImageSnapshot();
    auto srcImageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto blurImageShader = filteredSnapshot->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = MakeMeanBlurShader(srcImageShader, blurImageShader, alphaGradientShader);

    SkPaint paint;
    paint.setShader(shader);
    canvas.drawRect(SkRect::Make(clipIPadding.makeOffset(-clipIPadding.left(), -clipIPadding.top())), paint);
}
#else
void RSLinearGradientBlurFilter::DrawMaskLinearGradientBlur(Drawing::Surface* drSurface, RSPaintFilterCanvas& canvas,
    std::shared_ptr<RSDrawingFilter>& blurFilter, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
    const Drawing::RectI& clipIPadding)
{
    auto image = drSurface->GetImageSnapshot(clipIPadding);
    if (image == nullptr) {
        ROSEN_LOGE("RSLinearGradientBlurFilter::DrawMaskLinearGradientBlur image is null");
        return;
    }

    auto offscreenRect = clipIPadding;
    auto offscreenSurface = drSurface->MakeSurface(offscreenRect.GetWidth(), offscreenRect.GetHeight());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    auto imageInfo = image->GetImageInfo();
    auto tmpRect = Drawing::Rect(0, 0, imageInfo.GetWidth(), imageInfo.GetHeight());
    auto dstRect = tmpRect;
    tmpRect.MakeOutset(-1, -1);
    auto srcRect = Drawing::Rect(tmpRect.GetLeft(), tmpRect.GetTop(), tmpRect.GetRight(), tmpRect.GetBottom());
    blurFilter->DrawImageRect(offscreenCanvas, image, srcRect, dstRect);
    auto filteredSnapshot = offscreenSurface->GetImageSnapshot();
    Drawing::Matrix matrix;
    auto srcImageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    auto blurImageShader = Drawing::ShaderEffect::CreateImageShader(*filteredSnapshot, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    auto shader = MakeMeanBlurShader(srcImageShader, blurImageShader, alphaGradientShader);

    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    Drawing::Rect rect = clipIPadding;
    rect.Offset(-clipIPadding.GetLeft(), -clipIPadding.GetTop());
    canvas.AttachBrush(brush);
    canvas.DrawRect(rect);
    canvas.DetachBrush();
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSLinearGradientBlurFilter::MakeMeanBlurShader(sk_sp<SkShader> srcImageShader,
        sk_sp<SkShader> blurImageShader, sk_sp<SkShader> gradientShader)
#else
std::shared_ptr<Drawing::ShaderEffect> RSLinearGradientBlurFilter::MakeMeanBlurShader(
    std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> blurImageShader,
    std::shared_ptr<Drawing::ShaderEffect> gradientShader)
#endif
{
    const char* prog = R"(
        uniform shader srcImageShader;
        uniform shader blurImageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord)
        {
            vec3 srcColor = vec3(srcImageShader.eval(coord).r,
                srcImageShader.eval(coord).g, srcImageShader.eval(coord).b);
            vec3 blurColor = vec3(blurImageShader.eval(coord).r,
                blurImageShader.eval(coord).g, blurImageShader.eval(coord).b);
            float gradient = gradientShader.eval(coord).a;

            vec3 color = blurColor * gradient + srcColor * (1 - gradient);
            return vec4(color, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a) < 0.001) {
                return srcImageShader.eval(coord);
            }
            
            if (abs(gradientShader.eval(coord).a) > 0.999) {
                return blurImageShader.eval(coord);
            }

            return meanFilter(coord);
        }
    )";

#ifndef USE_ROSEN_DRAWING
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    if (!effect) {
        ROSEN_LOGE("MakeMeanBlurShader::RuntimeShader effect error: %{public}s\n", err.c_str());
        return nullptr;
    }

    SkRuntimeShaderBuilder builder(effect);
    builder.child("srcImageShader") = srcImageShader;
    builder.child("blurImageShader") = blurImageShader;
    builder.child("gradientShader") = gradientShader;
    return builder.makeShader(nullptr, false);
#else
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!effect) {
        ROSEN_LOGE("MakeMeanBlurShader::RuntimeShader effect error\n");
        return nullptr;
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    builder->SetChild("srcImageShader", srcImageShader);
    builder->SetChild("blurImageShader", blurImageShader);
    builder->SetChild("gradientShader", gradientShader);
    return builder->MakeShader(nullptr, false);
#endif
}

void RSLinearGradientBlurFilter::PreProcess(sk_sp<SkImage> image)
{
    return;
}

void RSLinearGradientBlurFilter::PostProcess(RSPaintFilterCanvas& canvas)
{
    return;
}

sk_sp<SkImage> RSLinearGradientBlurFilter::ProcessImage(SkCanvas& canvas, const sk_sp<SkImage>& image) const
{
#ifndef USE_ROSEN_DRAWING
    SkSurface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);

    auto clipBounds = canvas.getDeviceClipBounds();
    auto clipIPadding = clipBounds.makeOutset(-1, -1);
    SkMatrix mat = canvas.getTotalMatrix();
#else
    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        return;
    }
    Drawing::AutoCanvasRestore acr(canvas, true);
    
    auto clipBounds = canvas.GetDeviceClipBounds();
    auto clipIPadding = clipBounds;
    clipBounds.MakeOutset(-1, -1);
    Drawing::Matrix mat = canvas.GetTotalMatrix();
#endif
    uint8_t directionBias = CalcDirectionBias(mat);

#ifndef USE_ROSEN_DRAWING
    auto alphaGradientShader = MakeAlphaGradientShader(SkRect::Make(clipIPadding), para, directionBias);
#else
    auto alphaGradientShader = MakeAlphaGradientShader(clipIPadding, para, directionBias);
#endif
    if (alphaGradientShader == nullptr) {
        ROSEN_LOGE("RSLinearGradientBlurFilter::DrawLinearGradientBlurFilter alphaGradientShader null");
        return;
    }

#ifndef USE_ROSEN_DRAWING
    canvas.resetMatrix();
    canvas.translate(clipIPadding.left(), clipIPadding.top());
#else
    canvas.ResetMatrix();
    canvas.Translate(clipIPadding.GetLeft(), clipIPadding.GetTop());
#endif

    DrawLinearGradientBlur(surface, canvas, para, alphaGradientShader, clipIPadding);
}
} // namespace Rosen
} // namespace OHOS
