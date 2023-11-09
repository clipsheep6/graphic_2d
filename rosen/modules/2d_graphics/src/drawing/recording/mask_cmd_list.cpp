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

#include "recording/mask_cmd_list.h"

#include "recording/cmd_list_helper.h"
#include "utils/log.h"
#include "recording/color_space_cmd_list.h"
#include "recording/shader_effect_cmd_list.h"
#include "recording/color_filter_cmd_list.h"
#include "recording/image_filter_cmd_list.h"
#include "recording/mask_filter_cmd_list.h"
#include "recording/path_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::shared_ptr<MaskCmdList> MaskCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<MaskCmdList>();
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    } else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }
    return cmdList;
}

bool MaskCmdList::Playback(Path& path, Brush& brush) const
{
    uint32_t offset = 0;
    MaskPlayer player(path, brush, *this);
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        OpItem* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr != nullptr) {
            if (!player.Playback(curOpItemPtr->GetType(), itemPtr)) {
                LOGE("MaskCmdList::Playback failed!");
                break;
            }

            offset = curOpItemPtr->GetNextOpItemOffset();
        } else {
            LOGE("MaskCmdList::Playback failed, opItem is nullptr");
            break;
        }
    } while (offset != 0);

    return true;
}

/* OpItem */
std::unordered_map<uint32_t, MaskPlayer::MaskPlaybackFunc> MaskPlayer::opPlaybackFuncLUT_ = {
    { MaskOpItem::MASK_BRUSH_OPITEM,          MaskBrushOpItem::Playback },
    { MaskOpItem::MASK_PATH_OPITEM,           MaskPathOpItem::Playback },
};

MaskPlayer::MaskPlayer(Path& path, Brush& brush, const CmdList& cmdList)
    : path_(path), brush_(brush), cmdList_(cmdList) {}

bool MaskPlayer::Playback(uint32_t type, const void* opItem)
{
    if (type == MaskOpItem::OPITEM_HEAD) {
        return true;
    }

    auto it = opPlaybackFuncLUT_.find(type);
    if (it == opPlaybackFuncLUT_.end() || it->second == nullptr) {
        return false;
    }

    auto func = it->second;
    (*func)(*this, opItem);

    return true;
}

MaskBrushOpItem::MaskBrushOpItem(const BrushHandle& brushHandle)
    : MaskOpItem(MASK_BRUSH_OPITEM), brushHandle_(brushHandle) {}

void MaskBrushOpItem::Playback(MaskPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const MaskBrushOpItem*>(opItem);
        op->Playback(player.brush_, player.cmdList_);
    }
}

void MaskBrushOpItem::Playback(Brush& brush, const CmdList& cmdList) const
{
    auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
        cmdList, brushHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
        cmdList, brushHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
        cmdList, brushHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
        cmdList, brushHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
        cmdList, brushHandle_.maskFilterHandle);

    Filter filter;
    filter.SetColorFilter(colorFilter);
    filter.SetImageFilter(imageFilter);
    filter.SetMaskFilter(maskFilter);
    filter.SetFilterQuality(brushHandle_.filterQuality);

    const Color4f color4f = { brushHandle_.color.GetRedF(), brushHandle_.color.GetGreenF(),
        brushHandle_.color.GetBlueF(), brushHandle_.color.GetAlphaF() };

    brush.SetColor(color4f, colorSpace);
    brush.SetShaderEffect(shaderEffect);
    brush.SetBlendMode(brushHandle_.mode);
    brush.SetAntiAlias(brushHandle_.isAntiAlias);
    brush.SetFilter(filter);
}

MaskPathOpItem::MaskPathOpItem(const CmdListHandle& pathHandle)
    : MaskOpItem(MASK_PATH_OPITEM), pathHandle_(pathHandle) {}

void MaskPathOpItem::Playback(MaskPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const MaskPathOpItem*>(opItem);
        op->Playback(player.path_, player.cmdList_);
    }
}

void MaskPathOpItem::Playback(Path& path, const CmdList& cmdList) const
{
    auto readPath = CmdListHelper::GetFromCmdList<PathCmdList, Path>(
        cmdList, pathHandle_);
    path = *readPath;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
