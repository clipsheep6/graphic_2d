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

#include "skia_hm_symbol_config_ohos.h"

#include "utils/log.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {

std::shared_ptr<DrawingSymbolLayersGroups> SkiaHmSymbolConfigOhos::GetSymbolLayersGroups(uint32_t glyphId)
{
    return nullptr;
}

DrawingAnimationSetting SkiaHmSymbolConfigOhos::ConvertToDrawingAnimationSetting(AnimationSetting setting)
{
    DrawingAnimationSetting drawingSetting;
    drawingSetting.animationMode = setting.animationMode;
    drawingSetting.animationSubType = static_cast<DrawingAnimationSubType>(setting.animationSubType);
    drawingSetting.animationType = static_cast<DrawingAnimationType>(setting.animationType);

    std::vector<DrawingGroupSetting> groupSettings;
    for (size_t i = 0; i < setting.groupSettings.size(); i++) {
        DrawingGroupSetting groupSetting;
        groupSetting.animationIndex = setting.groupSettings.at(i).animationIndex;
        groupSetting.groupInfos = ConvertToDrawingGroupInfo(setting.groupSettings.at(i).groupInfos);
        groupSettings.push_back(groupSetting);
    }
    drawingSetting.groupSettings = groupSettings;

    return drawingSetting;
}

DrawingRenderGroup SkiaHmSymbolConfigOhos::ConvertToDrawingRenderGroup(RenderGroup group)
{
    DrawingRenderGroup drawingRenderGroup;
    drawingRenderGroup.color.a = group.color.a;
    drawingRenderGroup.color.r = group.color.r;
    drawingRenderGroup.color.g = group.color.g;
    drawingRenderGroup.color.b = group.color.b;
    drawingRenderGroup.groupInfos = ConvertToDrawingGroupInfo(group.groupInfos);

    return drawingRenderGroup;
}

std::vector<DrawingGroupInfo> SkiaHmSymbolConfigOhos::ConvertToDrawingGroupInfo(std::vector<GroupInfo> infos)
{
    std::vector<DrawingGroupInfo> groupInfos;
    for (size_t i = 0; i < infos.size(); i++) {
        DrawingGroupInfo groupInfo;
        groupInfo.layerIndexes = infos.at(i).layerIndexes;
        groupInfo.maskIndexes = infos.at(i).maskIndexes;
        groupInfos.push_back(groupInfo);
    }
    return groupInfos;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS