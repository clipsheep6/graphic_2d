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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ENGINE_HW_SYMBOL_TXT_H
#define ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ENGINE_HW_SYMBOL_TXT_H
#pragma once
#include <iostream>
#include <vector>

#include "third_party/skia/include/core/rs_hw_symbol.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {

class HWSymbolTxt {
public:
    HWSymbolTxt(){};
    ~HWSymbolTxt(){};

    void SetRenderColor(const std::vector<SColor>& colorList)
    {
        colorList_ = colorList;
    };

     void SetRenderColor(const SColor& colorList)
    {
        colorList_ = {colorList};
    };

    void SetRenderMode(SymbolRenderingStrategy renderMode)
    {
        renderMode_ = renderMode;
    };

    void SetSymbolEffect(const EffectStrategy& effectStratey)
    {
        effectStratey_ = effectStratey;
    };

    std::vector<SColor> GetRenderColor() const
    {
       return colorList_;
    };

    SymbolRenderingStrategy GetRenderMode() const
    {
        return renderMode_;
    };

    EffectStrategy GetEffectStrategy() const
    {
        return effectStratey_;
    };

    bool operator ==(HWSymbolTxt const &sym) const;
     
public:
    std::vector<SColor> colorList_;
    SymbolRenderingStrategy renderMode_ = SymbolRenderingStrategy::SINGLE;
    EffectStrategy effectStratey_ = EffectStrategy::NONE;
};

}
}
}
#endif