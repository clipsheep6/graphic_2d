/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "anim_dynamic_cfg_manager.h"
#include "anim_dynamic_configs.h"
#include "hgm_log.h"

namespace OHOS {
namespace Rosen {
namespace impl {
bool HgmAnimDynamicCfgManager::GetAnimDynamicCfgCallback(const sptr<RSIAnimDynamicCfgCallback> &callback)
{
    if (callback == nullptr) {
        HGM_LOGE("HgmAnimDynamicCfgManager %s: callback is NULL.", __func__);
        return static_cast<bool>(AnimDynamicCfgEvent::FAILURE);
    }

    std::lock_guard<std::mutex> lock(mutex_);

    AnimDynamicCfgs configs;

    callback->GetAnimDynamicConfigs(configs);
    animDynamicCfgCallbacks_.push_back(callback);
    HGM_LOGD("HgmAnimDynamicCfgManager %s: add a remote callback succeed.", __func__);
    return static_cast<bool>(AnimDynamicCfgEvent::SUCCESS);
}

void HgmAnimDynamicCfgManager::RemoveAnimDynamicCfgCallback(const sptr<RSIAnimDynamicCfgCallback> &callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = animDynamicCfgCallbacks_.begin(); it != animDynamicCfgCallbacks_.end(); it++) {
        if (*it == callback) {
            animDynamicCfgCallbacks_.erase(it);
            HGM_LOGD("HgmAnimDynamicCfgManager %s: remove a remote callback succeed.", __func__);
            break;
        }
    }
}
} // namespace impl
} // namespace Rosen
} // namespace OHOS