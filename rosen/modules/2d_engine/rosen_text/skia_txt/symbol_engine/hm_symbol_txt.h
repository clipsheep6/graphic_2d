/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef LIB_TXT_SRC_SYMBOL_ENGINE_HM_SYMBOL_TXT_H
#define LIB_TXT_SRC_SYMBOL_ENGINE_HM_SYMBOL_TXT_H

#include <iostream>
#include <vector>

#include "drawing.h"

namespace OHOS {
namespace Rosen {
namespace SPText {

class HMSymbolTxt {
public:
    HMSymbolTxt() {}
    ~HMSymbolTxt() {}

    void SetRenderColor(const std::vector<RSSColor>& colorList)
    {
        colorList_ = colorList;
    }

    void SetRenderColor(const RSSColor& colorList)
    {
        colorList_ = {colorList};
    }

    void SetRenderMode(RSSymbolRenderingStrategy renderMode)
    {
        renderMode_ = renderMode;
    }

    void SetSymbolEffect(const RSEffectStrategy& effectStrategy)
    {
        effectStrategy_ = effectStrategy;
    }

    std::vector<RSSColor> GetRenderColor() const
    {
        return colorList_;
    }

    RSSymbolRenderingStrategy GetRenderMode() const
    {
        return renderMode_;
    }

    RSEffectStrategy GetEffectStrategy() const
    {
        return effectStrategy_;
    }

    bool operator ==(HMSymbolTxt const &sym) const;
     
private:
    std::vector<RSSColor> colorList_;
    RSSymbolRenderingStrategy renderMode_ = RSSymbolRenderingStrategy::SINGLE;
    RSEffectStrategy effectStrategy_ = RSEffectStrategy::NONE;
};
}
}
}
#endif