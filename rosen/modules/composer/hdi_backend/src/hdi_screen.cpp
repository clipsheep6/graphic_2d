/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_screen.h"

#include "hdi_log.h"
#include "vsync_module.h"

namespace OHOS {
namespace Rosen {

std::unique_ptr<HdiScreen> HdiScreen::CreateHdiScreen(uint32_t screenId)
{
    return std::make_unique<HdiScreen>(screenId);
}

HdiScreen::HdiScreen(uint32_t screenId) : screenId_(screenId)
{
}

HdiScreen::~HdiScreen()
{
    Destory();
}

void HdiScreen::OnVsync(uint32_t sequence, uint64_t ns, void *data)
{
    // trigger vsync
    OHOS::VsyncError ret = OHOS::VsyncModule::GetInstance()->Trigger();
    if (ret != OHOS::VSYNC_ERROR_OK) {
        HLOGE("vsync trigger failed, ret is %{public}d", ret);
    }
}

bool HdiScreen::Init()
{
    if (device_ != nullptr) {
        HLOGI("HdiScreen has been initialized");
        return true;
    }

    device_ = HdiDevice::GetInstance();
    if (device_ == nullptr) {
        return false;
    }

    // start vsync
    OHOS::VsyncError vsyncRet = VsyncModule::GetInstance()->Start();
    if (vsyncRet != OHOS::VSYNC_ERROR_OK) {
        HLOGE("vsync start failed, ret is %{public}d", vsyncRet);
        return false;
    }

    int32_t ret = device_->RegScreenVBlankCallback(screenId_, HdiScreen::OnVsync, this);
    if (ret != DISPLAY_SUCCESS) {
        // Destory()
        HLOGE("RegScreenVBlankCallback failed, ret is %{public}d", ret);
        // return false
    }

    ret = device_->SetScreenVsyncEnabled(screenId_, true);
    if (ret != DISPLAY_SUCCESS) {
        Destory();
        HLOGE("SetScreenVsyncEnabled failed, ret is %{public}d", ret);
        return false;
    }

    return true;
}

int32_t HdiScreen::GetScreenCapability(DisplayCapability &dcap) const
{
    if (device_ == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device_->GetScreenCapability(screenId_, dcap);
}

int32_t HdiScreen::GetScreenSuppportedModes(std::vector<DisplayModeInfo> &modes) const
{
    if (device_ == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device_->GetScreenSuppportedModes(screenId_, modes);
}

int32_t HdiScreen::GetScreenMode(uint32_t &modeId) const
{
    if (device_ == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device_->GetScreenMode(screenId_, modeId);
}

int32_t HdiScreen::SetScreenMode(uint32_t modeId) const
{
    if (device_ == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device_->SetScreenMode(screenId_, modeId);
}

int32_t HdiScreen::GetScreenPowerStatus(DispPowerStatus &status) const
{
    if (device_ == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device_->GetScreenPowerStatus(screenId_, status);
}

int32_t HdiScreen::SetScreenPowerStatus(DispPowerStatus status) const
{
    if (device_ == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device_->SetScreenPowerStatus(screenId_, status);
}

int32_t HdiScreen::GetScreenBacklight(uint32_t &level) const
{
    if (device_ == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device_->GetScreenBacklight(screenId_, level);
}

int32_t HdiScreen::SetScreenBacklight(uint32_t level) const
{
    if (device_ == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return device_->SetScreenBacklight(screenId_, level);
}

void HdiScreen::Destory()
{
    // stop vsync
    OHOS::VsyncError ret = VsyncModule::GetInstance()->Stop();
    if (ret != OHOS::VSYNC_ERROR_OK) {
        HLOGE("vsync Stop failed, ret is %{public}d", ret);
    }
}

} // namespace Rosen
} // namespace OHOS
