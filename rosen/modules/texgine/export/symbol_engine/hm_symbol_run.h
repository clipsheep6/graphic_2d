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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ENGINE_HM_SYMBOL_RUN_H
#define ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ENGINE_HM_SYMBOL_RUN_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#ifndef USE_ROSEN_DRAWING
#include <include/core/HMSymbol.h>
#endif

#include "hm_symbol_txt.h"
#include "texgine/text_style.h"
#include "texgine/utils/exlog.h"
#include "texgine_text_blob.h"
#include "texgine_canvas.h"
#include "symbol_animation_config.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class HMSymbolRun {
public:
    HMSymbolRun() {}
    ~HMSymbolRun() {}

#ifndef USE_ROSEN_DRAWING
    SymbolLayers GetSymbolLayers(const SkGlyphID& glyphId, const HMSymbolTxt& symbolText);

    void SetSymbolRenderColor(const SymbolRenderingStrategy& renderMode, const std::vector<SColor>& colors,
        SymbolLayers& symbolInfo);

    bool GetAnimationGroups(const uint32_t glyohId, const EffectStrategy effectStrategy,
        AnimationSetting& animationOut);

    bool SetGroupsByEffect(const uint32_t glyphId, const EffectStrategy effectStrategy,
        std::vector<RenderGroup>& renderGroups);

    bool SymbolAnimation(const HMSymbolData symbol, const uint32_t glyohId,
        const std::pair<double, double> offset, const EffectStrategy effectMode);

#else
    RSSymbolLayers GetSymbolLayers(const uint16_t& glyphId, const HMSymbolTxt& symbolText);

    void SetSymbolRenderColor(const RSSymbolRenderingStrategy& renderMode, const std::vector<RSSColor>& colors,
        RSSymbolLayers& symbolInfo);

    bool GetAnimationGroups(const uint32_t glyohId, const RSEffectStrategy effectStrategy,
        RSAnimationSetting& animationOut);

    bool SetGroupsByEffect(const uint32_t glyphId, const RSEffectStrategy effectStrategy,
        std::vector<RSRenderGroup>& renderGroups);

    bool SymbolAnimation(const RSHMSymbolData symbol, const uint32_t glyohId,
        const std::pair<double, double> offset, const RSEffectStrategy effectMode);

#endif

    void DrawSymbol(TexgineCanvas &canvas, const std::shared_ptr<TexgineTextBlob> &blob,
        const std::pair<double, double>& offset, const TexginePaint &paint, const TextStyle &style);

    void SetAnimation(std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
        animationFunc)
    {
        if (animationFunc) {
            animationFunc_ = animationFunc;
        }
    }

    void SetSymbolId(const uint64_t& symbolId)
    {
        symbolId_ = symbolId;
    }

private:
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc_ = nullptr;

    uint64_t symbolId_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif