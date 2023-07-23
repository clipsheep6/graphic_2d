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

#ifndef ROSEN_JANK_STATS_H
#define ROSEN_JANK_STATS_H

#include <cstdint>
#include <string>
#include <vector>

#include "transaction/rs_render_service_client.h"

namespace OHOS {

namespace {
using UniqueId = int64_t;
struct JankFrames {
    bool isSetReportEventResponse_ = false;
    bool isSetReportEventComplete_ = false;
    bool isReportEventResponse_ = false;
    bool isReportEventComplete_ = false;
    bool isSetReportEventJankFrame_ = false;
    bool isUpdateJankFrame_ = false;
    uint64_t setTime_ = 0;
    int32_t SeqMissedFrames = 0;
    int32_t totalFrames_ = 0;
    int32_t totalMissedFrames_ = 0;
    uint64_t maxFrameTime_ = 0;
    int32_t maxSeqMissedFrames_ = 0;
    uint64_t totalFrameTime_ = 0;
    Rosen::DataBaseRs info_;
};
} // namespace

namespace Rosen {
class RSJankStats {
public:
    static RSJankStats& GetInstance();
    void SetStartTime();
    void SetEndTime();
    void SetRSJankStats(int times);
    void UpdateJankFrame(uint64_t duration, JankFrames& jankFrames);
    void ReportJankStats();
    void SetReportEventResponse(DataBaseRs info);
    void SetReportEventComplete(DataBaseRs info);
    void SetReportEventJankFrame(DataBaseRs info);
    void ReportEventResponse(JankFrames& jankFrames);
    void ReportEventComplete(JankFrames& jankFrames);
    void ReportEventJankFrame(JankFrames& jankFrames);
    void ReportEventFirstFrame();
    void SetFirstFrame();
    void SetPid(pid_t pid);

private:
    RSJankStats() {};
    ~RSJankStats() {};
    RSJankStats(const RSJankStats&) = delete;
    RSJankStats(const RSJankStats&&) = delete;
    void operator=(const RSJankStats&) = delete;
    void operator=(const RSJankStats&&) = delete;
    uint64_t ConvertTimeToSystime(uint64_t time);
    uint64_t GetCurrentSystimeMs();

    constexpr static int JANK_STATS_SIZE = 8;
    bool isfirstSetStart_ = true;
    bool isNeedReport_ = false;
    uint64_t startTime_ = 0;
    uint64_t endTime_ = 0;
    uint64_t lastReportTime_ = 0;
    bool isFirstFrame_ = false;
    int32_t appPid_ = 0;
    std::vector<uint16_t> rsJankStats_ = std::vector<uint16_t>(JANK_STATS_SIZE, 0);
    std::map<UniqueId, JankFrames> animateJankFrames_;

    enum JankRangeType : int16_t {
        JANK_FRAME_6_FREQ = 0,
        JANK_FRAME_15_FREQ,
        JANK_FRAME_20_FREQ,
        JANK_FRAME_36_FREQ,
        JANK_FRAME_48_FREQ,
        JANK_FRAME_60_FREQ,
        JANK_FRAME_120_FREQ,
        JANK_FRAME_180_FREQ,
        JANK_FRAME_INVALID,
    };
};

} // namespace Rosen
} // namespace OHOS
#endif