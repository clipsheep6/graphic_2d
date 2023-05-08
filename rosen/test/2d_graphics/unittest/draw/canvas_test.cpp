/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "draw/canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class CanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CanvasTest::SetUpTestCase() {}
void CanvasTest::TearDownTestCase() {}
void CanvasTest::SetUp() {}
void CanvasTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CreateAndDestroy001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    EXPECT_TRUE(nullptr != canvas);
}

/**
 * @tc.name: CanvasBindTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasBindTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Bitmap bitmap;
    canvas->Bind(bitmap);
}

/**
 * @tc.name: CanvasGetTotalMatrixTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasGetTotalMatrixTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    auto matrix = std::make_unique<Matrix>(canvas->GetTotalMatrix());
    EXPECT_TRUE(nullptr != matrix);
}

/**
 * @tc.name: CanvasGetLocalClipBoundsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasGetLocalClipBoundsTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    auto rect = std::make_unique<Rect>(canvas->GetLocalClipBounds());
    EXPECT_TRUE(nullptr != rect);
}

/**
 * @tc.name: CanvasGetDeviceClipBoundsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasGetDeviceClipBoundsTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    auto rect = std::make_unique<RectI>(canvas->GetDeviceClipBounds());
    EXPECT_TRUE(nullptr != rect);
}

/**
 * @tc.name: CanvasGetWidthTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasGetWidthTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    auto rect = canvas->GetWidth();
    EXPECT_EQ(rect, 0);
}

/**
 * @tc.name: CanvasGetHeightTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasGetHeightTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    auto rect = canvas->GetHeight();
    EXPECT_EQ(rect, 0);
}

/**
 * @tc.name: CanvasDrawPointTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawPointTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Point point(10.0f, 20.0f);
    canvas->DrawPoint(point);
}

/**
 * @tc.name: CanvasDrawLineTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawLineTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Point startPoint(10.0f, 20.0f);
    Point endPoint(30.0f, 20.0f);
    canvas->DrawLine(startPoint, endPoint);
}

/**
 * @tc.name: CanvasDrawRectTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawRectTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    canvas->DrawRect(rect);
}

/**
 * @tc.name: CanvasDrawRoundRectTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawRoundRectTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    RoundRect roundRect(rect, 1.0f, 1.0f);
    canvas->DrawRoundRect(roundRect);
}

/**
 * @tc.name: CanvasDrawNestedRoundRectTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawNestedRoundRectTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Rect rect1(0.0f, 0.0f, 10.0f, 20.0f);
    RoundRect roundRect1(rect1, 1.0f, 1.0f);
    Rect rect2(0.0f, 0.0f, 5.0f, 10.0f);
    RoundRect roundRect2(rect2, 1.0f, 1.0f);
    canvas->DrawNestedRoundRect(roundRect1, roundRect2);
}

/**
 * @tc.name: CanvasDrawArcTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawArcTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    canvas->DrawArc(rect, 0.0f, 90.0f);
}

/**
 * @tc.name: CanvasDrawPieTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawPieTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    canvas->DrawPie(rect, 0.0f, 90.0f);
}

/**
 * @tc.name: CanvasDrawOvalTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawOvalTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    canvas->DrawOval(rect);
}

/**
 * @tc.name: CanvasDrawCircleTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawCircleTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Point centerpoint(10.0f, 20.0f);
    canvas->DrawCircle(centerpoint, 10.0f);
}

/**
 * @tc.name: CanvasDrawPathTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawPathTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Path path;
    canvas->DrawPath(path);
}

/**
 * @tc.name: CanvasDrawBackgroundTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawBackgroundTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Brush brush(Color::COLOR_RED);
    canvas->DrawBackground(brush);
}

/**
 * @tc.name: CanvasDrawShadowTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CanvasDrawShadowTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(nullptr != canvas);
    Path path;
    Point3 planeParams(1.0f, 0.0f, 0.0f);
    Point3 devLightPos(1.0f, 1.0f, 1.0f);
    canvas->DrawShadow(path, planeParams, devLightPos, 1.0f, 
                        Color::COLOR_BLACK, Color::COLOR_BLUE, ShadowFlags::NONE);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
