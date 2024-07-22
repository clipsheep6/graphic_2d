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

#include "gpucontext_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "image/gpu_context.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t MAX_SIZE = 5000;
} // namespace
namespace Drawing {

class ShaderPersistentCache : public GPUContextOptions::PersistentCache {
public:
    ShaderPersistentCache() = default;
    ~ShaderPersistentCache() override = default;

    std::shared_ptr<Data> Load(const Data& key) override { return nullptr; };
    void Store(const Data& key, const Data& data) override {};
};

bool GPUContextFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    GPUContextOptions options;
    bool allowPathMaskCaching = GetObject<bool>();
    options.SetAllowPathMaskCaching(allowPathMaskCaching);
    options.GetAllowPathMaskCaching();
    return true;
}

bool GPUContextFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    GPUContextOptions options;
    auto persistentCache = std::make_shared<ShaderPersistentCache>();
    options.SetPersistentCache(persistentCache.get());
    options.GetPersistentCache();
    return true;
}

bool GPUContextFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    gpuContext->RegisterPostFunc([&](const std::function<void()>& task) {});
    return true;
}

bool GPUContextFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    gpuContext->ResetContext();
    return true;
}

bool GPUContextFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    gpuContext->Flush();
    gpuContext->Submit();
    bool syncCpu = GetObject<bool>();
    gpuContext->FlushAndSubmit(syncCpu);
    return true;
}

bool GPUContextFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    std::chrono::milliseconds msNotUsed;
    gpuContext->PerformDeferredCleanup(msNotUsed);
    return true;
}

bool GPUContextFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    int maxResource = GetObject<int>();
    size_t maxResourceBytes = GetObject<size_t>();
    gpuContext->SetResourceCacheLimits(maxResource, maxResourceBytes);
    gpuContext->GetResourceCacheLimits(&maxResource, &maxResourceBytes);
    return true;
}

bool GPUContextFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    int resourceCount = GetObject<int>();
    size_t resourceBytes = GetObject<size_t>();
    gpuContext->GetResourceCacheUsage(&resourceCount, &resourceBytes);
    return true;
}

bool GPUContextFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    gpuContext->FreeGpuResources();
    return true;
}

bool GPUContextFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* text = new char[count];
    for (size_t i = 0; i < count; i++) {
        text[i] =  GetObject<char>();
    }
    text[count - 1] = '\0';
    std::string str(text);
    gpuContext->DumpGpuStats(str);
    if (text != nullptr) {
        delete[] text;
        text = nullptr;
    }
    return true;
}

bool GPUContextFuzzTest011(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    gpuContext->ReleaseResourcesAndAbandonContext();
    return true;
}

bool GPUContextFuzzTest012(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    bool scratchResourcesOnly = GetObject<bool>();
    gpuContext->PurgeUnlockedResources(scratchResourcesOnly);
    return true;
}

bool GPUContextFuzzTest013(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    bool scratchResourcesOnly = GetObject<bool>();
    uint32_t fPid = GetObject<uint32_t>();
    uint32_t fTid = GetObject<uint32_t>();
    uint32_t fWid = GetObject<uint32_t>();
    uint32_t fFid = GetObject<uint32_t>();
    GPUResourceTag tag(fPid, fTid, fWid, fFid);
    gpuContext->PurgeUnlockedResourcesByTag(scratchResourcesOnly, tag);
    return true;
}

bool GPUContextFuzzTest014(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    bool scratchResourcesOnly = GetObject<bool>();
    std::set<pid_t> exitedPidSet;
    gpuContext->PurgeUnlockedResourcesByPid(scratchResourcesOnly, exitedPidSet);
    return true;
}

bool GPUContextFuzzTest015(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    bool scratchResourcesOnly = GetObject<bool>();
    std::set<pid_t> exitedPidSet;
    std::set<pid_t> protectedPidSet;
    gpuContext->PurgeCacheBetweenFrames(scratchResourcesOnly, exitedPidSet, protectedPidSet);
    return true;
}

bool GPUContextFuzzTest016(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    gpuContext->PurgeUnlockAndSafeCacheGpuResources();
    return true;
}

bool GPUContextFuzzTest017(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    uint32_t fPid = GetObject<uint32_t>();
    uint32_t fTid = GetObject<uint32_t>();
    uint32_t fWid = GetObject<uint32_t>();
    uint32_t fFid = GetObject<uint32_t>();
    GPUResourceTag tag(fPid, fTid, fWid, fFid);
    gpuContext->SetCurrentGpuResourceTag(tag);
    return true;
}

bool GPUContextFuzzTest018(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    uint32_t fPid = GetObject<uint32_t>();
    uint32_t fTid = GetObject<uint32_t>();
    uint32_t fWid = GetObject<uint32_t>();
    uint32_t fFid = GetObject<uint32_t>();
    GPUResourceTag tag(fPid, fTid, fWid, fFid);
    gpuContext->ReleaseByTag(tag);
    return true;
}

bool GPUContextFuzzTest019(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    uint32_t fPid = GetObject<uint32_t>();
    uint32_t fTid = GetObject<uint32_t>();
    uint32_t fWid = GetObject<uint32_t>();
    uint32_t fFid = GetObject<uint32_t>();
    GPUResourceTag tag(fPid, fTid, fWid, fFid);
    gpuContext->ReleaseByTag(tag);
    return true;
}

bool GPUContextFuzzTest020(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* categoryKey = new char[length];
    for (size_t i = 0; i < length; i++) {
        categoryKey[i] = GetObject<char>();
    }
    categoryKey[length - 1] = '\0';
    bool itemizeType = GetObject<bool>();
    TraceMemoryDump traceMemoryDump = TraceMemoryDump(categoryKey, itemizeType);
    uint32_t fPid = GetObject<uint32_t>();
    uint32_t fTid = GetObject<uint32_t>();
    uint32_t fWid = GetObject<uint32_t>();
    uint32_t fFid = GetObject<uint32_t>();
    GPUResourceTag tag(fPid, fTid, fWid, fFid);
    gpuContext->DumpMemoryStatisticsByTag(&traceMemoryDump, tag);
    if (categoryKey != nullptr) {
        delete [] categoryKey;
        categoryKey = nullptr;
    }
    return true;
}

bool GPUContextFuzzTest021(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* categoryKey = new char[length];
    for (size_t i = 0; i < length; i++) {
        categoryKey[i] = GetObject<char>();
    }
    categoryKey[length - 1] = '\0';
    bool itemizeType = GetObject<bool>();
    TraceMemoryDump traceMemoryDump = TraceMemoryDump(categoryKey, itemizeType);
    gpuContext->DumpMemoryStatistics(&traceMemoryDump);
    if (categoryKey != nullptr) {
        delete [] categoryKey;
        categoryKey = nullptr;
    }
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::GPUContextFuzzTest001(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest002(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest003(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest004(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest005(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest006(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest007(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest008(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest009(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest010(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest011(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest012(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest013(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest014(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest015(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest016(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest017(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest018(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest019(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest020(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest021(data, size);
    return 0;
}
