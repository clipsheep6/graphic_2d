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

#include "recording/draw_cmd_list.h"

#include <cstddef>
#include <memory>

#include "recording/draw_cmd.h"
#include "recording/recording_canvas.h"
#include "utils/log.h"
#include "utils/performanceCaculate.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::shared_ptr<DrawCmdList> DrawCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<DrawCmdList>(DrawCmdList::UnmarshalMode::DEFERRED);
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    } else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }

    int32_t* width = static_cast<int32_t*>(cmdList->opAllocator_.OffsetToAddr(0));
    int32_t* height = static_cast<int32_t*>(cmdList->opAllocator_.OffsetToAddr(sizeof(int32_t)));
    if (width && height) {
        cmdList->width_ = *width;
        cmdList->height_ = *height;
    } else {
        cmdList->width_ = 0;
        cmdList->height_ = 0;
    }
    return cmdList;
}

DrawCmdList::DrawCmdList(DrawCmdList::UnmarshalMode mode) : width_(0), height_(0), mode_(mode) {}

DrawCmdList::DrawCmdList(int32_t width, int32_t height, DrawCmdList::UnmarshalMode mode)
    : width_(width), height_(height), mode_(mode)
{
    opAllocator_.Add(&width_, sizeof(int32_t));
    opAllocator_.Add(&height_, sizeof(int32_t));
}

DrawCmdList::~DrawCmdList()
{
    ClearOp();
}

bool DrawCmdList::AddDrawOp(std::shared_ptr<DrawOpItem>&& drawOpItem)
{
    if (mode_ != DrawCmdList::UnmarshalMode::DEFERRED) {
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    drawOpItems_.emplace_back(drawOpItem);
    return true;
}

void DrawCmdList::ClearOp()
{
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        opAllocator_.ClearData();
        opAllocator_.Add(&width_, sizeof(int32_t));
        opAllocator_.Add(&height_, sizeof(int32_t));
        imageAllocator_.ClearData();
        bitmapAllocator_.ClearData();
        imageMap_.clear();
        imageHandleVec_.clear();
        drawOpItems_.clear();
        lastOpGenSize_ = 0;
        lastOpItemOffset_ = std::nullopt;
        opCnt_ = 0;
    }
    {
        std::lock_guard<std::mutex> lock(imageObjectMutex_);
        imageObjectVec_.clear();
    }
    {
        std::lock_guard<std::mutex> lock(imageBaseObjMutex_);
        imageBaseObjVec_.clear();
    }
}

int32_t DrawCmdList::GetWidth() const
{
    return width_;
}

int32_t DrawCmdList::GetHeight() const
{
    return height_;
}

void DrawCmdList::SetWidth(int32_t width)
{
    width_ = width;
}

void DrawCmdList::SetHeight(int32_t height)
{
    height_ = height;
}

bool DrawCmdList::IsEmpty() const
{
    if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        return drawOpItems_.empty();
    }
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (opAllocator_.GetSize() <= offset && drawOpItems_.size() == 0) {
        return true;
    }
    return false;
}

size_t DrawCmdList::GetOpItemSize() const
{
    return mode_ == DrawCmdList::UnmarshalMode::DEFERRED ? drawOpItems_.size() : opCnt_;
}

std::string DrawCmdList::GetOpsWithDesc() const
{
    std::string desc;
    for (auto& item : drawOpItems_) {
        if (item == nullptr) {
            continue;
        }
        desc += item->GetOpDesc();
        desc += "\n";
    }
    LOGD("DrawCmdList::GetOpsWithDesc %{public}s, opitem sz: %{public}zu", desc.c_str(), drawOpItems_.size());
    return desc;
}

void DrawCmdList::Dump(std::string& out)
{
    for (auto& item : drawOpItems_) {
        if (item == nullptr) {
            continue;
        }
        item->Dump(out);
        out += " ";
    }
    if (drawOpItems_.size() > 0) {
        out.pop_back();
    }
}

void DrawCmdList::MarshallingDrawOps()
{
    if (mode_ == DrawCmdList::UnmarshalMode::IMMEDIATE) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (replacedOpListForVector_.empty()) {
        for (auto& op : drawOpItems_) {
            op->Marshalling(*this);
        }
        return;
    }
    for (auto& [index, op] : replacedOpListForVector_) {
        op.swap(drawOpItems_[index]);
    }
    std::vector<uint32_t> opIndexForCache(replacedOpListForVector_.size());
    uint32_t opReplaceIndex = 0;
    for (auto index = 0u; index < drawOpItems_.size(); ++index) {
        drawOpItems_[index]->Marshalling(*this);
        if (index == static_cast<size_t>(replacedOpListForVector_[opReplaceIndex].first)) {
            opIndexForCache[opReplaceIndex] = lastOpItemOffset_.value();
            ++opReplaceIndex;
        }
    }
    for (auto index = 0u; index < replacedOpListForVector_.size(); ++index) {
        replacedOpListForVector_[index].second->Marshalling(*this);
        replacedOpListForBuffer_.emplace_back(opIndexForCache[index], lastOpItemOffset_.value());
    }
}

void DrawCmdList::CaculatePerformanceOpType()
{
    uint32_t offset = offset_;
    const int caculatePerformaceCount = 500;    // 被测单接口用例至少出现500次以上
    std::map<uint32_t, uint32_t> opTypeCountMap;
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            break;
        }
        uint32_t type = curOpItemPtr->GetType();
        if (opTypeCountMap.find(type) != opTypeCountMap.end()) {
            if (++opTypeCountMap[type] > caculatePerformaceCount) {
                performanceCaculateOpType_ = type;
                DRAWING_PERFORMANCE_START_CACULATE;
                return;
            }
        } else {
            opTypeCountMap[type] = 1;   // 记录出现的第1次
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
    } while (offset != 0);
}

void DrawCmdList::UnmarshallingDrawOps()
{
    if (PerformanceCaculate::GetDrawingTestRecordingEnabled()) {
        CaculatePerformanceOpType();
    }
    if (performanceCaculateOpType_ != 0) {
        LOGI("Drawing Performance UnmarshallingDrawOps begin %{public}lld", PerformanceCaculate::GetUpTime());
    }

    if (opAllocator_.GetSize() <= offset_) {
        return;
    }

    UnmarshallingPlayer player = { *this };
    drawOpItems_.clear();
    lastOpGenSize_ = 0;
    uint32_t opReplaceIndex = 0;
    uint32_t offset = offset_;
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("DrawCmdList::UnmarshallingOps failed, opItem is nullptr");
            break;
        }
        uint32_t type = curOpItemPtr->GetType();
        auto op = player.Unmarshalling(type, itemPtr);
        if (!op) {
            offset = curOpItemPtr->GetNextOpItemOffset();
            continue;
        }
        if (opReplaceIndex < replacedOpListForBuffer_.size() &&
            replacedOpListForBuffer_[opReplaceIndex].first == offset) {
            auto* replacePtr = opAllocator_.OffsetToAddr(replacedOpListForBuffer_[opReplaceIndex].second);
            if (replacePtr == nullptr) {
                LOGE("DrawCmdList::Unmarshalling replace Ops failed, replace op is nullptr");
                break;
            }
            auto* replaceOpItemPtr = static_cast<OpItem*>(replacePtr);
            auto replaceOp = player.Unmarshalling(replaceOpItemPtr->GetType(), replacePtr);
            if (replaceOp) {
                drawOpItems_.emplace_back(replaceOp);
                replacedOpListForVector_.emplace_back((drawOpItems_.size() - 1), op);
            } else {
                drawOpItems_.emplace_back(op);
            }
            opReplaceIndex++;
        } else {
            drawOpItems_.emplace_back(op);
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
        if (!replacedOpListForBuffer_.empty() && offset >= replacedOpListForBuffer_[0].second) {
            LOGD("DrawCmdList::UnmarshallingOps seek end by cache textOps");
            break;
        }
    } while (offset != 0);
    lastOpGenSize_ = opAllocator_.GetSize();

    if ((int)imageAllocator_.GetSize() > 0) {
        imageAllocator_.ClearData();
    }

    if (performanceCaculateOpType_ != 0) {
        LOGI("Drawing Performance UnmarshallingDrawOps end %{public}lld", PerformanceCaculate::GetUpTime());
    }
}

void DrawCmdList::Playback(Canvas& canvas, const Rect* rect)
{
    if (width_ <= 0 || height_ <= 0) {
        return;
    }
    if (performanceCaculateOpType_ != 0) {
        LOGI("Drawing Performance Playback begin %{public}lld", PerformanceCaculate::GetUpTime());
    }
    if (canvas.GetDrawingType() == DrawingType::RECORDING) {
        PlaybackToDrawCmdList(static_cast<RecordingCanvas&>(canvas).GetDrawCmdList());
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
#ifdef ROSEN_OHOS
    // invalidate cache if high contrast flag changed
    if (isCached_ && canvas.isHighContrastEnabled() != cachedHighContrast_) {
        ClearCache();
    }
    // Generate or clear cache if cache state changed
    if (canvas.GetCacheType() == Drawing::CacheType::ENABLED && !isCached_) {
        GenerateCache(&canvas, rect);
    } else if (canvas.GetCacheType() == Drawing::CacheType::DISABLED && isCached_) {
        ClearCache();
    }
#endif
    Rect tmpRect;
    if (rect != nullptr) {
        tmpRect = *rect;
    }
    if (mode_ == DrawCmdList::UnmarshalMode::IMMEDIATE) {
        PlaybackByBuffer(canvas, &tmpRect);
    } else if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        PlaybackByVector(canvas, &tmpRect);
    }
    if (performanceCaculateOpType_ != 0) {
        DRAWING_PERFORMANCE_STOP_CACULATE;
        performanceCaculateOpType_ = 0;
        LOGI("Drawing Performance Playback end %{public}lld", PerformanceCaculate::GetUpTime());
    }
}

void DrawCmdList::GenerateCache(Canvas* canvas, const Rect* rect)
{
#ifdef ROSEN_OHOS
    if (isCached_) {
        LOGD("DrawCmdList::GenerateCache Invoke multiple times");
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (mode_ == DrawCmdList::UnmarshalMode::IMMEDIATE) {
        GenerateCacheByBuffer(canvas, rect);
    } else if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        GenerateCacheByVector(canvas, rect);
    }
#endif
}

bool DrawCmdList::GetIsCache()
{
    return isCached_;
}

void DrawCmdList::SetIsCache(bool isCached)
{
    isCached_ = isCached;
}

bool DrawCmdList::GetCachedHighContrast()
{
    return cachedHighContrast_;
}

void DrawCmdList::SetCachedHighContrast(bool cachedHighContrast)
{
    cachedHighContrast_ = cachedHighContrast;
}

std::vector<std::pair<uint32_t, uint32_t>> DrawCmdList::GetReplacedOpList()
{
    return replacedOpListForBuffer_;
}

void DrawCmdList::SetReplacedOpList(std::vector<std::pair<uint32_t, uint32_t>> replacedOpList)
{
    replacedOpListForBuffer_ = replacedOpList;
}

void DrawCmdList::UpdateNodeIdToPicture(NodeId nodeId)
{
    if (drawOpItems_.size() == 0) {
        return;
    }
    for (size_t i = 0; i < drawOpItems_.size(); ++i) {
        auto opItem = drawOpItems_[i];
        if (!opItem) {
            continue;
        }
        opItem->SetNodeId(nodeId);
    }
}

void DrawCmdList::ClearCache()
{
#ifdef ROSEN_OHOS
    // restore the original op
    for (auto& [index, op] : replacedOpListForVector_) {
        op.swap(drawOpItems_[index]);
    }
    replacedOpListForVector_.clear();
    replacedOpListForBuffer_.clear();
    isCached_ = false;
#endif
}

void DrawCmdList::GenerateCacheByVector(Canvas* canvas, const Rect* rect)
{
#ifdef ROSEN_OHOS
    if (drawOpItems_.size() == 0) {
        return;
    }
    uint32_t opSize = drawOpItems_.size();
    for (auto index = 0u; index < opSize; ++index) {
        std::shared_ptr<DrawOpItem> op = drawOpItems_[index];
        if (!op || op->GetType() != DrawOpItem::TEXT_BLOB_OPITEM) {
            continue;
        }
        DrawTextBlobOpItem* textBlobOp = static_cast<DrawTextBlobOpItem*>(op.get());
        auto replaceCache = textBlobOp->GenerateCachedOpItem(canvas);
        if (replaceCache) {
            replacedOpListForVector_.emplace_back(index, op);
            drawOpItems_[index] = replaceCache;
        }
    }
    isCached_ = true;
    cachedHighContrast_ = canvas && canvas->isHighContrastEnabled();
#endif
}

void DrawCmdList::GenerateCacheByBuffer(Canvas* canvas, const Rect* rect)
{
#ifdef ROSEN_OHOS
    if (opAllocator_.GetSize() <= offset_) {
        return;
    }

    uint32_t offset = offset_;
    GenerateCachedOpItemPlayer player = { *this, canvas, rect };
    uint32_t maxOffset = opAllocator_.GetSize();
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("DrawCmdList::GenerateCacheByBuffer failed, opItem is nullptr");
            break;
        }
        bool replaceSuccess = player.GenerateCachedOpItem(curOpItemPtr->GetType(), itemPtr);
        if (replaceSuccess) {
            replacedOpListForBuffer_.push_back({offset, lastOpItemOffset_.value()});
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
    } while (offset != 0 && offset < maxOffset);
    isCached_ = true;
    cachedHighContrast_ = canvas && canvas->isHighContrastEnabled();
#endif
}

void DrawCmdList::PlaybackToDrawCmdList(std::shared_ptr<DrawCmdList> drawCmdList)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        std::lock_guard<std::recursive_mutex> lock(drawCmdList->mutex_);
        drawCmdList->drawOpItems_.insert(drawCmdList->drawOpItems_.end(), drawOpItems_.begin(), drawOpItems_.end());
        return;
    }

    void* addr = opAllocator_.OffsetToAddr(offset_);
    if (addr == nullptr) {
        return;
    }

#ifdef SUPPORT_OHOS_PIXMAP
    {
        std::lock_guard<std::mutex> lock(drawCmdList->imageObjectMutex_);
        drawCmdList->imageObjectVec_.swap(imageObjectVec_);
    }
#endif
    {
        std::lock_guard<std::mutex> lock(drawCmdList->imageBaseObjMutex_);
        drawCmdList->imageBaseObjVec_.swap(imageBaseObjVec_);
    }
    size_t size = opAllocator_.GetSize() - offset_;
    auto imageData = GetAllImageData();
    auto bitmapData = GetAllBitmapData();
    drawCmdList->opAllocator_.Add(addr, size);
    if (imageData.first != nullptr && imageData.second != 0) {
        drawCmdList->AddImageData(imageData.first, imageData.second);
    }

    if (bitmapData.first != nullptr && bitmapData.second != 0) {
        drawCmdList->AddBitmapData(bitmapData.first, bitmapData.second);
    }
}

void DrawCmdList::PlaybackByVector(Canvas& canvas, const Rect* rect)
{
    if (drawOpItems_.empty()) {
        return;
    }
    for (auto op : drawOpItems_) {
        if (op) {
            op->Playback(&canvas, rect);
        }
    }
    canvas.DetachPaint();
}

void DrawCmdList::PlaybackByBuffer(Canvas& canvas, const Rect* rect)
{
    if (opAllocator_.GetSize() <= offset_) {
        return;
    }
    uint32_t offset = offset_;
    if (lastOpGenSize_ != opAllocator_.GetSize()) {
        UnmarshallingPlayer player = { *this };
        drawOpItems_.clear();
        do {
            void* itemPtr = opAllocator_.OffsetToAddr(offset);
            auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
            if (curOpItemPtr == nullptr) {
                break;
            }
            uint32_t type = curOpItemPtr->GetType();
            if (auto op = player.Unmarshalling(type, itemPtr)) {
                drawOpItems_.emplace_back(op);
            }
            offset = curOpItemPtr->GetNextOpItemOffset();
        } while (offset != 0);
        lastOpGenSize_ = opAllocator_.GetSize();
    }
    for (auto op : drawOpItems_) {
        if (op) {
            op->Playback(&canvas, rect);
        }
    }
    canvas.DetachPaint();
}

size_t DrawCmdList::CountTextBlobNum()
{
    size_t textBlobCnt = 0;
    if (mode_ == DrawCmdList::UnmarshalMode::IMMEDIATE) {
        uint32_t offset = offset_;
        uint32_t maxOffset = opAllocator_.GetSize();
        do {
            void* itemPtr = opAllocator_.OffsetToAddr(offset);
            auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
            if (curOpItemPtr == nullptr) {
                break;
            }
            uint32_t type = curOpItemPtr->GetType();
            if (type == DrawOpItem::TEXT_BLOB_OPITEM) {
                textBlobCnt++;
            }
            offset = curOpItemPtr->GetNextOpItemOffset();
        } while (offset != 0 && offset < maxOffset);
    }
    return textBlobCnt;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
