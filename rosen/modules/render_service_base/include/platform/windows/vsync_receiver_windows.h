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

#ifndef WINDOWS_VSYNC_RECEIVER_WINDOWS_H
#define WINDOWS_VSYNC_RECEIVER_WINDOWS_H

#include <functional>
#include <memory>
#include <string>

#include "platform/common/rs_vsync_receiver.h"

namespace OHOS {
namespace Rosen {
class VSyncReceiverWindows : public RSVSyncReceiver {
public:
    VSyncReceiverWindows(
        const std::shared_ptr<RSEventHandler>& looper = nullptr,
        const std::string& name = "Uninitialized");
    virtual ~VSyncReceiverWindows();

    VsyncError Init() override;
    VsyncError RequestNextVSync(FrameCallback callback) override;
    VsyncError SetVSyncRate(FrameCallback callback, int32_t rate) override;
};
} // namespace Rosen
} // namespace OHOS

#endif // WINDOWS_VSYNC_RECEIVER_WINDOWS_H
