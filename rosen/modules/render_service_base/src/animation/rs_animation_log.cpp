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

#include "animation/rs_animation_log.h"

#include <regex>
#include <sys/time.h>

#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
const std::string CONFIG_FILE_NAME = "property.config";
const std::string COLON_SEPARATOR = ":";
const std::string COMMA_SEPARATOR = ",";
const std::string SEMICOLON_SEPARATOR = ";";
const std::string SPACE_SEPARATOR = " ";
const std::string PROPERTY_TAG = "PROPERTY";
const std::string NODE_ID_TAG = "ID";
const std::string ALL_NEED_TAG = "all";
const std::string ANIMATION_LOG_TAG = "RSAnimationLog";
const std::string ANIMATION_VALUE_LOG_TAG = "RSAnimationValueLog";
const std::string START_VALUE = "startValue";
const std::string END_VALUE = "endValue";
static constexpr int MIN_INFO_SIZE = 2;
static constexpr int MATRIX_DATA_SIZE = 9;
static constexpr int VECTOR_DATA_SIZE = 4;
static constexpr size_t LOG_FILE_MAX_SIZE = 10485760;

std::vector<std::string> SplitStringBySeparator(const std::string& str,
    const std::string& separator)
{
    std::regex re(separator);
    std::sregex_token_iterator pos(str.begin(), str.end(), re, -1);
    std::sregex_token_iterator endPos;
    std::vector<std::string> result;
    while (pos != endPos)
        result.emplace_back(*pos++);
 
    return result;
}

RSAnimationLog::RSAnimationLog()
{
    std::string logFilePath = ANIMATION_LOG_PATH + ANIMATION_LOG_FILE_NAME + ANIMATION_LOG_FILE_TYPE;
    CalcSizeSegmentationLogFile(logFilePath);
    logFile_ = std::ofstream(logFilePath, std::ios::app);
    
    if (!logFile_.is_open()) {
        ROSEN_LOGE("Open file err:[%s]", logFilePath.c_str());
        logFile_.close();
        return;
    }
}

RSAnimationLog::~RSAnimationLog()
{
    logFile_.close();
}

void RSAnimationLog::CalcSizeSegmentationLogFile(std::string logFilePath)
{
    std::ifstream logFile = std::ifstream(logFilePath.c_str(), std::ios::binary);
    if (!logFile) {
        ROSEN_LOGE("Open file err:[%s]", logFilePath.c_str());
        logFile.close();
        return;
    }
    logFile.seekg(0, logFile.end);
    size_t logFileSize = logFile.tellg();
    if (logFileSize >= LOG_FILE_MAX_SIZE) {
        std::string timestampPath = ANIMATION_LOG_PATH + ANIMATION_LOG_FILE_NAME + std::to_string(GetNowTime())
            + ANIMATION_LOG_FILE_TYPE;
        std::rename(logFilePath.c_str(), timestampPath.c_str());
    }
    logFile.close();
}

void RSAnimationLog::InitNodeAndPropertyInfo()
{
    std::string configFilePath = ANIMATION_LOG_PATH + CONFIG_FILE_NAME;
    std::ifstream configFile(configFilePath.c_str());
    if (!configFile.is_open()) {
        return;
    }

    std::string info;
    while (std::getline(configFile, info)) {
        std::vector<std::string> infos = SplitStringBySeparator(info, SEMICOLON_SEPARATOR);
        if (infos.empty()) {
            continue;
        }
        DealConfigInputInfo(infos.front());
    }
    configFile.close();
}

void RSAnimationLog::DealConfigInputInfo(const std::string& info)
{
    std::vector<std::string> splitResult = SplitStringBySeparator(info, COLON_SEPARATOR);
    if (splitResult.size() != MIN_INFO_SIZE) {
        return;
    }

    std::string tag = splitResult.front();
    if (tag == PROPERTY_TAG) {
        std::vector<std::string> props =
            SplitStringBySeparator(splitResult.back(), COMMA_SEPARATOR);
        for (std::string prop: props) {
            if (prop == ALL_NEED_TAG) {
                needWriteAllProperty_ = true;
                return;
            }
            auto iter = numToProperty_.find(prop);
            if (iter != numToProperty_.end()) {
                RSAnimatableProperty property = numToProperty_[prop];
                propertySet_.insert(property);
            }
        }
    }

    if (tag == NODE_ID_TAG) {
        std::vector<std::string> nodeIds =
            SplitStringBySeparator(splitResult.back(), COMMA_SEPARATOR);
        for (std::string nodeId : nodeIds) {
            if (nodeId == ALL_NEED_TAG) {
                needWriteAllNode_ = true;
                return;
            }
            auto id = atoll(nodeId.c_str());
            nodeIdSet_.insert(id);
        }
    }
}

void RSAnimationLog::ClearNodeAndPropertyInfo()
{
    propertySet_.clear();
    nodeIdSet_.clear();
}

bool RSAnimationLog::IsNeedWriteLog(const RSAnimatableProperty& property, const NodeId& id)
{
    auto itrp = propertySet_.find(property);
    if (itrp == propertySet_.end() && !needWriteAllProperty_) {
        return false;
    }

    auto itrn = nodeIdSet_.find(id);
    if (itrn == nodeIdSet_.end() && !needWriteAllNode_) {
        return false;
    }

    return true;
}

int64_t RSAnimationLog::GetNowTime()
{
    struct timeval start = {};
    gettimeofday(&start, nullptr);
    constexpr uint32_t secToUsec = 1000 * 1000;
    return static_cast<uint64_t>(start.tv_sec) * secToUsec + start.tv_usec;
}

template<>
std::string RSAnimationLog::GetLogValue(const int& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    std::string logValue = ANIMATION_LOG_TAG + std::to_string(id) + SPACE_SEPARATOR + std::to_string(GetNowTime()) +
        SPACE_SEPARATOR + propertyToNum_[property] + SPACE_SEPARATOR + std::to_string(value) + "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const float& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    std::string logValue = ANIMATION_LOG_TAG + std::to_string(id) + SPACE_SEPARATOR + std::to_string(GetNowTime()) +
        SPACE_SEPARATOR + propertyToNum_[property] + SPACE_SEPARATOR + std::to_string(value) + "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const Color& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    std::string logValue = ANIMATION_LOG_TAG + std::to_string(id) + SPACE_SEPARATOR + std::to_string(GetNowTime()) +
        SPACE_SEPARATOR + propertyToNum_[property] + SPACE_SEPARATOR + std::to_string(value.GetRed()) +
        SPACE_SEPARATOR + std::to_string(value.GetGreen()) + SPACE_SEPARATOR + std::to_string(value.GetBlue()) +
        SPACE_SEPARATOR + std::to_string(value.GetAlpha()) + "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const Matrix3f& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    auto data = value.GetConstData();
    std::string logValue = ANIMATION_LOG_TAG + std::to_string(id) + SPACE_SEPARATOR + std::to_string(GetNowTime()) +
        SPACE_SEPARATOR + propertyToNum_[property];
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(data[i]));
    }
    logValue += "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const Vector2f& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    std::string logValue = ANIMATION_LOG_TAG + std::to_string(id) + SPACE_SEPARATOR + std::to_string(GetNowTime()) +
        SPACE_SEPARATOR + propertyToNum_[property] + SPACE_SEPARATOR + std::to_string(value.x_) +
        SPACE_SEPARATOR + std::to_string(value.y_) + "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const Vector4f& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    auto data = value;
    std::string logValue = ANIMATION_LOG_TAG + std::to_string(id) + SPACE_SEPARATOR + std::to_string(GetNowTime()) +
        SPACE_SEPARATOR + propertyToNum_[property];
    for (int i = 0; i < VECTOR_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(data[i]));
    }
    logValue += "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const Quaternion& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    auto data = value;
    std::string logValue = ANIMATION_LOG_TAG + std::to_string(id) + SPACE_SEPARATOR + std::to_string(GetNowTime()) +
        SPACE_SEPARATOR + propertyToNum_[property];
    for (int i = 0; i < VECTOR_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(data[i]));
    }
    logValue += "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const std::shared_ptr<RSFilter>& value,
    const RSAnimatableProperty& property, const NodeId& id)
{
    std::string logValue = ANIMATION_LOG_TAG + std::to_string(id) + SPACE_SEPARATOR + std::to_string(GetNowTime()) +
        SPACE_SEPARATOR + propertyToNum_[property] + "\n";
    return logValue;
}

/* begin */
template<>
std::string RSAnimationLog::GetLogValue(const int& value,
    const RSAnimatableProperty& property, const AnimationId& id, const int& startValue, const int& endValue)
{
    std::string logValue = ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + std::to_string(id) + SPACE_SEPARATOR +
        std::to_string(GetNowTime()) + SPACE_SEPARATOR + propertyToNum_[property] + SPACE_SEPARATOR + 
        std::to_string(value) + "\n";
    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + START_VALUE + SPACE_SEPARATOR +
        std::to_string(startValue) + "\n";
    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + END_VALUE + SPACE_SEPARATOR +
        std::to_string(endValue) + "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const float& value,
    const RSAnimatableProperty& property, const AnimationId& id, const float& startValue, const float& endValue)
{
    std::string logValue = ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + std::to_string(id) + SPACE_SEPARATOR +
        std::to_string(GetNowTime()) + SPACE_SEPARATOR + propertyToNum_[property] + SPACE_SEPARATOR + 
        std::to_string(value) + "\n";
    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + START_VALUE + SPACE_SEPARATOR +
        std::to_string(startValue) + "\n";
    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + END_VALUE + SPACE_SEPARATOR +
        std::to_string(endValue) + "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const Color& value,
    const RSAnimatableProperty& property, const AnimationId& id, const Color& startValue, const Color& endValue)
{
    std::string logValue = ANIMATION_VALUE_LOG_TAG + std::to_string(id) + SPACE_SEPARATOR +
        std::to_string(GetNowTime()) + SPACE_SEPARATOR + propertyToNum_[property] + SPACE_SEPARATOR +
        std::to_string(value.GetRed()) + SPACE_SEPARATOR + std::to_string(value.GetGreen()) + SPACE_SEPARATOR +
        std::to_string(value.GetBlue()) + SPACE_SEPARATOR + std::to_string(value.GetAlpha()) + "\n";
    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + START_VALUE + SPACE_SEPARATOR +
        std::to_string(startValue.GetGreen()) + SPACE_SEPARATOR +
        std::to_string(startValue.GetBlue()) + SPACE_SEPARATOR +
        std::to_string(startValue.GetAlpha()) + "\n";
    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + END_VALUE + SPACE_SEPARATOR +
        std::to_string(endValue.GetGreen()) + SPACE_SEPARATOR +
        std::to_string(endValue.GetBlue()) + SPACE_SEPARATOR +
        std::to_string(endValue.GetAlpha()) + "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const Matrix3f& value,
    const RSAnimatableProperty& property, const AnimationId& id, const Matrix3f& startValue, const Matrix3f& endValue)
{
    auto data = value.GetConstData();
    std::string logValue = ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + std::to_string(id) + SPACE_SEPARATOR +
        std::to_string(GetNowTime()) + SPACE_SEPARATOR + propertyToNum_[property];
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(data[i]));
    }
    logValue += "\n";

    auto startData = startValue.GetConstData();
    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + START_VALUE;
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(startData[i]));
    }
    logValue += "\n";

    auto endData = endValue.GetConstData();
    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + END_VALUE;
    for (int i = 0; i < MATRIX_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(endData[i]));
    }
    logValue += "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const Vector2f& value,
    const RSAnimatableProperty& property, const AnimationId& id, const Vector2f& startValue, const Vector2f& endValue)
{
    std::string logValue = ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + std::to_string(id) + SPACE_SEPARATOR +
        std::to_string(GetNowTime()) + SPACE_SEPARATOR + propertyToNum_[property] + SPACE_SEPARATOR +
        std::to_string(value.x_) + SPACE_SEPARATOR + std::to_string(value.y_) + "\n";
    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + START_VALUE + SPACE_SEPARATOR +
        std::to_string(startValue.x_) + SPACE_SEPARATOR +
        std::to_string(startValue.y_) + "\n";
    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + END_VALUE + SPACE_SEPARATOR +
        std::to_string(endValue.x_) + SPACE_SEPARATOR +
        std::to_string(endValue.y_) + "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const Vector4f& value,
    const RSAnimatableProperty& property, const AnimationId& id, const Vector4f& startValue,
    const Vector4f& endValue)
{
    std::string logValue = ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + std::to_string(id) + SPACE_SEPARATOR +
        std::to_string(GetNowTime()) + SPACE_SEPARATOR + propertyToNum_[property];
    for (int i = 0; i < VECTOR_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(value[i]));
    }
    logValue += "\n";

    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + START_VALUE;
    for (int i = 0; i < VECTOR_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(startValue[i]));
    }
    logValue += "\n";

    logValue += ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + END_VALUE;
    for (int i = 0; i < VECTOR_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(endValue[i]));
    }
    logValue += "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const Quaternion& value,
    const RSAnimatableProperty& property, const AnimationId& id, const Quaternion& startValue,
    const Quaternion& endValue)
{
    std::string logValue = ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + std::to_string(id) + SPACE_SEPARATOR +
        std::to_string(GetNowTime()) + SPACE_SEPARATOR + propertyToNum_[property];
    for (int i = 0; i < VECTOR_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(value[i]));
    }
    logValue += "\n";
    
    logValue += ANIMATION_VALUE_LOG_TAG + START_VALUE;
    for (int i = 0; i < VECTOR_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(startValue[i]));
    }
    logValue += "\n";
    
    logValue += ANIMATION_VALUE_LOG_TAG + END_VALUE;
    for (int i = 0; i < VECTOR_DATA_SIZE; i++) {
        logValue += (SPACE_SEPARATOR + std::to_string(endValue[i]));
    }
    logValue += "\n";
    return logValue;
}

template<>
std::string RSAnimationLog::GetLogValue(const std::shared_ptr<RSFilter>& value,
    const RSAnimatableProperty& property, const AnimationId& id, const std::shared_ptr<RSFilter>& startValue,
    const std::shared_ptr<RSFilter>& endValue)
{
    std::string logValue = ANIMATION_VALUE_LOG_TAG + SPACE_SEPARATOR + std::to_string(id) + SPACE_SEPARATOR +
        std::to_string(GetNowTime()) + SPACE_SEPARATOR + propertyToNum_[property] + "\n";;
    return logValue;
}
/* end */

std::unordered_map<RSAnimatableProperty, std::string> RSAnimationLog::propertyToNum_ = {
    {RSAnimatableProperty::INVALID, "0"},
    {RSAnimatableProperty::ALPHA, "1"},
    {RSAnimatableProperty::BOUNDS_WIDTH, "2"},
    {RSAnimatableProperty::BOUNDS_HEIGHT, "3"},
    {RSAnimatableProperty::BOUNDS_SIZE, "4"},
    {RSAnimatableProperty::BOUNDS_POSITION_X, "5"},
    {RSAnimatableProperty::BOUNDS_POSITION_Y, "6"},
    {RSAnimatableProperty::BOUNDS_POSITION, "7"},
    {RSAnimatableProperty::BOUNDS, "8"},
    {RSAnimatableProperty::FRAME_WIDTH, "9"},
    {RSAnimatableProperty::FRAME_HEIGHT, "10"},
    {RSAnimatableProperty::FRAME_SIZE, "11"},
    {RSAnimatableProperty::FRAME_POSITION_X, "12"},
    {RSAnimatableProperty::FRAME_POSITION_Y, "13"},
    {RSAnimatableProperty::FRAME_POSITION, "14"},
    {RSAnimatableProperty::FRAME, "15"},
    {RSAnimatableProperty::PIVOT_X, "16"},
    {RSAnimatableProperty::PIVOT_Y, "17"},
    {RSAnimatableProperty::PIVOT, "18"},
    {RSAnimatableProperty::CORNER_RADIUS, "19"},
    {RSAnimatableProperty::POSITION_Z, "20"},
    {RSAnimatableProperty::TRANSLATE_X, "21"},
    {RSAnimatableProperty::TRANSLATE_Y, "22"},
    {RSAnimatableProperty::TRANSLATE_Z, "23"},
    {RSAnimatableProperty::TRANSLATE, "24"},
    {RSAnimatableProperty::SCALE_X, "25"},
    {RSAnimatableProperty::SCALE_Y, "26"},
    {RSAnimatableProperty::SCALE, "27"},
    {RSAnimatableProperty::ROTATION, "28"},
    {RSAnimatableProperty::ROTATION_X, "29"},
    {RSAnimatableProperty::ROTATION_Y, "30"},
    {RSAnimatableProperty::ROTATION_3D, "31"},
    {RSAnimatableProperty::BACKGROUND_COLOR, "32"},
    {RSAnimatableProperty::BGIMAGE_WIDTH, "33"},
    {RSAnimatableProperty::BGIMAGE_HEIGHT, "34"},
    {RSAnimatableProperty::BGIMAGE_POSITION_X, "35"},
    {RSAnimatableProperty::BGIMAGE_POSITION_Y, "36"},
    {RSAnimatableProperty::FOREGROUND_COLOR, "37"},
    {RSAnimatableProperty::SHADOW_OFFSET_X, "38"},
    {RSAnimatableProperty::SHADOW_OFFSET_Y, "39"},
    {RSAnimatableProperty::SHADOW_ALPHA, "40"},
    {RSAnimatableProperty::SHADOW_ELEVATION, "41"},
    {RSAnimatableProperty::SHADOW_RADIUS, "42"},
    {RSAnimatableProperty::SHADOW_COLOR, "43"},
    {RSAnimatableProperty::BORDER_COLOR, "44"},
    {RSAnimatableProperty::BORDER_WIDTH, "45"},
    {RSAnimatableProperty::FILTER, "46"},
    {RSAnimatableProperty::BACKGROUND_FILTER, "47"},
    {RSAnimatableProperty::SUB_LAYER_TRANSFORM, "48"},
};

std::unordered_map<std::string, RSAnimatableProperty> RSAnimationLog::numToProperty_ = {
    {"0", RSAnimatableProperty::INVALID},
    {"1", RSAnimatableProperty::ALPHA},
    {"2", RSAnimatableProperty::BOUNDS_WIDTH},
    {"3", RSAnimatableProperty::BOUNDS_HEIGHT},
    {"4", RSAnimatableProperty::BOUNDS_SIZE},
    {"5", RSAnimatableProperty::BOUNDS_POSITION_X},
    {"6", RSAnimatableProperty::BOUNDS_POSITION_Y},
    {"7", RSAnimatableProperty::BOUNDS_POSITION},
    {"8", RSAnimatableProperty::BOUNDS},
    {"9", RSAnimatableProperty::FRAME_WIDTH},
    {"10", RSAnimatableProperty::FRAME_HEIGHT},
    {"11", RSAnimatableProperty::FRAME_SIZE},
    {"12", RSAnimatableProperty::FRAME_POSITION_X},
    {"13", RSAnimatableProperty::FRAME_POSITION_Y},
    {"14", RSAnimatableProperty::FRAME_POSITION},
    {"15", RSAnimatableProperty::FRAME},
    {"16", RSAnimatableProperty::PIVOT_X},
    {"17", RSAnimatableProperty::PIVOT_Y},
    {"18", RSAnimatableProperty::PIVOT},
    {"19", RSAnimatableProperty::CORNER_RADIUS},
    {"20", RSAnimatableProperty::POSITION_Z},
    {"21", RSAnimatableProperty::TRANSLATE_X},
    {"22", RSAnimatableProperty::TRANSLATE_Y},
    {"23", RSAnimatableProperty::TRANSLATE_Z},
    {"24", RSAnimatableProperty::TRANSLATE},
    {"25", RSAnimatableProperty::SCALE_X},
    {"26", RSAnimatableProperty::SCALE_Y},
    {"27", RSAnimatableProperty::SCALE},
    {"28", RSAnimatableProperty::ROTATION},
    {"29", RSAnimatableProperty::ROTATION_X},
    {"30", RSAnimatableProperty::ROTATION_Y},
    {"31", RSAnimatableProperty::ROTATION_3D},
    {"32", RSAnimatableProperty::BACKGROUND_COLOR},
    {"33", RSAnimatableProperty::BGIMAGE_WIDTH},
    {"34", RSAnimatableProperty::BGIMAGE_HEIGHT},
    {"35", RSAnimatableProperty::BGIMAGE_POSITION_X},
    {"36", RSAnimatableProperty::BGIMAGE_POSITION_Y},
    {"37", RSAnimatableProperty::FOREGROUND_COLOR},
    {"38", RSAnimatableProperty::SHADOW_OFFSET_X},
    {"39", RSAnimatableProperty::SHADOW_OFFSET_Y},
    {"40", RSAnimatableProperty::SHADOW_ALPHA},
    {"41", RSAnimatableProperty::SHADOW_ELEVATION},
    {"42", RSAnimatableProperty::SHADOW_RADIUS},
    {"43", RSAnimatableProperty::SHADOW_COLOR},
    {"44", RSAnimatableProperty::BORDER_COLOR},
    {"45", RSAnimatableProperty::BORDER_WIDTH},
    {"46", RSAnimatableProperty::FILTER},
    {"47", RSAnimatableProperty::BACKGROUND_FILTER},
    {"48", RSAnimatableProperty::SUB_LAYER_TRANSFORM},
};
} // namespace Rosen
} // namespace OHOS
