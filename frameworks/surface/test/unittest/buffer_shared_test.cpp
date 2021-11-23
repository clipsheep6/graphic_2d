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

#include "buffer_shared_test.h"

#include "buffer_consumer_listener.h"
#include "buffer_manager.h"
#include "buffer_log.h"
#include "test_header.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
void BufferSharedTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid() << std::endl;
    surface = Surface::CreateSurfaceAsShared("shared", true);
    listener_1 = new BufferConsumerListener();
    surface->RegisterConsumerListener(listener_1);

    producer_1 = surface->GetProducer();
    producerSurface_1 = Surface::CreateSurfaceAsProducer(producer_1);
    producer_2 = surface->GetProducer();
    producerSurface_2 = Surface::CreateSurfaceAsProducer(producer_2);
}

void BufferSharedTest::TearDownTestCase()
{

}

namespace {
    /*
 * Function: RequestBuffer
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call RequestBufferNoFence with buffer=buffer_1, buffer_2，the param is same
 *                  2. check ret1 and ret2 are SURFACE_ERROR_OK, check buffer_1 and buffer_2 is not nullptr
 *                  3. check the addr of buffer_1 EQ buffer_2
 * */

HWTEST_F(BufferSharedTest, RequestBuffer, testing::ext::TestSize.Level0)
{
    PART("REQUEST BUFFER TWO TIMES"){
        SurfaceError ret1, ret2;
        STEP("1: request buffer") {
            ret1 = producerSurface_1->RequestBufferNoFence(buffer_1, requestConfig);
            ret2 = producerSurface_2->RequestBufferNoFence(buffer_2, requestConfig);
        }

        STEP("2: check ret1 ret2 buffer_1 buffer_2 ") {
            STEP_ASSERT_EQ(ret1, SURFACE_ERROR_OK);
            STEP_ASSERT_NE(buffer_1, nullptr);
            STEP_ASSERT_EQ(ret2, SURFACE_ERROR_OK);
            STEP_ASSERT_NE(buffer_2, nullptr);
        }

        STEP("3: check buffer addr ") {
            STEP_ASSERT_EQ(buffer_2, buffer_1);
        }  
    }
}
    /*
 * Function: RequestBuffer with different requestconfig
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call RequestBufferNoFence with buffer=buffer_diff, the requestconfig is not same with buffer_1
 *                  2. check ret1 is SURFACE_ERROR_INVALID_PARAM
 * */

HWTEST_F(BufferSharedTest, RequestBufferDiff, testing::ext::TestSize.Level0)
{
    PART("REQUEST BUFFER with different requestconfig"){
        SurfaceError ret1;
        sptr<SurfaceBuffer> buffer_diff = nullptr;
        STEP("1: request buffer") {
            ret1 = producerSurface_1->RequestBufferNoFence(buffer_diff, requestConfig2);
        }

        STEP("2: check ret1 ") {
            STEP_ASSERT_EQ(ret1, SURFACE_ERROR_INVALID_PARAM);
        }  
    }
}
    /*
 * Function: FlushBuffer
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call FlushBuffer with buffer=buffer_1, buffer_2
 *                  2. check ret1 and ret2 is SURFACE_ERROR_OK
 * */
HWTEST_F(BufferSharedTest, FlushBuffer, testing::ext::TestSize.Level0)
{
    PART("FlushBuffer") {
        SurfaceError ret1, ret2;
        STEP("1: FlushBuffer two times") {
            ret1 = producerSurface_1->FlushBuffer(buffer_1, fence, flushConfig);
            ret2 = producerSurface_2->FlushBuffer(buffer_2, fence, flushConfig);
        }

        STEP("2: check ret1 ret2") {
            STEP_ASSERT_EQ(ret1, SURFACE_ERROR_OK);
            STEP_ASSERT_EQ(ret2, SURFACE_ERROR_OK);
        }
    }
}
    /*
 * Function: AquiredBuffer
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call AcquireBuffer with buffer=sbuffer_1, sbuffer_2
 *                  2. check ret1 and ret2 are WM_ERROR_NULLPTR
 * */
HWTEST_F(BufferSharedTest, AquiredBuffer, testing::ext::TestSize.Level0)
{
    PART("AquiredBuffer") {
        SurfaceError ret1, ret2;

        STEP("1: AcquireBuffer two times") {
            ret1 = surface->AcquireBuffer(sbuffer_1, fence, timestamp, damage);
            ret2 = surface->AcquireBuffer(sbuffer_2, fence, timestamp, damage);
        }

        STEP("2: check ret1 ret2") {
            STEP_ASSERT_EQ(ret1, SURFACE_ERROR_OK);
            STEP_ASSERT_EQ(ret2, SURFACE_ERROR_OK);
        }

        STEP("3: check addr sbuffer_1 and sbuffer_2") {
            STEP_ASSERT_EQ(sbuffer_1, sbuffer_2);
        }
    }
}
    /*
 * Function: CancelBuffer
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call cancelBuffer with buffer=buffer_1
 *                  2. check ret1 is SURFACE_ERROR_INVALID_OPERATING
 *                  3. call cancelBuffer with buffer=buffer_2
 *                  4. check ret2 is SURFACE_ERROR_INVALID_OPERATING
 * */
HWTEST_F(BufferSharedTest, CancelBuffer, testing::ext::TestSize.Level0)
{
    PART("CancelBuffer") {
        SurfaceError ret1, ret2;
        STEP("1: Cancel buffer_1") {
            ret1 = producerSurface_1->CancelBuffer(buffer_1);
        }

        STEP("2: check ret1") {
            STEP_ASSERT_EQ(ret1, SURFACE_ERROR_INVALID_OPERATING);
        }

        STEP("3: Cancel buffer_2"){
            ret2 = producerSurface_2->CancelBuffer(buffer_2);
        }

        STEP("4: check ret2") {
            STEP_ASSERT_EQ(ret2, SURFACE_ERROR_INVALID_OPERATING);
        }

    }
}
    /*
 * Function: RelaseBuffer
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. releaseBuffer two times
 *                  2. check ret1 is SURFACE_ERROR_INVALID_OPERATING, check ret1 is SURFACE_ERROR_OK
 * */
HWTEST_F(BufferSharedTest, ReleaseBuffer, testing::ext::TestSize.Level0)
{
    PART("ReleaseBuffer") {
        SurfaceError ret1, ret2;

        STEP("1: releaseBuffer two times") {
            ret1 = surface->ReleaseBuffer(sbuffer_1, fence);
            ret2 = surface->ReleaseBuffer(sbuffer_2, fence);
        }

        STEP("2: check ret1, ret2") {
            STEP_ASSERT_EQ(ret1, SURFACE_ERROR_OK);
            STEP_ASSERT_EQ(ret2, SURFACE_ERROR_OK);
        }
    }
}
}
} // namespace OHOS
