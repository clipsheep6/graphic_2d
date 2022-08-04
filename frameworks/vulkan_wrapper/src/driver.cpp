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
#include "wrapper_log.h"


namespace vulkan {
namespace driver {

DriverLoader DriverLoader::loader_;
PFN_vkGetDeviceProcAddr pfn_vkGetDeviceProcAddr = nullptr;
PFN_vkGetPhysicalDeviceProperties2KHR fpn_vkGetPhysicalDeviceProperties2KHR = nullptr;
PFN_vkCreateDevice pfn_vkCreateDevice = nullptr;
PFN_vkGetNativeFenceFdOpenHarmony pfn_vkGetNativeFenceFdOpenHarmony = nullptr;
PFN_vkGetPhysicalDeviceProperties pfn_vkGetPhysicalDeviceProperties = nullptr;
PFN_vkGetPhysicalDeviceFeatures pfn_vkGetPhysicalDeviceFeatures = nullptr;
PFN_vkGetPhysicalDeviceMemoryProperties pfn_vkGetPhysicalDeviceMemoryProperties = nullptr;
PFN_vkGetPhysicalDeviceQueueFamilyProperties pfn_vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
 
bool EnsureInitialized() {
    WLOGD("Andrew:: EnsureInitialized is comming");
    static bool initialized = false;
    static std::mutex init_lock;

    std::lock_guard<std::mutex> lock(init_lock);

    if (initialized) {
        WLOGD("Andrew:: initialized is ready");
        return true;
    }

    if (DriverLoader::Load()) {
        WLOGD("Andrew:: DriverLoader::Load is ready");
        initialized = true;
    }
    return initialized;
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
    pfn_vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(DriverLoader::Device().GetInstanceProcAddr(*pInstance, "vkGetPhysicalDeviceProperties"));
    pfn_vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(DriverLoader::Device().GetInstanceProcAddr(*pInstance, "vkGetPhysicalDeviceFeatures"));
    pfn_vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(DriverLoader::Device().GetInstanceProcAddr(*pInstance, "vkGetPhysicalDeviceMemoryProperties"));
    pfn_vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(DriverLoader::Device().GetInstanceProcAddr(*pInstance, "vkGetPhysicalDeviceQueueFamilyProperties"));
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
        pfn_vkGetNativeFenceFdOpenHarmony= reinterpret_cast<PFN_vkGetNativeFenceFdOpenHarmony>(
            pfn_vkGetDeviceProcAddr(*pDevice, "vkGetNativeFenceFdOpenHarmony"));
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

VkResult EnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices)
{
    PFN_vkEnumeratePhysicalDevices pfn_vkEnumeratePhysicalDevices = 
        reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(GetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
    if (pfn_vkEnumeratePhysicalDevices) {
        return pfn_vkEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

void GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties)
{
    if (pfn_vkGetPhysicalDeviceProperties) {
        pfn_vkGetPhysicalDeviceProperties(physicalDevice, pProperties);
    }
}

void GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures)
{
    if (pfn_vkGetPhysicalDeviceFeatures) {
        pfn_vkGetPhysicalDeviceFeatures(physicalDevice, pFeatures);
    }
}

void GetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties)
{
    if (pfn_vkGetPhysicalDeviceMemoryProperties) {
        pfn_vkGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
    }
}

void GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, 
                                            uint32_t* pQueueFamilyPropertyCount, 
                                            VkQueueFamilyProperties* pQueueFamilyProperties)
{
    if (pfn_vkGetPhysicalDeviceQueueFamilyProperties) {
        pfn_vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
}


void QueryPresentationProperties(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDevicePresentationPropertiesOpenHarmony* presentation_properties) {
    VkPhysicalDeviceProperties2 properties = {
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        presentation_properties,
        {},
    };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
    presentation_properties->sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTATION_PROPERTIES_OPENHARMONY;
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

// VkResult AcquireImageOHOS(VkDevice device,
VkResult SetNativeFenceFdOpenHarmony(VkDevice device,
    int nativeFenceFd,
    VkSemaphore semaphore,
    VkFence fence)
{
    PFN_vkSetNativeFenceFdOpenHarmony acquireImage = reinterpret_cast<PFN_vkSetNativeFenceFdOpenHarmony>(GetDeviceProcAddr(device, "vkSetNativeFenceFdOpenHarmony"));
    if (acquireImage) {
        return acquireImage(device, nativeFenceFd, semaphore, fence);
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

VkResult GetNativeFenceFdOpenHarmony(
    VkQueue queue,
    uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores,
    VkImage image,
    int* pNativeFenceFd)
{
    if (pfn_vkGetNativeFenceFdOpenHarmony) {
        return pfn_vkGetNativeFenceFdOpenHarmony(queue, waitSemaphoreCount, pWaitSemaphores, image, pNativeFenceFd);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}







}  // namespace driver
}  // namespace vulkan
