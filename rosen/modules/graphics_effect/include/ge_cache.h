/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef GRAPHICS_EFFECT_GE_CACHE_H
#define GRAPHICS_EFFECT_GE_CACHE_H

#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "image/image.h"

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GECache {
public:
    static GECache& GetInstance();
    void Set(size_t key, const std::shared_ptr<Drawing::Image> value);
    const std::shared_ptr<Drawing::Image> Get(size_t key) const;
    void SetMaxCapacity(size_t size);
    void Clear();

private:
    GECache() = default;
    ~GECache() = default;
    GECache(const GECache&) = delete;
    GECache& operator=(const GECache&) = delete;

    void EvictLeastUsed();

    std::unordered_map<size_t, std::shared_ptr<Drawing::Image>> cache_;
    mutable std::mutex mutex_;

    size_t maxCapacity_ = 10; // Use 10 as the default capacity
    std::list<size_t> accessOrder_;
    std::unordered_set<size_t> keySet_;
};

} // namespace GraphicsEffectEngine
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_CACHE_H
