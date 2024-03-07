/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "c/drawing_bitmap.h"
#include "c/drawing_types.h"
#include "image/pixmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingBitmapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
protected:
    OH_Drawing_Bitmap* bitmap_ = nullptr;
};

void NativeDrawingBitmapTest::SetUpTestCase() {}
void NativeDrawingBitmapTest::TearDownTestCase() {}
void NativeDrawingBitmapTest::SetUp()
{
    bitmap_ = OH_Drawing_BitmapCreate();
    ASSERT_NE(bitmap_, nullptr);
}

void NativeDrawingBitmapTest::TearDown()
{
    if (bitmap_ != nullptr) {
        OH_Drawing_BitmapDestroy(bitmap_);
        bitmap_ = nullptr;
    }
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmap002
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmap002, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_ALPHA_8, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmap003
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmap003, TestSize.Level1)
{
    const unsigned int width = 0;
    const unsigned int height = 0;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGB_565, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmap004
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmap004, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_ARGB_4444, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
    EXPECT_EQ(OH_Drawing_BitmapGetPixels(bitmap_) == nullptr, false);
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmap005
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmap005, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmap006
 * @tc.desc: test for OH_Drawing_BitmapCreateFromPixels.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmap006, TestSize.Level1)
{
    OH_Drawing_Image_Info imageInfo;
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    void* pixels = OH_Drawing_BitmapGetPixels(bitmap);
    EXPECT_NE(pixels, nullptr);
    uint32_t rowBytes = width * height * 4;
    bitmap_ = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    EXPECT_NE(bitmap_, nullptr);
    bitmap_ = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, 0);
    EXPECT_EQ(bitmap_, nullptr);
    bitmap_ = OH_Drawing_BitmapCreateFromPixels(&imageInfo, nullptr, 0);
    EXPECT_EQ(bitmap_, nullptr);
    bitmap_ = OH_Drawing_BitmapCreateFromPixels(nullptr, nullptr, 0);
    EXPECT_EQ(bitmap_, nullptr);
}

/*
 * @tc.name: NativeDrawingBitmapTest_GetImageInfo001
 * @tc.desc: test for drawing_bitmapGetImageInfo.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_GetImageInfo001, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    OH_Drawing_Image_Info* imageInfo = new OH_Drawing_Image_Info();
    OH_Drawing_BitmapGetImageInfo(bitmap_, imageInfo);
    EXPECT_EQ(width, imageInfo->width);
    EXPECT_EQ(height, imageInfo->height);
}

/*
 * @tc.name: NativeDrawingBitmapTest_InstallPixels001
 * @tc.desc: test for drawing_bitmapInstallPixels.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_InstallPixels001, TestSize.Level1)
{
    const unsigned int width = 8;
    const unsigned int height = 8;
    OH_Drawing_Image_Info imageInfo {width, height, COLOR_FORMAT_ALPHA_8, ALPHA_FORMAT_PREMUL};
    void* pixels = new uint32_t[8 * 8];
    bool res = OH_Drawing_BitmapInstallPixels(bitmap_, &imageInfo, pixels, 16, nullptr, nullptr);
    EXPECT_EQ(res, true);
}

/*
 * @tc.name: NativeDrawingBitmapTest_PeekPixels001
 * @tc.desc: test for drawing_BitmapPeekPixels.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_PeekPixels001, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    Pixmap pixmap;
    OH_Drawing_Pixmap* p = (OH_Drawing_Pixmap*)&pixmap;
    bool res = OH_Drawing_BitmapPeekPixels(bitmap_, p);
    EXPECT_EQ(res, true);
}

/*
 * @tc.name: NativeDrawingBitmapTest_TryAllocPixels001
 * @tc.desc: test for drawing_BitmapTryAllocPixels.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_TryAllocPixels001, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    OH_Drawing_Image_Info imageInfo {300, 400, COLOR_FORMAT_ARGB_4444, ALPHA_FORMAT_PREMUL};
    bool res = OH_Drawing_BitmapTryAllocPixels(bitmap_, &imageInfo);
    EXPECT_EQ(res, true);
}

/*
 * @tc.name: NativeDrawingBitmapTest_ReadPixels001
 * @tc.desc: test for drawing_BitmapReadPixels.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_ReadPixels001, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    OH_Drawing_Image_Info imageInfo {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL};
    void* pixels = new uint32_t[width * height];
    bool res = OH_Drawing_BitmapReadPixels(bitmap_, &imageInfo, pixels, width * 4, 0, 0);
    EXPECT_EQ(res, true);
}

/*
 * @tc.name: NativeDrawingBitmapTest_ComputeByteSize001
 * @tc.desc: test for drawing_BitmapComputeByteSize.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_ComputeByteSize001, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    size_t size = 0;
    size = OH_Drawing_BitmapComputeByteSize(bitmap_);
    EXPECT_EQ(size > 0, true);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
