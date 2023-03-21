/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "sk_image_chain.h"
#include "sk_image_filter_factory.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Rosen {
class SKImageChainUnittest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: CreateSKImageChainTest001
 * @tc.desc: Ensure the ability of creating color picker from pixelmap.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, CreateSKImageChainTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest CreateSKImageChainTest001 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: CreateSKImageChainTest002
 * @tc.desc: Ensure the ability of creating color picker from pixelmap.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, CreateSKImageChainTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest CreateSKImageChainTest002 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    std::shared_ptr<Media::PixelMap> srcPixelMap = nullptr;
    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    auto imageChain = std::make_shared<SKImageChain>(srcPixelMap);
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: CreateSKImageChainTest003
 * @tc.desc: Ensure the ability of creating color picker from SkCanvas and SkImage.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, CreateSKImageChainTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest CreateSKImageChainTest002 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    SkCanvas* canvas = nullptr;
    sk_sp<SkImage> image = nullptr;
    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    auto imageChain = std::make_shared<SKImageChain>(canvas, image);
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: ForceCPUTest001
 * @tc.desc: Set whether to render with CPU.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, ForceCPUTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest CreateSKImageChainTest001 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    imageChain->ForceCPU(false);
    imageChain->ForceCPU(false);
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: ForceCPUTest002
 * @tc.desc: Set whether to render with CPU.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, ForceCPUTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest ForceCPUTest002 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    imageChain->ForceCPU(false);
    imageChain->ForceCPU(true);
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: ForceCPUTest003
 * @tc.desc: Set whether to render with CPU.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, ForceCPUTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest ForceCPUTest003 start";
    /**
     * @tc.steps: step1. Create a SkCanvas
     */
    SkCanvas canvas;
    /**
     * @tc.steps: step2. Call create From SkCanvas and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(&canvas, nullptr);
    imageChain->ForceCPU(true);
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: ForceCPUTest004
 * @tc.desc: Set whether to render with CPU.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, ForceCPUTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest ForceCPUTest004 start";
    /**
     * @tc.steps: step1. Create a SkCanvas
     */
    SkCanvas canvas;
    /**
     * @tc.steps: step2. Call create From SkCanvas and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(&canvas, nullptr);
    imageChain->ForceCPU(false);
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: SetFiltersTest001
 * @tc.desc: Set filters
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, SetFiltersTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest SetFiltersTest001 start";
    /**
     * @tc.steps: step1. Create a SkCanvas
     */
    SkCanvas canvas;
    /**
     * @tc.steps: step2. Call create From SkCanvas and call SetFilters
     */
    auto imageChain = std::make_shared<SKImageChain>(&canvas, nullptr);
    imageChain->SetFilters(nullptr);
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: SetFiltersTest002
 * @tc.desc: Set filters
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, SetFiltersTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest SetFiltersTest002 start";
    /**
     * @tc.steps: step1. Create a SkCanvas
     */
    SkCanvas canvas;
    /**
     * @tc.steps: step2. Call create From SkCanvas and call SetFilters
     */
    auto imageChain = std::make_shared<SKImageChain>(&canvas, nullptr);
    auto filter1 = SKImageFilterFactory::Blur(5.f);
    imageChain->SetFilters(filter1);
    auto filter2 = SKImageFilterFactory::Brightness(0.5f);
    imageChain->SetFilters(filter2);
    auto filter3 = SKImageFilterFactory::Grayscale();
    imageChain->SetFilters(filter3);
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: GetPixelMapTest001
 * @tc.desc: Get PixelMap
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, GetPixelMapTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest GetPixelMapTest001 start";
    /**
     * @tc.steps: step1. Create a SkCanvas
     */
    SkCanvas canvas;
    /**
     * @tc.steps: step2. Call create From SkCanvas and call GetPixelMap
     */
    auto imageChain = std::make_shared<SKImageChain>(&canvas, nullptr);
    auto pixelMap = imageChain->GetPixelMap();
    EXPECT_TRUE(imageChain != nullptr);
    EXPECT_TRUE(pixelMap == nullptr);
}

/**
 * @tc.name: GetPixelMapTest002
 * @tc.desc: Get PixelMap
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, GetPixelMapTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest GetPixelMapTest002 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    imageChain->Draw();
    auto pixelMap = imageChain->GetPixelMap();
    EXPECT_TRUE(imageChain != nullptr);
    EXPECT_TRUE(pixelMap != nullptr);
}

/**
 * @tc.name: DrawTest001
 * @tc.desc: Draw the picture source
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest001 start";
    /**
     * @tc.steps: step1. Create a imageChain from nullptr
     */
    auto imageChain = std::make_shared<SKImageChain>(nullptr);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */

    imageChain->Draw();
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: DrawTest002
 * @tc.desc: Draw the picture source
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest002 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    imageChain->Draw();
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: DrawTest003
 * @tc.desc: Draw the picture source
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest003 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    imageChain->ForceCPU(false);
    imageChain->Draw();
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: DrawTest004
 * @tc.desc: Draw the picture source
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest004 start";
    /**
     * @tc.steps: step1. Create a SkCanvas
     */
    SkCanvas canvas;
    /**
     * @tc.steps: step2. Call create From SkCanvas and call SetFilters
     */
    auto imageChain = std::make_shared<SKImageChain>(&canvas, nullptr);
    imageChain->Draw();
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: DrawTest005
 * @tc.desc: Draw the picture source
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest005 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.pixelFormat = PixelFormat::BGRA_8888;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    auto rect = SkRect::MakeEmpty();
    imageChain->SetClipRect(&rect);
    imageChain->Draw();
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: DrawTest006
 * @tc.desc: Draw the picture source
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest006 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.pixelFormat = PixelFormat::RGBA_8888;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    SkPath path;
    imageChain->SetClipPath(&path);
    imageChain->Draw();
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: DrawTest007
 * @tc.desc: Draw the picture source
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest007 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.pixelFormat = PixelFormat::RGB_565;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    SkRRect rRect;
    imageChain->SetClipRRect(&rRect);
    imageChain->Draw();
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: DrawTest008
 * @tc.desc: Draw the picture source
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest008 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.pixelFormat = PixelFormat::ALPHA_8;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    imageChain->Draw();
    EXPECT_TRUE(imageChain != nullptr);
}

/**
 * @tc.name: DrawTest009
 * @tc.desc: Draw the picture source
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest009 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    /**
     * @tc.steps: step2. Call create From pixelMap and call ForceCPU
     */
    auto imageChain = std::make_shared<SKImageChain>(std::move(pixmap));
    imageChain->Draw();
    EXPECT_TRUE(imageChain != nullptr);
}
} // namespace Rosen
} // namespace OHOS
