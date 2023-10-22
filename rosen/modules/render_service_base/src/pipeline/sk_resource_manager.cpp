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

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_task_dispatcher.h"

namespace OHOS::Rosen {
SKResourceManager& SKResourceManager::Instance()
{
    static SKResourceManager instance;
    return instance;
}

void SKResourceManager::HoldResource(sk_sp<SkRefCntBase> res)
{
#ifdef ROSEN_OHOS
    auto tid = gettid();
    if (!RSTaskDispatcher::GetInstance().HasRegisteredTask(tid)) {
        return;
    }
    std::scoped_lock<std::recursive_mutex> lock(purgeablemutex_);
    noPurgeableResource_[tid].emplace_back(res);
#endif
}

void SKResourceManager::ReleaseResource()
{
#ifdef ROSEN_OHOS
    RS_OPTIONAL_TRACE_NAME("SKResourceManager::ReleaseResource");
    std::scoped_lock<std::recursive_mutex> lock(purgeablemutex_);
    for (auto& resources : noPurgeableResource_) {
        if (resources.second.size() == 0) {
            continue;
        }
        auto tid = resources.first;
        std::vector<sk_sp<SkRefCntBase>>::iterator iter;
        for (iter = resources.second.begin(); iter != resources.second.end();) {
            if (*iter == nullptr) {
                iter = resources.second.erase(iter);
                continue;
            }
            if ((*iter)->unique()) {
                sk_sp<SkRefCntBase> it = *iter;
                iter = resources.second.erase(iter);
                {
                    std::scoped_lock<std::recursive_mutex> lock(noPureablemutex_);
                    purgeableResource_[tid].push(std::move(it));
                }
            } else {
                iter++;
            }
        }
        if (purgeableResource_[tid].size() > 0) {
            RSTaskDispatcher::GetInstance().PostTask(resources.first, [this] () {
                auto tid = gettid();
                {
                    RS_OPTIONAL_TRACE_NAME("SKResourceManager::purgeableResource");
                    std::scoped_lock<std::recursive_mutex> lock(noPureablemutex_);
                    while (purgeableResource_[tid].size() > 0) {
                        auto res = purgeableResource_[tid].front();
                        purgeableResource_[tid].pop();
                        res = nullptr;
                    }
                }
            });
        }
    }
#endif
}
} // OHOS::Rosen