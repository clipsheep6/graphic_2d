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

#include "recording/text_cmd_list.h"

#include "recording/cmd_list_helper.h"
#include "recording/path_cmd_list.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<TextBlobCmdList> TextBlobCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<TextBlobCmdList>();
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    } else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }
    return cmdList;
}


std::shared_ptr<TextBlobBuilder> TextBlobCmdList::Playback() const
{
    if (opAllocator_.GetSize() == 0) {
        return nullptr;
    }

    uint32_t offset = 0;
    auto builder = std::make_shared<TextBlobBuilder>();
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        OpItem* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("TextBlobCmdList Playback failed!");
            break;
        }

        switch (curOpItemPtr->GetType()) {
            case TextBlobBuilderOpItem::OPITEM_HEAD:
                break;
            case TextBlobBuilderOpItem::ALLOC_RUN_POS_OPITEM:
                static_cast<AllocRunPosOpItem*>(itemPtr)->Playback(*builder, *this);
                break;
            case TextBlobBuilderOpItem::BUILD_TEXT_BLOB_OPITEM:
                static_cast<BuildTextBlobOpItem*>(itemPtr)->Playback(*builder);
                break;
            default:
                LOGE("unknown OpItem type!");
                break;
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
    } while (offset != 0);

    return builder;
}

// option
AllocRunPosOpItem::AllocRunPosOpItem(const CmdListHandle& font, int32_t count) :
    TextBlobBuilderOpItem(ALLOC_RUN_POS_OPITEM), fontHandler_(font), count_(count) {}

void AllocRunPosOpItem::Playback(TextBlobBuilder& builder, const CmdList& cmdList) const
{
    Font ff;  // 临时替换， TextBlobBuilder 为临时替换， 应该是Font
    auto font = CmdListHelper::GetFromCmdList<TextBlobCmdList, TextBlobBuilder>(cmdList, fontHandler_);
    builder.AllocRunPos(ff, count_);
}

BuildTextBlobOpItem::BuildTextBlobOpItem() : TextBlobBuilderOpItem(BUILD_TEXT_BLOB_OPITEM) {}

void BuildTextBlobOpItem::Playback(TextBlobBuilder& builder) const
{
    builder.Make();
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
