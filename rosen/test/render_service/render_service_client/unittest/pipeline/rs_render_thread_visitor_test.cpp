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
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_thread_visitor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderThreadVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderThreadVisitorTest::SetUpTestCase() {}
void RSRenderThreadVisitorTest::TearDownTestCase() {}
void RSRenderThreadVisitorTest::SetUp() {}
void RSRenderThreadVisitorTest::TearDown() {}

/**
 * @tc.name: CreateDrawingContext001
 * @tc.desc: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderThreadVisitorTest, IsValidRootRenderNode001, TestSize.Level1)
{
    auto visitor = std::make_shared<RSRenderThreadVisitor>();
    auto node = std::make_shared<RSCanvasRenderNode>();
    visitor->IsValidRootRenderNode(node);
}
    
} // namespace OHOS::Rosen
