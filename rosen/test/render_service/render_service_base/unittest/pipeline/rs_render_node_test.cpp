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

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "skia_adapter/skia_canvas.h"
#include "pipeline/rs_dirty_region_manager.h"
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "third_party/flutter/skia/include/gpu/GrContext.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline Drawing::Canvas drawingCanvas_;
};

void RSRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSRenderNodeTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSRenderNodeTest::SetUp() {}
void RSRenderNodeTest::TearDown() {}

class DrawableTest : public RSDrawable {
public:
    bool OnUpdate(const RSRenderNode& content) override
    {
        return true;
    }
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override
    {
        return nullptr;
    }
    void OnSync() override
    {
        return;
    }
};

/**
 * @tc.name: UpdateRenderStatus001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, UpdateRenderStatus001, TestSize.Level1)
{
    RSSurfaceRenderNode node(id, context);
    RectI dirtyRegion;
    bool isPartialRenderEnabled = false;
    node.UpdateRenderStatus(dirtyRegion, isPartialRenderEnabled);
}

/**
 * @tc.name: UpdateParentChildrenRectTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, UpdateParentChildrenRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSBaseRenderNode> parentNode;
    node.UpdateParentChildrenRect(parentNode);
    RSBaseRenderNode rsBaseRenderNode(id, context);
    ASSERT_FALSE(rsBaseRenderNode.HasChildrenOutOfRect());
}

/**
 * @tc.name: ProcessTransitionBeforeChildrenTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, ProcessTransitionBeforeChildrenTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.ProcessTransitionBeforeChildren(*canvas_);
}

/**
 * @tc.name: ProcessTransitionBeforeChildrenTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, AddModifierTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderModifier> modifier = nullptr;
    RSRenderNode node(id, context);
    node.AddModifier(modifier);
    ASSERT_FALSE(node.IsDirty());
}

/**
 * @tc.name: InitCacheSurfaceTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, InitCacheSurfaceTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    CacheType type = CacheType::ANIMATE_PROPERTY;
    node.SetCacheType(type);
    node.InitCacheSurface(canvas_->GetGPUContext().get());
}

/**
 * @tc.name: DrawCacheSurfaceTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, DrawCacheSurfaceTest001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool isSubThreadNode = false;
    node.DrawCacheSurface(*canvas_, isSubThreadNode);
    if (node.GetCompletedCacheSurface() == nullptr) {
        ASSERT_EQ(NULL, node.GetCacheSurface());
    }
}

/**
 * @tc.name: DrawCacheSurfaceTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, DrawCacheSurfaceTest002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool isSubThreadNode = false;
    CacheType type = CacheType::ANIMATE_PROPERTY;
    node.SetCacheType(type);
    node.DrawCacheSurface(*canvas_, isSubThreadNode);
    ASSERT_EQ(node.GetCompletedCacheSurface(), NULL);
}

/**
 * @tc.name: MarkNodeGroupTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, MarkNodeGroupTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool isNodeGruop = true;
    auto nodeGruopType = node.GetNodeGroupType();
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_USER;
    if (type >= nodeGruopType) {
        node.MarkNodeGroup(type, isNodeGruop, false);
        ASSERT_EQ(node.GetNodeGroupType(), type);
    }
}

/**
 * @tc.name: SetDrawingCacheTypeTest
 * @tc.desc: test SetDrawingCacheType for all drawing cache types
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, SetDrawingCacheTypeTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    node.SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);
    node.SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);
}

/**
 * @tc.name: ResetFilterRectsInCacheTest
 * @tc.desc: test ResetFilterRectsInCache api
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, ResetFilterRectsInCacheTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    RSRenderNode cacheNode(id + 1, context);
    std::unordered_set<NodeId> setRects = {};
    std::unordered_map<NodeId, std::unordered_set<NodeId>> getRects = {};
    setRects.insert(cacheNode.GetId());

    node.ResetFilterRectsInCache(setRects);
    node.GetFilterRectsInCache(getRects);
    ASSERT_NE(getRects[node.GetId()].size(), 0);
}

/**
 * @tc.name: SetDrawingCacheChangedTest
 * @tc.desc: test SetDrawingCacheChanged while drawing cache changed
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, SetDrawingCacheChangedTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.InitRenderParams();
    bool isDrawingCacheChanged = true;

    node.SetDrawingCacheChanged(isDrawingCacheChanged);
    ASSERT_EQ(node.GetDrawingCacheChanged(), isDrawingCacheChanged);
}

/**
 * @tc.name: ResetDrawingCacheNeedUpdateTest001
 * @tc.desc: test ResetDrawingCacheNeedUpdateTest api
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, ResetDrawingCacheNeedUpdateTest001, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.InitRenderParams();
    bool isDrawingCacheChanged = true;

    node.SetDrawingCacheChanged(isDrawingCacheChanged);
    node.ResetDrawingCacheNeedUpdate();
    ASSERT_EQ(node.drawingCacheNeedUpdate_, false);
}

/**
 * @tc.name: ResetDrawingCacheNeedUpdateTest002
 * @tc.desc: don't reset DrawingCacheNeedUpdate after set DrawingCacheChanged
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, ResetDrawingCacheNeedUpdateTest002, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.InitRenderParams();
    node.SetDrawingCacheChanged(true);
    ASSERT_EQ(node.GetDrawingCacheChanged(), true);
}

/**
 * @tc.name: SetVisitedCacheRootIdsTest
 * @tc.desc: test SetVisitedCacheRootIds api
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, SetVisitedCacheRootIdsTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    RSRenderNode cacheNode(id + 1, context);
    std::unordered_set<NodeId> VisitedIds = {};
    VisitedIds.insert(cacheNode.GetId());

    node.SetVisitedCacheRootIds(VisitedIds);
    ASSERT_NE(node.GetVisitedCacheRootIds().size(), 0);
}

/**
 * @tc.name: SetDrawingCacheRootIdTest
 * @tc.desc: test SetDrawingCacheRootId api
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest,  SetDrawingCacheRootIdTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    RSRenderNode drawingCacheRootNode(id + 1, context);

    node.SetDrawingCacheRootId(drawingCacheRootNode.GetId());
    ASSERT_EQ(node.GetDrawingCacheRootId(), drawingCacheRootNode.GetId());
}

/**
 * @tc.name: SetGeoUpdateDelay01
 * @tc.desc: test SetGeoUpdateDelay once
 * @tc.type: FUNC
 * @tc.require: issueI8JMN8
 */
HWTEST_F(RSRenderNodeTest,  SetGeoUpdateDelay01, TestSize.Level2)
{
    RSRenderNode node(id, context);
    // test default value
    ASSERT_EQ(node.GetGeoUpdateDelay(), false);

    node.SetGeoUpdateDelay(true);
    ASSERT_EQ(node.GetGeoUpdateDelay(), true);
}

/**
 * @tc.name: SetGeoUpdateDelay02
 * @tc.desc: test SetGeoUpdateDelay would not be covered by later setting
 * @tc.type: FUNC
 * @tc.require: issueI8JMN8
 */
HWTEST_F(RSRenderNodeTest,  SetGeoUpdateDelay02, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetGeoUpdateDelay(true);
    node.SetGeoUpdateDelay(false);
    ASSERT_EQ(node.GetGeoUpdateDelay(), true);
}

/**
 * @tc.name: ResetGeoUpdateDelay01
 * @tc.desc: test SetGeoUpdateDelay would be reset
 * @tc.type: FUNC
 * @tc.require: issueI8JMN8
 */
HWTEST_F(RSRenderNodeTest,  ResetGeoUpdateDelay01, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetGeoUpdateDelay(true);
    node.ResetGeoUpdateDelay();
    ASSERT_EQ(node.GetGeoUpdateDelay(), false);
}

/**
 * @tc.name: SetBootAnimationTest
 * @tc.desc: SetBootAnimation and GetBootAnimation
 * @tc.type:FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSRenderNodeTest, SetBootAnimationTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetBootAnimation(true);
    ASSERT_EQ(node.GetBootAnimation(), true);
    node.SetBootAnimation(false);
    ASSERT_FALSE(node.GetBootAnimation());
}

/**
 * @tc.name: OnlyBasicGeoTransfromTest01
 * @tc.desc: Check node only contains BasicGeoTransfrom by default
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSRenderNodeTest, OnlyBasicGeoTransfromTest01, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), true);
}

/**
 * @tc.name: OnlyBasicGeoTransfromTest02
 * @tc.desc: Check node contains more than BasicGeoTransfrom if content dirty
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSRenderNodeTest, OnlyBasicGeoTransfromTest02, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetContentDirty();
    ASSERT_EQ(node.IsContentDirty(), true);
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), false);
}

/**
 * @tc.name: OnlyBasicGeoTransfromTest03
 * @tc.desc: Check node resets BasicGeoTransfrom as true
 * @tc.type: FUNC
 * @tc.require: issueI8KEUU
 */
HWTEST_F(RSRenderNodeTest, OnlyBasicGeoTransfromTest03, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetContentDirty();
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), false);
    node.ResetIsOnlyBasicGeoTransform();
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), true);
}

/**
 * @tc.name: UpdateDirtyRegionTest01
 * @tc.desc: check dirty region not join when not dirty
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest01, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    bool geoDirty = false;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), true);
}

/**
 * @tc.name: UpdateDirtyRegionTest02
 * @tc.desc: check dirty region not join when not geometry dirty
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest02, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.geometryChangeNotPerceived_ = false;
    bool geoDirty = false;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), true);
}

/**
 * @tc.name: UpdateDirtyRegionTest03
 * @tc.desc: check dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest03, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    bool isDirtyRectCorrect = (rsDirtyManager->GetCurrentFrameDirtyRegion() == absRect);
    ASSERT_EQ(isDirtyRectCorrect, true);
}

/**
 * @tc.name: UpdateDirtyRegionTest04
 * @tc.desc: check shadow dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest04, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    properties.SetShadowOffsetX(10.0f);
    properties.SetShadowOffsetY(10.0f);
    properties.SetShadowRadius(10.0f);
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: UpdateDirtyRegionTest05
 * @tc.desc: check outline dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest05, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    properties.SetOutlineWidth(10.0f);
    properties.SetOutlineStyle((uint32_t)BorderStyle::SOLID);
    properties.SetOutlineColor(RSColor(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX));
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: UpdateDisplayListTest020
 * @tc.desc: UpdateDisplayList test
 * @tc.type: FUNC
 * @tc.require: issuesI9SGVY
 */
HWTEST_F(RSRenderNodeTest, UpdateDisplayListTest020, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    // UpdateDisplayList test
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);
    nodeTest->drawCmdListNeedSync_ = false;
    nodeTest->UpdateDisplayList();
    EXPECT_TRUE(nodeTest->drawCmdListNeedSync_);

    nodeTest->drawableVecStatus_ = 48;
    std::shared_ptr<DrawableTest> drawableTest1 = std::make_shared<DrawableTest>();
    nodeTest->drawableVec_.at(1) = drawableTest1;
    nodeTest->drawCmdListNeedSync_ = false;
    nodeTest->UpdateDisplayList();
    EXPECT_TRUE(nodeTest->drawCmdListNeedSync_);

    nodeTest->drawableVecStatus_ = 32;
    nodeTest->UpdateDisplayList();
    EXPECT_EQ(nodeTest->stagingDrawCmdIndex_.contentIndex_, -1);
}

/**
 * @tc.name: UpdateRenderingTest021
 * @tc.desc: UpdateEffectRegion GetModifier UpdateShouldPaint SetSharedTransitionParam test
 * @tc.type: FUNC
 * @tc.require: issuesI9SGVY
 */
HWTEST_F(RSRenderNodeTest, UpdateRenderingTest021, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    // UpdateEffectRegion test
    std::optional<Drawing::RectI> region = std::nullopt;
    nodeTest->UpdateEffectRegion(region, false);
    Drawing::RectI rectI;
    region = rectI;
    nodeTest->UpdateEffectRegion(region, false);
    nodeTest->renderContent_->renderProperties_.useEffect_ = true;
    std::shared_ptr<RSObjAbsGeometry> boundsGeo = std::make_shared<RSObjAbsGeometry>();
    nodeTest->renderContent_->renderProperties_.boundsGeo_ = boundsGeo;
    nodeTest->UpdateEffectRegion(region, true);

    // GetModifier test
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier = nullptr;
    nodeTest->modifiers_.emplace(0, modifier);
    EXPECT_EQ(nodeTest->GetModifier(0), nullptr);
    EXPECT_EQ(nodeTest->GetModifier(1), nullptr);
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(propertyTest, nullptr);
    std::shared_ptr<RSDrawCmdListRenderModifier> drawCmdModifiersTest =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest);
    EXPECT_NE(drawCmdModifiersTest, nullptr);
    drawCmdModifiersTest->property_->id_ = 1;
    nodeTest->renderContent_->drawCmdModifiers_[RSModifierType::BOUNDS].emplace_back(drawCmdModifiersTest);
    EXPECT_NE(nodeTest->GetModifier(1), nullptr);

    // FilterModifiersByPid test
    nodeTest->FilterModifiersByPid(1);

    // UpdateShouldPaint test
    nodeTest->renderContent_->renderProperties_.alpha_ = -1.0f;
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(0.0f, 0.1f);
    nodeTest->renderContent_->renderProperties_.filter_ = filter;
    nodeTest->UpdateShouldPaint();

    // SetSharedTransitionParam test
    std::shared_ptr<SharedTransitionParam> sharedTransitionParam = nullptr;
    nodeTest->sharedTransitionParam_ = nullptr;
    nodeTest->SetSharedTransitionParam(sharedTransitionParam);
    RSRenderNode::SharedPtr inNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(inNode, nullptr);
    RSRenderNode::SharedPtr outNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(outNode, nullptr);
    sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
    EXPECT_NE(sharedTransitionParam, nullptr);
    nodeTest->parent_ = std::make_shared<RSRenderNode>(0);
    nodeTest->SetSharedTransitionParam(sharedTransitionParam);
    EXPECT_NE(nodeTest->sharedTransitionParam_, nullptr);
}

/**
 * @tc.name: ManageRenderingResourcesTest022
 * @tc.desc: SetGlobalAlpha NeedInitCacheSurface GetOptionalBufferSize test
 * @tc.type: FUNC
 * @tc.require: issuesI9SGVY
 */
HWTEST_F(RSRenderNodeTest, ManageRenderingResourcesTest022, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    // SetGlobalAlpha test
    nodeTest->SetGlobalAlpha(1.0f);
    EXPECT_EQ(nodeTest->globalAlpha_, 1.0f);
    nodeTest->SetGlobalAlpha(1.01f);
    EXPECT_EQ(nodeTest->globalAlpha_, 1.01f);

    // NeedInitCacheSurface and GetOptionalBufferSize test
    nodeTest->SetCacheType(CacheType::ANIMATE_PROPERTY);
    RSShadow rsShadow;
    std::optional<RSShadow> shadow(rsShadow);
    nodeTest->renderContent_->renderProperties_.shadow_ = shadow;
    nodeTest->renderContent_->renderProperties_.shadow_->radius_ = 1.0f;
    nodeTest->renderContent_->renderProperties_.isSpherizeValid_ = true;
    nodeTest->cacheSurface_ = nullptr;
    EXPECT_TRUE(nodeTest->NeedInitCacheSurface());

    nodeTest->boundsModifier_ = nullptr;
    nodeTest->frameModifier_ = nullptr;
    nodeTest->SetCacheType(CacheType::CONTENT);
    EXPECT_TRUE(nodeTest->NeedInitCacheSurface());

    nodeTest->boundsModifier_ = nullptr;
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(propertyTest, nullptr);
    std::shared_ptr<RSDrawCmdListRenderModifier> frameModifier =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest);
    EXPECT_NE(frameModifier, nullptr);
    frameModifier->property_ = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(frameModifier->property_, nullptr);
    nodeTest->frameModifier_ = frameModifier;
    nodeTest->cacheSurface_ = std::make_shared<Drawing::Surface>();
    EXPECT_NE(nodeTest->cacheSurface_, nullptr);
    nodeTest->cacheSurface_->cachedCanvas_ = nullptr;
    EXPECT_TRUE(nodeTest->NeedInitCacheSurface());
    nodeTest->cacheSurface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>();
    EXPECT_FALSE(nodeTest->NeedInitCacheSurface());
}

/**
 * @tc.name: NeedInitCacheCompletedSurfaceTest023
 * @tc.desc: NeedInitCacheCompletedSurface test
 * @tc.type: FUNC
 * @tc.require: issuesI9SGVY
 */
HWTEST_F(RSRenderNodeTest, NeedInitCacheCompletedSurfaceTest023, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    // NeedInitCacheCompletedSurface test
    nodeTest->boundsModifier_ = nullptr;
    nodeTest->frameModifier_ = nullptr;
    nodeTest->cacheCompletedSurface_ = nullptr;
    EXPECT_TRUE(nodeTest->NeedInitCacheCompletedSurface());
    nodeTest->cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    EXPECT_NE(nodeTest->cacheCompletedSurface_, nullptr);
    nodeTest->cacheCompletedSurface_->cachedCanvas_ = nullptr;
    EXPECT_TRUE(nodeTest->NeedInitCacheCompletedSurface());
    nodeTest->cacheCompletedSurface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>();
    EXPECT_NE(nodeTest->cacheCompletedSurface_->cachedCanvas_, nullptr);
    EXPECT_FALSE(nodeTest->NeedInitCacheCompletedSurface());
}

/**
 * @tc.name: InitCacheSurfaceTest024
 * @tc.desc: InitCacheSurface test
 * @tc.type: FUNC
 * @tc.require: issuesI9SGVY
 */
HWTEST_F(RSRenderNodeTest, InitCacheSurfaceTest024, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->boundsModifier_ = nullptr;
    nodeTest->frameModifier_ = nullptr;

    RSRenderNode::ClearCacheSurfaceFunc funcTest1 = nullptr;
    Drawing::GPUContext gpuContextTest1;
    nodeTest->cacheType_ = CacheType::ANIMATE_PROPERTY;
    RSShadow rsShadow;
    std::optional<RSShadow> shadow(rsShadow);
    nodeTest->renderContent_->renderProperties_.shadow_ = shadow;
    nodeTest->renderContent_->renderProperties_.shadow_->radius_ = 1.0f;
    nodeTest->renderContent_->renderProperties_.isSpherizeValid_ = false;
    nodeTest->cacheSurface_ = nullptr;
    nodeTest->InitCacheSurface(&gpuContextTest1, funcTest1, 1);
    EXPECT_EQ(nodeTest->cacheSurface_, nullptr);

    nodeTest->cacheSurface_ = std::make_shared<Drawing::Surface>();
    EXPECT_NE(nodeTest->cacheSurface_, nullptr);
    nodeTest->cacheSurfaceThreadIndex_ = 1;
    nodeTest->completedSurfaceThreadIndex_ = 1;
    Drawing::GPUContext gpuContextTest2;
    nodeTest->cacheType_ = CacheType::CONTENT;
    std::shared_ptr<Drawing::Surface> surfaceTest1 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest1, nullptr);
    std::shared_ptr<Drawing::Surface> surfaceTest2 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest2, nullptr);
    RSRenderNode::ClearCacheSurfaceFunc funcTest2 = [surfaceTest1, surfaceTest2](std::shared_ptr<Drawing::Surface>&& s1,
                                                        std::shared_ptr<Drawing::Surface>&& s2, uint32_t w,
                                                        uint32_t h) {};
    nodeTest->InitCacheSurface(&gpuContextTest2, funcTest2, 1);
    EXPECT_EQ(nodeTest->cacheSurface_, nullptr);

    Drawing::GPUContext* gpuContextTest3 = nullptr;
    std::shared_ptr<Drawing::Surface> surfaceTest3 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest3, nullptr);
    std::shared_ptr<Drawing::Surface> surfaceTest4 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest4, nullptr);
    RSRenderNode::ClearCacheSurfaceFunc funcTest3 = [surfaceTest3, surfaceTest4](std::shared_ptr<Drawing::Surface>&& s1,
                                                        std::shared_ptr<Drawing::Surface>&& s2, uint32_t w,
                                                        uint32_t h) {};
    nodeTest->clearCacheSurfaceFunc_ = nullptr;
    nodeTest->cacheSurfaceThreadIndex_ = 1;
    nodeTest->completedSurfaceThreadIndex_ = 1;
    nodeTest->InitCacheSurface(gpuContextTest3, funcTest3, 1);
    EXPECT_EQ(nodeTest->cacheSurface_, nullptr);
}

/**
 * @tc.name: DrawCacheSurfaceTest025
 * @tc.desc: DrawCacheSurface test
 * @tc.type: FUNC
 * @tc.require: issuesI9SGVY
 */
HWTEST_F(RSRenderNodeTest, DrawCacheSurfaceTest025, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->boundsModifier_ = nullptr;
    nodeTest->frameModifier_ = nullptr;
    Drawing::Canvas canvasTest1;
    std::shared_ptr<Drawing::GPUContext> gpuContextTest1 = std::make_shared<Drawing::GPUContext>();
    canvasTest1.gpuContext_ = gpuContextTest1;
    RSPaintFilterCanvas paintFilterCanvasTest1(&canvasTest1);

    nodeTest->boundsWidth_ = 0.0f;
    nodeTest->DrawCacheSurface(paintFilterCanvasTest1, 0, true);
    nodeTest->boundsWidth_ = 10.0f;
    nodeTest->DrawCacheSurface(paintFilterCanvasTest1, 0, false);

    nodeTest->boundsWidth_ = 10.0f;
    nodeTest->boundsHeight_ = 10.0f;
    nodeTest->cacheCompletedBackendTexture_.isValid_ = false;
    paintFilterCanvasTest1.canvas_->paintBrush_.hasFilter_ = true;
    nodeTest->DrawCacheSurface(paintFilterCanvasTest1, 0, true);
    EXPECT_TRUE(paintFilterCanvasTest1.canvas_->paintBrush_.hasFilter_);

    // RSSystemProperties::GetRecordingEnabled() is false
    nodeTest->cacheCompletedBackendTexture_.isValid_ = true;
    RSShadow rsShadow;
    std::optional<RSShadow> shadow(rsShadow);
    nodeTest->renderContent_->renderProperties_.shadow_ = shadow;
    nodeTest->renderContent_->renderProperties_.shadow_->radius_ = 1.0f;
    nodeTest->cacheType_ = CacheType::ANIMATE_PROPERTY;
    Drawing::Canvas canvasTest2;
    std::shared_ptr<Drawing::GPUContext> gpuContextTest2 = std::make_shared<Drawing::GPUContext>();
    canvasTest2.gpuContext_ = gpuContextTest2;
    RSPaintFilterCanvas paintFilterCanvasTest2(&canvasTest2);
    std::shared_ptr<Drawing::SkiaCanvas> implTest1 = std::make_shared<Drawing::SkiaCanvas>();
    implTest1->skCanvas_ = nullptr;
    paintFilterCanvasTest2.canvas_->impl_ = implTest1;
    paintFilterCanvasTest2.canvas_->paintBrush_.hasFilter_ = true;
    nodeTest->DrawCacheSurface(paintFilterCanvasTest2, 0, true);
    EXPECT_FALSE(paintFilterCanvasTest2.canvas_->paintBrush_.hasFilter_);

    // RSSystemProperties::GetRecordingEnabled() is false
    // cacheCompletedSurface_->GetImageSnapshot() and RSSystemProperties::GetRecordingEnabled() is false
    // so isUIFirst only is true
    nodeTest->cacheType_ = CacheType::CONTENT;
    Drawing::Canvas canvasTest3;
    std::shared_ptr<Drawing::GPUContext> gpuContextTest3 = std::make_shared<Drawing::GPUContext>();
    canvasTest3.gpuContext_ = gpuContextTest3;
    RSPaintFilterCanvas paintFilterCanvasTest3(&canvasTest3);
    std::shared_ptr<Drawing::SkiaCanvas> implTest2 = std::make_shared<Drawing::SkiaCanvas>();
    implTest2->skCanvas_ = nullptr;
    paintFilterCanvasTest3.canvas_->impl_ = implTest2;
    paintFilterCanvasTest3.canvas_->paintBrush_.hasFilter_ = true;
    nodeTest->DrawCacheSurface(paintFilterCanvasTest3, 0, true);
    EXPECT_FALSE(paintFilterCanvasTest3.canvas_->paintBrush_.hasFilter_);
}

/**
 * @tc.name: GetCompletedImageTest026
 * @tc.desc: GetCompletedImage test
 * @tc.type: FUNC
 * @tc.require: issuesI9SGVY
 */
HWTEST_F(RSRenderNodeTest, GetCompletedImageTest026, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    Drawing::Canvas canvastest;
    std::shared_ptr<Drawing::GPUContext> gpuContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_NE(gpuContext, nullptr);
    canvastest.gpuContext_ = gpuContext;
    RSPaintFilterCanvas canvas(&canvastest);

    nodeTest->cacheCompletedBackendTexture_.isValid_ = false;
    EXPECT_EQ(nodeTest->GetCompletedImage(canvas, 0, true), nullptr);

    nodeTest->cacheCompletedBackendTexture_.isValid_ = true;
    EXPECT_NE(nodeTest->GetCompletedImage(canvas, 0, true), nullptr);

    nodeTest->cacheCompletedSurface_ = nullptr;
    EXPECT_EQ(nodeTest->GetCompletedImage(canvas, 0, false), nullptr);

    std::shared_ptr<Drawing::Surface> cacheCompletedSurface = std::make_shared<Drawing::Surface>();
    EXPECT_NE(cacheCompletedSurface, nullptr);
    nodeTest->cacheCompletedSurface_ = cacheCompletedSurface;
    // cacheCompletedSurface_->GetImageSnapshot() is false
    EXPECT_EQ(nodeTest->GetCompletedImage(canvas, 0, false), nullptr);
}

/**
 * @tc.name: ManageCachingTest027
 * @tc.desc: GetCompletedCacheSurface GetCacheSurface MarkNodeGroup test
 * @tc.type: FUNC
 * @tc.require: issuesI9SGVY
 */
HWTEST_F(RSRenderNodeTest, ManageCachingTest027, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    // GetCompletedCacheSurface test
    nodeTest->cacheCompletedSurface_ = nullptr;
    EXPECT_EQ(nodeTest->GetCompletedCacheSurface(0, true, true), nullptr);
    EXPECT_EQ(nodeTest->GetCompletedCacheSurface(0, true, false), nullptr);
    std::shared_ptr<Drawing::Surface> cacheCompletedSurface = std::make_shared<Drawing::Surface>();
    EXPECT_NE(cacheCompletedSurface, nullptr);
    nodeTest->cacheCompletedSurface_ = cacheCompletedSurface;

    EXPECT_NE(nodeTest->cacheCompletedSurface_, nullptr);
    EXPECT_NE(nodeTest->cacheCompletedSurface_, nullptr);
    EXPECT_EQ(nodeTest->GetCompletedCacheSurface(0, true, false), nullptr);
    EXPECT_EQ(nodeTest->GetCompletedCacheSurface(0, false, false), nullptr);
    nodeTest->completedSurfaceThreadIndex_ = 0;
    EXPECT_EQ(nodeTest->GetCompletedCacheSurface(0, true, false), nullptr);
    // GetCacheSurface test
    nodeTest->cacheSurface_ = nullptr;
    EXPECT_EQ(nodeTest->GetCacheSurface(0, true, true), nullptr);
    EXPECT_EQ(nodeTest->GetCacheSurface(0, true, false), nullptr);
    std::shared_ptr<Drawing::Surface> cacheSurface = std::make_shared<Drawing::Surface>();
    nodeTest->cacheSurface_ = cacheSurface;
    EXPECT_NE(nodeTest->cacheSurface_, nullptr);
    EXPECT_EQ(nodeTest->GetCacheSurface(0, true, false), nullptr);
    EXPECT_EQ(nodeTest->GetCacheSurface(0, false, false), nullptr);
    nodeTest->completedSurfaceThreadIndex_ = 0;
    EXPECT_EQ(nodeTest->GetCacheSurface(0, true, false), nullptr);

    // MarkNodeGroup test
    nodeTest->nodeGroupIncludeProperty_ = false;
    nodeTest->nodeGroupType_ = RSRenderNode::NONE;
    std::shared_ptr<RSContext> contextTest = std::make_shared<RSContext>();
    contextTest->nodeMap.residentSurfaceNodeMap_.emplace(
        std::make_pair(0, std::make_shared<RSSurfaceRenderNode>(0, std::make_shared<RSContext>())));
    nodeTest->context_ = contextTest;
    nodeTest->MarkNodeGroup(RSRenderNode::GROUPED_BY_UI, true, true);
    EXPECT_EQ(nodeTest->nodeGroupType_, RSRenderNode::GROUPED_BY_UI);
    EXPECT_TRUE(nodeTest->nodeGroupIncludeProperty_);

    nodeTest->MarkNodeGroup(RSRenderNode::NONE, true, true);
    EXPECT_TRUE(nodeTest->nodeGroupIncludeProperty_);
    nodeTest->MarkNodeGroup(RSRenderNode::NONE, false, true);
    EXPECT_TRUE(nodeTest->nodeGroupIncludeProperty_);
}

/**
 * @tc.name: CheckGroupableAnimationTest028
 * @tc.desc: CheckGroupableAnimation test
 * @tc.type: FUNC
 * @tc.require: issuesI9SGVY
 */
HWTEST_F(RSRenderNodeTest, CheckGroupableAnimationTest028, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    nodeTest->CheckGroupableAnimation(0, true);

    nodeTest->CheckGroupableAnimation(1, true);

    std::shared_ptr<RSCanvasRenderNode> canvasRenderNodeTest = std::make_shared<RSCanvasRenderNode>(0);
    EXPECT_NE(canvasRenderNodeTest, nullptr);
    std::shared_ptr<RSContext> context = nullptr;
    nodeTest->context_ = context;
    // RSSystemProperties::GetAnimationCacheEnabled() is false
    canvasRenderNodeTest->CheckGroupableAnimation(1, true);
}

/**
 * @tc.name: ClearCacheSurfaceInThreadTest029
 * @tc.desc: ClearCacheSurfaceInThread test
 * @tc.type: FUNC
 * @tc.require: issuesI9SGVY
 */
HWTEST_F(RSRenderNodeTest, ClearCacheSurfaceInThreadTest029, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<Drawing::Surface> surfaceTest1 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest1, nullptr);
    std::shared_ptr<Drawing::Surface> surfaceTest2 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest2, nullptr);
    nodeTest->clearCacheSurfaceFunc_ = [surfaceTest1, surfaceTest2](std::shared_ptr<Drawing::Surface>&& s1,
                                           std::shared_ptr<Drawing::Surface>&& s2, uint32_t w, uint32_t h) {};
    nodeTest->ClearCacheSurfaceInThread();
}
} // namespace Rosen
} // namespace OHOS
