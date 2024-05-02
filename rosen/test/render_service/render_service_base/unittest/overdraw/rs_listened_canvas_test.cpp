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
    MOCK_METHOD1(DrawPoint, void(const Drawing::Point& point));
    MOCK_METHOD1(DrawRect, void(const Drawing::Rect& rect));
    MOCK_METHOD2(DrawLine, void(const Drawing::Point& startPt, const Drawing::Point& endPt));
    MOCK_METHOD1(DrawRoundRect, void(const Drawing::RoundRect& roundRect)); 
    MOCK_METHOD2(DrawNestedRoundRect, void(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner));
    MOCK_METHOD3(DrawArc, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD3(DrawPie, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD1(DrawPath, void(const Drawing::Path& path));
};

class MockRSCanvasListener : public RSCanvasListener {
public:
    explicit MockRSCanvasListener(Drawing::Canvas& canvas) : RSCanvasListener(canvas) {}
    MOCK_METHOD1(DrawPoint, void(const Drawing::Point& point));
    MOCK_METHOD1(DrawRect, void(const Drawing::Rect& rect));
    MOCK_METHOD2(DrawLine, void(const Drawing::Point& startPt, const Drawing::Point& endPt));
    MOCK_METHOD1(DrawRoundRect, void(const Drawing::RoundRect& roundRect)); 
    MOCK_METHOD2(DrawNestedRoundRect, void(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner)); 
    MOCK_METHOD3(DrawArc, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD3(DrawPie, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
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

HWTEST_F(RSListenedCanvasTest, RequestSplitToListener, Function | SmallTest | Level2)
{
    PART("CaseDescription")
    {
        auto mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
        std::shared_ptr<MockRSPaintFilterCanvas> mockRSPaintFilterCanvas = nullptr;
        STEP("1. create mock MockRSPaintFilterCanvas")
        {
            mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
        }

        std::shared_ptr<MockRSCanvasListener> mockRSCanvasListener = nullptr;
        STEP("2. create mock MockRSCanvasListener")
        {
            mockRSCanvasListener = std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);
        }

        // Drawing objects for testing
        Drawing::Point point(10, 10);
        Drawing::Rect rect(1, 2, 4, 6);
        Drawing::Point startPt(0, 0);
        Drawing::Point endPt(100, 100);

        // Expectations for DrawPoint
        STEP("3. expect MockRSCanvasListener call drawPoint once")
        {
            EXPECT_CALL(*mockRSPaintFilterCanvas, DrawPoint(point)).Times(1);
            EXPECT_CALL(*mockRSCanvasListener, DrawPoint(point)).Times(1);
        }

        // Expectations for DrawRect
        STEP("4. expect MockRSCanvasListener call drawRect once")
        {
            EXPECT_CALL(*mockRSPaintFilterCanvas, DrawRect(rect)).Times(1);
            EXPECT_CALL(*mockRSCanvasListener, DrawRect(rect)).Times(1);
        }

        // Expectations for DrawLine
        STEP("5. expect MockRSCanvasListener call drawLine once")
        {
            EXPECT_CALL(*mockRSPaintFilterCanvas, DrawLine(startPt, endPt)).Times(1);
            EXPECT_CALL(*mockRSCanvasListener, DrawLine(startPt, endPt)).Times(1);
        }

        std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;
        STEP("6. create RSListenedCanvas from MockRSPaintFilterCanvas")
        {
            listenedCanvas = std::make_shared<RSListenedCanvas>(*mockRSPaintFilterCanvas);
            listenedCanvas->SetListener(mockRSCanvasListener);
        }

        // Calls to Draw methods
        STEP("7. call RSListenedCanvas's drawPoint, drawRect, and drawLine")
        {
            listenedCanvas->DrawPoint(point);
            listenedCanvas->DrawRect(rect);
            listenedCanvas->DrawLine(startPt, endPt);
        }
    }
}




// /*
//  * Function: request will split to listener
//  * Type: Function
//  * EnvConditions: N/A
//  * CaseDescription: 1. create mock MockRSPaintFilterCanvas
//  *                  2. create mock MockRSCanvasListener
//  *                  3. expect MockRSCanvasListener call drawRect once
//  *                  4. create RSListenedCanvas from MockRSPaintFilterCanvas
//  *                  5. call RSListenedCanvas's drawRect
//  */


HWTEST_F(RSListenedCanvasTest, RequestSplitToListener_DrawRoundRect, Function | SmallTest | Level2)
{
    auto mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
    auto mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
    auto mockRSCanvasListener = std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);

    // 期望底层画布上的 DrawRoundRect 方法被调用一次
    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawRoundRect(_)).Times(1);

    // 期望监听器上的 DrawRoundRect 方法被调用一次
    EXPECT_CALL(*mockRSCanvasListener, DrawRoundRect(_)).Times(1);

    RSListenedCanvas listener(*mockRSPaintFilterCanvas);
    listener.SetListener(mockRSCanvasListener);

    // 调用 DrawRoundRect 方法
    listener.DrawPie(Drawing::Rect(0, 0, 100, 100), 0.0f, 90.0f);
    //listener.DrawRoundRect(Drawing::RoundRect());
}


HWTEST_F(RSListenedCanvasTest, RequestSplitToListener_DrawNestedRoundRect, Function | SmallTest | Level2)
{
    auto mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
    auto mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
    auto mockRSCanvasListener = std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);

    // 期望底层画布上的 DrawNestedRoundRect 方法被调用一次
    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawNestedRoundRect(_, _)).Times(1);

    // 期望监听器上的 DrawNestedRoundRect 方法被调用一次
    EXPECT_CALL(*mockRSCanvasListener, DrawNestedRoundRect(_, _)).Times(1);

    RSListenedCanvas listener(*mockRSPaintFilterCanvas);
    listener.SetListener(mockRSCanvasListener);

    // 调用 DrawNestedRoundRect 方法
    listener.DrawNestedRoundRect(Drawing::RoundRect(), Drawing::RoundRect());
}

HWTEST_F(RSListenedCanvasTest, RequestSplitToListener_DrawArc, Function | SmallTest | Level2)
{
    auto mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
    auto mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
    auto mockRSCanvasListener = std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);

    // 期望底层画布上的 DrawArc 方法被调用一次
    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawArc(_, _, _)).Times(1);

    // 期望监听器上的 DrawArc 方法被调用一次
    EXPECT_CALL(*mockRSCanvasListener, DrawArc(_, _, _)).Times(1);

    RSListenedCanvas listener(*mockRSPaintFilterCanvas);
    listener.SetListener(mockRSCanvasListener);

    // 调用 DrawArc 方法
    listener.DrawArc(Drawing::Rect(0, 0, 100, 100), 0.0f, 90.0f);
}

HWTEST_F(RSListenedCanvasTest, RequestSplitToListener_DrawPie, Function | SmallTest | Level2)
{
    auto mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
    auto mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
    auto mockRSCanvasListener = std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);

    // 期望底层画布上的 DrawPie 方法被调用一次
    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawPie(_, _, _)).Times(1);

    // 期望监听器上的 DrawPie 方法被调用一次
    EXPECT_CALL(*mockRSCanvasListener, DrawPie(_, _, _)).Times(1);

    RSListenedCanvas listener(*mockRSPaintFilterCanvas);
    listener.SetListener(mockRSCanvasListener);

    // 调用 DrawPie 方法
    listener.DrawPie(Drawing::Rect(0, 0, 100, 100), 0.0f, 90.0f);
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
} // namespace Rosen
} // namespace OHOS
