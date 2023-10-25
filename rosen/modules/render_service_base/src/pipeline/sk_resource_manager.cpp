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
#include "pipeline/sk_resource_manager.h"

#include "rs_trace.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_task_dispatcher.h"

namespace OHOS::Rosen {
SKResourceManager& SKResourceManager::Instance()
{
    static SKResourceManager instance;
    return instance;
}

void SKResourceManager::HoldResource(sk_sp<SkImage> img)
{
#ifdef ROSEN_OHOS
    auto tid = gettid();
    if (!RSTaskDispatcher::GetInstance().HasRegisteredTask(tid)) {
        return;
    }
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    skImages_[tid].push(img);
#endif
}

void SKResourceManager::ReleaseResource()
{
#ifdef ROSEN_OHOS
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    for (auto& skImages : skImages_) {
        if (skImages.second.size() > 0) {
            RSTaskDispatcher::GetInstance().PostTask(skImages.first, [this]() {
            auto tid = gettid();
            std::scoped_lock<std::recursive_mutex> lock(mutex_);
            size_t size = skImages_[tid].size();
            while (size-- > 0) {
                auto image = skImages_[tid].front();
                skImages_[tid].pop();
                if (image == nullptr) {
                    continue;
                }
                if (image->unique()) {
                    image = nullptr;
                } else {
                    skImages_[tid].push(image);
                }
            }
            });
        }
    }
#endif
}
} // OHOS::Rosen