/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "color_picker_unittest.h"
#include "color_picker.h"
#include "color.h"
#include "pixel_map.h"
#include "effect_errors.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
/**
 * @tc.name: CreateColorPickerFromPixelmapTest001
 * @tc.desc: Ensure the ability of creating color picker from pixelmap.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, CreateColorPickerFromPixelmapTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest CreateColorPickerFromPixelmapTest001 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    EXPECT_NE(pColorPicker, nullptr);
}

/**
 * @tc.name: CreateColorPickerFromPixelmapTest002
 * @tc.desc: Ensure the ability of creating color picker from pixelmap.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, CreateColorPickerFromPixelmapTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest CreateColorPickerFromPixelmapTest002 start";
    /**
     * @tc.steps: step1. Create a pixelMap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    EXPECT_NE(pColorPicker, nullptr);
}

/**
 * @tc.name: CreateColorPickerFromPixelmapTest003
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, CreateColorPickerFromPixelmapTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest CreateColorPickerFromPixelmapTest003 start";
    /**
     * @tc.steps: step1. Create a pixelMap
     */
    std::unique_ptr<Media::PixelMap> pixmap = nullptr;

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    EXPECT_NE(pColorPicker, nullptr);
}

/**
 * @tc.name: GetMainColorTest001
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetMainColorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest GetMainColorTest001 start";
    /**
     * @tc.steps: step1. Create a pixelMap
     */
    std::unique_ptr<Media::PixelMap> pixmap = nullptr;

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    EXPECT_EQ(pColorPicker, nullptr);

    /**
     * @tc.steps: step3. Get main color from pixmap
     */
    ColorManager::Color color;
    errorCode = pColorPicker->GetMainColor(color);
    EXPECT_EQ(color.r, 0.0f);
    EXPECT_EQ(color.g, 0.0f);
    EXPECT_EQ(color.b, 0.0f);
    EXPECT_EQ(color.a, 0.0f);
}

/**
 * @tc.name: GetMainColorTest002
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetMainColorTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest GetMainColorTest002 start";
    /**
     * @tc.steps: step1. Create a pixelMap
     */
    std::unique_ptr<Media::PixelMap> pixmap = nullptr;

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    EXPECT_EQ(pColorPicker, nullptr);

    /**
     * @tc.steps: step3. Get main color from pixmap
     */
    ColorManager::Color color;
    errorCode = pColorPicker->GetMainColor(color);
    EXPECT_EQ(color.r, 0.0f);
    EXPECT_EQ(color.g, 0.0f);
    EXPECT_EQ(color.b, 0.0f);
    EXPECT_EQ(color.a, 0.0f);
}

/**
 * @tc.name: GetMainColorTest003
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetMainColorTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest GetMainColorTest003 start";
    /**
     * @tc.steps: step1. Create a pixelMap
     */
    std::unique_ptr<Media::PixelMap> pixmap = nullptr;

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    EXPECT_EQ(pColorPicker, nullptr);

    /**
     * @tc.steps: step3. Get main color from pixmap
     */
    ColorManager::Color color;
    errorCode = pColorPicker->GetMainColor(color);
    EXPECT_EQ(color.r, 0.0f);
    EXPECT_EQ(color.g, 0.0f);
    EXPECT_EQ(color.b, 0.0f);
    EXPECT_EQ(color.a, 0.0f);
}
} // namespace Rosen
} // namespace OHOS
