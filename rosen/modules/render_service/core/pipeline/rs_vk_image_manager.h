/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RS_VK_IMAGE_MANAGER_H
#define RS_VK_IMAGE_MANAGER_H

#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "surface.h"
#include "sync_fence.h"
#include "vulkan/vulkan_core.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "native_window.h"
#include "common/rs_common_def.h"
#include "platform/ohos/backend/native_buffer_utils.h"

namespace OHOS {
namespace Rosen {

class NativeVkImageRes {
public:
#ifndef USE_ROSEN_DRAWING
    NativeVkImageRes(NativeWindowBuffer* nativeWindowBuffer, GrBackendTexture backendTexture,
#else
    NativeVkImageRes(NativeWindowBuffer* nativeWindowBuffer, Drawing::BackendTexture backendTexture,
#endif
        NativeBufferUtils::VulkanCleanupHelper* vulkanCleanupHelper)
        : mNativeWindowBuffer(nativeWindowBuffer),
        mBackendTexture_(backendTexture),
        mVulkanCleanupHelper(vulkanCleanupHelper)
    {
    }

    ~NativeVkImageRes();

#ifndef USE_ROSEN_DRAWING
    const GrBackendTexture& GetBackendTexture() const
#else
    const Drawing::BackendTexture& GetBackendTexture() const
#endif
    {
        return mBackendTexture_;
    }

    NativeBufferUtils::VulkanCleanupHelper* RefCleanupHelper()
    {
        return mVulkanCleanupHelper->Ref();
    }

    static std::shared_ptr<NativeVkImageRes> Create(sptr<OHOS::SurfaceBuffer> buffer);

    uint32_t GetThreadIndex() const
    {
        return threadIndex_;
    }

    void SetThreadIndex(const uint32_t threadIndex = UNI_MAIN_THREAD_INDEX)
    {
        threadIndex_ = threadIndex;
    }

private:
    NativeWindowBuffer* mNativeWindowBuffer;
#ifndef USE_ROSEN_DRAWING
    GrBackendTexture mBackendTexture_;
#else
    Drawing::BackendTexture mBackendTexture_;
#endif
    NativeBufferUtils::VulkanCleanupHelper* mVulkanCleanupHelper;
    uint32_t threadIndex_ = UNI_MAIN_THREAD_INDEX;
};

class RSVkImageManager {
public:
    RSVkImageManager() = default;
    ~RSVkImageManager() noexcept = default;

    std::shared_ptr<NativeVkImageRes> MapVkImageFromSurfaceBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence, uint32_t threadIndex);
    void UnMapVkImageFromSurfaceBuffer(int32_t seqNum);
    void UnMapVkImageFromSurfaceBufferForUniRedraw(int32_t seqNum);
    void ShrinkCachesIfNeeded(bool isForUniRedraw = false);
    std::shared_ptr<NativeVkImageRes> CreateImageCacheFromBuffer(sptr<OHOS::SurfaceBuffer> buffer,
        const sptr<SyncFence>& acquireFence);

private:
    void WaitAcquireFence(const sptr<SyncFence>& acquireFence);
    std::shared_ptr<NativeVkImageRes> NewImageCacheFromBuffer(
        const sptr<OHOS::SurfaceBuffer>& buffer, uint32_t threadIndex);

    mutable std::mutex opMutex_;
    static constexpr size_t MAX_CACHE_SIZE = 16;
    std::queue<int32_t> cacheQueue_; // fifo, size restricted by MAX_CACHE_SIZE
    std::unordered_map<int32_t, std::shared_ptr<NativeVkImageRes>> imageCacheSeqs_; // guarded by opMutex_
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_VK_IMAGE_MANAGER_H