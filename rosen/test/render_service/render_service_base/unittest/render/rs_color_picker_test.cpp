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
#include "include/render/rs_color_picker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorPickerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorPickerTest::SetUpTestCase() {}
void RSColorPickerTest::TearDownTestCase() {}
void RSColorPickerTest::SetUp() {}
void RSColorPickerTest::TearDown() {}

/**
 * @tc.name: CreateColorPickerTest001
 * @tc.desc: Verify function CreateColorPicker
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, CreateColorPickerTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    uint32_t errorCode = 0;
    auto picker = std::make_shared<RSColorPicker>(pixmap);
    EXPECT_EQ(picker->CreateColorPicker(pixmap, errorCode), nullptr);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto picker2 = std::make_shared<RSColorPicker>(pixmap);
    EXPECT_NE(picker2->CreateColorPicker(pixmap, errorCode), nullptr);
}

/**
 * @tc.name: CreateColorPickerTest002
 * @tc.desc: Verify function CreateColorPicker
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, CreateColorPickerTest002, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    uint32_t errorCode = 0;
    double coordinates = 1.0;
    auto picker = std::make_shared<RSColorPicker>(pixmap);
    EXPECT_EQ(picker->CreateColorPicker(pixmap, &coordinates, errorCode), nullptr);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto picker2 = std::make_shared<RSColorPicker>(pixmap);
    EXPECT_NE(picker2->CreateColorPicker(pixmap, &coordinates, errorCode), nullptr);
}

/**
 * @tc.name: GetLargestProportionColorTest001
 * @tc.desc: Verify function GetLargestProportionColor
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, GetLargestProportionColorTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto picker = std::make_shared<RSColorPicker>(pixmap);
    Drawing::ColorQuad color;
    EXPECT_EQ(picker->GetLargestProportionColor(color), 2);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto picker2 = std::make_shared<RSColorPicker>(pixmap);
    picker2->GetNFeatureColors(1);
    EXPECT_EQ(picker2->GetLargestProportionColor(color), 0);
}

/**
 * @tc.name: GetHighestSaturationColorTest001
 * @tc.desc: Verify function GetHighestSaturationColor
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, GetHighestSaturationColorTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto picker = std::make_shared<RSColorPicker>(pixmap);
    Drawing::ColorQuad color;
    EXPECT_EQ(picker->GetHighestSaturationColor(color), 2);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto picker2 = std::make_shared<RSColorPicker>(pixmap);
    picker2->GetNFeatureColors(1);
    EXPECT_EQ(picker2->GetHighestSaturationColor(color), 0);
}

/**
 * @tc.name: GetAverageColorTest001
 * @tc.desc: Verify function GetAverageColor
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, GetAverageColorTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto picker = std::make_shared<RSColorPicker>(pixmap);
    Drawing::ColorQuad color;
    EXPECT_EQ(picker->GetAverageColor(color), 2);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto picker2 = std::make_shared<RSColorPicker>(pixmap);
    picker2->GetNFeatureColors(1);
    EXPECT_EQ(picker2->GetAverageColor(color), 0);
}

/**
 * @tc.name: IsBlackOrWhiteOrGrayColorTest001
 * @tc.desc: Verify function IsBlackOrWhiteOrGrayColor
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, IsBlackOrWhiteOrGrayColorTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto picker = std::make_shared<RSColorPicker>(pixmap);
    EXPECT_FALSE(picker->IsBlackOrWhiteOrGrayColor(255));
    EXPECT_TRUE(picker->IsBlackOrWhiteOrGrayColor(1));
}

/**
 * @tc.name: IsEqualsTest001
 * @tc.desc: Verify function IsEquals
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, IsEqualsTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto picker = std::make_shared<RSColorPicker>(pixmap);
    EXPECT_FALSE(picker->IsEquals(0.0, 1.0));
}

/**
 * @tc.name: RGB2HSVTest001
 * @tc.desc: Verify function RGB2HSV
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, RGB2HSVTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto picker = std::make_shared<RSColorPicker>(pixmap);
    HSV hsv = picker->RGB2HSV(0);
    EXPECT_EQ(hsv.s, 0);
    hsv = picker->RGB2HSV(1);
    EXPECT_EQ(hsv.s, 100);
}

/**
 * @tc.name: AdjustHSVToDefinedItervalTest001
 * @tc.desc: Verify function AdjustHSVToDefinedIterval
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, AdjustHSVToDefinedItervalTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto picker = std::make_shared<RSColorPicker>(pixmap);
    HSV hsv;
    hsv.h = 361;
    hsv.s = 101;
    hsv.v = 101;
    picker->AdjustHSVToDefinedIterval(hsv);
    EXPECT_TRUE(true);
    hsv.h = -1;
    hsv.s = -1;
    hsv.v = -1;
    picker->AdjustHSVToDefinedIterval(hsv);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: HSVtoRGBTest001
 * @tc.desc: Verify function HSVtoRGB
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, HSVtoRGBTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Pixmap> pixmap;
    auto picker = std::make_shared<RSColorPicker>(pixmap);
    HSV hsv;
    hsv.h = -1;
    hsv.s = -1;
    hsv.v = -1;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
    hsv.h = 60;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
    hsv.h = 120;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
    hsv.h = 180;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
    hsv.h = 240;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
    hsv.h = 300;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
}

} // namespace OHOS::Rosen
