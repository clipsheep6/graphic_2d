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

#ifndef SKIAPICTURERECORDER_H
#define SKIAPICTURERECORDER_H

#include "include/core/SkPictureRecorder.h"

#include "impl_interface/picture_recorder_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPictureRecorder : public PictureRecorderImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaPictureRecorder() noexcept;
    ~SkiaPictureRecorder() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    CoreCanvas* BeginRecording(const Rect& bounds) override;
    CoreCanvas* BeginRecording(scalar width, scalar height) override;
    CoreCanvas* GetRecordingCanvas() override;
    std::shared_ptr<Picture> FinishRecordingAsPicture() override;

private:
    sk_sp<SkPictureRecorder> skiaPictureRecorder_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
