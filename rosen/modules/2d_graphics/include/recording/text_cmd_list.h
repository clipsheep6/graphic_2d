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

#ifndef TEXT_CMD_LIST_H
#define TEXT_CMD_LIST_H

#include "text/text_blob.h"
#include "recording/cmd_list.h"
#include "text/font.h"
#include "text/text_blob_builder.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class   TextBlobCmdList : public CmdList {
public:
    TextBlobCmdList() = default;
    ~TextBlobCmdList() override = default;

    uint32_t GetType() const override
    {
        return Type::REGION_CMD_LIST;
    }

    /*
     * @brief       Creates a TextBlobCmdList with contiguous buffers.
     * @param data  A contiguous buffers.
     */
    static std::shared_ptr<TextBlobCmdList> CreateFromData(const CmdListData& data, bool isCopy = false);

    /*
     * @brief  Calls the corresponding operations of all opitems in TextBlobCmdList to the region.
     */
    std::shared_ptr<TextBlobBuilder> Playback() const;
};

/* OpItem */
class TextBlobBuilderOpItem : public OpItem {
public:
    explicit TextBlobBuilderOpItem(uint32_t type) : OpItem(type) {}
    ~TextBlobBuilderOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD = 0,    // OPITEM_HEAD must be 0
        ALLOC_RUN_POS_OPITEM,
        BUILD_TEXT_BLOB_OPITEM,
        OPITEM,
    };
};

class AllocRunPosOpItem : public TextBlobBuilderOpItem {
public:
    explicit AllocRunPosOpItem(const CmdListHandle& font, int32_t count);
    ~AllocRunPosOpItem() = default;
    static void Playback(TextBlobCmdList& builder, const void* opItem);
    void Playback(TextBlobBuilder& builder,  const CmdList& cmdList) const;
private:
    CmdListHandle fontHandler_;
    int32_t count_;
};

// make
class BuildTextBlobOpItem : public TextBlobBuilderOpItem {
public:
    explicit BuildTextBlobOpItem();
    ~BuildTextBlobOpItem() = default;
    // static void Playback(TextBlobCmdList& builder, const void* opItem);
    void Playback(TextBlobBuilder& builder) const;
};


} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // REGION_CMD_LIST_H
