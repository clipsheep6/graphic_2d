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
#include <gtest/gtest.h>
#include <iservice_registry.h>
#include <surface.h>
#include <display_type.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class SurfaceIPCTest : public testing::Test, public IBufferConsumerListenerClazz {
public:
    static void SetUpTestCase();
    virtual void OnBufferAvailable() override;
    pid_t ChildProcessMain();

    static inline sptr<Surface> csurface = nullptr;
    static inline int32_t pipeFd[2] = {};
    static inline int32_t ipcSystemAbilityID = 34156;
    static inline BufferRequestConfig requestConfig = {};
    static inline BufferFlushConfig flushConfig = {};
};

void SurfaceIPCTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid();
    requestConfig = {
        .width = 0x100,  // small
        .height = 0x100, // small
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    flushConfig = { .damage = {
        .w = 0x100,
        .h = 0x100,
    } };
}

void SurfaceIPCTest::OnBufferAvailable()
{
}

pid_t SurfaceIPCTest::ChildProcessMain()
{
    pipe(pipeFd);
    pid_t pid = fork();
    if (pid != 0) {
        return pid;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

    auto producer = iface_cast<IBufferProducer>(robj);
    auto psurf = Surface::CreateSurfaceAsProducer(producer);

    sptr<SurfaceBuffer> buffer = nullptr;
    auto sret = psurf->RequestBufferNoFence(buffer, requestConfig);
    if (sret != OHOS::GSERROR_OK) {
        data = sret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }

    buffer->ExtraSet("123", 0x123);
    buffer->ExtraSet("345", (int64_t)0x345);
    buffer->ExtraSet("567", "567");

    sret = psurf->FlushBuffer(buffer, -1, flushConfig);
    data = sret;
    write(pipeFd[1], &data, sizeof(data));
    sleep(0);
    read(pipeFd[0], &data, sizeof(data));
    close(pipeFd[0]);
    close(pipeFd[1]);
    exit(0);
    return 0;
}

/*
* Function: produce and consumer surface by IPC
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. produce surface, fill buffer
*                  2. consume surface and check buffer
 */
HWTEST_F(SurfaceIPCTest, BufferIPC001, Function | MediumTest | Level2)
{
    auto pid = ChildProcessMain();
    ASSERT_GE(pid, 0);

    csurf = Surface::CreateSurfaceAsConsumer("test");
    csurf->RegisterConsumerListener(this);
    auto producer = csurf->GetProducer();
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sam->AddSystemAbility(ipcSystemAbilityID, producer->AsObject());

    int64_t data = 0;
    write(pipeFd[1], &data, sizeof(data));
    sleep(0);
    read(pipeFd[0], &data, sizeof(data));
    EXPECT_EQ(data, OHOS::GSERROR_OK);

    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence;
    int64_t timestamp;
    Rect damage;
    auto sret = csurf->AcquireBuffer(buffer, fence, timestamp, damage);
    EXPECT_EQ(sret, OHOS::GSERROR_OK);
    EXPECT_NE(buffer, nullptr);
    if (buffer != nullptr) {
        int32_t int32;
        int64_t int64;
        std::string str;
        buffer->ExtraGet("123", int32);
        buffer->ExtraGet("345", int64);
        buffer->ExtraGet("567", str);

        EXPECT_EQ(int32, 0x123);
        EXPECT_EQ(int64, 0x345);
        EXPECT_EQ(str, "567");
    }

    sret = csurf->ReleaseBuffer(buffer, -1);
    EXPECT_EQ(sret, OHOS::GSERROR_OK);

    write(pipeFd[1], &data, sizeof(data));
    close(pipeFd[0]);
    close(pipeFd[1]);
    sam->RemoveSystemAbility(ipcSystemAbilityID);
    int32_t ret = 0;
    do {
        waitpid(pid, nullptr, 0);
    } while (ret == -1 && errno == EINTR);
}

/*
* Function: RequestBufferNoFence and flush buffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBufferNoFence, check sret and buffer
*                  2. call flushbuffer and check sret
 */
HWTEST_F(SurfaceIPCTest, Cache001, Function | MediumTest | Level2)
{
    csurf->RegisterConsumerListener(this);
    auto producer = csurf->GetProducer();
    auto psurf = Surface::CreateSurfaceAsProducer(producer);

    sptr<SurfaceBuffer> buffer = nullptr;
    auto sret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(sret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);
    int32_t int32;
    int64_t int64;
    std::string str;
    buffer->ExtraGet("123", int32);
    buffer->ExtraGet("345", int64);
    buffer->ExtraGet("567", str);

    ASSERT_EQ(int32, 0x123);
    ASSERT_EQ(int64, 0x345);
    ASSERT_EQ(str, "567");

    sret = psurf->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(sret, OHOS::GSERROR_OK);
}
}
