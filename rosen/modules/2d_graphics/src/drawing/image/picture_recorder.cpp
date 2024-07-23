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


#include "image/picture_recorder.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

PictureRecorder::PictureRecorder() {
}

PictureRecorder::~PictureRecorder() {
}

std::shared_ptr<RecordingCanvas> PictureRecorder::BeginRecording(int32_t width, int32_t height) {
    record_ = std::make_shared<RecordingCanvas>(width, height);
    return record_;
}

std::shared_ptr<Picture> PictureRecorder::FinishingRecording() {
    if (!record_) {
        std::cout << "FinishingRecording 111" << std::endl;
        return nullptr;
    }
    std::cout << "FinishingRecording 222" << std::endl;
    // 获取 RecordingCanvas 记录的绘制命令列表
    std::shared_ptr<DrawCmdList> cmdList = record_->GetDrawCmdList();
    // 创建一个新的 Picture 对象,并传入绘制命令列表
    std::shared_ptr<Picture> picture = std::make_shared<Picture>(cmdList);
    record_ = nullptr;
    return picture;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS