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

#include "drawing_bitmap.h"
#include "drawing_types.h"
#include "drawing_rect.h"
#include "drawing_color_space.h"

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
 * @tc.name: NativeDrawingBitmapTest_bitmapBuild001
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapBuild001, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_ALPHA_8, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapBuild002
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapBuild002, TestSize.Level1)
{
    const unsigned int width = 0;
    const unsigned int height = 0;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGB_565, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapBuild003
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapBuild003, TestSize.Level1)
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
 * @tc.name: NativeDrawingBitmapTest_bitmapBuild004
 * @tc.desc: test for drawing_bitmap build.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapBuild004, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap_));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap_));
}

/*
 * @tc.name: NativeDrawingBitmapTest_bitmapCreateFromPixels005
 * @tc.desc: test for OH_Drawing_BitmapCreateFromPixels.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapCreateFromPixels005, TestSize.Level1)
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
 * @tc.name: NativeDrawingBitmapTest_bitmapGetImageInfo006
 * @tc.desc: test for drawing_bitmapGetImageInfo.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_bitmapGetImageInfo006, TestSize.Level1)
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
 * @tc.name: NativeDrawingBitmapTest_BitmapReadPixels007
 * @tc.desc: test for drawing_BitmapReadPixels.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_BitmapReadPixels007, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_BitmapFormat bitmapFormat {COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL};
    OH_Drawing_BitmapBuild(bitmap_, width, height, &bitmapFormat);
    OH_Drawing_Image_Info imageInfo {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL};
    void* pixels = new uint32_t[width * height];
    bool res = OH_Drawing_BitmapReadPixels(nullptr, nullptr, nullptr, width * 4, 0, 0);
    EXPECT_EQ(res, false);
    res = OH_Drawing_BitmapReadPixels(nullptr, &imageInfo, pixels, width * 4, 0, 0);
    EXPECT_EQ(res, false);
    res = OH_Drawing_BitmapReadPixels(bitmap_, nullptr, pixels, width * 4, 0, 0);
    EXPECT_EQ(res, false);
    res = OH_Drawing_BitmapReadPixels(bitmap_, &imageInfo, nullptr, width * 4, 0, 0);
    EXPECT_EQ(res, false);
    res = OH_Drawing_BitmapReadPixels(bitmap_, &imageInfo, pixels, width * 4, 0, 0);
    EXPECT_EQ(res, true);
    if (pixels != nullptr) {
        delete[] reinterpret_cast<uint32_t*>(pixels);
        pixels = nullptr;
    }
}

/*
 * @tc.name: NativeDrawingBitmapTest_GetColorFormat008
 * @tc.desc: test for drawing_BitmapGetColorFormat.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_GetColorFormat008, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_ColorFormat formats[] = {
        COLOR_FORMAT_UNKNOWN,
        COLOR_FORMAT_ALPHA_8,
        COLOR_FORMAT_RGB_565,
        COLOR_FORMAT_ARGB_4444,
        COLOR_FORMAT_RGBA_8888,
        COLOR_FORMAT_BGRA_8888
    };

    OH_Drawing_AlphaFormat alphaFormats[] = {
        ALPHA_FORMAT_UNKNOWN,
        ALPHA_FORMAT_OPAQUE,
        ALPHA_FORMAT_PREMUL,
        ALPHA_FORMAT_UNPREMUL
    };
    OH_Drawing_ColorFormat colorFormat_;
    for (int i = 1; i < 6; i++) {
        OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
        OH_Drawing_BitmapFormat bitmapFormat = {formats[i], alphaFormats[2]};
        OH_Drawing_BitmapBuild(bitmap, width, height, &bitmapFormat);
        if (bitmap == nullptr) {
            colorFormat_ = OH_Drawing_BitmapGetColorFormat(bitmap);
            EXPECT_EQ(colorFormat_, formats[0]);
        }
        colorFormat_ = OH_Drawing_BitmapGetColorFormat(bitmap);
        EXPECT_EQ(colorFormat_, formats[i]);
    }
}

/*
 * @tc.name: NativeDrawingBitmapTest_GetAlphaFormat009
 * @tc.desc: test for drawing_BitmapGetAlphaFormat.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_GetAlphaFormat009, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    OH_Drawing_ColorFormat formats[] = {
        COLOR_FORMAT_UNKNOWN,
        COLOR_FORMAT_ALPHA_8,
        COLOR_FORMAT_RGB_565,
        COLOR_FORMAT_ARGB_4444,
        COLOR_FORMAT_RGBA_8888,
        COLOR_FORMAT_BGRA_8888
    };

    OH_Drawing_AlphaFormat alphaFormats[] = {
        ALPHA_FORMAT_UNKNOWN,
        ALPHA_FORMAT_OPAQUE,
        ALPHA_FORMAT_PREMUL,
        ALPHA_FORMAT_UNPREMUL
    };
    OH_Drawing_AlphaFormat alphaFormat_;
    for (int i = 1; i < 4; i++) {
        OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
        OH_Drawing_BitmapFormat bitmapFormat = {formats[3], alphaFormats[i]};
        OH_Drawing_BitmapBuild(bitmap, width, height, &bitmapFormat);
        if (bitmap == nullptr) {
            alphaFormat_ = OH_Drawing_BitmapGetAlphaFormat(bitmap);
            EXPECT_EQ(alphaFormat_, alphaFormats[0]);
        }
        alphaFormat_ = OH_Drawing_BitmapGetAlphaFormat(bitmap);
        EXPECT_EQ(alphaFormat_, alphaFormats[i]);
    }
}

/*
 * @tc.name: NativeDrawingBitmapTest_CreateFromImageInfo014
 * @tc.desc: test for drawing_BitmapCreateFromImageInfo.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_CreateFromImageInfo014, TestSize.Level1)
{
    const unsigned int width = 500;
    const unsigned int height = 500;
    const unsigned int rowBytes = width * height * 4;
    OH_Drawing_ColorFormat colorFormat = COLOR_FORMAT_RGBA_8888;
    OH_Drawing_AlphaFormat alphaFormat = ALPHA_FORMAT_PREMUL;
    OH_Drawing_Image_Info imageInfo { width, height, colorFormat, alphaFormat };
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    EXPECT_NE(colorSpace, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, rowBytes);
    EXPECT_NE(bitmap, nullptr);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(bitmap));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(bitmap));
    EXPECT_EQ(colorFormat, OH_Drawing_BitmapGetColorFormat(bitmap));
    EXPECT_EQ(alphaFormat, OH_Drawing_BitmapGetAlphaFormat(bitmap));
    uint32_t byte;
    OH_Drawing_BitmapGetRowBytes(bitmap, &byte);
    EXPECT_EQ(rowBytes, byte);
    OH_Drawing_ColorSpaceType type;
    OH_Drawing_BitmapGetColorSpaceType(nullptr, &type);
    EXPECT_EQ(OH_Drawing_ColorSpaceType::NO_TYPE, type);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

/*
 * @tc.name: NativeDrawingBitmapTest_GetRowBytes015
 * @tc.desc: test for drawing_BitmapGetRowBytes.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_GetRowBytes015, TestSize.Level1)
{
    const unsigned int width = 600;
    const unsigned int height = 600;
    const unsigned int rowBytes = width * height * 4;
    OH_Drawing_ColorFormat format = COLOR_FORMAT_RGBA_8888;
    OH_Drawing_AlphaFormat alphaFormat = ALPHA_FORMAT_PREMUL;
    OH_Drawing_Image_Info imageInfo { width, height, format, alphaFormat };
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    EXPECT_NE(colorSpace, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, rowBytes);
    uint32_t byte;
    EXPECT_NE(bitmap, nullptr);
    EXPECT_EQ(OH_DRAWING_ERROR_INVALID_PARAMETER, OH_Drawing_BitmapGetRowBytes(nullptr, &byte));
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetRowBytes(bitmap, &byte));
    EXPECT_EQ(byte, rowBytes);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

/*
 * @tc.name: NativeDrawingBitmapTest_GetColorSpaceType016
 * @tc.desc: test for drawing_BitmapGetColorSpaceType.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_GetColorSpaceType016, TestSize.Level1)
{
    const unsigned int width = 600;
    const unsigned int height = 600;
    const unsigned int rowBytes = width * height * 4;
    OH_Drawing_ColorFormat format = COLOR_FORMAT_RGBA_8888;
    OH_Drawing_AlphaFormat alphaFormat = ALPHA_FORMAT_PREMUL;
    OH_Drawing_Image_Info imageInfo { width, height, format, alphaFormat };
    OH_Drawing_ColorSpace* colorSpaceSrgb = OH_Drawing_ColorSpaceCreateSrgb();
    EXPECT_NE(colorSpaceSrgb, nullptr);
    OH_Drawing_ColorSpace* colorSpaceSrgbLinear = OH_Drawing_ColorSpaceCreateSrgbLinear();
    EXPECT_NE(colorSpaceSrgbLinear, nullptr);
    OH_Drawing_Bitmap* bitmapSrgb = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpaceSrgb, rowBytes);
    EXPECT_NE(bitmapSrgb, nullptr);
    OH_Drawing_Bitmap* bitmapSrgbLinear = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpaceSrgbLinear, rowBytes);
    EXPECT_NE(bitmapSrgbLinear, nullptr);
    OH_Drawing_ColorSpaceType type;
    EXPECT_EQ(OH_DRAWING_ERROR_INVALID_PARAMETER, OH_Drawing_BitmapGetColorSpaceType(nullptr, &type));
    EXPECT_EQ(type, OH_Drawing_ColorSpaceType::NO_TYPE);
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetColorSpaceType(bitmapSrgb, &type));
    EXPECT_EQ(type, OH_Drawing_ColorSpaceType::SRGB);
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapGetColorSpaceType(bitmapSrgbLinear, &type));
    EXPECT_EQ(type, OH_Drawing_ColorSpaceType::SRGB_LINEAR);
    OH_Drawing_BitmapDestroy(bitmapSrgb);
    OH_Drawing_BitmapDestroy(bitmapSrgbLinear);
    OH_Drawing_ColorSpaceDestroy(colorSpaceSrgb);
    OH_Drawing_ColorSpaceDestroy(colorSpaceSrgbLinear);
}

/*
 * @tc.name: NativeDrawingBitmapTest_ExtractSubset017
 * @tc.desc: test for drawing_BitmapExtractSubset.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_ExtractSubset017, TestSize.Level1)
{
    const unsigned int width = 600;
    const unsigned int height = 600;
    const unsigned int rowBytes = width * height * 4;
    OH_Drawing_ColorFormat format = COLOR_FORMAT_RGBA_8888;
    OH_Drawing_AlphaFormat alphaFormat = ALPHA_FORMAT_PREMUL;
    OH_Drawing_Image_Info imageInfo { width, height, format, alphaFormat };
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    EXPECT_NE(colorSpace, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, rowBytes);
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_Bitmap* dstBitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(dstBitmap, nullptr);

    const unsigned int left = 100;
    const unsigned int top = 200;
    const unsigned int right = 500;
    const unsigned int bottom = 300;
    const unsigned int dstWidth = right - left;
    const unsigned int dstHeight = bottom - top;
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
    EXPECT_NE(rect, nullptr);
    bool retrieved;
    EXPECT_EQ(OH_DRAWING_ERROR_INVALID_PARAMETER, OH_Drawing_BitmapExtractSubset(nullptr, dstBitmap, rect, &retrieved));
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapExtractSubset(bitmap, dstBitmap, rect, &retrieved));
    EXPECT_EQ(retrieved, true);
    EXPECT_EQ(dstWidth, OH_Drawing_BitmapGetWidth(dstBitmap));
    EXPECT_EQ(dstHeight, OH_Drawing_BitmapGetHeight(dstBitmap));
    uint32_t byte;
    OH_Drawing_BitmapGetRowBytes(bitmap, &byte);
    EXPECT_EQ(rowBytes, byte);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_BitmapDestroy(dstBitmap);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

/*
 * @tc.name: NativeDrawingBitmapTest_IsAndSetImmutable018
 * @tc.desc: test for drawing_BitmapIsImmutable and drawing_BitmapSetImmutable.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingBitmapTest, NativeDrawingBitmapTest_IsAndSetImmutable018, TestSize.Level1)
{
    const unsigned int width = 200;
    const unsigned int height = 200;
    const unsigned int rowBytes = width * height * 4;
    OH_Drawing_ColorFormat format = COLOR_FORMAT_RGBA_8888;
    OH_Drawing_AlphaFormat alphaFormat = ALPHA_FORMAT_PREMUL;
    OH_Drawing_Image_Info imageInfo { width, height, format, alphaFormat };
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    EXPECT_NE(colorSpace, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromImageInfo(&imageInfo, colorSpace, rowBytes);
    bool immutable;
    EXPECT_EQ(OH_DRAWING_ERROR_INVALID_PARAMETER, OH_Drawing_BitmapIsImmutable(nullptr, &immutable));
    EXPECT_EQ(immutable, false);
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapIsImmutable(bitmap, &immutable));
    EXPECT_EQ(immutable, false);
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapSetImmutable(bitmap));
    EXPECT_EQ(OH_DRAWING_SUCCESS, OH_Drawing_BitmapIsImmutable(bitmap, &immutable));
    EXPECT_EQ(immutable, true);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
