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

#include "hm_symbol_run.h"
#include <include/pathops/SkPathOps.h>
#include <src/ports/skia_ohos/HmSymbolConfig_ohos.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {

SymbolLayers HMSymbolRun::GetSymbolLayers(const SkGlyphID& glyphId, const HMSymbolTxt& symbolText)
{
    SymbolLayers symbolInfo;
    symbolInfo.symbolGlyphId_ = glyphId;
    uint32_t symbolId = static_cast<uint32_t>(glyphId);
    SymbolLayersGroups* symbolInfoOrign = HmSymbolConfig_OHOS::getInstance()->getSymbolLayersGroups(symbolId);
    if (symbolInfoOrign == nullptr) {
        return symbolInfo;
    }

    symbolInfo.layers_ = symbolInfoOrign->layers_;
    symbolInfo.renderGroups_ = symbolInfoOrign->renderModeGroups_[SymbolRenderingStrategy::SINGLE];
    symbolInfo.symbolGlyphId_ = symbolInfoOrign->symbolGlyphId_;

    SymbolRenderingStrategy renderMode = symbolText.GetRenderMode();
    if (symbolInfoOrign->renderModeGroups_.find(renderMode) != symbolInfoOrign->renderModeGroups_.end()) {
        symbolInfo.renderGroups_ = symbolInfoOrign->renderModeGroups_[renderMode];
        std::vector<SColor> colorList = symbolText.GetRenderColor();
        if (!colorList.empty()) {
            SetSymbolRenderColor(renderMode, colorList, symbolInfo);
        }
    }
    return symbolInfo;
}

void HMSymbolRun::SetSymbolRenderColor(const SymbolRenderingStrategy& renderMode, const std::vector<SColor>& colors,
    SymbolLayers& symbolInfo)
{
    if (colors.empty()) {
        return;
    }
    switch(renderMode) {
        // SINGLE and HIERARCHICAL: Supports single color setting
        case SymbolRenderingStrategy::SINGLE:
        case SymbolRenderingStrategy::MULTIPLE_OPACITY:
            for (size_t i = 0; i < symbolInfo.renderGroups_.size(); ++i) {
                symbolInfo.renderGroups_[i].color_.r_ = colors[0].r_; // the 0 indicates the the first color is used
                symbolInfo.renderGroups_[i].color_.g_ = colors[0].g_; // the 0 indicates the the first color is used
                symbolInfo.renderGroups_[i].color_.b_ = colors[0].b_; // the 0 indicates the the first color is used
            }
            break;
        // MULTIPLE_COLOR: Supports mutiple color setting
        case SymbolRenderingStrategy::MULTIPLE_COLOR:
            for (size_t i = 0, j = 0; i < symbolInfo.renderGroups_.size() && j < colors.size(); ++i, ++j) {
                symbolInfo.renderGroups_[i].color_.r_ = colors[j].r_;
                symbolInfo.renderGroups_[i].color_.g_ = colors[j].g_;
                symbolInfo.renderGroups_[i].color_.b_ = colors[j].b_;
            }
            break;
        default:
            break;
    }
}

void HMSymbolRun::DrawSymbol(TexgineCanvas &canvas, const std::shared_ptr<TexgineTextBlob> &blob, float x, float y,
    const TexginePaint &paint, const TextStyle &style)
{
    if (blob == nullptr) {
        return;
    }

    SkGlyphID glyphId = blob->GetFirstGlyphID();
    SkPath path = blob->GetPathbyGlyphID(glyphId);

    HMSymbolData symbolData;
    symbolData.symbolInfo_ = GetSymbolLayers(glyphId, style.symbol);
    if (symbolData.symbolInfo_.symbolGlyphId_ != glyphId) {
        path = blob->GetPathbyGlyphID(symbolData.symbolInfo_.symbolGlyphId_);
    }
    symbolData.path_ = path;
    SkPoint offset = SkPoint::Make(x, y);
    canvas.DrawSymbol(symbolData, offset, paint);
}


}
}
}