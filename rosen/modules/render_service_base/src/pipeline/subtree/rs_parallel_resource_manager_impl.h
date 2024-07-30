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

#ifndef RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_RESOURCE_MANAGER_IMPL_H
#define RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_RESOURCE_MANAGER_IMPL_H

#include "pipeline/subtree/rs_parallel_resource_manager.h"
#include "pipeline/subtree/rs_parallel_macro.h"
#include "platform/common/rs_system_properties.h"
#include "skia_adapter/skia_gpu_context.h"
#include "image/image.h"
#include <cassert>
#include <memory>
#include <list>
#include <map>
#include <sys/prctl.h>

namespace OHOS::Rosen {

static inline const char* GetThreadName()
{
    static constexpr int NAME_LEN = 16;
    static thread_local char threadName[NAME_LEN + 1] = "";
    if (threadName[0] == 0) {
        prctl(PR_GET_NAME, threadName);
        threadName[NAME_LEN] = 0;
    }
    return threadName;
}

static inline ThreadTag GetThreadTag()
{
    return gettid();
}

static thread_local bool isRegistered;

struct RecycleBucket {
    void Collect(const Resource& res);
    bool Exchange(ContainerPtr&& empty);

    ContainerPtr container_ { std::make_unique<Container>() };
};

class RSGPUResourceManager {
public:
    void RegisterMigrate(const MigrateFunc& migrate);
    void Collect(ThreadTag dst, const Resource& res);
    void Dispatch();
private:
    void DoRelease();

    std::mutex bucketMutex_;
    BucketMapping bucketMapping_ {};
    MigrateMapping migrateMapping_ {};
};

inline void RecycleBucket::Collect(const Resource& res)
{
    container_->push_back(res);
}

inline bool RecycleBucket::Exchange(ContainerPtr&& empty)
{
    if (container_->empty()) {
        return false;
    }
    container_.swap(empty);
    return true;
}

void RSGPUResourceManager::RegisterMigrate(const MigrateFunc& migrate)
{
    ThreadTag dst = GetThreadTag();
    if (migrate != nullptr) {
        migrateMapping_.insert({dst, migrate});
        isRegistered = true;
    }
}

void RSGPUResourceManager::Collect(ThreadTag dst, const Resource& res)
{
    std::unique_lock<std::mutex> lock(bucketMutex_);
    bucketMapping_[dst].Collect(res);
}

void RSGPUResourceManager::Dispatch()
{
    static const auto s_migrateFunc = [this] () {DoRelease(); };
    for (const auto& item :migrateMapping_) {
        item.second(nullptr, s_migrateFunc);
    }
}

inline void RSGPUResourceManager::DoRelease()
{
    static thread_local ThreadTag current = GetThreadTag();
    ContainerPtr empty = std::make_unique<Container>();
    bool needClear = false;
    {
        std::unique_lock<std::mutex> lock(bucketMutex_);
        auto found = bucketMapping_.find(current);
        needClear = found != bucketMapping_.end()
                    && found->second.Exchange(std::move(empty));
    }
    if (needClear) {
        SUBTREE_TRACE_NAME_FMT("Release: %d",  empty->size());
        for (auto& callback : *empty) {
            callback();
        }
        empty->clear();
    }
}

RSParallelResourceManager& RSParallelResourceManager::Singleton()
{
    static RSParallelResourceManager sInstance;
    return sInstance;
}

RSParallelResourceManager::RSParallelResourceManager()
    : resManager_(std::make_unique<RSGPUResourceManager>())
{
}
    void RSParallelResourceManager::RegisterMigrate(Drawing::GPUContext* gpuctx, const MigrateFunc& migrate, bool force)
    {
        auto ctx = gpuctx;
        if (ctx == nullptr) {
            return;
        }
        auto skctx = ctx->GetImpl<Drawing::SkiaGPUContext>();
        if (skctx == nullptr) {
            return ;
        }
        auto grctx = skctx->GetGrContext();
        if (grctx ==nullptr) {
            return;
        }

        ThreadTag threadTag = GetThreadTag();
        const std::string threadName(GetThreadName());

        resManager_->RegisterMigrate(migrate);

        auto migrateCallback = [this, threadTag](void* ctx, const Resource& res) {
            RS_LOGD("[%{public}s]: Migrate Collect Tid: [%{public}d]", GetThreadName(), threadTag);
            this->resManager_->Collect(threadTag, res);
        };
        grctx->registerMigrateCallback(migrateCallback, force);
        RS_LOGD("[%{public}s]: Register ReleaseMigrate Done", threadName.c_str());
#ifdef SUBTREE_PARALLEL_DEBUG

    auto debugCallback = [threadName, threadTag](const std::string& debugLog) {
    RS_LOGD("Resource[%{public}s] ReleaseMigrate: [%{public}s](%{public}d)->[%{public}s](%{public}d)",
        debugLog.c_str(), GetThreadName(), gettid(), threadName.c_str(), threadTag);
    };
    grctx->registerDebugCallback(debugCallback);
#endif
}

void RSParallelResourceManager::UnRegisterMigrate(Drawing::GPUContext* gpuctx)
{
    auto ctx = gpuctx;
    if (ctx == nullptr) {
        return;
    }
    auto skctx = ctx->GetImpl<Drawing::SkiaGPUContext>();
    if (skctx == nullptr) {
        return;
    }
    auto grctx = skctx->GetGrContext();
    if (grctx == nullptr) {
        return;
    }
    grctx->registerMigrateCallback(nullptr);
}

void RSParallelResourceManager::ReleaseResource()
{
    resManager_->Dispatch();
}

struct RSParallelResourceHolder {
    RSParallelResourceHolder(const std::shared_ptr<Drawing::Image>& sharedImage)
        : res_(sharedImage) {}
    const std::shared_ptr<Drawing::Image> res_;
};

static void ReleaseResourceHolder(void* context)
{
    RSParallelResourceHolder* cleanupHelper = static_cast<RSParallelResourceHolder*>(context);
    if (cleanupHelper != nullptr) {
        delete cleanupHelper;
    }
}

ImagePtr RSParallelResourceManager::BuildFromTextureByRef(const ImagePtr& ref, ContextPtr& newCtx)
{
    if (newCtx == nullptr || ref == nullptr || ref->IsValid(newCtx.get())) {
        return ref;
    }
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::TOP_LEFT;
    Drawing::BitmapFormat format = {ref->GetColorType(), ref->GetAlphaType() };
    if (!backendTexture.IsValid()) {
        return nullptr;
    }
    auto image = std::make_shared<Drawing::Image>();
    auto holder = new RSParallelResourceHolder(ref);
    if (!image->BuildFromTexture(*newCtx, backendTexture.GetTextureInfo(),
        origin, format, nullptr, ReleaseResourceHolder, holder)) {
        return nullptr;
    }
    return image;
}

struct RSParallelResourceHolder2
{
    RSParallelResourceHolder2(const Drawing::Image& sharedImage)
        : res_(sharedImage) {}
    const Drawing::Image res_;
};

static void ReleaseResourceHolder2(void* context)
{
    RSParallelResourceHolder2* cleanupHelper = static_cast<RSParallelResourceHolder2*>(context);
    if (cleanupHelper != nullptr) {
        delete cleanupHelper;
    }
}

ImagePtr RSParallelResourceManager::GenerateSharedImageForDraw(const Drawing::Image& ref,
    ContextPtr& newCtx, bool isOriginTop)
{
    if (newCtx == nullptr || ref.IsValid(newCtx.get())) {
        return nullptr;
    }
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::TOP_LEFT;
    if (!isOriginTop) {
        origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    }
    Drawing::BitmapFormat format = {ref.GetColorType(), ref.GetAlphaType()};
    auto backendTexture = ref.GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        return nullptr;
    }
    auto image = std::make_shared<Drawing::Image>();
    auto holder = new RSParallelResourceHolder2(ref);
    if (!image->BuildFromTexture(*newCtx, backendTexture.GetTextureInfo(),
        origin, format, nullptr, ReleaseResourceHolder2, holder)) {
            return nullptr;
        }
        return image;
}

}
#endif