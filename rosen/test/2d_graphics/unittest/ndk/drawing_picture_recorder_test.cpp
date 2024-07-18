/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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


#include "gtest/gtest.h"
#include "drawing_picture_recorder.h"
#include "drawing_canvas.h"
#include "drawing_path.h"
#include "drawing_picture.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPictureRecorderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPictureRecorderTest::SetUpTestCase() {}
void NativeDrawingPictureRecorderTest::TearDownTestCase() {}
void NativeDrawingPictureRecorderTest::SetUp() {}
void NativeDrawingPictureRecorderTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPictureRecorderTest_pictureRecorderCreate001
 * @tc.desc: test for create drawing_picture_pecorder.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPictureRecorderTest, NativeDrawingPictureRecorderTest_pictureRecorderCreate001, TestSize.Level1)
{
    OH_Drawing_PictureRecorder* pictureRecorder=  OH_Drawing_PictureRecorderCreate();
    EXPECT_EQ(pictureRecorder == nullptr, false);
    OH_Drawing_PictureRecorderDestroy(pictureRecorder);
}

/*
 * @tc.name: NativeDrawingPictureRecorderTest_beginRecording001
 * @tc.desc: test for create drawing_picture_pecorder.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPictureRecorderTest, NativeDrawingPictureRecorderTest_beginRecording001, TestSize.Level1)
{
    OH_Drawing_PictureRecorder* pictureRecorder= OH_Drawing_PictureRecorderCreate();
    OH_Drawing_Canvas* canvas= OH_Drawing_BeginRecording(pictureRecorder,300,300);
    EXPECT_EQ(canvas == nullptr, false);
    OH_Drawing_PictureRecorderDestroy(pictureRecorder);
}

/*
 * @tc.name: NativeDrawingPictureRecorderTest_finishingRecording001
 * @tc.desc: test for create drawing_picture_pecorder.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPictureRecorderTest, NativeDrawingPictureRecorderTest_finishingRecording001, TestSize.Level1)
{
    OH_Drawing_PictureRecorder* pictureRecorder=  OH_Drawing_PictureRecorderCreate();
    OH_Drawing_Canvas* canvas= OH_Drawing_BeginRecording(pictureRecorder,300,300);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);
    constexpr int height = 300;
    constexpr int width = 300;
    constexpr float arc = 18.0f;
    int len = height / 4;
    float aX = width / 2;
    float aY = height / 4;
    float dX = aX - len * std::sin(arc);
    float dY = aY + len * std::cos(arc);
    float cX = aX + len * std::sin(arc);
    float cY = dY;
    float bX = aX + (len / 2.0);
    float bY = aY + std::sqrt((cX - dX) * (cX - dX) + (len / 2.0) * (len / 2.0));
    float eX = aX - (len / 2.0);
    float eY = bY;
    OH_Drawing_PathMoveTo(path, aX, aY);
    OH_Drawing_PathLineTo(path, bX, bY);
    OH_Drawing_PathLineTo(path, cX, cY);
    OH_Drawing_PathLineTo(path, dX, dY);
    OH_Drawing_PathLineTo(path, eX, eY);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    EXPECT_EQ(canvas == nullptr, false);
    OH_Drawing_Picture* picture= OH_Drawing_FinishingRecording(pictureRecorder);
    EXPECT_EQ(picture == nullptr, false);
    OH_Drawing_PictureRecorderDestroy(pictureRecorder);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS