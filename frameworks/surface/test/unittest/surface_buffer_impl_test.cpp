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
#include <display_type.h>
#include <surface.h>
#include <surface_buffer_impl.h>
#include <buffer_manager.h>
#include <buffer_utils.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class SurfaceBufferImplTest : public testing::Test {
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
    static inline sptr<SurfaceBufferImpl> buffer = nullptr;
    static inline int32_t val32 = 0;
    static inline int64_t val64 = 0;
};

void SurfaceBufferImplTest::SetUpTestCase()
{
    buffer = nullptr;
    val32 = 0;
    val64 = 0;
}

void SurfaceBufferImplTest::TearDownTestCase()
{
    buffer = nullptr;
}

/**
 * @tc.name: NewSeqIncrease001
 * @tc.desc: Verify the new SurfaceBufferImpl Num of SurfaceBufferImpl
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(SurfaceBufferImplTest, NewSeqIncrease001, Function | MediumTest| Level3)
{
    buffer = new SurfaceBufferImpl();
    int oldSeq = buffer->GetSeqNum();

    buffer = new SurfaceBufferImpl();
    ASSERT_EQ(oldSeq + 1, buffer->GetSeqNum());
}

/**
 * @tc.name: State001
 * @tc.desc: Verify the GetBufferHandle, GetVirAddr, GetFileDescriptor and GetSize of SurfaceBufferImpl
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(SurfaceBufferImplTest, State001, Function | MediumTest| Level3)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);
    ASSERT_EQ(buffer->GetVirAddr(), nullptr);
    ASSERT_EQ(buffer->GetFileDescriptor(), -1);
    ASSERT_EQ(buffer->GetSize(), 0u);
}

/**
 * @tc.name: GetterSetter001
 * @tc.desc: Verify the SetInt32 and GetInt32 positive of SurfaceBufferImpl
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter001, Function | MediumTest| Level3)
{
    GSError ret;

    ret = buffer->SetInt32(0, 0x7fffffff);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = buffer->GetInt32(0, val32);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val32, 0x7fffffff);
}

/**
 * @tc.name: GetterSetter002
 * @tc.desc: Verify the SetInt32 and GetInt32 nagative of SurfaceBufferImpl
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter002, Function | MediumTest| Level3)
{
    GSError ret;

    ret = buffer->SetInt32(0, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = buffer->GetInt32(0, val32);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val32, -1);
}

/**
 * @tc.name: GetterSetter003
 * @tc.desc: Verify the SetInt64 and GetInt64 positive of SurfaceBufferImpl
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter003, Function | MediumTest| Level3)
{
    GSError ret;

    ret = buffer->SetInt64(1, 0x7fffffffffLL);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = buffer->GetInt64(1, val64);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val64, 0x7fffffffffLL);
}

/**
 * @tc.name: GetterSetter004
 * @tc.desc: Verify the SetInt64 and GetInt64 nagative of SurfaceBufferImpl
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter004, Function | MediumTest| Level3)
{
    GSError ret;

    ret = buffer->SetInt64(1, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = buffer->GetInt64(1, val64);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val64, -1);
}

/**
 * @tc.name: GetterSetter005
 * @tc.desc: Verify the GetInt64 of SurfaceBufferImpl
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter005, Function | MediumTest| Level3)
{
    GSError ret;

    val64 = 0x123;
    ret = buffer->GetInt64(0, val64);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val64, 0x123);
}

/**
 * @tc.name: GetterSetter006
 * @tc.desc: Verify the GetInt32 of SurfaceBufferImpl
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter006, Function | MediumTest| Level3)
{
    GSError ret;

    val32 = 0x456;
    ret = buffer->GetInt32(1, val32);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val32, 0x456);
}

/**
 * @tc.name: State002
 * @tc.desc: Verify the GetBufferHandle, GetVirAddr, GetFileDescriptor and GetSize of SurfaceBufferImpl
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(SurfaceBufferImplTest, State002, Function | MediumTest| Level3)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);

    GSError ret = BufferManager::GetInstance()->Alloc(requestConfig, buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(buffer->GetVirAddr(), nullptr);
    ASSERT_EQ(buffer->GetFileDescriptor(), -1);
    ASSERT_NE(buffer->GetSize(), 0u);
    ASSERT_NE(buffer->GetBufferHandle(), nullptr);

    ret = BufferManager::GetInstance()->Free(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/**
 * @tc.name: Parcel001
 * @tc.desc: Verify the parcel of SurfaceBufferImpl
 * @tc.type:FUNC
 * @tc.require:AR000GGPA9
 * @tc.author:
 */
HWTEST_F(SurfaceBufferImplTest, Parcel001, Function | MediumTest| Level3)
{
    sptr<SurfaceBufferImpl> sbi = new SurfaceBufferImpl(0);
    const auto &bm = BufferManager::GetInstance();
    auto sret = bm->Alloc(requestConfig, sbi);
    ASSERT_EQ(sret, OHOS::GSERROR_OK);

    sbi->SetInt32(32, 32);
    sbi->SetInt64(64, 64);

    MessageParcel parcel;
    WriteSurfaceBufferImpl(parcel, sbi->GetSeqNum(), sbi);

    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t seq;
    ReadSurfaceBufferImpl(parcel, seq, buffer);
    ASSERT_NE(buffer, nullptr);

    int32_t val32 = 0;
    int64_t val64 = 0;
    ASSERT_EQ(buffer->GetInt32(32, val32), OHOS::GSERROR_OK);
    ASSERT_EQ(buffer->GetInt64(64, val64), OHOS::GSERROR_OK);
}
}
