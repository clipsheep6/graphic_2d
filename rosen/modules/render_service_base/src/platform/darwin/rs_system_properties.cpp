/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
#if (defined (ACE_ENABLE_GL) && defined (ACE_ENABLE_VK)) || (defined (RS_ENABLE_GL) && defined (RS_ENABLE_VK))
const GpuApiType RSSystemProperties::systemGpuApiType_ = GpuApiType::OPENGL;
#elif defined (ACE_ENABLE_GL) || defined (RS_ENABLE_GL)
const GpuApiType RSSystemProperties::systemGpuApiType_ = GpuApiType::OPENGL;
#else
const GpuApiType RSSystemProperties::systemGpuApiType_ = GpuApiType::VULKAN;
#endif

int RSSystemProperties::GetDumpFrameNum()
{
    return 0;
}

int RSSystemProperties::GetRecordingEnabled()
{
    return 0;
}

bool RSSystemProperties::IsSceneBoardEnabled()
{
    return false;
}

void RSSystemProperties::SetRecordingDisenabled()
{
    return;
}

std::string RSSystemProperties::GetRecordingFile()
{
    return "";
}

bool RSSystemProperties::GetUniRenderEnabled()
{
    return isUniRenderEnabled_;
}

bool RSSystemProperties::GetDrawOpTraceEnabled()
{
    return false;
}

bool RSSystemProperties::GetRenderNodeTraceEnabled()
{
    return {};
}

DirtyRegionDebugType RSSystemProperties::GetDirtyRegionDebugType()
{
    return {};
}

PartialRenderType RSSystemProperties::GetPartialRenderEnabled()
{
    return {};
}

PartialRenderType RSSystemProperties::GetUniPartialRenderEnabled()
{
    return {};
}

bool RSSystemProperties::GetOcclusionEnabled()
{
    return {};
}

std::string RSSystemProperties::GetRSEventProperty(const std::string &paraName)
{
    return {};
}

bool RSSystemProperties::GetDirectClientCompEnableStatus()
{
    return {};
}

bool RSSystemProperties::GetHighContrastStatus()
{
    return {};
}

uint32_t RSSystemProperties::GetCorrectionMode()
{
    return {};
}

DumpSurfaceType RSSystemProperties::GetDumpSurfaceType()
{
    return {};
}

long long int RSSystemProperties::GetDumpSurfaceId()
{
    return {};
}

void RSSystemProperties::SetDrawTextAsBitmap(bool flag)
{
}

bool RSSystemProperties::GetDrawTextAsBitmap()
{
    return {};
}

void RSSystemProperties::SetCacheEnabledForRotation(bool flag)
{
}

bool RSSystemProperties::GetCacheEnabledForRotation()
{
    return {};
}

#ifndef NEW_SKIA
bool RSSystemProperties::GetReleaseResourceEnabled()
{
    return {};
}
#endif

ParallelRenderingType RSSystemProperties::GetPrepareParallelRenderingEnabled()
{
    return {};
}

ParallelRenderingType RSSystemProperties::GetParallelRenderingEnabled()
{
    return {};
}

HgmRefreshRates RSSystemProperties::GetHgmRefreshRatesEnabled()
{
    return {};
}

void RSSystemProperties::SetHgmRefreshRateModesEnabled(std::string param)
{
    return;
}

HgmRefreshRateModes RSSystemProperties::GetHgmRefreshRateModesEnabled()
{
    return {};
}

bool RSSystemProperties::GetSkipForAlphaZeroEnabled()
{
    return {};
}

bool RSSystemProperties::GetSkipGeometryNotChangeEnabled()
{
    return {};
}

bool RSSystemProperties::GetAnimationCacheEnabled()
{
    return {};
}

float RSSystemProperties::GetAnimationScale()
{
    return 1.f;
}

bool RSSystemProperties::GetProxyNodeDebugEnabled()
{
    return false;
}

bool RSSystemProperties::GetFilterCacheEnabled()
{
    return false;
}

int RSSystemProperties::GetFilterCacheUpdateInterval()
{
    return 0;
}

int RSSystemProperties::GetFilterCacheSizeThreshold()
{
    return 0;
}

bool RSSystemProperties::GetFilterPartialRenderEnabled()
{
    return false;
}

bool RSSystemProperties::GetColorPickerPartialEnabled()
{
    return false;
}

bool RSSystemProperties::GetMaskLinearBlurEnabled()
{
    return true;
}

bool RSSystemProperties::GetDebugTraceEnabled()
{
    return false;
}

bool RSSystemProperties::FindNodeInTargetList(std::string node)
{
    return false;
}

bool RSSystemProperties::IsFoldScreenFlag()
{
    return false;
}

bool RSSystemProperties::GetKawaseEnabled()
{
    return false;
}

float RSSystemProperties::GetKawaseRandomColorFactor()
{
    return 0.f;
}

bool RSSystemProperties::GetRandomColorEnabled()
{
    return false;
}

bool RSSystemProperties::GetKawaseOriginalEnabled()
{
    return false;
}

bool RSSystemProperties::GetBlurEnabled()
{
    return true;
}

const std::vector<float>& RSSystemProperties::GetAiInvertCoef()
{
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = {0.0, 1.0, 0.55, 0.4, 1.6, 45.0};
    return aiInvertCoef;
}

bool RSSystemProperties::GetBoolSystemProperty(const char* name, bool defaultValue)
{
    return {};
}

int RSSystemProperties::WatchSystemProperty(const char* name, OnSystemPropertyChanged func, void* context)
{
    return {};
}

bool RSSystemProperties::GetAFBCEnabled()
{
    return {};
}

bool RSSystemProperties::GetASTCEnabled()
{
    return false;
}

bool RSSystemProperties::GetCachedBlurPartialRenderEnabled()
{
    return false;
}

bool RSSystemProperties::GetImageGpuResourceCacheEnable(int width, int height)
{
    return false;
}

bool RSSystemProperties::IsPhoneType()
{
    return false;
}

bool RSSystemProperties::IsPcType()
{
    return false;
}

bool RSSystemProperties::GetSyncTransactionEnabled()
{
    return false;
}

int RSSystemProperties::GetSyncTransactionWaitDelay()
{
    return 0;
}

bool RSSystemProperties::GetSingleFrameComposerEnabled()
{
    return false;
}

bool RSSystemProperties::GetSingleFrameComposerCanvasNodeEnabled()
{
    return false;
}

bool RSSystemProperties::GetSubSurfaceEnabled()
{
    return false;
}
bool RSSystemProperties::GetSecurityPermissionCheckEnabled()
{
    return false;
}

bool RSSystemProperties::GetEffectMergeEnabled()
{
    return true;
}

bool RSSystemProperties::GetDumpUICaptureEnabled()
{
    return false;
}

bool RSSystemProperties::GetDumpUIPixelmapEnabled()
{
    return false;
}

uint32_t RSSystemProperties::GetVirtualScreenScaleModeDFX()
{
    return {};
}

bool RSSystemProperties::GetHwcRegionDfxEnabled()
{
    return false;
}

bool RSSystemProperties::GetUIFirstForceEnabled()
{
    return false;
}
} // namespace Rosen
} // namespace OHOS
