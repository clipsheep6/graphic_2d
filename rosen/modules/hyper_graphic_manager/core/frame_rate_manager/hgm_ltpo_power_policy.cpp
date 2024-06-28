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
#include "hgm_ltpo_power_policy.h"

#include <functional>

#include "hgm_core.h"
#include "hgm_log.h"

#include "common/rs_common_def.h"
#include "common/rs_common_hook.h"

namespace OHOS::Rosen {
static const std::string IS_ANIMATION_LTPO_POWER_ENABLE = "animation_ltpo_power_enable";
static const std::string ANIMATION_DILE_FPS = "animation_idle_fps";
static const std::string ANIMATION_IDLE_DURATION = "animation_idle_duration";
constexpr int DEFAULT_ANIMATION_IDLE_FPS = 60;
constexpr int DEFAULT_ANIMATION_IDLE_DURATION = 2000;

HgmLtpoPowerPolicy::HgmLtpoPowerPolicy()
{
    RsCommonHook::Instance().RegisterStartNewAnimationListner(std::bind(&HgmLtpoPowerPolicy::StartNewAnimation, this));
}

HgmLtpoPowerPolicy& HgmLtpoPowerPolicy::Instance()
{
    static HgmLtpoPowerPolicy hlpp;
    return hlpp;
}

void HgmLtpoPowerPolicy::ConverStrToInt(int& targetNum, std::string sourceStr, int defaultValue)
{
    targetNum = std::stoi(sourceStr.c_str());
    if (targetNum == 0) {
        targetNum = defaultValue;
    }
}

void HgmLtpoPowerPolicy::SetConfig(std::unordered_map<std::string, std::string> animationPowerConfig)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (animationPowerConfig.count(IS_ANIMATION_LTPO_POWER_ENABLE) == 0) {
        isAnimationLtpoPowerEnable_ = false;
    } else {
        isAnimationLtpoPowerEnable_ = animationPowerConfig[IS_ANIMATION_LTPO_POWER_ENABLE] == "true" ? true : false;
    }
    if (!isAnimationLtpoPowerEnable_) {
        HGM_LOGI("HgmLtpoPowerPolicy::SetConfig isAnimationLtpoPowerEnable is false");
        return;
    }

    if (animationPowerConfig.count(DEFAULT_ANIMATION_IDLE_FPS) == 0) {
        animationIdleFps = DEFAULT_ANIMATION_IDLE_FPS;
    } else {
        ConverStrToInt(animationIdleFps, animationPowerConfig[ANIMATION_DILE_FPS], DEFAULT_ANIMATION_IDLE_FPS);
    }

    if (animationPowerConfig.count(ANIMATION_IDLE_DURATION) == 0) {
        animationIdleDuration = DEFAULT_ANIMATION_IDLE_DURATION;
    } else {
        ConverStrToInt(
            animationIdleDuration, animationPowerConfig[ANIMATION_IDLE_DURATION], DEFAULT_ANIMATION_IDLE_DURATION);
    }
    HGM_LOGI("HgmLtpoPowerPolicy::SetConfig update config success");
}

void HgmLtpoPowerPolicy::SetIdleState(bool isIdle)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isAnimationLtpoPowerEnable_ || isIdle_ == isIdle) {
        return;
    }
    isIdle_ = isIdle;
    firstAnimationTimestamp_ = HgmCore::Instance().GetCurrentTimestamp() / NS_PER_MS;
    lastAnimationTimestamp_ = firstAnimationTimestamp_;
}

void HgmLtpoPowerPolicy::StatisticAnimationTime(uint64_t timestamp)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (isAnimationLtpoPowerEnable_ || !isIdle_) {
        return;
    }
    lastAnimationTimestamp_ = timestamp;
}

void HgmLtpoPowerPolicy::StartNewAnimation()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (isAnimationLtpoPowerEnable_ || !isIdle_) {
        return;
    }
    firstAnimationTimestamp_ = HgmCore::Instance().GetCurrentTimestamp() / NS_PER_MS;
    lastAnimationTimestamp_ = firstAnimationTimestamp_;
}

void HgmLtpoPowerPolicy::GetAnimationIdleFps(FrameRateRange& rsRange)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (isAnimationLtpoPowerEnable_ || !isIdle_) {
        return;
    }
    if (lastAnimationTimestamp_ > firstAnimationTimestamp_ &&
        lastAnimationTimestamp_ - firstAnimationTimestamp_ < static_cast<uint64_t>(animationIdleDuration_)) {
        return;
    }
    rsRange.max_ = std::min(rsRange.max_, animationIdleFps_);
    rsRange.min_ = std::min(rsRange.min_, animationIdleFps_);
    rsRange.preferred_ = std::min(rsRange.preferred_, animationIdleFps_);
}

} // namespace OHOS::Rosen