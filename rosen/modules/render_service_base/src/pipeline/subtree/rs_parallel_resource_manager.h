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
#ifndef RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_RESOURCE_MANAGER_H
#define RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_RESOURCE_MANAGER_H

#include <list>
#include <map>
#include <mutex>
#include <set>
#include <unistd.h>

using OnMigrateFunc = std::function<void()>;
using MigrateFunc = std::function<void(void*, const OnMigrateFunc& func)>;
using OnFailedFunc = std::function<std::string()>;
using FailedFunc = std::function<void(const OnFailedFunc& func)>;
using DebugCallback = std::function<void(const std::string&)>;
using CheckFunc = std::function<bool()>;

//Release Migration
namespace OHOS::Rosen {
//Foward
class RSGPUResourceManager;
struct RecycleBucket;
namespace Drawing {
    class GPUContext;
    class Image;
}

//TypeDef
using ThreadTag = int;
using Resource = OnMigrateFunc;
using Container = std::list<Resource>;
using ContainerPtr = std::unique_ptr<Container>;
using BucketMapping = std::map<ThreadTag, RecycleBucket>;
using MigrateMapping = std::map<ThreadTag, MigrateFunc>;
using ImagePtr = std::shared_ptr<Drawing::Image>;
using ContextPtr = std::shared_ptr<Drawing::GPUContext>;

//Resource Holder
class RSB_EXPORT RSParallelResourceManager final {
    public:
    static RSParallelResourceManager& Singleton();
    void RegisterMigrate(Drawing::GPUContext* gpuctx, const MigrateFunc& migrate, bool force = false);
    void UnRegisterMigrate(Drawing::GPUContext* gpuctx);
    void ReleaseResource();
    ImagePtr BuildFromTextureByRef(const ImagePtr& ref, ContextPtr& newCtx, Drawing::BackendTexture& backendTexture);
    ImagePtr GenerateSharedImageForDraw(const Drawing::Image& ref, ContextPtr& newCtx, bool isOriginTop = true);

private:
    RSParallelResourceManager();
    ~RSParallelResourceManager() = default;
    RSParallelResourceManager(const RSParallelResourceManager &) = delete;
    RSParallelResourceManager(const RSParallelResourceManager &&) = delete;
    RSParallelResourceManager &operator = (const RSParallelResourceManager &) = delete;
    RSParallelResourceManager &operator = (const RSParallelResourceManager &&) = delete;

    friend class RSParallelSubmitManager;
    friend class RSGPUResourceManager;

    const std::unique_ptr<RSGPUResourceManager> resManager_;
};

}
#endif
