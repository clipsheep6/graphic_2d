/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_render_engine.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseRenderEngineUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBaseRenderEngineUnitTest::SetUpTestCase() {}
void RSBaseRenderEngineUnitTest::TearDownTestCase() {}
void RSBaseRenderEngineUnitTest::SetUp() {}
void RSBaseRenderEngineUnitTest::TearDown() {}

/**
 * @tc.name: SetHighContrast_001
 * @tc.desc: Test SetHighContrast, input false, expect RSBaseRenderEngine::IsHighContrastEnabled() to be same as input
 * @tc.type: FUNC
 * @tc.require: issueI6GJ1Z
 */
HWTEST(RSBaseRenderEngineUnitTest, SetHighContrast_001, TestSize.Level1)
{
    bool contrastEnabled = false;
    RSBaseRenderEngine::SetHighContrast(contrastEnabled);
    ASSERT_EQ(contrastEnabled, RSBaseRenderEngine::IsHighContrastEnabled());
}

/**
 * @tc.name: SetHighContrast_002
 * @tc.desc: Test SetHighContrast, input true, expect RSBaseRenderEngine::IsHighContrastEnabled() to be same as input
 * @tc.type: FUNC
 * @tc.require: issueI6GJ1Z
 */
HWTEST(RSBaseRenderEngineUnitTest, SetHighContrast_002, TestSize.Level1)
{
    bool contrastEnabled = true;
    RSBaseRenderEngine::SetHighContrast(contrastEnabled);
    ASSERT_EQ(contrastEnabled, RSBaseRenderEngine::IsHighContrastEnabled());
}

/**
 * @tc.name: NeedForceCPU001
 * @tc.desc: Test NeedForceCPU
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineUnitTest, NeedForceCPU001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = node->GetBuffer();

    std::vector<LayerInfoPtr> layers;
    layers.emplace_back(nullptr);
    bool ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(false, ret);

    layers.clear();
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layers.emplace_back(layer);
    ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: NeedForceCPU002
 * @tc.desc: Test NeedForceCPU
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineUnitTest, NeedForceCPU002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = node->GetBuffer();

    std::vector<LayerInfoPtr> layers;
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetBuffer(buffer, node->GetAcquireFence());
    layers.emplace_back(layer);
    bool ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(false, ret);

    buffer->SetSurfaceBufferColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_NATIVE);
    ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(true, ret);

#ifndef RS_ENABLE_EGLIMAGE
    buffer->SetSurfaceBufferColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    buffer->GetBufferHandle()->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP;
    ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(true, ret);

    buffer->GetBufferHandle()->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCRCB_420_SP;
    ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(true, ret);
#endif
}

/**
 * @tc.name: DrawDisplayNodeWithParams001
 * @tc.desc: Test DrawDisplayNodeWithParams
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineUnitTest, DrawDisplayNodeWithParams001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
    ASSERT_NE(canvas, nullptr);
    BufferDrawParam param;
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->DrawDisplayNodeWithParams(*canvas, *node, param);

    param.useCPU = true;
    renderEngine->DrawDisplayNodeWithParams(*canvas, *node, param);
}

/**
 * @tc.name: ShrinkCachesIfNeeded001
 * @tc.desc: Test ShrinkCachesIfNeeded
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineUnitTest, ShrinkCachesIfNeeded001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->ShrinkCachesIfNeeded(true);
    renderEngine->ShrinkCachesIfNeeded(false);
    renderEngine->Init();
    renderEngine->ShrinkCachesIfNeeded(true);
    renderEngine->ShrinkCachesIfNeeded(false);

    for (int i = 0; i <= static_cast<int>(renderEngine->MAX_RS_SURFACE_SIZE); i++) {
        renderEngine->rsSurfaces_[i] = nullptr;
    }
    renderEngine->ShrinkCachesIfNeeded();
    ASSERT_EQ(renderEngine->rsSurfaces_.size(), renderEngine->MAX_RS_SURFACE_SIZE);
}

/**
 * @tc.name: UnMapRsSurface001
 * @tc.desc: Test UnMapRsSurface
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineUnitTest, UnMapRsSurface001, TestSize.Level1)
{
    uint64_t id = 0;
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();

    renderEngine->UnMapRsSurface(0);

    for (int i = 0; i <= static_cast<int>(renderEngine->MAX_RS_SURFACE_SIZE); i++) {
        renderEngine->rsSurfaces_[i] = nullptr;
    }
    renderEngine->ShrinkCachesIfNeeded();
    renderEngine->UnMapRsSurface(0);
}

#ifdef RS_ENABLE_EGLIMAGE
/**
 * @tc.name: CreateEglImageFromBuffer001
 * @tc.desc: Test CreateEglImageFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineUnitTest, CreateEglImageFromBuffer001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
    auto img = renderEngine->CreateEglImageFromBuffer(*canvas, nullptr, nullptr);
    ASSERT_EQ(nullptr, img);
#ifdef NEW_SKIA
    [[maybe_unused]] auto grContext = canvas->recordingContext();
#else
    [[maybe_unused]] auto grContext = canvas->getGrContext();
#endif
    grContext = nullptr;
    img = renderEngine->CreateEglImageFromBuffer(*canvas, node->GetBuffer(), nullptr);
    ASSERT_EQ(nullptr, img);
}

/**
 * @tc.name: RegisterDeleteBufferListener001
 * @tc.desc: Test RegisterDeleteBufferListener
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineUnitTest, RegisterDeleteBufferListener001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->RegisterDeleteBufferListener(nullptr, true);
    renderEngine->RegisterDeleteBufferListener(nullptr, false);

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    renderEngine->RegisterDeleteBufferListener(node->GetConsumer(), true);
    renderEngine->RegisterDeleteBufferListener(node->GetConsumer(), false);
}
#endif

/**
 * @tc.name: RequestFrame001
 * @tc.desc: Test RequestFrame
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineUnitTest, RequestFrame001, TestSize.Level1)
{
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurfaceOhos> surfaceOhos = nullptr;
#else
    std::shared_ptr<RSSurfaceOhos> surfaceOhos = nullptr;
#endif
    BufferRequestConfig config;
    ASSERT_EQ(nullptr, RSRenderEngine::RequestFrame(surfaceOhos, config, false, false));
}

/**
 * @tc.name: RequestFrame002
 * @tc.desc: Test RequestFrame(sptr, BufferRequestConfig, bool, bool)
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineUnitTest, RequestFrame002, TestSize.Level1)
{
    BufferRequestConfig config;
    ASSERT_EQ(nullptr, RSRenderEngine::RequestFrame(nullptr, config, false, false));
}

/**
 * @tc.name: SetUiTimeStamp001
 * @tc.desc: Test SetUiTimeStamp
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineUnitTest, SetUiTimeStamp001, TestSize.Level1)
{
    u_int64_t surfaceId = 0;
    RSRenderEngine::SetUiTimeStamp(nullptr, surfaceId);
}
}
