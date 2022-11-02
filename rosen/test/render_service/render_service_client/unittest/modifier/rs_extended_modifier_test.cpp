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
#include "modifier/rs_extended_modifier.h"
#include "pipeline/rs_node_map.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSExtendedModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSExtendedModifierTest::SetUpTestCase() {}
void RSExtendedModifierTest::TearDownTestCase() {}
void RSExtendedModifierTest::SetUp() {}
void RSExtendedModifierTest::TearDown() {}

/*
 * @tc.name: CreateDrawingContext
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSExtendedModifierTest, CreateDrawingContext, TestSize.Level1)
{
    RSCanvasNode::SharedPtr node = RSCanvasNode::Create();
    RSNodeMap::MutableInstance().UnregisterNode(node->GetId());
    auto context = RSExtendedModifierHelper::CreateDrawingContext(node->GetId());
    // return the struct,so extract the SkCanvas from the struct.
    SkCanvas* canvas = context.canvas;
    ASSERT_EQ(canvas, nullptr);
}
}