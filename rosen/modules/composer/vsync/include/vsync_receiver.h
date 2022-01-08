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


#ifndef VSYNC_VSYNC_RECEIVER_H
#define VSYNC_VSYNC_RECEIVER_H

#include <refbase.h>
#include "ivsync_connection.h"
#include "file_descriptor_listener.h"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <unistd.h>
namespace OHOS {
namespace Rosen {

class VSyncCallBackListener : public OHOS::AppExecFwk::FileDescriptorListener {
public:
    using VsyncCallback = std::function<void(int64_t)>;
    VSyncCallBackListener() : vsyncCallbacks_(nullptr)
    {

    }

    ~VSyncCallBackListener()
    {

    }
    void SetCallback(VsyncCallback cb) {
        vsyncCallbacks_ = cb;
    }

private:
    void OnReadable(int32_t fileDescriptor) override
    {
        if (fileDescriptor < 0) {
            return;
        }

        int64_t now;
        ssize_t retVal = read(fileDescriptor, &now, sizeof(int64_t));
        if (retVal > 0 && vsyncCallbacks_ != nullptr) {
            vsyncCallbacks_(now);
        }
    }
    VsyncCallback vsyncCallbacks_;
};

class VSyncReceiver : public RefBase {
public:
    // check
    using VsyncCallback = std::function<void(int64_t)>;

    VSyncReceiver(const sptr<IVSyncConnection>& conn, const std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper);
    ~VSyncReceiver();
    // nocopyable
    VSyncReceiver(const VSyncReceiver &) = delete;
    VSyncReceiver &operator=(const VSyncReceiver &) = delete;

    VsyncError Init();
    void RequestNextVSync(VSyncCallBackListener::VsyncCallback callback);

private:
    sptr<IVSyncConnection> connection_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper_;
    std::shared_ptr<VSyncCallBackListener> listener_;

    std::mutex initMutex_;
    bool init_;
    int32_t fd_;
};

} // namespace Rosen
} // namespace OHOS

#endif