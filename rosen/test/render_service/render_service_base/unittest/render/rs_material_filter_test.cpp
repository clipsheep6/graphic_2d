/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "render/rs_material_filter.h"
#include "include/core/SkSurface.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMaterialFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaterialFilterTest::SetUpTestCase() {}
void RSMaterialFilterTest::TearDownTestCase() {}
void RSMaterialFilterTest::SetUp() {}
void RSMaterialFilterTest::TearDown() {}

/**
 * @tc.name: CreateMaterialStyle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyle001, TestSize.Level1)
{
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode);
}

/**
 * @tc.name: PreProcess001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, PreProcess001, TestSize.Level1)
{
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    sk_sp<SkImage> imageSnapshot = nullptr;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode);
    rsMaterialFilter.PreProcess(imageSnapshot);
}

/**
 * @tc.name: PreProcess002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, PreProcess002, TestSize.Level1)
{
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    sk_sp<SkImage> imageSnapshot = nullptr;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode);
    rsMaterialFilter.PreProcess(imageSnapshot);
}

static sk_sp<SkImage> CreateSkImage()
{
    const SkImageInfo info = SkImageInfo::MakeN32(200, 200, kOpaque_SkAlphaType);
    auto surface(SkSurface::MakeRaster(info));
    auto canvas = surface->getCanvas();
    canvas->clear(SK_ColorYELLOW);
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    canvas->drawRect(SkRect::MakeXYWH(50, 50, 100, 100), paint);
    return surface->makeImageSnapshot();
}

/**
 * @tc.name: PreProcess003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, PreProcess003, TestSize.Level1)
{
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    sk_sp<SkImage> imageSnapshot = CreateSkImage();
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode);
    rsMaterialFilter.PreProcess(imageSnapshot);
}

/**
 * @tc.name: PreProcess004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, PreProcess004, TestSize.Level1)
{
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    sk_sp<SkImage> imageSnapshot = CreateSkImage();
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode);
    rsMaterialFilter.PreProcess(imageSnapshot);
}

/**
 * @tc.name: PostProcess001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, PostProcess001, TestSize.Level1)
{
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    const SkImageInfo info = SkImageInfo::MakeN32(200, 200, kOpaque_SkAlphaType);
    auto surface(SkSurface::MakeRaster(info));
    auto canvas = surface->getCanvas();
    canvas->clear(SK_ColorYELLOW);
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    canvas->drawRect(SkRect::MakeXYWH(50, 50, 100, 100), paint);
    sk_sp<SkImage> imageSnapshot = surface->makeImageSnapshot();
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode);
    rsMaterialFilter.PreProcess(imageSnapshot);
    RSPaintFilterCanvas rsPaintFilterCanvas(canvas);
    rsMaterialFilter.PostProcess(rsPaintFilterCanvas);
}

/**
 * @tc.name: Add001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, Add001, TestSize.Level1)
{
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    std::shared_ptr<RSMaterialFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(style, dipScale, mode);
    std::shared_ptr<RSFilter> rhs = std::make_shared<RSMaterialFilter>(style, dipScale, mode);
    rsMaterialFilter->Add(rhs);
}

/**
 * @tc.name: Sub001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, Sub001, TestSize.Level1)
{
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    std::shared_ptr<RSMaterialFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(style, dipScale, mode);
    std::shared_ptr<RSFilter> rhs = std::make_shared<RSMaterialFilter>(style, dipScale, mode);
    rsMaterialFilter->Sub(rhs);
}

/**
 * @tc.name: Multiply001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, Multiply001, TestSize.Level1)
{
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    std::shared_ptr<RSMaterialFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(style, dipScale, mode);
    float rhs = 1.0;
    rsMaterialFilter->Multiply(rhs);
}

/**
 * @tc.name: Negate001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, Negate001, TestSize.Level1)
{
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    std::shared_ptr<RSMaterialFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(style, dipScale, mode);
    rsMaterialFilter->Negate();
}
}   // namespace OHOS::Rosen