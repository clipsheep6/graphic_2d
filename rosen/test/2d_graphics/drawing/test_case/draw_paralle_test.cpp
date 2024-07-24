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
std::shared_ptr<Drawing::Picture> threadFunction()
{
    Drawing::PictureRecorder picture;
    int width = 50;
    int hight = 50;
    std::shared_ptr<Drawing::RecordingCanvas> recording = picture.BeginRecording(width, hight);
    Drawing::Font font = Drawing::Font();
    // for test 50
    font.SetSize(50);
    std::shared_ptr<Drawing::TextBlob> textblob = Drawing::TextBlob::MakeFromString("thread",
        font, Drawing::TextEncoding::UTF8);
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    // fot test 50
    bitmap.Build(50, 50, bitmapFormat);
    bitmap.ClearWithColor(Drawing::Color::COLOR_BLUE);

    Drawing::Image image;
    image.BuildFromBitmap(bitmap);
    Drawing::Matrix matrix;
    // Set matrix to rotate by degrees 45 about a pivot point at (0, 0).
    matrix.Rotate(45, 0, 0);
    auto e = Drawing::ShaderEffect::CreateImageShader(image, Drawing::TileMode::REPEAT,
        Drawing::TileMode::MIRROR, Drawing::SamplingOptions(), matrix);
    auto c = Drawing::ColorSpace::CreateRefImage(image);
    auto rect = Drawing::Rect(500, 500, 700, 700);

    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetColor(pen.GetColor4f(), c);
    pen.SetWidth(10); // The thickness of the pen is 10
    pen.SetShaderEffect(e);
    recording->AttachPen(pen);
    int px = 500;
    int py1 = 60;
    int py2 = 360;
    int py3 = 660; 
    recording->DrawImage(image, px, py1, Drawing::SamplingOptions());
    HILOGE("thread 111");
    recording->DrawBitmap(bitmap, px, py2);
    HILOGE("thread 222");
    recording->DrawTextBlob(textblob.get(), px, py3);
    HILOGE("thread 333");
    recording->DrawRect(rect);
    HILOGE("thread 444");
    std::shared_ptr<Drawing::Picture> finishedPicture = picture.FinishingRecording();
    return finishedPicture;
}

void DrawParalleTest::OnTestFunctionCpu(Drawing::Canvas* canvas)
{
    std::shared_ptr<Drawing::Picture> finishedPicture;

    std::thread thread([&finishedPicture]() {
        finishedPicture = threadFunction();
    });
    thread.join();

    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    // for test 500
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

    std::thread thread([&finishedPicture]() {
        finishedPicture = threadFunction();
    });
    Drawing::Font font = Drawing::Font();
    // for test 100
    font.SetSize(100);
    std::shared_ptr<Drawing::TextBlob> textblob = Drawing::TextBlob::MakeFromString("gpu",
        font, Drawing::TextEncoding::UTF8);
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    // for test 100
    bitmap.Build(100, 100, bitmapFormat);
    bitmap.ClearWithColor(Drawing::Color::COLOR_YELLOW);

    Drawing::Image image;
    image.BuildFromBitmap(bitmap);
    Drawing::Matrix matrix;
    matrix.Rotate(45, 0, 0);
    auto e = Drawing::ShaderEffect::CreateImageShader(image, Drawing::TileMode::REPEAT,
        Drawing::TileMode::MIRROR, Drawing::SamplingOptions(), matrix);
    auto c = Drawing::ColorSpace::CreateRefImage(image);
    auto rect = Drawing::Rect(100, 100, 700, 700);

    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetColor(pen.GetColor4f(), c);
    pen.SetWidth(10);
    pen.SetShaderEffect(e);
    canvas->AttachPen(pen);
    int px = 100;
    int py1 = 60;
    int py2 = 360;
    int py3 = 660; 
    canvas->DrawImage(image, px, py1, Drawing::SamplingOptions());
    HILOGE("gpu 111");
    canvas->DrawBitmap(bitmap, px, py2);
    HILOGE("gpu 222");
    canvas->DrawTextBlob(textblob.get(), px, py3);
    HILOGE("gpu 333");
    canvas->DrawRect(rect);
    HILOGE("gpu 444");
    thread.join();

    if (finishedPicture != nullptr) {
        std::cout<<"finishedPicture is not nullptr"<<std::endl;
        canvas->DrawPicture(finishedPicture);
    }
}
} // namespace Rosen
} // namespace OHOS