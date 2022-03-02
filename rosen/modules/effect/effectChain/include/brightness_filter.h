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

#ifndef BRIGHTNESS_FILTER_H
#define BRIGHTNESS_FILTER_H

#include "algo_filter.h"

namespace OHOS {
namespace Rosen {
class BrightnessFilter : public AlgoFilter {
public:
    // brightness value ranges from -1.0 to 1.0, with 0.0 as the normal level
    static constexpr float DEFAULT_BRIGHTNESS = 0.0f;
    BrightnessFilter();
    void SetValue(const std::string& key, void* value, int size) override;
    std::string GetVertexShader() override;
    std::string GetFragmentShader() override;

private:
    void LoadFilterParams() override;
    float brightness_ = DEFAULT_BRIGHTNESS;
    GLint brightnessID_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // BRIGHTNESS_FILTER_H