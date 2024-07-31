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
#include "render/rs_maskcolor_shader_filter.h"

namespace OHOS {
namespace Rosen {
RSMaskColorShaderFilter::RSMaskColorShaderFilter(int colorMode, RSColor maskColor)
    : colorMode_(colorMode), maskColor_(maskColor)
{
    type_ = ShaderFilterType::MASK_COLOR;
    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&colorMode_, sizeof(colorMode_), hash_);
    hash_ = SkOpts::hash(&maskColor_, sizeof(maskColor_), hash_);
}

RSMaskColorShaderFilter::~RSMaskColorShaderFilter() = default;

int RSMaskColorShaderFilter::GetColorMode() const
{
    return colorMode_;
}

RSColor RSMaskColorShaderFilter::GetMaskColor() const
{
    return maskColor_;
}

void RSMaskColorShaderFilter::InitColorMod()
{
    if (colorMode_ == FASTAVERAGE && RSColorPickerCacheTask::ColorPickerPartialEnabled) {
        colorPickerTask_ = std::make_shared<RSColorPickerCacheTask>();
    }
}

Drawing::ColorQuad RSMaskColorShaderFilter::CalcAverageColor(std::shared_ptr<Drawing::Image> imageSnapshot)
{
    Drawing::ColorQuad color = Drawing::Color::COLOR_TRANSPARENT;
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::PickColorSyn GpuScaleImageids null");
        return color;
    }
    std::shared_ptr<Drawing::Pixmap> dst;
    const int buffLen = imageSnapshot->GetWidth() * imageSnapshot->GetHeight();
    auto pixelPtr = std::make_unique<uint32_t[]>(buffLen);
    auto info = imageSnapshot->GetImageInfo();
    dst = std::make_shared<Drawing::Pixmap>(info, pixelPtr.get(), info.GetWidth() * info.GetBytesPerPixel());
    bool flag = imageSnapshot->ReadPixels(*dst, 0, 0);
    if (!flag) {
        ROSEN_LOGE("RSPropertiesPainter::PickColorSyn ReadPixel Failed");
        return color;
    }
    uint32_t errorCode = 0;
    std::shared_ptr<RSColorPicker> colorPicker = RSColorPicker::CreateColorPicker(dst, errorCode);
    if (errorCode != 0) {
        return color;
    }
    colorPicker->GetAverageColor(color);
    return color;
}

void RSMaskColorShaderFilter::CaclMaskColor(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image)
{
    if (image == nullptr) return;
    if (colorMode_ == AVERAGE) {
        // update maskColor while persevere alpha
        std::shared_ptr<Drawing::Image> scaleImage = RSPropertiesPainter::GpuScaleImage(canvas.GetGPUContext(), image);
        auto colorPicker = CalcAverageColor(scaleImage);
        maskColor_ = RSColor(Drawing::Color::ColorQuadGetR(colorPicker), Drawing::Color::ColorQuadGetG(colorPicker),
            Drawing::Color::ColorQuadGetB(colorPicker), maskColor_.GetAlpha());
    }
    if (colorMode_ == FASTAVERAGE && RSColorPickerCacheTask::ColorPickerPartialEnabled) {
        RSColor color;
        if (colorPickerTask_->GetWaitRelease()) {
            if (colorPickerTask_->GetColor(color) && colorPickerTask_->GetFirstGetColorFinished()) {
                maskColor_ = RSColor(color.GetRed(), color.GetGreen(), color.GetBlue(), maskColor_.GetAlpha());
            }
            return;
        }
        if (RSColorPickerCacheTask::PostPartialColorPickerTask(colorPickerTask_, image)) {
            if (colorPickerTask_->GetColor(color)) {
                maskColor_ = RSColor(color.GetRed(), color.GetGreen(), color.GetBlue(), maskColor_.GetAlpha());
            }
            colorPickerTask_->SetStatus(CacheProcessStatus::WAITING);
        }
    }
}

void RSMaskColorShaderFilter::PreProcess(std::shared_ptr<Drawing::Image>& image)
{
    ROSEN_LOGI("RSPropertiesPainter::PreProcess");
}

void RSMaskColorShaderFilter::PostProcess(Drawing::Canvas& canvas)
{
    Drawing::Brush brush;
    brush.SetColor(maskColor_.AsArgbInt());

    if (canvas.GetDrawingType() == Drawing::DrawingType::PAINT_FILTER) {
        auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
        if (paintFilterCanvas && paintFilterCanvas->GetHDRPresent()) {
            paintFilterCanvas->PaintFilter(brush);
        }
    }

    canvas.DrawBackground(brush);
}

const std::shared_ptr<RSColorPickerCacheTask>& RSMaskColorShaderFilter::GetColorPickerCacheTask() const
{
    return colorPickerTask_;
}

void RSMaskColorShaderFilter::ReleaseColorPickerFilter()
{
    if (colorPickerTask_ == nullptr) {
        return;
    }
    colorPickerTask_->ReleaseColorPicker();
}
} // namespace Rosen
} // namespace OHOS