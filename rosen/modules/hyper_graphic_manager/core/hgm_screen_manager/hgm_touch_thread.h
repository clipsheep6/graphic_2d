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

#ifndef HGM_TOUCH_THREAD_H
#define HGM_TOUCH_THREAD_H

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <refbase.h>
#include <sched.h>
#include <string>
#include <sys/resource.h>
#include <thread>

#include "hgm_core.h"

namespace OHOS::Rosen {
class HgmTouchThread : public RefBase {
public:
    static sptr<OHOS::Rosen::HgmTouchThread> Instance() noexcept;

    void OnTouchEventReceived(int32_t event);
    void Init();

private:
    HgmTouchThread();
    ~HgmTouchThread();
    HgmTouchThread(const HgmTouchThread&);
    HgmTouchThread(const HgmTouchThread&&);
    HgmTouchThread& operator=(const HgmTouchThread&);
    HgmTouchThread& operator=(const HgmTouchThread&&);

    void CheckTimerLoop();
    void CheckTimer();
    void HandleTouchEvent(int32_t event);
    void HandleTouchDown();
    void HandleTouchMove();
    void HandleTouchUp();

    bool iSReceivedTouchDown_ = false;
    static std::once_flag createFlag_;
    static sptr<OHOS::Rosen::HgmTouchThread> instance_;

    // timer thread
    bool doCheck_ = false;
    bool resetTimer_ = false;
    int64_t startTime_ = 0;
    std::thread checkThread_;
    std::mutex touchEventMutex_;
    std::mutex checkIdleTimeMutex_;
    std::condition_variable checkTimerAfterTouchUp_;

    // time check
    static constexpr int64_t BEFORE_TOUCHUP_RATEDOWN = 3000000000;
    static constexpr int64_t BEFORE_IDLE_RATEDOWN = 1000000000;
    bool CheckTimeUp();

    // set regresh rate
    int32_t highRefreshRateTouchEvent_ = 90;
    int32_t lowRefreshRateTouchEvent_ = 60;
    int32_t SetTouchHighRefreshRate();
    int32_t SetTouchLowRefreshRate();

    // time period between 2 timer check
    std::condition_variable checkTimer_;
};

class FrameRefreshDetector {
public:
    static FrameRefreshDetector* Instance();
    void UpdateLastFrameTimestamp();
    bool IsFrameRefreshActive();

private:
    FrameRefreshDetector();
    FrameRefreshDetector(const FrameRefreshDetector& instance);
    FrameRefreshDetector& operator=(const FrameRefreshDetector& instance);
    ~FrameRefreshDetector() {}

    int64_t maxTimeBeforeIdle_ = 1000000000;
    int64_t lastFrameTimeStamp_ = 0;
    std::mutex detectorLock_;
};
} // namespace OHOS::Rosen
#endif