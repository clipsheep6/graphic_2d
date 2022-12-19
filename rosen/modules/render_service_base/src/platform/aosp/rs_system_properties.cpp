/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {

bool RSSystemProperties::GetUniRenderEnabled()
{
    return isUniRenderEnabled_;
}

DirtyRegionDebugType RSSystemProperties::GetDirtyRegionDebugType()
{
    return static_cast<DirtyRegionDebugType>(0);
}

bool RSSystemProperties::GetDrawTextAsBitmap()
{
    return false;
}

bool RSSystemProperties::GetRenderNodeTraceEnabled()
{
    return false;
}

PartialRenderType RSSystemProperties::GetPartialRenderEnabled()
{
    return static_cast<PartialRenderType>(0);
}

PartialRenderType RSSystemProperties::GetUniPartialRenderEnabled()
{
    return static_cast<PartialRenderType>(0);
}

ContainerWindowConfigType RSSystemProperties::GetContainerWindowConfig()
{
    return static_cast<ContainerWindowConfigType>(0);
}
bool RSSystemProperties::GetOcclusionEnabled()
{
    return false;
}

std::string RSSystemProperties::GetRSEventProperty(const std::string &paraName)
{
    return {};
}

bool RSSystemProperties::GetDirectClientCompEnableStatus()
{
    return false;
}

bool RSSystemProperties::GetHighContrastStatus()
{
    return false;
}

uint32_t RSSystemProperties::GetCorrectionMode()
{
    return 0;
}

bool RSSystemProperties::IsUniRenderMode()
{
    return false;
}

void RSSystemProperties::SetRenderMode(bool isUni)
{
}

DumpSurfaceType RSSystemProperties::GetDumpSurfaceType()
{
    return static_cast<DumpSurfaceType>(0);
}

uint64_t RSSystemProperties::GetDumpSurfaceId()
{
    return 0;
}

bool RSSystemProperties::GetTargetDirtyRegionDfxEnabled(std::vector<std::string>& dfxTargetSurfaceNames_)
{
    return false;
}
bool RSSystemProperties::GetDumpLayersEnabled()
{
    return false;
}

void RSSystemProperties::SetDrawTextAsBitmap(bool flag)
{
    return;
}
} // namespace Rosen
} // namespace OHOS
