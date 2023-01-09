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

#include "frame_collector.h"

#include <chrono>
#include <mutex>
#include <map>
#include <string>

#include "hilog/log.h"
#include "hitrace_meter.h"
#include "parameter.h"

#include "frame_saver.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001400, "FrameCollector" };
constexpr int32_t uimarksStart = static_cast<int32_t>(FrameEventType::UIMarksStart);
constexpr int32_t uimarksEnd = static_cast<int32_t>(FrameEventType::UIMarksEnd) - 1;
constexpr int32_t loopEnd = static_cast<int32_t>(FrameEventType::LoopEnd) - 1;
constexpr int32_t vsyncStart = static_cast<int32_t>(FrameEventType::WaitVsyncStart);
} // namespace

FrameCollector &FrameCollector::GetInstance()
{
    static FrameCollector instance;
    return instance;
}

void FrameCollector::SetRepaintCallback(std::function<void()> repaint)
{
    repaint_ = repaint;
}

const FrameInfoQueue &FrameCollector::LockGetFrameQueue()
{
    frameQueueMutex_.lock();
    return frameQueue_;
}

void FrameCollector::UnlockFrameQueue()
{
    frameQueueMutex_.unlock();
}

bool FrameCollector::IsEnabled() const
{
    return enabled_;
}

void FrameCollector::SetEnabled(bool enable)
{
    enabled_ = enable;
}

void FrameCollector::MarkFrameEvent(const FrameEventType &type, int64_t timeNs)
{
    const auto &index = static_cast<int32_t>(type);
    if (index >= static_cast<int32_t>(FrameEventType::Max)) {
        ::OHOS::HiviewDFX::HiLog::Warn(LABEL,
            "FrameCollector::MarkFrameEvent index(%{public}d) not exists", static_cast<int32_t>(index));
        return;
    }

    if (timeNs == 0) {
        timeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    if (usingSaver_) {
        saver_->SaveFrameEvent(type, timeNs);
    }

    if (!enabled_) {
        return;
    }

    ::OHOS::HiviewDFX::HiLog::Debug(LABEL, "FrameCollector::MarkFrameEvent "
        "index(%{public}d) occur at %{public}s", index, std::to_string(timeNs).c_str());
    ProcessFrameEvent(index, timeNs);
}

void FrameCollector::ProcessFrameEvent(int32_t index, int64_t timeNs)
{
    std::lock_guard lockPending(pendingMutex_);
    // lockFrameQueue: lock for {pbefore_, pafter_}
    std::lock_guard lockFrameQueue(frameQueueMutex_);
    if (ProcessUIMarkLocked(index, timeNs)) {
        return;
    }
    int nowFrameNumber = 0;
    if (index == vsyncStart) {
        nowFrameNumber = currentUIMarks_.frameNumber;
        if (!haveAfterVsync_) {
            pbefore_ = &frameQueue_.Push(FrameInfo());
            pbefore_->frameNumber = currentUIMarks_.frameNumber;
            for (auto i = uimarksStart; i <= uimarksEnd; i++) {
                pbefore_->times[i] = currentUIMarks_.times[i];
            }
            pbefore_->times[index] = timeNs;
            haveAfterVsync_ = true;
        } else {
            pafter_ = &frameQueue_.Push(FrameInfo());
            for (auto i = uimarksStart; i <= uimarksEnd; i++) {
                pafter_->times[i] = currentUIMarks_.times[i];
            }
            pafter_->frameNumber = currentUIMarks_.frameNumber;
            pafter_->times[index] = timeNs;
        }
        StartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, GetAsyncNameByFrameEventType(index), pbefore_->frameNumber);
        return;
    }

    if (!haveAfterVsync_) {
        return;
    }
    if (pbefore_ != nullptr && pbefore_->times[index] == 0) {
        pbefore_->times[index] = timeNs;
        nowFrameNumber = pbefore_->frameNumber;
    } else if (pafter_ != nullptr) {
        pafter_->times[index] = timeNs;
        nowFrameNumber = pafter_->frameNumber;
    } else {
        ::OHOS::HiviewDFX::HiLog::Error(LABEL, "no FrameInfo, mark.markType is %{public}s",
            GetAsyncNameByFrameEventType(index).c_str());
        return;
    }
    if (IsStartFrameEventType(index)) {
        StartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, GetAsyncNameByFrameEventType(index), nowFrameNumber);
    } else {
        FinishAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, GetAsyncNameByFrameEventType(index), nowFrameNumber);
    }

    if (index == loopEnd) {
        if (pafter_ == nullptr) {
            haveAfterVsync_ = false;
        } else {
            pbefore_ = pafter_;
            pafter_ = nullptr;
            haveAfterVsync_ = true;
        }
    }
}

bool FrameCollector::ProcessUIMarkLocked(int32_t index, int64_t timeNs)
{
    if (index > uimarksEnd) {
        return false;
    }

    if (IsStartFrameEventType(index)) {
        StartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, GetAsyncNameByFrameEventType(index), currentFrameNumber_);
    } else {
        FinishAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, GetAsyncNameByFrameEventType(index), currentFrameNumber_);
    }

    if (index < uimarksEnd) {
        pendingUIMarks_.times[index] = timeNs;
        return true;
    }

    // index == uimarksEnd
    pendingUIMarks_.times[index] = timeNs;
    currentUIMarks_ = pendingUIMarks_;
    pendingUIMarks_.frameNumber = ++currentFrameNumber_;
    return true;
}

void FrameCollector::ClearEvents()
{
    std::lock_guard lock(frameQueueMutex_);
    frameQueue_.Clear();
}

FrameCollector::FrameCollector()
{
    char value[0x20];
    GetParameter(switchRenderingText, "disable", value, sizeof(value));
    SwitchFunction(switchRenderingText, value, this);
    WatchParameter(switchRenderingText, SwitchFunction, this);
}

void FrameCollector::SwitchFunction(const char *key, const char *value, void *context)
{
    auto &that = *reinterpret_cast<FrameCollector *>(context);
    auto oldEnable = that.enabled_;
    std::string str = value;
    if (str == switchRenderingPaintText) {
        that.ClearEvents();
        that.usingSaver_ = false;
        that.enabled_ = true;
    }

    if (str == switchRenderingSaverText) {
        that.ClearEvents();
        that.usingSaver_ = true;
        that.enabled_ = false;
        that.saver_ = std::make_unique<FrameSaver>();
    }

    if (str == switchRenderingDisableText) {
        that.usingSaver_ = false;
        that.enabled_ = false;
    }

    if (that.enabled_ != oldEnable && that.repaint_ != nullptr) {
        that.repaint_();
    }
}
} // namespace Rosen
} // namespace OHOS
