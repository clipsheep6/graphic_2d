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
#include "symbol_animation_config.h"

#include "animation/rs_symbol_animation.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSSymbolAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSymbolAnimationTest::SetUpTestCase() {}
void RSSymbolAnimationTest::TearDownTestCase() {}
void RSSymbolAnimationTest::SetUp() {}
void RSSymbolAnimationTest::TearDown() {}

/**
 * @tc.name: SetSymbolGeometry001
 * @tc.desc: Verify the SetSymbolGeometry
 * @tc.type:FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolGeometry001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometry001 start";
    std::shared_ptr<RSCanvasNode> node = nullptr;
    auto bounds = Vector4f(100.f, 100.f, 200.f, 300.f);
    auto animation = std::make_shared<RSSymbolAnimation>();
    animation->SetSymbolGeometry(node, bounds);

    auto node1 = RSCanvasNode::Create();
    animation->SetSymbolGeometry(node1, bounds);
    EXPECT_TRUE(node1 != nullptr);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolGeometry001 end";
}

/**
 * @tc.name: SetScaleUnitAnimation001
 * @tc.desc: Verify the SetScaleUnitAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetScaleUnitAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetScaleUnitAnimation001 start";
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    auto animation = std::make_shared<RSSymbolAnimation>();
    bool result = animation->SetScaleUnitAnimation(symbolAnimationConfig);
    EXPECT_TRUE(!result);

    TextEngine::SymbolNode symbolNode;
    symbolAnimationConfig->SymbolNodes.emplace_back(symbolNode);
    symbolAnimationConfig->numNodes += 1;
    auto canvasNode = RSCanvasNode::Create();
    auto node = std::static_pointer_cast<RSNode>(canvasNode);
    auto animation1 = std::make_shared<RSSymbolAnimation>();
    animation1->SetNode(node);
    bool result1 = animation1->SetScaleUnitAnimation(symbolAnimationConfig);
    EXPECT_TRUE(result1);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetScaleUnitAnimation001 end";
}

/**
 * @tc.name: SetVariableColorAnimation001
 * @tc.desc: Verify the SetVariableColorAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetVariableColorAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetVariableColorAnimation001 start";
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    auto animation = std::make_shared<RSSymbolAnimation>();
    bool result = animation->SetVariableColorAnimation(symbolAnimationConfig);
    EXPECT_TRUE(!result);

    TextEngine::SymbolNode symbolNode;
    symbolAnimationConfig->SymbolNodes.emplace_back(symbolNode);
    symbolAnimationConfig->numNodes += 1;
    auto canvasNode = RSCanvasNode::Create();
    auto node = std::static_pointer_cast<RSNode>(canvasNode);
    auto animation1 = std::make_shared<RSSymbolAnimation>();
    animation1->SetNode(node);
    bool result1 = animation1->SetVariableColorAnimation(symbolAnimationConfig);
    EXPECT_TRUE(result1);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetVariableColorAnimation001 end";
}

/**
 * @tc.name: SetSymbolAnimation001
 * @tc.desc: Verify the SetSymbolAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSSymbolAnimationTest, SetSymbolAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolAnimation001 start";
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    auto animation = std::make_shared<RSSymbolAnimation>();
    bool result = animation->SetSymbolAnimation(symbolAnimationConfig);
    EXPECT_TRUE(!result);

    std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfigTmp = nullptr;
    auto canvasNode = RSCanvasNode::Create();
    auto node = std::static_pointer_cast<RSNode>(canvasNode);
    animation->SetNode(node);
    result = animation->SetSymbolAnimation(symbolAnimationConfigTmp);
    EXPECT_TRUE(!result);

    auto symbolAnimationConfig1 = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig1->effectStrategy = TextEngine::SymbolAnimationEffectStrategy::SYMBOL_SCALE;
    auto canvasNode1 = RSCanvasNode::Create();
    auto node1 = std::static_pointer_cast<RSNode>(canvasNode1);
    animation->SetNode(node1);
    result = animation->SetSymbolAnimation(symbolAnimationConfig1);
    EXPECT_TRUE(!result);

    auto symbolAnimationConfig2 = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig2->effectStrategy = TextEngine::SymbolAnimationEffectStrategy::SYMBOL_HIERARCHICAL;
    auto canvasNode2 = RSCanvasNode::Create();
    auto node2 = std::static_pointer_cast<RSNode>(canvasNode2);
    animation->SetNode(node2);
    result = animation->SetSymbolAnimation(symbolAnimationConfig2);
    EXPECT_TRUE(!result);

    auto symbolAnimationConfig3 = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig3->effectStrategy = TextEngine::SymbolAnimationEffectStrategy::INVALID_EFFECT_STRATEGY;
    auto canvasNode3 = RSCanvasNode::Create();
    auto node3 = std::static_pointer_cast<RSNode>(canvasNode3);
    animation->SetNode(node3);
    result = animation->SetSymbolAnimation(symbolAnimationConfig2);
    EXPECT_TRUE(!result);
    GTEST_LOG_(INFO) << "RSSymbolAnimationTest SetSymbolAnimation001 end";
}

} // namespace Rosen
} // namespace OHOS
