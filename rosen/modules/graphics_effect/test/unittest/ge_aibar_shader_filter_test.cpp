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

#include "ge_aibar_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEAIBarShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    static inline Drawing::Canvas canvas_;

private:
    std::shared_ptr<Drawing::RuntimeEffect> MakeGreyAdjustmentEffect();

    std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect_;
};

void GEAIBarShaderFilterTest::SetUpTestCase(void) {}
void GEAIBarShaderFilterTest::TearDownTestCase(void) {}

void GEAIBarShaderFilterTest::SetUp()
{
    canvas_.Restore();
}

void GEAIBarShaderFilterTest::TearDown() {}


std::shared_ptr<Drawing::Image> GEAIBarShaderFilterTest::MakeImage(Drawing::Canvas& canvas)
{
    auto image = std::make_shared<Drawing::Image>();
    if (image == nullptr) {
        GTEST_LOG_(ERROR) << "GERenderTest::MakeImage image is null";
        return nullptr;
    }
    float greyX = 0.0f;
    float greyY = 1.0f;

    auto greyAdjustEffect = MakeGreyAdjustmentEffect();
    if (!greyAdjustEffect) {
        GTEST_LOG_(ERROR) << "GERenderTest::MakeImage greyAdjustEffect is null";
        return nullptr;
    }
    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(greyAdjustEffect);
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("coefficient1", greyX);
    builder->SetUniform("coefficient2", greyY);
    return builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
}

std::shared_ptr<Drawing::RuntimeEffect> GEAIBarShaderFilterTest::MakeGreyAdjustmentEffect()
{
    static const std::string GreyGradationString(R"(
        uniform shader imageShader;
        uniform float coefficient1;
        uniform float coefficient2;

        float poww(float x, float y) {
            return (x < 0) ? -pow(-x, y) : pow(x, y);
        }

        float calculateT_y(float rgb) {
            if (rgb > 127.5) { rgb = 255 - rgb; }
            float b = 38.0;
            float c = 45.0;
            float d = 127.5;
            float A = 106.5;    // 3 * b - 3 * c + d;
            float B = -93;      // 3 * (c - 2 * b);
            float C = 114;      // 3 * b;
            float p = 0.816240163988;                   // (3 * A * C - pow(B, 2)) / (3 * pow(A, 2));
            float q = -rgb / 106.5 + 0.262253485943;    // -rgb/A - B*C/(3*pow(A,2)) + 2*pow(B,3)/(27*pow(A,3))
            float s1 = -(q / 2.0);
            float s2 = sqrt(pow(s1, 2) + pow(p / 3, 3));
            return poww((s1 + s2), 1.0 / 3) + poww((s1 - s2), 1.0 / 3) - (B / (3 * A));
        }

        float calculateGreyAdjustY(float rgb) {
            float t_r = calculateT_y(rgb);
            return (rgb < 127.5) ? (rgb + coefficient1 * pow((1 - t_r), 3)) : (rgb - coefficient2 * pow((1 - t_r), 3));
        }

        vec4 main(vec2 drawing_coord) {
            vec3 color = vec3(imageShader(drawing_coord).r, imageShader(drawing_coord).g, imageShader(drawing_coord).b);
            float Y = (0.299 * color.r + 0.587 * color.g + 0.114 * color.b) * 255;
            float U = (-0.147 * color.r - 0.289 * color.g + 0.436 * color.b) * 255;
            float V = (0.615 * color.r - 0.515 * color.g - 0.100 * color.b) * 255;
            Y = calculateGreyAdjustY(Y);
            color.r = (Y + 1.14 * V) / 255.0;
            color.g = (Y - 0.39 * U - 0.58 * V) / 255.0;
            color.b = (Y + 2.03 * U) / 255.0;

            return vec4(color, 1.0);
        }
    )");
    if (!greyAdjustEffect_) {
        std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect =
            Drawing::RuntimeEffect::CreateForShader(GreyGradationString);
        if (!greyAdjustEffect) {
            return nullptr;
        }
        greyAdjustEffect_ = std::move(greyAdjustEffect);
    }

    return greyAdjustEffect_;
}

/**
 * @tc.name: ProcessImage001
 * @tc.desc: Verify the ProcessImage
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage001 start";

    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    const Drawing::Rect src { 1.0f, 1.0f, 1.0f, 1.0f };
    const Drawing::Rect dst { 1.0f, 1.0f, 1.0f, 1.0f };
    auto image = MakeImage(canvas_);
    if (!image) {
        GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest image is null";
        return;
    }
    EXPECT_NE(geAIBarShaderFilter->ProcessImage(canvas_, image, src, dst), image);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage001 end";
}

/**
 * @tc.name: ProcessImage002
 * @tc.desc: Verify the ProcessImage: image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage002 start";

    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    const Drawing::Rect src { 1.0f, 1.0f, 1.0f, 1.0f };
    const Drawing::Rect dst { 1.0f, 1.0f, 1.0f, 1.0f };
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, nullptr, src, dst), nullptr);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage002 end";
}

/**
 * @tc.name: ProcessImage003
 * @tc.desc: Verify the ProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage003 start";

    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { -0.5, -0.5, -0.5, -0.5, -0.5 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    const Drawing::Rect src { 1.0f, 1.0f, 1.0f, 1.0f };
    const Drawing::Rect dst { 1.0f, 1.0f, 1.0f, 1.0f };
    auto image = MakeImage(canvas_);
    if (!image) {
        GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest image is null";
        return;
    }
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, image, src, dst), image);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage003 end";
}

/**
 * @tc.name: ProcessImage004
 * @tc.desc: Verify the ProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage004 start";

    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 2.0, 2.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    const Drawing::Rect src { 1.0f, 1.0f, 1.0f, 1.0f };
    const Drawing::Rect dst { 1.0f, 1.0f, 1.0f, 1.0f };
    auto image = MakeImage(canvas_);
    if (!image) {
        GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest image is null";
        return;
    }
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, image, src, dst), image);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage004 end";
}

/**
 * @tc.name: ProcessImage005
 * @tc.desc: Verify the ProcessImage: image is empty
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage005 start";

    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    const Drawing::Rect src { 1.0f, 1.0f, 1.0f, 1.0f };
    const Drawing::Rect dst { 1.0f, 1.0f, 1.0f, 1.0f };
    auto image = std::make_shared<Drawing::Image>();
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, image, src, dst), image);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage005 end";
}

/**
 * @tc.name: GetDescription001
 * @tc.desc: Verify the GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, GetDescription001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest GetDescription001 start";

    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->GetDescription(), "GEAIBarShaderFilter");

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest GetDescription001 end";
}

} // namespace Rosen
} // namespace OHOS
