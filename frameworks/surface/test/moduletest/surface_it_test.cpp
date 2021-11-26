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

#include "surface_it_test.h"

#include <cstdio>

#include <display_type.h>
#include <gtest/gtest.h>
#include <surface.h>

#include "surface_buffer_impl.h"
#include "buffer_manager.h"

using namespace OHOS;
using namespace std;

#define LOG GTEST_LOG_(INFO)

class SurfaceITest : public testing::Test {
public:
    static void SetUpTestCase(void) {
        consumeSurface = Surface::CreateSurfaceAsConsumer();
        bufferProducer = consumeSurface->GetProducer();
        producerSurface = Surface::CreateSurfaceAsProducer(bufferProducer);
    }
    static void TearDownTestCase(void) {
    }

protected:
    static inline sptr<Surface> consumeSurface;  //消费者
    static inline sptr<IBufferProducer> bufferProducer;
    static inline sptr<Surface> producerSurface; //生产者

    BufferRequestConfig requestConfig = {
        .width = 1920,
        .height = 1080,
        .strideAlignment = 8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    };
    BufferFlushConfig flushConfig = {
        .damage = {
            .w = 1920,
            .h = 1080,
        },
    };
    int64_t timeStamp;
    Rect damageRec;
};

class BufferConsumerListener: public IBufferConsumerListener {
public:
    BufferConsumerListener(int32_t *var = nullptr) {
        variable = var;
    }
    virtual ~BufferConsumerListener() = default;
    virtual void OnBufferAvailable() override {
        if (variable != nullptr) {
            (*variable)++;
        }
    }

private:
    int32_t *variable = nullptr;
};

namespace {
constexpr int64_t SEC_TO_USEC = 1000 * 1000;
constexpr int64_t OneFrameTime = 1 * SEC_TO_USEC / 60; // 1second, 60Hz
constexpr int64_t PERFORMANCE_COUNT = 1000;
int64_t GetNowTime() {
    struct timeval time;
    gettimeofday(&time, nullptr);
    return SEC_TO_USEC * time.tv_sec + time.tv_usec;
}


// void UserDataTest(const int iCountMax, const sptr<Surface>& tempSurface)
// {
//     int iCount = 0;
//     std::string key = "testkey";
//     std::string val = "testval";
//     for (iCount = 0; iCount < iCountMax; iCount++) {
//         char tempKey[10] = "";
//         char tempVal[10] = {0};
//         SurfaceError ret = tempSurface->SetUserData(std::string(tempKey), std::string(tempVal));
//         ASSERT_EQ(ret, SURFACE_ERROR_OK);
//         std::string retval = tempSurface->GetUserData(std::string(tempKey));
//         ASSERT_STREQ(retval.c_str(), tempVal);
//     }
//     ASSERT_EQ(iCount, iCountMax);
// }

// void UserDataTest(const int iCountMax, const sptr<Surface>& tempSurface)
// {
//     int iCount = 0;
//     std::string key = "testkey";
//     std::string val = "testval";
//     for (iCount = 0; iCount < iCountMax; iCount++) {
//         char tempKey[10] = "";
//         char tempVal[10] = {0};
//         SurfaceError ret = tempSurface->SetUserData(std::string(tempKey), std::string(tempVal));
//         ASSERT_EQ(ret, SURFACE_ERROR_OK);
//         std::string retval = tempSurface->GetUserData(std::string(tempKey));
//         ASSERT_STREQ(retval.c_str(), tempVal);
//     }
//     ASSERT_EQ(iCount, iCountMax);
// }




/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_0100
 * @tc.name    TestCreateSurfaceAsConsumer1
 * @tc.desc    CreateSurfaceAsConsumer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCreateSurfaceAsConsumer1, testing::ext::TestSize.Level1) {
    ASSERT_NE(consumeSurface, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_0200
 * @tc.name    TestCreateSurfaceAsConsumer2
 * @tc.desc    CreateSurfaceAsConsumer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCreateSurfaceAsConsumer2, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();
    Surface::CreateSurfaceAsConsumer();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_0300
 * @tc.name    TestCreateSurfaceAsConsumer3
 * @tc.desc    CreateSurfaceAsConsumer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCreateSurfaceAsConsumer3, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_NE(Surface::CreateSurfaceAsConsumer(), nullptr);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_0400
 * @tc.name    TestCreateSurfaceAsProducer1
 * @tc.desc    CreateSurfaceAsProducer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCreateSurfaceAsProducer1, testing::ext::TestSize.Level1) {
    ASSERT_NE(producerSurface, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_0500
 * @tc.name    TestCreateSurfaceAsProducer2
 * @tc.desc    CreateSurfaceAsProducer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCreateSurfaceAsProducer2, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();
    Surface::CreateSurfaceAsProducer(bufferProducer);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_0600
 * @tc.name    TestCreateSurfaceAsProducer3
 * @tc.desc    CreateSurfaceAsProducer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCreateSurfaceAsProducer3, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_NE(Surface::CreateSurfaceAsProducer(bufferProducer), nullptr);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_0700
 * @tc.name    TestIsConsumer1
 * @tc.desc    IsConsumer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestIsConsumer1, testing::ext::TestSize.Level1) {
    EXPECT_EQ(true, consumeSurface->IsConsumer());
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_0800
 * @tc.name    TestIsConsumer2
 * @tc.desc    IsConsumer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestIsConsumer2, testing::ext::TestSize.Level1) {
    EXPECT_EQ(false, producerSurface->IsConsumer());
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_0900
 * @tc.name    TestIsConsumer3
 * @tc.desc    IsConsumer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestIsConsumer3, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();
    consumeSurface->IsConsumer();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_1000
 * @tc.name    TestIsConsumer4
 * @tc.desc    IsConsumer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestIsConsumer4, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(Surface::CreateSurfaceAsConsumer()->IsConsumer(), true);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_1100
 * @tc.name    TestGetProducer1
 * @tc.desc    GetProducer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetProducer1, testing::ext::TestSize.Level1) {
    ASSERT_NE(nullptr, consumeSurface->GetProducer());
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_1200
 * @tc.name    TestGetProducer2
 * @tc.desc    GetProducer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetProducer2, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();
    consumeSurface->GetProducer();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_1300
 * @tc.name    TestGetProducer3
 * @tc.desc    GetProducer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetProducer3, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_NE(Surface::CreateSurfaceAsConsumer()->GetProducer(), nullptr);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_1400
 * @tc.name    TestRequestBuffer1
 * @tc.desc    RequestBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestRequestBuffer1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    ASSERT_EQ( producerSurface->RequestBuffer(buffer, fence, requestConfig), SURFACE_ERROR_OK);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}
/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_1500
 * @tc.name    TestRequestBuffer2
 * @tc.desc    RequestBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestRequestBuffer2, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    requestConfig.width = -1 ;
    ASSERT_EQ( producerSurface->RequestBuffer(buffer, fence, requestConfig),  SURFACE_ERROR_INVALID_PARAM );
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_1600
 * @tc.name    TestRequestBuffer3
 * @tc.desc    RequestBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestRequestBuffer3, testing::ext::TestSize.Level1) {
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    ASSERT_EQ(producerSurface->RequestBuffer(buffer, fence, requestConfig), SURFACE_ERROR_NO_CONSUMER);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_1700
 * @tc.name    TestRequestBuffer4
 * @tc.desc    RequestBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestRequestBuffer4, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer1 = nullptr ;
    sptr<SurfaceBuffer> buffer2 = nullptr ;
    sptr<SurfaceBuffer> buffer3 = nullptr ;
    sptr<SurfaceBuffer> buffer4 = nullptr ;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer1, fence, requestConfig);
    producerSurface->RequestBuffer(buffer2, fence, requestConfig);
    producerSurface->RequestBuffer(buffer3, fence, requestConfig);
    ASSERT_EQ(producerSurface->RequestBuffer(buffer4, fence, requestConfig), SURFACE_ERROR_NO_BUFFER);
    producerSurface->CancelBuffer(buffer1);
    producerSurface->CancelBuffer(buffer2);
    producerSurface->CancelBuffer(buffer3);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_1800
 * @tc.name    TestRequestBuffer5
 * @tc.desc    RequestBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestRequestBuffer5, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    ASSERT_EQ( consumeSurface->RequestBuffer(buffer, fence, requestConfig), SURFACE_ERROR_NOT_SUPPORT);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_1900
 * @tc.name    TestRequestBuffer6
 * @tc.desc    RequestBuffer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestRequestBuffer6, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    auto startTime = GetNowTime();
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto endTime = GetNowTime();
    EXPECT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_2000
 * @tc.name    TestRequestBuffer7
 * @tc.desc    RequestBuffer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest,  TestRequestBuffer7, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(producerSurface->RequestBuffer(buffer, fence, requestConfig), SURFACE_ERROR_OK);
        producerSurface->CancelBuffer(buffer) ;
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    consumeSurface->UnregisterConsumerListener();
}






/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_2100
 * @tc.name    TestCancelBuffer1
 * @tc.desc    CancelBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCancelBuffer1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(producerSurface->CancelBuffer(buffer), SURFACE_ERROR_OK);
    consumeSurface->UnregisterConsumerListener();
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_2200
 * @tc.name    TestCancelBuffer2
 * @tc.desc    CancelBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCancelBuffer2, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    producerSurface->CancelBuffer(buffer);
    ASSERT_EQ(producerSurface->CancelBuffer(buffer), SURFACE_ERROR_INVALID_OPERATING);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_2300
 * @tc.name    TestCancelBuffer3
 * @tc.desc    CancelBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCancelBuffer3, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(consumeSurface->CancelBuffer(buffer), SURFACE_ERROR_NOT_SUPPORT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_2400
 * @tc.name    TestCancelBuffer4
 * @tc.desc    CancelBuffer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCancelBuffer4, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    producerSurface->CancelBuffer(buffer);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_2500
 * @tc.name    TestCancelBuffer5
 * @tc.desc    CancelBuffer
 * @tc.size    Level2
 * @tc.type    Function
  */
HWTEST_F(SurfaceITest, TestCancelBuffer5, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        producerSurface->RequestBuffer(buffer, fence, requestConfig);
        ASSERT_EQ(producerSurface->CancelBuffer(buffer), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_2600
 * @tc.name    TestFlushBuffer1
 * @tc.desc    FlushBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestFlushBuffer1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t consumerFence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(producerSurface->FlushBuffer(buffer, fence, flushConfig), SURFACE_ERROR_OK);
    consumeSurface->AcquireBuffer(buffer, consumerFence, timeStamp, damage);
    consumeSurface->ReleaseBuffer(buffer, fence);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_2700
 * @tc.name    TestFlushBuffer2
 * @tc.desc    FlushBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestFlushBuffer2, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t consumerFence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    flushConfig.damage.w = -1;
    ASSERT_EQ(producerSurface->FlushBuffer(buffer, fence, flushConfig), SURFACE_ERROR_INVALID_PARAM);
    flushConfig.damage.w = 1920;
    producerSurface->FlushBuffer(buffer, fence, flushConfig);
    consumeSurface->AcquireBuffer(buffer, consumerFence, timeStamp, damage);
    consumeSurface->ReleaseBuffer(buffer, fence);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_2800
 * @tc.name    TestFlushBuffer3
 * @tc.desc    FlushBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestFlushBuffer3, testing::ext::TestSize.Level1) {
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    EXPECT_EQ(producerSurface->RequestBuffer(buffer, fence, requestConfig), SURFACE_ERROR_NO_CONSUMER);
    EXPECT_EQ(producerSurface->FlushBuffer(buffer, fence, flushConfig), 40001000);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_2900
 * @tc.name    TestFlushBuffer4
 * @tc.desc    FlushBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestFlushBuffer4, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t consumerFence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    producerSurface->FlushBuffer(buffer, fence, flushConfig);
    ASSERT_EQ(producerSurface->FlushBuffer(buffer, fence, flushConfig), SURFACE_ERROR_INVALID_OPERATING);
    consumeSurface->AcquireBuffer(buffer, consumerFence, timeStamp, damage);
    consumeSurface->ReleaseBuffer(buffer, fence);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_3000
 * @tc.name    TestFlushBuffer5
 * @tc.desc    FlushBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestFlushBuffer5, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(consumeSurface->FlushBuffer(buffer, fence, flushConfig), SURFACE_ERROR_NOT_SUPPORT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_3100
 * @tc.name    TestFlushBuffer6
 * @tc.desc    FlushBuffer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestFlushBuffer6, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t consumerFence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    producerSurface->FlushBuffer(buffer, fence, flushConfig);
    auto endTime = GetNowTime();
    consumeSurface->AcquireBuffer(buffer, consumerFence, timeStamp, damage);
    consumeSurface->ReleaseBuffer(buffer, fence);
    ASSERT_LT(endTime - startTime, OneFrameTime);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_3200
 * @tc.name    TestFlushBuffer7
 * @tc.desc    FlushBuffer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestFlushBuffer7, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    int32_t consumerFence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        producerSurface->RequestBuffer(buffer, fence, requestConfig);
        ASSERT_EQ(producerSurface->FlushBuffer(buffer, fence, flushConfig), SURFACE_ERROR_OK);
        consumeSurface->AcquireBuffer(buffer, consumerFence, timeStamp, damage);
        consumeSurface->ReleaseBuffer(buffer, fence);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_3300
 * @tc.name    TestAcquireBuffer1
 * @tc.desc    AcquireBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestAcquireBuffer1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    int32_t consumerFence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    producerSurface->FlushBuffer(buffer, fence, flushConfig);
    ASSERT_EQ( consumeSurface->AcquireBuffer(buffer, consumerFence, timeStamp, damage), SURFACE_ERROR_OK);
    consumeSurface->ReleaseBuffer(buffer, -1);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_3400
 * @tc.name    TestAcquireBuffer2
 * @tc.desc    AcquireBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestAcquireBuffer2, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t consumerFence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    ASSERT_EQ(consumeSurface->AcquireBuffer(buffer, consumerFence, timeStamp, damage), SURFACE_ERROR_NO_BUFFER);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_3500
 * @tc.name    TestAcquireBuffer3
 * @tc.desc    AcquireBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestAcquireBuffer3, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t consumerFence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    ASSERT_EQ(producerSurface->AcquireBuffer(buffer, consumerFence, timeStamp, damage), SURFACE_ERROR_NOT_SUPPORT);
    consumeSurface->UnregisterConsumerListener();
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_3600
 * @tc.name    TestAcquireBuffer4
 * @tc.desc    AcquireBuffer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestAcquireBuffer4, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    int32_t consumerfence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    producerSurface->FlushBuffer(buffer, fence, flushConfig);
    auto startTime = GetNowTime();
    consumeSurface->AcquireBuffer(buffer, consumerfence, timeStamp, damage);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    consumeSurface->ReleaseBuffer(buffer, -1);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_3700
 * @tc.name    TestAcquireBuffer5
 * @tc.desc    AcquireBuffer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestAcquireBuffer5, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    int32_t consumerfence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        producerSurface->RequestBuffer(buffer, fence, requestConfig);
        producerSurface->FlushBuffer(buffer, fence, flushConfig);
        ASSERT_EQ(consumeSurface->AcquireBuffer(buffer, consumerfence, timeStamp, damage), SURFACE_ERROR_OK);
        consumeSurface->ReleaseBuffer(buffer, -1);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    consumeSurface->UnregisterConsumerListener();
}




/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_3800
 * @tc.name    TestReleaseBuffer1
 * @tc.desc    ReleaseBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestReleaseBuffer1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    producerSurface->FlushBuffer(buffer, fence, flushConfig);
    consumeSurface->AcquireBuffer(buffer, fence, timeStamp, damageRec);
    ASSERT_EQ(consumeSurface->ReleaseBuffer(buffer, fence), SURFACE_ERROR_OK);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_3900
 * @tc.name    TestReleaseBuffer2
 * @tc.desc    ReleaseBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestReleaseBuffer2, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    producerSurface->FlushBuffer(buffer, fence, flushConfig);
    consumeSurface->AcquireBuffer(buffer, fence, timeStamp, damageRec);
    consumeSurface->ReleaseBuffer(buffer, fence);
    ASSERT_EQ(consumeSurface->ReleaseBuffer(buffer, fence), SURFACE_ERROR_INVALID_OPERATING);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_4000
 * @tc.name    TestReleaseBuffer3
 * @tc.desc    ReleaseBuffer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestReleaseBuffer3, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    producerSurface->FlushBuffer(buffer, fence, flushConfig);
    consumeSurface->AcquireBuffer(buffer, fence, timeStamp, damageRec);
    ASSERT_EQ(producerSurface->ReleaseBuffer(buffer, fence), SURFACE_ERROR_NOT_SUPPORT);
    consumeSurface->ReleaseBuffer(buffer, fence);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_4100
 * @tc.name    TestReleaseBuffer4
 * @tc.desc    ReleaseBuffer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestReleaseBuffer4, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    int32_t consumerfence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    producerSurface->FlushBuffer(buffer, -1, flushConfig);
    consumeSurface->AcquireBuffer(buffer, consumerfence, timeStamp, damage);
    auto startTime = GetNowTime();
    consumeSurface->ReleaseBuffer(buffer, -1);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_4200
 * @tc.name    TestReleaseBuffer5
 * @tc.desc    ReleaseBuffer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestReleaseBuffer5, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    int32_t consumerfence = 0;
    int64_t timeStamp = 0;
    Rect damage;
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        producerSurface->RequestBuffer(buffer, fence, requestConfig);
        producerSurface->FlushBuffer(buffer, -1, flushConfig);
        consumeSurface->AcquireBuffer(buffer, consumerfence, timeStamp, damage);
        ASSERT_EQ(consumeSurface->ReleaseBuffer(buffer, -1), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_4300
 * @tc.name    TestGetQueueSize1
 * @tc.desc    GetQueueSize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetQueueSize1, testing::ext::TestSize.Level1) {
    consumeSurface->SetQueueSize(1);
    ASSERT_EQ(consumeSurface->GetQueueSize(), 1);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_4400
 * @tc.name    TestGetQueueSize2
 * @tc.desc    GetQueueSize
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetQueueSize2, testing::ext::TestSize.Level2) {
    consumeSurface->SetQueueSize(1);
    auto startTime = GetNowTime();
    consumeSurface->GetQueueSize();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_4500
 * @tc.name    TestGetQueueSize3
 * @tc.desc    GetQueueSize
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest,  TestGetQueueSize3, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        consumeSurface->SetQueueSize(1);
        ASSERT_EQ(consumeSurface->GetQueueSize(), 1);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_4600
 * @tc.name    TestSetQueueSize1
 * @tc.desc    SetQueueSize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetQueueSize1, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetQueueSize(1), SURFACE_ERROR_OK);
}
/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_4700
 * @tc.name    TestSetQueueSize2
 * @tc.desc    SetQueueSize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetQueueSize2, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetQueueSize(0), SURFACE_ERROR_INVALID_PARAM);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_4800
 * @tc.name    TestSetQueueSize3
 * @tc.desc    SetQueueSize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetQueueSize3, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetQueueSize(SURFACE_MAX_QUEUE_SIZE), SURFACE_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_4900
 * @tc.name    TestSetQueueSize4
 * @tc.desc    SetQueueSize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetQueueSize4, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetQueueSize(SURFACE_MAX_QUEUE_SIZE+1),SURFACE_ERROR_INVALID_PARAM);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_5000
 * @tc.name    TestSetQueueSize5
 * @tc.desc    SetQueueSize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetQueueSize5, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetQueueSize(SURFACE_DEFAULT_QUEUE_SIZE),SURFACE_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_5100
 * @tc.name    TestSetQueueSize6
 * @tc.desc    SetQueueSize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetQueueSize6, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetQueueSize(-1),SURFACE_ERROR_INVALID_PARAM);
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_5200
 * @tc.name    TestSetQueueSize7
 * @tc.desc    SetQueueSize
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetQueueSize7, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();
    consumeSurface->SetQueueSize(1);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}
/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_5300
 * @tc.name    TestSetQueueSize8
 * @tc.desc    SetQueueSize
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetQueueSize8, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(consumeSurface->SetQueueSize(1), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_5400
 * @tc.name    TestGetName1
 * @tc.desc    GetName
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetName1, testing::ext::TestSize.Level1) {
    std::string name = "noname";
    ASSERT_EQ(consumeSurface->GetName(name), SURFACE_ERROR_OK);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_5500
 * @tc.name    TestGetName2
 * @tc.desc    GetName
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetName2, testing::ext::TestSize.Level2) {
    std::string name = "noname";
    auto startTime = GetNowTime();
    consumeSurface->GetName(name);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_5600
 * @tc.name    TestGetName3
 * @tc.desc    GetName
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetName3, testing::ext::TestSize.Level2) {
    std::string name = "noname";
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(consumeSurface->GetName(name), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}





/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_5700
 * @tc.name    TestSetDefaultWidthAndHeight1
 * @tc.desc    SetDefaultWidthAndHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultWidthAndHeight1, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetDefaultWidthAndHeight(1024, 720), SURFACE_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_5800
 * @tc.name    TestSetDefaultWidthAndHeight2
 * @tc.desc    SetDefaultWidthAndHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultWidthAndHeight2, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetDefaultWidthAndHeight(-1024, 720), SURFACE_ERROR_INVALID_PARAM);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_5900
 * @tc.name    TestSetDefaultWidthAndHeight3
 * @tc.desc    SetDefaultWidthAndHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultWidthAndHeight3, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetDefaultWidthAndHeight(1024, -720), SURFACE_ERROR_INVALID_PARAM);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_6000
 * @tc.name    TestSetDefaultWidthAndHeight4
 * @tc.desc    SetDefaultWidthAndHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultWidthAndHeight4, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetDefaultWidthAndHeight(-1024, -720), SURFACE_ERROR_INVALID_PARAM);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_6100
 * @tc.name    TestSetDefaultWidthAndHeight5
 * @tc.desc    SetDefaultWidthAndHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultWidthAndHeight5, testing::ext::TestSize.Level1) {
    ASSERT_EQ(producerSurface->SetDefaultWidthAndHeight(1024, 720), SURFACE_ERROR_NOT_SUPPORT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_6200
 * @tc.name    TestSetDefaultWidthAndHeight6
 * @tc.desc    SetDefaultWidthAndHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultWidthAndHeight6, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetDefaultWidthAndHeight(SURFACE_MAX_WIDTH,SURFACE_MAX_HEIGHT),SURFACE_ERROR_OK );
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_6300
 * @tc.name    TestSetDefaultWidthAndHeight7
 * @tc.desc    SetDefaultWidthAndHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultWidthAndHeight7, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetDefaultWidthAndHeight(SURFACE_MAX_WIDTH+1,SURFACE_MAX_HEIGHT),SURFACE_ERROR_INVALID_PARAM);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_6400
 * @tc.name    TestSetDefaultWidthAndHeight8
 * @tc.desc    SetDefaultWidthAndHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultWidthAndHeight8, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetDefaultWidthAndHeight(SURFACE_MAX_WIDTH,SURFACE_MAX_HEIGHT+1),SURFACE_ERROR_INVALID_PARAM);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_6500
 * @tc.name    TestSetDefaultWidthAndHeight9
 * @tc.desc    SetDefaultWidthAndHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultWidthAndHeight9, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();
    consumeSurface->SetDefaultWidthAndHeight(consumeSurface->GetDefaultWidth(), consumeSurface->GetDefaultHeight());
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_6600
 * @tc.name    TestSetDefaultWidthAndHeight10
 * @tc.desc    SetDefaultWidthAndHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultWidthAndHeight10, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(consumeSurface->SetDefaultWidthAndHeight(consumeSurface->GetDefaultWidth(), consumeSurface->GetDefaultHeight()), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}




/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_6700
 * @tc.name    TestGetDefaultWidth1
 * @tc.desc    GetDefaultWidth
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultWidth1, testing::ext::TestSize.Level1) {
    consumeSurface->SetDefaultWidthAndHeight(1080, 720);
    ASSERT_EQ(consumeSurface->GetDefaultWidth(), 1080);
}
/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_6800
 * @tc.name    TestGetDefaultWidth2
 * @tc.desc    GetDefaultWidth
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultWidth2, testing::ext::TestSize.Level1) {
    consumeSurface->SetDefaultWidthAndHeight(SURFACE_MAX_WIDTH,SURFACE_MAX_HEIGHT);
    ASSERT_EQ(consumeSurface->GetDefaultWidth(),SURFACE_MAX_WIDTH );
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_6900
 * @tc.name    TestGetDefaultWidth3
 * @tc.desc    GetDefaultWidth
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultWidth3, testing::ext::TestSize.Level2) {
    consumeSurface->SetDefaultWidthAndHeight(1080, 720);
    auto startTime = GetNowTime();
    consumeSurface->GetDefaultWidth();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_7000
 * @tc.name    TestGetDefaultWidth4
 * @tc.desc    GetDefaultWidth
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultWidth4, testing::ext::TestSize.Level2) {
    consumeSurface->SetDefaultWidthAndHeight(1080, 720);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(consumeSurface->GetDefaultWidth(), 1080);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_7100
 * @tc.name    TestGetDefaultHeight1
 * @tc.desc    GetDefaultHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultHeight1, testing::ext::TestSize.Level1) {
    consumeSurface->SetDefaultWidthAndHeight(1080, 720);
    ASSERT_EQ(consumeSurface->GetDefaultHeight(), 720);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_7200
 * @tc.name    TestGetDefaultHeight2
 * @tc.desc    GetDefaultHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultHeight2, testing::ext::TestSize.Level1) {
    consumeSurface->SetDefaultWidthAndHeight(SURFACE_MAX_WIDTH,SURFACE_MAX_HEIGHT);
    ASSERT_EQ(consumeSurface->GetDefaultHeight(), SURFACE_MAX_HEIGHT);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_7300
 * @tc.name    TestGetDefaultHeight3
 * @tc.desc    GetDefaultHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultHeight3, testing::ext::TestSize.Level2) {
    consumeSurface->SetDefaultWidthAndHeight(1080, 720);
    auto startTime = GetNowTime();
    consumeSurface->GetDefaultHeight();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_7400
 * @tc.name    TestGetDefaultHeight4
 * @tc.desc    GetDefaultHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultHeight4, testing::ext::TestSize.Level1) {
    consumeSurface->SetDefaultWidthAndHeight(1080, 720);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(consumeSurface->GetDefaultHeight(), 720);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_7500
 * @tc.name    TestSetDefaultUsage1
 * @tc.desc    SetDefaultUsage
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultUsage1, testing::ext::TestSize.Level1) {
    uint32_t usage = 3 ;
    ASSERT_EQ(consumeSurface->SetDefaultUsage(usage), SURFACE_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_7600
 * @tc.name    TestSetDefaultUsage2
 * @tc.desc    SetDefaultUsage
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultUsage2, testing::ext::TestSize.Level1) {
    uint32_t usage = 0 ;
    ASSERT_EQ(consumeSurface->SetDefaultUsage(usage), SURFACE_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_7700
 * @tc.name    TestSetDefaultUsage3
 * @tc.desc    SetDefaultUsage
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultUsage3, testing::ext::TestSize.Level1) {
    uint32_t usage = 3 ;
    ASSERT_EQ(producerSurface->SetDefaultUsage(usage), SURFACE_ERROR_NOT_SUPPORT);
}
/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_7800
 * @tc.name    TestSetDefaultUsage4
 * @tc.desc    SetDefaultUsage
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultUsage4, testing::ext::TestSize.Level2) {
    uint32_t usage = 3 ;
    auto startTime = GetNowTime();
    consumeSurface->SetDefaultUsage(usage);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_7900
 * @tc.name    TestSetDefaultUsage5
 * @tc.desc    SetDefaultUsage
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetDefaultUsage5, testing::ext::TestSize.Level2) {
    uint32_t usage = 3 ;
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(consumeSurface->SetDefaultUsage(usage), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}







/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_8000
 * @tc.name    TestGetDefaultUsage1
 * @tc.desc    GetDefaultUsage
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultUsage1, testing::ext::TestSize.Level1) {
    uint32_t usage = 3 ;
    consumeSurface->SetDefaultUsage(usage);
    ASSERT_EQ(consumeSurface->GetDefaultUsage(), 3);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_8100
 * @tc.name    TestGetDefaultUsage2
 * @tc.desc    GetDefaultUsage
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultUsage2, testing::ext::TestSize.Level2) {
    uint32_t usage = 3 ;
    consumeSurface->SetDefaultUsage(usage);
    auto startTime = GetNowTime();
    consumeSurface->GetDefaultUsage();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_8200
 * @tc.name    TestGetDefaultUsage3
 * @tc.desc    GetDefaultUsage
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetDefaultUsage3, testing::ext::TestSize.Level2) {
    uint32_t usage = 3 ;
    consumeSurface->SetDefaultUsage(usage);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(consumeSurface->GetDefaultUsage(), 3);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}






/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_8300
 * @tc.name    TestSetUserData1
 * @tc.desc    SetUserData
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetUserData1, testing::ext::TestSize.Level1) {
    ASSERT_EQ(consumeSurface->SetUserData("key", "123"), SURFACE_ERROR_OK);
    ASSERT_EQ(consumeSurface->GetUserData("key"), "123");
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_8400
 * @tc.name    TestSetUserData2
 * @tc.desc    SetUserData
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetUserData2, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();
    consumeSurface->SetUserData("key", "123");
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_8500
 * @tc.name    TestSetUserData3
 * @tc.desc    SetUserData
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetUserData3, testing::ext::TestSize.Level2) {
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(consumeSurface->SetUserData("key", "123"), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_8600
 * @tc.name    TestGetUserData1
 * @tc.desc    GetUserData
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetUserData1, testing::ext::TestSize.Level1) {
    consumeSurface->SetUserData("key", "123");
    ASSERT_EQ(consumeSurface->GetUserData("key"), "123");
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_8700
 * @tc.name    TestGetUserData2
 * @tc.desc    GetUserData
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetUserData2, testing::ext::TestSize.Level2) {
    consumeSurface->SetUserData("key", "123");
    auto startTime = GetNowTime();
    consumeSurface->GetUserData("key");
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_8800
 * @tc.name    TestGetUserData3
 * @tc.desc    GetUserData
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetUserData3, testing::ext::TestSize.Level2) {
    consumeSurface->SetUserData("key", "123");
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(consumeSurface->GetUserData("key"), "123");
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_8900
 * @tc.name    TestRegisterConsumerListener1
 * @tc.desc    RegisterConsumerListener
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestRegisterConsumerListener1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> pListener = new BufferConsumerListener();
    ASSERT_EQ(consumeSurface->RegisterConsumerListener(pListener), SURFACE_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_9000
 * @tc.name    TestRegisterConsumerListener2
 * @tc.desc    RegisterConsumerListener
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestRegisterConsumerListener2, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> pListener = new BufferConsumerListener();
    ASSERT_EQ(producerSurface->RegisterConsumerListener(pListener), SURFACE_ERROR_NOT_SUPPORT);
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_9100
 * @tc.name    TestRegisterConsumerListener3
 * @tc.desc    RegisterConsumerListener
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestRegisterConsumerListener3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> pListener = new BufferConsumerListener();
    auto startTime = GetNowTime();
    consumeSurface->RegisterConsumerListener(pListener);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_9200
 * @tc.name    TestRegisterConsumerListener4
 * @tc.desc    RegisterConsumerListener
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest,  TestRegisterConsumerListener4, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> pListener = new BufferConsumerListener();
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(consumeSurface->RegisterConsumerListener(pListener), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_9300
 * @tc.name    TestUnregisterConsumerListener1
 * @tc.desc    UnregisterConsumerListener
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestUnregisterConsumerListener1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> pListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(pListener);
    ASSERT_EQ(consumeSurface->UnregisterConsumerListener(), SURFACE_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_9400
 * @tc.name    TestUnregisterConsumerListener2
 * @tc.desc    UnregisterConsumerListener
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestUnregisterConsumerListener2, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> pListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(pListener);
    ASSERT_EQ(producerSurface->UnregisterConsumerListener(), SURFACE_ERROR_NOT_SUPPORT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_9500
 * @tc.name    TestUnregisterConsumerListener3
 * @tc.desc    UnregisterConsumerListener
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestUnregisterConsumerListener3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> pListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(pListener);
    auto startTime = GetNowTime();
    consumeSurface->UnregisterConsumerListener();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
}
/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_9600
 * @tc.name    TestUnregisterConsumerListener4
 * @tc.desc    UnregisterConsumerListener
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestUnregisterConsumerListener4, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> pListener = new BufferConsumerListener();
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        consumeSurface->RegisterConsumerListener(pListener);
        ASSERT_EQ(consumeSurface->UnregisterConsumerListener(), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_9700
 * @tc.name    TestCleanCache1
 * @tc.desc    CleanCache
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCleanCache1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(producerSurface->CleanCache(), SURFACE_ERROR_OK);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_9800
 * @tc.name    TestCleanCache2
 * @tc.desc    CleanCache
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCleanCache2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    producerSurface->CleanCache();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}
/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_9900
 * @tc.name    TestCleanCache3
 * @tc.desc    CleanCache
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestCleanCache3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        producerSurface->RequestBuffer(buffer, fence, requestConfig);
        ASSERT_EQ(producerSurface->CleanCache(), SURFACE_ERROR_OK);
        producerSurface->CancelBuffer(buffer);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    consumeSurface->UnregisterConsumerListener();
}







/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_10000
 * @tc.name    TestGetBufferHandle1
 * @tc.desc    GetBufferHandle
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetBufferHandle1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_NE(buffer->GetBufferHandle(), nullptr);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_10100
 * @tc.name    TestGetBufferHandle2
 * @tc.desc    GetBufferHandle
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetBufferHandle2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->GetBufferHandle();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_10200
 * @tc.name    TestGetBufferHandle3
 * @tc.desc    GetBufferHandle
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetBufferHandle3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        producerSurface->RequestBuffer(buffer, fence, requestConfig);
        ASSERT_NE(buffer->GetBufferHandle(), nullptr);
        producerSurface->CancelBuffer(buffer);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    consumeSurface->UnregisterConsumerListener();
}




/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_10300
 * @tc.name    TestGetWidth1
 * @tc.desc    GetWidth
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetWidth1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(buffer->GetWidth(), 1920);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_10400
 * @tc.name    TestGetWidth2
 * @tc.desc    GetWidth
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetWidth2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->GetWidth();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_10500
 * @tc.name    TestGetWidth3
 * @tc.desc    GetWidth
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetWidth3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(buffer->GetWidth(), 1920);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_10600
 * @tc.name    TestGetHeight1
 * @tc.desc    GetHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetHeight1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(buffer->GetHeight(), 1080);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_10700
 * @tc.name    TestGetHeight2
 * @tc.desc    GetHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetHeight2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->GetHeight();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}
/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_10800
 * @tc.name    TestGetHeight3
 * @tc.desc    GetHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetHeight3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(buffer->GetHeight(), 1080);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_10900
 * @tc.name    TestGetFormat1
 * @tc.desc    GetFormat
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetFormat1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(buffer->GetFormat(), PIXEL_FMT_RGBA_8888);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_11000
 * @tc.name    TestGetFormat2
 * @tc.desc    GetFormat
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetFormat2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->GetFormat();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_11100
 * @tc.name    TestGetFormat3
 * @tc.desc    GetFormat
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetFormat3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(buffer->GetFormat(), PIXEL_FMT_RGBA_8888);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_11200
 * @tc.name    TestGetUsage1
 * @tc.desc    GetUsage
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetUsage1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(buffer->GetUsage(), HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_11300
 * @tc.name    TestGetUsage2
 * @tc.desc    GetUsage
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetUsage2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->GetUsage();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_11400
 * @tc.name    TestGetUsage3
 * @tc.desc    GetUsage
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetUsage3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(buffer->GetUsage(), HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_11500
 * @tc.name    TestGetKey1
 * @tc.desc    GetKey
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetKey1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_NE(buffer->GetKey(), -1);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}
/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_11600
 * @tc.name    TestGetKey2
 * @tc.desc    GetKey
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetKey2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->GetKey();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_11700
 * @tc.name    TestGetKey3
 * @tc.desc    GetKey
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetKey3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_NE(buffer->GetKey(), -1);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_11800
 * @tc.name    TestGetPhyAddr1
 * @tc.desc    GetPhyAddr
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest,  TestGetPhyAddr1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_NE(buffer->GetPhyAddr(), 0);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_11900
 * @tc.name    TestGetPhyAddr2
 * @tc.desc    GetPhyAddr
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetPhyAddr2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->GetPhyAddr();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_12000
 * @tc.name    TestGetPhyAddr3
 * @tc.desc    GetPhyAddr
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest,  TestGetPhyAddr3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_NE(buffer->GetPhyAddr(), 0);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_12100
 * @tc.name    TestGetVirAddr1
 * @tc.desc    GetVirAddr
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetVirAddr1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_NE(buffer->GetVirAddr(), nullptr);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_12200
 * @tc.name    TestGetVirAddr2
 * @tc.desc    GetVirAddr
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetVirAddr2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->GetVirAddr();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}
/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_12300
 * @tc.name    TestGetVirAddr3
 * @tc.desc    GetVirAddr
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetVirAddr3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_NE(buffer->GetVirAddr(), nullptr);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}




/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_12400
 * @tc.name    TestGetFileDescriptor1
 * @tc.desc    GetFileDescriptor
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetFileDescriptor1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_NE(buffer->GetFileDescriptor(), -1);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_12500
 * @tc.name    TestGetFileDescriptor2
 * @tc.desc    GetFileDescriptor
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetFileDescriptor2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->GetFileDescriptor();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_12600
 * @tc.name    TestGetFileDescriptor3
 * @tc.desc    GetFileDescriptor
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetFileDescriptor3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_NE(buffer->GetFileDescriptor(), -1);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}



/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_12700
 * @tc.name    TestGetSize1
 * @tc.desc    GetSize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetSize1, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_GT(buffer->GetSize(), 0);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_12800
 * @tc.name    TestGetSize2
 * @tc.desc    GetSize
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetSize2, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->GetSize();
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_12900
 * @tc.name    TestGetSize3
 * @tc.desc    GetSize
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetSize3, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_GT(buffer->GetSize(), 0);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}




/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_13000
 * @tc.name    TestSetInt321
 * @tc.desc    SetInt32
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetInt321, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    uint32_t key = 1;
    int32_t value = 1;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(buffer->SetInt32(key, value), SURFACE_ERROR_OK);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_13100
 * @tc.name    TestSetInt322
 * @tc.desc    SetInt32
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetInt322, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    uint32_t key = 1;
    int32_t value = 1;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->SetInt32(key, value);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_13200
 * @tc.name    TestSetInt323
 * @tc.desc    SetInt32
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetInt323, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    uint32_t key = 1;
    int32_t value = 1;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(buffer->SetInt32(key, value), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_13300
 * @tc.name    TestGetInt321
 * @tc.desc    GetInt32
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetInt321, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    uint32_t key = 1;
    int32_t value = 1;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    buffer->SetInt32(key, value);
    ASSERT_EQ(buffer->GetInt32(key, value), SURFACE_ERROR_OK);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_13400
 * @tc.name    TestGetInt322
 * @tc.desc    GetInt32
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetInt322, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    uint32_t key = 10000;
    int32_t value = 10000;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(buffer->GetInt32(key, value), SURFACE_ERROR_NO_ENTRY);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_13500
 * @tc.name    TestGetInt323
 * @tc.desc    GetInt32
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetInt323, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    uint32_t key = 1;
    int32_t value = 1;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    buffer->SetInt32(key, value);
    auto startTime = GetNowTime();
    buffer->GetInt32(key, value);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_13600
 * @tc.name    TestGetInt324
 * @tc.desc    GetInt32
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetInt324, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    uint32_t key = 1;
    int32_t value = 1;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        buffer->SetInt32(key, value);
        ASSERT_EQ(buffer->GetInt32(key, value), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_13700
 * @tc.name    TestSetInt641
 * @tc.desc    SetInt64
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetInt641, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    int32_t fence = 0;
    uint64_t key = 1;
    int64_t value = 1;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(buffer->SetInt64(key, value), SURFACE_ERROR_OK);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_13800
 * @tc.name    TestSetInt642
 * @tc.desc    SetInt64
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetInt642, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    uint64_t key = 1;
    int64_t value = 1;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();
    buffer->SetInt64(key, value);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_13900
 * @tc.name    TestSetInt643
 * @tc.desc    SetInt64
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestSetInt643, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = 0;
    uint64_t key = 1;
    int64_t value = 1;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        ASSERT_EQ(buffer->SetInt64(key, value), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_14000
 * @tc.name    TestGetInt641
 * @tc.desc    GetInt64
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetInt641, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    uint64_t key = 1;
    int64_t value = 1;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    buffer->SetInt64(key, value);
    ASSERT_EQ(buffer->GetInt64(key, value), SURFACE_ERROR_OK);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_14100
 * @tc.name    TestGetInt642
 * @tc.desc    GetInt64
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetInt642, testing::ext::TestSize.Level1) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr ;
    uint64_t key = 10000;
    int64_t value = 10000;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    ASSERT_EQ(buffer->GetInt64(key, value), SURFACE_ERROR_NO_ENTRY);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_14200
 * @tc.name    TestGetInt643
 * @tc.desc    GetInt64
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetInt643, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    uint64_t key = 1;
    int64_t value = 1;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    buffer->SetInt64(key, value);
    auto startTime = GetNowTime();
    buffer->GetInt64(key, value);
    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_SURFACE_IT_TEST_14300
 * @tc.name    TestGetInt644
 * @tc.desc    GetInt64
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(SurfaceITest, TestGetInt644, testing::ext::TestSize.Level2) {
    sptr<IBufferConsumerListener> consumerListener = new BufferConsumerListener();
    consumeSurface->RegisterConsumerListener(consumerListener);
    sptr<SurfaceBuffer> buffer = nullptr;
    uint64_t key = 1;
    int64_t value = 1;
    int32_t fence = 0;
    producerSurface->RequestBuffer(buffer, fence, requestConfig);
    auto startTime = GetNowTime();

    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        buffer->SetInt64(key, value);
        ASSERT_EQ(buffer->GetInt64(key, value), SURFACE_ERROR_OK);
    }

    auto endTime = GetNowTime();
    ASSERT_LT(endTime - startTime, OneFrameTime * PERFORMANCE_COUNT);
    producerSurface->CancelBuffer(buffer);
    consumeSurface->UnregisterConsumerListener();
}
}