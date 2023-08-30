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

#ifndef RECORDING_TEXT_H
#define RECORDING_TEXT_H

#include "recording/text_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingTextBlob : public TextBlobBuilder {
public:
    RecordingTextBlob();
    ~RecordingTextBlob() = default;

    DrawingType GetDrawingType() const override
    {
        return DrawingType::RECORDING;
    }

    std::shared_ptr<TextBlobBuilderImpl::RunBuffer> AllocRunPos(const Font& font, int count) override;
    std::shared_ptr<TextBlob> Make() override;

    std::shared_ptr<TextBlobCmdList> GetCmdList() const;

private:
    std::shared_ptr<TextBlobCmdList> cmdList_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // RECORDING_REGION_H
