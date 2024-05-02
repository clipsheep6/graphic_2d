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

#include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_listened_canvas.h"
#include "platform/ohos/overdraw/rs_canvas_listener.h"

// #include "third_party/skia/include/core/SkRRect.h"
// #include "third_party/skia/include/core/SkPath.h"
// #include "third_party/skia/include/core/SkRegion.h"
// #include "third_party/skia/include/core/SkScalar.h"
// #include "third_party/skia/include/core/SkBlendMode.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSGPUOverdrawCanvasListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSGPUOverdrawCanvasListenerTest::SetUpTestCase() {}
void RSGPUOverdrawCanvasListenerTest::TearDownTestCase() {}
void RSGPUOverdrawCanvasListenerTest::SetUp() {}
void RSGPUOverdrawCanvasListenerTest::TearDown() {}

// class MockDrawingCanvas : public Drawing::Canvas {
// public:
//     MOCK_METHOD1(DrawPoint, void(const Drawing::Point& point));
// };

// class MockRSCanvasListener : public RSCanvasListener {
// public:
//     explicit MockRSCanvasListener(Drawing::Canvas& canvas) : RSCanvasListener(canvas) {}
//     MOCK_METHOD1(DrawPoint, void(const Drawing::Point& point));
// };
class MockOverDrawCanvas : public Drawing::Canvas {
public:
    MOCK_METHOD2(DrawLine, void(const Drawing::Point& startPt, const Drawing::Point& endPt));
    MOCK_METHOD1(DrawRect, void(const Drawing::Rect& rect));
    MOCK_METHOD1(DrawRoundRect, void(const Drawing::RoundRect& roundRect));
    MOCK_METHOD2(DrawNestedRoundRect, void(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner));
    MOCK_METHOD3(DrawArc, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD3(DrawPie, void(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle));
    MOCK_METHOD1(DrawOval, void(const Drawing::Rect& oval));
    MOCK_METHOD2(DrawCircle, void(const Drawing::Point& centerPt, const Drawing::scalar radius));
    MOCK_METHOD1(DrawPath, void(const Drawing::Path& path));
    MOCK_METHOD3(DrawBitmap, void(const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py));
};


HWTEST_F(RSGPUOverdrawCanvasListenerTest, RequestSplitToListener, TestSize.Level1)
{
    auto mockOverDrawCanvas = std::make_unique<MockOverDrawCanvas>();

    EXPECT_CALL(*mockOverDrawCanvas, DrawRect(_)).Times(1);
    EXPECT_CALL(*mockOverDrawCanvas, DrawRoundRect(_)).Times(1);
    EXPECT_CALL(*mockOverDrawCanvas, DrawNestedRoundRect(_, _)).Times(1);

    RSGPUOverdrawCanvasListener listener(*mockOverDrawCanvas);

    listener.DrawRect(Drawing::Rect());
    listener.DrawRoundRect(Drawing::RoundRect());
    listener.DrawNestedRoundRect(Drawing::RoundRect(), Drawing::RoundRect());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, RequestSplitToListener_DrawLine, TestSize.Level1)
{
    auto mockOverDrawCanvas = std::make_unique<MockOverDrawCanvas>();

    EXPECT_CALL(*mockOverDrawCanvas, DrawPath(_)).Times(1);

    RSGPUOverdrawCanvasListener listener(*mockOverDrawCanvas);

    listener.DrawLine(Drawing::Point(), Drawing::Point());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, RequestSplitToListener_DrawArc, TestSize.Level1)
{
    auto mockOverDrawCanvas = std::make_unique<MockOverDrawCanvas>();

    EXPECT_CALL(*mockOverDrawCanvas, DrawPath(_)).Times(1);

    RSGPUOverdrawCanvasListener listener(*mockOverDrawCanvas);

    listener.DrawArc(Drawing::Rect(), Drawing::scalar(), Drawing::scalar());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, RequestSplitToListener_DrawPie, TestSize.Level1)
{
    auto mockOverDrawCanvas = std::make_unique<MockOverDrawCanvas>();

    EXPECT_CALL(*mockOverDrawCanvas, DrawPath(_)).Times(1);

    RSGPUOverdrawCanvasListener listener(*mockOverDrawCanvas);

    listener.DrawPie(Drawing::Rect(), Drawing::scalar(), Drawing::scalar());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, RequestSplitToListener_DrawOval, TestSize.Level1)
{
    auto mockOverDrawCanvas = std::make_unique<MockOverDrawCanvas>();

    EXPECT_CALL(*mockOverDrawCanvas, DrawPath(_)).Times(1);

    RSGPUOverdrawCanvasListener listener(*mockOverDrawCanvas);

    listener.DrawOval(Drawing::Rect());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, RequestSplitToListener_DrawCircle, TestSize.Level1)
{
    auto mockOverDrawCanvas = std::make_unique<MockOverDrawCanvas>();

    EXPECT_CALL(*mockOverDrawCanvas, DrawPath(_)).Times(1);

    RSGPUOverdrawCanvasListener listener(*mockOverDrawCanvas);

    listener.DrawCircle(Drawing::Point(), Drawing::scalar());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, RequestSplitToListener_DrawPath, TestSize.Level1)
{
    auto mockOverDrawCanvas = std::make_unique<MockOverDrawCanvas>();

    EXPECT_CALL(*mockOverDrawCanvas, DrawPath(_)).Times(1);

    RSGPUOverdrawCanvasListener listener(*mockOverDrawCanvas);

    listener.DrawPath(Drawing::Path());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, RequestSplitToListener_DrawBitmap, TestSize.Level1)
{
    auto mockOverDrawCanvas = std::make_unique<MockOverDrawCanvas>();

    EXPECT_CALL(*mockOverDrawCanvas, DrawRect(_)).Times(1);

    RSGPUOverdrawCanvasListener listener(*mockOverDrawCanvas);

    listener.DrawBitmap(Drawing::Bitmap(), Drawing::scalar(), Drawing::scalar());
}

/**
 * @tc.name: Create001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, Create001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    delete listener;
}

/**
 * @tc.name: onDrawRect
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawRect001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawRect(rect);
    delete listener;
}

/**
 * @tc.name: onDrawOval
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawOval001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawOval(rect);
    delete listener;
}

/**
 * @tc.name: onDrawArc
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawArc001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    Drawing::scalar startAngle = 0.0;
    Drawing::scalar sweepAngle = 0.0;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawArc(rect, startAngle, sweepAngle);
    delete listener;
}

/**
 * @tc.name: onDrawArc
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawArc002, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    Drawing::scalar startAngle = 0.0;
    Drawing::scalar sweepAngle = 0.0;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawArc(rect, startAngle, sweepAngle);
    delete listener;
}

/**
 * @tc.name: onDrawPath
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawPath001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Path path;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawPath(path);
    delete listener;
}

/**
 * @tc.name: onDrawRegion
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawRegion001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Region region;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawRegion(region);
    delete listener;
}

/**
 * @tc.name: onDrawPicture
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawPicture001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Picture picture;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawPicture(picture);
    delete listener;
}
} // namespace Rosen
} // namespace OHOS
