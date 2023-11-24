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

#include "recording/cmd_list.h"

#include <algorithm>

#ifdef SUPPORT_OHOS_PIXMAP
#include "pixel_map.h"
#endif
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr uint32_t OPITEM_HEAD = 0;

CmdList::CmdList(const CmdListData& cmdListData)
{
    opAllocator_.BuildFromDataWithCopy(cmdListData.first, cmdListData.second);
}

CmdList::~CmdList()
{
#ifdef SUPPORT_OHOS_PIXMAP
    pixelMapVec_.clear();
#endif
}

uint32_t CmdList::AddCmdListData(const CmdListData& data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!lastOpItemOffset_.has_value()) {
        void* op = opAllocator_.Allocate<OpItem>(OPITEM_HEAD);
        if (op == nullptr) {
            LOGE("add OpItem head failed!");
            return 0;
        }
        lastOpItemOffset_.emplace(opAllocator_.AddrToOffset(op));
    }
    void* addr = opAllocator_.Add(data.first, data.second);
    if (addr == nullptr) {
        LOGE("CmdList AddCmdListData failed!");
        return 0;
    }
    return opAllocator_.AddrToOffset(addr);
}

const void* CmdList::GetCmdListData(uint32_t offset) const
{
    return opAllocator_.OffsetToAddr(offset);
}

CmdListData CmdList::GetData() const
{
    return std::make_pair(opAllocator_.GetData(), opAllocator_.GetSize());
}

bool CmdList::SetUpImageData(const void* data, size_t size)
{
    return imageAllocator_.BuildFromDataWithCopy(data, size);
}

uint32_t CmdList::AddImageData(const void* data, size_t size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    void* addr = imageAllocator_.Add(data, size);
    if (addr == nullptr) {
        LOGE("CmdList AddImageData failed!");
        return 0;
    }
    return imageAllocator_.AddrToOffset(addr);
}

const void* CmdList::GetImageData(uint32_t offset) const
{
    return imageAllocator_.OffsetToAddr(offset);
}

CmdListData CmdList::GetAllImageData() const
{
    return std::make_pair(imageAllocator_.GetData(), imageAllocator_.GetSize());
}

uint32_t CmdList::AddPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
#ifdef SUPPORT_OHOS_PIXMAP
    std::lock_guard<std::mutex> lock(pixelMapMutex_);
    pixelMapVec_.emplace_back(pixelMap);
    return static_cast<uint32_t>(pixelMapVec_.size()) - 1;
#else
    return 0;
#endif
}

std::shared_ptr<Media::PixelMap> CmdList::GetPixelMap(uint32_t id)
{
#ifdef SUPPORT_OHOS_PIXMAP
    std::lock_guard<std::mutex> lock(pixelMapMutex_);
    if (id >= pixelMapVec_.size()) {
        return nullptr;
    }
    return pixelMapVec_[id];
#else
    return nullptr;
#endif
}

uint32_t CmdList::GetAllPixelMap(std::vector<std::shared_ptr<Media::PixelMap>>& pixelMapList)
{
#ifdef SUPPORT_OHOS_PIXMAP
    std::lock_guard<std::mutex> lock(pixelMapMutex_);
    for (const auto &pixelMap : pixelMapVec_) {
        pixelMapList.emplace_back(pixelMap);
    }
    return pixelMapList.size();
#else
    return 0;
#endif
}

uint32_t CmdList::SetupPixelMap(const std::vector<std::shared_ptr<Media::PixelMap>>& pixelMapList)
{
#ifdef SUPPORT_OHOS_PIXMAP
    std::lock_guard<std::mutex> lock(pixelMapMutex_);
    for (const auto &pixelMap : pixelMapList) {
        pixelMapVec_.emplace_back(pixelMap);
    }
    return pixelMapVec_.size();
#else
    return 0;
#endif
}

uint32_t CmdList::AddImageObject(const std::shared_ptr<ExtendImageObject>& object)
{
#ifdef SUPPORT_OHOS_PIXMAP
    std::lock_guard<std::mutex> lock(imageObjectMutex_);
    imageObjectVec_.emplace_back(object);
    return static_cast<uint32_t>(imageObjectVec_.size()) - 1;
#else
    return 0;
#endif
}

std::shared_ptr<ExtendImageObject> CmdList::GetImageObject(uint32_t id)
{
#ifdef SUPPORT_OHOS_PIXMAP
    std::lock_guard<std::mutex> lock(imageObjectMutex_);
    if (id >= imageObjectVec_.size()) {
        return nullptr;
    }
    return imageObjectVec_[id];
#else
    return nullptr;
#endif
}

uint32_t CmdList::GetAllObject(std::vector<std::shared_ptr<ExtendImageObject>>& objectList)
{
#ifdef SUPPORT_OHOS_PIXMAP
    std::lock_guard<std::mutex> lock(imageObjectMutex_);
    for (const auto &object : imageObjectVec_) {
        objectList.emplace_back(object);
    }
    return objectList.size();
#else
    return 0;
#endif
}

uint32_t CmdList::SetupObject(const std::vector<std::shared_ptr<ExtendImageObject>>& objectList)
{
#ifdef SUPPORT_OHOS_PIXMAP
    std::lock_guard<std::mutex> lock(imageObjectMutex_);
    for (const auto &object : objectList) {
        imageObjectVec_.emplace_back(object);
    }
    return imageObjectVec_.size();
#else
    return 0;
#endif
}

uint32_t CmdList::AddImageBaseOj(const std::shared_ptr<ExtendImageBaseOj>& object)
{
    std::lock_guard<std::mutex> lock(imageBaseOjMutex_);
    imageBaseOjVec_.emplace_back(object);
    return static_cast<uint32_t>(imageBaseOjVec_.size()) - 1;
}

std::shared_ptr<ExtendImageBaseOj> CmdList::GetImageBaseOj(uint32_t id)
{
    std::lock_guard<std::mutex> lock(imageBaseOjMutex_);
    if (id >= imageBaseOjVec_.size()) {
        return nullptr;
    }
    return imageBaseOjVec_[id];
}

uint32_t CmdList::GetAllBaseOj(std::vector<std::shared_ptr<ExtendImageBaseOj>>& objectList)
{
    std::lock_guard<std::mutex> lock(imageBaseOjMutex_);
    for (const auto &object : imageBaseOjVec_) {
        objectList.emplace_back(object);
    }
    return objectList.size();
}

uint32_t CmdList::SetupBaseOj(const std::vector<std::shared_ptr<ExtendImageBaseOj>>& objectList)
{
    std::lock_guard<std::mutex> lock(imageBaseOjMutex_);
    for (const auto &object : objectList) {
        imageBaseOjVec_.emplace_back(object);
    }
    return imageBaseOjVec_.size();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
