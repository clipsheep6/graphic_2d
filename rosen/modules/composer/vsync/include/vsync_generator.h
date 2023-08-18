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


#ifndef VSYNC_VSYNC_GENERATOR_H
#define VSYNC_VSYNC_GENERATOR_H

#include <refbase.h>
#include "graphic_common.h"

#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>

namespace OHOS {
namespace Rosen {
class VSyncGenerator : public RefBase {
public:
    class Callback : public RefBase {
    public:
        virtual void OnVSyncEvent(int64_t now, int64_t period, int32_t refreshRate) = 0;
    };
    VSyncGenerator() = default;
    virtual ~VSyncGenerator() noexcept = default;
    virtual VsyncError UpdateMode(int64_t period, int64_t phase, int64_t referenceTime) = 0;
    virtual VsyncError AddListener(int64_t phase, const sptr<Callback>& cb) = 0;
    virtual VsyncError RemoveListener(const sptr<Callback>& cb) = 0;
    virtual VsyncError ChangePhaseOffset(const sptr<Callback>& cb, int64_t offset) = 0;
    virtual bool IsEnable() = 0;
    virtual VsyncError SetGeneratorRefreshRate(int32_t refreshRate) = 0;
    virtual int64_t GetVSyncPulse() = 0;
    virtual int64_t GetReferenceTimeOffset() = 0;
};

sptr<VSyncGenerator> CreateVSyncGenerator();
void DestroyVSyncGenerator();

namespace impl {
class VSyncGenerator : public OHOS::Rosen::VSyncGenerator {
public:
    static sptr<OHOS::Rosen::VSyncGenerator> GetInstance() noexcept;
    static void DeleteInstance() noexcept;

    // nocopyable
    VSyncGenerator(const VSyncGenerator &) = delete;
    VSyncGenerator &operator=(const VSyncGenerator &) = delete;
    VsyncError UpdateMode(int64_t period, int64_t phase, int64_t referenceTime) override;
    VsyncError AddListener(int64_t phase, const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb) override;
    VsyncError RemoveListener(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb) override;
    VsyncError ChangePhaseOffset(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb, int64_t offset) override;
    bool IsEnable() override;
    VsyncError SetGeneratorRefreshRate(int32_t refreshRate) override;
    int64_t GetVSyncPulse() override;
    int64_t GetReferenceTimeOffset() override;

private:
    friend class OHOS::Rosen::VSyncGenerator;

    struct Listener {
        int64_t phase_;
        sptr<OHOS::Rosen::VSyncGenerator::Callback> callback_;
        int64_t lastTime_;
    };

    VSyncGenerator();
    ~VSyncGenerator() noexcept override;

    int64_t ComputeNextVSyncTimeStamp(int64_t now, int64_t referenceTime);
    std::vector<Listener> GetListenerTimeouted(int64_t now, int64_t referenceTime);
    int64_t ComputeListenerNextVSyncTimeStamp(const Listener &listen, int64_t now, int64_t referenceTime);
    void ThreadLoop();
    void UpdateWakeupDelay(int64_t occurTimestamp, int64_t nextTimeStamp);
    void UpdatePulseLocked(int64_t period);

    int64_t period_;
    int64_t phase_;
    int64_t referenceTime_;
    int64_t wakeupDelay_;
    int64_t pulse_; // 360Hz脉冲的单位时间（单位：纳秒）
    int32_t refreshRate_; // vsync刷新率，更新period_时需要同时更改refreshRate_（单位：HZ）
    bool shouldUpdateVsyncInfo_;
    // 说明：referenceTimeOffset_表示generator的referenceTime_与硬件vsync时间戳之间的偏移值，必须是pulse的整数倍。
    // 使用硬件vsync时间戳更新referenceTime_时，计算referenceTime_与硬件vsync时间戳之间的差值并就近取一个pulse的整数倍值更新referenceTimeOffset_。
    // referenceTime_ = 硬件vsync时间戳 - referenceTimeOffset_。
    int64_t referenceTimeOffset_; // 单位：纳秒

    std::vector<Listener> listeners_;

    std::mutex mutex_;
    std::condition_variable con_;
    std::mutex waitForTimeoutMtx_;
    std::condition_variable waitForTimeoutCon_;
    std::thread thread_;
    bool vsyncThreadRunning_;
    static std::once_flag createFlag_;
    static sptr<OHOS::Rosen::VSyncGenerator> instance_;
};
} // impl
} // namespace Rosen
} // namespace OHOS

#endif
