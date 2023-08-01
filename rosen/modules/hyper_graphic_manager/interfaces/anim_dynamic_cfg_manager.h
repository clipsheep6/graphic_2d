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

#ifndef HGM_ANIM_DYNAMIC_CFG_MANAGER_H
#define HGM_ANIM_DYNAMIC_CFG_MANAGER_H


#include <list>
#include <mutex>
#include <unordered_map>

#include "ipc_callbacks/anim_dynamic_cfg_callback.h"
#include "refbase.h"

namespace OHOS {
namespace Rosen {

class HgmAnimDynamicCfgManager : public RefBase {
public:
    HgmAnimDynamicCfgManager() = default;
    virtual ~HgmAnimDynamicCfgManager() noexcept = default;

    virtual bool Init() noexcept = 0;

    virtual bool GetAnimDynamicCfgCallback(const sptr<RSIAnimDynamicCfgCallback> &callback) = 0;
    virtual void RemoveAnimDynamicCfgCallback(const sptr<RSIAnimDynamicCfgCallback> &callback) = 0;
};

enum class AnimDynamicCfgEvent: bool {
    FAILURE,
    SUCCESS,
};

namespace impl {
class HgmAnimDynamicCfgManager : public OHOS::Rosen::HgmAnimDynamicCfgManager {
public:
    static sptr<OHOS::Rosen::HgmAnimDynamicCfgManager> GetInstance() noexcept;

    // noncopyable
    HgmAnimDynamicCfgManager(const HgmAnimDynamicCfgManager &) = delete;
    HgmAnimDynamicCfgManager &operator=(const HgmAnimDynamicCfgManager &) = delete;

    bool Init() noexcept override;


    bool GetAnimDynamicCfgCallback(const sptr<RSIAnimDynamicCfgCallback> &callback) override;
    void RemoveAnimDynamicCfgCallback(const sptr<RSIAnimDynamicCfgCallback> &callback) override;

private:
    std::mutex mutex_;
    std::list<sptr<RSIAnimDynamicCfgCallback>> animDynamicCfgCallbacks_;
};
} // namespace impl


} // namespace Rosen
} // namespace OHOS
#endif // HGM_ANIM_DYNAMIC_CFG_MANAGER_H