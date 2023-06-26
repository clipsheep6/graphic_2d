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
#include "pipeline/rs_uni_render_composer_adapter.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniRenderComposerAdapterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

public:
    const uint32_t screenId_ = 0;
    const int32_t offsetX = 0; // screenOffset on x axis equals to 0
    const int32_t offsetY = 0; // screenOffset on y axis equals to 0
    const float mirrorAdaptiveCoefficient = 1.0f;

    sptr<RSScreenManager> screenManager_;
    std::unique_ptr<RSUniRenderComposerAdapter> composerAdapter_;
};

void RSUniRenderComposerAdapterTest::SetUpTestCase() {}
void RSUniRenderComposerAdapterTest::TearDownTestCase() {}
void RSUniRenderComposerAdapterTest::TearDown() {}
void RSUniRenderComposerAdapterTest::SetUp()
{
    screenManager_ = CreateOrGetScreenManager();
    ASSERT_NE(screenManager_, nullptr);
    screenManager_->Init();
    uint32_t width = 2560;
    uint32_t height = 1080;
    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ScreenState state = ScreenState::UNKNOWN;
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    std::unique_ptr<impl::RSScreen> rsScreen =
        std::make_unique<impl::RSScreen>(screenId_, false, HdiOutput::CreateHdiOutput(screenId_), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    screenManager_->MockHdiScreenConnected(rsScreen);
    auto info = screenManager_->QueryScreenInfo(screenId_);
    info.width = width;
    info.height = height;
    info.colorGamut = colorGamut;
    info.state = state;
    info.rotation = rotation;
    composerAdapter_ = std::make_unique<RSUniRenderComposerAdapter>();
    ASSERT_NE(composerAdapter_, nullptr);
    composerAdapter_->Init(info, offsetX, offsetY, mirrorAdaptiveCoefficient);
}

/**
 * @tc.name: Start001
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CommitLayers
 * @tc.type: FUNC
 * @tc.require: issueI6S774
 */
HWTEST_F(RSUniRenderComposerAdapterTest, Start001, TestSize.Level1)
{
    SetUp();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, 400, 600};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto layer1 = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer1, nullptr);
    std::vector<LayerInfoPtr> layers;
    layers.emplace_back(layer1);
    composerAdapter_->CommitLayers(layers);
    auto hdiBackend = composerAdapter_->hdiBackend_;
    composerAdapter_->hdiBackend_ = nullptr;
    composerAdapter_->CommitLayers(layers);
    composerAdapter_->hdiBackend_ = hdiBackend;
    composerAdapter_->output_ = nullptr;
    composerAdapter_->CommitLayers(layers);
}

/**
 * @tc.name: Start002
 * @tc.desc: Test RSUniRenderComposerAdapterTest.BuildComposeInfo
 * @tc.type: FUNC
 * @tc.require: issueI7FUVJ
 */
HWTEST_F(RSUniRenderComposerAdapterTest, Start002, TestSize.Level1)
{
    SetUp();
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);

    csurf->SetTransform(GRAPHIC_ROTATE_270);
    composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    surfaceNode->SetConsumer(csurf);
    composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    csurf->SetTransform(GRAPHIC_ROTATE_180);
    composerAdapter_->SrcRectRotateTransform(*surfaceNode);
    csurf->SetTransform(GRAPHIC_ROTATE_90);
    composerAdapter_->SrcRectRotateTransform(*surfaceNode);

    RectI dstRect{0, 0, 400, 600};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    auto layer = composerAdapter_->CreateLayer(*surfaceNode);
    ASSERT_NE(layer, nullptr);
}

/**
 * @tc.name: Start003
 * @tc.desc: Test RSUniRenderComposerAdapterTest.CheckStatusBeforeCreateLayer
 * @tc.type: FUNC
 * @tc.require: issueI7FUVJ
 */
HWTEST_F(RSUniRenderComposerAdapterTest, Start003, TestSize.Level1)
{
    SetUp();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    RectI dstRect{0, 0, 400, 600};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);
    composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode);
    auto layer1 = composerAdapter_->CreateLayer(*surfaceNode);
}

/**
 * @tc.name: Start004
 * @tc.desc: Test RSUniRenderComposerAdapterTest.LayerScaleDown
 * @tc.type: FUNC
 * @tc.require: issueI7FUVJ
 */
HWTEST_F(RSUniRenderComposerAdapterTest, Start004, TestSize.Level1)
{
    SetUp();
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    RectI dstRect{0, 0, 400, 600};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);

    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);
    surfaceNode->SetConsumer(csurf);

    composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    auto screenInfo = screenManager_->QueryScreenInfo(screenId_);

    screenInfo.rotation = ScreenRotation::ROTATION_90;
    composerAdapter_->IsOutOfScreenRegion(info);
    screenInfo.rotation = ScreenRotation::ROTATION_180;
    composerAdapter_->IsOutOfScreenRegion(info);
    screenInfo.rotation = ScreenRotation::ROTATION_270;
    composerAdapter_->IsOutOfScreenRegion(info);

    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    composerAdapter_->SetComposeInfoToLayer(layer, info, surfaceNode->GetConsumer(), &(*surfaceNode));

    composerAdapter_->LayerRotate(layer, *surfaceNode);
    composerAdapter_->LayerCrop(layer);
    composerAdapter_->LayerScaleDown(layer, *surfaceNode);
}

/**
 * @tc.name: Start005
 * @tc.desc: Test RSUniRenderComposerAdapterTest.DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issueI7FUVJ
 */
HWTEST_F(RSUniRenderComposerAdapterTest, Start005, TestSize.Level1)
{
    SetUp();
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    RectI dstRect{0, 0, 400, 600};
    surfaceNode->SetSrcRect(dstRect);
    surfaceNode->SetDstRect(dstRect);

    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);
    surfaceNode->SetConsumer(csurf);

    composerAdapter_->CheckStatusBeforeCreateLayer(*surfaceNode);
    ComposeInfo info = composerAdapter_->BuildComposeInfo(*surfaceNode);
    composerAdapter_->DealWithNodeGravity(*surfaceNode, info);
}

} // namespace