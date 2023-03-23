/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "texgine_framework.h"

namespace Texgine {
std::function<void(TexgineCanvas &canvas)> drawFunc;

void OnSkiaFrameworkDraw(SkCanvas &canvas)
{
    auto texgineCanvas = std::make_shared<TexgineCanvas>();
    texgineCanvas->SetCanvas(&canvas);
    drawFunc(*texgineCanvas);
}

void TexgineFramework::DrawString(TexgineCanvas &canvas, const std::string &str, double x, double y)
{
    SkiaFramework::DrawString(*canvas.GetCanvas(), str, x, y);
}

void TexgineFramework::SetWindowWidth(int width)
{
    framework_->SetWindowWidth(width);
}

void TexgineFramework::SetWindowHeight(int height)
{
    framework_->SetWindowHeight(height);
}

void TexgineFramework::SetWindowScale(double scale)
{
    framework_->SetWindowScale(scale);
}

void TexgineFramework::SetDrawFunc(const DrawFunc &onDraw)
{
    drawFunc = onDraw;
    framework_->SetDrawFunc(OnSkiaFrameworkDraw);
}

void TexgineFramework::Run()
{
    framework_->Run();
}
} // namespace Texgine
