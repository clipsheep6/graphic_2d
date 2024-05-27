/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "drawable/rs_surface_render_node_drawable.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_render_thread_params.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {

constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class RSSurfaceRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSSurfaceRenderNode> renderNode_;
    RSRenderNodeDrawableAdapter* drawable_;
    RSSurfaceRenderNodeDrawable* surfaceDrawable_;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
};

void RSSurfaceRenderNodeDrawableTest::SetUpTestCase() {}
void RSSurfaceRenderNodeDrawableTest::TearDownTestCase() {}
void RSSurfaceRenderNodeDrawableTest::SetUp()
{
    renderNode_ = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID);
    drawable_ = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode_);

    if (drawable_) {
        drawable_->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID);
        surfaceDrawable_ = static_cast<RSSurfaceRenderNodeDrawable*>(drawable_);
    }
    if (!drawable_->renderParams_) {
        std::cout<<"RSDisplayRenderNodeDrawableTest: failed to init render params."<<std::endl;
    }
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
}
void RSSurfaceRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateSurfaceRenderNodeDrawableTest
 * @tc.desc: Test If SurfaceRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CreateSurfaceRenderNodeDrawable, TestSize.Level1)
{
    NodeId id = 1;
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: FindInstanceChildOfDisplay
 * @tc.desc: Test FindInstanceChildOfDisplay, early return case
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, FindInstanceChildOfDisplay001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_EQ(surfaceDrawable_->FindInstanceChildOfDisplay(nullptr), INVALID_NODEID);

    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_EQ(surfaceDrawable_->FindInstanceChildOfDisplay(surfaceNode), INVALID_NODEID);
}

/**
 * @tc.name: FindInstanceChildOfDisplay
 * @tc.desc: Test FindInstanceChildOfDisplay, if surface parent is display
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, FindInstanceChildOfDisplay002, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    NodeId surfaceId = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceId);
    displayNode->AddChild(surfaceNode);

    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_EQ(surfaceDrawable_->FindInstanceChildOfDisplay(surfaceNode), surfaceId);
}

/**
 * @tc.name: CacheImgForCapture
 * @tc.desc: Test CacheImgForCapture
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CacheImgForCapture, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);

    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->CacheImgForCapture(*canvas_, displayNode);
}

/**
 * @tc.name: OnDraw
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnDraw, TestSize.Level1)
{
    // default case, shouldpaint == false
    ASSERT_NE(drawable_->renderParams_, nullptr);
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->OnDraw(*drawingCanvas_);
    // if should paint
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    surfaceDrawable_->OnDraw(*drawingCanvas_);
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, default case, empty dirty region
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface001, TestSize.Level1)
{
    
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(uniParams.get(), surfaceParams, renderNode_, region);
    ASSERT_TRUE(region.IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface002, TestSize.Level1)
{
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->isMainWindowType_ = true;
    surfaceParams->visibleRegion_ = Occlusion::Rect{0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE};
    surfaceParams->transparentRegion_ = Occlusion::Rect{0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE};
    uniParams->accumulatedDirtyRegion_ = Occlusion::Rect{0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE};
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(uniParams.get(), surfaceParams, renderNode_, region);
    ASSERT_FALSE(region.IsEmpty());
}

/**
 * @tc.name: OnCapture
 * @tc.desc: Test OnCapture
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnCapture, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    // default, should not paint
    surfaceDrawable_->OnCapture(*drawingCanvas_);
    // should paint
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    surfaceDrawable_->OnCapture(*drawingCanvas_);
}

/**
 * @tc.name: EnableRecordingOptimization
 * @tc.desc: Test EnableRecordingOptimization
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, EnableRecordingOptimization, TestSize.Level1)
{
    ASSERT_FALSE(surfaceDrawable_->EnableRecordingOptimization(*(drawable_->renderParams_)));
}

/**
 * @tc.name: CaptureSurface
 * @tc.desc: Test CaptureSurface, default case
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface001, TestSize.Level1)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->CaptureSurface(*renderNode_, *canvas_, *surfaceParams);
}

/**
 * @tc.name: CaptureSurface
 * @tc.desc: Test CaptureSurface, special case: security/protected layer.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface002, TestSize.Level1)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->isSecurityLayer_ = true;
    surfaceDrawable_->CaptureSurface(*renderNode_, *canvas_, *surfaceParams);
    surfaceParams->isSecurityLayer_ = false;
    surfaceParams->isProtectedLayer_ = true;
    surfaceDrawable_->CaptureSurface(*renderNode_, *canvas_, *surfaceParams);
}
}
