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

#include "screen_manager/rs_screen_mode_info_ext.h"

namespace OHOS {
namespace Rosen {
RSScreenModeInfoExt::RSScreenModeInfoExt(
    int32_t width, int32_t height, uint32_t refreshRate, int32_t id, uint32_t groupId)
    : RSScreenModeInfo(width, height, refreshRate, id), groupId_(groupId)
{
}

RSScreenModeInfoExt::RSScreenModeInfoExt(const RSScreenModeInfo& screenModeInfo, uint32_t groupId)
    : RSScreenModeInfo(screenModeInfo), groupId_(groupId)
{
}

RSScreenModeInfoExt::RSScreenModeInfoExt(const RSScreenModeInfoExt& other)
    : RSScreenModeInfo(other.GetScreenWidth(), other.GetScreenHeight(), other.GetScreenRefreshRate(),
                       other.GetScreenModeId()),
    groupId_(other.groupId_)
{
}

RSScreenModeInfoExt& RSScreenModeInfoExt::operator=(const RSScreenModeInfoExt& other)
{
    SetScreenWidth(other.GetScreenWidth());
    SetScreenHeight(other.GetScreenHeight());
    SetScreenRefreshRate(other.GetScreenRefreshRate());
    SetScreenModeId(other.GetScreenModeId());
    groupId_ = other.groupId_;
    return *this;
}

bool RSScreenModeInfoExt::Marshalling(Parcel& parcel) const
{
    return RSScreenModeInfo::Marshalling(parcel) && parcel.WriteUint32(groupId_);
}

RSScreenModeInfoExt* RSScreenModeInfoExt::Unmarshalling(Parcel& parcel)
{
    int32_t width;
    int32_t height;
    uint32_t refreshRate;
    int32_t id;
    uint32_t groupId;
    if (!(parcel.ReadInt32(width) && parcel.ReadInt32(height) && parcel.ReadUint32(refreshRate)
        && parcel.ReadInt32(id) && parcel.ReadUint32(groupId))) {
        return nullptr;
    }

    RSScreenModeInfoExt* screenModeInfo = new RSScreenModeInfoExt(width, height, refreshRate, id, groupId);
    return screenModeInfo;
}

uint32_t RSScreenModeInfoExt::GetGroupId() const
{
    return groupId_;
}

void RSScreenModeInfoExt::SetGroupId(uint32_t groupId)
{
    groupId_ = groupId;
}
} // namespace Rosen
} // namespace OHOS