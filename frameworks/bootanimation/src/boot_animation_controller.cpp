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
#include "boot_animation_controller.h"

#include "boot_animation_factory.h"
#include "config_policy_utils.h"
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <iremote_object.h>
#include <system_ability_definition.h>
#include "util.h"

using namespace OHOS;

void BootAnimationController::Start()
{
    LOGI("BootAnimationController START");
    WaitRenderServiceInit();
    std::string path = GetConfigFilePath();
    if (!ParseBootConfig(path, isCompatible_, isMultiDisplay_, animationConfigs_)) {
        LOGI("parse config json error, use default");
        isCompatible_ = true;
        CreateDefaultBootConfig();
    }

    BootStrategyType bootType = GetBootType();
    strategy_ = BootAnimationFactory::GetBootStrategy(bootType);
    if (strategy_) {
        strategy_->Display(animationConfigs_);
    }
}

void BootAnimationController::WaitRenderServiceInit() const
{
    while (true) {
        sptr<ISystemAbilityManager> saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (saMgr == nullptr) {
            LOGI("saMgr is null, please wait...");
            usleep(SLEEP_TIME_US);
            continue;
        }
        sptr<IRemoteObject> renderObj = saMgr->GetSystemAbility(RENDER_SERVICE);
        if (renderObj == nullptr) {
            LOGI("renderService is not initialized, please wait...");
            usleep(SLEEP_TIME_US);
        } else {
            LOGI("renderService is initialized");
            break;
        }
    }
}

std::string BootAnimationController::GetConfigFilePath()
{
    char buf[MAX_PATH_LEN] = {0};
    char *path = GetOneCfgFile(BOOT_CUSTOM_CONFIG_PATH_SUFFIX.c_str(), buf, MAX_PATH_LEN);
    if (path != nullptr) {
        LOGI("boot config path: %{public}s", path);
        return path;
    } else {
        LOGE("path not find %{public}s", BOOT_CUSTOM_CONFIG_PATH_SUFFIX.c_str());
    }
    return "";
}

void BootAnimationController::CreateDefaultBootConfig()
{
    BootAnimationConfig config;
    config.screenId = 0;
    animationConfigs_.emplace_back(config);
}

BootStrategyType BootAnimationController::GetBootType() const
{
    if (isCompatible_) {
        return BootStrategyType::COMPATIBLE;
    }

    if (isMultiDisplay_ || animationConfigs_.size() == 1) {
        return BootStrategyType::INDEPENDENT;
    }

    return BootStrategyType::ASSOCIATIVE;
}
