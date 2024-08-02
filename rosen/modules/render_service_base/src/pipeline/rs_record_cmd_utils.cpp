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

#include "pipeline/rs_record_cmd_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSRecordCmdUtils::RSRecordCmdUtils()
    : extendRecordingCanvas_(nullptr), cullRect_(Drawing::Rect()) {}

Drawing::Canvas* RSRecordCmdUtils::BeginRecording(const Drawing::Rect& bounds)
{
    extendRecordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(bounds.GetWidth(), bounds.GetHeight());
    cullRect_ = Drawing::Rect(bounds);
    return extendRecordingCanvas_.get();
}

std::shared_ptr<Drawing::RecordCmd> RSRecordCmdUtils::FinishRecording()
{
    if (extendRecordingCanvas_ == nullptr) {
        ROSEN_LOGE("RSRecordCmdUtils::FinishRecording: extendRecordingCanvas_ is nullptr.");
        return nullptr;
    }
    auto recordCmd = std::make_shared<Drawing::RecordCmd>(extendRecordingCanvas_->GetDrawCmdList(), cullRect_);
    extendRecordingCanvas_ = nullptr;
    cullRect_ = Drawing::Rect();
    return recordCmd;
}
} // namespace Rosen
} // namespace OHOS
