/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ANIMATION_SYMBOL_NODE_BUILD_H
#define ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ANIMATION_SYMBOL_NODE_BUILD_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#ifndef USE_ROSEN_DRAWING
#include "include/core/HMSymbol.h"
#include "include/core/SkPaint.h"
#include "include/pathops/SkPathOps.h"
#else
#include "drawing.h"
#endif
#include "symbol_engine/hm_symbol_txt.h"
#include "symbol_animation_config.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {

class SymbolNodeBuild {
public:
#ifndef USE_ROSEN_DRAWING
    SymbolNodeBuild(const AnimationSetting animationSetting, const HMSymbolData symbolData,
    const EffectStrategy effectStrategy, const std::pair<double, double> offset);

    void AddWholeAnimation(std::vector<RenderGroup>& renderGroups,
                           std::vector<SkPath> &pathLayers,
                           const Vector4f &nodeBounds,
                           std::shared_ptr<SymbolAnimationConfig> symbolAnimationConfig);

    void AddHierarchicalAnimation(std::vector<RenderGroup>& renderGroups,
                                  std::vector<SkPath> &pathLayers,
                                  const Vector4f &nodeBounds,
                                  std::vector<GroupSetting> &groupSettings,
                                  std::shared_ptr<SymbolAnimationConfig> symbolAnimationConfig);
#else
    SymbolNodeBuild(const RSAnimationSetting animationSetting, const RSHMSymbolData symbolData,
    const RSEffectStrategy effectStrategy, const std::pair<double, double> offset);

    void AddWholeAnimation(std::vector<RSRenderGroup>& renderGroups,
                           std::vector<RSPath> &pathLayers,
                           const Vector4f &nodeBounds,
                           std::shared_ptr<SymbolAnimationConfig> symbolAnimationConfig);

    void AddHierarchicalAnimation(std::vector<RSRenderGroup>& renderGroups,
                                  std::vector<RSPath> &pathLayers,
                                  const Vector4f &nodeBounds,
                                  const std::vector<RSGroupSetting> &groupSettings,
                                  std::shared_ptr<SymbolAnimationConfig> symbolAnimationConfig);
#endif
    ~SymbolNodeBuild() {}
    bool DecomposeSymbolAndDraw();
    void SetAnimation(std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
        animationFunc)
    {
        animationFunc_ = animationFunc;
    }

private:
#ifndef USE_ROSEN_DRAWING
    AnimationSetting animationSetting_;
    HMSymbolData symbolData_;
    EffectStrategy effectStrategy_;
#else
    RSAnimationSetting animationSetting_;
    RSHMSymbolData symbolData_;
    RSEffectStrategy effectStrategy_;
#endif
    double offsetX_;
    double offsetY_;

    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc_ = nullptr;
};
}
}
}

#endif