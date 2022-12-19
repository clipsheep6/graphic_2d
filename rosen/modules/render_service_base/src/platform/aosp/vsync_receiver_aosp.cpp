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

#include "platform/aosp/vsync_receiver_aosp.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSVSyncReceiver> RSVSyncReceiver::Create(
    const std::shared_ptr<RSEventHandler>& looper,
    const std::string& name)
{
    std::shared_ptr<VSyncReceiverAosp> aospReceiver = std::make_shared<VSyncReceiverAosp>(looper, name);
    std::shared_ptr<RSVSyncReceiver> rsReceiver = std::static_pointer_cast<RSVSyncReceiver>(aospReceiver);
    return rsReceiver; 
}

VSyncReceiverAosp::VSyncReceiverAosp(
    const std::shared_ptr<RSEventHandler>& looper,
    const std::string& name) : RSVSyncReceiver(looper, name)
{}

VSyncReceiverAosp::~VSyncReceiverAosp()
{}

VsyncError VSyncReceiverAosp::Init()
{
    return VSYNC_ERROR_OK;
}

VsyncError VSyncReceiverAosp::RequestNextVSync(FrameCallback callback)
{
    return VSYNC_ERROR_OK;
}

VsyncError VSyncReceiverAosp::SetVSyncRate(FrameCallback callback, int32_t rate)
{
    return VSYNC_ERROR_OK;
}
} // namespace Rosen
} // namespace OHOS

