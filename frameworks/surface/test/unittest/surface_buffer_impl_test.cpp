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
        .timeout = 10,
        .colorGamut = ColorGamut::COLOR_GAMUT_DCI_P3,
        .scalingMode = ScalingMode::SCALING_MODE_SCALE_CROP,
    };
    static inline sptr<SurfaceBuffer> buffer = nullptr;
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

/*
* Function: GetSeqNum
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. new SurfaceBufferImpl and GetSeqNum
*                  2. new SurfaceBufferImpl again and check GetSeqNum = oldSeq + 1
 */
HWTEST_F(SurfaceBufferImplTest, NewSeqIncrease001, Function | MediumTest | Level2)
{
    buffer = new SurfaceBufferImpl();
    int oldSeq = buffer->GetSeqNum();

    buffer = new SurfaceBufferImpl();
    ASSERT_EQ(oldSeq + 1, buffer->GetSeqNum());
}

/*
* Function: check buffer state
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. check buffer state, such as bufferhandle, virAddr, fileDescriptor and size
 */
HWTEST_F(SurfaceBufferImplTest, State001, Function | MediumTest | Level2)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);
    ASSERT_EQ(buffer->GetVirAddr(), nullptr);
    ASSERT_EQ(buffer->GetFileDescriptor(), -1);
    ASSERT_EQ(buffer->GetSize(), 0u);
}

/*
* Function: check buffer state
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandle and Alloc
*                  2. check buffer state, such as bufferhandle, virAddr and size
*                  3. call Free
*                  4. check ret
 */
HWTEST_F(SurfaceBufferImplTest, State002, Function | MediumTest | Level2)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);

    GSError ret = BufferManager::GetInstance()->Alloc(requestConfig, buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(buffer->GetVirAddr(), nullptr);
    ASSERT_NE(buffer->GetSize(), 0u);
    ASSERT_NE(buffer->GetBufferHandle(), nullptr);
    ASSERT_EQ(buffer->GetSurfaceBufferWidth(), 0x100);
    ASSERT_EQ(buffer->GetSurfaceBufferHeight(), 0x100);
    ASSERT_EQ(buffer->GetStride(), 0x8);
    ASSERT_EQ(buffer->GetFormat(), PIXEL_FMT_RGBA_8888);
    ASSERT_EQ(buffer->GetUsage(), HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);
    ASSERT_EQ(buffer->GetSurfaceBufferTimeout(), 10);
    ASSERT_EQ(buffer->GetSurfaceBufferColorGamut(), ColorGamut::COLOR_GAMUT_DCI_P3);
    ASSERT_EQ(buffer->GetSurfaceBufferScalingMode(), ScalingMode::SCALING_MODE_SCALE_CROP);

    ret = BufferManager::GetInstance()->Free(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: parcel
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. new SurfaceBufferImpl and Alloc
*                  2. call Set data interface
*                  3. call WriteSurfaceBufferImpl and ReadSurfaceBufferImpl
*                  4. call Get data interface
*                  5. check ret
 */
HWTEST_F(SurfaceBufferImplTest, Parcel001, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> sbi = new SurfaceBufferImpl(0);
    const auto &bm = BufferManager::GetInstance();
    auto sret = bm->Alloc(requestConfig, sbi);
    ASSERT_EQ(sret, OHOS::GSERROR_OK);

    MessageParcel parcel;
    WriteSurfaceBufferImpl(parcel, sbi->GetSeqNum(), sbi);

    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t seq;
    ReadSurfaceBufferImpl(parcel, seq, buffer);
    ASSERT_NE(buffer, nullptr);
}
}
