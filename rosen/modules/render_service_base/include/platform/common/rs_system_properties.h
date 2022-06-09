/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <set>
#include <string>

namespace OHOS {
namespace Rosen {
enum class UniRenderEnabledType {
    UNI_RENDER_DISABLED = 0,
    UNI_RENDER_ENABLED_FOR_ALL,
    UNI_RENDER_PARTIALLY_ENABLED,
};

enum class DirtyRegionDebugType {
    DISABLED = 0,
    CURRENT_SUB,
    CURRENT_WHOLE,
    MULTI_HISTORY,
    CURRENT_SUB_AND_WHOLE,
    CURRENT_WHOLE_AND_MULTI_HISTORY
};

class RSSystemProperties final {
public:
    ~RSSystemProperties() = default;

    static UniRenderEnabledType GetUniRenderEnabledType();
    static const std::set<std::string>& GetUniRenderEnabledList();
    static DirtyRegionDebugType GetDirtyRegionDebugType();

private:
    RSSystemProperties() = default;

    static inline UniRenderEnabledType uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    static inline std::set<std::string> uniRenderEnabledList_ { "clock0" };
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H
