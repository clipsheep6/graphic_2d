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

#ifndef SK_RESOURCE_MANAGER_H
#define SK_RESOURCE_MANAGER_H
#include <map>
#include <mutex>
#include <queue>
#include <unistd.h>

#include "third_party/skia/include/core/SkRefCnt.h"

#include "common/rs_macros.h"

namespace OHOS::Rosen {
class RSB_EXPORT SKResourceManager final {
public:
    static SKResourceManager& Instance();
    void HoldResource(sk_sp<SkRefCntBase> res);
    void ReleaseResource();
private:
    SKResourceManager() = default;
    ~SKResourceManager() = default;

    SKResourceManager(const SKResourceManager&) = delete;
    SKResourceManager(const SKResourceManager&&) = delete;
    SKResourceManager& operator=(const SKResourceManager&) = delete;
    SKResourceManager& operator=(const SKResourceManager&&) = delete;

    std::recursive_mutex purgeablemutex_;
    std::recursive_mutex noPureablemutex_;
    std::map<pid_t, std::vector<sk_sp<SkRefCntBase>>> noPurgeableResource_;
    std::map<pid_t, std::queue<sk_sp<SkRefCntBase>>> purgeableResource_;
};
} // OHOS::Rosen
#endif // SK_RESOURCE_MANAGER_H