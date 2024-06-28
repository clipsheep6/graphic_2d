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

#include "rs_animation_base_test.h"
#include "rs_animation_test_utils.h"

#include "animation/rs_symbol_animation.h"
#include "animation/rs_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSSymbolAnimationTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: SetSymbolAnimation001
 * @tc.desc: SetSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    /**
     * @tc.steps: step2.1 rsNode_ is nullptr, symbolAnimationConfig is nullptr
     */
    bool flag1 = symbolAnimation.SetSymbolAnimation(nullptr);
    EXPECT_TRUE(flag1 == false);
    /**
     * @tc.steps: step2.2 rsNode_ is nullptr, symbolAnimationConfig is not nullptr
     */
    bool flag2 = symbolAnimation.SetSymbolAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag2 == false);
    /**
     * @tc.steps: step2.3 rsNode_ is not nullptr, symbolAnimationConfig is nullptr
     */
    symbolAnimation.SetNode(rootNode);
    bool flag3 = symbolAnimation.SetSymbolAnimation(nullptr);
    EXPECT_TRUE(flag3 == false);
    /**
     * @tc.steps: step2.4 rsNode_ is not nullptr, symbolAnimationConfig is not nullptr
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::NONE;
    symbolAnimationConfig->symbolSpanId = 1996; // the 1996 is the unique ID of a symbol
    bool flag4 = symbolAnimation.SetSymbolAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag4 == true);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolAnimation001 end";
}

/**
 * @tc.name: SetSymbolAnimation002
 * @tc.desc: SetSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolAnimation002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 1996; // the 1996 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::VARIABLE_COLOR;
    auto newCanvasNode = RSCanvasNode::Create();
    rootNode->canvasNodesListMap[symbolAnimationConfig->symbolSpanId] = {{newCanvasNode->GetId(), newCanvasNode}};
    /**
     * @tc.steps: step2.1 test variable_color animation
     */
    bool flag1 = symbolAnimation.SetSymbolAnimation(symbolAnimationConfig);
    EXPECT_TRUE(flag1 == false);
    /**
     * @tc.steps: step2.2 test replace animation
     */
    symbolAnimationConfig->symbolSpanId++;
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::REPLACE_APPEAR;
    symbolAnimation.SetSymbolAnimation(symbolAnimationConfig);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolAnimation002 end";
}

/**
 * @tc.name: SetPublicAnimation001
 * @tc.desc: SetPublicAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetPublicAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 19; // the 19 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::SCALE;
    symbolAnimationConfig->animationMode = 1; // 1 is wholeymbol
    symbolAnimationConfig->animationStart = true;
    // init symbolNode
    Drawing::Path path;
    path.AddCircle(100, 100, 50); // 100 x, 100y, 50 radius
    Drawing::DrawingHMSymbolData symbol;
    symbol.path_ = path;
    TextEngine::SymbolNode symbolNode1;
    symbolNode1.symbolData = symbol;
    symbolNode1.nodeBoundary = {100, 100, 50, 50}; // 100 x, 100 y, 50 width, 50 height
    symbolAnimationConfig->symbolNodes.push_back(symbolNode1);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    /**
     * @tc.steps: step2.1 test SCALE animation withe wholeymbol
     */
    symbolAnimation.SetPublicAnimation(symbolAnimationConfig);
    /**
     * @tc.steps: step2.2 test SCALE animation withe two symbolNode and maskLayer
     */
    symbolAnimationConfig->animationMode = 0; // 0 is bylayer
    TextEngine::SymbolNode symbolNode2;
    symbolNode2.animationIndex = 1;
    symbolNode2.isMask = true;
    symbolAnimationConfig->symbolNodes.push_back(symbolNode2);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    symbolAnimation.SetPublicAnimation(symbolAnimationConfig);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation001 end";
}

/**
 * @tc.name: SetPublicAnimation002
 * @tc.desc: SetPublicAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetPublicAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 20; // the 20 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::SCALE;
    symbolAnimationConfig->animationStart = true;
    symbolAnimationConfig->animationMode = 0; // 0 is bylayer
    // init symbolNode
    Drawing::Path path;
    path.AddCircle(100, 100, 50); // 100 x, 100y, 50 radius
    Drawing::DrawingHMSymbolData symbol;
    symbol.path_ = path;
    TextEngine::SymbolNode symbolNode1;
    symbolNode1.symbolData = symbol;
    symbolNode1.nodeBoundary = {100, 100, 50, 50}; // 100 x, 100 y, 50 width, 50 height
    symbolAnimationConfig->symbolNodes.push_back(symbolNode1);
    TextEngine::SymbolNode symbolNode2;
    symbolNode2.animationIndex = 1;
    symbolAnimationConfig->symbolNodes.push_back(symbolNode2);
    TextEngine::SymbolNode symbolNode3;
    symbolNode3.animationIndex = -1; // -1 is the symbolNode whitout effect
    symbolAnimationConfig->symbolNodes.push_back(symbolNode3);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    /**
     * @tc.steps: step2. start test SCALE animation withe three symbolnodes
     */
    symbolAnimation.SetPublicAnimation(symbolAnimationConfig);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation002 end";
}

/**
 * @tc.name: SetPublicAnimation003
 * @tc.desc: SetPublicAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetPublicAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation003 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 21; // the 20 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::SCALE;
    symbolAnimationConfig->animationStart = true;
    // init symbolNode
    TextEngine::SymbolNode symbolNode;
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolNode.animationIndex = 1; // 1 is index of animation
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolNode.animationIndex = 2; // 2 is index of animation
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolNode.animationIndex = 3; // 3 is index of animation
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolNode.animationIndex = 4; // 4 is index of animation
    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    /**
     * @tc.steps: step2.1 start test SCALE animation withe five symbolnodes
     */
    symbolAnimation.SetPublicAnimation(symbolAnimationConfig);
    /**
     * @tc.steps: step2.2 start test VARIABLE_COLOR animation withe five symbolnodes
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::VARIABLE_COLOR;
    symbolAnimation.SetPublicAnimation(symbolAnimationConfig);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetPublicAnimation003 end";
}

/**
 * @tc.name: SetDisappearConfig001
 * @tc.desc: SetDisappearConfig of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetDisappearConfig001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetDisappearConfig001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    auto disappearConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    std::shared_ptr<TextEngine::SymbolAnimationConfig> nullptrConfig = nullptr;
    /**
     * @tc.steps: step2 start test SetDisappearConfig
     */
    bool flag1 = symbolAnimation.SetDisappearConfig(nullptrConfig, nullptrConfig);
    EXPECT_TRUE(flag1 == false);

    bool flag2 = symbolAnimation.SetDisappearConfig(symbolAnimationConfig, nullptrConfig);
    EXPECT_TRUE(flag2 == false);

    bool flag3 = symbolAnimation.SetDisappearConfig(nullptrConfig, disappearConfig);
    EXPECT_TRUE(flag3 == false);

    bool flag4 = symbolAnimation.SetDisappearConfig(symbolAnimationConfig, disappearConfig);
    EXPECT_TRUE(flag4 == true);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetDisappearConfig001 end";
}

/**
 * @tc.name: SetDisappearConfig002
 * @tc.desc: SetDisappearConfig of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetDisappearConfig002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetDisappearConfig001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    auto disappearConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    // init replaceNodesSwapMap
    OHOS::Rosen::AnimationNodeConfig animationNodeConfig;
    std::unordered_map<NodeId, OHOS::Rosen::AnimationNodeConfig> nodeMap;
    nodeMap.insert({1999, animationNodeConfig}); // 1999 is a nodeId
    rootNode->replaceNodesSwapMap.insert({APPEAR_STATUS, nodeMap});
    /**
     * @tc.steps: step2 start test SetDisappearConfig
     */
    bool flag1 = symbolAnimation.SetDisappearConfig(symbolAnimationConfig, disappearConfig);
    EXPECT_TRUE(flag1 == true);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetDisappearConfig001 end";
}

/**
 * @tc.name: SetReplaceAnimation001
 * @tc.desc: SetReplaceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetReplaceAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetReplaceAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->symbolSpanId = 96; // 96 is the unique ID of a symbol
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::REPLACE_APPEAR;
    symbolAnimationConfig->animationMode = 1; // 1 is wholesymbol
    symbolAnimationConfig->animationStart = true;
    // init symbolNode
    Drawing::Path path;
    path.AddCircle(100, 100, 50); // 100 x, 100y, 50 radius
    Drawing::DrawingHMSymbolData symbol;
    symbol.path_ = path;
    TextEngine::SymbolNode symbolNode1;
    symbolNode1.symbolData = symbol;
    symbolNode1.nodeBoundary = {100, 100, 50, 50}; // 100 x, 100 y, 50 width, 50 height
    symbolAnimationConfig->symbolNodes.push_back(symbolNode1);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
    /**
     * @tc.steps: step2.1 start test
     */
    symbolAnimation.SetReplaceAnimation(symbolAnimationConfig);
    /**
     * @tc.steps: step2.2 start test replace two node
     */
    symbolAnimation.SetReplaceAnimation(symbolAnimationConfig);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetReplaceAnimation001 end";
}

/**
 * @tc.name: ChooseAnimation001
 * @tc.desc: ChooseAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ChooseAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ChooseAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto newCanvasNode = RSCanvasNode::Create();
    std::vector<Drawing::DrawingPiecewiseParameter> parameters;
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    /**
     * @tc.steps: step2.1 test SCALE
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::SCALE;
    symbolAnimation.ChooseAnimation(newCanvasNode, parameters, symbolAnimationConfig);
    /**
     * @tc.steps: step2.2 test VARIABLE_COLOR
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::VARIABLE_COLOR;
    symbolAnimation.ChooseAnimation(newCanvasNode, parameters, symbolAnimationConfig);
    /**
     * @tc.steps: step2.3 test SYMBOL_PULSE
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::PULSE;
    symbolAnimation.ChooseAnimation(newCanvasNode, parameters, symbolAnimationConfig);
        /**
     * @tc.steps: step2.4 test NONE
     */
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::NONE;
    bool flag = symbolAnimation.ChooseAnimation(newCanvasNode, parameters, symbolAnimationConfig);
    EXPECT_TRUE(flag == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ChooseAnimation001 end";
}

/**
 * @tc.name: InitSupportAnimationTableTest
 * @tc.desc: Verify Init of SupportAnimationTable
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, InitSupportAnimationTableTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest InitSupportAnimationTable start";
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.InitSupportAnimationTable(); // init data
    symbolAnimation.InitSupportAnimationTable(); // if data exists, data will not init again
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest InitSupportAnimationTable end";
}

/**
 * @tc.name: ReplaceAnimationTest001
 * @tc.desc: Verify the basic ability, ReplaceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ReplaceAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ReplaceAnimationTest001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> disappearGroupParas =
        {DISAPPEAR_FIRST_PHASE_PARAS, DISAPPEAR_SECOND_PHASE_PARAS};
    std::vector<Drawing::DrawingPiecewiseParameter> appearGroupParas =
        {APPEAR_FIRST_PHASE_PARAS, APPEAR_SECOND_PHASE_PARAS};
    std::shared_ptr<RSCanvasNode> appearCanvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(ANIMATION_START_BOUNDS);
    canvasNode->SetFrame(ANIMATION_START_BOUNDS);
    /**
     * @tc.steps: step2. start ReplaceAnimation test
     */
    symbolAnimation.AppearAnimation(canvasNode, disappearGroupParas);
    symbolAnimation.AppearAnimation(canvasNode, appearGroupParas);

    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ReplaceAnimationTest001 end";
}

/**
 * @tc.name: PopNodeFromReplaceListTest001
 * @tc.desc: Verify PopNode From Replace list map
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, PopNodeFromReplaceListTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest PopNodeFromReplaceListTest001 start";
    /**
     * @tc.steps: step1.1 init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    uint64_t symbolSpanId = 1;
    rootNode->canvasNodesListMap[symbolSpanId] = {{canvasNode->GetId(), canvasNode}};
    symbolAnimation.PopNodeFromReplaceList(symbolSpanId);
    /**
     * @tc.steps: step1.2 if symbolSpanId not in canvasNodesListMap
     */
    symbolSpanId = 9999; // random value
    symbolAnimation.PopNodeFromReplaceList(symbolSpanId);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest PopNodeFromReplaceListTest001 end";
}

/**
 * @tc.name: PopNodeFromReplaceListTest002
 * @tc.desc: Verify PopNode From Replace list map
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, PopNodeFromReplaceListTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest PopNodeFromReplaceListTest002 start";
    /**
     * @tc.steps: step1 if INVALID_STATUS&APPEAR_STATUS in replaceNodesSwapMap
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    uint64_t symbolSpanId = 1;
    rootNode->canvasNodesListMap[symbolSpanId] = {{canvasNode->GetId(), canvasNode}};
    Drawing::Path path;
    path.AddCircle(100, 100, 50); // 100 x, 100y, 50 radius
    Drawing::DrawingHMSymbolData symbol;
    symbol.path_ = path;
    TextEngine::SymbolNode symbolNode;
    symbolNode.symbolData = symbol;
    symbolNode.nodeBoundary = {100, 100, 50, 50};
    AnimationNodeConfig appearNodeConfig = {symbolNode, symbolNode.animationIndex};
    rootNode->replaceNodesSwapMap[1] = {{canvasNode->GetId(), appearNodeConfig}}; // APPEAR_STATUS = 1
    rootNode->replaceNodesSwapMap[-1] = {{canvasNode->GetId(), appearNodeConfig}}; // INVALID_STATUS = -1
    symbolAnimation.PopNodeFromReplaceList(symbolSpanId);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest PopNodeFromReplaceListTest002 end";
}

/**
 * @tc.name: BounceAnimationTest001
 * @tc.desc: Verify the basic ability, BounceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, BounceAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {BOUNCE_FIRST_PHASE_PARAS,
                                                                     BOUNCE_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start BounceAnimation test
     */
    symbolAnimation.BounceAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest001 end";
}

/**
 * @tc.name: BounceAnimationTest002
 * @tc.desc: Verify the basic ability, BounceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, BounceAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest002 start";
    /**
     * @tc.steps: step1. if node is nullptr
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {BOUNCE_FIRST_PHASE_PARAS,
                                                                     BOUNCE_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start BounceAnimation test, no animation
     */
    symbolAnimation.BounceAnimation(nullptr, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest002 end";
}

/**
 * @tc.name: BounceAnimationTest003
 * @tc.desc: Verify the basic ability, BounceAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, BounceAnimationTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest003 start";
    /**
     * @tc.steps: step1. if parameters.size() < animationStageNum
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {BOUNCE_FIRST_PHASE_PARAS};
    /**
     * @tc.steps: step2. start BounceAnimation test, no animation
     */
    symbolAnimation.BounceAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest BounceAnimationTest003 end";
}

/**
 * @tc.name: ScaleAnimationBase001
 * @tc.desc: Verify the basic ability, ScaleAnimationBase of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ScaleAnimationBase001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    auto bounceSecondPhaseParas = BOUNCE_SECOND_PHASE_PARAS;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty = nullptr;
    symbolAnimation.AddScaleBaseModifier(canvasNode, bounceSecondPhaseParas, scaleProperty);
    symbolAnimation.ScaleAnimationBase(scaleProperty, bounceSecondPhaseParas, groupAnimation);
    /**
     * @tc.steps: step2. start ScaleAnimationBase test
     */
    EXPECT_FALSE(groupAnimation.empty());
    EXPECT_FALSE(groupAnimation[0] == nullptr);
    groupAnimation[0]->Start(canvasNode);
    EXPECT_TRUE(groupAnimation[0]->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase001 end";
}

/**
 * @tc.name: ScaleAnimationBase002
 * @tc.desc: Verify the basic ability, ScaleAnimationBase of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, ScaleAnimationBase002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    Drawing::DrawingPiecewiseParameter testParas = {};
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty = nullptr;
    symbolAnimation.AddScaleBaseModifier(canvasNode, testParas, scaleProperty);
    symbolAnimation.ScaleAnimationBase(scaleProperty, testParas, groupAnimation);
    /**
     * @tc.steps: step2. start ScaleAnimationBase test
     */
    EXPECT_TRUE(groupAnimation.empty());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest ScaleAnimationBase002 end";
}

/**
 * @tc.name: AppearAnimation001
 * @tc.desc: Verify the basic ability, AppearAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, AppearAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {APPEAR_FIRST_PHASE_PARAS,
                                                                     APPEAR_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start AppearAnimation test
     */
    symbolAnimation.AppearAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation001 end";
}

/**
 * @tc.name: AppearAnimation002
 * @tc.desc: Verify the basic ability, AppearAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, AppearAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation002 start";
    /**
     * @tc.steps: step1. if node is nullptr
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {APPEAR_FIRST_PHASE_PARAS,
                                                                     APPEAR_SECOND_PHASE_PARAS};
    /**
     * @tc.steps: step2. start AppearAnimation test, no animation
     */
    symbolAnimation.AppearAnimation(nullptr, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation002 end";
}

/**
 * @tc.name: AppearAnimation003
 * @tc.desc: Verify the basic ability, AppearAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, AppearAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation003 start";
    /**
     * @tc.steps: step1. if parameters.size() < animationStageNum
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {APPEAR_FIRST_PHASE_PARAS};
    /**
     * @tc.steps: step2. start AppearAnimation test, no animation
     */
    symbolAnimation.AppearAnimation(canvasNode, oneGroupParas);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest AppearAnimation003 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation001
 * @tc.desc: Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    for (unsigned long i = 0; i < ALPHA_VALUES.size(); i++) {
        float alphaValue = ALPHA_VALUES[i]; // the value of the key frame needs
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
        alphaPropertyPhase = std::make_shared<RSAnimatableProperty<float>>(alphaValue);
        symbolAnimation.alphaPropertyStages_.push_back(alphaPropertyPhase);
    }
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(canvasNode, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(animation == nullptr);
    EXPECT_FALSE(animation->IsStarted());
    animation->Start(canvasNode);
    EXPECT_TRUE(animation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation001 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation002
 * @tc.desc: Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation002 start";
    /**
     * @tc.steps: step1. if no alpha data
     */
    auto symbolAnimation = RSSymbolAnimation();
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(canvasNode, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(animation == nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation002 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation003
 * @tc.desc: Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation003 start";
    /**
     * @tc.steps: step1. if alphaPropertyStages_.size() != TIME_PERCENTS.size()
     */
    auto symbolAnimation = RSSymbolAnimation();
    for (unsigned long i = 0; i < ALPHA_VALUES.size() - 1; i++) {
        float alphaValue = ALPHA_VALUES[i]; // the value of the key frame needs
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
        alphaPropertyPhase = std::make_shared<RSAnimatableProperty<float>>(alphaValue);
        symbolAnimation.alphaPropertyStages_.push_back(alphaPropertyPhase);
    }
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(canvasNode, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(symbolAnimation.alphaPropertyStages_.size() == TIME_PERCENTS.size());
    EXPECT_TRUE(animation == nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation003 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation004
 * @tc.desc:Verify the basic ability, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation004 start";
    /**
     * @tc.steps: step1. if node is nullptr
     */
    auto symbolAnimation = RSSymbolAnimation();
    for (unsigned long i = 0; i < ALPHA_VALUES.size(); i++) {
        float alphaValue = ALPHA_VALUES[i]; // the value of the key frame needs
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
        alphaPropertyPhase = std::make_shared<RSAnimatableProperty<float>>(alphaValue);
        symbolAnimation.alphaPropertyStages_.push_back(alphaPropertyPhase);
    }
    auto animation = symbolAnimation.KeyframeAlphaSymbolAnimation(nullptr, VARIABLECOLOR_FIRST_PHASE_PARAS,
        ANIMATION_DURATION, TIME_PERCENTS);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(animation == nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation004 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation005
 * @tc.desc: Get Paras for KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation005 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    uint32_t totalDuration = 0;
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.GetKeyframeAlphaAnimationParas(oneGroupParas, totalDuration, timePercents);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(getParasFlag == false);
    EXPECT_FALSE(totalDuration == 0);
    EXPECT_FALSE(timePercents.size() == 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation005 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation006
 * @tc.desc: Get Paras for KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation006 start";
    /**
     * @tc.steps: step1. if oneGroupParas is empty
     */
    auto symbolAnimation = RSSymbolAnimation();
    uint32_t totalDuration = 0;
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {};
    auto getParasFlag = symbolAnimation.GetKeyframeAlphaAnimationParas(oneGroupParas, totalDuration, timePercents);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(getParasFlag == false);
    EXPECT_TRUE(totalDuration == 0);
    EXPECT_TRUE(timePercents.size() == 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation006 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation007
 * @tc.desc: A Part of Get Paras, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation007 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.CalcTimePercents(timePercents, ANIMATION_DURATION, oneGroupParas);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_FALSE(getParasFlag == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation007 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation008
 * @tc.desc: A Part of Get Paras, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation008 start";
    /**
     * @tc.steps: step1. if totalDuration <= 0
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.CalcTimePercents(timePercents, 0, oneGroupParas);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(getParasFlag == false);
    EXPECT_TRUE(timePercents.size() == 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation008 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation009
 * @tc.desc: A Part of Get Paras, KeyframeAlphaSymbolAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation009 start";
    /**
     * @tc.steps: step1. if interval < 0
     */
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<float> timePercents;
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_FIRST_PHASE_PARAS};
    auto getParasFlag = symbolAnimation.CalcTimePercents(timePercents, ANIMATION_DURATION, oneGroupParas);
    /**
     * @tc.steps: step2. start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(getParasFlag == false);
    EXPECT_TRUE(timePercents.size() == 2);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation009 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation0010
 * @tc.desc: SetKeyframeAlphaAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation0010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0010 start";
    /**
     * @tc.steps: step1.1 init data if effectStrategy is variablecolor
     */
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>(VARIABLE_COLOR_CONFIG);
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto flag1 = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, oneGroupParas, symbolAnimationConfig);
    /**
     * @tc.steps: step1.2 if node is nullptr
     */
    auto flag2 = symbolAnimation.SetKeyframeAlphaAnimation(nullptr, oneGroupParas, symbolAnimationConfig);
    /**
     * @tc.steps: step1.3 if symbolAnimationConfig is nullptr
     */
    auto flag3 = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, oneGroupParas, nullptr);
    /**
     * @tc.steps: step1.4 if parameters is empty
     */
    std::vector<Drawing::DrawingPiecewiseParameter> emptyParameters = {};
    auto flag4 = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, emptyParameters, symbolAnimationConfig);
    /**
     * @tc.steps: step2 start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(flag1 == true);
    EXPECT_TRUE(flag2 == false);
    EXPECT_TRUE(flag3 == false);
    EXPECT_TRUE(flag4 == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0010 end";
}

/**
 * @tc.name: KeyframeAlphaSymbolAnimation0011
 * @tc.desc: SetKeyframeAlphaAnimation of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, KeyframeAlphaSymbolAnimation0011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0011 start";
    /**
     * @tc.steps: step1 init data if effectStrategy is pulse
     */
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>(PULSE_CONFIG);
    auto symbolAnimation = RSSymbolAnimation();
    std::vector<Drawing::DrawingPiecewiseParameter> oneGroupParas = {VARIABLECOLOR_FIRST_PHASE_PARAS,
                                                                     VARIABLECOLOR_SECOND_PHASE_PARAS};
    auto flag = symbolAnimation.SetKeyframeAlphaAnimation(canvasNode, oneGroupParas, symbolAnimationConfig);

    /**
     * @tc.steps: step2 start KeyframeAlphaSymbolAnimation test
     */
    EXPECT_TRUE(flag == true);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest KeyframeAlphaSymbolAnimation0011 end";
}

/**
 * @tc.name: SetSymbolGeometryTest001
 * @tc.desc: set geometry of a node, SetSymbolGeometry of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolGeometryTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest001 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto flag = symbolAnimation.SetSymbolGeometry(canvasNode, ANIMATION_START_BOUNDS);
    /**
     * @tc.steps: step2. start SetSymbolGeometry test
     */
    EXPECT_FALSE(flag == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest001 end";
}

/**
 * @tc.name: SetSymbolGeometryTest002
 * @tc.desc: set geometry of a node, SetSymbolGeometry of RSSymbolAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolGeometryTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest002 start";
    /**
     * @tc.steps: step1. init data
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNode(rootNode);
    auto flag = symbolAnimation.SetSymbolGeometry(nullptr, ANIMATION_START_BOUNDS);
    /**
     * @tc.steps: step2. start SetSymbolGeometry test
     */
    EXPECT_TRUE(flag == false);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometryTest002 end";
}

/**
 * @tc.name: SetNodePivotTest001
 * @tc.desc: move the scale center to the center of a node
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetNodePivotTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest001 start";
    /**
     * @tc.steps: step1. init data if scale center equals the center of a node
     */
    auto symbolAnimation = RSSymbolAnimation();
    symbolAnimation.SetNodePivot(canvasNode);
    /**
     * @tc.steps: step2. start SetNodePivot test
     */
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest001 end";
}

/**
 * @tc.name: SetNodePivotTest002
 * @tc.desc: move the scale center to the center of a node
 * @tc.type: FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetNodePivotTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest002 start";
    /**
     * @tc.steps: step1. init data if scale center not equals the center of a node
     */
    auto symbolAnimation = RSSymbolAnimation();
    auto newCanvasNode = RSCanvasNode::Create();
    Vector2f pivotOffset = Vector2f(0.1f, 0.1f); // 0.1f is offset on x-axis and y-axis
    auto pivotProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(pivotOffset);
    auto pivotModifier = std::make_shared<RSPivotModifier>(pivotProperty);
    newCanvasNode->AddModifier(pivotModifier);
    symbolAnimation.SetNodePivot(newCanvasNode);
    /**
     * @tc.steps: step2. start SetNodePivot test
     */
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetNodePivotTest002 end";
}
} // namespace Rosen
} // namespace OHOS