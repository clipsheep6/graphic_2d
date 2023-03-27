/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "iconsumer_surface.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceOhosRasterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceOhosRasterTest::SetUpTestCase() {}
void RSSurfaceOhosRasterTest::TearDownTestCase() {}
void RSSurfaceOhosRasterTest::SetUp() {}
void RSSurfaceOhosRasterTest::TearDown() {}

/**
 * @tc.name: SetSurfaceBufferUsage001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosRasterTest, SetSurfaceBufferUsage001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosRaster raster(pSurface);
    uint64_t usage = 1;
    raster.SetSurfaceBufferUsage(usage);
}

/**
 * @tc.name: RequestFrame001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosRasterTest, RequestFrame001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosRaster raster(pSurface);
    int32_t width = 1;
    int32_t height = 1;
    uint64_t uiTimestamp = 1;
    raster.RequestFrame(width, height, uiTimestamp);
}

/**
 * @tc.name: ClearBuffer001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosRasterTest, ClearBuffer001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosRaster raster(pSurface);
    raster.ClearBuffer();
}

/**
 * @tc.name: SetUiTimeStamp001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosRasterTest, SetUiTimeStamp001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosRaster raster(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    uint64_t uiTimestamp = 1;
    raster.SetUiTimeStamp(frame, uiTimestamp);
}

/**
 * @tc.name: SetUiTimeStamp002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosRasterTest, SetUiTimeStamp002, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosRaster raster(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame = std::make_unique<RSSurfaceFrameOhosRaster>(10, 10);
    uint64_t uiTimestamp = 1;
    raster.SetUiTimeStamp(frame, uiTimestamp);
}

/**
 * @tc.name: FlushFrame001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosRasterTest, FlushFrame001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosRaster raster(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    uint64_t uiTimestamp = 1;
    raster.SetUiTimeStamp(frame, uiTimestamp);
    raster.FlushFrame(frame, uiTimestamp);
}

/**
 * @tc.name: FlushFrame002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosRasterTest, FlushFrame002, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosRaster raster(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame = std::make_unique<RSSurfaceFrameOhosRaster>(10, 10);
    uint64_t uiTimestamp = 1;
    raster.SetUiTimeStamp(frame, uiTimestamp);
    raster.FlushFrame(frame, uiTimestamp);
}

/**
 * @tc.name: ResetBufferAge001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosRasterTest, ResetBufferAge001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosRaster raster(pSurface);
    raster.ResetBufferAge();
}
} // namespace Rosen
} // namespace OHOS