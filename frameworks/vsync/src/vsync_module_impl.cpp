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


#include <chrono>
#include <mutex>
#include <unistd.h>
#include <xf86drm.h>

#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "vsync_log.h"
#include "vsync_module_impl.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncModuleImpl" };
constexpr int USLEEP_TIME = 100 * 1000;
constexpr int RETRY_TIMES = 5;
}

sptr<VsyncModuleImpl> VsyncModuleImpl::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new VsyncModuleImpl();
        }
    }
    return instance;
}

int64_t VsyncModuleImpl::WaitNextVBlank()
{
    drmVBlank vblank = {
        .request = drmVBlankReq {
            .type = DRM_VBLANK_RELATIVE,
            .sequence = 1,
        }
    };

    int ret = DrmModule::GetInstance()->DrmWaitBlank(drmFd_, vblank);
    if (ret != 0) {
        VLOG_ERROR_API(errno, drmWaitVBlank);
        return -1;
    }

    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

void VsyncModuleImpl::VsyncMainThread()
{
    while (vsyncThreadRunning_) {
        int64_t timestamp = WaitNextVBlank();
        if (timestamp < 0) {
            VLOGE("WaitNextVBlank return negative time");
            continue;
        }
        vsyncManager_->Callback(timestamp);
    }
}

bool VsyncModuleImpl::RegisterSystemAbility()
{
    if (isRegisterSA_) {
        return true;
    }
    auto sm = DrmModule::GetInstance()->GetSystemAbilityManager();
    if (sm) {
        sm->AddSystemAbility(vsyncSystemAbilityId_, vsyncManager_);
        isRegisterSA_ = true;
    }
    return isRegisterSA_;
}

void VsyncModuleImpl::UnregisterSystemAbility()
{
    auto sm = DrmModule::GetInstance()->GetSystemAbilityManager();
    if (sm) {
        sm->RemoveSystemAbility(vsyncSystemAbilityId_);
        isRegisterSA_ = false;
    }
}

VsyncModuleImpl::VsyncModuleImpl()
{
    drmFd_ = -1;
    vsyncThread_ = nullptr;
    vsyncThreadRunning_ = false;
    vsyncSystemAbilityId_ = 0;
    isRegisterSA_ = false;
    vsyncManager_ = new VsyncManager();
    sc = DrmModule::GetInstance();
}

VsyncModuleImpl::~VsyncModuleImpl()
{
    if (vsyncThreadRunning_) {
        Stop();
    }

    if (isRegisterSA_) {
        UnregisterSystemAbility();
    }
}

VsyncError VsyncModuleImpl::Start()
{
    VsyncError ret = InitSA();
    if (ret != VSYNC_ERROR_OK) {
        return ret;
    }

    if (isSetDrmFd_ == false) {
        for (const auto &name : DrmModuleNames) {
            drmFd_ = DrmModule::GetInstance()->DrmOpen(name, "");
            if (drmFd_ < 0) {
                VLOGW("drmOpen %{public}s failed with %{public}d", name, errno);
            } else {
                break;
            }
        }
        if (drmFd_ < 0) {
            return VSYNC_ERROR_API_FAILED;
        }
    } else {
        VLOGD("SetDrmFd fd is %{public}d", drmFd_);
    }

    vsyncThreadRunning_ = true;
    vsyncThread_ = std::make_unique<std::thread>([this]()->void {
        VsyncMainThread();
    });

    return VSYNC_ERROR_OK;
}

VsyncError VsyncModuleImpl::Stop()
{
    if (isRegisterSA_) {
        UnregisterSystemAbility();
    }

    if (!vsyncThreadRunning_) {
        return VSYNC_ERROR_INVALID_OPERATING;
    }

    vsyncThreadRunning_ = false;
    vsyncThread_->join();
    vsyncThread_.reset();

    if (isSetDrmFd_ == false) {
        int ret = DrmModule::GetInstance()->DrmClose(drmFd_);
        if (ret) {
            VLOG_ERROR_API(errno, drmClose);
            return VSYNC_ERROR_API_FAILED;
        }
        drmFd_ = -1;
    }

    return VSYNC_ERROR_OK;
}

void VsyncModuleImpl::SetDrmFd(int32_t Fd)
{
    drmFd_ = Fd;
    isSetDrmFd_ = true;
}

VsyncError VsyncModuleImpl::InitSA()
{
    return InitSA(VSYNC_MANAGER_ID);
}

VsyncError VsyncModuleImpl::InitSA(int32_t vsyncSystemAbilityId)
{
    vsyncSystemAbilityId_ = vsyncSystemAbilityId;

    int tryCount = 0;
    while (!RegisterSystemAbility()) {
        if (tryCount++ >= RETRY_TIMES) {
            VLOGE("RegisterSystemAbility failed after %{public}d tries!!!", RETRY_TIMES);
            return VSYNC_ERROR_SERVICE_NOT_FOUND;
        } else {
            VLOGE("RegisterSystemAbility failed, try again:%{public}d", tryCount);
            usleep(USLEEP_TIME);
        }
    }

    return VSYNC_ERROR_OK;
}
} // namespace OHOS
