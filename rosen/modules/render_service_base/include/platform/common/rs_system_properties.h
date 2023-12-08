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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H
#define RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H

#include <atomic>
#include <string>
#include <vector>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum class DirtyRegionDebugType {
    DISABLED = 0,
    CURRENT_SUB,
    CURRENT_WHOLE,
    MULTI_HISTORY,
    CURRENT_SUB_AND_WHOLE,
    CURRENT_WHOLE_AND_MULTI_HISTORY,
    EGL_DAMAGE,
    DISPLAY_DIRTY,
    CUR_DIRTY_DETAIL_ONLY_TRACE = 10,
    UPDATE_DIRTY_REGION,
    OVERLAY_RECT,
    FILTER_RECT,
    SHADOW_RECT,
    PREPARE_CLIP_RECT,
    REMOVE_CHILD_RECT,
    RENDER_PROPERTIES_RECT,
    CANVAS_NODE_SKIP_RECT,
};

enum class SurfaceRegionDebugType {
    DISABLED = 0,
    OPAQUE_REGION,
    VISIBLE_REGION,
};

enum class PartialRenderType {
    DISABLED = 0,                               // 0, disable partial render
    SET_DAMAGE,                                 // 1, set damageregion, without draw_op dropping
    SET_DAMAGE_AND_DROP_OP,                     // 2, drop draw_op if node is not in dirty region
    SET_DAMAGE_AND_DROP_OP_OCCLUSION,           // 3, drop draw_op if node is not in visible region (unirender)
    SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY     // 4, drop draw_op if node is not in visible dirty region (unirender)
};

enum class DumpSurfaceType {
    DISABLED = 0,
    SINGLESURFACE,
    ALLSURFACES,
    PIXELMAP,
};

enum class ParallelRenderingType {
    AUTO = 0,
    DISABLE = 1,
    ENABLE = 2
};

enum class HgmRefreshRates {
    SET_RATE_NULL = 0,
    SET_RATE_30 = 30,
    SET_RATE_60 = 60,
    SET_RATE_90 = 90,
    SET_RATE_120 = 120
};

enum class HgmRefreshRateModes {
    SET_RATE_MODE_AUTO = -1,
    SET_RATE_MODE_NULL = 0,
    SET_RATE_MODE_LOW = 1,
    SET_RATE_MODE_MEDIUN = 2,
    SET_RATE_MODE_HIGH = 3
};

enum class GpuApiType {
    OPENGL = 0,
    VULKAN,
    DDGR,
};

using OnSystemPropertyChanged = void(*)(const char*, const char*, void*);

class RSB_EXPORT RSSystemProperties final {
public:
    ~RSSystemProperties() = default;

    // used by clients
    static std::string GetRecordingFile();
    static bool IsSceneBoardEnabled();
    static int GetDumpFrameNum();
    static void SetRecordingDisenabled();
    static int GetRecordingEnabled();

    static bool GetUniRenderEnabled();
    static bool GetRenderNodeTraceEnabled();
    static bool GetDrawOpTraceEnabled();
    static DirtyRegionDebugType GetDirtyRegionDebugType();
    static PartialRenderType GetPartialRenderEnabled();
    static PartialRenderType GetUniPartialRenderEnabled();
    static bool GetOcclusionEnabled();
    static std::string GetRSEventProperty(const std::string &paraName);
    static bool GetDirectClientCompEnableStatus();
    static bool GetHighContrastStatus();
    static uint32_t GetCorrectionMode();
    static DumpSurfaceType GetDumpSurfaceType();
    static long long int GetDumpSurfaceId();
    static bool GetTargetDirtyRegionDfxEnabled(std::vector<std::string>& dfxTargetSurfaceNames_);
    static bool GetOpaqueRegionDfxEnabled();
    static bool GetVisibleRegionDfxEnabled();
    static SurfaceRegionDebugType GetSurfaceRegionDfxType();
    static bool GetDumpLayersEnabled();
    static bool GetHardwareComposerEnabled();
    static bool GetAFBCEnabled();
    static bool GetReleaseResourceEnabled();
    static bool GetRSScreenRoundCornerEnable();

    static void SetDrawTextAsBitmap(bool flag);
    static bool GetDrawTextAsBitmap();
    static void SetCacheEnabledForRotation(bool flag);
    static bool GetCacheEnabledForRotation();
    static ParallelRenderingType GetPrepareParallelRenderingEnabled();
    static ParallelRenderingType GetParallelRenderingEnabled();
    static HgmRefreshRates GetHgmRefreshRatesEnabled();
    static void SetHgmRefreshRateModesEnabled(std::string param);
    static HgmRefreshRateModes GetHgmRefreshRateModesEnabled();

    static float GetAnimationScale();
    static bool GetProxyNodeDebugEnabled();
    static bool GetFilterCacheEnabled();
    static int GetFilterCacheUpdateInterval();
    static int GetFilterCacheSizeThreshold();
    static bool GetFilterPartialRenderEnabled();
    static bool GetColorPickerPartialEnabled();
    static bool GetMaskLinearBlurEnabled();
    static bool GetKawaseEnabled();
    static float GetKawaseRandomColorFactor();
    static bool GetRandomColorEnabled();
    static bool GetKawaseOriginalEnabled();
    static bool GetBlurEnabled();
    static bool GetSkipForAlphaZeroEnabled();
    static bool GetSkipGeometryNotChangeEnabled();
    static bool GetPropertyDrawableEnable();

    static bool GetAnimationCacheEnabled();

    static bool GetBoolSystemProperty(const char* name, bool defaultValue);
    static int WatchSystemProperty(const char* name, OnSystemPropertyChanged func, void* context);
    static bool GetUIFirstEnabled();
    static bool GetDebugTraceEnabled();
    static bool FindNodeInTargetList(std::string node);
    static bool IsFoldScreenFlag();
    static bool GetCacheCmdEnabled();
    static bool GetASTCEnabled();
    static bool GetImageGpuResourceCacheEnable(int width, int height);
#if defined (ENABLE_DDGR_OPTIMIZE)
    static bool GetDDGRIntegrateEnable();
#endif
    static bool GetSnapshotWithDMAEnabled();
    static bool IsPhoneType();
    static bool GetSyncTransactionEnabled();
    static int GetSyncTransactionWaitDelay();
    static bool GetUseShadowBatchingEnabled();
    static bool GetSingleFrameComposerEnabled();
    static bool GetSingleFrameComposerCanvasNodeEnabled();
    static bool GetSubSurfaceEnabled();

    static inline GpuApiType GetGpuApiType()
    {
        return RSSystemProperties::systemGpuApiType_;
    }

    static inline GpuApiType GetDefaultHiGpuV200Platform()
    {
        return RSSystemProperties::DefaultHiGpuV200Platform_;
    }

    static inline bool GetAceVulkanEnabled() noexcept
    {
        return aceVulkanEnabled_;
    }

    static inline bool GetRsVulkanEnabled() noexcept
    {
        return rsVulkanEnabled_;
    }
private:
    RSSystemProperties() = default;

    static inline bool isUniRenderEnabled_ = false;
    inline static bool isDrawTextAsBitmap_ = false;
    inline static bool cacheEnabledForRotation_ = false;
    static const GpuApiType systemGpuApiType_;
    static const GpuApiType DefaultHiGpuV200Platform_ = GpuApiType::OPENGL;
    static const bool aceVulkanEnabled_;
    static const bool rsVulkanEnabled_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H
