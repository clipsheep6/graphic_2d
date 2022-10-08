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
#include "pipeline/rs_node_map.h"
#include "ui/rs_base_node.h"

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

/**
 * @tc.name: RegisterNode001
 * @tc.desc: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeMapTest, RegisterNode001, TestSize.Level1)
{
    auto instance = RSNodeMap::MutableInstance();
    auto sharedPtr = std::make_shared<RSBaseNode>();
    EXPECT_EQ(instance.RegisterNode(sharedPtr),false);
}

/**
 * @tc.name: RegisterNode001
 * @tc.desc: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeMapTest, RegisterNode001, TestSize.Level1)
{
    auto instance = RSNodeMap::MutableInstance();
    auto sharedPtr = std::make_shared<RSBaseNode>();
    EXPECT_EQ(instance.RegisterNode(sharedPtr),false);
} 
    
} // namespace OHOS::Rosen
