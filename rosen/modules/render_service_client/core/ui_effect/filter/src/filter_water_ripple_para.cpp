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

#include "ui_effect/filter/include/filter_water_ripple_para.h"
 
namespace OHOS {
namespace Rosen {

    WaterRipplePara::WaterRipplePara()
    {
        this->type_ = FilterPara::ParaType::WATER_RIPPLE;
    }

    WaterRipplePara::~WaterRipplePara() = default;
    
    void WaterRipplePara::SetProgress(float progress)
    {
        progress_ = progress;
    }
 
    float WaterRipplePara::GetProgress() const
    {
        return progress_;
    }
 
    void WaterRipplePara::SetWaveCount(uint32_t waveCount)
    {
        waveCount_ = waveCount;
    }
 
    uint32_t WaterRipplePara::GetWaveCount() const
    {
        return waveCount_;
    }
 
    void WaterRipplePara::SetRippleCenterX(float rippleCenterX)
    {
        rippleCenterX_ = rippleCenterX;
    }
 
    float WaterRipplePara::GetRippleCenterX() const
    {
        return rippleCenterX_;
    }

    void WaterRipplePara::SetRippleCenterY(float rippleCenterY)
    {
        rippleCenterY_ = rippleCenterY;
    }
 
    float WaterRipplePara::GetRippleCenterY() const
    {
        return rippleCenterY_;
    }

    void WaterRipplePara::SetRippleMode(uint32_t rippleMode)
    {
        rippleMode_ = rippleMode;
    }

    uint32_t WaterRipplePara::GetRippleMode() const
    {
        return rippleMode_;
    }

} // namespace Rosen
} // namespace OHOS
