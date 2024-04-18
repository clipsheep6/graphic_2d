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
#include "hm_symbol_node_build.h"
#include "include/pathops/SkPathOps.h"

namespace OHOS {
namespace Rosen {
namespace SPText {

static const std::map<RSEffectStrategy, TextEngine::SymbolAnimationEffectStrategy> ANIMATION_TYPES = {
    {RSEffectStrategy::NONE, TextEngine::SymbolAnimationEffectStrategy::SYMBOL_NONE},
    {RSEffectStrategy::SCALE, TextEngine::SymbolAnimationEffectStrategy::SYMBOL_SCALE},
    {RSEffectStrategy::VARIABLE_COLOR, TextEngine::SymbolAnimationEffectStrategy::SYMBOL_VARIABLE_COLOR},
    {RSEffectStrategy::APPEAR, TextEngine::SymbolAnimationEffectStrategy::SYMBOL_APPEAR},
    {RSEffectStrategy::DISAPPEAR, TextEngine::SymbolAnimationEffectStrategy::SYMBOL_DISAPPEAR},
    {RSEffectStrategy::BOUNCE, TextEngine::SymbolAnimationEffectStrategy::SYMBOL_BOUNCE},
    {RSEffectStrategy::PULSE, TextEngine::SymbolAnimationEffectStrategy::SYMBOL_PULSE},
    {RSEffectStrategy::REPLACE_APPEAR, TextEngine::SymbolAnimationEffectStrategy::SYMBOL_REPLACE_APPEAR}};

static void MergePath(RSPath& multPath, const std::vector<RSGroupInfo>& groupInfos, std::vector<RSPath>& pathLayers)
{
    for (const auto& groupInfo : groupInfos) {
        RSPath pathTemp;
        for (auto k : groupInfo.layerIndexes) {
            if (k >= pathLayers.size()) {
                continue;
            }
            pathTemp.AddPath(pathLayers[k]);
        }
        for (size_t h : groupInfo.maskIndexes) {
            if (h >= pathLayers.size()) {
                continue;
            }
            RSPath outPath;
            auto isOk = outPath.Op(pathTemp, pathLayers[h], RSPathOp::DIFFERENCE);
            if (isOk) {
                pathTemp = outPath;
            }
        }
        multPath.AddPath(pathTemp);
    }
}

SymbolNodeBuild::SymbolNodeBuild(const RSAnimationSetting animationSetting, const RSHMSymbolData symbolData,
    const RSEffectStrategy effectStrategy,
    const std::pair<double, double> offset) : animationSetting_(animationSetting),
    symbolData_(symbolData), effectStrategy_(effectStrategy),
    offsetX_(offset.first), offsetY_(offset.second) {}

void SymbolNodeBuild::AddWholeAnimation(const RSHMSymbolData &symbolData, const Vector4f &nodeBounds,
    std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfig)
{
    TextEngine::SymbolNode symbolNode;
    symbolNode.symbolData = symbolData;
    symbolNode.nodeBoundary = nodeBounds;
    symbolAnimationConfig->SymbolNodes.push_back(symbolNode);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->SymbolNodes.size();
}

void SymbolNodeBuild::AddHierarchicalAnimation(RSHMSymbolData &symbolData, const Vector4f &nodeBounds,
    std::vector<RSGroupSetting> &groupSettings,
    std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfig)
{
    std::vector<RSPath> paths;
    RSHMSymbol::PathOutlineDecompose(symbolData.path_, paths);
    std::vector<RSPath> pathLayers;
    RSHMSymbol::MultilayerPath(symbolData.symbolInfo_.layers, paths, pathLayers);
    RSSColor color;
    size_t i = 0;
    auto renderGroups = symbolData.symbolInfo_.renderGroups;
    for (auto& groupSetting: groupSettings) {
        RSPath multPath;
        MergePath(multPath, groupSetting.groupInfos, pathLayers);
        if (i < renderGroups.size()) {
            color = renderGroups[i].color;
            i++;
        }
        TextEngine::SymbolNode symbolNode = {multPath, color, nodeBounds, symbolData, groupSetting.animationIndex};
        symbolAnimationConfig->SymbolNodes.push_back(symbolNode);
    }
    symbolAnimationConfig->numNodes = symbolAnimationConfig->SymbolNodes.size();
}

void SymbolNodeBuild::ClearAnimation()
{
    if (animationFunc_ == nullptr) {
        return;
    }
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    if (symbolAnimationConfig == nullptr) {
        return;
    }
    symbolAnimationConfig->effectStrategy = TextEngine::SymbolAnimationEffectStrategy::SYMBOL_NONE;
    symbolAnimationConfig->symbolSpanId = symblSpanId_;
    animationFunc_(symbolAnimationConfig);
}

bool SymbolNodeBuild::DecomposeSymbolAndDraw()
{
    if (symbolData_.symbolInfo_.renderGroups.size() <= 0) {
        return false;
    }
    if (animationFunc_ == nullptr) {
        return false;
    }
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    if (symbolAnimationConfig == nullptr) {
        return false;
    }
    auto rect = symbolData_.path_.GetBounds();
    float nodeWidth = rect.GetWidth();
    float nodeHeight = rect.GetHeight();
    Vector4f nodeBounds = Vector4f(offsetX_, offsetY_, nodeWidth, nodeHeight);

    if (effectStrategy_ == RSEffectStrategy::VARIABLE_COLOR || animationMode_ == 0) {
        AddHierarchicalAnimation(symbolData_, nodeBounds, animationSetting_.groupSettings, symbolAnimationConfig);
    } else {
        AddWholeAnimation(symbolData_, nodeBounds, symbolAnimationConfig);
    }

    auto iter = ANIMATION_TYPES.find(effectStrategy_);
    if (iter != ANIMATION_TYPES.end()) {
        symbolAnimationConfig->effectStrategy = iter->second;
    }
    symbolAnimationConfig->repeatCount = repeatCount_;
    symbolAnimationConfig->animationMode = animationMode_;
    symbolAnimationConfig->animationStart = animationStart_;
    symbolAnimationConfig->symbolSpanId = symblSpanId_;
    symbolAnimationConfig->commonSubType = commonSubType_;
    animationFunc_(symbolAnimationConfig);
    return true;
}
}
}
}