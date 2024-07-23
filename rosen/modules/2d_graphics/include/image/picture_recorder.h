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

#include "recording/recording_canvas.h"
#include "recording/draw_cmd_list.h"
#include "image/picture.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PictureRecorder {
public:
    PictureRecorder();
    ~PictureRecorder();
    std::shared_ptr<RecordingCanvas> BeginRecording(int32_t width, int32_t height);
    std::shared_ptr<Picture> FinishingRecording();
private:
    std::shared_ptr<RecordingCanvas> record_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif