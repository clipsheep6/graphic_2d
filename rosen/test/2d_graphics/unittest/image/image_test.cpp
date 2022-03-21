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
#include "image/image.h"
#include "image/bitmap.h"
#include "image/picture.h"
#include "utils/matrix.h"
#include "utils/size.h"
#include "draw/brush.h"
#include "effect/color_space.h"
#include "engine_adapter/impl_factory.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ImageTest::SetUpTestCase() {}
void ImageTest::TearDownTestCase() {}
void ImageTest::SetUp() {}
void ImageTest::TearDown() {}

/**
 * @tc.name: CreateAndDestory001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageTest, CreateAndDestory001, TestSize.Level1)
{
    // The best way to create Image.
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
}

/**
 * @tc.name: BuildFromBitmap001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageTest, BuildFromBitmap001, TestSize.Level1)
{
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
    Bitmap bitmap;
    image->BuildFromBitmap(bitmap);
}

/**
 * @tc.name: GetWidth001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageTest, GetWidth001, TestSize.Level1)
{
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
    ASSERT_EQ(0, image->GetWidth());
}

/**
 * @tc.name: GetHeight001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageTest, GetHeight001, TestSize.Level1)
{
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
    ASSERT_EQ(0, image->GetHeight());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS