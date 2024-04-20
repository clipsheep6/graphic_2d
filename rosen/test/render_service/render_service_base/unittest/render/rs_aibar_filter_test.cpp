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

#include "render/rs_aibar_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAIBarFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAIBarFilterTest::SetUpTestCase() {}
void RSAIBarFilterTest::TearDownTestCase() {}
void RSAIBarFilterTest::SetUp() {}
void RSAIBarFilterTest::TearDown() {}

/**
 * @tc.name: DrawImageRectTest001
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarFilterTest, DrawImageRectTest001, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    filter->DrawImageRect(canvas, image, src, dst);
}

/**
 * @tc.name: GetDescriptionTest001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarFilterTest, GetDescriptionTest001, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    std::string str = filter->GetDescription();
    EXPECT_EQ(str, "RSAIBarFilter");
}

/**
 * @tc.name: GetAiInvertCoefTest001
 * @tc.desc: Verify function GetAiInvertCoef
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarFilterTest, GetAiInvertCoefTest001, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_EQ(filter->GetAiInvertCoef(), aiInvertCoef);
}

/**
 * @tc.name: IsAiInvertCoefValidTest001
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarFilterTest, IsAiInvertCoefValidTest001, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_TRUE(filter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: MakeBinarizationShaderTest001
 * @tc.desc: Verify function MakeBinarizationShader
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarFilterTest, MakeBinarizationShaderTest001, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    float imageWidth = 1.0f;
    float imageHeight = 1.0f;
    auto imageShader = std::make_shared<Drawing::ShaderEffect>();
    auto runtimeShaderBuilder = filter->MakeBinarizationShader(imageWidth, imageHeight, imageShader);
    EXPECT_NE(runtimeShaderBuilder, nullptr);
}

} // namespace OHOS::Rosen