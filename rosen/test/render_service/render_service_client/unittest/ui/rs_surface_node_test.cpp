/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "animation/rs_animation.h"
#include "core/transaction/rs_interfaces.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceNodeTest::SetUpTestCase() {}
void RSSurfaceNodeTest::TearDownTestCase() {}
void RSSurfaceNodeTest::SetUp() {}
void RSSurfaceNodeTest::TearDown() {}

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "animation/rs_animation.h"
#include "core/transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceNodeTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceNodeTest::SetUpTestCase() {}
void RSSurfaceNodeTest::TearDownTestCase() {}
void RSSurfaceNodeTest::SetUp() {}
void RSSurfaceNodeTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, Create001, TestSize.Level1)
{
	// return shared_ptr
    RSSurfaceNode::Create();
}

/**
 * @tc.name: Create002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, Create002, TestSize.Level1)
{
	// return shared_ptr
    RSSurfaceNode::Create(true);
}

/**
 * @tc.name: Create003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, Create003, TestSize.Level1)
{
	// return shared_ptr
    RSSurfaceNode::Create(false);
}


/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSSurfaceNode and add child
     */
    RSSurfaceNode::SharedPtr rootNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(rootNode != nullptr);

    RSSurfaceNode::SharedPtr child1 = RSSurfaceNode::Create(c);
    RSSurfaceNode::SharedPtr child2 = RSSurfaceNode::Create(c);
    RSSurfaceNode::SharedPtr child3 = RSSurfaceNode::Create(c);
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);

    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(child2);
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, LifeCycle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSSurfaceNode and add child
     */
    RSSurfaceNode::SharedPtr rootNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(rootNode != nullptr);

    RSSurfaceNode::SharedPtr child1 = RSSurfaceNode::Create(c);
    RSSurfaceNode::SharedPtr child2 = RSSurfaceNode::Create(c);
    RSSurfaceNode::SharedPtr child3 = RSSurfaceNode::Create(c);
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveFromTree();
}

/**
 * @tc.name: LifeCycle003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, LifeCycle003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSSurfaceNode and add child
     */
    RSSurfaceNode::SharedPtr rootNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(rootNode != nullptr);

    RSSurfaceNode::SharedPtr child1 = RSSurfaceNode::Create(c);
    RSSurfaceNode::SharedPtr child2 = RSSurfaceNode::Create(c);
    RSSurfaceNode::SharedPtr child3 = RSSurfaceNode::Create(c);
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->ClearChildren();
}

/**
 * @tc.name: LifeCycle004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, LifeCycle004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSSurfaceNode and add child
     */
    RSSurfaceNode::SharedPtr rootNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(rootNode != nullptr);
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr child1 = RSSurfaceNode::Create(c);
    RSSurfaceNode::SharedPtr child2 = RSSurfaceNode::Create(c);
    RSSurfaceNode::SharedPtr child3 = RSSurfaceNode::Create(c);
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(child3);
}

/**
 * @tc.name: Recording001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, Recording001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSSurfaceNode and RSUIDirector
     */
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);

    /**
     * @tc.steps: step2. begin recording
     */
    EXPECT_FALSE(surfaceNode->IsRecording());
    surfaceNode->BeginRecording(500, 400);
    EXPECT_TRUE(surfaceNode->IsRecording());
}

/**
 * @tc.name: SetPaintOrder001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetPaintOrder001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSSurfaceNode and RSUIDirector
     */
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPaintOrder(true);
}

/**
 * @tc.name: SetandGetBounds001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBounds001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBounds(floatData[0], floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetBounds002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBounds002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBounds(floatData[3], floatData[1], floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetBounds003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBounds003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBounds(floatData[3], floatData[2], floatData[1], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsSize001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsSize001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsSize(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsSize002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsSize002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsSize(floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsSize003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsSize003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsSize(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsPosition001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPosition001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector2f quaternion(floatData[2], floatData[3]);
    surfaceNode->SetBoundsPosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsPosition002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPosition002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector2f quaternion(floatData[2], floatData[0]);
    surfaceNode->SetBoundsPosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsPosition003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPosition003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector2f quaternion(floatData[1], floatData[3]);
    surfaceNode->SetBoundsPosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsWidth001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsWidth001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsWidth002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsWidth003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsWidth003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsWidth004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsWidth004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsWidth005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsWidth005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsHeight001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsHeight001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsHeight002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsHeight002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsHeight003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsHeight003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsHeight004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsHeight004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsHeight005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsHeight005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsPositionX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPositionX001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsPositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsPositionX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPositionX002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsPositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsPositionX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPositionX003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsPositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsPositionX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPositionX004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsPositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsPositionX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPositionX005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsPositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionX(), floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsPositionY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPositionY001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsPositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsPositionY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPositionY002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsPositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsPositionY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPositionY003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsPositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsPositionY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPositionY004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsPositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsPositionY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBoundsPositionY005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsPositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBoundsPositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetFrame001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrame001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrame(floatData[0], floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetFrame002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrame002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrame(floatData[3], floatData[1], floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetFrame003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrame003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrame(floatData[3], floatData[2], floatData[1], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetFrameSize001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameSize001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameSize(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetFrameSize002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameSize002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameSize(floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetFrameSize003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameSize003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameSize(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[2]));
}

/**
 * @tc.name: GetFramePosition001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, GetFramePosition001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector2f quaternion(floatData[2], floatData[3]);
    surfaceNode->SetFramePosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[3]));
}

/**
 * @tc.name: GetFramePosition002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, GetFramePosition002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector2f quaternion(floatData[2], floatData[0]);
    surfaceNode->SetFramePosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[0]));
}

/**
 * @tc.name: GetFramePosition003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, GetFramePosition003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector2f quaternion(floatData[1], floatData[3]);
    surfaceNode->SetFramePosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetFrameWidth001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameWidth001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetFrameWidth002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameWidth002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetFrameWidth003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameWidth003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetFrameWidth004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameWidth004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetFrameWidth005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameWidth005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetFrameHeight001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameHeight001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetFrameHeight002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameHeight002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetFrameHeight003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameHeight003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetFrameHeight004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameHeight004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[4]));
}

/**
 * @tc.name: SetandGetFrameHeight005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameHeight005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFrameHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFrameHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetFramePositionX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFramePositionX001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFramePositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[1]));
}

/**
 * @tc.name: SetandGetFramePositionX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFramePositionX002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFramePositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[2]));
}

/**
 * @tc.name: SetandGetFramePositionX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFramePositionX003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFramePositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[3]));
}

/**
 * @tc.name: SetandGetFramePositionX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFramePositionX004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFramePositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[4]));
}

/**
 * @tc.name: SetandGetFramePositionX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFramePositionX005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFramePositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionX(), floatData[0]));
}

/**
 * @tc.name: SetandGetFramePositionY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFramePositionY001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFramePositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[1]));
}

/**
 * @tc.name: SetandGetFramePositionY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFramePositionY002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFramePositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[2]));
}

/**
 * @tc.name: SetandGetFramePositionY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFramePositionY003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFramePositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetFramePositionY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFramePositionY004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFramePositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[4]));
}

/**
 * @tc.name: SetandGetFramePositionY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFramePositionY005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFramePositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetFramePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetPositionZ001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPositionZ001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPositionZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPositionZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetPositionZ002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPositionZ002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPositionZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPositionZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetPositionZ003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPositionZ003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPositionZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPositionZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetPositionZ004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPositionZ004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPositionZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPositionZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetPositionZ005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPositionZ005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPositionZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPositionZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetCornerRadius001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetCornerRadius001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetCornerRadius(), floatData[1]));
}

/**
 * @tc.name: SetandGetCornerRadius002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetCornerRadius002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetCornerRadius(), floatData[2]));
}

/**
 * @tc.name: SetandGetCornerRadius003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetCornerRadius003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetCornerRadius(), floatData[3]));
}

/**
 * @tc.name: SetandGetCornerRadius004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetCornerRadius004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetCornerRadius(), floatData[4]));
}

/**
 * @tc.name: SetandGetCornerRadius005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetCornerRadius005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetCornerRadius(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationThree001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationThree001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotation(floatData[1], floatData[2], floatData[3]);
}

/**
 * @tc.name: SetandGetRotation001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotation001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotation(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotation002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotation002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotation(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotation003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotation003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotation(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotation004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotation004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotation(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotation(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotation005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotation005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotation(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotation(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationX001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotationX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotationX(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotationX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationX002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotationX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotationX(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotationX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationX003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotationX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotationX(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotationX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationX004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotationX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotationX(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotationX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationX005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotationX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotationX(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationY001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotationY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotationY(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotationY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationY002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotationY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotationY(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotationY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationY003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotationY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotationY(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotationY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationY004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotationY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotationY(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotationY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationY005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetRotationY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetRotationY(), floatData[0]));
}


/**
 * @tc.name: SetandGetScaleX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScaleX001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScaleX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleX(), floatData[1]));
}

/**
 * @tc.name: SetandGetScaleX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScaleX002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScaleX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleX(), floatData[2]));
}

/**
 * @tc.name: SetandGetScaleX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScaleX003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScaleX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleX(), floatData[3]));
}

/**
 * @tc.name: SetandGetScaleX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScaleX004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScaleX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleX(), floatData[4]));
}

/**
 * @tc.name: SetandGetScaleX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScaleX005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScaleX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleX(), floatData[0]));
}

/**
 * @tc.name: SetandGetScale001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScale001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScale(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleY(), floatData[1]));
}

/**
 * @tc.name: SetandGetScale002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScale002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScale(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleY(), floatData[2]));
}

/**
 * @tc.name: SetandGetScale003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScale003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScale(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleY(), floatData[3]));
}

/**
 * @tc.name: SetandGetScale004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScale004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScale(floatData[3], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleY(), floatData[0]));
}

/**
 * @tc.name: SetandGetScale0005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScale0005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScale(floatData[2], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleY(), floatData[1]));
}

/**
 * @tc.name: SetandGetScaleY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScaleY001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScaleY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleY(), floatData[1]));
}

/**
 * @tc.name: SetandGetScaleY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScaleY002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScaleY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleY(), floatData[2]));
}

/**
 * @tc.name: SetandGetScaleY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScaleY003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScaleY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleY(), floatData[3]));
}

/**
 * @tc.name: SetandGetScaleY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScaleY004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScaleY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleY(), floatData[4]));
}

/**
 * @tc.name: SetandGetScaleY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetScaleY005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetScaleY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetScaleY(), floatData[0]));
}

/**
 * @tc.name: SetandGetAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetAlpha001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetAlpha(), floatData[1]));
}

/**
 * @tc.name: SetandGetAlpha002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetAlpha002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetAlpha(), floatData[2]));
}

/**
 * @tc.name: SetandGetAlpha003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetAlpha003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetAlpha(), floatData[3]));
}

/**
 * @tc.name: SetandGetAlpha004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetAlpha004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetAlpha(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetAlpha(), floatData[4]));
}

/**
 * @tc.name: SetandGetAlpha005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetAlpha005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetAlpha(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetAlpha(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImageSize001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageSize001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageSize(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageWidth(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageSize002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageSize002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageSize(floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageSize003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageSize003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageSize(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageWidth001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageWidth002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageWidth002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageWidth003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImageWidth004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageWidth004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImageWidth005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageWidth005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImageHeight001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageHeight001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageHeight002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageHeight002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageHeight003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageHeight003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImageHeight004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageHeight004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageHeight(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImageHeight005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImageHeight005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImageHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImageHeight(), floatData[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandSetBgImagePosition001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePosition(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandSetBgImagePosition002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandSetBgImagePosition002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePosition(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandSetBgImagePosition003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePosition(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImagePositionX001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionX(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImagePositionX002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImagePositionX003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionX(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImagePositionX004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionX(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImagePositionX005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionX(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImagePositionY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImagePositionY001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionY(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImagePositionY002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionY(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImagePositionY003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImagePositionY004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionY(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBgImagePositionY005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBgImagePositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBgImagePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetBorderWidth001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderWidth001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBorderWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBorderWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetBorderWidth002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderWidth002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBorderWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBorderWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetBorderWidth003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderWidth003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBorderWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBorderWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetBorderWidth004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderWidth004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBorderWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBorderWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetBorderWidth005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderWidth005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBorderWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetBorderWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetPivot001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivot001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivot(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotX(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotY(), floatData[1]));
}

/**
 * @tc.name: SetandGetPivot002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivot002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivot(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotY(), floatData[2]));
}

/**
 * @tc.name: SetandGetPivot003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivot003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivot(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotY(), floatData[3]));
}

/**
 * @tc.name: SetandGetPivotX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivotX001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivotX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotX(), floatData[1]));
}

/**
 * @tc.name: SetandGetPivotX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivotX002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivotX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotX(), floatData[2]));
}

/**
 * @tc.name: SetandGetPivotX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivotX003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivotX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotX(), floatData[3]));
}

/**
 * @tc.name: SetandGetPivotX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivotX004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivotX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotX(), floatData[4]));
}

/**
 * @tc.name: SetandGetPivotX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivotX005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivotX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotX(), floatData[0]));
}

/**
 * @tc.name: SetandGetPivotY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivotY001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivotY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotY(), floatData[1]));
}

/**
 * @tc.name: SetandGetPivotY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivotY002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivotY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotY(), floatData[2]));
}

/**
 * @tc.name: SetandGetPivotY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivotY003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivotY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotY(), floatData[3]));
}

/**
 * @tc.name: SetandGetPivotY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivotY004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivotY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotY(), floatData[4]));
}

/**
 * @tc.name: SetandGetPivotY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetPivotY005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetPivotY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetPivotY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffset001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffset001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffset(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffset002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffset002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffset(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffset003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffset003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffset(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}
/**
 * @tc.name: SetandGetShadowOffsetX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffsetX001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffsetX002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffsetX003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffsetX004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffsetX005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffsetY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffsetY001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffsetY002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffsetY003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffsetY004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowOffsetY005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowAlpha001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[1], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowAlpha002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[2], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowAlpha003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[3], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowAlpha004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[4], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowAlpha005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[0], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowElevation001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowElevation001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowElevation002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowElevation002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowElevation003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowElevation003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowElevation004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowElevation004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowElevation005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowElevation005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowRadius001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowRadius001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowRadius002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowRadius002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowRadius003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowRadius003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowRadius004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowRadius004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowRadius005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowRadius005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowColor001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowColor001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = 0x034123;
    surfaceNode->SetShadowColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowColor002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = 0x45ba87;
    surfaceNode->SetShadowColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowColor003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = 0x32aadd;
    surfaceNode->SetShadowColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowColor004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    surfaceNode->SetShadowColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetShadowColor005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    surfaceNode->SetShadowColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetTranslateThree001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateThree001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslate(floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateY(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateThree002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateThree002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslate(floatData[0], floatData[0], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateX(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateY(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateThree003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateThree003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslate(floatData[1], floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateY(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateX001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateX(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateX002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateX(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateX003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateX(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateX004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateX(), floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateX005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateX(), floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateY001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateY(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateY002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateY(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateY003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateY(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateY004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateY(), floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateY005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateY(), floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateZ001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateZ001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateZ002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateZ002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateZ003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateZ003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateZ004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateZ004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateZ005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateZ005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTranslateZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetTranslateZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetClipToBounds001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetClipToBounds001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetClipToBounds(true);
    EXPECT_EQ(surfaceNode->GetStagingProperties().GetClipToBounds(), true);
}

/**
 * @tc.name: SetandGetClipToBounds002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetClipToBounds002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetClipToBounds(false);
    EXPECT_EQ(surfaceNode->GetStagingProperties().GetClipToBounds(), false);
}

/**
 * @tc.name: SetandGetClipToFrame001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetClipToFrame001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetClipToFrame(true);
    EXPECT_EQ(surfaceNode->GetStagingProperties().GetClipToFrame(), true);
}

/**
 * @tc.name: SetandGetClipToFrame002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetClipToFrame002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetClipToFrame(false);
    EXPECT_EQ(surfaceNode->GetStagingProperties().GetClipToFrame(), false);
}

/**
 * @tc.name: SetandGetVisible001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetVisible001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetVisible(true);
    EXPECT_EQ(surfaceNode->GetStagingProperties().GetVisible(), true);
}

/**
 * @tc.name: SetandGetVisible002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetVisible002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetVisible(false);
    EXPECT_EQ(surfaceNode->GetStagingProperties().GetVisible(), false);
}

/**
 * @tc.name: SetandGetBorderStyle001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderStyle001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    BorderStyle borderStyle = BorderStyle::SOLID;
    surfaceNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderStyle002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    BorderStyle borderStyle = BorderStyle::DASHED;
    surfaceNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderStyle003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    BorderStyle borderStyle = BorderStyle::DOTTED;
    surfaceNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderStyle004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    BorderStyle borderStyle = BorderStyle::NONE;
    surfaceNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetFrameGravity001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::CENTER;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::TOP;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::BOTTOM;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::LEFT;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::RIGHT;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity006
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity006, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::TOP_LEFT;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity007
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity007, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::TOP_RIGHT;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity008
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity008, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::BOTTOM_LEFT;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity009
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity009, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::BOTTOM_RIGHT;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity010
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity010, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::RESIZE;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity011
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity011, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::RESIZE_ASPECT;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity012
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetFrameGravity012, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Gravity gravity = Gravity::RESIZE_ASPECT_FILL;
    surfaceNode->SetFrameGravity(gravity);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: NotifyTransition001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, NotifyTransition001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->NotifyTransition({RSTransitionEffect(RSTransitionEffectType::FADE_IN)}, surfaceNode->GetId());
}

/**
 * @tc.name: NotifyTransition002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, NotifyTransition002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->NotifyTransition({RSTransitionEffect(RSTransitionEffectType::FADE_IN)}, surfaceNode->GetId());
}

/**
 * @tc.name: SetandGetForegroundColor001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetForegroundColor001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = 0x034123;
    surfaceNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetForegroundColor002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    surfaceNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetForegroundColor003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    surfaceNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBackgroundColor001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = 0x034123;
    surfaceNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBackgroundColor002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    surfaceNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBackgroundColor003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    surfaceNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderColor001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = 0x034123;
    surfaceNode->SetBorderColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderColor002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    surfaceNode->SetBorderColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetBorderColor003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    surfaceNode->SetBorderColor(colorValue);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetRotationVector001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetRotationVector001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector4f quaternion(std::numeric_limits<int>::min(), 2.f, 3.f, 4.f);
    surfaceNode->SetRotation(quaternion);
}

/**
 * @tc.name: SetandGetTranslateVector001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetTranslateVector001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector2f quaternion(std::numeric_limits<int>::max(), 2.f);
    surfaceNode->SetTranslate(quaternion);
}

/**
 * @tc.name: SetandGetSublayerTransform001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetSublayerTransform001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Matrix3f sublayerTransform = Matrix3f::IDENTITY;
    surfaceNode->SetSublayerTransform(sublayerTransform);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetSublayerTransform() == sublayerTransform);
}

/**
 * @tc.name: SetandGetSublayerTransform002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetSublayerTransform002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Matrix3f sublayerTransform = Matrix3f::ZERO;
    surfaceNode->SetSublayerTransform(sublayerTransform);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetSublayerTransform() == sublayerTransform);
}

/**
 * @tc.name: CreateBlurFilter001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, CreateBlurFilter001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, CreateBlurFilter002, TestSize.Level2)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, CreateBlurFilter003, TestSize.Level3)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, CreateBlurFilter004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, CreateBlurFilter005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateNormalFilter001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, CreateNormalFilter001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, CreateNormalFilter002, TestSize.Level2)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, CreateNormalFilter003, TestSize.Level3)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, CreateNormalFilter004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, CreateNormalFilter005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: SetandGetClipBounds001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetandGetClipBounds001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSPath> clipPath = RSPath::CreateRSPath();
    surfaceNode->SetClipBounds(clipPath);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetClipBounds() == clipPath);
}

/**
 * @tc.name: GetId001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, GetId001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->GetId();
}

/**
 * @tc.name: GetStagingProperties001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, GetStagingProperties001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->GetStagingProperties();
}

/**
 * @tc.name: GetMotionPathOption001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, GetMotionPathOption002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->GetMotionPathOption();
}

/**
 * @tc.name: SetBgImage001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetBgImage001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    auto image = std::make_shared<RSImage>();
    surfaceNode->SetBgImage(image);
}

/**
 * @tc.name: SetBackgroundShader001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetBackgroundShader001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    auto shader = RSShader::CreateRSShader();
    surfaceNode->SetBackgroundShader(shader);
}

/**
 * @tc.name: SetCompositingFilter001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetCompositingFilter001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    auto compositingFilter = RSFilter::CreateBlurFilter(0.0f, 0.0f);
    surfaceNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetShadowPath001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, SetShadowPath001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    auto shadowpath = RSPath::CreateRSPath();
    surfaceNode->SetShadowPath(shadowpath);
}

/**
 * @tc.name: Create001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, Create001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    auto surface = surfaceNode->GetSurface();
    ASSERT_TRUE(surface != nullptr);
}

/**
 * @tc.name: Marshalling001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, Marshalling001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    Parcel parcel;
    surfaceNode->Marshalling(parcel);
}

/**
 * @tc.name: Marshalling002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, Marshalling002, TestSize.Level1)
{
    Parcel parcel;
    auto surfaceNode = RSSurfaceNode::Unmarshalling(parcel);
    EXPECT_TRUE(surfaceNode == nullptr);
}

/**
 * @tc.name: Marshalling003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, Marshalling003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    Parcel parcel;
    surfaceNode->Marshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
}

/**
 * @tc.name: Marshalling004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, Marshalling004, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    Parcel parcel;
    surfaceNode->Marshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
}

/**
 * @tc.name: Marshalling005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, Marshalling005, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode1 = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode1 != nullptr);
    RSSurfaceNode::SharedPtr surfaceNode2 = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode2 != nullptr);

    Parcel parcel;
    surfaceNode1->Marshalling(parcel);
    surfaceNode2->Marshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
}

/**
 * @tc.name: GetSurface001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, GetSurface001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    surfaceNode->GetSurface();
}

/**
 * @tc.name: GetType001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, GetType001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);
    ASSERT_TRUE(surfaceNode->GetType() == RSUINodeType::SURFACE_NODE);
}

/**
 * @tc.name: GetType002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, GetType002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c, true);
    ASSERT_TRUE(surfaceNode != nullptr);
    ASSERT_TRUE(surfaceNode->GetType() == RSUINodeType::SURFACE_NODE);
}

/**
 * @tc.name: GetType003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, GetType003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c, false);
    ASSERT_TRUE(surfaceNode != nullptr);
    ASSERT_TRUE(surfaceNode->GetType() == RSUINodeType::SURFACE_NODE);
}

/**
 * @tc.name: TakeSurfaceCapture001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSSurfaceNodeTest, TakeSurfaceCapture001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create RSDisplayNode
    */
    RSSurfaceNodeTest c;
    RSSurfaceNodeTest::SharedPtr surfaceNode = RSDisplayNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto surfaceCaptureMock = std::make_shared<TestSurfaceCapture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(surfaceNode, surfaceCaptureMock);
}
} // namespace OHOS::Rosen