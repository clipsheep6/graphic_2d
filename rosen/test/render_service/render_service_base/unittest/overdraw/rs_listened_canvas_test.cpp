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


class MockDrawingCanvas : public Drawing::Canvas {
public:
    MOCK_METHOD1(DrawRegion, void(const Drawing::Region& region));
};

class MockRSPaintFilterCanvas : public RSPaintFilterCanvas {
public:
    explicit MockRSPaintFilterCanvas(Drawing::Canvas* canvas) : RSPaintFilterCanvas(canvas) {}
    MOCK_METHOD1(DrawPath, void(const Drawing::Path& path));
    MOCK_METHOD1(DrawPoint, void(const Drawing::Point& point));
    MOCK_METHOD1(DrawRect, void(const Drawing::Rect& rect));
    MOCK_METHOD2(DrawLine, void(const Drawing::Point& startPt, const Drawing::Point& endPt));
    MOCK_METHOD1(DrawRoundRect, void(const Drawing::RoundRect& roundRect));
    MOCK_METHOD2(DrawNestedRoundRect, void(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner));
    MOCK_METHOD3(DrawArc, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD3(DrawPie, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD1(DrawOval, void(const Drawing::Rect& rect));
    MOCK_METHOD2(DrawCircle, void(const Drawing::Point& centerPt, Drawing::scalar radius));
    MOCK_METHOD1(DrawBackground, void(const Drawing::Brush& brush));
    MOCK_METHOD1(DrawRegion, void(const Drawing::Region& region));
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
    MOCK_METHOD1(DrawPoint, void(const Drawing::Point& point));
    MOCK_METHOD1(DrawRect, void(const Drawing::Rect& rect));
    MOCK_METHOD2(DrawLine, void(const Drawing::Point& startPt, const Drawing::Point& endPt));
    MOCK_METHOD1(DrawRoundRect, void(const Drawing::RoundRect& roundRect));
    MOCK_METHOD2(DrawNestedRoundRect, void(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner));
    MOCK_METHOD3(DrawArc, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD3(DrawPie, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD1(DrawOval, void(const Drawing::Rect& rect));
    MOCK_METHOD2(DrawCircle, void(const Drawing::Point& centerPt, Drawing::scalar radius));
    MOCK_METHOD1(DrawPath, void(const Drawing::Path& path));
    MOCK_METHOD1(DrawBackground, void(const Drawing::Brush& brush));
    MOCK_METHOD1(DrawRegion, void(const Drawing::Region& region));
    MOCK_METHOD3(DrawBitmap, void(const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py));
    MOCK_METHOD4(DrawImage, void(const Drawing::Image& image, const Drawing::scalar px,
        const Drawing::scalar py, const Drawing::SamplingOptions& sampling));
    MOCK_METHOD5(DrawImageRect, void(const Drawing::Image& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint));
    MOCK_METHOD3(DrawImageRect, void(const Drawing::Image& image, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling));
    MOCK_METHOD1(DrawPicture, void(const Drawing::Picture& picture));
    MOCK_METHOD1(Clear, void(const Drawing::ColorQuad color));
};
class RSListenedCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    std::unique_ptr<MockDrawingCanvas> mockDrawingCanvas;
    std::shared_ptr<MockRSPaintFilterCanvas> mockRSPaintFilterCanvas;
    std::shared_ptr<MockRSCanvasListener> mockRSCanvasListener;
    std::shared_ptr<RSListenedCanvas> listenedCanvas;
};

void RSListenedCanvasTest::SetUpTestCase() {}
void RSListenedCanvasTest::TearDownTestCase() {}
void RSListenedCanvasTest::SetUp()
{
    mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
    mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
    mockRSCanvasListener = std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);
    listenedCanvas = std::make_shared<RSListenedCanvas>(*mockRSPaintFilterCanvas);
    listenedCanvas->SetListener(mockRSCanvasListener);
}
void RSListenedCanvasTest::TearDown()
{
    mockDrawingCanvas.reset();
    mockRSPaintFilterCanvas.reset();
    mockRSCanvasListener.reset();
    listenedCanvas.reset();
}

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

void setExpectations(std::shared_ptr<MockRSPaintFilterCanvas>& mockRSPaintFilterCanvas,
                     std::shared_ptr<MockRSCanvasListener>& mockRSCanvasListener)
{
    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawPoint(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawPoint(_)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawRect(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawRect(_)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawLine(_, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawLine(_, _)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawPie(_, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawPie(_, _, _)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawRoundRect(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawRoundRect(_)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawArc(_, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawArc(_, _, _)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawNestedRoundRect(_, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawNestedRoundRect(_, _)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawOval(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawOval(_)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawCircle(_, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawCircle(_, _)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawPath(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawPath(_)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawBackground(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawBackground(_)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawRegion(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawRegion(_)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawBitmap(_, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawBitmap(_, _, _)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawImage(_, _, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawImage(_, _, _, _)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawImageRect(_, _, _, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawImageRect(_, _, _, _, _)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawImageRect(_, _, _)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawImageRect(_, _, _)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, DrawPicture(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, DrawPicture(_)).Times(1);

    EXPECT_CALL(*mockRSPaintFilterCanvas, Clear(_)).Times(1);
    EXPECT_CALL(*mockRSCanvasListener, Clear(_)).Times(1);
}

void callDrawMethods(std::shared_ptr<RSListenedCanvas>& listenedCanvas)
{
    listenedCanvas->DrawPoint(Drawing::Point());
    listenedCanvas->DrawRect(Drawing::Rect());
    listenedCanvas->DrawLine(Drawing::Point(), Drawing::Point());
    listenedCanvas->DrawPie(Drawing::Rect(), 0.0f, 90.0f);
    listenedCanvas->DrawArc(Drawing::Rect(), 0.0f, 90.0f);
    listenedCanvas->DrawRoundRect(Drawing::RoundRect());
    listenedCanvas->DrawNestedRoundRect(Drawing::RoundRect(), Drawing::RoundRect());
    listenedCanvas->DrawOval(Drawing::Rect());
    listenedCanvas->DrawCircle(Drawing::Point(), 0.0f);
    listenedCanvas->DrawPath(Drawing::Path());
    listenedCanvas->DrawBackground(Drawing::Brush());
    listenedCanvas->DrawRegion(Drawing::Region());
    listenedCanvas->DrawBitmap(Drawing::Bitmap(), 0.0f, 0.0f);
    listenedCanvas->DrawImage(Drawing::Image(), 0.0f, 0.0f, Drawing::SamplingOptions());
    listenedCanvas->DrawImageRect(Drawing::Image(), Drawing::Rect(), Drawing::Rect(),
        Drawing::SamplingOptions(), Drawing::SrcRectConstraint());
    listenedCanvas->DrawImageRect(Drawing::Image(), Drawing::Rect(), Drawing::SamplingOptions());
    listenedCanvas->DrawPicture(Drawing::Picture());
    listenedCanvas->Clear(Drawing::ColorQuad());
}

HWTEST_F(RSListenedCanvasTest, RequestSplitToListener, Function | SmallTest | Level2)
{
    std::unique_ptr<MockDrawingCanvas> mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
    std::shared_ptr<MockRSPaintFilterCanvas> mockRSPaintFilterCanvas =
        std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
    //std::shared_ptr<MockRSPaintFilterCanvas> mockRSPaintFilterCanvas = nullptr;
    std::shared_ptr<MockRSCanvasListener> mockRSCanvasListener =
        std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);
    setExpectations(mockRSPaintFilterCanvas, mockRSCanvasListener);
    // Step 4: Create RSListenedCanvas object
    std::shared_ptr<RSListenedCanvas> listenedCanvas =
        std::make_shared<RSListenedCanvas>(*mockRSPaintFilterCanvas);
    //std::shared_ptr<MockRSCanvasListener> mockRSCanvasListener = nullptr;
    //std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;

    
    callDrawMethods(listenedCanvas);
}
} // namespace Rosen
} // namespace OHOS
