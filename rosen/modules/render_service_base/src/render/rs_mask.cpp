/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "render/rs_mask.h"

#include "include/core/SkPictureRecorder.h"

#ifdef USE_ROSEN_DRAWING
#include "recording/recording_path.h"
#endif
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
std::shared_ptr<RSMask> RSMask::CreateGradientMask(const SkPaint& maskPaint)
#else
std::shared_ptr<RSMask> RSMask::CreateGradientMask(const Drawing::Brush& maskBrush)
#endif
{
    auto mask = std::make_shared<RSMask>();
    if (mask) {
#ifndef USE_ROSEN_DRAWING
        mask->SetMaskPaint(maskPaint);
#else
        mask->SetMaskBrush(maskBrush);
#endif
        mask->SetMaskType(MaskType::GRADIENT);
    }
    return mask;
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<RSMask> RSMask::CreatePathMask(const SkPath& maskPath, const SkPaint& maskPaint)
#else
std::shared_ptr<RSMask> RSMask::CreatePathMask(const Drawing::Path& maskPath, const Drawing::Brush& maskBrush)
#endif
{
    auto mask = std::make_shared<RSMask>();
    if (mask) {
        mask->SetMaskPath(maskPath);
#ifndef USE_ROSEN_DRAWING
        mask->SetMaskPaint(maskPaint);
#else
        mask->SetMaskBrush(maskBrush);
#endif
        mask->SetMaskType(MaskType::PATH);
    }
    return mask;
}

std::shared_ptr<RSMask> RSMask::CreateSVGMask(double x, double y, double scaleX, double scaleY,
    const sk_sp<SkSVGDOM>& svgDom)
{
    auto mask = std::make_shared<RSMask>();
    if (mask) {
        mask->SetSvgX(x);
        mask->SetSvgY(y);
        mask->SetScaleX(scaleX);
        mask->SetScaleY(scaleY);
        mask->SetSvgDom(svgDom);
        mask->SetMaskType(MaskType::SVG);
    }
    return mask;
}

RSMask::RSMask()
{
#ifdef USE_ROSEN_DRAWING
    maskPath_ = std::make_shared<Drawing::Path>();
#endif
}

RSMask::~RSMask()
{
}

void RSMask::SetSvgX(double x)
{
    svgX_ = x;
}

double RSMask::GetSvgX() const
{
    return svgX_;
}

void RSMask::SetSvgY(double y)
{
    svgY_ = y;
}

double RSMask::GetSvgY() const
{
    return svgY_;
}

void RSMask::SetScaleX(double scaleX)
{
    scaleX_ = scaleX;
}

double RSMask::GetScaleX() const
{
    return scaleX_;
}

void RSMask::SetScaleY(double scaleY)
{
    scaleY_ = scaleY;
}

double RSMask::GetScaleY() const
{
    return scaleY_;
}

#ifndef USE_ROSEN_DRAWING
void RSMask::SetMaskPath(const SkPath& path)
{
    maskPath_ = path;
}
#else
void RSMask::SetMaskPath(const Drawing::Path& path)
{
    if (path.GetDrawingType() == Drawing::DrawingType::RECORDING) {
        maskPath_ = std::make_shared<Drawing::RecordingPath>();
        maskPath_->AddPath(path);
    } else {
        maskPath_ = std::make_shared<Drawing::Path>(path);
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
SkPath RSMask::GetMaskPath() const
#else
std::shared_ptr<Drawing::Path> RSMask::GetMaskPath() const
#endif
{
    return maskPath_;
}

#ifndef USE_ROSEN_DRAWING
void RSMask::SetMaskPaint(const SkPaint& paint)
{
    maskPaint_ = paint;
}

SkPaint RSMask::GetMaskPaint() const
{
    return maskPaint_;
}
#else
void RSMask::SetMaskBrush(const Drawing::Brush& brush)
{
    maskBrush_ = brush;
}

Drawing::Brush RSMask::GetMaskBrush() const
{
    return maskBrush_;
}
#endif

void RSMask::SetSvgDom(const sk_sp<SkSVGDOM>& svgDom)
{
    svgDom_ = svgDom;
}

sk_sp<SkSVGDOM> RSMask::GetSvgDom() const
{
    return svgDom_;
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkPicture> RSMask::GetSvgPicture() const
#else
std::shared_ptr<Drawing::Picture> RSMask::GetSvgPicture() const
#endif
{
    return svgPicture_;
}

void RSMask::SetMaskType(MaskType type)
{
    type_ = type;
}

bool RSMask::IsSvgMask() const
{
    return (type_ == MaskType::SVG);
}

bool RSMask::IsGradientMask() const
{
    return (type_ == MaskType::GRADIENT);
}

bool RSMask::IsPathMask() const
{
    return (type_ == MaskType::PATH);
}

#ifdef ROSEN_OHOS
bool RSMask::Marshalling(Parcel& parcel) const
{
    if (!(RSMarshallingHelper::Marshalling(parcel, type_) &&
            RSMarshallingHelper::Marshalling(parcel, svgX_) &&
            RSMarshallingHelper::Marshalling(parcel, svgY_) &&
            RSMarshallingHelper::Marshalling(parcel, scaleX_) &&
            RSMarshallingHelper::Marshalling(parcel, scaleY_) &&
#ifndef USE_ROSEN_DRAWING
            RSMarshallingHelper::Marshalling(parcel, maskPaint_) &&
#else
            RSMarshallingHelper::Marshalling(parcel, maskBrush_) &&
#endif
            RSMarshallingHelper::Marshalling(parcel, maskPath_))) {
        ROSEN_LOGE("RSMask::Marshalling failed!");
        return false;
    }
    if (IsSvgMask()) {
        ROSEN_LOGD("SVG RSMask::Marshalling");
        SkPictureRecorder recorder;
        SkCanvas* recordingCanvas = recorder.beginRecording(SkRect::MakeSize(svgDom_->containerSize()));
        svgDom_->render(recordingCanvas);
        sk_sp<SkPicture> picture = recorder.finishRecordingAsPicture();
        if (!RSMarshallingHelper::Marshalling(parcel, picture)) {
            ROSEN_LOGE("RSMask::Marshalling SkPicture failed!");
            return false;
        }
    }
    return true;
}

RSMask* RSMask::Unmarshalling(Parcel& parcel)
{
    auto rsMask = std::make_unique<RSMask>();
    if (!(RSMarshallingHelper::Unmarshalling(parcel, rsMask->type_) &&
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->svgX_) &&
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->svgY_) &&
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->scaleX_) &&
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->scaleY_) &&
#ifndef USE_ROSEN_DRAWING
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->maskPaint_) &&
#else
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->maskBrush_) &&
#endif
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->maskPath_))) {
        ROSEN_LOGE("RSMask::Unmarshalling failed!");
        return nullptr;
    }
    if (rsMask->IsSvgMask()) {
        ROSEN_LOGD("SVG RSMask::Unmarshalling");
        if (!RSMarshallingHelper::Unmarshalling(parcel, rsMask->svgPicture_)) {
            ROSEN_LOGE("RSMask::Unmarshalling SkPicture failed!");
            return nullptr;
        }
    }
    return rsMask.release();
}
#endif
} // namespace Rosen
} // namespace OHOS