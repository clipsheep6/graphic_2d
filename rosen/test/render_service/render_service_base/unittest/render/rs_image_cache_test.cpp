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

#include "render/rs_image_cache.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImageCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImageCacheTest::SetUpTestCase() {}
void RSImageCacheTest::TearDownTestCase() {}
void RSImageCacheTest::SetUp() {}
void RSImageCacheTest::TearDown() {}

/**
 * @tc.name: InstanceTest001
 * @tc.desc: Verify function Instance
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, InstanceTest001, TestSize.Level1)
{
    RSImageCache::Instance();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: CacheDrawingImageTest001
 * @tc.desc: Verify function CacheDrawingImage
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, CacheDrawingImageTest001, TestSize.Level1)
{
    auto imageCache = std::make_shared<RSImageCache>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache->CacheDrawingImage(1, img);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetDrawingImageCacheTest001
 * @tc.desc: Verify function GetDrawingImageCache
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, GetDrawingImageCacheTest001, TestSize.Level1)
{
    auto imageCache = std::make_shared<RSImageCache>();
    EXPECT_EQ(imageCache->GetDrawingImageCache(0), nullptr);
    auto img = std::make_shared<Drawing::Image>();
    imageCache->CacheDrawingImage(1, img);
    EXPECT_EQ(imageCache->GetDrawingImageCache(0), nullptr);
}

/**
 * @tc.name: IncreaseDrawingImageCacheRefCountTest001
 * @tc.desc: Verify function IncreaseDrawingImageCacheRefCount
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, IncreaseDrawingImageCacheRefCountTest001, TestSize.Level1)
{
    auto imageCache = std::make_shared<RSImageCache>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache->CacheDrawingImage(1, img);
    imageCache->IncreaseDrawingImageCacheRefCount(0);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ReleaseDrawingImageCacheTest001
 * @tc.desc: Verify function ReleaseDrawingImageCache
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, ReleaseDrawingImageCacheTest001, TestSize.Level1)
{
    auto imageCache = std::make_shared<RSImageCache>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache->CacheDrawingImage(1, img);
    imageCache->ReleaseDrawingImageCache(0);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetPixelMapCacheTest001
 * @tc.desc: Verify function GetPixelMapCache
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, GetPixelMapCacheTest001, TestSize.Level1)
{
    auto imageCache = std::make_shared<RSImageCache>();
    EXPECT_EQ(imageCache->GetPixelMapCache(0), nullptr);
    auto img = std::make_shared<Drawing::Image>();
    imageCache->CacheDrawingImage(1, img);
    EXPECT_EQ(imageCache->GetPixelMapCache(0), nullptr);
}

/**
 * @tc.name: IncreasePixelMapCacheRefCountTest001
 * @tc.desc: Verify function IncreasePixelMapCacheRefCount
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, IncreasePixelMapCacheRefCountTest001, TestSize.Level1)
{
    auto imageCache = std::make_shared<RSImageCache>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache->CacheDrawingImage(1, img);
    imageCache->IncreasePixelMapCacheRefCount(0);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ReleasePixelMapCacheTest001
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, ReleasePixelMapCacheTest001, TestSize.Level1)
{
    auto imageCache = std::make_shared<RSImageCache>();
    imageCache->ReleasePixelMapCache(0);
    EXPECT_TRUE(true);
    auto img = std::make_shared<Drawing::Image>();
    imageCache->CacheDrawingImage(1, img);
    imageCache->ReleasePixelMapCache(0);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: CacheRenderDrawingImageByPixelMapIdTest001
 * @tc.desc: Verify function CacheRenderDrawingImageByPixelMapId
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, CacheRenderDrawingImageByPixelMapIdTest001, TestSize.Level1)
{
    auto imageCache = std::make_shared<RSImageCache>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache->CacheRenderDrawingImageByPixelMapId(0, img, 0);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetRenderDrawingImageCacheByPixelMapIdTest001
 * @tc.desc: Verify function GetRenderDrawingImageCacheByPixelMapId
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, GetRenderDrawingImageCacheByPixelMapIdTest001, TestSize.Level1)
{
    auto imageCache = std::make_shared<RSImageCache>();
    EXPECT_EQ(imageCache->GetRenderDrawingImageCacheByPixelMapId(0, 0), nullptr);
    auto img = std::make_shared<Drawing::Image>();
    imageCache->CacheRenderDrawingImageByPixelMapId(0, img, 0);
    EXPECT_EQ(imageCache->GetRenderDrawingImageCacheByPixelMapId(0, 0), nullptr);
}

/**
 * @tc.name: ReleaseDrawingImageCacheByPixelMapIdTest001
 * @tc.desc: Verify function ReleaseDrawingImageCacheByPixelMapId
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, ReleaseDrawingImageCacheByPixelMapIdTest001, TestSize.Level1)
{
    auto imageCache = std::make_shared<RSImageCache>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache->CacheRenderDrawingImageByPixelMapId(0, img, 0);
    imageCache->ReleaseDrawingImageCacheByPixelMapId(0);
    EXPECT_TRUE(true);
}

} // namespace OHOS::Rosen