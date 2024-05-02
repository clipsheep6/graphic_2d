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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_listened_canvas.h"

#include "third_party/skia/include/core/SkRRect.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkRegion.h"
#include "third_party/skia/include/core/SkScalar.h"
#include "third_party/skia/include/core/SkBlendMode.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockOverDrawCanvas : public Drawing::OverDrawCanvas {
public:
    MockOverDrawCanvas() : Drawing::OverDrawCanvas(nullptr) {}

    MOCK_METHOD(void, DrawRect, (const Drawing::Rect&), (override));
    MOCK_METHOD(void, DrawRoundRect, (const Drawing::RoundRect&), (override));
    MOCK_METHOD(void, DrawLine, (const Drawing::Point&, const Drawing::Point&), (override));
    MOCK_METHOD(void, DrawPath, (const Drawing::Path&), (override));
    MOCK_METHOD(void, DrawNestedRoundRect, (const Drawing::RoundRect&, const Drawing::RoundRect&), (override));
    MOCK_METHOD(void, DrawArc, (const Drawing::Rect&, Drawing::scalar, Drawing::scalar), (override));
    MOCK_METHOD(void, DrawPie, (const Drawing::Rect&, Drawing::scalar, Drawing::scalar), (override));
    MOCK_METHOD(void, DrawOval, (const Drawing::Rect&), (override));
    MOCK_METHOD(void, DrawCircle, (const Drawing::Point&, Drawing::scalar), (override));
    MOCK_METHOD(void, DrawBitmap, (const Drawing::Bitmap&, const Drawing::scalar, const Drawing::scalar), (override));
    MOCK_METHOD(void, DrawImage, (const Drawing::Image&, Drawing::scalar, Drawing::scalar,
        const Drawing::SamplingOptions&), (override));
    MOCK_METHOD(void, DrawImageRect, (const Drawing::Image&, const Drawing::Rect&, const Drawing::Rect&,
        const Drawing::SamplingOptions&, Drawing::SrcRectConstraint), (override));
};
class RSGPUOverdrawCanvasListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Canvas> canvas_;
    std::shared_ptr<MockOverDrawCanvas> overdrawCanvasMock_;
    std::shared_ptr<OHOS::Rosen::RSGPUOverdrawCanvasListener> listener_;
};

void RSGPUOverdrawCanvasListenerTest::SetUpTestCase() {}
void RSGPUOverdrawCanvasListenerTest::TearDownTestCase() {}
void RSGPUOverdrawCanvasListenerTest::SetUp()
{
    canvas_ = std::make_shared<Drawing::Canvas>();
    overdrawCanvasMock_ = std::make_shared<MockOverDrawCanvas>();
    listener_ = std::make_shared<OHOS::Rosen::RSGPUOverdrawCanvasListener>(*canvas_);
    listener_->SetOverdrawCanvas(overdrawCanvasMock_);
}
void RSGPUOverdrawCanvasListenerTest::TearDown() {}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawRect, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawRect(testing::_)).Times(1);
    listener_->DrawRect(Drawing::Rect());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawRoundRect, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawRoundRect(testing::_)).Times(1);
    listener_->DrawRoundRect(Drawing::RoundRect());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawLine, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawPath(testing::_)).Times(1);
    listener_->DrawLine(Drawing::Point(), Drawing::Point());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, DrawNestedRoundRect, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawNestedRoundRect(testing::_, testing::_)).Times(1);
    listener_->DrawNestedRoundRect(Drawing::RoundRect(), Drawing::RoundRect());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawArc, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawPath(testing::_)).Times(1);
    listener_->DrawArc(Drawing::Rect(), Drawing::scalar(), Drawing::scalar());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawPie, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawPath(testing::_)).Times(1);
    listener_->DrawPie(Drawing::Rect(), Drawing::scalar(), Drawing::scalar());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawOval, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawPath(testing::_)).Times(1);
    listener_->DrawOval(Drawing::Rect());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawCircle, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawPath(testing::_)).Times(1);
    listener_->DrawCircle(Drawing::Point(), Drawing::scalar());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawPath, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawPath(_)).Times(1);
    listener_->DrawPath(Drawing::Path());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawBitmap, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawRect(_)).Times(1);
    listener_->DrawBitmap(Drawing::Bitmap(), Drawing::scalar(), Drawing::scalar());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawImage, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawRect(_)).Times(1);
    listener_->DrawImage(Drawing::Image(), Drawing::scalar(), Drawing::scalar(),
        Drawing::SamplingOptions());
}

HWTEST_F(RSGPUOverdrawCanvasListenerTest, listenerDrawImageRect, TestSize.Level1)
{
    EXPECT_CALL(*overdrawCanvasMock_, DrawRect(_)).Times(1);
    listener_->DrawImageRect(Drawing::Image(), Drawing::Rect(), Drawing::Rect(),
        Drawing::SamplingOptions(), Drawing::SrcRectConstraint());
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
