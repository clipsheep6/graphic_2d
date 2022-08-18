/*
* Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_GLCACHE_H
#define OHOS_GLCACHE_H

#include <include/gpu/GrContext.h>
#include <include/gpu/GrContextOptions.h>
#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include "cache_data.h"

namespace OHOS {
namespace Rosen {
namespace GLCache {
class GLCache : public GrContextOptions::PersistentCache {
public:
    static GLCache& Instance();

    virtual void InitGLCache(const char *identity, ssize_t size);
    virtual void InitGLCache()
    {
        InitGLCache(nullptr, 0);
    }

    virtual void SetCacheDir(const std::string& filename);

    sk_sp<SkData> load(const SkData &key) override;
    void store(const SkData &key, const SkData &data) override;

private:
    GLCache();
    GLCache(const GLCache &) = delete;
    void operator=(const GLCache &) = delete;
    
    CacheData *GetCacheData()
    {
        return cacheData_.get();
    }
    
    void WriteToDisk();
    
    bool initialized_ = false;
    std::unique_ptr<CacheData> cacheData_;
    std::string cacheDir_;
    std::vector<uint8_t> idHash_;
    mutable std::mutex mutex_;

    bool savePending_ = false;
    unsigned int saveDelaySeconds_ = 3;
    
    size_t bufferSize_ = 16 * 1024;
    bool cacheDirty_ = false;
    
    static constexpr uint8_t ID_KEY = 0;
    
    static const size_t glslKeySize = 1024;
    static const size_t glslValueSize = 512 * 1024;
    static const size_t glslTotalSize = 2 * 1024 * 1024;
};
}   // namespace GLCache
}   // namespace Rosen
}   // namespace OHOS
#endif // OHOS_GLCACHE_H
