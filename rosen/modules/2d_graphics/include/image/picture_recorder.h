/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef PICTURE_RECORDER_H
#define PICTURE_RECORDER_H

#include "drawing/engine_adapter/impl_interface/picture_recorder_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PictureRecorder {
public:
    PictureRecorder() noexcept;
    virtual ~PictureRecorder() {};

    CoreCanvas* BeginRecording(const Rect& bounds);
    CoreCanvas* BeginRecording(scalar width, scalar height);
    CoreCanvas* GetRecordingCanvas();
    std::shared_ptr<Picture> FinishRecordingAsPicture();
private:
    std::shared_ptr<PictureRecorderImpl> pictureRecorderImplPtr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif