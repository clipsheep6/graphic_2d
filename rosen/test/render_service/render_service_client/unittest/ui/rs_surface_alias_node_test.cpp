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
#include "limit_number.h"
#include "ui/rs_surface_alias_node.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceAliasNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceAliasNodeTest::SetUpTestCase() {}
void RSSurfaceAliasNodeTest::TearDownTestCase() {}
void RSSurfaceAliasNodeTest::SetUp() {}
void RSSurfaceAliasNodeTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, Create001, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    ASSERT_TRUE(surfaceAliasNode != nullptr);

    auto surface = surfaceAliasNode->GetSurface();
    ASSERT_TRUE(surface == nullptr);
}

/**
 * @tc.name: SetandGetBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBounds001, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBounds(TestSrc::limitNumber::floatLimit[0], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[3]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionX(), TestSrc::limitNumber::floatLimit[0]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionY(), TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBounds002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBounds002, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBounds(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[0]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionX(), TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionY(), TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBounds003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBounds003, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBounds(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[2],
        TestSrc::limitNumber::floatLimit[1], TestSrc::limitNumber::floatLimit[0]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionX(), TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionY(), TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBounds004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBounds004, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    Vector4f quaternion(TestSrc::limitNumber::floatLimit[0], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[3]);
    surfaceAliasNode->SetBounds(quaternion);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionX(), TestSrc::limitNumber::floatLimit[0]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionY(), TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBounds005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBounds005, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    Vector4f quaternion(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[0]);
    surfaceAliasNode->SetBounds(quaternion);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionX(), TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionY(), TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBounds006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBounds006, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    Vector4f quaternion(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[2],
        TestSrc::limitNumber::floatLimit[1], TestSrc::limitNumber::floatLimit[0]);
    surfaceAliasNode->SetBounds(quaternion);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionX(), TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionY(), TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBoundsSize001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsSize001, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsSize(TestSrc::limitNumber::floatLimit[0], TestSrc::limitNumber::floatLimit[1]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[0]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[1]));
}

/**
 * @tc.name: SetandGetBoundsSize002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsSize002, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsSize(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[1]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[1]));
}

/**
 * @tc.name: SetandGetBoundsSize003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsSize003, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsSize(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[2]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[2]));
}

/**
 * @tc.name: SetandGetBoundsSize004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsSize004, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    Vector2f quaternion(TestSrc::limitNumber::floatLimit[0], TestSrc::limitNumber::floatLimit[1]);
    surfaceAliasNode->SetBoundsSize(quaternion);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[0]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[1]));
}

/**
 * @tc.name: SetandGetBoundsSize005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsSize005, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    Vector2f quaternion(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[1]);
    surfaceAliasNode->SetBoundsSize(quaternion);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[1]));
}

/**
 * @tc.name: SetandGetBoundsSize006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsSize006, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    Vector2f quaternion(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[2]);
    surfaceAliasNode->SetBoundsSize(quaternion);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[2]));
}

/**
 * @tc.name: SetandGetBoundsPosition001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsPosition001, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    Vector2f quaternion(TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[3]);
    surfaceAliasNode->SetBoundsPosition(quaternion);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionX(), TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionY(), TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBoundsPosition002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsPosition002, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    Vector2f quaternion(TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[0]);
    surfaceAliasNode->SetBoundsPosition(quaternion);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionX(), TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionY(), TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBoundsPosition003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsPosition003, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    Vector2f quaternion(TestSrc::limitNumber::floatLimit[1], TestSrc::limitNumber::floatLimit[3]);
    surfaceAliasNode->SetBoundsPosition(quaternion);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionX(), TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsPositionY(), TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBoundsWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsWidth001, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[1]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsWidth002, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[2]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[2]));
}

/**
 * @tc.name: SetandGetBoundsWidth003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsWidth003, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[3]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBoundsWidth004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsWidth004, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[4]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[4]));
}

/**
 * @tc.name: SetandGetBoundsWidth005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsWidth005, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[0]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsWidth(), TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBoundsHeight001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsHeight001, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[1]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[1]));
}

/**
 * @tc.name: SetandGetBoundsHeight002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsHeight002, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[2]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[2]));
}

/**
 * @tc.name: SetandGetBoundsHeight003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsHeight003, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[3]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBoundsHeight004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsHeight004, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[4]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[4]));
}

/**
 * @tc.name: SetandGetBoundsHeight005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, SetandGetBoundsHeight005, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    surfaceAliasNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[0]);
    EXPECT_TRUE(
        ROSEN_EQ(surfaceAliasNode->GetStagingProperties().GetBoundsHeight(), TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: GetSurface001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, GetSurface001, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    ASSERT_TRUE(surfaceAliasNode != nullptr);

    surfaceAliasNode->GetSurface();
}

/**
 * @tc.name: GetType001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceAliasNodeTest, GetType001, TestSize.Level1)
{
    RSSurfaceAliasNode::SharedPtr surfaceAliasNode = RSSurfaceAliasNode::Create();
    ASSERT_TRUE(surfaceAliasNode != nullptr);
    ASSERT_TRUE(surfaceAliasNode->GetType() == RSUINodeType::SURFACE_ALIAS_NODE);
}
} // namespace OHOS::Rosen