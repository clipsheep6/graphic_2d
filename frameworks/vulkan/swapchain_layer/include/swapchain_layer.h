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

#ifndef SWAPCHAIN_LAYER_H
#define SWAPCHAIN_LAYER_H

#include <cstring>
#include <unordered_map>
#include <vulkan/vulkan.h>

#define SWAPCHAIN_SURFACE_NAME "VK_LAYER_OpenHarmony_OHOS_surface"
#define VK_LAYER_API_VERSION VK_MAKE_VERSION(1, 1, VK_HEADER_VERSION)

namespace OHOS::SWAPCHAIN {
namespace {
static const VkExtensionProperties instanceExtensions[] = {
    {
        .extensionName = VK_KHR_SURFACE_EXTENSION_NAME,
        .specVersion = 25,
    },
    {
        .extensionName = VK_OPENHARMONY_OHOS_SURFACE_EXTENSION_NAME,
        .specVersion = 1,
    }
};

static const VkExtensionProperties deviceExtensions[] = {{
    .extensionName = VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    .specVersion = 70,
}};

constexpr VkLayerProperties swapchainLayer = {
    SWAPCHAIN_SURFACE_NAME,
    VK_LAYER_API_VERSION,
    1,
    "Vulkan Swapchain",
};
}

VKAPI_ATTR VkResult VKAPI_CALL CreateSwapchainKHR(
    VkDevice device,
    const VkSwapchainCreateInfoKHR* createInfo,
    const VkAllocationCallbacks* allocator,
    VkSwapchainKHR* swapchainHandle
);
VKAPI_ATTR void VKAPI_CALL DestroySwapchainKHR(
    VkDevice device,
    VkSwapchainKHR vkSwapchain,
    const VkAllocationCallbacks* pAllocator
);
VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainImagesKHR(
    VkDevice device,
    VkSwapchainKHR vkSwapchain,
    uint32_t* count,
    VkImage* images
);
VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImageKHR(
    VkDevice device,
    VkSwapchainKHR swapchainHandle,
    uint64_t timeout,
    VkSemaphore semaphore,
    VkFence vkFence,
    uint32_t* imageIndex
);
VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImage2KHR(
    VkDevice device,
    const VkAcquireNextImageInfoKHR* pAcquireInfo,
    uint32_t* pImageIndex
);
VKAPI_ATTR VkResult VKAPI_CALL QueuePresentKHR(
    VkQueue queue,
    const VkPresentInfoKHR* presentInfo
);
VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    const VkSurfaceKHR surface,
    VkBool32* pSupported
);
VKAPI_ATTR VkResult VKAPI_CALL CreateOHOSSurfaceOpenHarmony(
    VkInstance instance,
    const VkOHOSSurfaceCreateInfoOpenHarmony* pCreateInfo,
    const VkAllocationCallbacks* allocator,
    VkSurfaceKHR* outSurface
);
VKAPI_ATTR void VKAPI_CALL DestroySurfaceKHR(
    VkInstance instance,
    VkSurfaceKHR vkSurface,
    const VkAllocationCallbacks* pAllocator
);
VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR* capabilities
);
VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice,
    const VkSurfaceKHR surface,
    uint32_t* count,
    VkSurfaceFormatKHR* formats
);
VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice,
    const VkSurfaceKHR surface,
    uint32_t* count,
    VkPresentModeKHR* pPresentModes
);
VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(
    const VkInstanceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkInstance* pInstance
);
VKAPI_ATTR void VKAPI_CALL DestroyInstance(
    VkInstance instance,
    const VkAllocationCallbacks* pAllocator
);
VKAPI_ATTR VkResult VKAPI_CALL CreateDevice(
    VkPhysicalDevice gpu,
    const VkDeviceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDevice* pDevice
);
VKAPI_ATTR void VKAPI_CALL DestroyDevice(
    VkDevice device,
    const VkAllocationCallbacks* pAllocator
);
VKAPI_ATTR VkResult VKAPI_CALL CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pMessenger
);
VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    const VkAllocationCallbacks* pAllocator
);
VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(
    const char* pLayerName,
    uint32_t* pCount,
    VkExtensionProperties* pProperties
);
VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties(
    uint32_t* pCount,
    VkLayerProperties* pProperties
);
VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice,
    uint32_t* pCount,
    VkLayerProperties* pProperties
);
VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice,
    const char* pLayerName,
    uint32_t* pCount,
    VkExtensionProperties* pProperties
);
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetPhysicalDeviceProcAddr(
    VkInstance instance,
    const char* funcName
);
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(
    VkDevice device,
    const char* funcName
);
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(
    VkInstance instance,
    const char* funcName
);

struct FuncNameHash {
    std::size_t operator()(const char* key) const
    {
        return std::hash<std::string>()(key);
    }
};

struct FuncNameCmp {
    bool operator()(const char* name1, const char* name2) const
    {
        return std::strcmp(name1, name2) == 0;
    }
};

const std::unordered_map<const char*, PFN_vkVoidFunction, FuncNameHash, FuncNameCmp> layerInterceptProcMap = {
    {"vkCreateSwapchainKHR", reinterpret_cast<PFN_vkVoidFunction>(CreateSwapchainKHR)},
    {"vkDestroySwapchainKHR", reinterpret_cast<PFN_vkVoidFunction>(DestroySwapchainKHR)},
    {"vkGetSwapchainImagesKHR", reinterpret_cast<PFN_vkVoidFunction>(GetSwapchainImagesKHR)},
    {"vkAcquireNextImageKHR", reinterpret_cast<PFN_vkVoidFunction>(AcquireNextImageKHR)},
    {"vkAcquireNextImage2KHR", reinterpret_cast<PFN_vkVoidFunction>(AcquireNextImage2KHR)},
    {"vkQueuePresentKHR", reinterpret_cast<PFN_vkVoidFunction>(QueuePresentKHR)},
    {"vkGetPhysicalDeviceSurfaceSupportKHR", reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfaceSupportKHR)},
    {"vkCreateOHOSSurfaceOpenHarmony", reinterpret_cast<PFN_vkVoidFunction>(CreateOHOSSurfaceOpenHarmony)},
    {"vkDestroySurfaceKHR", reinterpret_cast<PFN_vkVoidFunction>(DestroySurfaceKHR)},
    {"vkGetPhysicalDeviceSurfaceCapabilitiesKHR", reinterpret_cast<PFN_vkVoidFunction>(
        GetPhysicalDeviceSurfaceCapabilitiesKHR)},
    {"vkGetPhysicalDeviceSurfaceFormatsKHR", reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfaceFormatsKHR)},
    {"vkGetPhysicalDeviceSurfacePresentModesKHR", reinterpret_cast<PFN_vkVoidFunction>(
        GetPhysicalDeviceSurfacePresentModesKHR)},
    {"vkCreateInstance", reinterpret_cast<PFN_vkVoidFunction>(CreateInstance)},
    {"vkDestroyInstance", reinterpret_cast<PFN_vkVoidFunction>(DestroyInstance)},
    {"vkCreateDevice", reinterpret_cast<PFN_vkVoidFunction>(CreateDevice)},
    {"vkDestroyDevice", reinterpret_cast<PFN_vkVoidFunction>(DestroyDevice)},
    {"vkCreateDebugUtilsMessengerEXT", reinterpret_cast<PFN_vkVoidFunction>(CreateDebugUtilsMessengerEXT)},
    {"vkDestroyDebugUtilsMessengerEXT", reinterpret_cast<PFN_vkVoidFunction>(DestroyDebugUtilsMessengerEXT)},
    {"vkEnumerateInstanceExtensionProperties", reinterpret_cast<PFN_vkVoidFunction>(
        EnumerateInstanceExtensionProperties)},
    {"vkEnumerateInstanceLayerProperties", reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceLayerProperties)},
    {"vkEnumerateDeviceLayerProperties", reinterpret_cast<PFN_vkVoidFunction>(EnumerateDeviceLayerProperties)},
    {"vkEnumerateDeviceExtensionProperties", reinterpret_cast<PFN_vkVoidFunction>(EnumerateDeviceExtensionProperties)},
    {"vkGetPhysicalDeviceProcAddr", reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceProcAddr)},
    {"vkGetDeviceProcAddr", reinterpret_cast<PFN_vkVoidFunction>(GetDeviceProcAddr)},
    {"vkGetInstanceProcAddr", reinterpret_cast<PFN_vkVoidFunction>(GetInstanceProcAddr)}
};
} // namespace OHOS::SWAPCHAIN

extern "C" {
VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
    const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    return OHOS::SWAPCHAIN::EnumerateInstanceExtensionProperties(pLayerName, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t* pCount, VkLayerProperties* pProperties)
{
    return OHOS::SWAPCHAIN::EnumerateInstanceLayerProperties(pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t* pCount, VkLayerProperties* pProperties)
{
    return OHOS::SWAPCHAIN::EnumerateDeviceLayerProperties(VK_NULL_HANDLE, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    return OHOS::SWAPCHAIN::EnumerateDeviceExtensionProperties(VK_NULL_HANDLE, pLayerName, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice dev, const char* funcName)
{
    return OHOS::SWAPCHAIN::GetDeviceProcAddr(dev, funcName);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetInstanceProcAddr(VkInstance instance, const char* funcName)
{
    return OHOS::SWAPCHAIN::GetInstanceProcAddr(instance, funcName);
}
}
#endif // SWAPCHAIN_LAYER_H
