/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <memory>
#include <vector>

#include "include/core/SkSurface.h"

#include "iconsumer_surface.h"
#include "utils/log.h"
#include "rs_trace.h"
#include "window.h"
#if !defined(NEW_SKIA)
#include "memory/rs_tag_tracker.h"
#endif

#include "rs_surface_factory.h"
#include "render_context_factory.h"
#include "ohos/rs_render_surface_ohos.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const int32_t INVALID_WIDTH = 720;
const int32_t INVALID_HEIGHT = 1280;
class RSRenderSurfaceOhosTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RSRenderSurfaceOhosTest" };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RSRenderSuface> renderSurfaceOhos;
    static std::shared_ptr<DrawingContext> drawingContext;
    static std::shared_ptr<RSRenderSurfaceFrame> frame;
    static std::shared_ptr<RenderContextBase> renderContext;
};

void RSRenderSurfaceOhosTest::SetUpTestCase()
{
    sptr<IConsumerSurface> csurf = IConsumerSurface::Create();
    sptr<IBufferProducer> producer = csurf->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    PlatformName platformName = PlatformName::OHOS;
    drawingContext = std::make_shared<DrawingContext>();
    renderContext = RenderContextFactory::CreateRenderContext();
    renderSurfaceOhos = RSSurfaceFactory::CreateRSSurface(platformName, pSurface, nullptr);
    frame = renderSurfaceOhos->GetRSRenderSurfaceFrame();
}

void RSRenderSurfaceOhosTest::TearDownTestCase()
{
    drawingContext = nullptr;
    renderContext = nullptr;
    frame = nullptr;
    renderSurfaceOhos = nullptr;
}

void RSRenderSurfaceOhosTest::SetUp()
{
    renderSurfaceOhos->SetDrawingContext(drawingContext);
    renderSurfaceOhos->SetRSRenderSurfaceFrame(frame);
    renderSurfaceOhos->SetRenderContext(renderContext);
}

void RSRenderSurfaceOhosTest::TearDown() {}

/**
 * @tc.name: IsValidTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, IsValidTest001, Function | SmallTest | Level2)
{
    bool res = renderSurfaceOhos->IsValid();
    ASSERT_TRUE(res);
}

/**
 * @tc.name: IsValidTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, IsValidTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRSRenderSurfaceFrame(nullptr);
    bool res = renderSurfaceOhos->IsValid();
    ASSERT_FALSE(res);
}

/**
 * @tc.name: GetSurfaceOhosTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetSurfaceOhosTest001, Function | SmallTest | Level2)
{
    sptr<Surface> surface = renderSurfaceOhos->GetSurfaceOhos();
    ASSERT_NE(surface, nullptr);
}

/**
 * @tc.name: GetSurfaceOhosTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetSurfaceOhosTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRSRenderSurfaceFrame(nullptr);
    sptr<Surface> surface = renderSurfaceOhos->GetSurfaceOhos();
    ASSERT_EQ(surface, nullptr);
}

/**
 * @tc.name: GetQueueSizeTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetQueueSizeTest001, Function | SmallTest | Level2)
{
    uint32_t queueSize = renderSurfaceOhos->GetQueueSize();
    ASSERT_NE(queueSize, 0);
}

/**
 * @tc.name: GetQueueSizeTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetQueueSizeTest002, Function | SmallTest | Level2)
{
    uint32_t queueSize = renderSurfaceOhos->GetQueueSize();
    ASSERT_EQ(queueSize, 0);
}

/**
 * @tc.name: RequestFrameTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, RequestFrameTest001, Function | SmallTest | Level2)
{
    std::shared_ptr<RenderSurfaceFrame> frameTemp = renderSurfaceOhos->RequestFrame();
    ASSERT_NE(frameTemp, nullptr);
}

/**
 * @tc.name: RequestFrameTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, RequestFrameTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRenderContext(nullptr);
    std::shared_ptr<RenderSurfaceFrame> frameTemp = renderSurfaceOhos->RequestFrame();
    ASSERT_EQ(frameTemp, nullptr);
}

/**
 * @tc.name: RequestFrameTest003
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, RequestFrameTest003, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRSRenderSurfaceFrame(nullptr);
    std::shared_ptr<RenderSurfaceFrame> frameTemp = renderSurfaceOhos->RequestFrame();
    ASSERT_EQ(frameTemp, nullptr);
}

/**
 * @tc.name: FlushFrameTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, FlushFrameTest001, Function | SmallTest | Level2)
{
    bool res = renderSurfaceOhos->FlushFrame();
    ASSERT_TRUE(res);
}

/**
 * @tc.name: FlushFrameTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, FlushFrameTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRenderContext(nullptr);
    std::shared_ptr<RenderSurfaceFrame> frame = renderSurfaceOhos->FlushFrame();
    ASSERT_FALSE(res);
}

/**
 * @tc.name: SetDamageRegionTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetDamageRegionTest001, Function | SmallTest | Level2)
{
    std::vector<RectI> damageRects;
    RectI rect = {0, 0, 100, 200};
    damageRects.push_back(rect);
    renderSurfaceOhos->SetDamageRegion(damageRects);
    ASSERT_EQ(frame->damageRects.size(), 1);
}

/**
 * @tc.name: SetDamageRegionTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetDamageRegionTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRenderContext(nullptr);
    std::vector<RectI> damageRects;
    RectI rect = {0, 0, 100, 200};
    damageRects.push_back(rect);
    renderSurfaceOhos->SetDamageRegion(damageRects);
    ASSERT_EQ(frame->damageRects.size(), 0);
}

/**
 * @tc.name: SetDamageRegionTest003
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetDamageRegionTest003, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRSRenderSurfaceFrame(nullptr);
    std::vector<RectI> damageRects;
    RectI rect = {0, 0, 100, 200};
    damageRects.push_back(rect);
    renderSurfaceOhos->SetDamageRegion(damageRects);
    ASSERT_EQ(frame->damageRects.size(), 0);
}

/**
 * @tc.name: GetSurfaceTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetSurfaceTest001, Function | SmallTest | Level2)
{
    sk_sp<SkSurface> skSurface = renderSurfaceOhos->GetSurface();
    ASSERT_NE(skSurface, nullptr);
}

/**
 * @tc.name: GetSurfaceTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetSurfaceTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRSRenderSurfaceFrame(nullptr);
    sk_sp<SkSurface> skSurface = renderSurfaceOhos->GetSurface();
    ASSERT_EQ(skSurface, nullptr);
}

/**
 * @tc.name: GetSurfaceTest003
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetSurfaceTest003, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetDrawingContext(nullptr);
    sk_sp<SkSurface> skSurface = renderSurfaceOhos->GetSurface();
    ASSERT_EQ(skSurface, nullptr);
}

/**
 * @tc.name: GetColorSpaceTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetColorSpaceTest001, Function | SmallTest | Level2)
{
    ColorGamut colorSpace = renderSurfaceOhos->GetColorSpace();
    ASSERT_NE(colorSpace, nullptr);
}

/**
 * @tc.name: GetColorSpaceTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetColorSpaceTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRSRenderSurfaceFrame(nullptr);
    ColorGamut colorSpace = renderSurfaceOhos->GetColorSpace();
    ASSERT_EQ(colorSpace, nullptr);
}

/**
 * @tc.name: SetColorSpaceTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetColorSpaceTest001, Function | SmallTest | Level2)
{
    ColorGamut colorSpace = ColorGamut::COLOR_GAMUT_DISPLAY_P3;
    renderSurfaceOhos->SetColorSpace(colorSpace);
    ASSERT_EQ(colorSpace, renderSurfaceOhos->GetColorSpace());
}

/**
 * @tc.name: SetColorSpaceTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetColorSpaceTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRSRenderSurfaceFrame(nullptr);
    ColorGamut colorSpace = ColorGamut::COLOR_GAMUT_DISPLAY_P3;
    renderSurfaceOhos->SetColorSpace(colorSpace);
    ASSERT_NE(colorSpace, renderSurfaceOhos->GetColorSpace());
}

/**
 * @tc.name: SetSurfaceBufferUsageTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetSurfaceBufferUsageTest001, Function | SmallTest | Level2)
{
    uint64_t bufferUsage = BUFFER_USAGE_CPU_READ;
    renderSurfaceOhos->SetSurfaceBufferUsage(bufferUsage);
    ASSERT_EQ(bufferUsage, frame->bufferUsage);
}

/**
 * @tc.name: SetSurfaceBufferUsageTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetSurfaceBufferUsageTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRSRenderSurfaceFrame(nullptr);
    uint64_t bufferUsage = BUFFER_USAGE_CPU_READ;
    renderSurfaceOhos->SetSurfaceBufferUsage(bufferUsage);
    ASSERT_NE(bufferUsage, frame->bufferUsage);
}

/**
 * @tc.name: SetSurfacePixelFormatTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetSurfacePixelFormatTest001, Function | SmallTest | Level2)
{
    uint64_t pixelFormat = PIXEL_FMT_RGB_555;
    renderSurfaceOhos->SetSurfacePixelFormat(pixelFormat);
    ASSERT_EQ(pixelFormat, frame->pixelFormat);
}

/**
 * @tc.name: SetSurfacePixelFormatTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetSurfacePixelFormatTest002, Function | SmallTest | Level2)
{
    uint64_t pixelFormat = PIXEL_FMT_RGB_555;
    renderSurfaceOhos->SetSurfacePixelFormat(pixelFormat);
    ASSERT_NE(pixelFormat, frame->pixelFormat);
}

/**
 * @tc.name: GetReleaseFenceTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetReleaseFenceTest001, Function | SmallTest | Level2)
{
    int32_t fence = renderSurfaceOhos->GetReleaseFence();
    ASSERT_NE(fence, -1);
}

/**
 * @tc.name: GetReleaseFenceTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, GetReleaseFenceTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRSRenderSurfaceFrame(nullptr);
    int32_t fence = renderSurfaceOhos->GetReleaseFence();
    ASSERT_EQ(fence, -1);
}

/**
 * @tc.name: SetReleaseFenceTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetReleaseFenceTest001, Function | SmallTest | Level2)
{
    int32_t fence = 1;
    renderSurfaceOhos->SetReleaseFence(fence);
    ASSERT_EQ(fence, renderSurfaceOhos->GetReleaseFence());
}

/**
 * @tc.name: SetReleaseFenceTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSurfaceOhosTest, SetColorSpaceTest002, Function | SmallTest | Level2)
{
    renderSurfaceOhos->SetRSRenderSurfaceFrame(nullptr);
    int32_t fence = 1;
    renderSurfaceOhos->SetReleaseFence(fence);
    ASSERT_NE(fence, renderSurfaceOhos->GetColorSpace());
}
}
}