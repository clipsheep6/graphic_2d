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

#include "platform/windows/vsync_receiver_windows.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSVSyncReceiver> RSVSyncReceiver::Create(
    const std::shared_ptr<RSEventHandler>& looper, const std::string& name)
{
    std::shared_ptr<VSyncReceiverWindows> windowsReceiver = std::make_shared<VSyncReceiverWindows>(looper, name);
    std::shared_ptr<RSVSyncReceiver> rsReceiver = std::static_pointer_cast<RSVSyncReceiver>(windowsReceiver);
    return rsReceiver;
}

VSyncReceiverWindows::VSyncReceiverWindows(const std::shared_ptr<RSEventHandler>& looper, const std::string& name)
    : RSVSyncReceiver(looper, name)
{}

VSyncReceiverWindows::~VSyncReceiverWindows() {}

VsyncError VSyncReceiverWindows::Init()
{
    return VSYNC_ERROR_OK;
}

VsyncError VSyncReceiverWindows::RequestNextVSync(FrameCallback callback)
{
    return VSYNC_ERROR_OK;
}

VsyncError VSyncReceiverWindows::SetVSyncRate(FrameCallback callback, int32_t rate)
{
    return VSYNC_ERROR_OK;
}
} // namespace Rosen
} // namespace OHOS
