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
#include "pipeline/rs_node_map.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSNodeMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeMapTest::SetUpTestCase() {}
void RSNodeMapTest::TearDownTestCase() {}
void RSNodeMapTest::SetUp() {}
void RSNodeMapTest::TearDown() {}

/*
 * @tc.name: RegisterNode
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSNodeMapTest, RegisterNode, TestSize.Level1)
{
    RSCanvasNode::SharedPtr node = RSCanvasNode::Create();
    RSNodeMap& map = RSNodeMap::MutableInstance();
    ASSERT_EQ(map.RegisterNode(node), false);
}

/*
 * @tc.name: UnregisterNode
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSNodeMapTest, UnregisterNode, TestSize.Level1)
{
    RSCanvasNode::SharedPtr node = RSCanvasNode::Create();
    RSNodeMap& map = RSNodeMap::MutableInstance();
    map.UnregisterNode(node->GetId());
    map.UnregisterNode(node->GetId());
}

/*
 * @tc.name: GetNode
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSNodeMapTest, GetNode, TestSize.Level1)
{
    RSCanvasNode::SharedPtr node = RSCanvasNode::Create();
    RSNodeMap& map = RSNodeMap::MutableInstance();
    auto node_g1 = map.GetNode(node->GetId());
    ASSERT_NE(node_g1, nullptr);
    map.UnregisterNode(node->GetId());
    auto node_g2 = map.GetNode(node->GetId());
    ASSERT_EQ(node_g2, nullptr);
}
} // namespace OHOS::Rosen