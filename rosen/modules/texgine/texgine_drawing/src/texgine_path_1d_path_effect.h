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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PATH_1D_PATH_EFFECT_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PATH_1D_PATH_EFFECT_H

#include <memory>

#include <include/effects/Sk1DPathEffect.h>

#include "texgine_path.h"
#include "texgine_path_effect.h"

namespace Texgine {
class TexginePath1DPathEffect {
public:
    enum Style {
        kTranslate_Style,
        kRotate_Style,
        kMorph_Style,

        kLastEnum_Style = kMorph_Style,
    };

    static std::shared_ptr<TexginePathEffect> Make(TexginePath& path, float advance, float phase, Style style);
};
} // namespace Texgine

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PATH_1D_PATH_EFFECT_H