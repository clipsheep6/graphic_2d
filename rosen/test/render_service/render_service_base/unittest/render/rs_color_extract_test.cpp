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
#include "include/render/rs_color_extract.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorExtractTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorExtractTest::SetUpTestCase() {}
void RSColorExtractTest::TearDownTestCase() {}
void RSColorExtractTest::SetUp() {}
void RSColorExtractTest::TearDown() {}

/**
 * @tc.name: QuantizedRedTest001
 * @tc.desc: Verify function QuantizedRed
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, QuantizedRedTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->QuantizedRed(0), 0);
}

/**
 * @tc.name: QuantizedGreenTest001
 * @tc.desc: Verify function QuantizedGreen
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, QuantizedGreenTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->QuantizedGreen(0), 0);
}

/**
 * @tc.name: QuantizedBlueTest001
 * @tc.desc: Verify function QuantizedBlue
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, QuantizedBlueTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->QuantizedBlue(0), 0);
}

/**
 * @tc.name: ModifyWordWidthTest001
 * @tc.desc: Verify function ModifyWordWidth
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, ModifyWordWidthTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->ModifyWordWidth(0, 0, 1), 0);
    EXPECT_EQ(extract->ModifyWordWidth(0, 1, 0), 0);
}

/**
 * @tc.name: GetARGB32ColorRTest001
 * @tc.desc: Verify function GetARGB32ColorR
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, GetARGB32ColorRTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->GetARGB32ColorR(0), 0);
}

/**
 * @tc.name: GetARGB32ColorGTest001
 * @tc.desc: Verify function GetARGB32ColorG
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, GetARGB32ColorGTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->GetARGB32ColorG(0), 0);
}

/**
 * @tc.name: GetARGB32ColorBTest001
 * @tc.desc: Verify function GetARGB32ColorB
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, GetARGB32ColorBTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->GetARGB32ColorB(0), 0);
}

/**
 * @tc.name: QuantizeFromRGB888Test001
 * @tc.desc: Verify function QuantizeFromRGB888
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, QuantizeFromRGB888Test001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->QuantizeFromRGB888(0), 0);
}

/**
 * @tc.name: ApproximateToRGB888Test001
 * @tc.desc: Verify function ApproximateToRGB888
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, ApproximateToRGB888Test001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->ApproximateToRGB888(0, 0, 0), 0);
}

/**
 * @tc.name: ApproximateToRGB888Test002
 * @tc.desc: Verify function ApproximateToRGB888
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, ApproximateToRGB888Test002, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->ApproximateToRGB888(0), 0);
}

/**
 * @tc.name: QuantizePixelsTest001
 * @tc.desc: Verify function QuantizePixels
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, QuantizePixelsTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_FALSE(extract->QuantizePixels(0).empty());
}

/**
 * @tc.name: SplitBoxesTest001
 * @tc.desc: Verify function SplitBoxes
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, SplitBoxesTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    std::priority_queue<RSColorExtract::VBox, std::vector<RSColorExtract::VBox>, std::less<RSColorExtract::VBox>> q;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    RSColorExtract colorExtract(pixmap);
    RSColorExtract::VBox v(1, 0, &colorExtract);
    q.push(v);
    colorExtract.SplitBoxes(q, 0);
    EXPECT_FALSE(q.empty());
    extract->SplitBoxes(q, 2);
    EXPECT_TRUE(q.empty());
    RSColorExtract::VBox vv(1, 0, &colorExtract);
    q.push(vv);
    extract->SplitBoxes(q, 2);
    EXPECT_TRUE(q.empty());
}

/**
 * @tc.name: cmpTest001
 * @tc.desc: Verify function cmp
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, cmpTest001, TestSize.Level1)
{
    std::pair<uint32_t, uint32_t> a(1, 2);
    std::pair<uint32_t, uint32_t> b(2, 1);
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_TRUE(extract->cmp(a, b));
}

/**
 * @tc.name: GenerateAverageColorsTest001
 * @tc.desc: Verify function GenerateAverageColors
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, GenerateAverageColorsTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    std::priority_queue<RSColorExtract::VBox, std::vector<RSColorExtract::VBox>, std::less<RSColorExtract::VBox>> q;
    EXPECT_TRUE(extract->GenerateAverageColors(q).empty());
    RSColorExtract colorExtract(pixmap);
    RSColorExtract::VBox v(1, 0, &colorExtract);
    q.push(v);
    EXPECT_FALSE(extract->GenerateAverageColors(q).empty());
}

/**
 * @tc.name: SetFeatureColorNumTest001
 * @tc.desc: Verify function SetFeatureColorNum
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, SetFeatureColorNumTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    extract->SetFeatureColorNum(0);
    EXPECT_EQ(extract->specifiedFeatureColorNum_, 0);
}

/**
 * @tc.name: Rgb2GrayTest001
 * @tc.desc: Verify function Rgb2Gray
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, Rgb2GrayTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->Rgb2Gray(0), 0);
}

/**
 * @tc.name: CalcGrayMsdTest001
 * @tc.desc: Verify function CalcGrayMsd
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, CalcGrayMsdTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->CalcGrayMsd(), 0);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto extract2 = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract2->CalcGrayMsd(), 0);
}

/**
 * @tc.name: NormalizeRgbTest001
 * @tc.desc: Verify function NormalizeRgb
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, NormalizeRgbTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_NE(extract->NormalizeRgb(11), 0.f);
    EXPECT_NE(extract->NormalizeRgb(1), 0.f);
}

/**
 * @tc.name: CalcRelativeLumTest001
 * @tc.desc: Verify function CalcRelativeLum
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, CalcRelativeLumTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->CalcRelativeLum(0), 0.f);
}

/**
 * @tc.name: CalcContrastToWhiteTest001
 * @tc.desc: Verify function CalcContrastToWhite
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, CalcContrastToWhiteTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract->CalcContrastToWhite(), 0.f);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto extract2 = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_NE(extract2->CalcContrastToWhite(), 0.f);
}

/**
 * @tc.name: GetNFeatureColorsTest001
 * @tc.desc: Verify function GetNFeatureColors
 * @tc.type:FUNC
 */
HWTEST_F(RSColorExtractTest, GetNFeatureColorsTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto extract = std::make_shared<RSColorExtract>(pixmap);
    extract->GetNFeatureColors(0);
    EXPECT_EQ(extract->colorValLen_, 0);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto extract2 = std::make_shared<RSColorExtract>(pixmap);
    extract2->GetNFeatureColors(0);
    extract2->GetNFeatureColors(1);
    EXPECT_NE(extract2->colorValLen_, 0);
}

} // namespace OHOS::Rosen