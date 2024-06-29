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

    auto pixelMap = std::make_shared<Media::PixelMap>();

    RSPixelMapUtil::ExtractDrawingImage();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest ExtractDrawingImageTest001 end";
}

HWTEST_F(RsPixelMapUtilTest, TransformDataSetForAstc001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest TransformDataSetForAstc001 start";

    RSPixelMapUtil::TransformDataSetForAstc();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest TransformDataSetForAstc001 end";
}

HWTEST_F(RsPixelMapUtilTest, DrawPixelMap001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest DrawPixelMap001 start";

    RSPixelMapUtil::DrawPixelMap();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest DrawPixelMap001 end";
}

HWTEST_F(RsPixelMapUtilTest, IsYUVFormat001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest IsYUVFormat001 start";

    RSPixelMapUtil::IsYUVFormat();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest IsYUVFormat001 end";
}

HWTEST_F(RsPixelMapUtilTest, IsSupportZeroCopy001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest IsSupportZeroCopy001 start";

    RSPixelMapUtil::IsSupportZeroCopy();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest IsSupportZeroCopy001 end";
}

HWTEST_F(RsPixelMapUtilTest, ConvertYUVPixelMapToDrawingImage001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest ConvertYUVPixelMapToDrawingImage001 start";

    RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage();

    GTEST_LOG_(INFO) << "RSAnimationFractionTest ConvertYUVPixelMapToDrawingImage001 end";
}
} // Rosen
} // OHOS

