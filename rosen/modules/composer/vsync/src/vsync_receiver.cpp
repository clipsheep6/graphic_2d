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

#include "vsync_receiver.h"
#include <hilog/log.h>
#include "event_handler.h"
#include "graphic_common.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr int32_t INVALID_FD = -1;
}

VSyncReceiver::VSyncReceiver(const sptr<IVSyncConnection>& conn, const std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper)
        : connection_(conn), looper_(looper), listener_(std::make_shared<VSyncCallBackListener>()), init_(false), fd_(INVALID_FD)
{
};

VsyncError VSyncReceiver::Init()
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (init_) {
        return VSYNC_ERROR_OK;
    }

    fd_ = connection_->GetReceiveFd();
    if (fd_ == INVALID_FD) {
        return VSYNC_ERROR_INNER;
    }

    looper_->AddFileDescriptorListener(fd_, OHOS::AppExecFwk::FILE_DESCRIPTOR_INPUT_EVENT, listener_);
    init_ = true;
    return VSYNC_ERROR_OK;
}

VSyncReceiver::~VSyncReceiver()
{
    if (fd_ != INVALID_FD) {
        looper_->RemoveFileDescriptorListener(fd_);
        close(fd_);
        fd_ = INVALID_FD;
    }
}

void VSyncReceiver::RequestNextVSync(VSyncCallBackListener::VsyncCallback callback)
{
    listener_->SetCallback(callback);
    connection_->RequestNextVSync();
}
} // namespace Rosen
} // namespace OHOS
