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
#ifndef HGM_LTPO_POWER_POLICY_H
#define HGM_LTPO_POWER_POLICY_H
#include <mutex>
#include <string>
#include <unordered_map>

#include "animation/rs_frame_rate_range.h"
namespace OHOS::Rosen {
class HgmLtpoPowerPolicy {
private:
    std::recursive_mutex mutex_;
    bool isAnimationLtpoPowerEnable_ = false;
    bool isIdle_ = false;
    uint64_t firstAnimationTimestamp_ = 0;
    uint64_t lastAnimationTimestamp_ = 0;
    // Unit: ms
    int animationIdleDuration_ = 2000;
    int animationIdleFps_ = 60;

    HgmLtpoPowerPolicy();
    ~HgmLtpoPowerPolicy() = default;
    HgmLtpoPowerPolicy(const HgmLtpoPowerPolicy&) = delete;
    HgmLtpoPowerPolicy(const HgmLtpoPowerPolicy&&) = delete;
    HgmLtpoPowerPolicy& operator=(const HgmLtpoPowerPolicy&) = delete;
    HgmLtpoPowerPolicy& operator=(const HgmLtpoPowerPolicy&&) = delete;
    void ConverStrToInt(int& targetNum, std::string sourceStr, int defaultValue);

public:
    static HgmLtpoPowerPolicy& Instance();
    void SetConfig(std::unordered_map<std::string, std::string> animationPowerConfig);
    void SetIdleState(bool isIdle);
    void StatisticAnimationTime(uint64_t timestamp);
    void StartNewAnimation();
    void GetAnimationIdleFps(FrameRateRange& rsRange);
};
} // namespace OHOS::Rosen

#endif