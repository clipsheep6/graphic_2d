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

#include "rs_system_parameters.h"

#include <cstdlib>
#include <parameter.h>
#include <parameters.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
bool RSSystemParameters::GetCalcCostEnabled()
{
    return std::atoi((system::GetParameter("rosen.calcCost.enabled", "0")).c_str()) != 0;
}

bool RSSystemParameters::GetDrawingCacheEnabled()
{
    return std::atoi((system::GetParameter("rosen.drawingCache.enabled", "1")).c_str()) != 0;
}

bool RSSystemParameters::GetDrawingCacheEnabledDfx()
{
    return std::atoi((system::GetParameter("rosen.drawingCache.enabledDfx", "0")).c_str()) != 0;
}
} // namespace Rosen
} // namespace OHOS
