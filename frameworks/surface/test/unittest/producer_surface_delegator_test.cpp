/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <securec.h>
#include <gtest/gtest.h>
#include <surface.h>
#include <consumer_surface.h>
#include <native_window.h>
#include "buffer_consumer_listener.h"
#include "sync_fence.h"
#include <message_option.h>
#include <message_parcel.h>
#include "transact_surface_delegator_stub.h"
#include "buffer_log.h"
#include "producer_surface_delegator.h"
#include "buffer_queue_producer.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class ProducerSurfaceDelegatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<IConsumerSurface> csurf = nullptr;
    static inline sptr<ProducerSurfaceDelegator> qwe = nullptr;
    static inline sptr<IBufferProducer> producer = nullptr;
    static inline sptr<Surface> pSurface = nullptr;
    static inline sptr<SurfaceBuffer> pBuffer = nullptr;
    static inline sptr<SurfaceBuffer> cBuffer = nullptr;
};

class IRemoteObjectMocker : public IRemoteObject {
public:
    IRemoteObjectMocker() : IRemoteObject{u"IRemoteObjectMocker"} {}
    ~IRemoteObjectMocker() {}
    int32_t GetObjectRefCount() { return 0; }

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        return 0;
    }

    bool IsProxyObject() const
    {
        return true;
    }

    bool CheckObjectLegality() const{
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface()
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string> &args)
    {
        return 0;
    }
};

void ProducerSurfaceDelegatorTest::SetUpTestCase()
{
    csurf = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    csurf->RegisterConsumerListener(listener);
    producer = csurf->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    qwe = new ProducerSurfaceDelegator();
    sptr<IRemoteObjectMocker> remoteObjectMocker = new IRemoteObjectMocker();
    qwe->SetClient(remoteObjectMocker);
    pBuffer = SurfaceBuffer::Create();
}

void ProducerSurfaceDelegatorTest::TearDownTestCase()
{
    pSurface->UnRegisterReleaseListener();
    qwe = nullptr;
    csurf = nullptr;
    producer = nullptr;
    pSurface = nullptr;
    pBuffer = nullptr;
}

/*
* Function: DequeueBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call DequeueBuffer
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceDelegatorTest, DequeueBuffer001, Function | MediumTest | Level2)
{
    int32_t slot = 1;
    qwe->SetSurface(nullptr);
    GSError ret = qwe->DequeueBuffer(slot, pBuffer);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: QueueBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call QueueBuffer
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceDelegatorTest, QueueBuffer001, Function | MediumTest | Level2)
{
    int32_t slot = 1;
    int32_t acquireFence = 3;
    GSError ret = qwe->QueueBuffer(slot, acquireFence);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: QueueBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call QueueBuffer
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceDelegatorTest, QueueBuffer002, Function | MediumTest | Level2)
{
    int32_t slot = 1;
    int32_t acquireFence = 3;
    sptr<Surface> aSurface = Surface::CreateSurfaceAsProducer(producer);
    qwe->SetSurface(aSurface);
    GSError ret = qwe->QueueBuffer(slot, acquireFence);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: DequeueBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call DequeueBuffer
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceDelegatorTest, DequeueBuffer002, Function | MediumTest | Level2)
{
    int32_t slot = 1;
    qwe->SetSurface(pSurface);
    GSError ret = qwe->DequeueBuffer(slot, pBuffer);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = qwe->DequeueBuffer(slot, cBuffer);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: QueueBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call QueueBuffer
*                  2. check SendMessage ret
 */
HWTEST_F(ProducerSurfaceDelegatorTest, QueueBuffer003, Function | MediumTest | Level2)
{
    int32_t slot = 1;
    int32_t acquireFence = 3;
    qwe->SetSurface(pSurface);
    GSError ret = qwe->QueueBuffer(slot, acquireFence);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseBuffer
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceDelegatorTest, ReleaseBuffer001, Function | MediumTest | Level2)
{
    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    GSError ret = qwe->ReleaseBuffer(pBuffer, fence);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseBuffer
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceDelegatorTest, ReleaseBuffer002, Function | MediumTest | Level2)
{
    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    GSError ret = qwe->ReleaseBuffer(nullptr, fence);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OnRemoteRequest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OnRemoteRequest
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceDelegatorTest, OnRemoteRequest001, Function | MediumTest | Level2)
{
    uint32_t code = 0; // DEQUEUEBUFFER
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(1);
    data.WriteInt32(2);
    int ret = qwe->OnRemoteRequest(Code, data, reply, option);
    ASSERT_NE(ret, ERR_NULL_OBJECT);

    code = 1; // QUEUEBUFFER
    MessageParcel dataQueue;
    dataQueue.WriteInt32(10);
    dataQueue.WriteFileDescriptor(20);
    GSError ret = qwe->OnRemoteRequest(code, dataQueue, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}
} // namespace OHOS::Rosen