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
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <unistd.h>

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkImage.h>
#else
#include "image/image.h"
#endif

#include "common/rs_macros.h"

namespace OHOS::Rosen {
class RSB_EXPORT SKResourceManager final {
public:
    static SKResourceManager& Instance();
#ifndef USE_ROSEN_DRAWING
    void HoldResource(sk_sp<SkImage> img);
#else
    void HoldResource(const std::shared_ptr<Drawing::Image> &img);
#endif
    void ReleaseResource();
private:
    SKResourceManager() = default;
    ~SKResourceManager() = default;

    std::recursive_mutex mutex_;
#ifndef USE_ROSEN_DRAWING
    std::map<pid_t, std::list<sk_sp<SkImage>>> skImages_;
#else
    std::map<pid_t, std::set<std::shared_ptr<Drawing::Image>>> images_;
#endif
};
} // OHOS::Rosen
#endif // SK_RESOURCE_MANAGER_H