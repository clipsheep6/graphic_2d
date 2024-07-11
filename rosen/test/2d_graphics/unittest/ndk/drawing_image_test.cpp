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
#include "drawing_error_code.h"
#include "drawing_image.h"

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
 * @tc.name: NativeImageTest_IsOpaqueo001
 * @tc.desc: test IsOpaque
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeImageTest, NativeImageTest_IsOpaque001, TestSize.Level1)
{
    OH_Drawing_Image *image = OH_Drawing_ImageCreate();
    EXPECT_NE(image, nullptr);
    OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    const int32_t width = 256;
    const int32_t height = 256;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    bool isOpaque;
    OH_Drawing_ErrorCode code = OH_Drawing_ImageIsOpaque(nullptr, &isOpaque);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    code = OH_Drawing_ImageIsOpaque(image, &isOpaque);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    EXPECT_EQ(isOpaque, true);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeImageTest_CreateFromRaster001
 * @tc.desc: test CreateFromRaster
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeImageTest, NativeImageTest_CreateFromRaster001, TestSize.Level1)
{
    OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    const int32_t width = 256;
    const int32_t height = 256;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_Pixmap *pixmap = OH_Drawing_PixmapCreate();
    bool isset;
    OH_Drawing_BitmapPeekPixels(bitmap, pixmap, &isset);
    auto releaseProc = [](const OH_Drawing_Pixmap *pixels, void *releaseContext) -> void {};
    OH_Drawing_Image *image = OH_Drawing_ImageCreateFromRaster(nullptr, releaseProc, nullptr);
    EXPECT_EQ(image, nullptr);
    image = OH_Drawing_ImageCreateFromRaster(pixmap, releaseProc, nullptr);
    EXPECT_NE(image, nullptr);
    OH_Drawing_PixmapDestroy(pixmap);
    OH_Drawing_BitmapDestroy(bitmap);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS