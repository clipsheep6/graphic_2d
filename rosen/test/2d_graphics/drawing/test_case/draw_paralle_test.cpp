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
    std::shared_ptr<Drawing::RecordingCanvas> recording = picture.BeginRecording(50, 50);
    Drawing::Font font = Drawing::Font();
    font.SetSize(50);
    std::shared_ptr<Drawing::TextBlob> textblob = Drawing::TextBlob::MakeFromString("thread",
        font, Drawing::TextEncoding::UTF8);
    // Drawing::Path path;
    // path.MoveTo(0, 0); // from (0, 0)
    // path.LineTo(200, 200); // to (300, 300)
    // recording->DrawPath(path);
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(50, 50, bitmapFormat);
    bitmap.ClearWithColor(Drawing::Color::COLOR_BLUE);

    Drawing::Image image;
    image.BuildFromBitmap(bitmap);
    Drawing::Matrix matrix;
    // Set matrix to rotate by degrees 45 about a pivot point at (0, 0).
    matrix.Rotate(45, 0, 0);
    auto e = Drawing::ShaderEffect::CreateImageShader(image, Drawing::TileMode::REPEAT, Drawing::TileMode::MIRROR, Drawing::SamplingOptions(), matrix);
    auto c = Drawing::ColorSpace::CreateRefImage(image);
    auto rect = Drawing::Rect(500, 500, 700, 700);

    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetColor(pen.GetColor4f(), c);
    pen.SetWidth(10); // The thickness of the pen is 10
    pen.SetShaderEffect(e);
    recording->AttachPen(pen);
    recording->DrawImage(image, 500, 60, Drawing::SamplingOptions());
    HILOGE("thread 111");
    recording->DrawBitmap(bitmap, 500, 360);
    HILOGE("thread 222");
    recording->DrawTextBlob(textblob.get(), 500, 660);
    HILOGE("thread 333");
    recording->DrawRect(rect);
    HILOGE("thread 444");
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

    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);
    auto bitmapCanvas = std::make_shared<Drawing::Canvas>();

    bitmapCanvas->Bind(bitmap);
    bitmapCanvas->Clear(0xFFFF0000);
    bitmapCanvas->DrawPicture(finishedPicture);
    canvas->DrawBitmap(bitmap, 0, 0);
}

void DrawParalleTest::OnTestFunctionGpuUpScreen(Drawing::Canvas* canvas)
{
    std::shared_ptr<Drawing::Picture> finishedPicture;

    // 创建子线程并执行绘制操作
    std::thread thread([&finishedPicture]() {
        finishedPicture = threadFunction();
    });
    Drawing::Font font = Drawing::Font();
    font.SetSize(100);
    std::shared_ptr<Drawing::TextBlob> textblob = Drawing::TextBlob::MakeFromString("gpu",
        font, Drawing::TextEncoding::UTF8);
    // Drawing::Path path;
    // path.MoveTo(0, 0); // from (0, 0)
    // path.LineTo(200, 200); // to (300, 300)
    // recording->DrawPath(path);
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(100, 100, bitmapFormat);
    bitmap.ClearWithColor(Drawing::Color::COLOR_YELLOW);

    Drawing::Image image;
    image.BuildFromBitmap(bitmap);
    Drawing::Matrix matrix;
    // Set matrix to rotate by degrees 45 about a pivot point at (0, 0).
    matrix.Rotate(45, 0, 0);
    auto e = Drawing::ShaderEffect::CreateImageShader(image, Drawing::TileMode::REPEAT, Drawing::TileMode::MIRROR, Drawing::SamplingOptions(), matrix);
    auto c = Drawing::ColorSpace::CreateRefImage(image);
    auto rect = Drawing::Rect(100, 100, 700, 700);

    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetColor(pen.GetColor4f(), c);
    pen.SetWidth(10); // The thickness of the pen is 10
    pen.SetShaderEffect(e);
    canvas->AttachPen(pen);
    canvas->DrawImage(image, 100, 60, Drawing::SamplingOptions());
    HILOGE("gpu 111");
    canvas->DrawBitmap(bitmap, 100, 360);
    HILOGE("gpu 222");
    canvas->DrawTextBlob(textblob.get(), 100, 660);
    HILOGE("gpu 333");
    canvas->DrawRect(rect);
    HILOGE("gpu 444");
    // 等待子线程执行完毕
    thread.join();

    // 使用绘制结果进行进一步处理
    if (finishedPicture != nullptr) {
        std::cout<<"finishedPicture is not nullptr"<<std::endl;
        canvas->DrawPicture(finishedPicture);
    }
}
} // namespace Rosen
} // namespace OHOS