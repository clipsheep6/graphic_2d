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

#include <cstddef>
#include "gtest/gtest.h"
#include "color_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RSColorFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorFilterTest::SetUpTestCase() {}
void RSColorFilterTest::TearDownTestCase() {}
void RSColorFilterTest::SetUp() {}
void RSColorFilterTest::TearDown() {}

/**
 * @tc.name: CreateBlendModeColorFilter001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateBlendModeColorFilter001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(11, OHOS::Rosen::Drawing::BlendMode::CLEAR);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateBlendModeColorFilter002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateBlendModeColorFilter002, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(10, OHOS::Rosen::Drawing::BlendMode::SRC);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateComposeColorFilter001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateComposeColorFilter001, TestSize.Level1)
{
    ColorFilter f1(ColorFilter::FilterType::NO_TYPE);
    ColorFilter f2(ColorFilter::FilterType::MATRIX);
    auto colorFilter = ColorFilter::CreateComposeColorFilter(f1, f2);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateComposeColorFilter002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateComposeColorFilter002, TestSize.Level1)
{
    ColorFilter f1(ColorFilter::FilterType::COMPOSE);
    ColorFilter f2(ColorFilter::FilterType::MATRIX);
    auto colorFilter = ColorFilter::CreateComposeColorFilter(f1, f2);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateMatrixColorFilter001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateMatrixColorFilter001, TestSize.Level1)
{
    ColorMatrix m;
    auto colorFilter = ColorFilter::CreateMatrixColorFilter(m);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateMatrixColorFilter002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateMatrixColorFilter002, TestSize.Level1)
{
    ColorMatrix m;
    auto colorFilter = ColorFilter::CreateMatrixColorFilter(m);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateLinearToSrgbGamma001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateLinearToSrgbGamma001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateLinearToSrgbGamma002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateLinearToSrgbGamma002, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateSrgbGammaToLinear001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateSrgbGammaToLinear001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateSrgbGammaToLinear();
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateSrgbGammaToLinear002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateSrgbGammaToLinear002, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateSrgbGammaToLinear();
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: GetType001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, GetType001, TestSize.Level1)
{
    ColorFilter::FilterType filter = ColorFilter::FilterType::NO_TYPE;
    auto colorFilter = std::make_shared<ColorFilter>(filter);
    auto filterType = colorFilter->GetType();
    EXPECT_TRUE(filterType == ColorFilter::FilterType::NO_TYPE);
}

/**
 * @tc.name: GetType002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, GetType002, TestSize.Level1)
{
    auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::MATRIX);
    auto filterType = colorFilter->GetType();
    EXPECT_TRUE(filterType == ColorFilter::FilterType::MATRIX);
}

/**
 * @tc.name: Compose001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, Compose001, TestSize.Level1)
{
    auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::MATRIX);
    ColorFilter filter(ColorFilter::FilterType::BLEND_MODE);
    colorFilter->Compose(filter);
}

/**
 * @tc.name: Compose002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, Compose002, TestSize.Level1)
{
    auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::MATRIX);
    ColorFilter filter(ColorFilter::FilterType::BLEND_MODE);
    colorFilter->Compose(filter);
}

/**
 * @tc.name: CreateInstance001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateInstance001, TestSize.Level1)
{
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE,
        10, OHOS::Rosen::Drawing::BlendMode::DST);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateInstance002, TestSize.Level1)
{
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE,
        2, OHOS::Rosen::Drawing::BlendMode::SRC);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateInstance003, TestSize.Level1)
{
    ColorMatrix m;
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE, m);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateInstance004, TestSize.Level1)
{
    ColorMatrix m;
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE, m);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateInstance005, TestSize.Level1)
{
    ColorFilter f1(ColorFilter::FilterType::BLEND_MODE);
    ColorFilter f2(ColorFilter::FilterType::BLEND_MODE);
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE, f1, f2);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance006
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateInstance006, TestSize.Level1)
{
    ColorFilter f1(ColorFilter::FilterType::BLEND_MODE);
    ColorFilter f2(ColorFilter::FilterType::BLEND_MODE);
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE, f1, f2);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance007
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateInstance007, TestSize.Level1)
{
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance008
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSColorFilterTest, CreateInstance008, TestSize.Level1)
{
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE);
    EXPECT_FALSE(colorFilter == nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS