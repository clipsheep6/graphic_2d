/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "hgm_touch_manager.h"

#include "common/rs_optional_trace.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "hgm_log.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr int32_t IDLE_TIMER_EXPIRED = 200; // ms
    constexpr int32_t IDLE_AFTER_TOUCH_UP = 3000; // ms
}

void TouchStateMachine::TouchEventHandle(TouchEvent event)
{
    switch (event) {
    case TouchEvent::DOWN:
        TouchDown();
        break;
    case TouchEvent::UP:
        TouchUp();
        break;
    case TouchEvent::THREE_SECONDS_TIMEOUT:
        TouchUpTimeout();
        break;
    case TouchEvent::RS_200_MS_TIMEOUT:
        RSIdleTimeout();
        break;
    default:
        break;
    }
}

void TouchStateMachine::TouchDown()
{
    currentState = TouchState::DOWN;
    auto& hgmCore = HgmCore::Instance();
    StopTouchTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId());
    HGM_LOGI("[touch machine] Swicth to touch down status");
}

void TouchStateMachine::TouchUp()
{
    if (currentState == TouchState::DOWN) {
        currentState = TouchState::UP;
        auto& hgmCore = HgmCore::Instance();
        StartTouchTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId(), IDLE_AFTER_TOUCH_UP, nullptr, [this]() {
            TouchUpTimeout();
        });
        HGM_LOGI("[touch machine] DOWN Swicth to touch UP status");
    } else {
        HGM_LOGD("[touch machine] NOT support to touch up status");
    }
}

void TouchStateMachine::TouchUpTimeout()
{
    if (currentState == TouchState::UP) {
        currentState = TouchState::IDLE;
        if (idleEventCallback_ != nullptr) {
            idleEventCallback_();
        }
        HGM_LOGI("[touch machine] Switch to 3S touch up Idle status");
    } else {
        HGM_LOGD("[touch machine] 3S Touch up already in Idle status");
    }
}

void TouchStateMachine::RSIdleTimeout() 
{
    if (currentState == TouchState::UP) {
        currentState = TouchState::IDLE;
        if (idleEventCallback_ != nullptr) {
            idleEventCallback_();
        }
        HGM_LOGI("[touch machine] Switch to RS 200MS Idle status");
    } else {
        HGM_LOGD("[touch machine] RS 200MS already in Idle status");
    }
}

void TouchStateMachine::StartTouchTimer(ScreenId screenId, int32_t interval,
    std::function<void()> resetCallback, std::function<void()> expiredCallback)
{
    auto newTimer = std::make_shared<HgmOneShotTimer>("idle_touchUp_timer" + std::to_string(screenId),
        std::chrono::milliseconds(interval), resetCallback, expiredCallback);
    touchTimerMap_[screenId] = newTimer;
    newTimer->Start();
}

void TouchStateMachine::StopTouchTimer(ScreenId screenId)
{
    if (auto timer = touchTimerMap_.find(screenId); timer != touchTimerMap_.end()) {
        touchTimerMap_.erase(timer);
    }
}

void HgmTouchManager::HandleRSFrameUpdate(bool idleTimerExpired)
{
    RS_TRACE_FUNC();

    auto& hgmCore = HgmCore::Instance();
    if (touchMachine_.GetState() == TouchState::UP) {
        if (idleTimerExpired) {
            if (rsRealIdleCnt > 0) {
                RS_TRACE_NAME_FMT("RS Idle 200 MS timer expired");
                HGM_LOGI("[touch manager] the %{public}d time report RS idle", rsRealIdleCnt);
                rsRealIdleCnt--;
                // support XX RS idle detect
                ResetRSTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId());
                StartRSTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId(), IDLE_TIMER_EXPIRED, nullptr, [this]() {
                    rsIdleUpdateCallback_(true);
                });
            } else {
                touchMachine_.TouchEventHandle(TouchEvent::RS_200_MS_TIMEOUT);
                StopRSTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId());
            }
        } else {
            HGM_LOGI("[touch manager] Start RS 200 MS Idle timer");
            rsRealIdleCnt = rsIdleCnt_;
            StartRSTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId(), IDLE_TIMER_EXPIRED, nullptr, [this]() {
                rsIdleUpdateCallback_(true);
            });
        }
    } else {
        StopRSTimer(hgmCore.GetFrameRateMgr()->GetCurScreenId());
    }
}

std::shared_ptr<HgmOneShotTimer> HgmTouchManager::GetRSTimer(ScreenId screenId) const
{
    if (auto timer = rsTimerMap_.find(screenId); timer != rsTimerMap_.end()) {
        return timer->second;
    }
    return nullptr;
}

void HgmTouchManager::StartRSTimer(ScreenId screenId, int32_t interval,
    std::function<void()> resetCallback, std::function<void()> expiredCallback)
{
    if (auto oldtimer = GetRSTimer(screenId); oldtimer == nullptr) {
        auto newTimer = std::make_shared<HgmOneShotTimer>("idle_rsIdle_timer" + std::to_string(screenId),
            std::chrono::milliseconds(interval), resetCallback, expiredCallback);
        rsTimerMap_[screenId] = newTimer;
        newTimer->Start();
    }
}

void HgmTouchManager::ResetRSTimer(ScreenId screenId) const
{
    if (auto timer = GetRSTimer(screenId); timer != nullptr) {
        timer->Reset();
    }
}

void HgmTouchManager::StopRSTimer(ScreenId screenId)
{
    if (auto timer = rsTimerMap_.find(screenId); timer != rsTimerMap_.end()) {
        rsTimerMap_.erase(timer);
    }
}

} // namespace Rosen
} // namespace OHOS