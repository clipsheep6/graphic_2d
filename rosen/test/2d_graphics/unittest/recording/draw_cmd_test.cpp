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

#include "recording/draw_cmd.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DrawCmdTest::SetUpTestCase() {}
void DrawCmdTest::TearDownTestCase() {}
void DrawCmdTest::SetUp() {}
void DrawCmdTest::TearDown() {}

/**
 * @tc.name: DrawOpItemPlayback001
 * @tc.desc: Test the playback of the DrawOpItem function.
 * @tc.type: FUNC
 * @tc.require: I7OAIR
 */
HWTEST_F(DrawCmdTest, DrawOpItemPlayback001, TestSize.Level1)
{
    auto canvas = std::make_shared<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    auto drawCmdList = std::make_unique<DrawCmdList>();
    Rect rect;
    CanvasPlayer player = { *canvas, *drawCmdList, rect};
    DrawLineOpItem::Playback(player, nullptr);
    DrawRectOpItem::Playback(player, nullptr);
    DrawRoundRectOpItem::Playback(player, nullptr);
    DrawNestedRoundRectOpItem::Playback(player, nullptr);
    DrawArcOpItem::Playback(player, nullptr);
    DrawPieOpItem::Playback(player, nullptr);
    DrawOvalOpItem::Playback(player, nullptr);
    DrawCircleOpItem::Playback(player, nullptr);
    DrawPathOpItem::Playback(player, nullptr);
    DrawBackgroundOpItem::Playback(player, nullptr);
    DrawShadowOpItem::Playback(player, nullptr);
    DrawBitmapOpItem::Playback(player, nullptr);
    DrawAdaptivePixelMapOpItem::Playback(player, nullptr);
    DrawImageOpItem::Playback(player, nullptr);
    DrawAdaptiveImageOpItem::Playback(player, nullptr);
    DrawImageRectOpItem::Playback(player, nullptr);
    DrawPictureOpItem::Playback(player, nullptr);
    ClipRectOpItem::Playback(player, nullptr);
    ClipRoundRectOpItem::Playback(player, nullptr);
    ClipPathOpItem::Playback(player, nullptr);
    SetMatrixOpItem::Playback(player, nullptr);
    ResetMatrixOpItem::Playback(player, nullptr);
    ConcatMatrixOpItem::Playback(player, nullptr);
    TranslateOpItem::Playback(player, nullptr);
    ScaleOpItem::Playback(player, nullptr);
    RotateOpItem::Playback(player, nullptr);
    ShearOpItem::Playback(player, nullptr);
    FlushOpItem::Playback(player, nullptr);
    ClearOpItem::Playback(player, nullptr);
    SaveOpItem::Playback(player, nullptr);
    SaveLayerOpItem::Playback(player, nullptr);
    RestoreOpItem::Playback(player, nullptr);
    AttachPenOpItem::Playback(player, nullptr);
    AttachBrushOpItem::Playback(player, nullptr);
    DetachPenOpItem::Playback(player, nullptr);
    DetachBrushOpItem::Playback(player, nullptr);
    ClipAdaptiveRoundRectOpItem::Playback(player, nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS