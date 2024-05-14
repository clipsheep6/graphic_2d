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

#include <gtest/gtest.h>

#include "ge_linear_gradient_blur_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GELinearGradientBlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GELinearGradientBlurShaderFilterTest::SetUpTestCase(void) {}
void GELinearGradientBlurShaderFilterTest::TearDownTestCase(void) {}

void GELinearGradientBlurShaderFilterTest::SetUp() {}

void GELinearGradientBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, GetDescription001, TestSize.Level1)
{
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::string expectStr = "GELinearGradientBlurShaderFilter";
    EXPECT_EQ(filter->GetDescription(), expectStr);
}

/**
 * @tc.name: GetDetailedDescription001
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, GetDetailedDescription001, TestSize.Level1)
{
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::string expectStr = "GELinearGradientBlurShaderFilterBlur, radius: " +std::to_string(params.blurRadius);
    EXPECT_EQ(filter->GetDetailedDescription(), expectStr);
}

/**
 * @tc.name: SetBoundsGeometry001
 * @tc.desc: Verify function SetBoundsGeometry
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, SetBoundsGeometry001, TestSize.Level1)
{
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);
    filter->SetBoundsGeometry(2.f, 2.f);
}

/**
 * @tc.name: ProcessImage001
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage001, TestSize.Level1)
{
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);

    ASSERT_TRUE(filter != nullptr);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = nullptr;
    Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);

    EXPECT_EQ(filter->ProcessImage(canvas, image, src, dst), image);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS