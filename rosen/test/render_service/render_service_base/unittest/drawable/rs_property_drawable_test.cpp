/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "common/rs_obj_geometry.h"
#include "drawable/rs_property_drawable.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyDrawableTest::SetUpTestCase() {}
void RSPropertyDrawableTest::TearDownTestCase() {}
void RSPropertyDrawableTest::SetUp() {}
void RSPropertyDrawableTest::TearDown() {}

/**
 * @tc.name: OnSyncAndCreateDrawFuncTest001
 * @tc.desc: class RSPropertyDrawable OnSync and CreateDrawFunc test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, OnSyncAndCreateDrawFuncTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPropertyDrawable> propertyDrawable =
        std::make_shared<DrawableV2::RSPropertyDrawable>();
    EXPECT_NE(propertyDrawable, nullptr);

    propertyDrawable->needSync_ = false;
    propertyDrawable->OnSync();

    propertyDrawable->needSync_ = true;
    propertyDrawable->drawCmdList_ = nullptr;
    propertyDrawable->stagingDrawCmdList_ = nullptr;
    propertyDrawable->OnSync();
    EXPECT_FALSE(propertyDrawable->needSync_);

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    EXPECT_NE(drawCmdList, nullptr);
    propertyDrawable->drawCmdList_ = drawCmdList;
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    propertyDrawable->propertyDescription_ = "RSPropertyDrawable test";
    propertyDrawable->CreateDrawFunc()(&canvas, &rect);
}

/**
 * @tc.name: RSPropertyDrawCmdListUpdaterTest002
 * @tc.desc: ~RSPropertyDrawCmdListUpdater test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, RSPropertyDrawCmdListUpdaterTest002, TestSize.Level1)
{
    DrawableV2::RSPropertyDrawable targetTest1;
    std::shared_ptr<DrawableV2::RSPropertyDrawCmdListUpdater> propertyDrawCmdListUpdaterTest1 =
        std::make_shared<DrawableV2::RSPropertyDrawCmdListUpdater>(0, 0, &targetTest1);
    EXPECT_NE(propertyDrawCmdListUpdaterTest1, nullptr);

    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas = std::make_unique<ExtendRecordingCanvas>(0, 0);
    EXPECT_NE(recordingCanvas, nullptr);
    propertyDrawCmdListUpdaterTest1->recordingCanvas_ = std::move(recordingCanvas);
    DrawableV2::RSPropertyDrawable target;
    propertyDrawCmdListUpdaterTest1->target_ = &target;

    DrawableV2::RSPropertyDrawable targetTest2;
    std::shared_ptr<DrawableV2::RSPropertyDrawCmdListUpdater> propertyDrawCmdListUpdaterTest2 =
        std::make_shared<DrawableV2::RSPropertyDrawCmdListUpdater>(0, 0, &targetTest2);
    propertyDrawCmdListUpdaterTest2->recordingCanvas_ = nullptr;
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest003
 * @tc.desc: class RSFrameOffsetDrawable OnGenerate and OnUpdate test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, OnGenerateAndOnUpdateTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFrameOffsetDrawable> frameOffsetDrawable =
        std::make_shared<DrawableV2::RSFrameOffsetDrawable>();
    EXPECT_NE(frameOffsetDrawable, nullptr);

    RSRenderNode node(0);
    node.renderContent_->renderProperties_.frameOffsetX_ = 0.0f;
    node.renderContent_->renderProperties_.frameOffsetY_ = 0.0f;
    EXPECT_EQ(frameOffsetDrawable->OnGenerate(node), nullptr);

    node.renderContent_->renderProperties_.frameOffsetX_ = 1.0f;
    EXPECT_NE(frameOffsetDrawable->OnGenerate(node), nullptr);
}

/**
 * @tc.name: OnUpdateTest004
 * @tc.desc: class RSClipToBoundsDrawable OnUpdate test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSPropertyDrawableTest, OnUpdateTest004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToBoundsDrawable> clipToBoundsDrawable =
        std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_NE(clipToBoundsDrawable, nullptr);

    RSRenderNode nodeTest1(0);
    nodeTest1.renderContent_->renderProperties_.clipPath_ = std::make_shared<RSPath>();
    EXPECT_NE(nodeTest1.renderContent_->renderProperties_.clipPath_, nullptr);
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest1));

    RSRenderNode nodeTest2(0);
    RectT<float> rect(1.0f, 1.0f, 1.0f, 1.0f);
    RRectT<float> rectt(rect, 1.0f, 1.0f);
    nodeTest2.renderContent_->renderProperties_.clipRRect_ = rectt;
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest2));

    RSRenderNode nodeTest3(0);
    nodeTest3.renderContent_->renderProperties_.cornerRadius_ = 1.0f;
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest3));

    RSRenderNode nodeTest4(0);
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest4));
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest005
 * @tc.desc: class RSClipToFrameDrawable OnGenerate and OnUpdate test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, OnGenerateAndOnUpdateTest005, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToFrameDrawable> clipToFrameDrawable =
        std::make_shared<DrawableV2::RSClipToFrameDrawable>();
    EXPECT_NE(clipToFrameDrawable, nullptr);

    RSRenderNode node(0);
    node.renderContent_->renderProperties_.clipToFrame_ = false;
    EXPECT_EQ(clipToFrameDrawable->OnGenerate(node), nullptr);

    std::shared_ptr<RSObjGeometry> geometry = std::make_shared<RSObjGeometry>();
    EXPECT_NE(geometry, nullptr);

    geometry->width_ = 1.0f;
    geometry->height_ = 1.0f;
    node.renderContent_->renderProperties_.frameGeo_ = geometry;
    node.renderContent_->renderProperties_.clipToFrame_ = true;
    EXPECT_NE(clipToFrameDrawable->OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSFilterDrawableTest006
 * @tc.desc: class RSFilterDrawable and OnSync test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest006, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);
    EXPECT_EQ(filterDrawable->cacheManager_, nullptr);

    filterDrawable->needSync_ = true;
    filterDrawable->stagingFilter_ = nullptr;
    filterDrawable->isFilterCacheValid_ = true;
    filterDrawable->OnSync();
    EXPECT_FALSE(filterDrawable->needSync_);
    EXPECT_FALSE(filterDrawable->isFilterCacheValid_);

    std::shared_ptr<RSShaderFilter> shaderFilter = std::make_shared<RSShaderFilter>();
    EXPECT_NE(shaderFilter, nullptr);
    shaderFilter->type_ = RSShaderFilter::LINEAR_GRADIENT_BLUR;
    filterDrawable->filter_ = std::make_shared<RSDrawingFilter>(shaderFilter);
    EXPECT_NE(filterDrawable->filter_, nullptr);
    filterDrawable->filter_->type_ = RSFilter::LINEAR_GRADIENT_BLUR;
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    EXPECT_NE(cacheManager, nullptr);
    filterDrawable->cacheManager_ = std::move(cacheManager);
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    filterDrawable->CreateDrawFunc()(&canvas, &rect);
}

/**
 * @tc.name: ClearCacheIfNeededTest007
 * @tc.desc: class RSFilterDrawable ClearCacheIfNeeded test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, ClearCacheIfNeededTest007, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);
    filterDrawable->cacheManager_ = nullptr;
    filterDrawable->ClearCacheIfNeeded();
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    EXPECT_NE(cacheManager, nullptr);
    filterDrawable->cacheManager_ = std::move(cacheManager);
    filterDrawable->isFilterCacheValid_ = true;
    filterDrawable->forceClearCacheForLastFrame_ = true;
    filterDrawable->ClearCacheIfNeeded();
    EXPECT_FALSE(filterDrawable->isFilterCacheValid_);
    filterDrawable->forceClearCacheForLastFrame_ = false;
    filterDrawable->lastCacheType_ = FilterCacheType::NONE;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->lastCacheType_ = FilterCacheType::SNAPSHOT;
    filterDrawable->forceUseCache_ = true;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->forceUseCache_ = false;
    filterDrawable->forceClearCache_ = true;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->forceClearCache_ = false;
    filterDrawable->filterRegionChanged_ = true;
    filterDrawable->rotationChanged_ = false;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->filterRegionChanged_ = false;
    filterDrawable->filterInteractWithDirty_ = false;
    filterDrawable->pendingPurge_ = true;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->filterInteractWithDirty_ = true;
    filterDrawable->cacheUpdateInterval_ = 0;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->filterInteractWithDirty_ = false;
    filterDrawable->pendingPurge_ = false;
    filterDrawable->rotationChanged_ = true;
    filterDrawable->cacheUpdateInterval_ = 0;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->cacheUpdateInterval_ = 1;
    filterDrawable->ClearCacheIfNeeded();
}

/**
 * @tc.name: RecordFilterInfosTest008
 * @tc.desc: class RSFilterDrawable RecordFilterInfos ClearFilterCache UpdateFlags test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSPropertyDrawableTest, RecordFilterInfosTest008, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);

    std::shared_ptr<RSDrawingFilter> rsFilter = nullptr;
    filterDrawable->RecordFilterInfos(rsFilter);
    std::shared_ptr<RSShaderFilter> shaderFilter = std::make_shared<RSShaderFilter>();
    EXPECT_NE(shaderFilter, nullptr);
    filterDrawable->cachedFilterHash_ = 1;
    rsFilter = std::make_shared<RSDrawingFilter>(shaderFilter);
    filterDrawable->RecordFilterInfos(rsFilter);
    EXPECT_EQ(filterDrawable->cachedFilterHash_, rsFilter->Hash());

    // RSProperties::FilterCacheEnabled is false
    filterDrawable->ClearFilterCache();

    filterDrawable->pendingPurge_ = true;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, false);
    EXPECT_FALSE(filterDrawable->pendingPurge_);
    filterDrawable->filterInteractWithDirty_ = true;
    filterDrawable->cacheUpdateInterval_ = 3;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    EXPECT_EQ(filterDrawable->cacheUpdateInterval_, 2);
    filterDrawable->filterInteractWithDirty_ = false;
    filterDrawable->rotationChanged_ = true;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    EXPECT_EQ(filterDrawable->cacheUpdateInterval_, 1);
    filterDrawable->rotationChanged_ = false;
    filterDrawable->filterType_ = RSFilter::AIBAR;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    EXPECT_EQ(filterDrawable->cacheUpdateInterval_, 1);
}

/**
 * @tc.name: IsAIBarCacheValidTest009
 * @tc.desc: class RSFilterDrawable IsAIBarCacheValid test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSPropertyDrawableTest, IsAIBarCacheValidTest009, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);

    EXPECT_FALSE(filterDrawable->IsAIBarCacheValid());
    filterDrawable->filterType_ = RSFilter::AIBAR;
    EXPECT_FALSE(filterDrawable->IsAIBarCacheValid());
    filterDrawable->cacheUpdateInterval_ = 1;
    filterDrawable->forceClearCacheForLastFrame_ = true;
    EXPECT_FALSE(filterDrawable->IsAIBarCacheValid());
    filterDrawable->forceClearCacheForLastFrame_ = false;
    EXPECT_TRUE(filterDrawable->IsAIBarCacheValid());
}
} // namespace OHOS::Rosen