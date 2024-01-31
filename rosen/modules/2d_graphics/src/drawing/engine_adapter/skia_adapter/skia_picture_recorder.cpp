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

#include "skia_picture_recorder.h"
#include "include/core/SkRect.h"
#include "utils/log.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPictureRecorder::SkiaPictureRecorder() noexcept : skiaPictureRecorder_(std::make_shared<SkPictureRecorder>()) {}

CoreCanvas* SkiaPictureRecorder::BeginRecording(const Rect& bounds)
{
    if (!skiaPictureRecorder_) {
        LOGE("skiaPictureRecorder_ is null, return on line %{public}d", __LINE__);
        return nullptr;
    }
    SkRect skBounds = SkRect::MakeXYWH(bounds.GetLeft(), bounds.GetTop(), bounds.GetWidth(), bounds.GetHeight());
    auto skCanvas = skiaPictureRecorder_->beginRecording(skBounds);
    return std::make_shared<CoreCanvas>((void*)skCanvas);
}

CoreCanvas* SkiaPictureRecorder::BeginRecording(scalar width, scalar height)
{
    SkRect skBounds = SkRect::MakeWH(width, height);
    return BeginRecording(skBounds);
}

CoreCanvas* SkiaPictureRecorder::GetRecordingCanvas()
{
    if (!skiaPictureRecorder_) {
        LOGE("skiaPictureRecorder_ is null, return on line %{public}d", __LINE__);
        return nullptr;
    }
    return skiaPictureRecorder_->getRecordingCanvas();
}

std::shared_ptr<Picture> SkiaPictureRecorder::FinishRecordingAsPicture()
{
    if (!skiaPictureRecorder_) {
        LOGE("skiaPictureRecorder_ is null, return on line %{public}d", __LINE__);
        return nullptr;
    }
    sk_sp<SkPicture> skPicture = skiaPictureRecorder_->finishRecordingAsPicture();
    std::shared_ptr<Picture> pic = std::make_shared<Picture>();
    pic->GetImpl<SkiaPicture>()->SetSkPicture(skPicture);
    return pic;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
