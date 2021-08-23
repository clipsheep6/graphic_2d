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

#include "buffer_queue_producer_remote_test.h"

#include "buffer_consumer_listener.h"

namespace OHOS {
void BufferQueueProducerRemoteTest::SetUpTestCase()
{
    bq = new BufferQueue("test");
    bqp = new BufferQueueProducer(bq);
    bq->Init();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    bq->RegisterConsumerListener(listener);

    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sm->AddSystemAbility(systemAbilityID, bqp);

    robj = sm->GetSystemAbility(systemAbilityID);
    bp = iface_cast<IBufferProducer>(robj);
}

void BufferQueueProducerRemoteTest::TearDownTestCase()
{
    bp = nullptr;
    robj = nullptr;

    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sm->RemoveSystemAbility(systemAbilityID);

    bqp = nullptr;
    bq = nullptr;
}

namespace {
HWTEST_F(BufferQueueProducerRemoteTest, IsProxy, testing::ext::TestSize.Level0)
{
    ASSERT_FALSE(robj->IsProxyObject());
}

HWTEST_F(BufferQueueProducerRemoteTest, QueueSize, testing::ext::TestSize.Level0)
{
    SurfaceError ret = bp->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ASSERT_EQ(bp->GetQueueSize(), 2u);
}

HWTEST_F(BufferQueueProducerRemoteTest, ReqCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    int32_t flushFence;
    int32_t sequence;

    SurfaceError ret = bp->RequestBuffer(sequence, buffer, releaseFence, requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueProducerRemoteTest, ReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    int32_t flushFence;
    int32_t sequence;

    SurfaceError ret = bp->RequestBuffer(sequence, buffer, releaseFence, requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(sequence, bedata);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueProducerRemoteTest, ReqReqReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer1;
    sptr<SurfaceBuffer> buffer2;
    sptr<SurfaceBuffer> buffer3;
    int32_t releaseFence;
    int32_t flushFence;
    int32_t sequence1;
    int32_t sequence2;
    int32_t sequence3;
    SurfaceError ret;

    ret = bp->RequestBuffer(sequence1, buffer1, releaseFence, requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(buffer1, nullptr);

    ret = bp->RequestBuffer(sequence2, buffer2, releaseFence, requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(buffer2, nullptr);

    ret = bp->RequestBuffer(sequence3, buffer3, releaseFence, requestConfig, deletingBuffers);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(buffer3, nullptr);

    ret = bp->CancelBuffer(sequence1, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(sequence2, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(sequence3, bedata);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(buffer1.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueProducerRemoteTest, SetQueueSizeDeleting, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(bp.GetRefPtr());
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 2u);

    SurfaceError ret = bp->SetQueueSize(1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);

    ret = bp->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);
}

HWTEST_F(BufferQueueProducerRemoteTest, ReqFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    int32_t flushFence;
    int32_t sequence;

    SurfaceError ret = bp->RequestBuffer(sequence, buffer, releaseFence, requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->FlushBuffer(sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueProducerRemoteTest, ReqFluFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    int32_t flushFence;
    int32_t sequence;

    SurfaceError ret = bp->RequestBuffer(sequence, buffer, releaseFence, requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->FlushBuffer(sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->FlushBuffer(sequence, bedata, -1, flushConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}
}
} // namespace OHOS
