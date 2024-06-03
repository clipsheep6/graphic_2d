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

#include "platform/common/rs_log.h"
#include "render/rs_particles_drawable.h"
#include "render/rs_pixel_map_util.h"

namespace OHOS {
namespace Rosen {
constexpr float DEGREE_TO_RADIAN = M_PI / 180;
constexpr float DOUBLE = 2.f;
constexpr float DEFAULT_RADIUS = 100;
RSParticlesDrawable::RSParticlesDrawable(const std::vector<std::shared_ptr<RSRenderParticle>>& particles,
    std::vector<std::shared_ptr<RSImage>>& imageVector, size_t imageCount)
    : particles_(particles), imageVector_(imageVector), imageCount_(imageCount)
{
    count_.resize(imageCount_);
    imageRsxform_.resize(imageCount_);
    imageTex_.resize(imageCount_);
    imageColors_.resize(imageCount_);
}

std::shared_ptr<Drawing::Image> RSParticlesDrawable::MakeCircleImage(int radius)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(radius * DOUBLE, radius * DOUBLE, format);
    bitmap.ClearWithColor(Drawing::Color::COLOR_TRANSPARENT);
    auto recordingCanvas = Drawing::Canvas();
    recordingCanvas.Bind(bitmap);
    Drawing::Brush brush;
    brush.SetBlendMode(Drawing::BlendMode::CLEAR);
    Drawing::Rect r = { 0, 0, radius * DOUBLE, radius * DOUBLE };
    recordingCanvas.AttachBrush(brush);
    recordingCanvas.DrawRect(r);
    recordingCanvas.DetachBrush();
    brush.SetColor(Drawing::Color::COLOR_BLACK);
    brush.SetBlendMode(Drawing::BlendMode::SRC_OVER);
    brush.SetAntiAlias(true);
    recordingCanvas.AttachBrush(brush);
    recordingCanvas.DrawOval(r);
    recordingCanvas.DetachBrush();
    return bitmap.MakeImage();
}

/**
 *  A compressed form of a rotation+scale matrix.
 *
 *  [ cos     -sin    tx ]
 *  [ sin      cos    ty ]
 *  [  0        0      1 ]
 */
Drawing::RSXform RSParticlesDrawable::MakeRSXform(Vector2f center, Vector2f position, float spin, float scale)
{
    float cos = std::cos(spin * DEGREE_TO_RADIAN) * scale;
    float sin = std::sin(spin * DEGREE_TO_RADIAN) * scale;
    float tx = position.x_ - cos * center.x_ + sin * center.y_;
    float ty = position.y_ - sin * center.x_ - cos * center.y_;
    return Drawing::RSXform::Make(cos, sin, tx, ty);
}

void RSParticlesDrawable::CaculatePointAtlsArry(
    const std::shared_ptr<RSRenderParticle>& particle, Vector2f position, float opacity, float scale)
{
    if (circleImage_ == nullptr) {
        circleImage_ = MakeCircleImage(DEFAULT_RADIUS);
    }
    auto radius = particle->GetRadius();
    Color color = particle->GetColor();
    auto alpha = color.GetAlpha();
    color.SetAlpha(alpha * opacity);
    pointRsxform_.push_back(
        MakeRSXform(Vector2f(DEFAULT_RADIUS, DEFAULT_RADIUS), position, 0.f, radius * scale / DEFAULT_RADIUS));
    pointTex_.push_back(Drawing::Rect(0, 0, DEFAULT_RADIUS * DOUBLE, DEFAULT_RADIUS * DOUBLE));
    pointColors_.push_back(Drawing::Color(color.AsArgbInt()).CastToColorQuad());
    pointCount_++;
}

void RSParticlesDrawable::CaculateImageAtlsArry(Drawing::Canvas& canvas,
    const std::shared_ptr<RSRenderParticle>& particle, Vector2f position, float opacity, float scale)
{
    auto imageSize = particle->GetImageSize();
    auto spin = particle->GetSpin();
    auto imageIndex = particle->GetImageIndex();
    auto image = particle->GetImage();
    auto pixelmap = image->GetPixelMap();
    if (imageIndex >= imageCount_) {
        return;
    }
    float left = position.x_;
    float top = position.y_;
    float right = position.x_ + imageSize.x_ * scale;
    float bottom = position.y_ + imageSize.y_ * scale;
    float width = imageSize.x_;
    float height = imageSize.y_;
    RectF destRect(left, top, width, height);
    auto imageFit = image->GetImageFit();
    if (imageFit != ImageFit::FILL) {
        image->SetFrameRect(destRect);
        image->ApplyImageFit();
        Color color = particle->GetColor();
        auto alpha = color.GetAlpha();
        color.SetAlpha(alpha * opacity);
        imageRsxform_[imageIndex].push_back(
            MakeRSXform(Vector2f(pixelmap->GetWidth() / DOUBLE, pixelmap->GetHeight() / DOUBLE), position, spin,
                image->GetDstRect().GetWidth() / pixelmap->GetWidth() * scale));
        imageTex_[imageIndex].push_back(Drawing::Rect(0, 0, pixelmap->GetWidth(), pixelmap->GetHeight()));
        imageColors_[imageIndex].push_back(Drawing::Color(color.AsArgbInt()).CastToColorQuad());
        count_[imageIndex]++;
    } else {
        canvas.Save();
        canvas.Translate(position.x_, position.y_);
        canvas.Rotate(spin, imageSize.x_ * scale / DOUBLE, imageSize.y_ * scale / DOUBLE);
        image->SetScale(scale);
        image->SetImageRepeat(0);
        Drawing::Rect rect { left, top, right, bottom };
        Drawing::Brush brush;
        brush.SetAntiAlias(true);
        brush.SetAlphaF(opacity);
        canvas.AttachBrush(brush);
        image->CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), false);
        canvas.DetachBrush();
        canvas.Restore();
    }
}

void RSParticlesDrawable::Draw(Drawing::Canvas& canvas, std::shared_ptr<RectF> bounds)
{
    if (particles_.empty()) {
        ROSEN_LOGE("RSParticlesDrawable::Draw particles_ is empty");
        return;
    }
    for (const auto& particle : particles_) {
        if (particle != nullptr && particle->IsAlive()) {
            auto position = particle->GetPosition();
            float opacity = particle->GetOpacity();
            float scale = particle->GetScale();
            if (bounds == nullptr || !(bounds->Intersect(position.x_, position.y_)) || opacity <= 0.f || scale <= 0.f) {
                continue;
            }
            auto particleType = particle->GetParticleType();
            auto clipBounds = Drawing::Rect(
                bounds->left_, bounds->top_, bounds->left_ + bounds->width_, bounds->top_ + bounds->height_);
            canvas.ClipRect(clipBounds, Drawing::ClipOp::INTERSECT, true);
            if (particleType == ParticleType::POINTS) {
                CaculatePointAtlsArry(particle, position, opacity, scale);
            } else {
                CaculateImageAtlsArry(canvas, particle, position, opacity, scale);
            }
        }
    }
    DrawParticles(canvas);
}

void RSParticlesDrawable::DrawParticles(Drawing::Canvas& canvas)
{
    if (circleImage_ != nullptr) {
        Drawing::Brush brush;
        brush.SetAntiAlias(true);
        canvas.AttachBrush(brush);
        canvas.DrawAtlas(circleImage_.get(), pointRsxform_.data(), pointTex_.data(),
            pointColors_.data(), pointCount_, Drawing::BlendMode::DST_IN,
            Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), nullptr);
        canvas.DetachBrush();
    }
    while (imageCount_--) {
        if (imageVector_[imageCount_] != nullptr) {
            auto pixelmap = imageVector_[imageCount_]->GetPixelMap();
            if (!pixelmap) {
                ROSEN_LOGE("RSParticlesDrawable::Draw !pixel");
                continue;
            }
            auto image = RSPixelMapUtil::ExtractDrawingImage(pixelmap);
            if (!image) {
                ROSEN_LOGE("RSParticlesDrawable::Draw !image");
                continue;
            }
            Drawing::Brush brush;
            brush.SetAntiAlias(true);
            canvas.AttachBrush(brush);
            canvas.DrawAtlas(image.get(), imageRsxform_[imageCount_].data(), imageTex_[imageCount_].data(),
                imageColors_[imageCount_].data(), count_[imageCount_], Drawing::BlendMode::SRC_IN,
                Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), nullptr);
            canvas.DetachBrush();
        }
    }
}

} // namespace Rosen
} // namespace OHOS