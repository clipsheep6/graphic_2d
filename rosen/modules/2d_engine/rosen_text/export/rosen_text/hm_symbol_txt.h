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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_HM_SYMBOL_TXT_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_HM_SYMBOL_TXT_H

#include <iostream>
#include <vector>
#include "draw/color.h"

#include "text/hm_symbol.h"

namespace OHOS {
namespace Rosen {

enum VisualMode {
    VISUAL_MEDIUM = 0,
    VISUAL_SMALL = 1,
    VISUAL_LARGER = 2
};

class RS_EXPORT HMSymbolTxt {
public:
    HMSymbolTxt() {}
    ~HMSymbolTxt() {}

    void SetRenderColor(const std::vector<Drawing::DrawingSColor>& colorList);

    void SetRenderColor(const std::vector<Drawing::Color>& colorList);

    void SetRenderColor(const Drawing::Color& color);

    void SetRenderColor(const Drawing::DrawingSColor& colorList);

    void SetRenderMode(const uint32_t& renderMode);

    void SetSymbolEffect(const uint32_t& effectStrategy);

    // set animation mode: the 1 is whole or iteratuve, 0 is hierarchical or cumulative
    void SetAnimationMode(const uint16_t animationMode);

    void SetRepeatCount(const int repeatCount);

    void SetAnimationStart(const bool animationStart);

    // set common subtype of symbol animation attribute
    void SetCommonSubType(Drawing::DrawingCommonSubType commonSubType);

    void SetVisualMode(const VisualMode visual);

    std::vector<Drawing::DrawingSColor> GetRenderColor() const;

    Drawing::DrawingSymbolRenderingStrategy GetRenderMode() const;

    Drawing::DrawingEffectStrategy GetEffectStrategy() const;

    uint16_t GetAnimationMode() const;

    int GetRepeatCount() const;

    bool GetAnimationStart() const;

    std::map<std::string, int> GetVisualMap() const;

    Drawing::DrawingCommonSubType GetCommonSubType() const;

private:
    std::vector<Drawing::DrawingSColor> colorList_;
    Drawing::DrawingSymbolRenderingStrategy renderMode_ = Drawing::DrawingSymbolRenderingStrategy::SINGLE;
    Drawing::DrawingEffectStrategy effectStrategy_ = Drawing::DrawingEffectStrategy::NONE;

    // animationMode_ is the implementation mode of the animation effect:
    // common_animations: the 0 is the byLayer effect and 1 is the wholeSymbol effect;
    // variable_color : the 0 is the cumulative effect and 1 is the iteratuve effect.
    uint16_t animationMode_ = 0;
    int repeatCount_ = 1;
    bool animationStart_ = false;
    std::map<std::string, int> visualMap_;
    Drawing::DrawingCommonSubType commonSubType_ = Drawing::DrawingCommonSubType::DOWN;
};
}
}
#endif