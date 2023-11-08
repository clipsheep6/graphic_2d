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

#ifndef GPU_CONTEXT_IMPL_H
#define GPU_CONTEXT_IMPL_H

#include <chrono>

#include "base_impl.h"
#include "image/trace_memory_dump.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct GPUResourceTag;
class GPUContext;
class GPUContextOptions;
class GPUContextImpl : public BaseImpl {
public:
    GPUContextImpl() {};
    ~GPUContextImpl() override {};

    virtual bool BuildFromGL(const GPUContextOptions& options) = 0;

    virtual void Flush() = 0;
    virtual void FlushAndSubmit(bool syncCpu) = 0;
    virtual void PerformDeferredCleanup(std::chrono::milliseconds msNotUsed) = 0;

    virtual void GetResourceCacheLimits(int* maxResource, size_t* maxResourceBytes) const = 0;
    virtual void SetResourceCacheLimits(int maxResource, size_t maxResourceBytes) = 0;

    virtual void GetResourceCacheUsage(int* resourceCount, size_t* resourceBytes) const = 0;

    virtual void FreeGpuResources() = 0;

    virtual void DumpGpuStats(std::string& out) = 0;

    virtual void ReleaseResourcesAndAbandonContext() = 0;

    virtual void PurgeUnlockedResources(bool scratchResourcesOnly) = 0;

    virtual void PurgeUnlockedResourcesByTag(bool scratchResourcesOnly, const GPUResourceTag tag) = 0;

    virtual void PurgeUnlockAndSafeCacheGpuResources() = 0;

    virtual void ReleaseByTag(const GPUResourceTag tag) = 0;

    virtual void DumpMemoryStatisticsByTag(TraceMemoryDump* traceMemoryDump, GPUResourceTag tag) = 0;

    virtual void DumpMemoryStatistics(TraceMemoryDump* traceMemoryDump) = 0;

    virtual void SetCurrentGpuResourceTag(const GPUResourceTag tag) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // GPU_CONTEXT_IMPL_H
