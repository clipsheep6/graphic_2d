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
#include <thread>
#include <unistd.h>
#include <condition_variable>
#include <gtest/gtest.h>
#include <iservice_registry.h>
#include "vsync_receiver.h"
#include "vsync_controller.h"
#include "vsync_sampler.h"
#include "vsync_generator.h"
#include "vsync_distributor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
int32_t appVSyncFlag = 0;
int32_t rsVSyncFlag = 0;
constexpr int32_t SOFT_VSYNC_PERIOD = 16;
static void OnVSyncApp(int64_t time, void *data)
{
    appVSyncFlag = 1;
}

static void OnVSyncRS(int64_t time, void *data)
{
    rsVSyncFlag = 1;
}
}
class VSyncTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    pid_t ChildProcessMain();
    static void ThreadMain();

    static inline sptr<VSyncController> appController = nullptr;
    static inline sptr<VSyncController> rsController = nullptr;
    static inline sptr<VSyncDistributor> appDistributor = nullptr;
    static inline sptr<VSyncDistributor> rsDistributor = nullptr;
    static inline sptr<VSyncGenerator> vsyncGenerator = nullptr;
    static inline sptr<VSyncSampler> vsyncSampler = nullptr;
    static inline std::thread samplerThread;

    static inline int32_t pipeFd[2] = {};
    static inline int32_t ipcSystemAbilityID = 34156;
};

void VSyncTest::SetUpTestCase()
{
    vsyncGenerator = CreateVSyncGenerator();
    vsyncSampler = CreateVSyncSampler();
    appController = new VSyncController(vsyncGenerator, 0);
    rsController = new VSyncController(vsyncGenerator, 0);

    appDistributor = new VSyncDistributor(appController, "appController");
    rsDistributor = new VSyncDistributor(rsController, "rsController");

    samplerThread = std::thread(std::bind(&VSyncTest::ThreadMain));
}

void VSyncTest::TearDownTestCase()
{
    vsyncGenerator = nullptr;
    vsyncSampler = nullptr;
    appDistributor = nullptr;
    rsDistributor = nullptr;
    appDistributor = nullptr;
    rsDistributor = nullptr;

    if (samplerThread.joinable()) {
        samplerThread.join();
    }
}

pid_t VSyncTest::ChildProcessMain()
{
    pipe(pipeFd);
    pid_t pid = fork();
    if (pid != 0) {
        return pid;
    }

    int64_t data;
    read(pipeFd[0], &data, sizeof(data));

    sptr<IRemoteObject> robj = nullptr;
    while (true) {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        robj = sam->GetSystemAbility(ipcSystemAbilityID);
        if (robj != nullptr) {
            break;
        }
        sleep(0);
    }

    auto conn = iface_cast<IVSyncConnection>(robj);
    sptr<VSyncReceiver> receiver = new VSyncReceiver(conn);
    receiver->Init();

    VSyncReceiver::FrameCallback fcb = {
        .userData_ = nullptr,
        .callback_ = OnVSyncApp,
    };

    receiver->RequestNextVSync(fcb);
    while (appVSyncFlag == 0) {}
    EXPECT_EQ(appVSyncFlag, 1);
    close(pipeFd[0]);
    close(pipeFd[1]);
    exit(0);
    return 0;
}

void VSyncTest::ThreadMain()
{
    bool ret = true;
    std::condition_variable con;
    std::mutex mtx;
    std::unique_lock<std::mutex> locker(mtx);
    while (ret) {
        const auto &now = std::chrono::steady_clock::now().time_since_epoch();
        int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
        ret = vsyncSampler->AddSample(timestamp);
        con.wait_for(locker, std::chrono::milliseconds(SOFT_VSYNC_PERIOD));
    }
}

/*
* Function: RequestNextVSync001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. RequestNextVSync by IPC
 */
HWTEST_F(VSyncTest, RequestNextVSync001, Function | MediumTest | Level2)
{
    auto pid = ChildProcessMain();
    ASSERT_GE(pid, 0);

    sptr<VSyncConnection> connServer = new VSyncConnection(appDistributor, "app");
    appDistributor->AddConnection(connServer);
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sam->AddSystemAbility(ipcSystemAbilityID, connServer->AsObject());
    while (appVSyncFlag == 0) {}
    close(pipeFd[0]);
    close(pipeFd[1]);
    sam->RemoveSystemAbility(ipcSystemAbilityID);
    waitpid(pid, nullptr, NULL);
}

/*
* Function: RequestNextVSync002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. RequestNextVSync by local
 */
HWTEST_F(VSyncTest, RequestNextVSync002, Function | MediumTest | Level2)
{
    sptr<VSyncConnection> connRS = new VSyncConnection(rsDistributor, "rs");
    sptr<VSyncReceiver> receiver = new VSyncReceiver(connRS);
    receiver->Init();

    VSyncReceiver::FrameCallback fcb = {
        .userData_ = nullptr,
        .callback_ = OnVSyncRS,
    };

    receiver->RequestNextVSync(fcb);
    while (rsVSyncFlag == 0) {}
    EXPECT_EQ(rsVSyncFlag, 1);
}
}
