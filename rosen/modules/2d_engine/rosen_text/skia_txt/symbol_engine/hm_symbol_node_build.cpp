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

/**
 * @brief  Obtain the group id to layer
 * @param groupIds (output paramer) the index of groupIds if layer index, the groupIds[index] is the group index
 * @param renderGroup the renderGroup info of symbol
 * @param index the renderGroup index
 */
static void GetLayersGroupId(std::vector<size_t>& groupIds, const RSRenderGroup& renderGroup, size_t index)
{
    for (auto& groupInfo : renderGroup.groupInfos) {
        for (auto& j : groupInfo.layerIndexes) {
            if (j < groupIds.size()) {
                groupIds[j] = index;
            }
        }
        for (auto& j : groupInfo.maskIndexes) {
            if (j < groupIds.size()) {
                groupIds[j] = index;
            }
        }
    }
}

/**
 * @brief Merge the paths of Group and obtain the corresponding rendering color.
 * @param groupInfos a group informations of dynamic drawing
 * @param pathLayers the path data of layers on symbol
 * @param indexs the indexs of groups on renderGroups, the indexs.size == pathLayers.size
 * @param pathsColor (output paramer) the result of the merge, where each merged path corresponds to a specific color
 * @param renderGroups the groups informations of static drawing
 */
static void MergePathAndColor(const std::vector<RSGroupInfo>& groupInfos,
    std::vector<RSPath>& pathLayers, const std::vector<size_t>& indexs,
    std::vector<TextEngine::LayersColor>& pathsColor, const std::vector<RSRenderGroup>& renderGroups)
{
    size_t i = 0;
    RSSColor color = {0, 0, 0, 0}; // default color with 0 alpha
    for (const auto& groupInfo : groupInfos) {
        TextEngine::LayersColor layer;
        size_t t = 0;
        for (size_t j = 0; j < groupInfo.layerIndexes.size(); j++) {
            auto k = groupInfo.layerIndexes[j];
            if (k >= pathLayers.size() || k >= indexs.size()) {
                continue;
            }
            if (j == 0) { // initialize layer
                layer.color = indexs[k] < renderGroups.size() ? renderGroups[indexs[k]].color : color;
                layer.path.AddPath(pathLayers[k]);
                t = k;
                continue;
            }
            // If the groupIndex of two paths is different, updata pathsColor and layer
            if (indexs[t] != indexs[k]) {
                pathsColor.push_back(layer);
                layer.path.Reset();
                layer.color = indexs[k] < renderGroups.size() ? renderGroups[indexs[k]].color : color;
                t = k;
            }
            layer.path.AddPath(pathLayers[k]);
        }
        pathsColor.push_back(layer);

        RSPath maskPath;
        for (size_t h : groupInfo.maskIndexes) {
            if (h < pathLayers.size()) {
                maskPath.AddPath(pathLayers[h]);
            }
        }
        if (!maskPath.IsValid()) {
            i = pathsColor.size();
            continue;
        }

        for (size_t j = i; j < pathsColor.size(); j++) {
            Drawing::Path outPath;
            bool isOk = outPath.Op(pathsColor[j].path, maskPath, Drawing::PathOp::DIFFERENCE);
            pathsColor[j].path = isOk ? outPath : pathsColor[j].path;
        }
        i = pathsColor.size();
    }
}

/**
 * @brief check if the symbol group is a mask layer, that it has only mask indexes
 * @param multPath (output paramer) combine all paths of mask indexes in groupInfos
 * @param groupInfos the groupsInfos of symbol
 * @param pathLayers the all paths of symbol
 * @return true if the group is a mask layer
 */
static bool IsMaskLayer(RSPath& multPath, const std::vector<RSGroupInfo>& groupInfos, std::vector<RSPath>& pathLayers)
{
    for (const auto& groupInfo : groupInfos) {
        RSPath pathTemp;
        if (groupInfo.layerIndexes.size() > 0) {
            return false;
        }
        for (auto k : groupInfo.maskIndexes) {
            if (k >= pathLayers.size()) {
                continue;
            }
            pathTemp.AddPath(pathLayers[k]);
        }
        multPath.AddPath(pathTemp);
    }
    return true;
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

    // Obtain the group id of layer
    std::vector<size_t> groupIds(pathLayers.size(), pathLayers.size());
    for (size_t idex = 0; idex < symbolData.symbolInfo_.renderGroups.size(); idex++) {
        GetLayersGroupId(groupIds, symbolData.symbolInfo_.renderGroups[idex], idex);
    }

    // Splitting animation nodes information
    for (auto& groupSetting: groupSettings) {
        TextEngine::SymbolNode symbolNode;
        RSPath maskPath;
        bool isMask = IsMaskLayer(maskPath, groupSetting.groupInfos, pathLayers);
        if (!isMask) {
            MergePathAndColor(groupSetting.groupInfos, pathLayers, groupIds, symbolNode.pathsColor,
                symbolData.symbolInfo_.renderGroups);
        } else {
            symbolNode.path = maskPath;
        }
        symbolNode.nodeBoundary = nodeBounds;
        symbolNode.symbolData = symbolData;
        symbolNode.animationIndex = groupSetting.animationIndex;
        symbolNode.isMask = isMask;
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