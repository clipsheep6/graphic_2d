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

#include "hdi_framebuffer_surface.h"
#include "surface_buffer_impl.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiFramebufferSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<HdiFramebufferSurface> hdiFramebufferSurface_ = nullptr;
};

void HdiFramebufferSurfaceTest::SetUpTestCase()
{
}

void HdiFramebufferSurfaceTest::TearDownTestCase()
{
    hdiFramebufferSurface_ = nullptr;
}

namespace {
/*
* Function: CreateFramebufferSurface001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call CreateFramebufferSurface
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, CreateFramebufferSurface001, Function | MediumTest| Level3)
{
    hdiFramebufferSurface_ = HdiFramebufferSurface::CreateFramebufferSurface(50, 50); // both width and height are 50
    hdiFramebufferSurface_->OnBufferAvailable();
    ASSERT_NE(hdiFramebufferSurface_, nullptr);
}

/*
* Function: GetBufferQueueSize001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferQueueSize
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, GetBufferQueueSize001, Function | MediumTest| Level3)
{
    ASSERT_EQ(hdiFramebufferSurface_->GetBufferQueueSize(), HdiFramebufferSurface::MAX_BUFFER_SIZE);
}

/*
* Function: GetFramebuffer001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetFramebuffer
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, GetFramebuffer001, Function | MediumTest| Level3)
{
    std::unique_ptr<FrameBufferEntry> fbEntry = hdiFramebufferSurface_->GetFramebuffer();
    ASSERT_EQ(fbEntry, nullptr);
}

/*
* Function: ReleaseFramebuffer001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseFramebuffer
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, ReleaseFramebuffer001, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    ASSERT_EQ(hdiFramebufferSurface_->ReleaseFramebuffer(buffer, fence), 0);
}

/*
* Function: GetSurface001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetSurface
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, GetSurface001, Function | MediumTest| Level3)
{
    sptr<OHOS::Surface> producerSurface = hdiFramebufferSurface_->GetSurface();
    ASSERT_NE(producerSurface, nullptr);

    BufferRequestConfig config {};
    config.width = 50; // screen width is 50
    config.height = 50; // screen height is 50
    config.strideAlignment = 0x8;
    config.format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    config.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB;
    sptr<SurfaceBuffer> buffer;
    int32_t fenceFd = -1;
    GSError retCode = producerSurface->RequestBuffer(buffer, fenceFd, config);
    ASSERT_EQ(retCode, GSERROR_OK);

    BufferFlushConfig flushConfig = {};
    flushConfig.damage.w = 50;
    flushConfig.damage.h = 50;
    retCode = producerSurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(retCode, GSERROR_OK);
}

/*
* Function: ReleaseFramebuffer002
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseFramebuffer
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, ReleaseFramebuffer002, Function | MediumTest| Level3)
{
    auto fbEntry = hdiFramebufferSurface_->GetFramebuffer();
    ASSERT_NE(fbEntry, nullptr);
    sptr<SurfaceBuffer> buffer = fbEntry->buffer;
    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    int32_t ret = hdiFramebufferSurface_->ReleaseFramebuffer(buffer, releaseFence);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: Dump001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call Dump
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, Dump001, Function | MediumTest| Level3)
{
    std::string dumpInfo = "";
    hdiFramebufferSurface_->Dump(dumpInfo);
    ASSERT_NE(dumpInfo.size(), 0);
}
}
} // namespace Rosen
} // namespace OHOS