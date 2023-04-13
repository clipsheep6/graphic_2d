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

#ifndef MASK_FILTER_CMD_LILST_H
#define MASK_FILTER_CMD_LILST_H

#include "effect/mask_filter.h"
#include "recording/mem_allocator.h"
#include "recording/op_item.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum MaskFilterOpItemType : uint32_t {
    MF_OPITEM_HEAD,
    MF_CREATE_BLUR,
};

class MaskFilterCmdList {
public:
    MaskFilterCmdList() = default;
    ~MaskFilterCmdList() = default;

    static std::shared_ptr<MaskFilterCmdList> CreateFromData(CmdListData data);

    template<typename T, typename... Args>
    void AddOp(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        T* op = allocator_.Allocate<T>(std::forward<Args>(args)...);
        if (!op) {
            return;
        }
        if (lastOpItem) {
            Offset_t offset = allocator_.AddrToOffset(op);
            lastOpItem->SetNextOpItemOffset(offset);
        }
        lastOpItem = op;
    }

    Offset_t AddCmdListData(CmdListData src);

    CmdListData GetData() const;

    std::shared_ptr<MaskFilter> Playback();

private:
    MemAllocator allocator_;
    std::mutex mutex_;
    OpItem* lastOpItem = nullptr;
};

/* OpItem */
class CreateBlurMaskFilterOpItem : public OpItem {
public:
    CreateBlurMaskFilterOpItem(BlurType blurType, scalar sigma);
    ~CreateBlurMaskFilterOpItem() = default;

    std::shared_ptr<MaskFilter> Playback();

private:
    BlurType blurType_;
    scalar sigma_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
