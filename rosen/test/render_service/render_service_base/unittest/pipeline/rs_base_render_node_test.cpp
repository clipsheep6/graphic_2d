/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_render_thread_visitor.h"
#include "platform/common/rs_log.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSBaseRenderNodeTest::SetUpTestCase() {}
void RSBaseRenderNodeTest::TearDownTestCase() {}
void RSBaseRenderNodeTest::SetUp() {}
void RSBaseRenderNodeTest::TearDown() {}

/**
 * @tc.name: AddChild001
 * @tc.desc: test results of AddChild
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSBaseRenderNodeTest, AddChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto childtwo = std::make_shared<RSBaseRenderNode>(id + 2, context);
    int index = -1;
    node->SetIsOnTheTree(true);
    node->AddChild(node, index);
    ASSERT_EQ(node->GetChildrenCount(), 0);

    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);
    ASSERT_TRUE(childone->IsOnTheTree());

    index = 0;
    node->AddChild(childtwo, index);
    ASSERT_EQ(node->GetChildrenCount(), 2);
    ASSERT_TRUE(childtwo->IsOnTheTree());

    node->AddChild(childtwo, index);
    ASSERT_EQ(node->GetChildrenCount(), 2);
}

/**
 * @tc.name: RemoveChild001
 * @tc.desc: test results of RemoveChild
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSBaseRenderNodeTest, RemoveChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. RemoveChild
     */
    bool skipTransition = false;
    node->RemoveChild(childone, skipTransition);
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: ClearChildren001
 * @tc.desc: test results of ClearChildren
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSBaseRenderNodeTest, ClearChildren001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->ClearChildren();
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: MoveChild001
 * @tc.desc: test results of MoveChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, MoveChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->MoveChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);
}

/**
 * @tc.name: SetIsOnTheTree001
 * @tc.desc: test results of SetIsOnTheTree
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, SetIsOnTheTree001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = nullptr;
    int index = -1;
    node->SetIsOnTheTree(true);
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 0);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->ClearChildren();
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: AddCrossParentChild001
 * @tc.desc: test results of AddCrossParentChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, AddCrossParentChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddCrossParentChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->ClearChildren();
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: RemoveCrossParentChild001
 * @tc.desc: test results of RemoveCrossParentChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, RemoveCrossParentChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto newParent = std::make_shared<RSBaseRenderNode>(id + 2, context);
    int index = -1;
    node->AddCrossParentChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->RemoveCrossParentChild(childone, newParent);
}

/**
 * @tc.name: SetIsOnTheTree002
 * @tc.desc: test results of SetIsOnTheTree
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, SetIsOnTheTree002, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = nullptr;
    int index = -1;
    node->SetIsOnTheTree(true);
    node->AddChild(childone, index);
    bool skipTransition = false;
    node->RemoveChild(childone, skipTransition);
    EXPECT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: AddCrossParentChildTest001
 * @tc.desc: test results of AddCrossParentChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, AddCrossParentChildTest001, TestSize.Level1)
{
    int32_t index = 1;
    int32_t index_ = 0;
    std::shared_ptr<RSBaseRenderNode> child = nullptr;
    std::shared_ptr<RSBaseRenderNode> child_ = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->AddCrossParentChild(child, index);
    node->AddCrossParentChild(child_, index_);
    ASSERT_EQ(node->GetChildrenCount(), 1);
}

/**
 * @tc.name: RemoveCrossParentChildTest001
 * @tc.desc: test results of RemoveCrossParentChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, RemoveCrossParentChildTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSBaseRenderNode> child = nullptr;
    std::weak_ptr<RSBaseRenderNode> newParent;
    node->RemoveCrossParentChild(child, newParent);
}

/**
 * @tc.name: RemoveFromTreeTest
 * @tc.desc: test results of RemoveFromTree
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, RemoveFromTreeTest, TestSize.Level1)
{
    bool skipTransition = false;
    bool skipTransition_ = true;
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->RemoveFromTree(skipTransition);
    node->RemoveFromTree(skipTransition_);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: PrepareTest
 * @tc.desc: test results of Prepare
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, PrepareTest, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node->Prepare(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsPureContainerTest
 * @tc.desc: test results of IsPureContainer
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, IsPureContainer, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_TRUE(node->IsPureContainer());
}

/**
 * @tc.name: IsContentNodeTest
 * @tc.desc: test results of IsContentNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, IsContentNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_TRUE(node->IsContentNode());
}

/**
 * @tc.name: SetContainBootAnimation
 * @tc.desc: test results of SetContainBootAnimation
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, SetContainBootAnimation, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    bool isContainBootAnimation = true;
    node->SetContainBootAnimation(isContainBootAnimation);
    ASSERT_TRUE(node->isContainBootAnimation_);
}

/**
 * @tc.name: UpdateChildrenRect
 * @tc.desc: test results of UpdateChildrenRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, UpdateChildrenRect, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RectI rect(1, 1, 1, 1);
    node->UpdateChildrenRect(rect);
    EXPECT_TRUE(!node->GetChildrenRect().IsEmpty());

    node->childrenRect_.Clear();
    node->UpdateChildrenRect(rect);
    EXPECT_TRUE(!node->GetChildrenRect().IsEmpty());

    rect.Clear();
    node->UpdateChildrenRect(rect);
    node->childrenRect_.Clear();
    ASSERT_TRUE(node->GetChildrenRect().IsEmpty());
}

/**
 * @tc.name: SetParent
 * @tc.desc: test results of SetParent
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, SetParent, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto parent = std::make_shared<RSBaseRenderNode>(id + 1, context);
    node->SetParent(parent);
    ASSERT_NE(node->parent_.lock(), nullptr);
}

/**
 * @tc.name: ResetParent
 * @tc.desc: test results of ResetParent
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, ResetParent, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->ResetParent();
    ASSERT_EQ(node->parent_.lock(), nullptr);
}

/**
 * @tc.name: SubSurfaceNodeNeedDraw
 * @tc.desc: test results of SubSurfaceNodeNeedDraw
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, SubSurfaceNodeNeedDraw, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto parent = std::make_shared<RSBaseRenderNode>(id + 1, context);
    node->AddSubSurfaceNode(parent);
    PartialRenderType opDropType = PartialRenderType::SET_DAMAGE;
    ASSERT_FALSE(parent->SubSurfaceNodeNeedDraw(opDropType));
}

/**
 * @tc.name: AddSubSurfaceNode
 * @tc.desc: test results of AddSubSurfaceNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, AddSubSurfaceNode, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto parent = std::make_shared<RSBaseRenderNode>(id + 1, context);
    node->AddSubSurfaceNode(parent);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RemoveSubSurfaceNode
 * @tc.desc: test results of RemoveSubSurfaceNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, RemoveSubSurfaceNode, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto parent = std::make_shared<RSBaseRenderNode>(id + 1, context);
    node->RemoveSubSurfaceNode(parent);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DumpTree
 * @tc.desc: test results of DumpTree
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, DumpTree, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    int32_t depth = 0;
    std::string out = "string";
    node->DumpTree(depth, out);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DumpNodeType
 * @tc.desc: test results of DumpNodeType
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, DumpNodeType, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::string out = "string";
    node->DumpNodeType(out);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DumpSubClassNode
 * @tc.desc: test results of DumpSubClassNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, DumpSubClassNode, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::string out = "string";
    node->DumpSubClassNode(out);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DumpDrawCmdModifiers
 * @tc.desc: test results of DumpDrawCmdModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, DumpDrawCmdModifiers, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::string out = "string";
    node->DumpDrawCmdModifiers(out);

    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSGeometryTransRenderModifier>(property) };
    std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>> map;
    map[RSModifierType::ENV_FOREGROUND_COLOR] = list;
    node->renderContent_->drawCmdModifiers_ = map;
    node->DumpDrawCmdModifiers(out);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DumpDrawCmdModifier
 * @tc.desc: test results of DumpDrawCmdModifier
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, DumpDrawCmdModifier, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::string propertyDesc = "noDesc";
    RSModifierType type;
    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    std::shared_ptr<RSRenderModifier> modifier = modifierCast;

    type = RSModifierType::ENV_FOREGROUND_COLOR;
    node->DumpDrawCmdModifier(propertyDesc, type, modifier);

    type = RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY;
    node->DumpDrawCmdModifier(propertyDesc, type, modifier);

    type = RSModifierType::GEOMETRYTRANS;
    node->DumpDrawCmdModifier(propertyDesc, type, modifier);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetContentDirty
 * @tc.desc: test results of SetContentDirty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, SetContentDirty, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->SetContentDirty();
    ASSERT_TRUE(node->isContentDirty_);
}

/**
 * @tc.name: SetDirty
 * @tc.desc: test results of SetDirty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, SetDirty, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    bool forceAddToActiveList = true;
    node->SetDirty(forceAddToActiveList);

    node->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    node->SetDirty(forceAddToActiveList);
    ASSERT_EQ(node->dirtyStatus_, RSRenderNode::NodeDirty::DIRTY);
}

/**
 * @tc.name: CollectSurface
 * @tc.desc: test results of CollectSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, CollectSurface, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::weak_ptr<RSContext> contextArgs = {};
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(id + 1, contextArgs);
    std::vector<RSRenderNode::SharedPtr> vec;
    bool isUniRender = true;
    bool onlyFirstLevel = true;
    node->CollectSurface(renderNode, vec, isUniRender, onlyFirstLevel);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: CollectSurfaceForUIFirstSwitch
 * @tc.desc: test results of CollectSurfaceForUIFirstSwitch
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, CollectSurfaceForUIFirstSwitch, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    uint32_t leashWindowCount = 2;
    uint32_t minNodeNum = 1;
    node->CollectSurfaceForUIFirstSwitch(leashWindowCount, minNodeNum);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Prepare
 * @tc.desc: test results of Prepare
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, Prepare, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(1);
    std::shared_ptr<RSNodeVisitor> visitor;
    node->Prepare(visitor);

    std::shared_ptr<RSNodeVisitor> visitorTwo = std::make_shared<RSRenderThreadVisitor>();
    node->Prepare(visitorTwo);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Process
 * @tc.desc: test results of Process
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, Process, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSNodeVisitor> visitor;
    node->Process(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SendCommandFromRT
 * @tc.desc: test results of SendCommandFromRT
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, SendCommandFromRT, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    uint64_t timeoutNS = 1;
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderPropertyBase>();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSNodeGetShowingPropertyAndCancelAnimation>(id, property, timeoutNS);
    NodeId nodeId = 0;
    node->SendCommandFromRT(command, nodeId);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: InternalRemoveSelfFromDisappearingChildren
 * @tc.desc: test results of InternalRemoveSelfFromDisappearingChildren
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, InternalRemoveSelfFromDisappearingChildren, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto parent = std::make_shared<RSBaseRenderNode>(id + 1, context);
    node->SetParent(parent);
    node->InternalRemoveSelfFromDisappearingChildren();

    std::weak_ptr<RSBaseRenderNode> contextArgs;
    node->SetParent(contextArgs);
    node->InternalRemoveSelfFromDisappearingChildren();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: FallbackAnimationsToRoot
 * @tc.desc: test results of FallbackAnimationsToRoot
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, FallbackAnimationsToRoot, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->FallbackAnimationsToRoot();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ActivateDisplaySync
 * @tc.desc: test results of ActivateDisplaySync
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, ActivateDisplaySync, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->ActivateDisplaySync();
    node->ActivateDisplaySync();
    ASSERT_NE(node->displaySync_, nullptr);
}

/**
 * @tc.name: UpdateDisplaySyncRange
 * @tc.desc: test results of UpdateDisplaySyncRange
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, UpdateDisplaySyncRange, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->UpdateDisplaySyncRange();

    node->displaySync_ = std::make_shared<RSRenderDisplaySync>(1);
    node->UpdateDisplaySyncRange();
    ASSERT_NE(node->displaySync_, nullptr);
}

/**
 * @tc.name: Animate
 * @tc.desc: test results of Animate
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, Animate, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    int64_t timestamp = 4;
    int64_t period = 2;
    bool isDisplaySyncEnabled = true;
    node->Animate(timestamp, period, isDisplaySyncEnabled);

    node->displaySync_ = std::make_shared<RSRenderDisplaySync>(1);
    node->Animate(timestamp, period, isDisplaySyncEnabled);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Update
 * @tc.desc: test results of Update
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, Update, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RSDirtyRegionManager dirtyManager;
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(id + 1);
    bool parentDirty = true;
    std::optional<RectI> clipRect;
    node->Update(dirtyManager, parent, parentDirty, clipRect);

    node->shouldPaint_ = false;
    node->isLastVisible_ = true;
    ASSERT_TRUE(node->Update(dirtyManager, parent, parentDirty, clipRect));
}

/**
 * @tc.name: UpdateBufferDirtyRegion
 * @tc.desc: test results of UpdateBufferDirtyRegion
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, UpdateBufferDirtyRegion, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RectI dirtyRect(1, 1, 1, 1);
    RectI drawRegion;
    node->UpdateBufferDirtyRegion(dirtyRect, drawRegion);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDirtyRegion
 * @tc.desc: test results of UpdateDirtyRegion
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, UpdateDirtyRegion, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RSDirtyRegionManager dirtyManager;
    bool geoDirty = false;
    std::optional<RectI> clipRect;
    node->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    node->UpdateDirtyRegion(dirtyManager, geoDirty, clipRect);

    geoDirty = true;
    node->UpdateDirtyRegion(dirtyManager, geoDirty, clipRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsSelfDrawingNode
 * @tc.desc: test results of IsSelfDrawingNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, IsSelfDrawingNode, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_FALSE(node->IsSelfDrawingNode());
}

/**
 * @tc.name: IsDirty
 * @tc.desc: test results of IsDirty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, IsDirty, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_FALSE(node->IsDirty());
}

/**
 * @tc.name: IsContentDirty
 * @tc.desc: test results of IsContentDirty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, IsContentDirty, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_FALSE(node->IsContentDirty());
}

/**
 * @tc.name: UpdateRenderStatus
 * @tc.desc: test results of UpdateRenderStatus
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, UpdateRenderStatus, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RectI dirtyRegion(1, 1, 1, 1);
    bool isPartialRenderEnabled = false;
    node->UpdateRenderStatus(dirtyRegion, isPartialRenderEnabled);

    isPartialRenderEnabled = true;
    node->UpdateRenderStatus(dirtyRegion, isPartialRenderEnabled);
    ASSERT_TRUE(node->isRenderUpdateIgnored_);
}

/**
 * @tc.name: UpdateParentChildrenRect
 * @tc.desc: test results of UpdateParentChildrenRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, UpdateParentChildrenRect, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSRenderNode> parentNode;
    node->UpdateParentChildrenRect(parentNode);

    parentNode = std::make_shared<RSRenderNode>(id + 1);
    node->UpdateParentChildrenRect(parentNode);

    node->shouldPaint_ = false;
    node->UpdateParentChildrenRect(parentNode);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsBackgroundFilterCacheValid
 * @tc.desc: test results of IsBackgroundFilterCacheValid
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, IsBackgroundFilterCacheValid, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_FALSE(node->IsBackgroundFilterCacheValid());
}

/**
 * @tc.name: UpdateFilterCacheWithDirty
 * @tc.desc: test results of UpdateFilterCacheWithDirty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, UpdateFilterCacheWithDirty, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RSDirtyRegionManager dirtyManager;
    bool isForeground = true;
    node->UpdateFilterCacheWithDirty(dirtyManager, isForeground);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RenderTraceDebug
 * @tc.desc: test results of RenderTraceDebug
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, RenderTraceDebug, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->RenderTraceDebug();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: AddModifier
 * @tc.desc: test results of AddModifier
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, AddModifier, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSRenderModifier> modifier;
    bool isSingleFrameComposer = true;
    node->AddModifier(modifier, isSingleFrameComposer);

    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    std::shared_ptr<RSRenderModifier> modifierTwo = modifierCast;
    node->AddModifier(modifierTwo, isSingleFrameComposer);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: AddGeometryModifier
 * @tc.desc: test results of AddGeometryModifier
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, AddGeometryModifier, TestSize.Level1)
{
    {
        auto node = std::make_shared<RSBaseRenderNode>(id, context);
        Drawing::Matrix matrix;
        PropertyId id = 1;
        std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
            std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
        std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
            std::make_shared<RSGeometryTransRenderModifier>(property);
        std::shared_ptr<RSRenderModifier> modifier = modifierCast;
        node->AddGeometryModifier(modifier);

        modifierCast->drawStyle_ = RSModifierType::BOUNDS;
        node->AddGeometryModifier(modifier);

        node->boundsModifier_ = modifier;
        node->AddGeometryModifier(modifier);
        ASSERT_NE(node->boundsModifier_, nullptr);
    }

    {
        auto node = std::make_shared<RSBaseRenderNode>(id, context);
        Drawing::Matrix matrix;
        PropertyId id = 1;
        std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
            std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
        std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
            std::make_shared<RSGeometryTransRenderModifier>(property);
        modifierCast->drawStyle_ = RSModifierType::FRAME;
        std::shared_ptr<RSRenderModifier> modifier = modifierCast;
        node->AddGeometryModifier(modifier);

        node->frameModifier_ = modifier;
        node->AddGeometryModifier(modifier);
        ASSERT_NE(node->frameModifier_, nullptr);
    }
}
} // namespace OHOS::Rosen