/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <include/core/SkData.h>
#include <include/core/SkDrawable.h>
#include <include/core/SkMatrix.h>
#include <include/core/SkPath.h>
#include <include/core/SkPicture.h>
#include <include/core/SkRRect.h>
#include <include/core/SkRect.h>
#include <include/core/SkRegion.h>
#include <include/core/SkTextBlob.h>
#include <src/core/SkDrawShadowInfo.h>
#include <test_header.h>

#include "platform/ohos/overdraw/rs_canvas_listener.h"
#include "platform/ohos/overdraw/rs_listened_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSListenedCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSListenedCanvasTest::SetUpTestCase() {}
void RSListenedCanvasTest::TearDownTestCase() {}
void RSListenedCanvasTest::SetUp() {}
void RSListenedCanvasTest::TearDown() {}

class MockDrawingCanvas : public Drawing::Canvas {
public:
    MOCK_METHOD1(DrawRegion, void(const Drawing::Region& region));
};

class MockRSPaintFilterCanvas : public RSPaintFilterCanvas {
public:
    explicit MockRSPaintFilterCanvas(Drawing::Canvas* canvas) : RSPaintFilterCanvas(canvas) {}
    MOCK_METHOD1(DrawRect, void(const Drawing::Rect& rect));
    MOCK_METHOD1(DrawRoundRect, void(const Drawing::RoundRect& roundRect));
    MOCK_METHOD2(DrawNestedRoundRect, void(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner));
    MOCK_METHOD3(DrawPie, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD2(DrawCircle, void(const Drawing::Point& centerPt, Drawing::scalar radius));
    MOCK_METHOD1(DrawBackground, void(const Drawing::Brush& brush));
    MOCK_METHOD3(DrawTextBlob, void(const Drawing::TextBlob* blob,
        const Drawing::scalar x, const Drawing::scalar y));
    MOCK_METHOD3(DrawBitmap, void(const Drawing::Bitmap& bitmap,
        const Drawing::scalar px, const Drawing::scalar py));
    MOCK_METHOD4(DrawImage, void(const Drawing::Image& image, const Drawing::scalar px,
        const Drawing::scalar py, const Drawing::SamplingOptions& sampling));
    MOCK_METHOD5(DrawImageRect, void(const Drawing::Image& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint));
    MOCK_METHOD3(DrawImageRect, void(const Drawing::Image& image, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling));
    MOCK_METHOD1(DrawPicture, void(const Drawing::Picture& picture));
    MOCK_METHOD1(Clear, void(const Drawing::ColorQuad color));
};

class MockRSCanvasListener : public RSCanvasListener {
public:
    explicit MockRSCanvasListener(Drawing::Canvas& canvas) : RSCanvasListener(canvas) {}
    MOCK_METHOD1(DrawRect, void(const Drawing::Rect& rect));
    MOCK_METHOD1(DrawRoundRect, void(const Drawing::RoundRect& roundRect));
    MOCK_METHOD2(DrawNestedRoundRect, void(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner));
    MOCK_METHOD3(DrawPie, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD2(DrawCircle, void(const Drawing::Point& centerPt, Drawing::scalar radius));
    MOCK_METHOD1(DrawBackground, void(const Drawing::Brush& brush));
    MOCK_METHOD3(DrawTextBlob, void(const Drawing::TextBlob* blob,
        const Drawing::scalar x, const Drawing::scalar y));
    MOCK_METHOD3(DrawBitmap, void(const Drawing::Bitmap& bitmap,
        const Drawing::scalar px, const Drawing::scalar py));
    MOCK_METHOD4(DrawImage, void(const Drawing::Image& image, const Drawing::scalar px,
        const Drawing::scalar py, const Drawing::SamplingOptions& sampling));
    MOCK_METHOD5(DrawImageRect, void(const Drawing::Image& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint));
    MOCK_METHOD3(DrawImageRect, void(const Drawing::Image& image, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling));
    MOCK_METHOD1(DrawPicture, void(const Drawing::Picture& picture));
    MOCK_METHOD1(Clear, void(const Drawing::ColorQuad color));
};

/*
 * Function: request will pass through listened canvas
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. create mock MockRSPaintFilterCanvas
 *                  2. expect MockRSPaintFilterCanvas call drawRect once
 *                  3. create RSListenedCanvas from MockRSPaintFilterCanvas
 *                  4. call RSListenedCanvas's drawRect
 */
HWTEST_F(RSListenedCanvasTest, RequestPassThrough, Function | SmallTest | Level2)
{
    PART("CaseDescription")
    {
        auto mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
        std::shared_ptr<MockRSPaintFilterCanvas> mockRSPaintFilterCanvas = nullptr;
        STEP("1. create mock MockRSPaintFilterCanvas")
        {
            mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
        }

        Drawing::Rect rect = Drawing::Rect(1, 2, 4, 6);
        STEP("2. expect MockRSPaintFilterCanvas call drawRect once")
        {
            EXPECT_CALL(*mockRSPaintFilterCanvas, DrawRect(rect)).Times(1);
        }

        std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;
        STEP("3. create RSListenedCanvas from MockRSPaintFilterCanvas")
        {
            listenedCanvas = std::make_shared<RSListenedCanvas>(*mockRSPaintFilterCanvas);
        }

        STEP("4. call RSListenedCanvas's drawRect")
        {
            listenedCanvas->DrawRect(rect);
        }
    }
}

/*
 * Function: request will split to listener
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. create mock MockRSPaintFilterCanvas
 *                  2. create mock MockRSCanvasListener
 *                  3. expect MockRSCanvasListener call drawRect once
 *                  4. create RSListenedCanvas from MockRSPaintFilterCanvas
 *                  5. call RSListenedCanvas's drawRect
 */
HWTEST_F(RSListenedCanvasTest, RequestSplitToListener, Function | SmallTest | Level2)
{
    auto mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
    std::shared_ptr<MockRSPaintFilterCanvas> mockRSPaintFilterCanvas = nullptr;
    mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
    std::shared_ptr<MockRSCanvasListener> mockRSCanvasListener = nullptr;
    mockRSCanvasListener = std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);
    std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;
    listenedCanvas = std::make_shared<RSListenedCanvas>(*mockRSPaintFilterCanvas);
    listenedCanvas->SetListener(mockRSCanvasListener);

    Drawing::Rect rect = Drawing::Rect(1, 2, 4, 6);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawRect(rect)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawRect(rect)).Times(1);

    listenedCanvas->DrawRect(rect);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawRoundRect(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawRoundRect(_)).Times(1);

    listenedCanvas->DrawRoundRect(Drawing::RoundRect());

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawNestedRoundRect(_, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawNestedRoundRect(_, _)).Times(1);

    listenedCanvas->DrawNestedRoundRect(Drawing::RoundRect(), Drawing::RoundRect());

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawPie(_, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawPie(_, _, _)).Times(1);

    listenedCanvas->DrawPie(Drawing::Rect(), 0.0f, 90.0f);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawCircle(_, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawCircle(_, _)).Times(1);

    listenedCanvas->DrawCircle(Drawing::Point(), 0.0f);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawBackground(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawBackground(_)).Times(1);

    listenedCanvas->DrawBackground(Drawing::Brush());

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawBitmap(_, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawBitmap(_, _, _)).Times(1);

    listenedCanvas->DrawBitmap(Drawing::Bitmap(), 0.0f, 0.0f);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawImage(_, _, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawImage(_, _, _, _)).Times(1);

    listenedCanvas->DrawImage(Drawing::Image(), 0.0f, 0.0f, Drawing::SamplingOptions());

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawImageRect(_, _, _, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawImageRect(_, _, _, _, _)).Times(1);

    listenedCanvas->DrawImageRect(Drawing::Image(), Drawing::Rect(), Drawing::Rect(),
        Drawing::SamplingOptions(), Drawing::SrcRectConstraint());

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawImageRect(_, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawImageRect(_, _, _)).Times(1);

    listenedCanvas->DrawImageRect(Drawing::Image(), Drawing::Rect(), Drawing::SamplingOptions());

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawPicture(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawPicture(_)).Times(1);

    listenedCanvas->DrawPicture(Drawing::Picture());

    EXPECT_CALL(*mockRSPaintFilterCanvas, Clear(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, Clear(_)).Times(1);

    listenedCanvas->Clear(Drawing::ColorQuad());
}

/**
 * @tc.name: onDrawOval001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawOval001, TestSize.Level1)
{
    MockDrawingCanvas canvas;
    Drawing::Rect rect;
    RSListenedCanvas listenedCanvas(canvas);
    auto listener = std::make_shared<MockRSCanvasListener>(canvas);
    listenedCanvas.DrawOval(rect);
    listenedCanvas.SetListener(listener);
    listenedCanvas.DrawOval(rect);
}

/**
 * @tc.name: onDrawArc001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawArc001, TestSize.Level1)
{
    MockDrawingCanvas canvas;
    Drawing::Rect rect;
    Drawing::scalar startAngle = 0.1;
    Drawing::scalar sweepAngle = 0.2;
    RSListenedCanvas listenedCanvas(canvas);
    auto listener = std::make_shared<MockRSCanvasListener>(canvas);
    listenedCanvas.DrawArc(rect, startAngle, sweepAngle);
    listenedCanvas.SetListener(listener);
    listenedCanvas.DrawArc(rect, startAngle, sweepAngle);
}

/**
 * @tc.name: onDrawPath001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawPath001, TestSize.Level1)
{
    MockDrawingCanvas canvas;
    Drawing::Path path;
    RSListenedCanvas listenedCanvas(canvas);
    auto listener = std::make_shared<MockRSCanvasListener>(canvas);
    listenedCanvas.DrawPath(path);
    listenedCanvas.SetListener(listener);
    listenedCanvas.DrawPath(path);
}

/**
 * @tc.name: onDrawRegion001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawRegion001, TestSize.Level1)
{
    MockDrawingCanvas canvas;
    Drawing::Region region;
    RSListenedCanvas listenedCanvas(canvas);
    auto listener = std::make_shared<MockRSCanvasListener>(canvas);
    listenedCanvas.DrawRegion(region);
    listenedCanvas.SetListener(listener);
    listenedCanvas.DrawRegion(region);
}

/**
 * @tc.name: onDrawPoints001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawPoints001, TestSize.Level1)
{
    MockDrawingCanvas canvas;
    Drawing::scalar x = 0.1;
    Drawing::scalar y = 0.2;
    auto point = Rosen::Drawing::Point(x, y);
    RSListenedCanvas listenedCanvas(canvas);
    listenedCanvas.DrawPoint(point);
    auto listener = std::make_shared<MockRSCanvasListener>(canvas);
    listenedCanvas.SetListener(listener);
    listenedCanvas.DrawPoint(point);
}
} // namespace Rosen
} // namespace OHOS
