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

#include "ge_kawase_blur_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEKawaseBlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEKawaseBlurShaderFilterTest::SetUpTestCase(void) {}
void GEKawaseBlurShaderFilterTest::TearDownTestCase(void) {}

void GEKawaseBlurShaderFilterTest::SetUp() {}

void GEKawaseBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetRadius001
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, GetRadius001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params{1};
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);

    EXPECT_EQ(geKawaseBlurShaderFilter->GetRadius(), 1);
}

/**
 * @tc.name: ProcessImage001
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params{1};
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = nullptr;
    Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);

    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas, image, src, dst), image);
}

/**
 * @tc.name: ProcessImage002
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage002, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params{0};
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);

    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);

    geKawaseBlurShaderFilter->ProcessImage(canvas, image, src, dst);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS