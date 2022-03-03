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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_LOG_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_LOG_H

#include <fstream>
#include <set>
#include <unordered_map>

#include "animation/rs_animatable_property.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
const std::string ANIMATION_LOG_PATH = "/etc/rosen/";
const std::string ANIMATION_LOG_FILE_NAME = "Animation.log";

class RSAnimationLog {
public:
    RSAnimationLog();
    ~RSAnimationLog();

    static RSAnimationLog& Instance();

    void InitNodeAndPropertyInfo();

    void ClearNodeAndPropertyInfo();

    bool IsNeedWriteLog(const RSAnimatableProperty& property, const NodeId& id);

    template<typename T>
    void WritePropertyValueToLog(const T& value, const RSAnimatableProperty& property, const NodeId& id)
    {
        if (!logFile_.is_open()) {
            std::string logFilePath = ANIMATION_LOG_PATH + ANIMATION_LOG_FILE_NAME;
            logFile_.open(logFilePath, std::ios::app);
        }
        std::string logValue = GetLogValue(value, property, id);
        logFile_.write(logValue.c_str(), logValue.size());
    }

private:
    void DealConfigInputInfo(const std::string& info);

    int64_t GetNowTime();

    template<typename T>
    std::string GetLogValue(const T& value, const RSAnimatableProperty& property, const NodeId& id);

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

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_LOG_H
