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
#include <buffer_queue_producer.h>
#include <consumer_surface.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class ConsumerSurfaceTest : public testing::Test {
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
    static inline sptr<Surface> cs = nullptr;
    static inline sptr<Surface> ps = nullptr;
};

void ConsumerSurfaceTest::SetUpTestCase()
{
    cs = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cs->RegisterConsumerListener(listener);
    auto p = cs->GetProducer();
    ps = Surface::CreateSurfaceAsProducer(p);
}

void ConsumerSurfaceTest::TearDownTestCase()
{
    cs = nullptr;
}

/**
 * @tc.name: ConsumerSurface001
 * @tc.desc: Verify the ConsumerSurface of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ConsumerSurfaceTest, ConsumerSurface001, Function | MediumTest| Level1)
{
    ASSERT_NE(cs, nullptr);

    sptr<ConsumerSurface> qs = static_cast<ConsumerSurface*>(cs.GetRefPtr());
    ASSERT_NE(qs, nullptr);
    ASSERT_NE(qs->GetProducer(), nullptr);
}

/**
 * @tc.name: QueueSize001
 * @tc.desc: Verify the SetQueueSize and GetQueueSize of ConsumerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ConsumerSurfaceTest, QueueSize001, Function | MediumTest| Level3)
{
    ASSERT_EQ(cs->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);
    GSError ret = cs->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = cs->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(cs->GetQueueSize(), 2u);
}

/**
 * @tc.name: QueueSize002
 * @tc.desc: Verify the SetQueueSize and GetQueueSize of ConsumerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ConsumerSurfaceTest, QueueSize002, Function | MediumTest| Level3)
{
    sptr<ConsumerSurface> qs = static_cast<ConsumerSurface*>(cs.GetRefPtr());
    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(qs->GetProducer().GetRefPtr());
    ASSERT_EQ(bqp->GetQueueSize(), 2u);

    GSError ret = cs->SetQueueSize(1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = cs->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel001
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ConsumerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ConsumerSurfaceTest, ReqCanFluAcqRel001, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;

    GSError ret = cs->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = cs->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel002
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ConsumerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ConsumerSurfaceTest, ReqCanFluAcqRel002, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    GSError ret = cs->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = cs->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel002
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ConsumerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ConsumerSurfaceTest, ReqCanFluAcqRel003, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    GSError ret = cs->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = cs->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = cs->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: ReqCanFluAcqRel004
 * @tc.desc: Verify the RequestBuffer, CancelBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer of ConsumerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ConsumerSurfaceTest, ReqCanFluAcqRel004, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence;

    GSError ret = cs->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = cs->CancelBuffer(buffer);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = ps->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: UserData001
 * @tc.desc: Verify the SetUserData of ConsumerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ConsumerSurfaceTest, UserData001, Function | MediumTest| Level3)
{
    GSError ret;

    std::string strs[SURFACE_MAX_USER_DATA_COUNT];
    constexpr int32_t stringLengthMax = 32;
    char str[stringLengthMax] = {};
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        auto secRet = snprintf_s(str, sizeof(str), sizeof(str) - 1, "%d", i);
        ASSERT_GT(secRet, 0);

        strs[i] = str;
        ret = cs->SetUserData(strs[i], "magic");
        ASSERT_EQ(ret, OHOS::GSERROR_OK);
    }

    ret = cs->SetUserData("-1", "error");
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    std::string retStr;
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        retStr = cs->GetUserData(strs[i]);
        ASSERT_EQ(retStr, "magic");
    }
}

/**
 * @tc.name: RegisterConsumerListener001
 * @tc.desc: Verify the RegisterConsumerListener of ConsumerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ConsumerSurfaceTest, RegisterConsumerListener001, Function | MediumTest| Level3)
{
    class TestConsumerListener : public IBufferConsumerListener {
    public:
        void OnBufferAvailable() override
        {
            sptr<SurfaceBuffer> buffer;
            int32_t flushFence;

            cs->AcquireBuffer(buffer, flushFence, timestamp, damage);
            int32_t *p = (int32_t*)buffer->GetVirAddr();
            if (p != nullptr) {
                for (int32_t i = 0; i < 128; i++) {
                    ASSERT_EQ(p[i], i);
                }
            }

            cs->ReleaseBuffer(buffer, -1);
        }
    };
    sptr<IBufferConsumerListener> listener = new TestConsumerListener();
    GSError ret = cs->RegisterConsumerListener(listener);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SurfaceBuffer> buffer;
    int releaseFence;

    ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    int32_t *p = (int32_t*)buffer->GetVirAddr();
    if (p != nullptr) {
        for (int32_t i = 0; i < 128; i++) {
            p[i] = i;
        }
    }

    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: RegisterConsumerListener002
 * @tc.desc: Verify the RegisterConsumerListener of ConsumerSurface
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(ConsumerSurfaceTest, RegisterConsumerListener002, Function | MediumTest| Level3)
{
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    GSError ret = cs->RegisterConsumerListener(listener);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SurfaceBuffer> buffer;
    int releaseFence;

    ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    int32_t *p = (int32_t*)buffer->GetVirAddr();
    if (p != nullptr) {
        for (int32_t i = 0; i < requestConfig.width * requestConfig.height; i++) {
            p[i] = i;
        }
    }

    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}
}
