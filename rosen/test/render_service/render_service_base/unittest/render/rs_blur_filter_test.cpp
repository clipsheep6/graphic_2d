/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "render/rs_blur_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBlurFilterTest::SetUpTestCase() {}
void RSBlurFilterTest::TearDownTestCase() {}
void RSBlurFilterTest::SetUp() {}
void RSBlurFilterTest::TearDown() {}

/**
 * @tc.name: GetBlurRadiusXTest001
 * @tc.desc: Verify function GetBlurRadiusX
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, GetBlurRadiusXTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    float blurRadiusX1 = filter->GetBlurRadiusX();
    EXPECT_EQ(blurRadiusX, blurRadiusX1);
}

/**
 * @tc.name: GetBlurRadiusYTest001
 * @tc.desc: Verify function GetBlurRadiusY
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, GetBlurRadiusYTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    float blurRadiusY1 = filter->GetBlurRadiusY();
    EXPECT_EQ(blurRadiusY, blurRadiusY1);
}

/**
 * @tc.name: GetDescriptionTest001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, GetDescriptionTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    std::string str = blurFilter->GetDescription();
    std::string str2 = "RSBlurFilter blur radius is " + std::to_string(blurRadiusX) + " sigma";
    EXPECT_EQ(str, str2);
}

/**
 * @tc.name: IsValidTest001
 * @tc.desc: Verify function IsValid
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, IsValidTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    EXPECT_TRUE(blurFilter->IsValid());
}

/**
 * @tc.name: ComposeTest001
 * @tc.desc: Verify function Compose
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, ComposeTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto composedFilter = blurFilter1->Compose(blurFilter2);
    EXPECT_NE(composedFilter, nullptr);
}

/**
 * @tc.name: AddTest001
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, AddTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Add(nullptr);
    EXPECT_NE(blurFilter2, nullptr);

    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::DEFAULT, 1.0f);
    auto blurFilter3 = blurFilter1->Add(materialFilter);
    EXPECT_NE(blurFilter3, nullptr);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    auto blurFilter4 = blurFilter1->Add(rhs);
    EXPECT_NE(blurFilter4, nullptr);
}

/**
 * @tc.name: SubTest001
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, SubTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Add(nullptr);
    EXPECT_NE(blurFilter2, nullptr);

    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::DEFAULT, 1.0f);
    auto blurFilter3 = blurFilter1->Add(materialFilter);
    EXPECT_NE(blurFilter3, nullptr);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    auto blurFilter4 = blurFilter1->Add(rhs);
    EXPECT_NE(blurFilter4, nullptr);
}

/**
 * @tc.name: MultiplyTest001
 * @tc.desc: Verify function Multiply
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, MultiplyTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Multiply(1.0f);
    EXPECT_NE(blurFilter2, nullptr);
}

/**
 * @tc.name: NegateTest001
 * @tc.desc: Verify function Negate
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, NegateTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Negate();
    EXPECT_NE(blurFilter2, nullptr);
}

/**
 * @tc.name: RSBlurFilterTest001
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, RSBlurFilterTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);

    float blurRadiusX1 = 1.2f;
    std::shared_ptr<RSFilter> other1 = std::make_shared<RSBlurFilter>(blurRadiusX1, blurRadiusY);

    float threshold = 1.0f;
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
}

/**
 * @tc.name: IsNearZero001
 * @tc.desc: Verify function IsNearZero
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, IsNearZero001, TestSize.Level1)
{
    float blurRadiusX = 0.2f;
    float blurRadiusY = 0.2f;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);

    float threshold = 0.5f;
    EXPECT_TRUE(filter->IsNearZero(threshold));
}

/**
 * @tc.name: SetGreyCoefTest001
 * @tc.desc: Verify function SetGreyCoef
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, SetGreyCoefTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    Vector2f vector2;
    std::optional<Vector2f> greyCoef = vector2;
    blurFilter->SetGreyCoef(greyCoef);
    EXPECT_EQ(blurFilter->greyCoef_, greyCoef);
}

/**
 * @tc.name: CanSkipFrameTest001
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, CanSkipFrameTest001, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    EXPECT_TRUE(blurFilter->CanSkipFrame());
}
} // namespace OHOS::Rosen
