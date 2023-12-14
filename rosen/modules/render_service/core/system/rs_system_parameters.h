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

#ifndef ROSEN_RENDER_SERVICE_CORE_SYSTEM_RS_SYSTEM_PARAMETERS_H
#define ROSEN_RENDER_SERVICE_CORE_SYSTEM_RS_SYSTEM_PARAMETERS_H

#include <atomic>
#include <string>
#include <vector>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum class QuickSkipPrepareType {
    DISABLED = 0,           // 0, disable quick skip preparation
    STATIC_PROCESS,         // 1, skip no-command process's preparation
    STATIC_APP_INSTANCE,    // 2, in case of dirty process, skip static app instance's preparation
    STATIC_CACHE,           // 3, in case of dirty instance, simplify static (drawing)cache's preparation
    STATIC_CACHE_SURFACE,   // 4, in case of dirty instance, simplify surface's static cache's preparation
};

class RSB_EXPORT RSSystemParameters final {
public:
    ~RSSystemParameters() = default;
    static bool GetCalcCostEnabled();
    static bool GetDrawingCacheEnabled();
    static bool GetDrawingCacheEnabledDfx();
    static bool GetShowRefreshRateEnabled();
    static QuickSkipPrepareType GetQuickSkipPrepareType();
    static bool GetVSyncControlEnabled();
    static bool GetSystemAnimatedScenesEnabled();
    static bool GetFilterCacheOcculusionEnabled();
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CORE_SYSTEM_RS_SYSTEM_PARAMETERS_H
