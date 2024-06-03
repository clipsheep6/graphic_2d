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
const std::string OUT_STR1 =
    "DISPLAY_NODERS_NODESURFACE_NODECANVAS_NODEROOT_NODEPROXY_NODECANVAS_DRAWING_NODEEFFECT_NODEUNKNOWN_NODE";
const std::string OUT_STR2 =
    "| RS_NODE[0], instanceRootNodeId[0], SharedTransitionParam: [0 -> 0], [nodeGroup1], uifirstRootNodeId_: 1, "
    "Properties: Bounds[-inf -inf -inf -inf] Frame[-inf -inf -inf -inf], GetBootAnimation: true, "
    "isContainBootAnimation: true, isNodeDirty: 1, isPropertyDirty: true, isSubTreeDirty: true, IsPureContainer: true, "
    "Children list needs update, current count: 0 expected count: 0+1\n";
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

class PropertyDrawableTest : public RSPropertyDrawable {
public:
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override
    {
        return;
    }
    bool Update(const RSRenderContent& content) override
    {
        return false;
    };
};
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
 * @tc.name: ManageDrawingCacheTest001
 * @tc.desc: SetDrawingCacheChanged and ResetDrawingCacheNeedUpdate test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, ManageDrawingCacheTest001, TestSize.Level2)
{
    // SetDrawingCacheChanged test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);

    nodeTest->lastFrameSynced_ = true;
    nodeTest->stagingRenderParams_->needSync_ = false;
    nodeTest->stagingRenderParams_->isDrawingCacheChanged_ = true;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);

    nodeTest->SetDrawingCacheChanged(false);
    EXPECT_FALSE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    nodeTest->lastFrameSynced_ = false;
    nodeTest->stagingRenderParams_->needSync_ = false;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    // ResetDrawingCacheNeedUpdate test
    nodeTest->drawingCacheNeedUpdate_ = true;
    nodeTest->ResetDrawingCacheNeedUpdate();
    EXPECT_FALSE(nodeTest->drawingCacheNeedUpdate_);

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
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
 * @tc.name: RSRenderNodeDirtyTest001
 * @tc.desc: SetDirty OnRegisterTest SetParentSubTreeDirty and InitRenderParams test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, RSRenderNodeDirtyTest001, TestSize.Level1)
{
    std::shared_ptr<RSContext> contextTest1 = nullptr;
    RSRenderNode nodetest1(0, contextTest1);
    // SetDirty test
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    parent->isSubTreeDirty_ = false;
    nodetest1.parent_ = parent;
    nodetest1.dirtyStatus_ = RSRenderNode::NodeDirty::CLEAN;
    // dirtyStatus_ is NodeDirty::CLEAN
    nodetest1.SetDirty(true);
    EXPECT_EQ(nodetest1.dirtyStatus_, RSRenderNode::NodeDirty::DIRTY);
    // dirtyStatus_ is not NodeDirty::CLEAN
    nodetest1.SetDirty(true);
    EXPECT_TRUE(parent->isSubTreeDirty_);

    // OnRegister and InitRenderParams test
    std::shared_ptr<RSContext> contextTest2 = std::make_shared<RSContext>();
    EXPECT_NE(contextTest2, nullptr);
    std::shared_ptr<RSRenderNode> nodeTest2 = std::make_shared<RSRenderNode>(0, contextTest2);
    EXPECT_NE(nodeTest2, nullptr);
    nodeTest2->renderDrawable_ = nullptr;
    std::shared_ptr<RSContext> contextTest3 = std::make_shared<RSContext>();
    EXPECT_NE(contextTest3, nullptr);
    nodeTest2->OnRegister(contextTest3);
    EXPECT_EQ(nodeTest2->renderContent_->type_, nodeTest2->GetType());
}

/**
 * @tc.name: RSRenderNodeDumpTest002
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, RSRenderNodeDumpTest002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    // DumpNodeType test;
    std::string outTest1 = "";
    nodeTest->DumpNodeType(RSRenderNodeType::DISPLAY_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::RS_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::SURFACE_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::CANVAS_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::ROOT_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::PROXY_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::CANVAS_DRAWING_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::EFFECT_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::UNKNOW, outTest1);
    EXPECT_EQ(outTest1, OUT_STR1);

    // DumpTree test;
    std::string outTest2 = "";
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(inNode, nullptr);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(outNode, nullptr);
    nodeTest->sharedTransitionParam_ = std::make_shared<SharedTransitionParam>(inNode, outNode);
    EXPECT_NE(nodeTest->sharedTransitionParam_, nullptr);
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    nodeTest->uifirstRootNodeId_ = 1;
    nodeTest->SetBootAnimation(true);
    nodeTest->SetContainBootAnimation(true);
    nodeTest->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    nodeTest->renderContent_->renderProperties_.isDirty_ = true;
    nodeTest->isSubTreeDirty_ = true;
    nodeTest->renderContent_->renderProperties_.isDrawn_ = false;
    nodeTest->renderContent_->renderProperties_.alphaNeedApply_ = false;
    nodeTest->renderContent_->drawCmdModifiers_.clear();
    nodeTest->isFullChildrenListValid_ = false;
    nodeTest->disappearingChildren_.emplace_back(std::make_shared<RSRenderNode>(0), 0);
    nodeTest->DumpTree(0, outTest2);
    EXPECT_EQ(outTest2, OUT_STR2);

    // DumpSubClassNode test
    std::string outTest3 = "";
    nodeTest->DumpSubClassNode(outTest3);
    EXPECT_EQ(outTest3, "");
}

/**
 * @tc.name: IsContentNodeTest003
 * @tc.desc: IsContentNode test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, IsContentNodeTest003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->renderContent_->drawCmdModifiers_.clear();
    nodeTest->renderContent_->renderProperties_.isDrawn_ = false;
    EXPECT_TRUE(nodeTest->IsContentNode());
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> property =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    nodeTest->renderContent_->drawCmdModifiers_[RSModifierType::CONTENT_STYLE].emplace_back(
        std::make_shared<RSDrawCmdListRenderModifier>(property));
    EXPECT_TRUE(nodeTest->IsContentNode());
}

/**
 * @tc.name: AddChildTest004
 * @tc.desc: AddChild test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, AddChildTest004, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::shared_ptr<RSRenderNode> childTest1 = nullptr;
    nodeTest->AddChild(childTest1, 0);

    childTest1 = std::make_shared<RSRenderNode>(0);
    nodeTest->AddChild(childTest1, 0);

    std::shared_ptr<RSRenderNode> childTest2 = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(childTest2, nullptr);
    std::shared_ptr<RSRenderNode> parent = nullptr;
    childTest2->parent_ = parent;
    nodeTest->isOnTheTree_ = false;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->AddChild(childTest2, -1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->AddChild(childTest2, 1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    parent = std::make_shared<RSRenderNode>(0);
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->isOnTheTree_ = true;
    nodeTest->AddChild(childTest2, 1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
}

/**
 * @tc.name: MoveChildTest005
 * @tc.desc: MoveChild test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, MoveChildTest005, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSRenderNode> childTest = nullptr;
    nodeTest->MoveChild(childTest, 0);

    childTest = std::make_shared<RSRenderNode>(0);
    nodeTest->isFullChildrenListValid_ = true;
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    childTest->parent_ = parent;
    nodeTest->MoveChild(childTest, 0);
    EXPECT_TRUE(nodeTest->isFullChildrenListValid_);

    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> child3 = std::make_shared<RSRenderNode>(3);
    nodeTest->AddChild(child1);
    nodeTest->AddChild(child2);
    nodeTest->AddChild(child3);
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->MoveChild(child2, 3);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->MoveChild(child2, -1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->MoveChild(child2, 4);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
}

/**
 * @tc.name: ParentChildRelationshipTest006
 * @tc.desc: RemoveChild and ResetClearSurfaeFunc test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, ParentChildRelationshipTest006, TestSize.Level1)
{
    // RemoveChild test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSRenderNode> childTest = nullptr;
    nodeTest->RemoveChild(childTest, false);

    childTest = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    childTest->parent_ = parent;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveChild(childTest, false);
    EXPECT_TRUE(nodeTest->isFullChildrenListValid_);

    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->disappearingTransitionCount_ = 1;
    nodeTest->isBootAnimation_ = true;

    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> child3 = std::make_shared<RSRenderNode>(3);
    nodeTest->AddChild(child1);
    nodeTest->AddChild(child2);
    nodeTest->AddChild(child3);
    nodeTest->RemoveChild(child1, false);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveChild(child2, true);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    // ResetClearSurfaeFunc test
    nodeTest->ResetClearSurfaeFunc();
    EXPECT_EQ(nodeTest->clearSurfaceTask_, nullptr);
}

/**
 * @tc.name: RSRenderNodeTreeTest007
 * @tc.desc: SetIsOnTheTree ResetChildRelevantFlags and UpdateChildrenRect test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, RSRenderNodeTreeTest007, TestSize.Level1)
{
    // SetIsOnTheTree test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    nodeTest->isOnTheTree_ = false;
    nodeTest->SetIsOnTheTree(false, 0, 1, 1, 1);

    nodeTest->isOnTheTree_ = true;
    nodeTest->SetIsOnTheTree(false, 0, 1, 1, 1);
    EXPECT_FALSE(nodeTest->isOnTheTree_);

    nodeTest->children_.emplace_back(std::make_shared<RSRenderNode>(0));
    nodeTest->children_.emplace_back(std::make_shared<RSRenderNode>(0));
    std::shared_ptr<RSRenderNode> renderNodeTest = nullptr;
    nodeTest->children_.emplace_back(renderNodeTest);

    nodeTest->disappearingChildren_.emplace_back(std::make_shared<RSRenderNode>(0), 0);
    nodeTest->SetIsOnTheTree(true, 0, 1, 1, 1);
    EXPECT_TRUE(nodeTest->isOnTheTree_);

    // ResetChildRelevantFlags test
    nodeTest->ResetChildRelevantFlags();
    EXPECT_FALSE(nodeTest->hasChildrenOutOfRect_);

    // UpdateChildrenRect test
    RectI subRect = RectI { 0, 0, 1, 1 };
    nodeTest->childrenRect_.Clear();
    nodeTest->UpdateChildrenRect(subRect);
    nodeTest->UpdateChildrenRect(subRect);
}

/**
 * @tc.name: AddCrossParentChildTest008
 * @tc.desc: AddCrossParentChild test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, AddCrossParentChildTest008, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->isOnTheTree_ = false;
    std::shared_ptr<RSRenderNode> childTest = nullptr;
    nodeTest->AddCrossParentChild(childTest, -1);

    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> child3 = std::make_shared<RSRenderNode>(3);
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->AddCrossParentChild(child1, -1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->AddCrossParentChild(child2, 3);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    nodeTest->isOnTheTree_ = true;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->AddCrossParentChild(child3, 4);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
}

/**
 * @tc.name: RemoveCrossParentChildTest009
 * @tc.desc: RemoveCrossParentChild test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, RemoveCrossParentChild009, TestSize.Level1)
{
    // RemoveChild test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSRenderNode> newParent1 = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> newParent2 = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> childTest = nullptr;
    nodeTest->RemoveCrossParentChild(childTest, newParent1);

    childTest = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    childTest->parent_ = parent;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveCrossParentChild(childTest, newParent1);
    EXPECT_TRUE(nodeTest->isFullChildrenListValid_);

    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->disappearingTransitionCount_ = 1;

    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    nodeTest->AddCrossParentChild(child1);
    nodeTest->AddCrossParentChild(child2);

    nodeTest->RemoveCrossParentChild(child1, newParent1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    nodeTest->disappearingTransitionCount_ = 0;
    parent = nullptr;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveCrossParentChild(child2, newParent2);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
}

/**
 * @tc.name: RemoveFromTreeTest010
 * @tc.desc: RemoveCrossParentChild test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, RemoveFromTreeTest010, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSRenderNode> parentTest1 = nullptr;
    nodeTest->parent_ = parentTest1;
    nodeTest->RemoveFromTree(false);

    std::shared_ptr<RSRenderNode> parentTest2 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parentTest2, nullptr);

    nodeTest->parent_ = parentTest2;
    parentTest2->isFullChildrenListValid_ = true;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveFromTree(false);
    EXPECT_TRUE(parentTest2->isFullChildrenListValid_);

    parentTest2->isFullChildrenListValid_ = true;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveFromTree(true);
    EXPECT_FALSE(parentTest2->isFullChildrenListValid_);
}

/**
 * @tc.name: ClearChildrenTest011
 * @tc.desc: ClearChildren test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, ClearChildrenTest011, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    nodeTest->children_.clear();
    nodeTest->ClearChildren();

    nodeTest->disappearingTransitionCount_ = 1;
    nodeTest->children_.emplace_back(std::make_shared<RSRenderNode>(0));
    nodeTest->children_.emplace_back(std::make_shared<RSRenderNode>(0));
    std::shared_ptr<RSRenderNode> renderNodeTest1 = nullptr;
    nodeTest->children_.emplace_back(renderNodeTest1);
    EXPECT_EQ(nodeTest->children_.size(), 3);
    nodeTest->ClearChildren();
    EXPECT_EQ(nodeTest->children_.size(), 0);

    nodeTest->disappearingTransitionCount_ = 0;
    std::shared_ptr<RSRenderNode> renderNodeTest2 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(renderNodeTest2, nullptr);
    renderNodeTest2->disappearingTransitionCount_ = 1;
    nodeTest->children_.emplace_back(renderNodeTest2);
    EXPECT_EQ(nodeTest->children_.size(), 1);
    nodeTest->ClearChildren();
    EXPECT_EQ(nodeTest->children_.size(), 0);

    std::shared_ptr<RSRenderNode> renderNodeTest3 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(renderNodeTest3, nullptr);
    renderNodeTest3->disappearingTransitionCount_ = 0;
    nodeTest->children_.emplace_back(renderNodeTest3);
    EXPECT_EQ(nodeTest->children_.size(), 1);
    nodeTest->ClearChildren();
    EXPECT_EQ(nodeTest->children_.size(), 0);
}

/**
 * @tc.name: DrawSurfaceNodesTest012
 * @tc.desc: IsFirstLevelNode SubSurfaceNodeNeedDraw test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, DrawSurfaceNodesTest012, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->firstLevelNodeId_ = 0;
    EXPECT_TRUE(nodeTest->IsFirstLevelNode());

    std::vector<std::weak_ptr<RSRenderNode>> subSurfaceNodesTest1;
    std::shared_ptr<RSSurfaceRenderNode> surfaceTest1 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(surfaceTest1, nullptr);
    surfaceTest1->SetOldDirtyInSurface(RectI { 0, 0, 1, 1 });
    surfaceTest1->visibleRegion_.rects_.emplace_back(RectI { 0, 0, 2, 2 });
    subSurfaceNodesTest1.emplace_back(surfaceTest1);
    nodeTest->subSurfaceNodes_.emplace(0, subSurfaceNodesTest1);
    auto isSubNodeNeedDrawTest1 = nodeTest->SubSurfaceNodeNeedDraw(PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION);
    EXPECT_TRUE(isSubNodeNeedDrawTest1);
    nodeTest->subSurfaceNodes_.clear();

    std::vector<std::weak_ptr<RSRenderNode>> subSurfaceNodesTest2;
    std::shared_ptr<RSSurfaceRenderNode> surfaceTest2 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(surfaceTest2, nullptr);
    surfaceTest2->SetOldDirtyInSurface(RectI { 0, 0, 1, 1 });
    surfaceTest2->visibleRegion_.rects_.clear();
    std::vector<std::weak_ptr<RSRenderNode>> subSurfaceNodesTest3;
    std::shared_ptr<RSSurfaceRenderNode> surfaceTest3 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(surfaceTest3, nullptr);
    surfaceTest3->SetOldDirtyInSurface(RectI { 0, 0, 1, 1 });
    surfaceTest3->visibleRegion_.rects_.emplace_back(RectI { 0, 0, 2, 2 });
    subSurfaceNodesTest3.emplace_back(surfaceTest3);
    surfaceTest2->subSurfaceNodes_.emplace(0, subSurfaceNodesTest3);
    subSurfaceNodesTest2.emplace_back(surfaceTest2);
    nodeTest->subSurfaceNodes_.emplace(0, subSurfaceNodesTest2);
    auto isSubNodeNeedDrawTest2 = nodeTest->SubSurfaceNodeNeedDraw(PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION);
    EXPECT_TRUE(isSubNodeNeedDrawTest2);
    nodeTest->subSurfaceNodes_.clear();

    auto isSubNodeNeedDrawTest4 = nodeTest->SubSurfaceNodeNeedDraw(PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION);
    EXPECT_FALSE(isSubNodeNeedDrawTest4);
}

/**
 * @tc.name: UpdateDrawingCacheInfoBeforeChildrenTest013
 * @tc.desc: CheckDrawingCacheType and UpdateDrawingCacheInfoBeforeChildren test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoBeforeChildrenTest013, TestSize.Level1)
{
    RSRenderNode node(0);

    // CheckDrawingCacheType test
    node.nodeGroupType_ = RSRenderNode::NONE;
    node.CheckDrawingCacheType();
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    node.nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;
    node.CheckDrawingCacheType();
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);
    node.nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    node.CheckDrawingCacheType();
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);

    // UpdateDrawingCacheInfoBeforeChildren test
    // shouldPaint_ is false
    node.shouldPaint_ = false;
    node.UpdateDrawingCacheInfoBeforeChildren(false);
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    // shouldPaint_ is true
    node.shouldPaint_ = true;
    node.UpdateDrawingCacheInfoBeforeChildren(true);
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    // isScreenRotation is true
    node.nodeGroupType_ = RSRenderNode::NONE;
    node.UpdateDrawingCacheInfoBeforeChildren(false);
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);

    // isScreenRotation is false
    node.nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    node.stagingRenderParams_ = std::move(stagingRenderParams);
    node.UpdateDrawingCacheInfoBeforeChildren(false);
    EXPECT_FALSE(node.stagingRenderParams_->needSync_);
}

/**
 * @tc.name: RemoveModifierTest014
 * @tc.desc: RemoveModifier test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, ProcessTest014, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest1 =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier1 =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest1);
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest2 =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier2 =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest2);

    nodeTest->modifiers_.emplace(0, modifier1);
    nodeTest->modifiers_.emplace(1, modifier2);

    EXPECT_EQ(nodeTest->modifiers_.size(), 2);
    nodeTest->RemoveModifier(1);
    EXPECT_EQ(nodeTest->modifiers_.size(), 1);
    nodeTest->RemoveModifier(1);
    EXPECT_EQ(nodeTest->modifiers_.size(), 1);
}

/**
 * @tc.name: AccmulateDirtyInOcclusionTest015
 * @tc.desc: AccmulateDirtyInOcclusion
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, AccmulateDirtyInOcclusionTest015, TestSize.Level1)
{
    // AccmulateDirtyInOcclusion AccmulateDirtyTypes AccmulateDirtyStatus GetMutableRenderProperties
    // ResetAccmulateDirtyTypes ResetAccmulateDirtyStatus RecordCurDirtyTypes test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->curDirtyTypes_.set(static_cast<size_t>(RSModifierType::PIVOT_Z), true);
    nodeTest->curDirtyTypes_.set(static_cast<size_t>(RSModifierType::PERSP), true);

    nodeTest->AccmulateDirtyInOcclusion(true);
    nodeTest->AccmulateDirtyInOcclusion(false);

    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::PIVOT_Z), true);
    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::PERSP), true);
    nodeTest->RecordCurDirtyTypes();
}

/**
 * @tc.name: GenerateFullChildrenListTest016
 * @tc.desc: RemoveModifier and ApplyPositionZModifier test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, GenerateFullChildrenListTest016, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    nodeTest->isFullChildrenListValid_ = false;
    nodeTest->GenerateFullChildrenList();
    EXPECT_TRUE(nodeTest->isFullChildrenListValid_);

    std::shared_ptr<RSRenderNode> childrenTest1 = nullptr;
    std::shared_ptr<RSRenderNode> childrenTest2 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(childrenTest2, nullptr);
    std::shared_ptr<RSRenderNode> childrenTest3 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(childrenTest3, nullptr);
    nodeTest->children_.emplace_back(childrenTest1);
    nodeTest->isContainBootAnimation_ = true;
    childrenTest2->isBootAnimation_ = false;
    childrenTest2->isBootAnimation_ = true;
    nodeTest->children_.emplace_back(childrenTest2);

    std::shared_ptr<RSRenderNode> disappearingChildrenTest1 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(disappearingChildrenTest1, nullptr);
    std::shared_ptr<RSRenderNode> disappearingChildrenTest2 = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(disappearingChildrenTest2, nullptr);
    disappearingChildrenTest1->isBootAnimation_ = false;
    nodeTest->disappearingChildren_.emplace_back(std::pair(disappearingChildrenTest1, 0));
    nodeTest->disappearingChildren_.emplace_back(std::pair(disappearingChildrenTest2, 1));

    // ApplyPositionZModifier test
    disappearingChildrenTest1->dirtyTypes_.set(static_cast<size_t>(RSModifierType::POSITION_Z), false);
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest1 =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(propertyTest1, nullptr);
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier1 =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest1);
    EXPECT_NE(modifier1, nullptr);
    modifier1->SetType(RSModifierType::POSITION_Z);
    disappearingChildrenTest2->modifiers_.emplace(0, modifier1);

    nodeTest->isChildrenSorted_ = false;
    nodeTest->GenerateFullChildrenList();
    EXPECT_TRUE(nodeTest->isChildrenSorted_);

    disappearingChildrenTest2->dirtyTypes_.set(static_cast<size_t>(RSModifierType::POSITION_Z), true);
    nodeTest->isChildrenSorted_ = false;
    nodeTest->GenerateFullChildrenList();
    EXPECT_TRUE(nodeTest->isChildrenSorted_);
}

/**
 * @tc.name: ApplyModifiersTest017
 * @tc.desc: ApplyModifiers test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, ApplyModifiersTest017, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest1 =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(propertyTest1, nullptr);
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier1 =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest1);
    EXPECT_NE(modifier1, nullptr);
    modifier1->SetType(RSModifierType::POSITION_Z);
    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::POSITION_Z), false);
    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::SANDBOX), true);
    nodeTest->modifiers_.emplace(0, modifier1);
    nodeTest->isFullChildrenListValid_ = false;
    nodeTest->renderContent_->renderProperties_.alpha_ = 0.0f;
    RSRenderNode::SharedPtr inNode = std::make_shared<RSRenderNode>(0);
    RSRenderNode::SharedPtr outNode = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<SharedTransitionParam> sharedTransitionParam =
        std::make_shared<SharedTransitionParam>(inNode, outNode);
    nodeTest->sharedTransitionParam_ = sharedTransitionParam;
    nodeTest->GenerateFullChildrenList();
    EXPECT_TRUE(nodeTest->isFullChildrenListValid_);

    nodeTest->modifiers_.clear();
    nodeTest->isChildrenSorted_ = false;
    nodeTest->GenerateFullChildrenList();

    nodeTest->isChildrenSorted_ = true;
    nodeTest->childrenHasSharedTransition_ = true;
    nodeTest->GenerateFullChildrenList();

    nodeTest->dirtyTypes_.reset();
    EXPECT_TRUE(nodeTest->dirtyTypes_.none());
    nodeTest->childrenHasSharedTransition_ = false;
    nodeTest->GenerateFullChildrenList();
}

/**
 * @tc.name: InvalidateHierarchyTest018
 * @tc.desc: MarkParentNeedRegenerateChildren and UpdateDrawableVec UpdateDrawableVecInternal test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, InvalidateHierarchyTest018, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest1 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest1, nullptr);

    std::shared_ptr<RSRenderNode> parentTest1 = nullptr;
    nodeTest1->parent_ = parentTest1;
    nodeTest1->MarkParentNeedRegenerateChildren();
    std::shared_ptr<RSRenderNode> parentTest2 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parentTest2, nullptr);
    nodeTest1->parent_ = parentTest2;
    nodeTest1->MarkParentNeedRegenerateChildren();
    EXPECT_FALSE(parentTest2->isChildrenSorted_);

    nodeTest1->SetIsUsedBySubThread(false);
    nodeTest1->renderContent_->renderProperties_.pixelStretch_ = 1.0f;
    nodeTest1->dirtyTypes_.set(static_cast<size_t>(RSModifierType::BOUNDS), true);
    nodeTest1->dirtyTypes_.set(static_cast<size_t>(RSModifierType::SCALE), true);
    std::unique_ptr<PropertyDrawableTest> property = std::make_unique<PropertyDrawableTest>();
    nodeTest1->renderContent_->propertyDrawablesVec_.at(0) = std::move(property);
    nodeTest1->UpdateDrawableVec();

    std::shared_ptr<RSRenderNode> nodeTest2 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest2, nullptr);
    nodeTest2->SetIsUsedBySubThread(true);
    std::shared_ptr<RSContext> contextTest = std::make_shared<RSContext>();
    nodeTest2->context_ = contextTest;
    nodeTest2->UpdateDrawableVec();
}

/**
 * @tc.name: UpdateDrawableVecV2Test019
 * @tc.desc: UpdateDrawableVecV2 test
 * @tc.type: FUNC
 * @tc.require: issueI9SE93
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawableVecV2Test019, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    nodeTest->UpdateDrawableVecV2();

    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::BOUNDS), true);
    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::ROTATION_X), true);
    std::shared_ptr<DrawableTest> drawableTest1 = std::make_shared<DrawableTest>();
    nodeTest->drawableVec_.at(1) = drawableTest1;
    EXPECT_TRUE(nodeTest->dirtySlots_.empty());

    nodeTest->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    nodeTest->UpdateDrawableVecV2();
    auto sum = nodeTest->dirtySlots_.size();
    EXPECT_NE(nodeTest->dirtySlots_.size(), 0);

    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::PIVOT), true);
    std::shared_ptr<DrawableTest> drawableTest2 = std::make_shared<DrawableTest>();
    nodeTest->drawableVec_.at(4) = drawableTest2;
    RSShadow rsShadow;
    std::optional<RSShadow> shadow(rsShadow);
    shadow->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE;
    nodeTest->renderContent_->renderProperties_.shadow_ = shadow;
    RRect rrect;
    nodeTest->renderContent_->renderProperties_.rrect_ = rrect;
    nodeTest->UpdateDrawableVecV2();
    EXPECT_NE(nodeTest->dirtySlots_.size(), sum);
}
} // namespace Rosen
} // namespace OHOS