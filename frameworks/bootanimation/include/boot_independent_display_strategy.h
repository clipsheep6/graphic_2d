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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_INDEPENDENT_DISPLAY_STRATEGY_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_INDEPENDENT_DISPLAY_STRATEGY_H

#include "boot_animation_strategy.h"
#include "boot_animation_operation.h"

namespace OHOS {
class BootIndependentDisplayStrategy : public BootAnimationStrategy {

public:
    BootIndependentDisplayStrategy() = default;

    virtual ~BootIndependentDisplayStrategy() = default;

    void Display(std::vector<BootAnimationConfig>& configs) override;

private:
    std::vector<std::shared_ptr<BootAnimationOperation>> operators_;
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_INDEPENDENT_DISPLAY_STRATEGY_H
