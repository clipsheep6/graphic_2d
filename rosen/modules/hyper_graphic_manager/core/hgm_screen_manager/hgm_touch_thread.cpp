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

#include "hgm_touch_thread.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include "hgm_core.h"

namespace OHOS::Rosen {
namespace {
static int64_t GetSysTimeNs()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}
}

std::once_flag HgmTouchThread::createFlag_;
sptr<OHOS::Rosen::HgmTouchThread> HgmTouchThread::instance_ = nullptr;

HgmTouchThread::HgmTouchThread()
{
    doCheck_ = true;
    checkThread_ = std::thread(std::bind(&HgmTouchThread::CheckTimerLoop, this));
    pthread_setname_np(checkThread_.native_handle(), "HgmTouchThread");
}

HgmTouchThread::~HgmTouchThread()
{
    // {
    //     std::unique_lock<std::mutex> locker(mutex_);
    //     vsyncThreadRunning_ = false;
    // }
    if (checkThread_.joinable()) {
        checkTimerAfterTouchUp_.notify_all();
        checkThread_.join();
    }
}

sptr<OHOS::Rosen::HgmTouchThread> HgmTouchThread::Instance() noexcept
{
    std::call_once(createFlag_, []() {
        auto instance = new HgmTouchThread();
        instance_ = instance;
    });

    return instance_;
}

void HgmTouchThread::Init()
{
}

void HgmTouchThread::CheckTimer()
{
    // got touch event, waiting for an up event
    if (!iSReceivedTouchDown_) {
        HGM_LOGI("HgmTouchThread::CheckTimer waiting for a touch up event");
        std::unique_lock<std::mutex> lck(checkIdleTimeMutex_);
        checkTimerAfterTouchUp_.wait(lck);
        startTime_ = GetSysTimeNs();
        return;
    }

    // got up event, start timing
    if (!CheckTimeUp()) {
        return;
    }

    // timing ends, lower the refreshrate
    SetTouchLowRefreshRate();
    iSReceivedTouchDown_ = false;
}

void HgmTouchThread::OnTouchEventReceived(int32_t event)
{
    HandleTouchEvent(event);
}

void HgmTouchThread::HandleTouchEvent(int32_t event)
{
    if (event == static_cast<int32_t>(HGM_TOUCH_DOWN)) {
        HandleTouchDown();
    } else if (event == static_cast<int32_t>(HGM_TOUCH_MOVE)) {
        HandleTouchMove();
    } else if (event == static_cast<int32_t>(HGM_TOUCH_UP)) {
        HandleTouchUp();
    } else {
        HGM_LOGE("HgmTouchThread unknown touch event reveived: %{public}d", event);
    }
}

void HgmTouchThread::HandleTouchDown()
{
    std::lock_guard<std::mutex> lck(touchEventMutex_);

    SetTouchHighRefreshRate();
    startTime_ = GetSysTimeNs();
    iSReceivedTouchDown_ = true;
}

void HgmTouchThread::HandleTouchMove()
{
    std::lock_guard<std::mutex> lck(touchEventMutex_);

    if (resetTimer_) {
        startTime_ = GetSysTimeNs();
    }
    iSReceivedTouchDown_ = true;
}

void HgmTouchThread::HandleTouchUp()
{
    std::lock_guard<std::mutex> lck(touchEventMutex_);

    resetTimer_ = true;
    checkTimerAfterTouchUp_.notify_all();
}

void HgmTouchThread::CheckTimerLoop()
{
    while (doCheck_) {
        CheckTimer();
    }
}

bool HgmTouchThread::CheckTimeUp()
{
    std::unique_lock<std::mutex> lck(checkIdleTimeMutex_);
    checkTimer_.wait_for(lck, std::chrono::milliseconds(200));

    // check if no touch event
    int64_t currentTime = GetSysTimeNs();
    if (currentTime > startTime_ && currentTime - startTime_ > BEFORE_TOUCHUP_RATEDOWN) {
        return true;
    }

    // check if no frame refresh, idle
    if (currentTime - startTime_ > BEFORE_IDLE_RATEDOWN &&
        !FrameRefreshDetector::Instance()->IsFrameRefreshActive()) {
        return true;
    }

    return false;
}

int32_t HgmTouchThread::SetTouchHighRefreshRate()
{
    auto &hgmCore = HgmCore::Instance();
    return hgmCore.SetScreenRefreshRate(hgmCore.GetActiveScreenId(), 0, highRefreshRateTouchEvent_);
}

int32_t HgmTouchThread::SetTouchLowRefreshRate()
{
    auto &hgmCore = HgmCore::Instance();
    return hgmCore.SetScreenRefreshRate(hgmCore.GetActiveScreenId(), 0, lowRefreshRateTouchEvent_);
}

FrameRefreshDetector::FrameRefreshDetector() {}

FrameRefreshDetector* FrameRefreshDetector::Instance()
{
    static FrameRefreshDetector instance;
    return &instance;
}

void FrameRefreshDetector::UpdateLastFrameTimestamp()
{
    std::lock_guard<std::mutex> lck(detectorLock_);
    lastFrameTimeStamp_ = GetSysTimeNs();
}

bool FrameRefreshDetector::IsFrameRefreshActive()
{
    std::lock_guard<std::mutex> lck(detectorLock_);

    int64_t timeDiff = GetSysTimeNs() - lastFrameTimeStamp_;
    return timeDiff < maxTimeBeforeIdle_ ? true : false;
}
} // namespace OHOS::Rosen