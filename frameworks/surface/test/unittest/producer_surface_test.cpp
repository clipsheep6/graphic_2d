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
#include <securec.h>
#include <gtest/gtest.h>
#include <display_type.h>
#include <surface.h>
#include "buffer_consumer_listener.h"
#include <consumer_surface.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class ProducerSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = {
            .w = 0x100,
            .h = 0x100,
        },
    };
    static inline int64_t timestamp = 0;
    static inline Rect damage = {};
    static inline sptr<Surface> csurface = nullptr;
    static inline sptr<IBufferProducer> producer = nullptr;
    static inline sptr<Surface> psurface = nullptr;
};

void ProducerSurfaceTest::SetUpTestCase()
{
    csurface = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    csurface->RegisterConsumerListener(listener);
    producer = csurface->GetProducer();
    psurface = Surface::CreateSurfaceAsProducer(producer);
}

void ProducerSurfaceTest::TearDownTestCase()
{
    csurface = nullptr;
    producer = nullptr;
    psurface = nullptr;
}

/**
 * @tc.name: ProducerSurface001
 * @tc.desc: Verify the CreateSurfaceAsProducer of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, ProducerSurface001, Function | MediumTest| Level1)
{
    ASSERT_NE(psurface, nullptr);
}

/**
 * @tc.name: QueueSize001
 * @tc.desc: Verify the SetQueueSize and GetQueueSize of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, QueueSize001, Function | MediumTest| Level3)
{
    ASSERT_EQ(psurface->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);
    GSError ret = psurface->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(psurface->GetQueueSize(), 2u);
}

/**
 * @tc.name: QueueSize002
 * @tc.desc: Verify the SetQueueSize and GetQueueSize of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, QueueSize002, Function | MediumTest| Level3)
{
    sptr<ConsumerSurface> cs = static_cast<ConsumerSurface*>(csurface.GetRefPtr());
    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(cs->GetProducer().GetRefPtr());
    ASSERT_EQ(bqp->GetQueueSize(), 2u);

    GSError ret = psurface->SetQueueSize(1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel001
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel001, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;

    GSError ret = psurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = psurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel002
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel002, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;

    GSError ret = psurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = psurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel003
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel003, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    GSError ret = psurface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = psurface->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = csurface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = csurface->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = csurface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = csurface->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel004
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel004, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence;

    GSError ret = psurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel005
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel005, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence;

    GSError ret = psurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->CancelBuffer(buffer);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel006
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel006, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    sptr<SurfaceBuffer> buffer1;
    sptr<SurfaceBuffer> buffer2;
    int releaseFence;

    GSError ret = psurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->RequestBuffer(buffer1, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->RequestBuffer(buffer2, releaseFence, requestConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = psurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->CancelBuffer(buffer1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->CancelBuffer(buffer2);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel007
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel007, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence;

    GSError ret = psurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurface->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = psurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: UserData001
 * @tc.desc: Verify the UserData of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, UserData001, Function | MediumTest| Level3)
{
    GSError ret;

    std::string strs[SURFACE_MAX_USER_DATA_COUNT];
    constexpr int32_t stringLengthMax = 32;
    char str[stringLengthMax] = {};
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        auto secRet = snprintf_s(str, sizeof(str), sizeof(str) - 1, "%d", i);
        ASSERT_GT(secRet, 0);

        strs[i] = str;
        ret = psurface->SetUserData(strs[i], "magic");
        ASSERT_EQ(ret, OHOS::GSERROR_OK);
    }

    ret = psurface->SetUserData("-1", "error");
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    std::string retStr;
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        retStr = psurface->GetUserData(strs[i]);
        ASSERT_EQ(retStr, "magic");
    }
}

/**
 * @tc.name: RegisterConsumerListener001
 * @tc.desc: Verify the RegisterConsumerListener of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, RegisterConsumerListener001, Function | MediumTest| Level3)
{
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    GSError ret = psurface->RegisterConsumerListener(listener);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: UniqueId001
 * @tc.desc: Verify the GetUnique of ProducerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ProducerSurfaceTest, UniqueId001, Function | MediumTest| Level3)
{
    uint64_t uniqueId = psurface->GetUniqueId();
    ASSERT_NE(uniqueId, 0);
}
}
