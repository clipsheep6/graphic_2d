/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "brush.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RSBrushTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBrushTest::SetUpTestCase() {}
void RSBrushTest::TearDownTestCase() {}
void RSBrushTest::SetUp() {}
void RSBrushTest::TearDown() {}

/**
 * @tc.name: CreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, CreateAndDestory001, TestSize.Level1)
{
    std::unique_ptr<Brush> ptrBrush = std::make_unique<Brush>();
    ASSERT_TRUE(nullptr != ptrBrush);
}

/**
 * @tc.name: CreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, CreateAndDestory002, TestSize.Level1)
{
    std::unique_ptr<Brush> ptrBrush = std::make_unique<Brush>();
    ASSERT_TRUE(nullptr != ptrBrush);
}

/**
 * @tc.name: CopyConstructor001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, CopyConstructor001, TestSize.Level1)
{
    Brush brush;
    Brush newBrush = brush;
    EXPECT_FALSE(brush != newBrush);
}

/**
 * @tc.name: CopyConstructor002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, CopyConstructor002, TestSize.Level1)
{
    Brush brush;
    Brush newBrush = brush;
    ASSERT_FALSE(brush != newBrush);
}

/**
 * @tc.name: CreateBrushWithColor001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, CreateBrushWithColor001, TestSize.Level1)
{
    Color color;
    std::unique_ptr<Brush> brush = std::make_unique<Brush>(color);
    EXPECT_FALSE(nullptr == brush);
}

/**
 * @tc.name: CreateBrushWithColor002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, CreateBrushWithColor002, TestSize.Level1)
{
    Color color;
    std::unique_ptr<Brush> brush = std::make_unique<Brush>(color);
    EXPECT_FALSE(nullptr == brush);
}

/**
 * @tc.name: CreateBrushWithShaderEffect001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, CreateBrushWithShaderEffect001, TestSize.Level1)
{
    auto shaderEffect = ShaderEffect::CreateColorShader(1);
    auto brush = std::make_unique<Brush>(shaderEffect);
    EXPECT_FALSE(nullptr == brush);
}

/**
 * @tc.name: CreateBrushWithShaderEffect002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, CreateBrushWithShaderEffect002, TestSize.Level1)
{
    auto shaderEffect = ShaderEffect::CreateColorShader(1);
    auto brush = std::make_unique<Brush>(shaderEffect);
    EXPECT_FALSE(nullptr == brush);
}

/**
 * @tc.name: CreateBrushWithShaderEffect001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, CreateBrushWithRGB001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>(222);
    EXPECT_FALSE(nullptr == brush);
}

/**
 * @tc.name: CreateBrushWithShaderEffect002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, CreateBrushWithRGB002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>(123);
    EXPECT_FALSE(nullptr == brush);
}

/**
 * @tc.name: GetColor001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetColor001, TestSize.Level1)
{
    Color color;
    auto brush = std::make_unique<Brush>(color);
    auto c = brush->GetColor();
    EXPECT_TRUE(c == color);
}

/**
 * @tc.name: GetColor002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetColor002, TestSize.Level1)
{
    Color color;
    auto brush = std::make_unique<Brush>(color);
    auto c = brush->GetColor();
    EXPECT_TRUE(color == c);
}

/**
 * @tc.name: SetColor001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetColor001, TestSize.Level1)
{
    Color color;
    auto brush = std::make_unique<Brush>();
    brush->SetColor(color);
}

/**
 * @tc.name: SetColor002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetColor002, TestSize.Level1)
{
    Color color;
    auto brush = std::make_unique<Brush>();
    brush->SetColor(color);
}

/**
 * @tc.name: SetARGB001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetARGB001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->SetARGB(255, 255, 255, 1);
}

/**
 * @tc.name: SetARGB002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetARGB002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->SetARGB(0, 0, 0, 1);
}

/**
 * @tc.name: GetColor4f001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetColor4f001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetColor4f();
}

/**
 * @tc.name: GetColor4f002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetColor4f002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetColor4f();
}

/**
 * @tc.name: GetColorSpace001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetColorSpace001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetColorSpace();
}

/**
 * @tc.name: GetColorSpace002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetColorSpace002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    auto colorSpace = brush->GetColorSpace();
}

/**
 * @tc.name: SetColorWithColor4fAndColorSpace001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetColorWithColor4fAndColorSpace001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    OHOS::Rosen::Drawing::Color4f color;
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
    brush->SetColor(color, colorSpace);
}

/**
 * @tc.name: SetColorWithColor4fAndColorSpace002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetColorWithColor4fAndColorSpace002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    OHOS::Rosen::Drawing::Color4f color;
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
    brush->SetColor(color, colorSpace);
}

/**
 * @tc.name: GetAlpha001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetAlpha001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetAlpha();
}

/**
 * @tc.name: GetAlpha002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetAlpha002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetAlpha();
}

/**
 * @tc.name: SetAlpha001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetAlpha001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->SetAlpha(1);
}

/**
 * @tc.name: SetAlpha002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetAlpha002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->SetAlpha(32);
}

/**
 * @tc.name: SetAlphaF001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetAlphaF001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->SetAlphaF(5.5);
}

/**
 * @tc.name: SetAlphaF002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetAlphaF002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->SetAlphaF(3.5);
}

/**
 * @tc.name: GetBlendMode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetBlendMode001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetBlendMode();
}

/**
 * @tc.name: GetBlendMode002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetBlendMode002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetBlendMode();
}

/**
 * @tc.name: SetBlendMode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetBlendMode001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->SetBlendMode(BlendMode::PLUS);
}

/**
 * @tc.name: SetBlendMode002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetBlendMode002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->SetBlendMode(BlendMode::SRC);
}

/**
 * @tc.name: SetFilter001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetFilter001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    Filter filter;
    brush->SetFilter(filter);
}

/**
 * @tc.name: SetFilter002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetFilter002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    Filter filter;
    brush->SetFilter(filter);
}

/**
 * @tc.name: GetFilter001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetFilter001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetFilter();
}

/**
 * @tc.name: GetFilter001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetFilter002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetFilter();
}

/**
 * @tc.name: SetShaderEffect001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetShaderEffect001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    auto shaderEffect = ShaderEffect::CreateColorShader(1);
    brush->SetShaderEffect(shaderEffect);
}

/**
 * @tc.name: SetShaderEffect002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetShaderEffect002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    auto shaderEffect = ShaderEffect::CreateColorShader(1);
    brush->SetShaderEffect(shaderEffect);
}

/**
 * @tc.name: GetShaderEffect001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetShaderEffect001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetShaderEffect();
}

/**
 * @tc.name: GetShaderEffect002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, GetShaderEffect002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->GetShaderEffect();
}

/**
 * @tc.name: IsAntiAlias001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, IsAntiAlias001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->IsAntiAlias();
}

/**
 * @tc.name: IsAntiAlias002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, IsAntiAlias002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->IsAntiAlias();
}

/**
 * @tc.name: SetAntiAlias001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetAntiAlias001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->SetAntiAlias(true);
}

/**
 * @tc.name: SetAntiAlias002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, SetAntiAlias002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->SetAntiAlias(false);
}

/**
 * @tc.name: Reset001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, Reset001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->Reset();
}

/**
 * @tc.name: Reset002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, Reset002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    brush->Reset();
}

/**
 * @tc.name: IsEquals001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, IsEquals001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    auto newBrush = std::make_unique<Brush>();
    EXPECT_FALSE(brush == newBrush);
}

/**
 * @tc.name: IsEquals002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, IsEquals002, TestSize.Level1)
{
    auto brush = std::make_shared<Brush>();
    auto newBrush = brush;
    EXPECT_FALSE(brush == newBrush);
}

/**
 * @tc.name: IsNotEquals001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, IsNotEquals001, TestSize.Level1)
{
    auto brush = std::make_shared<Brush>();
    auto newBrush = brush;
    EXPECT_FALSE(brush != newBrush);
}

/**
 * @tc.name: IsNotEquals002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSBrushTest, IsNotEquals002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    auto newBrush = std::make_unique<Brush>();
    EXPECT_FALSE(brush != newBrush);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
