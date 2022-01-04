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
#include <native_window.h>
#include <display_type.h>
#include <surface_type.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};

class NativeWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {};
    static inline BufferFlushConfig flushConfig = {};
    static inline sptr<OHOS::Surface> cSurface = nullptr;
    static inline sptr<OHOS::IBufferProducer> producer = nullptr;
    static inline sptr<OHOS::Surface> pSurface = nullptr;
    static inline sptr<OHOS::SurfaceBuffer> sBuffer = nullptr;
    static inline NativeWindow* nativeWindow = nullptr;
    static inline NativeWindowBuffer* nativeWindowBuffer = nullptr;
};

void NativeWindowTest::SetUpTestCase()
{
    requestConfig = {
        .width = 0x100,  // small
        .height = 0x100, // small
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };

    cSurface = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    producer = cSurface->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    int32_t fence;
    pSurface->RequestBuffer(sBuffer, fence, requestConfig);
}

void NativeWindowTest::TearDownTestCase()
{
    flushConfig = { .damage = {
        .w = 0x100,
        .h = 0x100,
    } };
    pSurface->FlushBuffer(sBuffer, -1, flushConfig);
    sBuffer = nullptr;
    cSurface = nullptr;
    producer = nullptr;
    pSurface = nullptr;
    nativeWindow = nullptr;
    nativeWindowBuffer = nullptr;
}

/**
 * @tc.name: CreateNativeWindow001
 * @tc.desc: Verify the CreateNativeWindowFromSurface of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, CreateNativeWindow001, Function | MediumTest| Level1)
{
    ASSERT_EQ(CreateNativeWindowFromSurface(nullptr), nullptr);
}

/**
 * @tc.name: CreateNativeWindow002
 * @tc.desc: Verify the CreateNativeWindowFromSurface of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, CreateNativeWindow002, Function | MediumTest| Level1)
{
    nativeWindow = CreateNativeWindowFromSurface(&pSurface);
    ASSERT_NE(nativeWindow, nullptr);
}

/**
 * @tc.name: HandleOpt001
 * @tc.desc: Verify the NativeWindowHandleOpt of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, HandleOpt001, Function | MediumTest| Level1)
{
    int code = SET_USAGE;
    int32_t usage = HBM_USE_CPU_READ;
    ASSERT_EQ(NativeWindowHandleOpt(nullptr, code, usage), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/**
 * @tc.name: HandleOpt002
 * @tc.desc: Verify the NativeWindowHandleOpt of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, HandleOpt002, Function | MediumTest| Level3)
{
    int code = SET_USAGE;
    int32_t usage = HBM_USE_CPU_READ;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, usage), OHOS::GSERROR_OK);

    code = GET_USAGE;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, &usage), OHOS::GSERROR_OK);
}

/**
 * @tc.name: HandleOpt003
 * @tc.desc: Verify the NativeWindowHandleOpt of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, HandleOpt003, Function | MediumTest| Level3)
{
    int code = SET_BUFFER_GEOMETRY;
    int32_t height = 0x100;
    int32_t width = 0x100;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, height, width), OHOS::GSERROR_OK);

    code = GET_BUFFER_GEOMETRY;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, &height, &width), OHOS::GSERROR_OK);
}

/**
 * @tc.name: HandleOpt004
 * @tc.desc: Verify the NativeWindowHandleOpt of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, HandleOpt004, Function | MediumTest| Level3)
{
    int code = SET_FORMAT;
    int32_t format = PIXEL_FMT_RGBA_8888;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, format), OHOS::GSERROR_OK);

    code = GET_FORMAT;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, &format), OHOS::GSERROR_OK);
}

/**
 * @tc.name: HandleOpt005
 * @tc.desc: Verify the NativeWindowHandleOpt of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, HandleOpt005, Function | MediumTest| Level3)
{
    int code = SET_STRIDE;
    int32_t stride = 0x8;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, stride), OHOS::GSERROR_OK);

    code = GET_STRIDE;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, &stride), OHOS::GSERROR_OK);
}

/**
 * @tc.name: CreateNativeWindowBuffer001
 * @tc.desc: Verify the CreateNativeWindowBufferFromSurfaceBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, CreateNativeWindowBuffer001, Function | MediumTest| Level1)
{
    ASSERT_EQ(CreateNativeWindowBufferFromSurfaceBuffer(nullptr), nullptr);
}

/**
 * @tc.name: CreateNativeWindowBuffer002
 * @tc.desc: Verify the CreateNativeWindowBufferFromSurfaceBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, CreateNativeWindowBuffer002, Function | MediumTest| Level1)
{
    nativeWindowBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer);
    ASSERT_NE(nativeWindowBuffer, nullptr);
}

/**
 * @tc.name: RequestBuffer001
 * @tc.desc: Verify the NativeWindowRequestBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, RequestBuffer001, Function | MediumTest| Level1)
{
    ASSERT_EQ(NativeWindowRequestBuffer(nullptr, &nativeWindowBuffer, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/**
 * @tc.name: RequestBuffer002
 * @tc.desc: Verify the NativeWindowRequestBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, RequestBuffer002, Function | MediumTest| Level1)
{
    ASSERT_EQ(NativeWindowRequestBuffer(nativeWindow, nullptr, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/**
 * @tc.name: RequestBuffer003
 * @tc.desc: Verify the NativeWindowRequestBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, RequestBuffer003, Function | MediumTest| Level3)
{
    int fenceFd = -1;
    ASSERT_EQ(NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd), OHOS::GSERROR_OK);
}

/**
 * @tc.name: GetBufferHandle001
 * @tc.desc: Verify the GetBufferHandleFromNative of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, GetBufferHandle001, Function | MediumTest| Level1)
{
    ASSERT_EQ(GetBufferHandleFromNative(nullptr), nullptr);
}

/**
 * @tc.name: GetBufferHandle002
 * @tc.desc: Verify the GetBufferHandleFromNative of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, GetBufferHandle002, Function | MediumTest| Level3)
{
    struct NativeWindowBuffer *buffer = new NativeWindowBuffer();
    buffer->sfbuffer = sBuffer;
    ASSERT_NE(GetBufferHandleFromNative(nativeWindowBuffer), nullptr);
    delete buffer;
}

/**
 * @tc.name: FlushBuffer001
 * @tc.desc: Verify the NativeWindowFlushBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, FlushBuffer001, Function | MediumTest| Level3)
{
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect * rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;

    ASSERT_EQ(NativeWindowFlushBuffer(nullptr, nullptr, fenceFd, *region), OHOS::GSERROR_INVALID_ARGUMENTS);
    delete region;
}

/**
 * @tc.name: FlushBuffer002
 * @tc.desc: Verify the NativeWindowFlushBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, FlushBuffer002, Function | MediumTest| Level3)
{
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect * rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;

    ASSERT_EQ(NativeWindowFlushBuffer(nativeWindow, nullptr, fenceFd, *region), OHOS::GSERROR_INVALID_ARGUMENTS);
    delete region;
}

/**
 * @tc.name: FlushBuffer003
 * @tc.desc: Verify the NativeWindowFlushBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, FlushBuffer003, Function | MediumTest| Level3)
{
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect * rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;

    ASSERT_EQ(NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region), OHOS::GSERROR_OK);
    delete region;
}

/**
 * @tc.name: CancelBuffer001
 * @tc.desc: Verify the NativeWindowCancelBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, CancelBuffer001, Function | MediumTest| Level1)
{
    ASSERT_EQ(NativeWindowCancelBuffer(nullptr, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/**
 * @tc.name: CancelBuffer002
 * @tc.desc: Verify the NativeWindowCancelBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, CancelBuffer002, Function | MediumTest| Level1)
{
    ASSERT_EQ(NativeWindowCancelBuffer(nativeWindow, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/**
 * @tc.name: CancelBuffer003
 * @tc.desc: Verify the NativeWindowCancelBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, CancelBuffer003, Function | MediumTest| Level3)
{
    ASSERT_EQ(NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer), OHOS::GSERROR_OK);
}

/**
 * @tc.name: Reference001
 * @tc.desc: Verify the NativeObjectReference of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, Reference001, Function | MediumTest| Level3)
{
    struct NativeWindowBuffer *buffer = new NativeWindowBuffer();
    buffer->sfbuffer = sBuffer;
    ASSERT_EQ(NativeObjectReference(reinterpret_cast<void *>(buffer)), OHOS::GSERROR_OK);
    delete buffer;
}

/**
 * @tc.name: Unreference001
 * @tc.desc: Verify the NativeObjectUnreference of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, Unreference001, Function | MediumTest| Level3)
{
    struct NativeWindowBuffer *buffer = new NativeWindowBuffer();
    buffer->sfbuffer = sBuffer;
    ASSERT_EQ(NativeObjectUnreference(reinterpret_cast<void *>(buffer)), OHOS::GSERROR_OK);
    delete buffer;
}

/**
 * @tc.name: DestoryNativeWindow001
 * @tc.desc: Verify the DestoryNativeWindow of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, DestoryNativeWindow001, Function | MediumTest| Level1)
{
    DestoryNativeWindow(nullptr);
}

/**
 * @tc.name: DestoryNativeWindow002
 * @tc.desc: Verify the DestoryNativeWindow of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, DestoryNativeWindow002, TestSize.Level0)
{
    DestoryNativeWindow(nativeWindow);
}

/**
 * @tc.name: DestoryNativeWindowBuffer001
 * @tc.desc: Verify the DestoryNativeWindowBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, DestoryNativeWindowBuffer001, TestSize.Level0)
{
    DestoryNativeWindowBuffer(nullptr);
    DestoryNativeWindowBuffer(nativeWindowBuffer);
}

/**
 * @tc.name: DestoryNativeWindowBuffer002
 * @tc.desc: Verify the DestoryNativeWindowBuffer of nativewindow
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(NativeWindowTest, DestoryNativeWindowBuffer002, TestSize.Level0)
{
    DestoryNativeWindowBuffer(nativeWindowBuffer);
}
}
