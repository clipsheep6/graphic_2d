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

#include "recording/recording_text.h"

#include "recording/cmd_list_helper.h"
#include "recording/recording_path.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingTextBlob::RecordingTextBlob() : cmdList_(std::make_shared<TextBlobCmdList>()) {}

std::shared_ptr<TextBlobCmdList> RecordingTextBlob::GetCmdList() const
{
    return cmdList_;
}

std::shared_ptr<RunBuffer> RecordingTextBlob::AllocRunPos(const Font& font, int count)
{
    TextBlobBuilder builder;   // 临时替换， 此处应该是Font 入参
    auto pathHandle = CmdListHelper::AddRecordedToCmdList<RecordingTextBlob>(*cmdList_, builder);
    cmdList_->AddOp<AllocRunPosOpItem>(pathHandle, count);

    return nullptr;
}

std::shared_ptr<TextBlob> RecordingTextBlob::Make()
{
    cmdList_->AddOp<BuildTextBlobOpItem>();
    return nullptr;

}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
