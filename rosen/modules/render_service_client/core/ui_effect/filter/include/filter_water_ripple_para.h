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
 
namespace OHOS {
namespace Rosen {
class WaterRipplePara : public FilterPara {
public:
    WaterRipplePara();

    ~WaterRipplePara() override;

    void SetProgress(float progress);
    float GetProgress() const;
    void SetWaveCount(uint32_t waveCount);
    uint32_t GetWaveCount () const;
    void SetRippleCenterX(float rippleCenterX);
    float GetRippleCenterX() const;
    void SetRippleCenterY(float rippleCenterY);
    float GetRippleCenterY() const;
    void SetRippleMode(uint32_t rippleMode);
    uint32_t GetRippleMode() const;

private:
    float rippleCenterX_ = 0.0f;
    float rippleCenterY_ = 0.0f;
    float progress_ = 0.0f;
    uint32_t waveCount_ = 0;
    uint32_t rippleMode_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_WATER_RIPPPLE_PARA_H
