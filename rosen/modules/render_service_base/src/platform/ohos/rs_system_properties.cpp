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

#include <cstdlib>
#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "scene_board_judgement.h"

namespace OHOS {
namespace Rosen {

constexpr int DEFAULT_CACHE_WIDTH = 1344;
constexpr int DEFAULT_CACHE_HEIGHT = 2772;

static void ParseDfxSurfaceNamesString(const std::string& paramsStr,
    std::vector<std::string>& splitStrs, const std::string& seperator)
{
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = paramsStr.find(seperator);
    if (std::string::npos == pos2) {
        splitStrs.push_back(paramsStr);
        return;
    }
    while (std::string::npos != pos2) {
        splitStrs.push_back(paramsStr.substr(pos1, pos2 - pos1));
        pos1 = pos2 + seperator.size();
        pos2 = paramsStr.find(seperator, pos1);
    }
    if (pos1 != paramsStr.length()) {
        splitStrs.push_back(paramsStr.substr(pos1));
    }
}

bool RSSystemProperties::IsSceneBoardEnabled()
{
    return SceneBoardJudgement::IsSceneBoardEnabled();
}

// used by clients
int RSSystemProperties::GetDumpFrameNum()
{
    static CachedHandle g_Handle = CacheParameterCreate("debug.graphic.recording.frameNum", "0");
    int changed = 0;
    const char *num = CacheParameterGetChanged(g_Handle, &changed);
    // return std::atoi((system::GetParameter("debug.graphic.recording.frameNum", "0")).c_str());
    return std::atoi(num);
}

bool RSSystemProperties::GetRecordingEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("debug.graphic.recording.enabled", "0");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    // return (system::GetParameter("debug.graphic.recording.enabled", "0") != "0");
    return std::atoi(enable) != 0;
}
}

void RSSystemProperties::SetRecordingDisenabled()
{
    system::SetParameter("debug.graphic.recording.enabled", "0");
    RS_LOGD("RSSystemProperties::SetRecordingDisenabled");
}

std::string RSSystemProperties::GetRecordingFile()
{
    static CachedHandle g_Handle = CacheParameterCreate("debug.graphic.dumpfile.path", "");
    int changed = 0;
    // return system::GetParameter("debug.graphic.dumpfile.path", "");
    return CacheParameterGetChanged(g_Handle, &changed);
}

bool RSSystemProperties::GetUniRenderEnabled()
{
    static bool inited = false;
    if (inited) {
        return isUniRenderEnabled_;
    }

    isUniRenderEnabled_ = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())
        ->GetUniRenderEnabled();
    inited = true;
    ROSEN_LOGI("RSSystemProperties::GetUniRenderEnabled:%{public}d", isUniRenderEnabled_);
    return isUniRenderEnabled_;
}

bool RSSystemProperties::GetDrawOpTraceEnabled()
{
    static bool code = system::GetParameter("persist.rosen.drawoptrace.enabled", "0") != "0";
    return code;
}

bool RSSystemProperties::GetRenderNodeTraceEnabled()
{
    static bool isNeedTrace = system::GetParameter("persist.rosen.rendernodetrace.enabled", "0") != "0";
    return isNeedTrace;
}

DirtyRegionDebugType RSSystemProperties::GetDirtyRegionDebugType()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.dirtyregiondebug.enabled", "0");
    int changed = 0;
    const char *type = CacheParameterGetChanged(g_Handle, &changed);
    return static_cast<DirtyRegionDebugType>(std::atoi(type));
}

PartialRenderType RSSystemProperties::GetPartialRenderEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.partialrender.enabled", "2");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return static_cast<PartialRenderType>(std::atoi(enable));
}

PartialRenderType RSSystemProperties::GetUniPartialRenderEnabled()
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_VK)
    static CachedHandle g_Handle = CacheParameterCreate("rosen.uni.partialrender.enabled", "0");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return static_cast<PartialRenderType>(std::atoi(enable));
#else
    static CachedHandle g_Handle = CacheParameterCreate("rosen.uni.partialrender.enabled", "4");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return static_cast<PartialRenderType>(std::atoi(enable));
#endif
}

ReleaseGpuResourceType RSSystemProperties::GetReleaseGpuResourceEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("persist.release.gpuresource.enabled", "2");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return static_cast<ReleaseGpuResourceType>(std::atoi(enable));
}

bool RSSystemProperties::GetOcclusionEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.occlusion.enabled", "1");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(enable) != 0;
}

bool RSSystemProperties::GetHardwareComposerEnabled()
{
    static bool hardwareComposerEnabled = system::GetParameter(
        "persist.rosen.hardwarecomposer.enabled", "1") != "0";
    return hardwareComposerEnabled;
}

bool RSSystemProperties::GetAFBCEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.afbc.enabled", "1");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(enable) != 0;
}

std::string RSSystemProperties::GetRSEventProperty(const std::string &paraName)
{
    return system::GetParameter(paraName, "0");
}

bool RSSystemProperties::GetDirectClientCompEnableStatus()
{
    // If the value of rosen.directClientComposition.enabled is not 0 then enable the direct CLIENT composition.
    // Direct CLIENT composition will be processed only when the num of layer is larger than 11
    static CachedHandle g_Handle = CacheParameterCreate("rosen.directClientComposition.enabled", "1");
    int changed = 0;
    const char *status = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(status) != 0;
}

bool RSSystemProperties::GetHighContrastStatus()
{
    // If the value of rosen.directClientComposition.enabled is not 0 then enable the direct CLIENT composition.
    // Direct CLIENT composition will be processed only when the num of layer is larger than 11
    static CachedHandle g_Handle = CacheParameterCreate("rosen.HighContrast.enabled", "0");
    int changed = 0;
    const char *status = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(status) != 0;
    // return std::atoi((system::GetParameter("rosen.HighContrast.enabled", "0")).c_str()) != 0;
}

bool RSSystemProperties::GetTargetDirtyRegionDfxEnabled(std::vector<std::string>& dfxTargetSurfaceNames_)
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.dirtyregiondebug.surfacenames", "0");
    int changed = 0;
    const char *targetSurfacesStr = CacheParameterGetChanged(g_Handle, &changed);
    // std::string targetSurfacesStr = system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
    if (std::atoi(targetSurfacesStr) == 0) {
        dfxTargetSurfaceNames_.clear();
        return false;
    }
    dfxTargetSurfaceNames_.clear();
    ParseDfxSurfaceNamesString(targetSurfacesStr, dfxTargetSurfaceNames_, ",");
    return true;
}

bool RSSystemProperties::GetOpaqueRegionDfxEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.uni.opaqueregiondebug", "0");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    // return std::atoi((system::GetParameter("rosen.uni.opaqueregiondebug", "0")).c_str()) != 0;
    return std::atoi(enable) != 0;
}

bool RSSystemProperties::GetVisibleRegionDfxEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.uni.visibleregiondebug", "0");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(enable) != 0;
    // return std::atoi((system::GetParameter("rosen.uni.visibleregiondebug", "0")).c_str()) != 0;
}

SurfaceRegionDebugType RSSystemProperties::GetSurfaceRegionDfxType()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.uni.surfaceregiondebug", "0");
    int changed = 0;
    const char *type = CacheParameterGetChanged(g_Handle, &changed);
    return static_cast<SurfaceRegionDebugType>(std::atoi(type));
}

uint32_t RSSystemProperties::GetCorrectionMode()
{
    // If the value of rosen.directClientComposition.enabled is not 0 then enable the direct CLIENT composition.
    // Direct CLIENT composition will be processed only when the num of layer is larger than 11
    static CachedHandle g_Handle = CacheParameterCreate("rosen.CorrectionMode", "999");
    int changed = 0;
    const char *mode = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(mode);
}

DumpSurfaceType RSSystemProperties::GetDumpSurfaceType()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.dumpsurfacetype.enabled", "0");
    int changed = 0;
    const char *type = CacheParameterGetChanged(g_Handle, &changed);
    return static_cast<DumpSurfaceType>(std::atoi(type));
}

long long int RSSystemProperties::GetDumpSurfaceId()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.dumpsurfaceid", "0");
    int changed = 0;
    const char *surfaceId = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoll(surfaceId);
}

bool RSSystemProperties::GetDumpLayersEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.dumplayer.enabled", "0");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(enable) != 0;
}

void RSSystemProperties::SetDrawTextAsBitmap(bool flag)
{
    isDrawTextAsBitmap_ = flag;
}
bool RSSystemProperties::GetDrawTextAsBitmap()
{
    return isDrawTextAsBitmap_;
}

void RSSystemProperties::SetCacheEnabledForRotation(bool flag)
{
    cacheEnabledForRotation_ = flag;
}

bool RSSystemProperties::GetCacheEnabledForRotation()
{
    return cacheEnabledForRotation_;
}

ParallelRenderingType RSSystemProperties::GetPrepareParallelRenderingEnabled()
{
    static ParallelRenderingType systemPropertiePrepareType = static_cast<ParallelRenderingType>(
        std::atoi((system::GetParameter("persist.rosen.prepareparallelrender.enabled", "1")).c_str()));
    return systemPropertiePrepareType;
}

ParallelRenderingType RSSystemProperties::GetParallelRenderingEnabled()
{
    static ParallelRenderingType systemPropertieType = static_cast<ParallelRenderingType>(
        std::atoi((system::GetParameter("persist.rosen.parallelrender.enabled", "0")).c_str()));
    return systemPropertieType;
}

HgmRefreshRates RSSystemProperties::GetHgmRefreshRatesEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.sethgmrefreshrate.enabled", "0");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return static_cast<HgmRefreshRates>(std::atoi(enable));
}

void RSSystemProperties::SetHgmRefreshRateModesEnabled(std::string param)
{
    system::SetParameter("persist.rosen.sethgmrefreshratemode.enabled", param);
    RS_LOGD("RSSystemProperties::SetHgmRefreshRateModesEnabled set to %{public}s", param.c_str());
}

HgmRefreshRateModes RSSystemProperties::GetHgmRefreshRateModesEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("persist.rosen.sethgmrefreshratemode.enabled", "0");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return static_cast<HgmRefreshRateModes>(std::atoi(enable));
}

bool RSSystemProperties::GetSkipForAlphaZeroEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("persist.skipForAlphaZero.enabled", "1");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(enable) != 0;
}

bool RSSystemProperties::GetSkipGeometryNotChangeEnabled()
{
    static bool skipGeoNotChangeEnabled =
        std::atoi((system::GetParameter("persist.skipGeometryNotChange.enabled", "1")).c_str()) != 0;
    return skipGeoNotChangeEnabled;
}

bool RSSystemProperties::GetPropertyDrawableEnable()
{
    static bool propertyDrawableEnable =
        std::atoi((system::GetParameter("persist.propertyDrawableGenerate.enabled", "0")).c_str()) != 0;
    return propertyDrawableEnable;
}

float RSSystemProperties::GetAnimationScale()
{
    static CachedHandle g_Handle = CacheParameterCreate("persist.sys.graphic.animationscale", "1.0");
    int changed = 0;
    const char *scale = CacheParameterGetChanged(g_Handle, &changed);
    return std::atof(scale);
}

bool RSSystemProperties::GetFilterCacheEnabled()
{
    // Determine whether the filter cache should be enabled. The default value is 1, which means that it is enabled.
    // If dirty-region is not properly implemented, the filter cache will act as a skip-frame strategy for filters.
    static bool filterCacheEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.filterCacheEnabled", "1")).c_str()) != 0;
    return filterCacheEnabled;
}

int RSSystemProperties::GetFilterCacheUpdateInterval()
{
    // Configure whether to enable skip-frame for the filter cache. The default value is 1, which means that the cached
    // image is updated with a delay of 1 frame.
    static int filterCacheUpdateInterval =
        std::atoi((system::GetParameter("persist.sys.graphic.filterCacheUpdateInterval", "1")).c_str());
    return filterCacheUpdateInterval;
}

int RSSystemProperties::GetFilterCacheSizeThreshold()
{
    // Set the minimum size for enabling skip-frame in the filter cache. By default, this value is 400, which means that
    // skip-frame is only enabled for regions where both the width and height are greater than 400.
    static int filterCacheSizeThreshold =
        std::atoi((system::GetParameter("persist.sys.graphic.filterCacheSizeThreshold", "400")).c_str());
    return filterCacheSizeThreshold;
}

bool RSSystemProperties::GetFilterPartialRenderEnabled()
{
    // Determine whether the filter partial render should be enabled. The default value is 0,
    // which means that it is unenabled.
    static bool enabled =
        std::atoi((system::GetParameter("persist.sys.graphic.filterPartialRenderEnabled", "0")).c_str()) != 0;
    return enabled;
}

bool RSSystemProperties::GetKawaseEnabled()
{
    static bool kawaseBlurEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.kawaseEnable", "1")).c_str()) != 0;
    return kawaseBlurEnabled;
}

float RSSystemProperties::GetKawaseRandomColorFactor()
{
    static float randomFactor =
        std::atof((system::GetParameter("persist.sys.graphic.kawaseFactor", "1.75")).c_str());
    return randomFactor;
}

bool RSSystemProperties::GetRandomColorEnabled()
{
    static bool randomColorEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.randomColorEnable", "1")).c_str()) != 0;
    return randomColorEnabled;
}

bool RSSystemProperties::GetKawaseOriginalEnabled()
{
    static bool kawaseOriginalEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.kawaseOriginalEnable", "0")).c_str()) != 0;
    return kawaseOriginalEnabled;
}

bool RSSystemProperties::GetBlurEnabled()
{
    static bool blurEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.blurEnabled", "1")).c_str()) != 0;
    return blurEnabled;
}

bool RSSystemProperties::GetProxyNodeDebugEnabled()
{
    static bool proxyNodeDebugEnabled = system::GetParameter("persist.sys.graphic.proxyNodeDebugEnabled", "0") != "0";
    return proxyNodeDebugEnabled;
}

bool RSSystemProperties::GetUIFirstEnabled()
{
#ifdef ROSEN_EMULATOR
    return false;
#else
    static CachedHandle g_Handle = CacheParameterCreate("rosen.ui.first.enabled", "1");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(enable) != 0;
#endif
}

bool RSSystemProperties::GetDebugTraceEnabled()
{
    static bool openDebugTrace =
        std::atoi((system::GetParameter("persist.sys.graphic.openDebugTrace", "0")).c_str()) != 0;
    return openDebugTrace;
}

bool RSSystemProperties::FindNodeInTargetList(std::string node)
{
    static std::string targetStr = system::GetParameter("persist.sys.graphic.traceTargetList", "");
    static auto strSize = targetStr.size();
    if (strSize == 0) {
        return false;
    }
    static std::vector<std::string> targetVec;
    static bool loaded = false;
    if (!loaded) {
        const std::string pattern = ";";
        targetStr += pattern;
        strSize = targetStr.size();
        std::string::size_type pos;
        for (std::string::size_type i = 0; i < strSize; i++) {
            pos = targetStr.find(pattern, i);
            if (pos >= strSize) {
                break;
            }
            auto str = targetStr.substr(i, pos - i);
            if (str.size() > 0) {
                targetVec.emplace_back(str);
            }
            i = pos;
        }
        loaded = true;
    }
    bool res = std::find(targetVec.begin(), targetVec.end(), node) != targetVec.end();
    return res;
}

bool RSSystemProperties::GetCacheCmdEnabled()
{
    static CachedHandle g_Handle = CacheParameterCreate("rosen.cacheCmd.enabled", "1");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(enable) != 0;
}

bool RSSystemProperties::GetASTCEnabled()
{
    static bool isASTCEnabled = std::atoi((system::GetParameter("persist.rosen.astc.enabled", "0")).c_str()) != 0;
    return isASTCEnabled;
}

bool RSSystemProperties::GetImageGpuResourceCacheEnable(int width, int height)
{
    static bool cacheEnable =
        std::atoi((system::GetParameter("persist.sys.graphic.gpuResourceCacheEnable", "1")).c_str()) != 0;
    if (!cacheEnable) {
        return false;
    }

    // default cache full screen image gpu resource.
    static int widthConfig =
        std::atoi((system::GetParameter("persist.sys.graphic.gpuResourceCacheWidth", "0")).c_str());
    static int heightConfig =
        std::atoi((system::GetParameter("persist.sys.graphic.gpuResourceCacheHeight", "0")).c_str());
    int cacheWidth = widthConfig == 0 ? DEFAULT_CACHE_WIDTH : widthConfig;
    int cacheHeight = heightConfig == 0 ? DEFAULT_CACHE_HEIGHT : heightConfig;
    if (width >= cacheWidth && height >= cacheHeight) {
        return true;
    }
    return false;
}

bool RSSystemProperties::GetBoolSystemProperty(const char* name, bool defaultValue)
{
    static CachedHandle g_Handle = CacheParameterCreate(name, defaultValue ? "1" : "0");
    int changed = 0;
    const char *enable = CacheParameterGetChanged(g_Handle, &changed);
    return std::atoi(enable) != 0;
}

int RSSystemProperties::WatchSystemProperty(const char* name, OnSystemPropertyChanged func, void* context)
{
    return WatchParameter(name, func, context);
}
#if defined (ENABLE_DDGR_OPTIMIZE)
bool RSSystemProperties::GetDDGRIntegrateEnable()
{
    static bool isDataStEnable =
        std::atoi((system::GetParameter("ddgr.data.st.enable", "1")).c_str()) != 0;
    return isDataStEnable;
}
#endif

bool RSSystemProperties::GetSnapshotWithDMAEnabled()
{
    static bool isSupportDma = system::GetParameter("const.product.devicetype", "pc") == "phone" ||
        system::GetParameter("const.product.devicetype", "pc") == "tablet" ||
        system::GetParameter("const.product.devicetype", "pc") == "pc";
    return isSupportDma && system::GetBoolParameter("rosen.snapshotDma.enabled", true);
}

bool RSSystemProperties::IsPhoneType()
{
    static bool isPhone = system::GetParameter("const.product.devicetype", "pc") == "phone";
    return isPhone;
}

bool RSSystemProperties::GetSyncTransactionEnabled()
{
    static bool syncTransactionEnabled =
        std::atoi((system::GetParameter("persist.sys.graphic.syncTransaction.enabled", "1")).c_str()) != 0;
    return syncTransactionEnabled;
}

int RSSystemProperties::GetSyncTransactionWaitDelay()
{
    static int syncTransactionWaitDelay =
        std::atoi((system::GetParameter("persist.sys.graphic.syncTransactionWaitDelay", "100")).c_str());
    return syncTransactionWaitDelay;
}
} // namespace Rosen
} // namespace OHOS
