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

#include "render/rs_pixel_map_util.h"
#include "pixel_map.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RsPixelMapUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsPixelMapUtilTest::SetUpTestCase() {}
void RsPixelMapUtilTest::TearDownTestCase() {}

void RsPixelMapUtilTest::SetUp() {}
void RsPixelMapUtilTest::TearDown() {}

HWTEST_F(RsPixelMapUtilTest, ExtractDrawingImageTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest ExtractDrawingImageTest001 start";

    std::shared_ptr<Media::PixelMap> pixelMap;
    EXPECT_EQ(RSPixelMapUtil::ExtractDrawingImage(pixelMap), nullptr);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest ExtractDrawingImageTest001 end";
}

HWTEST_F(RsPixelMapUtilTest, ExtractDrawingImageTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest ExtractDrawingImageTest001 start";

    for (auto format : {
        UNKNOWN = 0,
        ARGB_8888 = 1,  // Each pixel is stored on 4 bytes.
        RGB_565 = 2,    // Each pixel is stored on 2 bytes
        RGBA_8888 = 3,
        BGRA_8888 = 4,
        RGB_888 = 5,
        ALPHA_8 = 6,
        RGBA_F16 = 7,
        NV21 = 8,  // Each pixel is sorted on 3/2 bytes.
        NV12 = 9,
        CMYK = 10,
        RGBA_1010102 = 14,
    }) {
        auto pixelMap = std::make_shared<Media::PixelMap>();
        pixelMap->imageInfo_.size = { 10, 10 };
        pixelMap->pixelFormat = format;
        auto imgPtr = RSPixelMapUtil::ExtractDrawingImage(pixelMap);

        EXPECT_EQ(pixelMap->imageInfo_.width, imgPtr->GetWidth());
        EXPECT_EQ(pixelMap->imageInfo_.height, imgPtr->GetHeight());
    }

    GTEST_LOG_(INFO) << "RSAnimationFractionTest ExtractDrawingImageTest001 end";
}

HWTEST_F(RsPixelMapUtilTest, TransformDataSetForAstc001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest TransformDataSetForAstc001 start";

    // auto pixelMap = std::make_shared<Media::PixelMap>();
    // Drawing::Rect src;
    // Drawing::Rect dst;
    // Drawing::Canvas canvas;

    // RSPixelMapUtil::TransformDataSetForAstc();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest TransformDataSetForAstc001 end";
}

HWTEST_F(RsPixelMapUtilTest, DrawPixelMap001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest DrawPixelMap001 start";

    // RSPixelMapUtil::DrawPixelMap();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest DrawPixelMap001 end";
}

HWTEST_F(RsPixelMapUtilTest, IsYUVFormat001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest IsYUVFormat001 start";

    // RSPixelMapUtil::IsYUVFormat();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest IsYUVFormat001 end";
}

HWTEST_F(RsPixelMapUtilTest, IsSupportZeroCopy001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest IsSupportZeroCopy001 start";

    // RSPixelMapUtil::IsSupportZeroCopy();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest IsSupportZeroCopy001 end";
}

HWTEST_F(RsPixelMapUtilTest, ConvertYUVPixelMapToDrawingImage001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest ConvertYUVPixelMapToDrawingImage001 start";

    // RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest ConvertYUVPixelMapToDrawingImage001 end";
}
} // Rosen
} // OHOS

