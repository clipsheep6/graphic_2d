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

#include "platform/ohos/vsync_receiver_ohos.h"

#include "platform/ohos/event_handler_ohos.h"

namespace OHOS {
namespace Rosen {
VSyncReceiverOhos::VSyncReceiverOhos(
    const sptr<IVSyncConnection>& conn, const std::shared_ptr<RSEventHandler>& looper, const std::string& name)
    : RSVSyncReceiver(looper, name)
{
    auto ohosHandler = std::static_pointer_cast<EventHandlerOhos>(looper);
    receiver_ = std::make_shared<VSyncReceiver>(conn, ohosHandler->Get(), name);
}

VSyncReceiverOhos::~VSyncReceiverOhos() {}

VsyncError VSyncReceiverOhos::Init()
{
    if (receiver_ != nullptr) {
        return receiver_->Init();
    }
    return VSYNC_ERROR_NULLPTR;
}

VsyncError VSyncReceiverOhos::RequestNextVSync(FrameCallback callback)
{
    if (receiver_ != nullptr) {
        VSyncReceiver::FrameCallback tmpCallback;
        tmpCallback.userData_ = callback.userData_;
        tmpCallback.callback_ = callback.callback_;
        return receiver_->RequestNextVSync(tmpCallback);
    }
    return VSYNC_ERROR_NULLPTR;
}

VsyncError VSyncReceiverOhos::SetVSyncRate(FrameCallback callback, int32_t rate)
{
    if (receiver_ != nullptr) {
        VSyncReceiver::FrameCallback tmpCallback;
        tmpCallback.userData_ = callback.userData_;
        tmpCallback.callback_ = callback.callback_;
        return receiver_->SetVSyncRate(tmpCallback, rate);
    }
    return VSYNC_ERROR_NULLPTR;
}
} // namespace Rosen
} // namespace OHOS
