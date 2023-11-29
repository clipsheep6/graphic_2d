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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ENGINE_HW_SYMBOL_RUN_H
#define ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ENGINE_HW_SYMBOL_RUN_H

#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <include/core/SkTextBlob.h>
#include <include/core/rs_hw_symbol.h>

#include "hw_symbol_txt.h"
#include "texgine/text_style.h"
#include "texgine/utils/exlog.h"
#include "texgine_text_blob.h"
#include "texgine_canvas.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class HWSymbolRun {
public:
    HWSymbolRun(){};
    ~HWSymbolRun(){};

    static SymbolLayers GetSymbolLayers(const SkGlyphID& glyphId, const HWSymbolTxt& symbolText);

    static void SetSymbolRenderColor(const SymbolRenderingStrategy& renderMode, const std::vector<SColor>& colors,
        SymbolLayers& symbolInfo);

    static void DrawSymbol(TexgineCanvas &canvas, const std::shared_ptr<TexgineTextBlob> &blob, float x, float y,
        const TexginePaint &paint, const TextStyle &style);
};

}
}
}

#endif