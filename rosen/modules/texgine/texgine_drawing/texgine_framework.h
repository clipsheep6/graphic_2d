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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FRAMEWORK_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FRAMEWORK_H

#include <memory>

#include <skia_framework.h>

#include "texgine_canvas.h"

namespace Texgine {
class TexgineFramework {
public:
    using DrawFunc = std::function<void(TexgineCanvas &canvas)>;

    static void DrawString(TexgineCanvas &canvas, const std::string &str, double x, double y);
    void SetWindowWidth(int width);
    void SetDrawFunc(const DrawFunc &onDraw);
    void Run();

private:
    std::shared_ptr<SkiaFramework> framework_ = std::make_shared<SkiaFramework>();
};
} // namespace Texgine

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FRAMEWORK_H