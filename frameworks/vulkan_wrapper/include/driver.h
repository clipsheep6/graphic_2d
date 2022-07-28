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

#ifndef LIBVULKAN_DRIVER_H
#define LIBVULKAN_DRIVER_H 1

#include "vulkan/vulkan.h"
#include "vk_ohos_native_buffer.h"


namespace vulkan {
namespace driver {

VKAPI_ATTR VkResult CreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance);
VKAPI_ATTR VkResult EnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,VkExtensionProperties* pProperties);
VKAPI_ATTR PFN_vkVoidFunction GetInstanceProcAddr(VkInstance instance, const char* pName);
VKAPI_ATTR void DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator);
VKAPI_ATTR VkResult CreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator,
                                 VkDevice* pDevice);
VKAPI_ATTR void GetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties);
VKAPI_ATTR VkResult EnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices);
VKAPI_ATTR PFN_vkVoidFunction GetDeviceProcAddr(VkDevice device, const char* pName);
VKAPI_ATTR void GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties);
VKAPI_ATTR void GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties);
VKAPI_ATTR void GetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties);




}  // namespace driver
}  // namespace vulkan

#endif  // LIBVULKAN_DRIVER_H
