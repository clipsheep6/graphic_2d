// /*
#include <gtest/gtest.h>
#include <gmock/gmock.h> // Include the Google Mock framework header
#include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"
namespace OHOS {
namespace Rosen {
// Define a mock class for OverDrawCanvas
class MockOverDrawCanvas : public Drawing::OverDrawCanvas {
public:
    MockOverDrawCanvas() : Drawing::OverDrawCanvas(nullptr) {}
    
    MOCK_METHOD(void, DrawRect, (const Drawing::Rect&), (override));
    MOCK_METHOD(void, DrawRoundRect, (const Drawing::RoundRect&), (override));
    // Add more mock methods as needed
};

// Fixture for RSGPUOverdrawCanvasListener tests
class RSGPUOverdrawCanvasListenerTest : public testing::Test {
protected:
    void SetUp() override {
        // Initialize any necessary objects before each test
        canvas_ = std::make_shared<Drawing::Canvas>();
        overdrawCanvasMock_ = std::make_shared<MockOverDrawCanvas>(); // Create a mock object
        listener_ = std::make_shared<OHOS::Rosen::RSGPUOverdrawCanvasListener>(*canvas_);
        listener_->SetOverdrawCanvas(overdrawCanvasMock_); // Set the mock object
    }

    void TearDown() override {
        // Clean up any objects after each test
    }

    std::shared_ptr<Drawing::Canvas> canvas_;
    std::shared_ptr<MockOverDrawCanvas> overdrawCanvasMock_;
    std::shared_ptr<OHOS::Rosen::RSGPUOverdrawCanvasListener> listener_;
};

// Test case for DrawRect when overdrawCanvas_ is not null
TEST_F(RSGPUOverdrawCanvasListenerTest, DrawRect_WhenOverdrawCanvasIsNotNull) {
    // Set up expectations
    EXPECT_CALL(*overdrawCanvasMock_, DrawRect(testing::_)).Times(1);

    // Perform the action to be tested
    listener_->DrawRect(Drawing::Rect(0, 0, 100, 100));
}

// Test case for DrawRoundRect when overdrawCanvas_ is not null
TEST_F(RSGPUOverdrawCanvasListenerTest, DrawRoundRect_WhenOverdrawCanvasIsNotNull) {
    // Set up expectations
    EXPECT_CALL(*overdrawCanvasMock_, DrawRoundRect(testing::_)).Times(1);

    // Perform the action to be tested
    listener_->DrawRoundRect(Drawing::RoundRect(Drawing::Rect(0, 0, 100, 100), 10, 10));
}
}
}






//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *     http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, Hardware
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

// #include <gmock/gmock.h>
// #include <gtest/gtest.h>
// #include <include/core/SkData.h>
// #include <include/core/SkDrawable.h>
// #include <include/core/SkMatrix.h>
// #include <include/core/SkPath.h>
// #include <include/core/SkPicture.h>
// #include <include/core/SkRRect.h>
// #include <include/core/SkRect.h>
// #include <include/core/SkRegion.h>
// #include <include/core/SkTextBlob.h>
// #include <src/core/SkDrawShadowInfo.h>
// #include <test_header.h>

// #include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"
// #include "platform/ohos/overdraw/rs_listened_canvas.h"
// #include "platform/ohos/overdraw/rs_canvas_listener.h"

// // #include "third_party/skia/include/core/SkRRect.h"
// // #include "third_party/skia/include/core/SkPath.h"
// // #include "third_party/skia/include/core/SkRegion.h"
// // #include "third_party/skia/include/core/SkScalar.h"
// // #include "third_party/skia/include/core/SkBlendMode.h"

// using namespace testing;
// using namespace testing::ext;
// using namespace ::testing;

// namespace OHOS {
// namespace Rosen {
// class RSGPUOverdrawCanvasListenerTest : public testing::Test {
// public:
//     static void SetUpTestCase();
//     static void TearDownTestCase();
//     void SetUp() override;
//     void TearDown() override;
// protected:
//     std::shared_ptr<MockCanvas> mockCanvas;
//     RSGPUOverdrawCanvasListener::GPUContext mockGPUContext;
//     std::shared_ptr<RSGPUOverdrawCanvasListener> listener;
// };

// void RSGPUOverdrawCanvasListenerTest::SetUpTestCase() {}
// void RSGPUOverdrawCanvasListenerTest::TearDownTestCase() {}
// void RSGPUOverdrawCanvasListenerTest::SetUp() {
//         mockCanvas = std::make_shared<MockCanvas>();
//         // 设置构造函数的期望
//         EXPECT_CALL(*mockCanvas, GetGPUContext()).WillRepeatedly(Return(&mockGPUContext));
//         // 创建监听器对象
//         listener = std::make_shared<RSGPUOverdrawCanvasListener>(*mockCanvas);
// }
// void RSGPUOverdrawCanvasListenerTest::TearDown() {
//         mockCanvas.reset();
//         listener.reset();
// }

// // class MockDrawingCanvas : public Drawing::Canvas {
// // public:
// //     MOCK_METHOD1(DrawPoint, void(const Drawing::Point& point));
// // };

// // class MockRSCanvasListener : public RSCanvasListener {
// // public:
// //     explicit MockRSCanvasListener(Drawing::Canvas& canvas) : RSCanvasListener(canvas) {}
// //     MOCK_METHOD1(DrawPoint, void(const Drawing::Point& point));
// // };
// class MockCanvas : public Drawing::Canvas {
// public:
//     MOCK_METHOD0(GetGPUContext, GPUContext*());
// };

// HWTEST_F(RSGPUOverdrawCanvasListenerTest, RequestSplitToListener_DrawPoint, Function | SmallTest | Level2)
// {
//     // 设置 overdrawCanvas_ 为空
//     listener->SetOverdrawCanvas(nullptr);

//     // 捕获标准错误流
//     testing::internal::CaptureStderr();

//     // 执行 DrawLine
//     listener->DrawLine(Drawing::Point(0, 0), Drawing::Point(10, 10));

//     // 获取捕获的标准错误流内容
//     std::string captured_stderr = testing::internal::GetCapturedStderr();

//     // 期望标准错误流中包含特定的错误消息
//     EXPECT_THAT(captured_stderr, ::testing::HasSubstr("overdrawCanvas_ is nullptr"));
// }


// /**
//  * @tc.name: Create001
//  * @tc.desc: test
//  * @tc.type:FUNC
//  * @tc.require:#I8K89C
//  */
// HWTEST_F(RSGPUOverdrawCanvasListenerTest, Create001, TestSize.Level1)
// {
//     Drawing::Canvas canvas;
//     auto listener = new RSGPUOverdrawCanvasListener(canvas);
//     delete listener;
// }

// /**
//  * @tc.name: onDrawRect
//  * @tc.desc: test
//  * @tc.type:FUNC
//  * @tc.require:#I8K89C
//  */
// HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawRect001, TestSize.Level1)
// {
//     Drawing::Canvas canvas;
//     Drawing::Rect rect;
//     auto listener = new RSGPUOverdrawCanvasListener(canvas);
//     listener->DrawRect(rect);
//     delete listener;
// }

// /**
//  * @tc.name: onDrawOval
//  * @tc.desc: test
//  * @tc.type:FUNC
//  * @tc.require:#I8K89C
//  */
// HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawOval001, TestSize.Level1)
// {
//     Drawing::Canvas canvas;
//     Drawing::Rect rect;
//     auto listener = new RSGPUOverdrawCanvasListener(canvas);
//     listener->DrawOval(rect);
//     delete listener;
// }

// /**
//  * @tc.name: onDrawArc
//  * @tc.desc: test
//  * @tc.type:FUNC
//  * @tc.require:#I8K89C
//  */
// HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawArc001, TestSize.Level1)
// {
//     Drawing::Canvas canvas;
//     Drawing::Rect rect;
//     Drawing::scalar startAngle = 0.0;
//     Drawing::scalar sweepAngle = 0.0;
//     auto listener = new RSGPUOverdrawCanvasListener(canvas);
//     listener->DrawArc(rect, startAngle, sweepAngle);
//     delete listener;
// }

// /**
//  * @tc.name: onDrawArc
//  * @tc.desc: test
//  * @tc.type:FUNC
//  * @tc.require:#I8K89C
//  */
// HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawArc002, TestSize.Level1)
// {
//     Drawing::Canvas canvas;
//     Drawing::Rect rect;
//     Drawing::scalar startAngle = 0.0;
//     Drawing::scalar sweepAngle = 0.0;
//     auto listener = new RSGPUOverdrawCanvasListener(canvas);
//     listener->DrawArc(rect, startAngle, sweepAngle);
//     delete listener;
// }

// /**
//  * @tc.name: onDrawPath
//  * @tc.desc: test
//  * @tc.type:FUNC
//  * @tc.require:#I8K89C
//  */
// HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawPath001, TestSize.Level1)
// {
//     Drawing::Canvas canvas;
//     Drawing::Path path;
//     auto listener = new RSGPUOverdrawCanvasListener(canvas);
//     listener->DrawPath(path);
//     delete listener;
// }

// /**
//  * @tc.name: onDrawRegion
//  * @tc.desc: test
//  * @tc.type:FUNC
//  * @tc.require:#I8K89C
//  */
// HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawRegion001, TestSize.Level1)
// {
//     Drawing::Canvas canvas;
//     Drawing::Region region;
//     auto listener = new RSGPUOverdrawCanvasListener(canvas);
//     listener->DrawRegion(region);
//     delete listener;
// }

// /**
//  * @tc.name: onDrawPicture
//  * @tc.desc: test
//  * @tc.type:FUNC
//  * @tc.require:#I8K89C
//  */
// HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawPicture001, TestSize.Level1)
// {
//     Drawing::Canvas canvas;
//     Drawing::Picture picture;
//     auto listener = new RSGPUOverdrawCanvasListener(canvas);
//     listener->DrawPicture(picture);
//     delete listener;
// }
// } // namespace Rosen
// } // namespace OHOS
