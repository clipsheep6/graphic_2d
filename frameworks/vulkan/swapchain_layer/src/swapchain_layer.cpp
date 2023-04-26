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

#include <cassert>
#include <cstdio>
#include <limits>
#include <thread>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <malloc.h>
#include <algorithm>
#include <new>
#include <securec.h>
#include <cstring>
#include <cinttypes>
#include <refbase.h>

#include <window.h>
#include <graphic_common.h>
#include <native_window.h>
#include "vk_dispatch_table_helper.h"
#include "vk_layer_dispatch_table.h"
#include "sync_fence.h"

#include "vulkan/vk_ohos_native_buffer.h"
#include "swapchain_layer_log.h"
#include "swapchain_utils.h"
#include "swapchain_layer.h"
#include "layer_data.h"

namespace OHOS::SWAPCHAIN {
VkResult GetExtensionProperties(const uint32_t count, const VkExtensionProperties* layerExtensions,
                                uint32_t* pCount, VkExtensionProperties* pProperties)
{
    uint32_t copySize;

    if (pProperties == nullptr || layerExtensions == nullptr) {
        *pCount = count;
        return VK_SUCCESS;
    }

    copySize = *pCount < count ? *pCount : count;
    errno_t ret = memcpy_s(pProperties, copySize * sizeof(VkExtensionProperties),
                           layerExtensions, copySize * sizeof(VkExtensionProperties));
    if (ret != EOK) {
        return VK_INCOMPLETE;
    }
    *pCount = copySize;
    if (copySize < count) {
        return VK_INCOMPLETE;
    }

    return VK_SUCCESS;
}

VkResult GetLayerProperties(const uint32_t count, const VkLayerProperties* layerProperties,
                            uint32_t* pCount, VkLayerProperties* pProperties)
{
    uint32_t copySize;

    if (pProperties == nullptr || layerProperties == nullptr) {
        *pCount = count;
        return VK_SUCCESS;
    }

    copySize = *pCount < count ? *pCount : count;
    errno_t ret = memcpy_s(pProperties, copySize * sizeof(VkLayerProperties),
                           layerProperties, copySize * sizeof(VkLayerProperties));
    if (ret != EOK) {
        return VK_INCOMPLETE;
    }
    *pCount = copySize;
    if (copySize < count) {
        return VK_INCOMPLETE;
    }

    return VK_SUCCESS;
}

VKAPI_ATTR VkResult GetSwapchainGrallocUsageOpenHarmony(VkDevice device,
    VkFormat format,
    VkImageUsageFlags imageUsage,
    uint64_t* grallocUsage)
{
    PFN_vkGetSwapchainGrallocUsageOpenHarmony getSwapchainGrallocUsage =
        reinterpret_cast<PFN_vkGetSwapchainGrallocUsageOpenHarmony>(
            GetDeviceProcAddr(device, "vkGetSwapchainGrallocUsageOpenHarmony"));
    if (getSwapchainGrallocUsage != nullptr) {
        return getSwapchainGrallocUsage(device, format, imageUsage, grallocUsage);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

VKAPI_ATTR VkResult SetNativeFenceFdOpenHarmony(VkDevice device,
    int nativeFenceFd,
    VkSemaphore semaphore,
    VkFence fence)
{
    PFN_vkSetNativeFenceFdOpenHarmony acquireImage = reinterpret_cast<PFN_vkSetNativeFenceFdOpenHarmony>(
        GetDeviceProcAddr(device, "vkSetNativeFenceFdOpenHarmony"));
    if (acquireImage != nullptr) {
        return acquireImage(device, nativeFenceFd, semaphore, fence);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

void ReleaseSwapchainImage(VkDevice device, NativeWindow* window, int releaseFence, Swapchain::Image& image,
                           bool deferIfPending)
{
    VkLayerDispatchTable* pDisp =
        GetLayerDataPtr(GetDispatchKey(device), g_layerDataMap)->deviceDispatchTable.get();

    pDisp->DeviceWaitIdle(device);
    if (image.requested) {
        if (releaseFence >= 0) {
            if (image.requestFence >= 0) {
                close(image.requestFence);
            }
        } else {
            releaseFence = image.requestFence;
        }
        image.requestFence = -1;

        if (window != nullptr) {
            NativeWindowCancelBuffer(window, image.buffer);
        } else {
            if (releaseFence >= 0) {
                close(releaseFence);
            }
        }
        image.requested = false;
    }

    if (deferIfPending && IsFencePending(image.releaseFence)) {
        return;
    }

    if (image.releaseFence >= 0) {
        close(image.releaseFence);
        image.releaseFence = -1;
    }

    if (image.image != VK_NULL_HANDLE) {
        pDisp->DestroyImage(device, image.image, nullptr);
        image.image = VK_NULL_HANDLE;
    }
}

void ReleaseSwapchain(VkDevice device, Swapchain* swapchain)
{
    if (swapchain->surface.swapchainHandle != HandleFromSwapchain(swapchain)) {
        return;
    }

    for (uint32_t i = 0; i < swapchain->numImages; i++) {
        if (!swapchain->images[i].requested) {
            ReleaseSwapchainImage(device, nullptr, -1, swapchain->images[i], true);
        }
    }
    swapchain->surface.swapchainHandle = VK_NULL_HANDLE;
}

VKAPI_ATTR VkResult SetWindowInfo(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo, int32_t* numImages)
{
    GraphicPixelFormat pixelFormat = GetPixelFormat(createInfo->imageFormat);
    Surface& surface = *SurfaceFromHandle(createInfo->surface);

    NativeWindow* window = surface.window;
    int err = NativeWindowHandleOpt(window, SET_FORMAT, pixelFormat);
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("native_window_set_buffers_format(%{public}d) failed: (%{public}d)", pixelFormat, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    err = NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY,
                                static_cast<int>(createInfo->imageExtent.width),
                                static_cast<int>(createInfo->imageExtent.height));
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow SET_BUFFER_GEOMETRY width:%{public}d,height:%{public}d failed: %{public}d",
            createInfo->imageExtent.width, createInfo->imageExtent.height, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    if (createInfo->presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
        createInfo->presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {
        *numImages = 1;
    }

    uint64_t nativeUsage;
    VkResult result = GetSwapchainGrallocUsageOpenHarmony(device, createInfo->imageFormat,
                                                          createInfo->imageUsage, &nativeUsage);
    if (result != VK_SUCCESS) {
        SWLOGE("GetSwapchainGrallocUsageOpenHarmony failed: %{public}d", result);
        return result;
    }
    if (createInfo->flags & VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR) {
        nativeUsage |= OHOS::BUFFER_USAGE_PROTECTED;
    }
    err = NativeWindowHandleOpt(window, SET_USAGE, nativeUsage);
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow SET_USAGE failed: %{public}d", err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    return VK_SUCCESS;
}

VkResult SetSwapchainCreateInfo(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo,
    int32_t* numImages)
{
    GraphicColorDataSpace colorDataSpace = GetColorDataspace(createInfo->imageColorSpace);
    if (colorDataSpace == GRAPHIC_COLOR_DATA_SPACE_UNKNOWN) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (createInfo->oldSwapchain != VK_NULL_HANDLE) {
        ReleaseSwapchain(device, SwapchainFromHandle(createInfo->oldSwapchain));
    }

    return SetWindowInfo(device, createInfo, numImages);
}

void InitImageCreateInfo(const VkSwapchainCreateInfoKHR* createInfo, VkImageCreateInfo* imageCreate)
{
    imageCreate->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    bool swapchainCreateProtected = createInfo->flags & VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR;
    imageCreate->flags = swapchainCreateProtected ? VK_IMAGE_CREATE_PROTECTED_BIT : 0u;
    imageCreate->imageType = VK_IMAGE_TYPE_2D;
    imageCreate->format = createInfo->imageFormat;
    imageCreate->extent = {0, 0, 1};
    imageCreate->mipLevels = 1;
    imageCreate->arrayLayers = 1;
    imageCreate->samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreate->tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreate->usage = createInfo->imageUsage;
    imageCreate->sharingMode = createInfo->imageSharingMode;
    imageCreate->queueFamilyIndexCount = createInfo->queueFamilyIndexCount;
    imageCreate->pQueueFamilyIndices = createInfo->pQueueFamilyIndices;
}

VKAPI_ATTR VkResult CreateImages(int32_t& numImages, Swapchain* swapchain, const VkSwapchainCreateInfoKHR* createInfo,
    VkImageCreateInfo& imageCreate, VkDevice device)
{
    VkLayerDispatchTable* pDisp =
        GetLayerDataPtr(GetDispatchKey(device), g_layerDataMap)->deviceDispatchTable.get();
    Surface& surface = *SurfaceFromHandle(createInfo->surface);
    NativeWindow* window = surface.window;
    if (createInfo->oldSwapchain != VK_NULL_HANDLE) {
        SWLOGI("recreate swapchain ,clean buffer queue");
        window->surface->CleanCache();
    }
    VkResult result = VK_SUCCESS;
    for (int32_t i = 0; i < numImages; i++) {
        Swapchain::Image& img = swapchain->images[i];

        NativeWindowBuffer* buffer;
        int err = NativeWindowRequestBuffer(window, &buffer, &img.requestFence);
        if (err != OHOS::GSERROR_OK) {
            SWLOGE("dequeueBuffer[%{public}u] failed: (%{public}d)", i, err);
            result = VK_ERROR_SURFACE_LOST_KHR;
            break;
        }
        img.buffer = buffer;
        img.requested = true;
        imageCreate.extent = VkExtent3D {static_cast<uint32_t>(img.buffer->sfbuffer->GetSurfaceBufferWidth()),
                                          static_cast<uint32_t>(img.buffer->sfbuffer->GetSurfaceBufferHeight()), 1};
        ((VkNativeBufferOpenHarmony*)(imageCreate.pNext))->handle = img.buffer->sfbuffer->GetBufferHandle();
        result = pDisp->CreateImage(device, &imageCreate, nullptr, &img.image);
        if (result != VK_SUCCESS) {
            SWLOGD("vkCreateImage native buffer failed: %{public}u", result);
            break;
        }
    }

    SWLOGD("swapchain init shared %{public}d", swapchain->shared);
    for (int32_t i = 0; i < numImages; i++) {
        Swapchain::Image& img = swapchain->images[i];
        if (img.requested) {
            if (!swapchain->shared) {
                NativeWindowCancelBuffer(window, img.buffer);
                img.requestFence = -1;
                img.requested = false;
            }
        }
    }
    return result;
}

static void DestroySwapchainInternal(VkDevice device, VkSwapchainKHR swapchainHandle,
                                     const VkAllocationCallbacks* allocator)
{
    Swapchain* swapchain = SwapchainFromHandle(swapchainHandle);
    if (swapchain == nullptr) {
        return;
    }

    bool active = swapchain->surface.swapchainHandle == swapchainHandle;
    NativeWindow* window = active ? swapchain->surface.window : nullptr;

    for (uint32_t i = 0; i < swapchain->numImages; i++) {
        ReleaseSwapchainImage(device, window, -1, swapchain->images[i], false);
    }

    if (active) {
        swapchain->surface.swapchainHandle = VK_NULL_HANDLE;
    }
    if (allocator == nullptr) {
        allocator = &GetDefaultAllocator();
    }
    swapchain->~Swapchain();
    allocator->pfnFree(allocator->pUserData, swapchain);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo,
    const VkAllocationCallbacks* allocator, VkSwapchainKHR* swapchainHandle)
{
    Surface& surface = *SurfaceFromHandle(createInfo->surface);
    if (surface.swapchainHandle != createInfo->oldSwapchain) {
        return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;
    }

    int32_t numImages = static_cast<int32_t>(MIN_BUFFER_SIZE);
    VkResult result = SetSwapchainCreateInfo(device, createInfo, &numImages);
    if (result != VK_SUCCESS) {
        return result;
    }

    if (allocator == nullptr) {
        allocator = &GetDefaultAllocator();
    }
    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Swapchain), alignof(Swapchain),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (mem == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    Swapchain* swapchain = new (mem) Swapchain(surface, numImages, createInfo->presentMode,
        TranslateVulkanToNativeTransform(createInfo->preTransform));
    VkSwapchainImageCreateInfoOpenHarmony swapchainImageCreate = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_IMAGE_CREATE_INFO_OPENHARMONY,
        .pNext = nullptr,
    };
    VkNativeBufferOpenHarmony imageNativeBuffer = {
        .sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_OPENHARMONY,
        .pNext = &swapchainImageCreate,
    };

    VkImageCreateInfo imageCreate = {
        .pNext = &imageNativeBuffer,
    };

    InitImageCreateInfo(createInfo, &imageCreate);
    result = CreateImages(numImages, swapchain, createInfo, imageCreate, device);
    if (result != VK_SUCCESS) {
        DestroySwapchainInternal(device, HandleFromSwapchain(swapchain), allocator);
        return result;
    }
    surface.swapchainHandle = HandleFromSwapchain(swapchain);
    *swapchainHandle = surface.swapchainHandle;
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL DestroySwapchainKHR(
    VkDevice device, VkSwapchainKHR vkSwapchain, const VkAllocationCallbacks* pAllocator)
{
    DestroySwapchainInternal(device, vkSwapchain, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainImagesKHR(
    VkDevice device, VkSwapchainKHR vkSwapchain, uint32_t* count, VkImage* images)
{
    const Swapchain& swapchain = *SwapchainFromHandle(vkSwapchain);
    if (images == nullptr) {
        *count = swapchain.numImages;
        return VK_SUCCESS;
    }

    VkResult result = VK_SUCCESS;
    uint32_t numImages = swapchain.numImages;
    if (*count < swapchain.numImages) {
        numImages = *count;
        result = VK_INCOMPLETE;
    }
    for (uint32_t i = 0; i < numImages; i++) {
        images[i] = swapchain.images[i].image;
    }
    *count = numImages;
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchainHandle,
    uint64_t timeout, VkSemaphore semaphore, VkFence vkFence, uint32_t* imageIndex)
{
    Swapchain& swapchain = *SwapchainFromHandle(swapchainHandle);
    NativeWindow* nativeWindow = swapchain.surface.window;
    VkResult result;

    if (swapchain.surface.swapchainHandle != swapchainHandle) {
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    if (swapchain.shared) {
        *imageIndex = 0;
        return SetNativeFenceFdOpenHarmony(device, -1, semaphore, vkFence);
    }

    NativeWindowBuffer* nativeWindowBuffer;
    int fence;
    int32_t ret = NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fence);
    if (ret != OHOS::GSERROR_OK) {
        SWLOGE("dequeueBuffer failed: (%{public}d)", ret);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    uint32_t index = 0;
    while (index < swapchain.numImages) {
        if (swapchain.images[index].buffer->sfbuffer == nativeWindowBuffer->sfbuffer) {
            swapchain.images[index].requested = true;
            swapchain.images[index].requestFence = fence;
            break;
        }
        index++;
    }

    if (index == swapchain.numImages) {
        SWLOGE("dequeueBuffer returned unrecognized buffer");
        if (NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer) != OHOS::GSERROR_OK) {
            SWLOGE("NativeWindowCancelBuffer failed: (%{public}d)", ret);
        }
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    result = SetNativeFenceFdOpenHarmony(device, -1, semaphore, vkFence);
    if (result != VK_SUCCESS) {
        if (NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer) != OHOS::GSERROR_OK) {
            SWLOGE("NativeWindowCancelBuffer failed: (%{public}d)", ret);
        }
        swapchain.images[index].requested = false;
        swapchain.images[index].requestFence = -1;
        return result;
    }

    *imageIndex = index;
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult AcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex)
{
    return AcquireNextImageKHR(device, pAcquireInfo->swapchain, pAcquireInfo->timeout,
                               pAcquireInfo->semaphore, pAcquireInfo->fence, pImageIndex);
}

const VkPresentRegionKHR* GetPresentRegion(
    const VkPresentRegionKHR* regions, const Swapchain& swapchain, uint32_t index)
{
    return (regions && !swapchain.mailboxMode) ? &regions[index] : nullptr;
}

const VkPresentRegionKHR* GetPresentRegions(const VkPresentInfoKHR* presentInfo)
{
    const VkPresentRegionsKHR* presentRegions = nullptr;
    const VkPresentRegionsKHR* nextRegions = reinterpret_cast<const VkPresentRegionsKHR*>(presentInfo->pNext);
    while (nextRegions != nullptr) {
        if (nextRegions->sType == VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR) {
            presentRegions = nextRegions;
        }
        nextRegions = reinterpret_cast<const VkPresentRegionsKHR*>(nextRegions->pNext);
    }

    if (presentRegions == nullptr) {
        return nullptr;
    } else {
        return presentRegions->pRegions;
    }
}

PFN_vkGetNativeFenceFdOpenHarmony pfn_vkGetNativeFenceFdOpenHarmony = nullptr;

VkResult GetNativeFenceFdOpenHarmony(
    VkQueue queue,
    uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores,
    VkImage image,
    int* pNativeFenceFd)
{
    if (pfn_vkGetNativeFenceFdOpenHarmony != nullptr) {
        return pfn_vkGetNativeFenceFdOpenHarmony(queue, waitSemaphoreCount, pWaitSemaphores, image, pNativeFenceFd);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

VkResult GetReleaseFence(VkQueue queue, const VkPresentInfoKHR* presentInfo,
    Swapchain::Image& img, int32_t &fence)
{
    VkResult result = GetNativeFenceFdOpenHarmony(queue, presentInfo->waitSemaphoreCount,
        presentInfo->pWaitSemaphores, img.image, &fence);
    if (img.releaseFence >= 0) {
        close(img.releaseFence);
        img.releaseFence = -1;
    }
    if (fence >= 0) {
        img.releaseFence = dup(fence);
    }
    return result;
}

struct Region::Rect* GetRegionRect(
    const VkAllocationCallbacks* defaultAllocator, struct Region::Rect** rects, int32_t rectangleCount)
{
    return static_cast<struct Region::Rect*>(
                defaultAllocator->pfnReallocation(
                    defaultAllocator->pUserData, *rects,
                    sizeof(Region::Rect) *rectangleCount,
                    alignof(Region::Rect), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND));
}

void InitRegionRect(const VkRectLayerKHR* layer, struct Region::Rect* rect)
{
    rect->x = layer->offset.x;
    rect->y = layer->offset.y;
    rect->w = layer->extent.width;
    rect->h = layer->extent.height;
}

VkResult FlushBuffer(const VkPresentRegionKHR* region, struct Region::Rect* rects,
    Swapchain& swapchain, Swapchain::Image& img, int32_t fence)
{
    const VkAllocationCallbacks* defaultAllocator = &GetDefaultAllocator();
    Region localRegion;
    if (memset_s(&localRegion, sizeof(localRegion), 0, sizeof(Region)) != EOK) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    if (region != nullptr) {
        int32_t rectangleCount = region->rectangleCount;
        if (rectangleCount > 0) {
            struct Region::Rect* tmpRects = GetRegionRect(defaultAllocator, &rects, rectangleCount);
            if (tmpRects != nullptr) {
                rects = tmpRects;
            } else {
                rectangleCount = 0;
            }
        }
        for (int32_t r = 0; r < rectangleCount; ++r) {
            InitRegionRect(&region->pRectangles[r], &rects[r]);
        }

        localRegion.rects = rects;
        localRegion.rectNumber = rectangleCount;
    }
    NativeWindow* window = swapchain.surface.window;
    // the acquire fence will be close by BufferQueue module
    int err = NativeWindowFlushBuffer(window, img.buffer, fence, localRegion);
    VkResult scResult = VK_SUCCESS;
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("queueBuffer failed: (%{public}d)", err);
        scResult = VK_ERROR_SURFACE_LOST_KHR;
    } else {
        if (img.requestFence >= 0) {
            close(img.requestFence);
            img.requestFence = -1;
        }
        img.requested = false;
    }

    if (swapchain.shared && scResult == VK_SUCCESS) {
        NativeWindowBuffer* buffer = nullptr;
        int releaseFence = -1;
        err = NativeWindowRequestBuffer(window, &buffer, &releaseFence);
        if (err != OHOS::GSERROR_OK) {
            scResult = VK_ERROR_SURFACE_LOST_KHR;
        } else if (img.buffer != buffer) {
            scResult = VK_ERROR_SURFACE_LOST_KHR;
        } else {
            img.requestFence = releaseFence;
            img.requested = true;
        }
    }

    return scResult;
}

VKAPI_ATTR VkResult VKAPI_CALL QueuePresentKHR(
    VkQueue queue, const VkPresentInfoKHR* presentInfo)
{
    VkResult ret = VK_SUCCESS;

    const VkPresentRegionKHR* regions = GetPresentRegions(presentInfo);
    const VkAllocationCallbacks* defaultAllocator = &GetDefaultAllocator();
    struct Region::Rect* rects = nullptr;

    for (uint32_t i = 0; i < presentInfo->swapchainCount; i++) {
        Swapchain& swapchain = *(reinterpret_cast<Swapchain*>(presentInfo->pSwapchains[i]));
        Swapchain::Image& img = swapchain.images[presentInfo->pImageIndices[i]];
        const VkPresentRegionKHR* region = GetPresentRegion(regions, swapchain, i);
        int32_t fence = -1;
        ret = GetReleaseFence(queue, presentInfo, img, fence);
        if (swapchain.surface.swapchainHandle == presentInfo->pSwapchains[i]) {
            if (ret == VK_SUCCESS) {
                ret = FlushBuffer(region, rects, swapchain, img, fence);
            } else {
                ReleaseSwapchain(nullptr, &swapchain);
            }
        } else {
            SWLOGE("QueuePresentKHR swapchainHandle != pSwapchains[%{public}d]", i);
            ReleaseSwapchainImage(nullptr, nullptr, fence, img, true);
            ret = VK_ERROR_OUT_OF_DATE_KHR;
        }

        if (presentInfo->pResults) {
            presentInfo->pResults[i] = ret;
        }
    }
    if (rects != nullptr) {
        defaultAllocator->pfnFree(defaultAllocator->pUserData, rects);
    }
    return ret;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, const VkSurfaceKHR surface, VkBool32* pSupported)
{
    *pSupported = VK_TRUE;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateOHOSSurfaceOpenHarmony(VkInstance instance,
    const VkOHOSSurfaceCreateInfoOpenHarmony* pCreateInfo,
    const VkAllocationCallbacks* allocator, VkSurfaceKHR* outSurface)
{
    if (allocator == nullptr) {
        allocator = &GetDefaultAllocator();
    }
    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Surface), alignof(Surface),
                                         VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (mem == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    Surface* surface = new (mem) Surface;
    surface->window = pCreateInfo->window;
    surface->swapchainHandle = VK_NULL_HANDLE;
    NativeWindowHandleOpt(pCreateInfo->window, GET_USAGE, &(surface->consumerUsage));

    if (surface->consumerUsage == 0) {
        SWLOGE("native window get usage failed, error num : %{public}d", VK_ERROR_SURFACE_LOST_KHR);
        surface->~Surface();
        allocator->pfnFree(allocator->pUserData, surface);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    *outSurface = HandleFromSurface(surface);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL DestroySurfaceKHR(
    VkInstance instance, VkSurfaceKHR vkSurface, const VkAllocationCallbacks* pAllocator)
{
    Surface* surface = SurfaceFromHandle(vkSurface);
    if (surface == nullptr) {
        return;
    }
    if (pAllocator == nullptr) {
        pAllocator = &GetDefaultAllocator();
    }
    surface->~Surface();
    pAllocator->pfnFree(pAllocator->pUserData, surface);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* capabilities)
{
    int width = 0;
    int height = 0;
    uint32_t maxBufferCount = 10;
    if (surface != VK_NULL_HANDLE) {
        NativeWindow* window = SurfaceFromHandle(surface)->window;
        int err = NativeWindowHandleOpt(window, GET_BUFFER_GEOMETRY, &height, &width);
        if (err != OHOS::GSERROR_OK) {
            SWLOGE("NATIVE_WINDOW_DEFAULT_WIDTH query failed: (%{public}d)", err);
            return VK_ERROR_SURFACE_LOST_KHR;
        }
        maxBufferCount = window->surface->GetQueueSize();
    }

    capabilities->minImageCount = std::min(maxBufferCount, MIN_BUFFER_SIZE);
    capabilities->maxImageCount = maxBufferCount;
    capabilities->currentExtent = VkExtent2D {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    capabilities->minImageExtent = VkExtent2D {1, 1};
    capabilities->maxImageExtent = VkExtent2D {4096, 4096};
    capabilities->maxImageArrayLayers = 1;
    capabilities->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    capabilities->supportedUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
                                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t* count, VkSurfaceFormatKHR* formats)
{
    std::vector<VkSurfaceFormatKHR> allFormats = {{VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};
    VkResult result = VK_SUCCESS;
    if (formats != nullptr) {
        uint32_t transferCount = allFormats.size();
        if (transferCount > *count) {
            transferCount = *count;
            result = VK_INCOMPLETE;
        }
        std::copy(allFormats.begin(), allFormats.begin() + transferCount, formats);
        *count = transferCount;
    } else {
        *count = allFormats.size();
    }

    return result;
}

void QueryPresentationProperties(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDevicePresentationPropertiesOpenHarmony* presentationProperties)
{
    VkPhysicalDeviceProperties2 properties = {
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        presentationProperties,
        {},
    };

    presentationProperties->sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTATION_PROPERTIES_OPENHARMONY;
    presentationProperties->pNext = nullptr;
    presentationProperties->sharedImage = VK_FALSE;

    DispatchKey key = GetDispatchKey(physicalDevice);
    LayerData* curLayerData = GetLayerDataPtr(key, g_layerDataMap);
    if (curLayerData->instanceDispatchTable->GetPhysicalDeviceProperties2) {
        curLayerData->instanceDispatchTable->GetPhysicalDeviceProperties2(physicalDevice, &properties);
    } else if (curLayerData->instanceDispatchTable->GetPhysicalDeviceProperties2KHR) {
        curLayerData->instanceDispatchTable->GetPhysicalDeviceProperties2KHR(physicalDevice, &properties);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t* count, VkPresentModeKHR* pPresentModes)
{
    std::vector<VkPresentModeKHR> presentModes;
    presentModes.push_back(VK_PRESENT_MODE_MAILBOX_KHR);
    presentModes.push_back(VK_PRESENT_MODE_FIFO_KHR);

    VkPhysicalDevicePresentationPropertiesOpenHarmony presentProperties;
    QueryPresentationProperties(physicalDevice, &presentProperties);
    if (presentProperties.sharedImage) {
        presentModes.push_back(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR);
        presentModes.push_back(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR);
    }

    uint32_t numModes = static_cast<uint32_t>(presentModes.size());
    VkResult result = VK_SUCCESS;
    if (pPresentModes != nullptr) {
        if (*count < numModes) {
            result = VK_INCOMPLETE;
        }
        *count = std::min(*count, numModes);
        std::copy_n(presentModes.data(), *count, pPresentModes);
    } else {
        *count = numModes;
    }
    return result;
}

VkLayerInstanceCreateInfo* GetChainInfo(const VkInstanceCreateInfo* pCreateInfo, VkLayerFunction func)
{
    auto chainInfo = static_cast<const VkLayerInstanceCreateInfo*>(pCreateInfo->pNext);
    while (chainInfo) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerInstanceCreateInfo*>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerInstanceCreateInfo*>(chainInfo->pNext);
    }
    SWLOGE("Failed to find VkLayerInstanceCreateInfo");
    return nullptr;
}

VkLayerDeviceCreateInfo* GetChainInfo(const VkDeviceCreateInfo* pCreateInfo, VkLayerFunction func)
{
    auto chainInfo = static_cast<const VkLayerDeviceCreateInfo*>(pCreateInfo->pNext);
    while (chainInfo) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerDeviceCreateInfo*>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerDeviceCreateInfo*>(chainInfo->pNext);
    }
    SWLOGE("Failed to find VkLayerDeviceCreateInfo");
    return nullptr;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(
    const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance)
{
    VkLayerInstanceCreateInfo* chainInfo = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    if (chainInfo == nullptr || chainInfo->u.pLayerInfo == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr =
        chainInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateInstance fpCreateInstance =
        (PFN_vkCreateInstance)fpGetInstanceProcAddr(nullptr, "vkCreateInstance");
    if (fpCreateInstance == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    chainInfo->u.pLayerInfo = chainInfo->u.pLayerInfo->pNext;

    VkResult result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS)
        return result;

    LayerData* instanceLayerData = GetLayerDataPtr(GetDispatchKey(*pInstance), g_layerDataMap);
    instanceLayerData->instance = *pInstance;
    instanceLayerData->instanceDispatchTable = std::make_unique<VkLayerInstanceDispatchTable>();
    layer_init_instance_dispatch_table(*pInstance, instanceLayerData->instanceDispatchTable.get(),
                                       fpGetInstanceProcAddr);
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyInstance(
    VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey instanceKey = GetDispatchKey(instance);
    LayerData* curLayerData = GetLayerDataPtr(instanceKey, g_layerDataMap);
    curLayerData->instanceDispatchTable->DestroyInstance(instance, pAllocator);
    FreeLayerDataPtr(instanceKey, g_layerDataMap);
}

bool CheckExtensionAvailable(const std::string extensionName,
                             const std::vector<VkExtensionProperties>& deviceExtensions)
{
    bool extensionAvailable = false;
    for (uint32_t i = 0; i < deviceExtensions.size(); i++) {
        if (!strcmp(extensionName.data(), deviceExtensions[i].extensionName)) {
            extensionAvailable = true;
            break;
        }
    }
    return extensionAvailable;
}

VkResult AddDeviceExtensions(VkPhysicalDevice gpu, const LayerData* gpuLayerData,
                             std::vector<const char*>& enabledExtensions)
{
    VkResult result;
    uint32_t deviceExtensionCount;
    result = gpuLayerData->instanceDispatchTable->EnumerateDeviceExtensionProperties(
        gpu, nullptr, &deviceExtensionCount, nullptr);
    if (result == VK_SUCCESS && deviceExtensionCount > 0) {
        std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
        result = gpuLayerData->instanceDispatchTable->EnumerateDeviceExtensionProperties(
            gpu, nullptr, &deviceExtensionCount, deviceExtensions.data());
        if (result == VK_SUCCESS) {
            if (!CheckExtensionAvailable(VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME, deviceExtensions)) {
                return VK_ERROR_INITIALIZATION_FAILED;
            }
            enabledExtensions.push_back(VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME);
            if (CheckExtensionAvailable(VK_OPENHARMONY_EXTERNAL_MEMORY_OHOS_NATIVE_BUFFER_EXTENSION_NAME,
                                        deviceExtensions)) {
                enabledExtensions.push_back(VK_OPENHARMONY_EXTERNAL_MEMORY_OHOS_NATIVE_BUFFER_EXTENSION_NAME);
            }
        }
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDevice(VkPhysicalDevice gpu,
    const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice)
{
    DispatchKey gpuKey = GetDispatchKey(gpu);
    LayerData* gpuLayerData = GetLayerDataPtr(gpuKey, g_layerDataMap);

    VkDeviceCreateInfo createInfo = *pCreateInfo;
    std::vector<const char*> enabledExtensions;
    for (uint32_t i = 0; i < createInfo.enabledExtensionCount; i++) {
        enabledExtensions.push_back(createInfo.ppEnabledExtensionNames[i]);
    }

    VkResult result = AddDeviceExtensions(gpu, gpuLayerData, enabledExtensions);
    if (result != VK_SUCCESS) {
        return result;
    }

    createInfo.enabledExtensionCount = ToUint32(enabledExtensions.size());
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkLayerDeviceCreateInfo* linkInfo = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    if (linkInfo == nullptr || linkInfo->u.pLayerInfo == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = linkInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = linkInfo->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice =
        (PFN_vkCreateDevice)fpGetInstanceProcAddr(gpuLayerData->instance, "vkCreateDevice");
    if (fpCreateDevice == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    linkInfo->u.pLayerInfo = linkInfo->u.pLayerInfo->pNext;

    result = fpCreateDevice(gpu, &createInfo, pAllocator, pDevice);
    if (result != VK_SUCCESS) {
        return result;
    }

    LayerData* deviceLayerData = GetLayerDataPtr(GetDispatchKey(*pDevice), g_layerDataMap);

    deviceLayerData->deviceDispatchTable = std::make_unique<VkLayerDispatchTable>();
    deviceLayerData->instance = gpuLayerData->instance;
    layer_init_device_dispatch_table(*pDevice, deviceLayerData->deviceDispatchTable.get(), fpGetDeviceProcAddr);

    VkLayerDeviceCreateInfo* callbackInfo = GetChainInfo(pCreateInfo, VK_LOADER_DATA_CALLBACK);
    if (callbackInfo == nullptr || callbackInfo->u.pfnSetDeviceLoaderData == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    deviceLayerData->fpSetDeviceLoaderData = callbackInfo->u.pfnSetDeviceLoaderData;
    if (fpGetDeviceProcAddr != nullptr) {
        pfn_vkGetNativeFenceFdOpenHarmony = reinterpret_cast<PFN_vkGetNativeFenceFdOpenHarmony>(
            fpGetDeviceProcAddr(*pDevice, "vkGetNativeFenceFdOpenHarmony"));
    }

    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey deviceKey = GetDispatchKey(device);
    LayerData* deviceData = GetLayerDataPtr(deviceKey, g_layerDataMap);
    deviceData->deviceDispatchTable->DestroyDevice(device, pAllocator);

    FreeLayerDataPtr(deviceKey, g_layerDataMap);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
    DispatchKey instanceKey = GetDispatchKey(instance);
    LayerData* curLayerData = GetLayerDataPtr(instanceKey, g_layerDataMap);
    VkResult res = curLayerData->instanceDispatchTable->CreateDebugUtilsMessengerEXT(
        instance, pCreateInfo, pAllocator, pMessenger);
    if (res == VK_SUCCESS) {
        curLayerData->debugCallbacks[*pMessenger] = *pCreateInfo;
    }
    return res;
}

VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey instanceKey = GetDispatchKey(instance);
    LayerData* curLayerData = GetLayerDataPtr(instanceKey, g_layerDataMap);
    curLayerData->debugCallbacks.erase(messenger);
    curLayerData->instanceDispatchTable->DestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(
    const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    if (pLayerName && !strcmp(pLayerName, swapchainLayer.layerName)) {
        return GetExtensionProperties(std::size(instanceExtensions), instanceExtensions, pCount, pProperties);
    }

    return VK_ERROR_LAYER_NOT_PRESENT;
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties(uint32_t* pCount, VkLayerProperties* pProperties)
{
    return GetLayerProperties(1, &swapchainLayer, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t* pCount, VkLayerProperties* pProperties)
{
    return GetLayerProperties(1, &swapchainLayer, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    if (pLayerName && !strcmp(pLayerName, swapchainLayer.layerName)) {
        return GetExtensionProperties(
            std::size(deviceExtensions), deviceExtensions, pCount, pProperties);
    }

    if (physicalDevice == nullptr) {
        SWLOGE("physicalDevice is null.");
        return VK_ERROR_LAYER_NOT_PRESENT;
    }

    DispatchKey key = GetDispatchKey(physicalDevice);
    LayerData* curLayerData = GetLayerDataPtr(key, g_layerDataMap);
    return curLayerData->instanceDispatchTable->EnumerateDeviceExtensionProperties(physicalDevice, nullptr,
                                                                                   pCount, pProperties);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetPhysicalDeviceProcAddr(VkInstance instance, const char* funcName)
{
    if (instance == VK_NULL_HANDLE) {
        SWLOGE("instance is null.");
        return nullptr;
    }

    LayerData* layerData = GetLayerDataPtr(GetDispatchKey(instance), g_layerDataMap);
    VkLayerInstanceDispatchTable* pTable = layerData->instanceDispatchTable.get();

    if (pTable->GetPhysicalDeviceProcAddr == nullptr) {
        return nullptr;
    }
    return pTable->GetPhysicalDeviceProcAddr(instance, funcName);
}

static inline PFN_vkVoidFunction LayerInterceptProc(const char* name)
{
    if (!name) {
        return nullptr;
    }
    const auto it = layerInterceptProcMap.find(name);
    if (it != layerInterceptProcMap.end()) {
        return it->second;
    }
    return nullptr;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(VkDevice device, const char* funcName)
{
    if (device == VK_NULL_HANDLE) {
        SWLOGE("device is null.");
        return nullptr;
    }

    PFN_vkVoidFunction addr = LayerInterceptProc(funcName);
    if (addr != nullptr) {
        return addr;
    }
    LayerData* devData = GetLayerDataPtr(GetDispatchKey(device), g_layerDataMap);
    VkLayerDispatchTable* pTable = devData->deviceDispatchTable.get();
    if (pTable->GetDeviceProcAddr == nullptr) {
        return nullptr;
    }
    return pTable->GetDeviceProcAddr(device, funcName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char* funcName)
{
    PFN_vkVoidFunction addr = LayerInterceptProc(funcName);
    if (addr != nullptr) {
        return addr;
    }
    if (instance == VK_NULL_HANDLE) {
        SWLOGE("libvulkan_swapchain GetInstanceProcAddr instance is null");
        return nullptr;
    }
    LayerData* layerData = GetLayerDataPtr(GetDispatchKey(instance), g_layerDataMap);
    if (layerData == nullptr) {
        SWLOGE("libvulkan_swapchain GetInstanceProcAddr layerData is null");
        return nullptr;
    }

    VkLayerInstanceDispatchTable* pTable = layerData->instanceDispatchTable.get();
    if (pTable == nullptr) {
        SWLOGE("libvulkan_swapchain GetInstanceProcAddr pTable = null");
        return nullptr;
    }
    if (pTable->GetInstanceProcAddr == nullptr) {
        SWLOGE("libvulkan_swapchain GetInstanceProcAddr pTable->GetInstanceProcAddr = null");
        return nullptr;
    }
    addr = pTable->GetInstanceProcAddr(instance, funcName);

    return addr;
}
}  // namespace OHOS::SWAPCHAIN
