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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_CACHE_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_CACHE_H

#include <unordered_map>
#include <unordered_set>
#include "include/core/SkImage.h"

namespace OHOS {
namespace Rosen {
class RSImageCache {
public:
    static RSImageCache& Instance();

    void CacheSkiaImage(uint64_t uniqueId, sk_sp<SkImage> img);
    sk_sp<SkImage> GetSkiaImageCache(uint64_t uniqueId) const;
    sk_sp<SkImage> GetSkiaImageCacheAndResetQuery(uint64_t uniqueId);
    std::vector<uint64_t> QueryCacheValid(const std::vector<uint64_t>& toQuery);
    void ReleaseSkiaImageCache(uint64_t uniqueId);

private:
    RSImageCache() = default;
    ~RSImageCache() = default;

    RSImageCache(const RSImageCache&) = delete;
    RSImageCache(const RSImageCache&&) = delete;
    RSImageCache& operator=(const RSImageCache&) = delete;
    RSImageCache& operator=(const RSImageCache&&) = delete;

    std::unordered_map<uint64_t, sk_sp<SkImage>> skiaImageCache_;
    std::unordered_set<uint64_t> queryed_;
};

class RSResCacheQuery {
public:
    static RSResCacheQuery& Instance();

    void AddCacheId(uint64_t uniqueId);
    bool IsCached(uint64_t uniqueId);
    void ClearQueryed();
    void QueryCacheValid();

private:
    RSResCacheQuery() = default;
    ~RSResCacheQuery() = default;

    RSResCacheQuery(const RSResCacheQuery&) = delete;
    RSResCacheQuery(const RSResCacheQuery&&) = delete;
    RSResCacheQuery& operator=(const RSResCacheQuery&) = delete;
    RSResCacheQuery& operator=(const RSResCacheQuery&&) = delete;

    std::unordered_set<uint64_t> needQuery_;
    std::unordered_set<uint64_t> queryed_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_CACHE_H
