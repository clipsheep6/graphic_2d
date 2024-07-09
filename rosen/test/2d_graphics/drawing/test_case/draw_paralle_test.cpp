/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "draw_paralle_test.h"
#include "image/picture_recorder.h"
#include <memory>
#include <thread>

namespace OHOS {
namespace Rosen {
std::shared_ptr<Drawing::Picture> threadFunction() {
    Drawing::PictureRecorder picture;
    int32_t width = 300;
    int32_t height = 300;
    std::shared_ptr<Drawing::RecordingCanvas> recording = picture.BeginRecording(width, height);
    // Drawing::Path path;
    // path.MoveTo(0, 0); // from (0, 0)
    // path.LineTo(300, 300); // to (300, 300)
    // recording->DrawPath(path);

    Drawing::Pen pen;
    pen.SetColor(0xFFFF0000); // color:red
    pen.SetWidth(10.0f); // width:10
    recording->AttachPen(pen);
    Drawing::Path path;
    path.MoveTo(0, 0); // from (0, 0)
    path.LineTo(300, 300); // to (300, 300)
    recording->DrawPath(path);
    recording->DetachPen();
    std::shared_ptr<Drawing::Picture> finishedPicture = picture.FinishingRecording();
    return finishedPicture;
}

void DrawParalleTest::OnTestFunctionCpu(Drawing::Canvas* canvas)
{
    std::shared_ptr<Drawing::Picture> finishedPicture;

    // 创建子线程并执行绘制操作
    std::thread thread([&finishedPicture]() {
        finishedPicture = threadFunction();
    });

    // 等待子线程执行完毕
    thread.join();

    // 使用绘制结果进行进一步处理
    if (finishedPicture != nullptr) {
        finishedPicture->Playback(canvas);
    }
}
} // namespace Rosen
} // namespace OHOS