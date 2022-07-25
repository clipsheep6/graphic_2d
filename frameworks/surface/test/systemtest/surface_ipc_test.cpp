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

    static inline sptr<Surface> cSurface = nullptr;
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
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);

    sptr<SurfaceBuffer> buffer = nullptr;
    int releaseFence = -1;
    auto sRet = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    if (sRet != OHOS::GSERROR_OK) {
        data = sRet;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }

    buffer->GetExtraData()->ExtraSet("123", 0x123);
    buffer->GetExtraData()->ExtraSet("345", (int64_t)0x345);
    buffer->GetExtraData()->ExtraSet("567", "567");

    ExtDataHandle *handle = new ExtDataHandle();
    handle->fd = -1;
    handle->reserveInts = 1;
    handle->reserve[0] = 1;
    sRet = pSurface->SetTunnelHandle(handle);
    if (sRet != OHOS::GSERROR_OK) {
        data = sRet;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }

    sRet = pSurface->FlushBuffer(buffer, -1, flushConfig);
    data = sRet;
    write(pipeFd[1], &data, sizeof(data));
    sleep(0);
    read(pipeFd[0], &data, sizeof(data));
    sleep(1);
    PresentTimestampType type = PresentTimestampType::HARDWARE_DISPLAY_PTS_DELAY;
    int64_t time = 0;
    sRet = pSurface->GetPresentTimestamp(buffer->GetSeqNum(), type, time);
    if (sRet != OHOS::GSERROR_OK || time != 1) {
        data = sRet;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }
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

    cSurface = Surface::CreateSurfaceAsConsumer("test");
    cSurface->RegisterConsumerListener(this);
    auto producer = cSurface->GetProducer();
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sam->AddSystemAbility(ipcSystemAbilityID, producer->AsObject());

    int64_t data = 0;
    write(pipeFd[1], &data, sizeof(data));
    sleep(0);
    read(pipeFd[0], &data, sizeof(data));
    EXPECT_EQ(data, OHOS::GSERROR_OK);

    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = -1;
    int64_t timestamp;
    Rect damage;
    auto sRet = cSurface->AcquireBuffer(buffer, fence, timestamp, damage);
    EXPECT_EQ(sRet, OHOS::GSERROR_OK);
    EXPECT_NE(buffer, nullptr);
    if (buffer != nullptr) {
        int32_t int32;
        int64_t int64;
        std::string str;
        buffer->GetExtraData()->ExtraGet("123", int32);
        buffer->GetExtraData()->ExtraGet("345", int64);
        buffer->GetExtraData()->ExtraGet("567", str);

        EXPECT_EQ(int32, 0x123);
        EXPECT_EQ(int64, 0x345);
        EXPECT_EQ(str, "567");
        sptr<SurfaceTunnelHandle> handleGet = nullptr;
        handleGet = cSurface->GetTunnelHandle();
        ASSERT_NE(handleGet, nullptr);
        ASSERT_EQ(handleGet->GetHandle()->fd, -1);
        ASSERT_EQ(handleGet->GetHandle()->reserveInts, 1);
        ASSERT_EQ(handleGet->GetHandle()->reserve[0], 1);

        PresentTimestamp timestamp = {HARDWARE_DISPLAY_PTS_DELAY, 1};  // mock data for test
        sRet = cSurface->SetPresentTimestamp(buffer->GetSeqNum(), timestamp);
        EXPECT_EQ(sRet, OHOS::GSERROR_OK);
    }

    sRet = cSurface->ReleaseBuffer(buffer, -1);
    EXPECT_EQ(sRet, OHOS::GSERROR_OK);

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
* Function: RequestBuffer and flush buffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer in this process, check sRet and buffer
 */
HWTEST_F(SurfaceIPCTest, Connect001, Function | MediumTest | Level2)
{
    cSurface->RegisterConsumerListener(this);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);

    sptr<SurfaceBuffer> buffer = nullptr;
    int releaseFence = -1;
    auto sRet = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(sRet, OHOS::GSERROR_INVALID_OPERATING);  // RequestBuffer cannot be called in two processes
    ASSERT_EQ(buffer, nullptr);
}

/*
* Function: disconnect
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call Disconnect in other process, check sRet
 */
HWTEST_F(SurfaceIPCTest, Disconnect001, Function | MediumTest | Level1)
{
    cSurface->RegisterConsumerListener(this);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    auto sRet = pSurface->Disconnect();
    ASSERT_EQ(sRet, OHOS::GSERROR_INVALID_OPERATING);  // Disconnect cannot be called in two processes
}
}
