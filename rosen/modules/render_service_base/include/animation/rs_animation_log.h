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

#ifndef RENDER_SERVICE_BASE_ANIMATION_RS_ANIMATION_LOG_H
#define RENDER_SERVICE_BASE_ANIMATION_RS_ANIMATION_LOG_H

#include <fstream>
#include <set>
#include <unordered_map>

#include "animation/rs_animatable_property.h"
#include "platform/common/rs_log.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
const std::string ANIMATION_LOG_PATH = "/etc/rosen/";
const std::string ANIMATION_LOG_FILE_NAME = "Animation";
const std::string ANIMATION_LOG_FILE_TYPE = ".log";

class RSAnimationLog {
public:
    RSAnimationLog();
    ~RSAnimationLog();

    void InitNodeAndPropertyInfo();

    void CalcSizeSegmentationLogFile(std::string logFilePath);

    void ClearNodeAndPropertyInfo();

    bool IsNeedWriteLog(const RSAnimatableProperty& property, const NodeId& id);

    template<typename T>
    void WritePropertyValueToLog(const T& value, const RSAnimatableProperty& property, const NodeId& id)
    {
        if (!logFile_.is_open()) {
            return;
        }

        std::string logValue = GetLogValue(value, property, id);
        logFile_.write(logValue.c_str(), logValue.size());
        logFile_.flush();
    }

    template<typename T>
    void WritePropertyValueToLog(const T& value, const RSAnimatableProperty& property, const AnimationId& id,
        const T& startValue, const T& endValue)
    {
        if (!logFile_.is_open()) {
            ROSEN_LOGE("logFile_ Open error");
            return;
        }

        std::string logValue = GetLogValue(value, property, id, startValue, endValue);
        logFile_.write(logValue.c_str(), logValue.size());
        logFile_.flush();
    }

private:
    void DealConfigInputInfo(const std::string& info);

    int64_t GetNowTime();

    template<typename T>
    std::string GetLogValue(const T& value, const RSAnimatableProperty& property, const NodeId& id);

    template<typename T>
    std::string GetLogValue(const T& value, const RSAnimatableProperty& property, const AnimationId& id,
        const T& startValue, const T& endValue);

    bool needWriteAllNode_ {false};
    bool needWriteAllProperty_ {false};
    std::ofstream logFile_;
    std::set<NodeId> nodeIdSet_;
    std::set<RSAnimatableProperty> propertySet_;
    static std::unordered_map<RSAnimatableProperty, std::string> propertyToNum_;
    static std::unordered_map<std::string, RSAnimatableProperty> numToProperty_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_ANIMATION_RS_ANIMATION_LOG_H
