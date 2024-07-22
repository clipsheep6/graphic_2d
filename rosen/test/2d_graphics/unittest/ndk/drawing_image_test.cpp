/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "drawing_bitmap.h"
#include "drawing_color_space.h"
#include "drawing_error_code.h"
#include "drawing_image.h"
#include "drawing_sampling_options.h"
#include "effect/color_space.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeImageTest::SetUpTestCase() {}
void NativeImageTest::TearDownTestCase() {}
void NativeImageTest::SetUp() {}
void NativeImageTest::TearDown() {}

/*
 * @tc.name: NativeImageTest_BuildFromBitmap001
 * @tc.desc: test BuildFromBitmap
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeImageTest, NativeImageTest_BuildFromBitmap001, TestSize.Level1)
{
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    EXPECT_NE(image, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    EXPECT_TRUE(OH_Drawing_ImageBuildFromBitmap(image, bitmap));
    EXPECT_TRUE(!OH_Drawing_ImageBuildFromBitmap(image, nullptr));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_TRUE(!OH_Drawing_ImageBuildFromBitmap(nullptr, nullptr));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
}

/*
 * @tc.name: NativeImageTest_GetWidth001
 * @tc.desc: test GetWidth
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeImageTest, NativeImageTest_GetWidth001, TestSize.Level1)
{
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    EXPECT_NE(image, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    EXPECT_EQ(OH_Drawing_ImageGetWidth(image), width);
    EXPECT_EQ(OH_Drawing_ImageGetWidth(nullptr), -1);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
}

/*
 * @tc.name: NativeImageTest_GetHeight001
 * @tc.desc: test GetHeight
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeImageTest, NativeImageTest_GetHeight001, TestSize.Level1)
{
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    EXPECT_NE(image, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    EXPECT_EQ(OH_Drawing_ImageGetHeight(image), height);
    EXPECT_EQ(OH_Drawing_ImageGetHeight(nullptr), -1);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
}

/*
 * @tc.name: NativeImageTest_GetImageInfo001
 * @tc.desc: test GetImageInfo
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeImageTest, NativeImageTest_GetImageInfo001, TestSize.Level1)
{
    OH_Drawing_ImageGetImageInfo(nullptr, nullptr);
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    EXPECT_NE(image, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    OH_Drawing_ImageGetImageInfo(image, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ImageGetImageInfo(nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_Image_Info imageInfo;
    OH_Drawing_ImageGetImageInfo(image, &imageInfo);
    EXPECT_EQ(imageInfo.width, width);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
}

/*
 * @tc.name: NativeImageTest_GetColorSpace001
 * @tc.desc: test GetColorSpace
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeImageTest, NativeImageTest_GetColorSpace001, TestSize.Level1)
{
    OH_Drawing_ImageGetImageInfo(nullptr, nullptr);
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    EXPECT_NE(image, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);

    OH_Drawing_ImageGetImageInfo(image, nullptr);
    OH_Drawing_Image_Info imageInfo;
    OH_Drawing_ImageGetImageInfo(image, &imageInfo);
    OH_Drawing_ColorSpace *colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    EXPECT_NE(colorSpace, nullptr);
    OH_Drawing_Bitmap *bitmapNew = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, 0);
    EXPECT_NE(bitmapNew, nullptr);
    OH_Drawing_ImageBuildFromBitmap(image, bitmapNew);
    colorSpace = OH_Drawing_ImageGetColorSpace(image);
    EXPECT_NE(colorSpace, nullptr);
    EXPECT_EQ(reinterpret_cast<ColorSpace*>(colorSpace)->GetType(), ColorSpace::ColorSpaceType::NO_TYPE);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_BitmapDestroy(bitmapNew);
    OH_Drawing_ImageDestroy(image);
}

/*
 * @tc.name: NativeImageTest_GetUniqueID001
 * @tc.desc: test GetUniqueID
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeImageTest, NativeImageTest_GetUniqueID001, TestSize.Level1)
{
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    EXPECT_NE(image, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_ErrorCode code;
    uint32_t cUnique = 0;
    code = OH_Drawing_ImageGetUniqueID(nullptr, &cUnique);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    code = OH_Drawing_ImageGetUniqueID(image, nullptr);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    code = OH_Drawing_ImageGetUniqueID(image, &cUnique);
    EXPECT_TRUE(cUnique == 0);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    code = OH_Drawing_ImageGetUniqueID(image, &cUnique);
    EXPECT_TRUE(cUnique > 0);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
}

/*
 * @tc.name: NativeImageTest_IsOpaque001
 * @tc.desc: test IsOpaque
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeImageTest, NativeImageTest_IsOpaque001, TestSize.Level1)
{
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    EXPECT_NE(image, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    EXPECT_EQ(OH_Drawing_ImageIsOpaque(nullptr),false);
    EXPECT_EQ(OH_Drawing_ImageIsOpaque(image),false);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    EXPECT_EQ(OH_Drawing_ImageIsOpaque(image),true);

    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
}

/*
 * @tc.name: NativeImageTest_ScalePixels001
 * @tc.desc: test ScalePixels
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeImageTest, NativeImageTest_ScalePixels001, TestSize.Level1)
{
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    EXPECT_NE(image, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST, OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);
    bool isScaled;
    OH_Drawing_ErrorCode code;
    code = OH_Drawing_ImageScalePixels(nullptr, bitmap, samplingOptions, true, &isScaled);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    code = OH_Drawing_ImageScalePixels(image, bitmap, samplingOptions, true, &isScaled);
    EXPECT_EQ(isScaled, false);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    code = OH_Drawing_ImageScalePixels(image, bitmap, samplingOptions, true, &isScaled);
    EXPECT_EQ(isScaled, true);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    code = OH_Drawing_ImageScalePixels(image, bitmap, samplingOptions, false, &isScaled);
    EXPECT_EQ(isScaled, true);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS