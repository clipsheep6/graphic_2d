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
#ifndef UIEFFECT_FILTER_WATER_RIPPPLE_PARA_H
#define UIEFFECT_FILTER_WATER_RIPPPLE_PARA_H
#include <iostream>
#include "filter_para.h"
#include "common/rs_vector2.h"
 
namespace OHOS {
namespace Rosen {
class WaterRipplePara : public FilterPara {
public:
    WaterRipplePara()
    {
        this->type_ = FilterPara::ParaType::WATER_RIPPLE;
    }
    ~WaterRipplePara() override = default;
    
    void SetProgress(float progress)
    {
        progress_ = progress;
    }
 
    float GetProgress() const
    {
        return progress_;
    }
 
    void SetWaveNum(float waveNum)
    {
        waveNum_ = waveNum;
    }
 
    float GetWaveNum () const
    {
        return waveNum_;
    }
 
    void SetRippleCenter(Vector2f rippleCenter)
    {
        rippleCenter_ = rippleCenter;
    }
 
    const Vector2f GetRippleCenter() const
    {
        return rippleCenter_;
    }
 
 
 
private:
    Vector2f rippleCenter_;
    float progress_ = 0.0f;
    float waveNum_ = 0.0f;
 
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_BLUR_PARA_H
