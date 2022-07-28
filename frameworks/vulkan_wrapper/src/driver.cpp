/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "driver.h"
#include "driver_loader.h"
#include <mutex>
#include <malloc.h>


namespace vulkan {
namespace driver {

DriverLoader DriverLoader::loader_;
PFN_vkGetDeviceProcAddr pfn_vkGetDeviceProcAddr = nullptr;
PFN_vkGetPhysicalDeviceProperties2KHR fpn_vkGetPhysicalDeviceProperties2KHR = nullptr;
PFN_vkCreateDevice pfn_vkCreateDevice = nullptr;
PFN_vkQueueSignalReleaseImageOHOS pfn_vkQueueSignalReleaseImageOHOS = nullptr;

bool EnsureInitialized() {
    static bool initialized = false;
    static std::mutex init_lock;

    std::lock_guard<std::mutex> lock(init_lock);

    if (initialized) {
        return true;
    }

    if (DriverLoader::Load()) {
        initialized = true;
    }
    return initialized;
}

VKAPI_ATTR void* DefaultAllocate(void*,
                                 size_t size,
                                 size_t alignment,
                                 VkSystemAllocationScope) {
    void* ptr = nullptr;
    int ret = posix_memalign(&ptr, std::max(alignment, sizeof(void*)), size);
    return ret == 0 ? ptr : nullptr;
}

VKAPI_ATTR void* DefaultReallocate(void*,
                                   void* ptr,
                                   size_t size,
                                   size_t alignment,
                                   VkSystemAllocationScope) {
    if (size == 0) {
        free(ptr);
        return nullptr;
    }

    size_t old_size = ptr ? malloc_usable_size(ptr) : 0;
    if (size <= old_size)
        return ptr;

    void* new_ptr = nullptr;
    if (posix_memalign(&new_ptr, std::max(alignment, sizeof(void*)), size) != 0)
        return nullptr;
    if (ptr) {
        memcpy(new_ptr, ptr, std::min(old_size, size));
        free(ptr);
    }
    return new_ptr;
}

VKAPI_ATTR void DefaultFree(void*, void* ptr) {
    free(ptr);
}

const VkAllocationCallbacks& GetDefaultAllocator() {
    static const VkAllocationCallbacks kDefaultAllocCallbacks = {
        .pUserData = nullptr,
        .pfnAllocation = DefaultAllocate,
        .pfnReallocation = DefaultReallocate,
        .pfnFree = DefaultFree,
    };

    return kDefaultAllocCallbacks;
}

VkResult CreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                    const VkAllocationCallbacks* pAllocator,
                    VkInstance* pInstance) 
{
    if (!EnsureInitialized())
    {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    VkResult result = VK_SUCCESS;

    result = DriverLoader::Device().CreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS) {
        return result;
    }
    pfn_vkGetDeviceProcAddr = 
        reinterpret_cast<PFN_vkGetDeviceProcAddr>(DriverLoader::Device().GetInstanceProcAddr(*pInstance, "vkGetDeviceProcAddr"));
    fpn_vkGetPhysicalDeviceProperties2KHR = 
        reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(
            DriverLoader::Device().GetInstanceProcAddr(*pInstance, "vkGetPhysicalDeviceProperties2KHR"));
    pfn_vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(DriverLoader::Device().GetInstanceProcAddr(*pInstance, "vkCreateDevice"));

    return VK_SUCCESS; 
}


VkResult EnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,VkExtensionProperties* pProperties)
{
    if (!EnsureInitialized()) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult result = DriverLoader::Device().EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
    if (result != VK_SUCCESS) {
        return result;
    }
    return VK_SUCCESS; 
}

PFN_vkVoidFunction GetInstanceProcAddr(VkInstance instance, const char* pName)
{
    if (!EnsureInitialized()) {
        return nullptr;
    }

    PFN_vkVoidFunction func = DriverLoader::Device().GetInstanceProcAddr(instance, pName);

    return func;
}

void DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyInstance pfn_vkDestroyInstance = 
        reinterpret_cast<PFN_vkDestroyInstance>(GetInstanceProcAddr(instance, "vkDestroyInstance"));
    if (pfn_vkDestroyInstance) {
        pfn_vkDestroyInstance(instance, pAllocator);
    }
}

VkResult CreateDevice(VkPhysicalDevice physicalDevice,
                      const VkDeviceCreateInfo* pCreateInfo,
                      const VkAllocationCallbacks* pAllocator,
                      VkDevice* pDevice)
{
    VkResult result = VK_SUCCESS;
    if (pfn_vkCreateDevice) {
        result = pfn_vkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
    }

    if (pDevice && pfn_vkGetDeviceProcAddr) {
        pfn_vkQueueSignalReleaseImageOHOS = reinterpret_cast<PFN_vkQueueSignalReleaseImageOHOS>(
            pfn_vkGetDeviceProcAddr(*pDevice, "vkQueueSignalReleaseImageOHOS"));
    }

    return result;
}


PFN_vkVoidFunction GetDeviceProcAddr(VkDevice device, const char* pName)
{

    PFN_vkVoidFunction func = nullptr;

    if (pfn_vkGetDeviceProcAddr) {
        func = pfn_vkGetDeviceProcAddr(device, pName);
    }

    return func;
}

void DestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyImage destroyImage = reinterpret_cast<PFN_vkDestroyImage>(GetDeviceProcAddr(device, "vkDestroyImage"));
    if (destroyImage) {
        return destroyImage(device, image, pAllocator);
    }
    return;
}

VkResult CreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage)
{
    PFN_vkCreateImage createImage = reinterpret_cast<PFN_vkCreateImage>(GetDeviceProcAddr(device, "vkCreateImage"));
    if (createImage) {
        return createImage(device, pCreateInfo, pAllocator, pImage);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

void QueryPresentationProperties(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDevicePresentationPropertiesOHOS* presentation_properties) {
    // Request the android-specific presentation properties via GPDP2
    VkPhysicalDeviceProperties2 properties = {
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        presentation_properties,
        {},
    };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
    presentation_properties->sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTATION_PROPERTIES_OHOS;
#pragma clang diagnostic pop
    presentation_properties->pNext = nullptr;
    presentation_properties->sharedImage = VK_FALSE;

    GetPhysicalDeviceProperties2(physicalDevice, &properties);
}

void GetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice,
                                  VkPhysicalDeviceProperties2* pProperties) {
    if (fpn_vkGetPhysicalDeviceProperties2KHR) {
        fpn_vkGetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
    }
}

VkResult AcquireImageOHOS(VkDevice device,
    VkImage image,
    int nativeFenceFd,
    VkSemaphore semaphore,
    VkFence fence)
{
    PFN_vkAcquireImageOHOS acquireImage = reinterpret_cast<PFN_vkAcquireImageOHOS>(GetDeviceProcAddr(device, "vkAcquireImageOHOS"));
    if (acquireImage) {
        return acquireImage(device, image, nativeFenceFd, semaphore, fence);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

VkResult BindImageMemory2KHR(VkDevice device,
                             uint32_t bindInfoCount,
                             const VkBindImageMemoryInfo* pBindInfos)
{
    PFN_vkBindImageMemory2KHR bindImageMemory2KHR = reinterpret_cast<PFN_vkBindImageMemory2KHR>(GetDeviceProcAddr(device, "vkBindImageMemory2KHR"));
    if (bindImageMemory2KHR) {
        return bindImageMemory2KHR(device, bindInfoCount, pBindInfos);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

VkResult BindImageMemory2(VkDevice device,
                             uint32_t bindInfoCount,
                             const VkBindImageMemoryInfo* pBindInfos)
{
    PFN_vkBindImageMemory2 bindImageMemory2 = reinterpret_cast<PFN_vkBindImageMemory2>(GetDeviceProcAddr(device, "vkBindImageMemory2"));
    if (bindImageMemory2) {
        return bindImageMemory2(device, bindInfoCount, pBindInfos);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

VkResult QueueSignalReleaseImageOHOS(
    VkQueue queue,
    uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores,
    VkImage image,
    int* pNativeFenceFd)
{
    if (pfn_vkQueueSignalReleaseImageOHOS) {
        return pfn_vkQueueSignalReleaseImageOHOS(queue, waitSemaphoreCount, pWaitSemaphores, image, pNativeFenceFd);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}







}  // namespace driver
}  // namespace vulkan
