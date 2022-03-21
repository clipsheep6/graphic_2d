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
#include "image/bitmap.h"
#include "draw/color.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class BitmapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BitmapTest::SetUpTestCase() {}
void BitmapTest::TearDownTestCase() {}
void BitmapTest::SetUp() {}
void BitmapTest::TearDown() {}

/**
 * @tc.name: ImageBuildTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageBuildTest001, TestSize.Level1)
{
    // The best way to Build Bitmap.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat;
    bitmap->Build(100, 200, bitmapFormat);
}

/**
 * @tc.name: ImageBuildTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageBuildTest002, TestSize.Level1)
{
    // The best way to Build Bitmap.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat;
    bitmap->Build(150, 99, bitmapFormat);
}

/**
 * @tc.name: ImageBuildTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageBuildTest003, TestSize.Level1)
{
    // The best way to Build Bitmap.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat;
    bitmap->Build(111, 450, bitmapFormat);
}

/**
 * @tc.name: ImageGetWidthTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageGetWidthTest001, TestSize.Level1)
{
    // The best way to get width.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat;
    bitmap->Build(111, 450, bitmapFormat);
    ASSERT_EQ(111, bitmap->GetWidth());
}

/**
 * @tc.name: ImageGetWidthTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageGetWidthTest002, TestSize.Level1)
{
    // The best way to get width.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat;
    bitmap->Build(151, 150, bitmapFormat);
    ASSERT_EQ(151, bitmap->GetWidth());
}

/**
 * @tc.name: ImageGetHeightTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageGetHeightTest001, TestSize.Level1)
{
    // The best way to get height.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat;
    bitmap->Build(111, 450, bitmapFormat);
    ASSERT_EQ(450, bitmap->GetHeight());
}

/**
 * @tc.name: ImageGetHeightTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageGetHeightTest002, TestSize.Level1)
{
    // The best way to get height.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat;
    bitmap->Build(151, 150, bitmapFormat);
    ASSERT_EQ(150, bitmap->GetHeight());
}

/**
 * @tc.name: ImageCreateAndDestory001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageCreateAndDestory001, TestSize.Level1)
{
    // The best way to create Bitmap.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
}

/**
 * @tc.name: ImageSetAndPixelsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageSetAndPixelsTest001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat *bitmapFormat1= nullptr;
    bitmap->SetPixels(bitmapFormat1);
    EXPECT_EQ(bitmapFormat1, bitmap->GetPixels());
}

/**
 * @tc.name: ImageSetAndPixelsTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageSetAndPixelsTest002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat * bitmapFormat2= nullptr;
    bitmap->SetPixels(bitmapFormat2);
    EXPECT_EQ(bitmapFormat2, bitmap->GetPixels());
}

/**
 * @tc.name: ImageCopyPixelsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageCopyPixelsTest001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    Bitmap bitmap1;
    bitmap->CopyPixels(bitmap1, 100, 105, 201, 845);
}

/**
 * @tc.name: ImageCopyPixelsTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageCopyPixelsTest002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    Bitmap bitmap1;
    bitmap->CopyPixels(bitmap1, 66, 5, 99, 320);
}

/**
 * @tc.name: ImageClearWithColor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageClearWithColor001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    bitmap->ClearWithColor(COLORTYPE_UNKNOWN);
}

/**
 * @tc.name: ImageClearWithColor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageClearWithColor002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    bitmap->ClearWithColor(COLORTYPE_ALPHA_8);
}

/**
 * @tc.name: ImageIsValid001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageIsValid001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ASSERT_TRUE(bitmap->IsValid());
}

/**
 * @tc.name: ImageIsValid002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageIsValid002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ASSERT_FALSE(! bitmap->IsValid());
}

/**
 * @tc.name: ImageImageGetColorTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageImageGetColorTest001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ASSERT_EQ(Color::COLOR_TRANSPARENT, bitmap->GetColor(0, 0));
}

/**
 * @tc.name: ImageGetColorTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageGetColorTest002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ASSERT_EQ(Color::COLOR_TRANSPARENT, bitmap->GetColor(1, 2));
}

/**
 * @tc.name: ImageGetFormatTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageGetFormatTest001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat;
    bitmap->Build(111, 450, bitmapFormat);
    ASSERT_EQ(ColorType::COLORTYPE_UNKNOWN, bitmap->GetFormat().colorType);
    ASSERT_EQ(AlphaType::ALPHATYPE_UNKNOWN, bitmap->GetFormat().alphaType);
    bitmap->Free();
}

/**
 * @tc.name: ImageGetFormatTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ImageGetFormatTest002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat;
    bitmap->Build(151, 150, bitmapFormat);
    ASSERT_EQ(ColorType::COLORTYPE_UNKNOWN, bitmap->GetFormat().colorType);
    ASSERT_EQ(AlphaType::ALPHATYPE_UNKNOWN, bitmap->GetFormat().alphaType);
    bitmap->Free();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS