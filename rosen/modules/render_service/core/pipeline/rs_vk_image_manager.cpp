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

#include "rs_vk_image_manager.h"

#include "SkImageInfo.h"
#include "include/core/SkColorSpace.h"
#include "native_buffer_inner.h"
#include "platform/common/rs_log.h"
#include "include/gpu/GrBackendSurface.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"

namespace OHOS {
namespace Rosen {

NativeVkImageRes::~NativeVkImageRes()
{
    NativeBufferUtils::DeleteVkImage(mVulkanCleanupHelper);
    DestoryNativeWindowBuffer(mNativeWindowBuffer);
}

std::shared_ptr<NativeVkImageRes> NativeVkImageRes::Create(sptr<OHOS::SurfaceBuffer> buffer)
{
    auto width = buffer->GetSurfaceBufferWidth();
    auto height = buffer->GetSurfaceBufferHeight();
    NativeWindowBuffer* nativeWindowBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&buffer);
    auto backendTexture = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(nativeWindowBuffer,
        width, height);
#ifndef USE_ROSEN_DRAWING
    if (!backendTexture.isValid()) {
        return nullptr;
    }
    GrVkImageInfo imageInfo;
    backendTexture.getVkImageInfo(&imageInfo);

    return std::make_unique<NativeVkImageRes>(
        nativeWindowBuffer,
        backendTexture,
        new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            imageInfo.fImage, imageInfo.fAlloc.fMemory));
#else
    return std::make_unique<NativeVkImageRes>(
        nativeWindowBuffer,
        backendTexture,
        new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            backendTexture.vkImage, backendTexture.vkAlloc.memory));
#endif
}

std::shared_ptr<NativeVkImageRes> RSVkImageManager::MapVkImageFromSurfaceBuffer(
    const sptr<OHOS::SurfaceBuffer>& buffer,
    const sptr<SyncFence>& acquireFence,
    uint32_t threadIndex)
{
    WaitAcquireFence(acquireFence);
    std::lock_guard<std::mutex> lock(opMutex_);
    auto bufferId = buffer->GetSeqNum();
    if (imageCacheSeqs_.count(bufferId) == 0) {
        return NewImageCacheFromBuffer(buffer, threadIndex);
    } else {
        return imageCacheSeqs_[bufferId];
    }
}

std::shared_ptr<NativeVkImageRes> RSVkImageManager::CreateImageCacheFromBuffer(sptr<OHOS::SurfaceBuffer> buffer,
    const sptr<SyncFence>& acquireFence)
{
    WaitAcquireFence(acquireFence);
    auto bufferId = buffer->GetSeqNum();
    auto imageCache = NativeVkImageRes::Create(buffer);
    if (imageCache == nullptr) {
        ROSEN_LOGE(
            "RSVkImageManager::CreateImageCacheFromBuffer: failed to create ImageCache for buffer id %{public}d.",
            bufferId);
        return nullptr;
    }
    return imageCache;
}

void RSVkImageManager::WaitAcquireFence(const sptr<SyncFence>& acquireFence)
{
    if (acquireFence == nullptr) {
        return;
    }
    acquireFence->Wait(3000); // 3000ms
}

std::shared_ptr<NativeVkImageRes> RSVkImageManager::NewImageCacheFromBuffer(
    const sptr<OHOS::SurfaceBuffer>& buffer, uint32_t threadIndex)
{
    auto bufferId = buffer->GetSeqNum();
    auto imageCache = NativeVkImageRes::Create(buffer);
    if (imageCache == nullptr) {
        ROSEN_LOGE("RSVkImageManager::NewImageCacheFromBuffer: failed to create ImageCache for buffer id %{public}d.",
            bufferId);
        return {};
    }

    imageCache->SetThreadIndex(threadIndex);
    imageCacheSeqs_.emplace(bufferId, imageCache);
    cacheQueue_.push(bufferId);
    return imageCache;
}

void RSVkImageManager::ShrinkCachesIfNeeded(bool isForUniRedraw)
{
    while (cacheQueue_.size() > MAX_CACHE_SIZE) {
        const int32_t id = cacheQueue_.front();
        if (isForUniRedraw) {
            UnMapVkImageFromSurfaceBufferForUniRedraw(id);
        } else {
            UnMapVkImageFromSurfaceBuffer(id);
        }
        cacheQueue_.pop();
    }
}

void RSVkImageManager::UnMapVkImageFromSurfaceBuffer(int32_t seqNum)
{
    uint32_t threadIndex = UNI_MAIN_THREAD_INDEX;
    {
        std::lock_guard<std::mutex> lock(opMutex_);
        if (imageCacheSeqs_.count(seqNum) == 0) {
            return;
        }
        threadIndex = imageCacheSeqs_[seqNum]->GetThreadIndex();
    }
    auto func = [this, seqNum]() {
        {
            std::lock_guard<std::mutex> lock(opMutex_);
            if (imageCacheSeqs_.count(seqNum) == 0) {
                return;
            }
            (void)imageCacheSeqs_.erase(seqNum);
        }
        RS_LOGD("RSVkImageManager::UnMapVkImageFromSurfaceBuffer: %{public}d", seqNum);
    };
    if (threadIndex == UNI_MAIN_THREAD_INDEX) {
        RSMainThread::Instance()->PostTask(func);
    } else {
        RSSubThreadManager::Instance()->PostTask(func, threadIndex);
    }
}

void RSVkImageManager::UnMapVkImageFromSurfaceBufferForUniRedraw(int32_t seqNum)
{
    RSHardwareThread::Instance().PostTask([this, seqNum]() {
        std::lock_guard<std::mutex> lock(opMutex_);
        if (imageCacheSeqs_.count(seqNum) == 0) {
            return;
        }
        (void)imageCacheSeqs_.erase(seqNum);
        RS_LOGD("RSVkImageManager::UnMapVkImageFromSurfaceBufferForRedraw");
    });
}

} // namespace Rosen
} // namespace OHOS