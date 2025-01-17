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

#include "luminance/rs_luminance_control.h"

namespace {
constexpr float HDR_DEFAULT_TMO_NIT = 1000.0f;
}

namespace OHOS {
namespace Rosen {
RSLuminanceControl::~RSLuminanceControl()
{
    // destructor
}

bool RSLuminanceControl::SetHdrStatus(ScreenId screenId, bool isHdrOn, int32_t type)
{
    // Update HDR status in order to determine brightness.
    return false;
}

bool RSLuminanceControl::IsHdrOn(ScreenId screenId)
{
    return false;
}

bool RSLuminanceControl::IsDimmingOn(ScreenId screenId)
{
    return false;
}

void RSLuminanceControl::DimmingIncrease(ScreenId screenId)
{
    // Update HDR dimming index.
}

void RSLuminanceControl::SetSdrLuminance(ScreenId screenId, uint32_t level)
{
    // Update SDR brightness.
}

uint32_t RSLuminanceControl::GetNewHdrLuminance(ScreenId screenId)
{
    return 0;
}

void RSLuminanceControl::SetNowHdrLuminance(ScreenId screenId, uint32_t level)
{
    // Update HDR brightness after dimming increase.
}

bool RSLuminanceControl::IsNeedUpdateLuminance(ScreenId screenId)
{
    return false;
}

float RSLuminanceControl::GetHdrTmoNits(ScreenId screenId, int mode)
{
    return HDR_DEFAULT_TMO_NIT;
}

float RSLuminanceControl::GetHdrDisplayNits(ScreenId screenId)
{
    return HDR_DEFAULT_TMO_NIT;
}

double RSLuminanceControl::GetHdrBrightnessRatio(ScreenId screenId, int mode)
{
    return 1.0; // 1.0 refers to default value, no need to process.
}
} // namespace Rosen
} // namespace OHOS