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


#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_VSYNC_RECEIVER_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_VSYNC_RECEIVER_H

#include <memory>
#include <string>

#include "graphic_common.h"
#include "rs_event_handler.h"

namespace OHOS {
namespace Rosen {
class RSVSyncReceiver {
public:
    using VSyncCallback = std::function<void(int64_t, void*)>;
    struct FrameCallback {
        void *userData_;
        VSyncCallback callback_;
    };
    RSVSyncReceiver(
        const std::shared_ptr<RSEventHandler>& looper = nullptr,
        const std::string& name = "Uninitialized") {}
    virtual ~RSVSyncReceiver() {}

    static std::shared_ptr<RSVSyncReceiver> Create(
        const std::shared_ptr<RSEventHandler>& looper = nullptr,
        const std::string& name = "Uninitialized");

    virtual VsyncError Init() = 0;
    virtual VsyncError RequestNextVSync(FrameCallback callback) = 0;
    virtual VsyncError SetVSyncRate(FrameCallback callback, int32_t rate) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_CORE_COMMON_RS_VSYNC_RECEIVER_H
