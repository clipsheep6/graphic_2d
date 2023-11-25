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

#include "hw_symbol_run.h"
#include "third_party/skia/include/pathops/SkPathOps.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
HWSymbolRun::HWSymbolRun()
{
    Read();
}

// demo test
void HWSymbolRun::Read()
{
    //68
    std::vector<std::vector<size_t>> paths8 = {{3}, {2, 6}, {0, 1, 4, 5}};
    GroupInfo groupInfo;
    groupInfo.layerIndexes_ = std::vector<size_t>{0};
    RenderGroup group11;
    group11.groupInfo_ = {groupInfo};
    group11.color_ = SColor{1, 0X00, 0X7A, 0XFF};
    
    groupInfo.layerIndexes_ = std::vector<size_t>{1};
    RenderGroup group22;
    group22.groupInfo_ = {groupInfo};
    group22.color_ = SColor{0.8, 0X00, 0X7A, 0XFF};

    groupInfo.layerIndexes_ = std::vector<size_t>{2};
    RenderGroup group33;
    group33.groupInfo_ = {groupInfo};
    group33.color_ = SColor{0.3, 0X00, 0X7A, 0XFF};

    std::vector<RenderGroup> Groups8 = {group11, group22, group33};
    std::map<SymbolRenderingStrategy, std::vector<RenderGroup>> renderGroups8;
    renderGroups8.insert({SymbolRenderingStrategy::MULTIPLE_OPACITY, Groups8});

    groupInfo.layerIndexes_ = std::vector<size_t>{0, 1, 2};
    group11.groupInfo_ = {groupInfo};
    group11.color_ = SColor{1, 0X80, 0X80, 0X80};
    renderGroups8.insert({SymbolRenderingStrategy::SINGLE, std::vector<RenderGroup> {group11}});
    SymbolLayersGroups symbol8 = {8, paths8, renderGroups8};
    symbolDemoInfo_.insert({8, symbol8});
}

// demo test 
void HWSymbolRun::PathOutlineDecompose(const SkPath& path, std::vector<SkPath>& paths)
{
    SkPath::RawIter iter = SkPath::RawIter(path);
    SkPoint pts[4]; // the 4 is number of points
    SkPath::Verb Verb;
    SkPath path_stemp;
    while ((Verb = iter.next(pts)) != SkPath::kDone_Verb) {
        switch (Verb) {
            case SkPath::kMove_Verb:
                if (!path_stemp.isEmpty()) {
                    paths.push_back(path_stemp);
                }
                path_stemp.reset();
                path_stemp.moveTo(pts[0]); // the 0 is first point
                break;
            case SkPath::kLine_Verb:
                path_stemp.lineTo(pts[1]); // the 1 is second point
                break;
            case SkPath::kQuad_Verb:
                path_stemp.quadTo(pts[1], pts[2]); // the 1 and 2 is second and third point
                break;
            case SkPath::kCubic_Verb:
                path_stemp.cubicTo(pts[1], pts[2], pts[3]); // the 1, 2 and 3 if the second, third and fourth point
                break;
            case SkPath::kConic_Verb:
                path_stemp.conicTo(pts[1], pts[2], iter.conicWeight()); // the 1 and 2 is second and third point
                break;
            case SkPath::kClose_Verb:
                path_stemp.close();
                break;
            case SkPath::kDone_Verb:
                if (!path_stemp.isEmpty()) {
                    paths.push_back(path_stemp);
                }
                path_stemp.reset();
                SkUNREACHABLE;
            default:
                break;
        }
    }
    if (!path_stemp.isEmpty()) {
        paths.push_back(path_stemp);
    }
}

// demo test
void HWSymbolRun::MultilayerPath(const std::vector<std::vector<size_t>>& multMap,
    const std::vector<SkPath>& paths, std::vector<SkPath>& multPaths)
{
    if (multMap.empty()) {
        SkPath path;
        for (size_t i = 0; i < paths.size(); i++) {
            path.addPath(paths[i]);
        }
        multPaths.push_back(path);
        return;
    }

    for (size_t i = 0; i < multMap.size(); i++) {
        SkPath path;
        for (size_t j = 0; j < multMap[i].size(); j++) {
            if (multMap[i][j] < paths.size()) {
                path.addPath(paths[multMap[i][j]]);
            }
        }
        multPaths.push_back(path);
    }
}

// demo test
std::vector<SkPath> HWSymbolRun::GetPathLayers(const std::vector<std::vector<size_t>>& layers, const SkPath& path)
{
    std::vector<SkPath> paths;
    PathOutlineDecompose(path, paths);

    std::vector<SkPath> pathLayers;
    MultilayerPath(layers, paths, pathLayers);
    return pathLayers;
}

bool HWSymbolRun::GetLayerGroups(SkGlyphID symbolId, SymbolLayersGroups& symbolInfo)
{
    // todo : 调用资源框架的接口，获取symbol的多次渲染信息
    if (symbolDemoInfo_.find(symbolId) == symbolDemoInfo_.end()) {
        return false;
    }
    symbolInfo = symbolDemoInfo_[symbolId];
    return true;
}

SymbolLayers HWSymbolRun::GetSymbolLayers(const SkGlyphID& glyphId, const HWSymbolTxt& symbolText)
{
    SymbolLayersGroups symbolInfoOrign;
    SymbolLayers symbolInfo;
    symbolInfo.symbolGlyphId_ = glyphId;
    bool check = GetLayerGroups(glyphId, symbolInfoOrign);
    if (!check) {
        return symbolInfo;
    }

    symbolInfo.layers_ = symbolInfoOrign.layers_;
    symbolInfo.renderGroups_ = symbolInfoOrign.renderModeGroups_[SymbolRenderingStrategy::SINGLE];
    symbolInfo.symbolGlyphId_ = symbolInfoOrign.symbolGlyphId_;

    SymbolRenderingStrategy renderMode = symbolText.GetRenderModer();
    if (symbolInfoOrign.renderModeGroups_.find(renderMode) != symbolInfoOrign.renderModeGroups_.end()) {
        symbolInfo.renderGroups_ = symbolInfoOrign.renderModeGroups_[renderMode];
        std::vector<SColor> colorList = symbolText.GetRenderColor();
        if (!colorList.empty()) {
            SetSymbolRenderColor(renderMode, colorList, symbolInfo);
        }
    }
    return symbolInfo;
}

void HWSymbolRun::SetSymbolRenderColor(const SymbolRenderingStrategy& renderMode, const std::vector<SColor>& colors,
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

void HWSymbolRun::DrawSymbol(TexgineCanvas &canvas, const std::shared_ptr<TexgineTextBlob> &blob, float x, float y,
    const TexginePaint &paint, const TextStyle &style)
{
    
    SkGlyphID glyphId = blob->GetFirstGlyphID();
    SkPath path = blob->GetPathbyGlyphID(glyphId);
    canvas.DrawSymbol(path, paint);

    HWSymbolData symbolData;
    HWSymbolRun symbol;
    symbolData.symbolInfo_ = symbol.GetSymbolLayers(glyphId, style.symbol);
     if (symbolData.symbolInfo_.symbolGlyphId_ != glyphId) {
        path = blob->GetPathbyGlyphID(symbolData.symbolInfo_.symbolGlyphId_);
    }
    symbolData.path_ = path;
    SkPoint offset = SkPoint::Make(x, y);
    canvas->drawSymbol(symbolData, offset, paint);
}

// demo test
void HWSymbolRun::TestDrawSymbol(TexgineCanvas &canvas, const std::shared_ptr<TexgineTextBlob> &blob, float x, float y,
    const TexginePaint &paint, const TextStyle &style)
{
    if (blob == nullptr) {
        return;
    }

    SkGlyphID glyphId = blob->GetFirstGlyphID();
    SkPath path = blob->GetPathbyGlyphID(glyphId);

    TexginePaint paint1 = paint;
    paint1.SetAntiAlias(true);
    paint1.SetStyle(TexginePaint::Style::STROKEANDFILL);
    canvas.DrawPath(path, paint1);

    HWSymbolRun symbol;
    SymbolLayers symbolInfo = symbol.GetSymbolLayers(glyphId, style.symbol);
    if (symbolInfo.symbolGlyphId_ != glyphId) {
        path = blob->GetPathbyGlyphID(symbolInfo.symbolGlyphId_);
    }
    path.offset(x, y);
    if (symbolInfo.renderGroups_.empty()) {
        canvas.DrawPath(path, paint1);
        return;
    }

    auto layer = symbolInfo.layers_;
    std::vector<SkPath> pathLayers = GetPathLayers(layer, path);
    std::vector<RenderGroup> groups = symbolInfo.renderGroups_;
    for (auto group: groups) {
        SkPath multPath;
        for (auto groupInfo: group.groupInfo_) {
            SkPath pathStemp;
            for (auto k: groupInfo.layerIndexes_) {
                if (k < pathLayers.size()) {
                    pathStemp.addPath(pathLayers[k]);
                }
            }
            SkPath pathMask;
            for (auto h : groupInfo.maskIndexes_) {
                if (h < pathLayers.size()) {
                    pathMask.addPath(pathLayers[h]);
                }
            }
            if (!pathMask.isEmpty()) {
                SkPath outPath;
                bool check = Op(pathStemp, pathMask, SkPathOp::kDifference_SkPathOp, &outPath);
                if (check) {
                    pathStemp = outPath;
                }
            }
            multPath.addPath(pathStemp);
        }
        paint1.SetColor(SkColorSetRGB(group.color_.r_, group.color_.g_, group.color_.b_));
        paint1.SetAlphaf(group.color_.a_);
        canvas.DrawPath(multPath, paint1);
    }
}

}
}
}