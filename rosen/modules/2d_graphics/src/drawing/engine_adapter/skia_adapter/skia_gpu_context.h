/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef SKIA_GPUCONTEXT_H
#define SKIA_GPUCONTEXT_H

#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#include "include/gpu/GrContextOptions.h"

#include "impl_interface/gpu_context_impl.h"
#include "image/gpu_context.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPersistentCache : public GrContextOptions::PersistentCache {
public:
    explicit SkiaPersistentCache(GPUContextOptions::PersistentCache* cache);
    ~SkiaPersistentCache() {}

    sk_sp<SkData> load(const SkData& key) override;
    void store(const SkData& key, const SkData& data) override;
private:
    GPUContextOptions::PersistentCache* cache_;
};

class SkiaGPUContext : public GPUContextImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaGPUContext();
    ~SkiaGPUContext() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    bool BuildFromGL(const GPUContextOptions& options) override;

#ifdef RS_ENABLE_VK
    bool BuildFromVK(const GrVkBackendContext& context) override;
#endif

    void Flush() override;
    void FlushAndSubmit(bool syncCpu) override;
    void Submit() override;
    void PerformDeferredCleanup(std::chrono::milliseconds msNotUsed) override;

    void GetResourceCacheLimits(int* maxResource, size_t* maxResourceBytes) const override;
    void SetResourceCacheLimits(int maxResource, size_t maxResourceBytes) override;

    void GetResourceCacheUsage(int* resourceCount, size_t* resourceBytes) const override;

    void FreeGpuResources() override;

    void DumpGpuStats(std::string& out) override;

    void ReleaseResourcesAndAbandonContext() override;

    void PurgeUnlockedResources(bool scratchResourcesOnly) override;

    void PurgeUnlockedResourcesByTag(bool scratchResourcesOnly, const GPUResourceTag &tag) override;

    void PurgeUnlockAndSafeCacheGpuResources() override;

    void ReleaseByTag(const GPUResourceTag &tag) override;

    void DumpMemoryStatisticsByTag(TraceMemoryDump* traceMemoryDump, GPUResourceTag &tag) override;

    void DumpMemoryStatistics(TraceMemoryDump* traceMemoryDump) override;

    void SetCurrentGpuResourceTag(const GPUResourceTag &tag) override;

#ifdef NEW_SKIA
    sk_sp<GrDirectContext> GetGrContext() const;
    void SetGrContext(const sk_sp<GrDirectContext>& grContext);
#else
    sk_sp<GrContext> GetGrContext() const;
    void SetGrContext(const sk_sp<GrContext>& grContext);
#endif
#ifdef NEW_SKIA
    const sk_sp<GrDirectContext> ExportSkiaContext() const
#else
    const sk_sp<GrContext> ExportSkiaContext() const
#endif
    {
        return grContext_;
    }

private:
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext_;
#else
    sk_sp<GrContext> grContext_;
#endif
    std::shared_ptr<SkiaPersistentCache> skiaPersistentCache_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_GPUCONTEXT_H
