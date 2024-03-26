/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "sync_fence_tracker.h"
#include <chrono>
#include "hilog/log.h"
#include "sync_report.h"
#include "rs_trace.h"
#include "sync_report.h"

namespace OHOS {
using namespace OHOS::HiviewDFX;
namespace {
constexpr int BLUR_EVENT_ID = 100007;
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400
#undef LOG_TAG
#define LOG_TAG "SyncFence"
}

SyncFenceTracker::SyncFenceTracker(const std::string threadName)
    : threadName_(threadName),
    fencesQueued_(0),
    fencesSignaled_(0)
{
    runner_ = OHOS::AppExecFwk::EventRunner::Create(threadName_);
    handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner_);
    if (threadName == "Acquire Fence") {
        isGpuFence_ = true;
    }
}

void SyncFenceTracker::TrackFence(const sptr<SyncFence>& fence, uint32_t bufferCount)
{
    if (fence->SyncFileReadTimestamp() != SyncFence::FENCE_PENDING_TIMESTAMP) {
        RS_TRACE_NAME_FMT("%s %d has signaled", threadName_.c_str(), fencesQueued_.load());
        fencesQueued_++;
        fencesSignaled_++;
        return;
    }

    RS_TRACE_NAME_FMT("%s %d", threadName_.c_str(), fencesQueued_.load());
    if (handler_) {
        int flushEndTime = 0;
        handler_->PostTask([this, fence, bufferCount, flushEndTime]() {
            Loop(fence, bufferCount, flushEndTime);
        });
        fencesQueued_++;
    }
}

void SyncFenceTracker::Loop(const sptr<SyncFence>& fence, uint32_t bufferCount, int flushEndTime)
{
    uint32_t fenceIndex = 0;
    fenceIndex = fencesSignaled_.load();
    {
        RS_TRACE_NAME_FMT("Waiting for %s %d", threadName_.c_str(), fenceIndex);
        if (isGpuFence_) {
            Rosen::SyncReport::GetInstance().MonitorGpuStart(bufferCount, flushEndTime);
        }
        int32_t result = fence->Wait(SYNC_TIME_OUT);
        if (isGpuFence_) {
            Rosen::SyncReport::GetInstance().MonitorGpuEnd();
        }
        if (result < 0) {
            HILOG_DEBUG(LOG_CORE, "Error waiting for SyncFence: %s", strerror(result));
        }
    }
    fencesSignaled_++;
}

void SyncFenceTracker::SetBlurCnt(int blurCnt)
{
    if (handler_) {
        int flushBeginTime = GetTime();
        handler_->PostTask([blurCnt, flushBeginTime]() {
            Rosen::SyncReport::GetInstance().SetFrameParam(BLUR_EVENT_ID, flushBeginTime, 0, blurCnt);
        });
    }
}

int SyncFenceTracker::GetTime()
{
    auto now = std::chrono::steady_clock::now();
    int time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    return time;
}
} // namespace OHOS
