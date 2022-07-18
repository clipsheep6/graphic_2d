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

#include "animation/rs_animation_manager_map.h"

#include "animation/rs_ui_animation_manager.h"

namespace OHOS {
namespace Rosen {
RSAnimationManagerMap& RSAnimationManagerMap::Instance()
{
    static RSAnimationManagerMap managerMap;
    return managerMap;
}

const std::shared_ptr<RSUIAnimationManager>& RSAnimationManagerMap::GetAnimationManager(const int32_t id)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = managerMap_.find(id);
    if (iter != managerMap_.end()) {
        return iter->second;
    }
    auto manager = std::make_shared<RSUIAnimationManager>();
    managerMap_.emplace(id, manager);
    return manager;
}
} // namespace Rosen
} // namespace OHOS
