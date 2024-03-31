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

#ifndef HGM_TOUCH_MANAGER_H
#define HGM_TOUCH_MANAGER_H

#include <mutex>
#include <unordered_map>

#include "hgm_screen.h"
#include "hgm_one_shot_timer.h"

namespace OHOS {
namespace Rosen {
enum class TouchState : uint32_t {
    DOWN = 1,
    UP = 2,
    IDLE = 3
};

enum class TouchEvent {
    DOWN,
    UP,
    THREE_SECONDS_TIMEOUT,
    RS_TIMEOUT
};

using IdleEventCallback = std::function<void()>;

class TouchStateMachine {
public:
    TouchStateMachine() : currentState(TouchState::IDLE) {}

    void RegisterIdleEventCallback(IdleEventCallback idleEventCallback)
    {
        idleEventCallback_ = idleEventCallback;
    }

    void TouchEventHandle(TouchEvent event);
    
    void TouchDown();
    void TouchUp();
    void TouchUpTimeout();
    void RSIdleTimeout();

    void StartTouchTimer(ScreenId screenId, int32_t interval,
        std::function<void()> resetCallback, std::function<void()> expiredCallback);
    void StopTouchTimer(ScreenId screenId);

    TouchState GetState() const
    {
        return currentState;
    }
private:
    TouchState currentState;
    std::mutex touchStateMutex_;
    std::unordered_map<ScreenId, std::shared_ptr<HgmOneShotTimer>> touchTimerMap_;
    IdleEventCallback idleEventCallback_ = nullptr;
};
class HgmTouchManager {
public:
    HgmTouchManager() = default;
    ~HgmTouchManager() = default;

    void HandleRSFrameUpdate(bool idleTimerExpired);
    std::shared_ptr<HgmOneShotTimer> GetRSTimer(ScreenId screenId) const;
    void ResetRSTimer(ScreenId screenId) const;
    void StartRSTimer(ScreenId screenId, int32_t interval,
        std::function<void()> resetCallback, std::function<void()> expiredCallback);
    void StopRSTimer(ScreenId screenId);

    void SetRSIdleUpdateCallback(std::function<void(bool)> rsIdleUpdateCallback)
    {
        rsIdleUpdateCallback_ = rsIdleUpdateCallback;
    }
    std::function<void(bool)> rsIdleUpdateCallback_;

    TouchStateMachine touchMachine_;
private:
    std::unordered_map<ScreenId, std::shared_ptr<HgmOneShotTimer>> rsTimerMap_;
    int32_t rsIdleCnt_ = 3;
    int32_t rsRealIdleCnt = 3;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_TOUCH_MANAGER_H