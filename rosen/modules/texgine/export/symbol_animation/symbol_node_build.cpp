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
#include "symbol_node_build.h"
#include "include/pathops/SkPathOps.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#ifndef USE_ROSEN_DRAWING
static void MergePath(SkPath& multPath, std::vector<GroupInfo>& groupInfos, std::vector<SkPath>& pathLayers)
#else
static void MergePath(RSPath& multPath, std::vector<RSGroupInfo>& groupInfos, std::vector<RSPath>& pathLayers)
#endif
{
    for (auto groupInfo : groupInfos) {
#ifndef USE_ROSEN_DRAWING
        SkPath pathTemp;
#else
        RSPath pathTemp;
#endif
        for (auto k : groupInfo.layerIndexes) {
            if (k >= pathLayers.size()) {
                continue;
            }
#ifndef USE_ROSEN_DRAWING
            pathTemp.addPath(pathLayers[k]);
#else
            pathTemp.AddPath(pathLayers[k]);
#endif
        }
        for (size_t h : groupInfo.maskIndexes) {
            if (h >= pathLayers.size()) {
                continue;
            }
#ifndef USE_ROSEN_DRAWING
            SkPath outPath;
            auto isOk = Op(pathTemp, pathLayers[h], SkPathOp::kDifference_SkPathOp, &outPath);
#else
            RSPath outPath;
            auto isOk = outPath.Op(pathTemp, pathLayers[h], RSPathOp::DIFFERENCE);
#endif
            if (isOk) {
                pathTemp = outPath;
            }
        }
#ifndef USE_ROSEN_DRAWING
        multPath.addPath(pathTemp);
#else
        multPath.AddPath(pathTemp);
#endif
    }
}

#ifndef USE_ROSEN_DRAWING
SymbolNodeBuild::SymbolNodeBuild(const AnimationSetting animationSetting, const HMSymbolData symbolData,
    const EffectStrategy effectStrategy, const std::pair<double, double> offset)
#else
SymbolNodeBuild::SymbolNodeBuild(const RSAnimationSetting animationSetting, const RSHMSymbolData symbolData,
    const RSEffectStrategy effectStrategy, const std::pair<double, double> offset)
#endif
{
    animationSetting_ = animationSetting;
    symbolData_ = symbolData;
    effectStrategy_ = effectStrategy;
    offsetX_ = offset.first;
    offsetY_ = offset.second;
}

#ifndef USE_ROSEN_DRAWING
void SymbolNodeBuild::AddWholeAnimation(std::vector<RenderGroup>& renderGroups,
                                        std::vector<SkPath> &pathLayers,
                                        const Vector4f &nodeBounds,
                                        std::shared_ptr<SymbolAnimationConfig> symbolAnimationConfig)
#else
void SymbolNodeBuild::AddWholeAnimation(std::vector<RSRenderGroup>& renderGroups,
                                        std::vector<RSPath> &pathLayers,
                                        const Vector4f &nodeBounds,
                                        std::shared_ptr<SymbolAnimationConfig> symbolAnimationConfig)
#endif
{
    for (auto group : renderGroups) {
#ifndef USE_ROSEN_DRAWING
        SkPath multPath;
#else
        RSPath multPath;
#endif
        MergePath(multPath, group.groupInfos, pathLayers);
        SymbolNode symbolNode = {multPath, group.color, nodeBounds, 0};
        symbolAnimationConfig->SymbolNodes.push_back(symbolNode);
    }
    symbolAnimationConfig->numNodes = symbolAnimationConfig->SymbolNodes.size();
}

#ifndef USE_ROSEN_DRAWING
void SymbolNodeBuild::AddHierarchicalAnimation(std::vector<RenderGroup>& renderGroups,
                                               std::vector<SkPath> &pathLayers,
                                               const Vector4f &nodeBounds,
                                               const std::vector<GroupSetting> &groupSettings,
                                               std::shared_ptr<SymbolAnimationConfig> symbolAnimationConfig)
#else
void SymbolNodeBuild::AddHierarchicalAnimation(std::vector<RSRenderGroup>& renderGroups,
                                               std::vector<RSPath> &pathLayers,
                                               const Vector4f &nodeBounds,
                                               const std::vector<RSGroupSetting> &groupSettings,
                                               std::shared_ptr<SymbolAnimationConfig> symbolAnimationConfig)
#endif
{
#ifndef USE_ROSEN_DRAWING
    SColor color;
#else
    RSSColor color;
#endif
    size_t i = 0;
    for (auto groupSetting: groupSettings) {
#ifndef USE_ROSEN_DRAWING
        SkPath multPath;
#else
        RSPath multPath;
#endif
        MergePath(multPath, groupSetting.groupInfos, pathLayers);
        if (i <= renderGroups.size()) {
            color = renderGroups[i].color;
            i++;
        }
        SymbolNode symbolNode = {multPath, color, nodeBounds, groupSetting.animationIndex};
        symbolAnimationConfig->SymbolNodes.push_back(symbolNode);
    }
    symbolAnimationConfig->numNodes = symbolAnimationConfig->SymbolNodes.size();
}

bool SymbolNodeBuild::DecomposeSymbolAndDraw()
{
    if (symbolData_.symbolInfo_.renderGroups.size() <= 0) {
        return false;
    }
    if (animationFunc_ == nullptr) {
        return false;
    }
    auto symbolAnimationConfig = std::make_shared<SymbolAnimationConfig>();
#ifndef USE_ROSEN_DRAWING
    std::vector<SkPath> paths;
    HMSymbol::PathOutlineDecompose(symbolData_.path_, paths);
    std::vector<SkPath> pathLayers;
    HMSymbol::MultilayerPath(symbolData_.symbolInfo_.layers, paths, pathLayers);
    auto rect = symbolData_.path_.getBounds();
    float nodeWidth = rect.fRight - rect.fLeft;
    float nodeHeight = rect.fTop - rect.fBottom;
#else
    std::vector<RSPath> paths;
    RSHMSymbol::PathOutlineDecompose(symbolData_.path_, paths);
    std::vector<RSPath> pathLayers;
    RSHMSymbol::MultilayerPath(symbolData_.symbolInfo_.layers, paths, pathLayers);
    auto rect = symbolData_.path_.GetBounds();
    float nodeWidth = rect.GetWidth();
    float nodeHeight = rect.GetHeight();
#endif
    Vector4f nodeBounds = Vector4f(offsetX_, offsetY_, nodeWidth, nodeHeight);

#ifndef USE_ROSEN_DRAWING
    if (effectStrategy_ == EffectStrategy::SCALE) {
#else
    if (effectStrategy_ == RSEffectStrategy::SCALE) {
#endif
        AddWholeAnimation(symbolData_.symbolInfo_.renderGroups, pathLayers,
            nodeBounds, symbolAnimationConfig);
        symbolAnimationConfig->effectStrategy = SymbolAnimationEffectStrategy::SYMBOL_SCALE;
    }
#ifndef USE_ROSEN_DRAWING
    if (effectStrategy_ == EffectStrategy::HIERARCHICAL) {
#else
    if (effectStrategy_ == RSEffectStrategy::HIERARCHICAL) {
#endif
        AddHierarchicalAnimation(symbolData_.symbolInfo_.renderGroups, pathLayers,
            nodeBounds, animationSetting_.groupSettings, symbolAnimationConfig);
        symbolAnimationConfig->effectStrategy = SymbolAnimationEffectStrategy::SYMBOL_HIERARCHICAL;
    }
    animationFunc_(symbolAnimationConfig);
    return true;
}
}
}
}