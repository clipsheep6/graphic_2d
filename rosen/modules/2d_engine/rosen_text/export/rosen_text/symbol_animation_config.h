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

#ifndef ROSEN_TEXT_SYMBOL_ANIMATION_CONFIG_H
#define ROSEN_TEXT_SYMBOL_ANIMATION_CONFIG_H

namespace OHOS {
namespace Rosen {
namespace TextEngine {

enum SymbolAnimationEffectStrategy{
    INVALID_EFFECT_STRATEGY = 0,
    SYMBOL_NONE = 1,
    SYMBOL_SCALE = 2,
    SYMBOL_HIERARCHICAL = 3,
};


struct SymbolAnimationConfig{
    SymbolAnimationEffectStrategy effectStrategy;
};
}
}
}

#endif