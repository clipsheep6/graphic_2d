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

#include "animation/rs_animation_trace_utils.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationTraceUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationTraceUtilsTest::SetUpTestCase() {}
void RSAnimationTraceUtilsTest::TearDownTestCase() {}
void RSAnimationTraceUtilsTest::SetUp() {}
void RSAnimationTraceUtilsTest::TearDown() {}

/**
 * @tc.name: ParseRenderPropertyVaule001
 * @tc.desc: test results of ParseRenderPropertyVaule
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTraceUtilsTest, ParseRenderPropertyVaule001, TestSize.Level1)
{
    RSAnimationTraceUtils traceUtils;
    auto value = std::make_shared<RSRenderPropertyBase>();
    RSRenderPropertyType type = RSRenderPropertyType::INVALID;
    std::string strRes = traceUtils.ParseRenderPropertyVaule(value, type);
    EXPECT_TRUE(strRes == "None");

    type = RSRenderPropertyType::PROPERTY_FLOAT;
    strRes = traceUtils.ParseRenderPropertyVaule(value, type);
    EXPECT_TRUE(strRes != "");

    type = RSRenderPropertyType::PROPERTY_COLOR;
    strRes = traceUtils.ParseRenderPropertyVaule(value, type);
    EXPECT_TRUE(strRes != "");

    type = RSRenderPropertyType::PROPERTY_MATRIX3F;
    strRes = traceUtils.ParseRenderPropertyVaule(value, type);
    EXPECT_TRUE(strRes != "");

    type = RSRenderPropertyType::PROPERTY_QUATERNION;
    strRes = traceUtils.ParseRenderPropertyVaule(value, type);
    EXPECT_TRUE(strRes != "");

    type = RSRenderPropertyType::PROPERTY_FILTER;
    strRes = traceUtils.ParseRenderPropertyVaule(value, type);
    EXPECT_TRUE(strRes != "");

    type = RSRenderPropertyType::PROPERTY_VECTOR2F;
    strRes = traceUtils.ParseRenderPropertyVaule(value, type);
    EXPECT_TRUE(strRes != "");

    type = RSRenderPropertyType::PROPERTY_VECTOR4F;
    strRes = traceUtils.ParseRenderPropertyVaule(value, type);
    EXPECT_TRUE(strRes != "");

    type = RSRenderPropertyType::PROPERTY_VECTOR4_COLOR;
    strRes = traceUtils.ParseRenderPropertyVaule(value, type);
    EXPECT_TRUE(strRes != "");
}

/**
 * @tc.name: addAnimationNameTrace001
 * @tc.desc: test results of addAnimationNameTrace
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTraceUtilsTest, addAnimationNameTrace001, TestSize.Level1)
{
    RSAnimationTraceUtils traceUtils;
    std::string name = "";
    traceUtils.isDebugOpen_ = true;
    traceUtils.addAnimationNameTrace(name, 1);
    EXPECT_TRUE(traceUtils.isDebugOpen_);
}

/**
 * @tc.name: addAnimationFinishTrace001
 * @tc.desc: test results of addAnimationFinishTrace
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTraceUtilsTest, addAnimationFinishTrace001, TestSize.Level1)
{
    RSAnimationTraceUtils traceUtils;
    traceUtils.isDebugOpen_ = true;
    traceUtils.addAnimationFinishTrace(1, 1);
    EXPECT_TRUE(traceUtils.isDebugOpen_);
}

/**
 * @tc.name: addAnimationCreateTrace001
 * @tc.desc: test results of addAnimationCreateTrace
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTraceUtilsTest, addAnimationCreateTrace001, TestSize.Level1)
{
    RSAnimationTraceUtils traceUtils;
    traceUtils.isDebugOpen_ = true;
    auto startValue = std::make_shared<RSRenderPropertyBase>();
    auto endValue = std::make_shared<RSRenderPropertyBase>();
    traceUtils.addAnimationCreateTrace(1, 2, 1, 2, 1, startValue, endValue, 2, 1);
    EXPECT_TRUE(traceUtils.isDebugOpen_);
}

/**
 * @tc.name: addAnimationFrameTrace001
 * @tc.desc: test results of addAnimationFrameTrace
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTraceUtilsTest, addAnimationFrameTrace001, TestSize.Level1)
{
    RSAnimationTraceUtils traceUtils;
    traceUtils.isDebugOpen_ = true;
    auto value = std::make_shared<RSRenderPropertyBase>();
    traceUtils.addAnimationFrameTrace(1, 2, 1, 1.f, value, 2);
    EXPECT_TRUE(traceUtils.isDebugOpen_);
}

/**
 * @tc.name: addSpringInitialVelocityTrace001
 * @tc.desc: test results of addSpringInitialVelocityTrace
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTraceUtilsTest, addSpringInitialVelocityTrace001, TestSize.Level1)
{
    RSAnimationTraceUtils traceUtils;
    traceUtils.isDebugOpen_ = true;
    auto initialVelocity = std::make_shared<RSRenderPropertyBase>();
    auto value = std::make_shared<RSRenderPropertyBase>();
    traceUtils.addSpringInitialVelocityTrace(1, 2, initialVelocity, value);
    EXPECT_TRUE(traceUtils.isDebugOpen_);
}

/**
 * @tc.name: addRenderNodeTrace001
 * @tc.desc: test results of addRenderNodeTrace
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTraceUtilsTest, addRenderNodeTrace001, TestSize.Level1)
{
    RSAnimationTraceUtils traceUtils;
    RSRenderNode node(1);
    std::string name = "";
    traceUtils.isDebugOpen_ = true;
    traceUtils.addRenderNodeTrace(node, name);
    EXPECT_TRUE(traceUtils.isDebugOpen_);

    name = "node";
    traceUtils.addRenderNodeTrace(node, name);
    EXPECT_TRUE(traceUtils.isDebugOpen_);

    traceUtils.isDebugOpen_ = false;
    traceUtils.addRenderNodeTrace(node, name);
    EXPECT_TRUE(!traceUtils.isDebugOpen_);
}
} // namespace OHOS::Rosen