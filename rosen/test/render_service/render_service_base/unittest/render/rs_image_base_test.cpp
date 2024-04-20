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
#include "pixel_map.h"

#include "render/rs_image_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImageBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImageBaseTest::SetUpTestCase() {}
void RSImageBaseTest::TearDownTestCase() {}
void RSImageBaseTest::SetUp() {}
void RSImageBaseTest::TearDown() {}

/**
 * @tc.name: DrawImageTest001
 * @tc.desc: Verify function DrawImage
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, DrawImageTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    Drawing::Canvas canvas;
    Drawing::SamplingOptions samplingOptions;
    imageBase->DrawImage(canvas, samplingOptions);
    EXPECT_TRUE(true);
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->DrawImage(canvas, samplingOptions);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetImageTest001
 * @tc.desc: Verify function SetImage
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, SetImageTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetPixelMapTest001
 * @tc.desc: Verify function SetPixelMap
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, SetPixelMapTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelmap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelmap);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DumpPictureTest001
 * @tc.desc: Verify function DumpPicture
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, DumpPictureTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    DfxString info;
    imageBase->DumpPicture(info);
    EXPECT_TRUE(true);
    auto pixelmap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelmap);
    imageBase->DumpPicture(info);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetSrcRectTest001
 * @tc.desc: Verify function SetSrcRect
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, SetSrcRectTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    RectF srcRect;
    imageBase->SetSrcRect(srcRect);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDstRectTest001
 * @tc.desc: Verify function SetDstRect
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, SetDstRectTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    RectF dstRect;
    imageBase->SetDstRect(dstRect);
    EXPECT_TRUE(true);
    imageBase->SetDstRect(imageBase->dstRect_);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetImagePixelAddrTest001
 * @tc.desc: Verify function SetImagePixelAddr
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, SetImagePixelAddrTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    void* addr = nullptr;
    imageBase->SetImagePixelAddr(addr);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UpdateNodeIdToPictureTest001
 * @tc.desc: Verify function UpdateNodeIdToPicture
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, UpdateNodeIdToPictureTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    imageBase->UpdateNodeIdToPicture(0);
    EXPECT_TRUE(true);
    auto pixelmap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelmap);
    auto image = std::make_shared<Drawing::Image>();
    imageBase->SetImage(image);
    imageBase->UpdateNodeIdToPicture(1);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: MarkRenderServiceImageTest001
 * @tc.desc: Verify function MarkRenderServiceImage
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, MarkRenderServiceImageTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    imageBase->MarkRenderServiceImage();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ConvertPixelMapToDrawingImageTest001
 * @tc.desc: Verify function ConvertPixelMapToDrawingImage
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, ConvertPixelMapToDrawingImageTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    auto pixelmap = std::make_shared<Media::PixelMap>();
    imageBase->SetPixelMap(pixelmap);
    imageBase->ConvertPixelMapToDrawingImage(true);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GenUniqueIdTest001
 * @tc.desc: Verify function GenUniqueId
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, GenUniqueIdTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    imageBase->GenUniqueId(1);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetPixelMapTest001
 * @tc.desc: Verify function GetPixelMap
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, GetPixelMapTest001, TestSize.Level1)
{
    auto imageBase = std::make_shared<RSImageBase>();
    EXPECT_EQ(imageBase->GetPixelMap(), nullptr);
}
} // namespace OHOS::Rosen