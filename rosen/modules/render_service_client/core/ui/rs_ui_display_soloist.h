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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DISPLAY_SOLOIST_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DISPLAY_SOLOIST_H

#include <atomic>
#include <functional>
#include <mutex>

#include "command/rs_animation_command.h"
#include "common/rs_common_def.h"
#include "rs_frame_rate_linker.h"
#include "transaction/rs_interfaces.h"
#include "vsync_receiver.h"

namespace OHOS {
namespace Rosen {

using SoloistIdType = uint32_t;
using TimestampType = int64_t;
using DisplaySoloistOnFrameCallback = std::function<void(long long, long long, void*)>;

const std::string TIME_OUT_TASK = "vsync_time_out_task_";
constexpr int64_t TIME_OUT_MILLISECONDS = 600;
static const std::vector<int32_t> REFRESH_RATE_LIST{ 90, 120, 144 };
static std::vector<int32_t> REFRESH_RATE_FACTORS;
static std::unordered_map<int32_t, std::vector<int32_t>> RATE_TO_FACTORS;
static std::once_flag COMPUTE_FACTORS_FLAG;
constexpr float SECOND_IN_NANO = 1000000000.0f;
constexpr int32_t FRAME_RATE_0 = 0;
constexpr int32_t SOLOIST_ERROR = -1;
constexpr int32_t EXEC_SUCCESS = 0;

enum class ActiveStatus : int32_t {
    INACTIVE = 0,
    ACTIVE = 1,
    NEED_REMOVE = 2,
};

class RSC_EXPORT SoloistId {
public:
    SoloistId();
    ~SoloistId();

    SoloistIdType GetId() const;
    static std::shared_ptr<SoloistId> Create();
private:
    static SoloistIdType GenerateId();
    const SoloistIdType id_;
};

class RSC_EXPORT RSDisplaySoloist {
public:
    RSDisplaySoloist(SoloistIdType instanceId);
    ~RSDisplaySoloist()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        destroyed_ = true;
    }

    void Init();
    bool JudgeWhetherSkip(TimestampType timestamp);
    void RequestNextVSync();

    void FlushFrameRate(int32_t rate);
    void SetSubFrameRateLinkerEnable(bool enabled);

    int64_t GetVSyncPeriod();
    bool SetVSyncRate(int32_t vsyncRate);
    int32_t GetVSyncRate();

    RSDisplaySoloist() = default;

    bool IsCommonDivisor(int32_t expectedRate, int32_t vsyncRate);
    std::vector<int32_t> FindRefreshRateFactors(int32_t refreshRate);
    void FindAllRefreshRateFactors();
    int32_t FindMatchedRefreshRate(int32_t vsyncRate, int32_t targetRate);
    int32_t FindAccurateRefreshRate(int32_t approximateRate);
    int32_t SearchMatchedRate(const FrameRateRange& frameRateRange, int32_t vsyncRate,
                              int32_t iterCount = 1);

    std::shared_ptr<AppExecFwk::EventHandler> subVsyncHandler_ = nullptr;
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> subReceiver_ = nullptr;
    std::pair<DisplaySoloistOnFrameCallback, void*> callback_;
    static void OnVsync(TimestampType timestamp, void* client);
    void VsyncCallbackInner(TimestampType timestamp);
    VSyncReceiver::FrameCallback subFrameCallback_ = {
        .userData_ = this,
        .callback_ = OnVsync,
    };
    enum ActiveStatus subStatus_ = ActiveStatus::INACTIVE;
    bool hasInitVsyncReceiver_ = false;

    int32_t sourceVsyncRate_ = 0;
    int32_t drawFPS_;
    int32_t currRate_;
    int32_t currCnt_;
    TimestampType timestamp_;
    TimestampType targetTimestamp_;

    SoloistIdType instanceId_ = 0;
    bool useExclusiveThread_ = false;
    FrameRateRange frameRateRange_;
    std::shared_ptr<RSFrameRateLinker> frameRateLinker_;

    void OnVsyncTimeOut();
    std::mutex mtx_;
    bool hasRequestedVsync_ = false;
    bool destroyed_ = false;
    std::string vsyncTimeoutTaskName_ = "";
    AppExecFwk::EventHandler::Callback vsyncTimeoutCallback_ = std::bind(&RSDisplaySoloist::OnVsyncTimeOut, this);
};

using IdToSoloistMapType = std::unordered_map<SoloistIdType, std::shared_ptr<RSDisplaySoloist>>;

class RSC_EXPORT RSDisplaySoloistManager {
public:
    static RSDisplaySoloistManager& GetInstance() noexcept;
    bool InitVsyncReceiver();
    void RequestNextVSync();

    void Start(SoloistIdType id);
    void Stop(SoloistIdType id);

    void InsertOnVsyncCallback(SoloistIdType id, DisplaySoloistOnFrameCallback cb, void* data);
    void InsertFrameRateRange(SoloistIdType id, FrameRateRange frameRateRange);
    void InsertUseExclusiveThreadFlag(SoloistIdType id, bool useExclusiveThread);
    void RemoveSoloist(SoloistIdType id);

    void FlushFrameRate(int32_t rate);
    void SetMainFrameRateLinkerEnable(bool enabled);

    FrameRateRange GetFrameRateRange();
    IdToSoloistMapType GetIdToSoloistMap();
    std::shared_ptr<RSFrameRateLinker> GetFrameRateLinker();
    enum ActiveStatus GetManagerStatus();

    int64_t GetVSyncPeriod() const;
    bool SetVSyncRate(int32_t vsyncRate);
    int32_t GetVSyncRate() const;

private:
    RSDisplaySoloistManager() = default;
    RSDisplaySoloistManager(const RSDisplaySoloistManager&) = delete;
    RSDisplaySoloistManager(const RSDisplaySoloistManager&&) = delete;
    RSDisplaySoloistManager& operator=(const RSDisplaySoloistManager&) = delete;
    RSDisplaySoloistManager& operator=(const RSDisplaySoloistManager&&) = delete;

    std::shared_ptr<AppExecFwk::EventHandler> vsyncHandler_ = nullptr;
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver_ = nullptr;
    static void OnVsync(TimestampType timestamp, void* client);
    void VsyncCallbackInner(TimestampType timestamp);
    void DispatchSoloistCallback(TimestampType timestamp);
    VSyncReceiver::FrameCallback managerFrameCallback_ = {
        .userData_ = this,
        .callback_ = OnVsync,
    };

    enum ActiveStatus managerStatus_ = ActiveStatus::INACTIVE;
    bool hasInitVsyncReceiver_ = false;
    int32_t sourceVsyncRate_ = 0;

    FrameRateRange frameRateRange_;
    std::shared_ptr<RSFrameRateLinker> frameRateLinker_;
    IdToSoloistMapType idToSoloistMap_;

    void OnVsyncTimeOut();
    std::mutex mtx_;
    std::mutex dataUpdateMtx_;
    std::string vsyncTimeoutTaskName_ = "soloist_manager_time_out_task";
    AppExecFwk::EventHandler::Callback vsyncTimeoutCallback_ =
        std::bind(&RSDisplaySoloistManager::OnVsyncTimeOut, this);
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DISPLAY_SOLOIST_H
