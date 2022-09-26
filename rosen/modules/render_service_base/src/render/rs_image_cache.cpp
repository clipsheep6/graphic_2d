/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "render/rs_image_cache.h"

#include "transaction/rs_render_service_client.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSImageCache& RSImageCache::Instance()
{
    static RSImageCache instance;
    return instance;
}

void RSImageCache::CacheSkiaImage(uint64_t uniqueId, sk_sp<SkImage> img)
{
    if (img) {
        skiaImageCache_.emplace(uniqueId, img);
    }
}

sk_sp<SkImage> RSImageCache::GetSkiaImageCache(uint64_t uniqueId) const
{
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end()) {
        return it->second;
    }
    return nullptr;
}

sk_sp<SkImage> RSImageCache::GetSkiaImageCacheAndResetQuery(uint64_t uniqueId)
{
    auto image = GetSkiaImageCache(uniqueId);
    // erase queryed status
    auto it = queryed_.find(uniqueId);
    if (it != queryed_.end()) {
        queryed_.erase(it);
    }
    return image;
}

std::vector<uint64_t> RSImageCache::QueryCacheValid(const std::vector<uint64_t>& toQuery)
{
    std::vector<uint64_t> valid;
    for (auto id : toQuery) {
        if (skiaImageCache_.find(id) != skiaImageCache_.end()) {
            valid.emplace_back(id);
            queryed_.emplace(id);
        }
    }
    return valid;
}

void RSImageCache::ReleaseSkiaImageCache(uint64_t uniqueId)
{
    // release the skimage if nobody else holds it
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end() && (!it->second || it->second->unique()) &&
        queryed_.find(uniqueId) == queryed_.end()) {
        skiaImageCache_.erase(it);
    }
}

// class RSResCacheQuery
RSResCacheQuery& RSResCacheQuery::Instance()
{
    static RSResCacheQuery instance;
    return instance;
}

void RSResCacheQuery::AddCacheId(uint64_t uniqueId)
{
    if (uniqueId != 0) {
        needQuery_.emplace(uniqueId);
    }
}

bool RSResCacheQuery::IsCached(uint64_t uniqueId)
{
    auto it = queryed_.find(uniqueId);
    return it != queryed_.end();
}

void RSResCacheQuery::ClearQueryed()
{
    queryed_.clear();
}

void RSResCacheQuery::QueryCacheValid()
{
    if (needQuery_.empty()) {
        return;
    }
    queryed_.clear();
    std::unordered_set<uint64_t> toQuery;
    toQuery.swap(needQuery_);
    std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())
        ->QueryCacheValid(toQuery, queryed_);
}
} // namespace Rosen
} // namespace OHOS
