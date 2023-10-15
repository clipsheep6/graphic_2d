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


#ifndef VSYNC_VSYNC_DISTRIBUTOR_H
#define VSYNC_VSYNC_DISTRIBUTOR_H

#include <refbase.h>

#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>

#include "local_socketpair.h"
#include "vsync_controller.h"
#include "vsync_connection_stub.h"

namespace OHOS {
namespace Rosen {
class VSyncDistributor;
struct ConnectionInfo {
    std::string name_;
    uint64_t postVSyncCount_;
    ConnectionInfo(std::string name): postVSyncCount_(0)
    {
        this->name_ = name;
    }
};

class VSyncConnection : public VSyncConnectionStub {
public:

    VSyncConnection(const sptr<VSyncDistributor>& distributor, std::string name,
                    const sptr<IRemoteObject>& token = nullptr);
    ~VSyncConnection();

    virtual VsyncError RequestNextVSync() override;
    virtual VsyncError GetReceiveFd(int32_t &fd) override;
    virtual VsyncError SetVSyncRate(int32_t rate) override;
    virtual VsyncError GetVSyncPeriod(int64_t &period) override;
    virtual VsyncError Destroy() override;

    int32_t PostEvent(int64_t now, int64_t period, int64_t vsyncCount);

    int32_t rate_;
    int32_t highPriorityRate_ = -1;
    bool highPriorityState_ = false;
    ConnectionInfo info_;
    int32_t proxyPid_;
private:
    VsyncError CleanAllLocked();
    class VSyncConnectionDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit VSyncConnectionDeathRecipient(wptr<VSyncConnection> conn);
        virtual ~VSyncConnectionDeathRecipient() = default;

        void OnRemoteDied(const wptr<IRemoteObject>& token) override;

    private:
        wptr<VSyncConnection> conn_;
    };
    sptr<VSyncConnectionDeathRecipient> vsyncConnDeathRecipient_ = nullptr;
    sptr<IRemoteObject> token_ = nullptr;
    // Circular reference， need check
    wptr<VSyncDistributor> distributor_;
    sptr<LocalSocketPair> socketPair_;
    bool isDead_;
    std::mutex mutex_;
};

class VSyncDistributor : public RefBase, public VSyncController::Callback {
public:

    VSyncDistributor(sptr<VSyncController> controller, std::string name);
    ~VSyncDistributor();
    // nocopyable
    VSyncDistributor(const VSyncDistributor &) = delete;
    VSyncDistributor &operator=(const VSyncDistributor &) = delete;

    VsyncError AddConnection(const sptr<VSyncConnection>& connection);
    VsyncError RemoveConnection(const sptr<VSyncConnection> &connection);
    VsyncError RequestNextVSync(const sptr<VSyncConnection>& connection);
    VsyncError SetVSyncRate(int32_t rate, const sptr<VSyncConnection>& connection);
    VsyncError SetHighPriorityVSyncRate(int32_t highPriorityRate, const sptr<VSyncConnection>& connection);
    VsyncError GetVSyncConnectionInfos(std::vector<ConnectionInfo>& infos);
    VsyncError GetVSyncPeriod(int64_t &period);

private:

    // check, add more info
    struct VSyncEvent {
        int64_t timestamp;
        int64_t vsyncCount;
        int64_t period;
    };
    void ThreadMain();
    void EnableVSync();
    void DisableVSync();
    void OnVSyncEvent(int64_t now, int64_t period);
    void CollectConnections(bool &waitForVSync, int64_t timestamp,
                            std::vector<sptr<VSyncConnection>> &conns, int64_t vsyncCount);
    void PostVSyncEvent(const std::vector<sptr<VSyncConnection>> &conns, int64_t timestamp);

    std::thread threadLoop_;
    sptr<VSyncController> controller_;
    std::mutex mutex_;
    std::condition_variable con_;
    std::vector<sptr<VSyncConnection> > connections_;
    VSyncEvent event_;
    bool vsyncEnabled_;
    std::string name_;
    bool vsyncThreadRunning_;
    std::unordered_map<int32_t, int32_t> connectionCounter_;
};
} // namespace Rosen
} // namespace OHOS

#endif
