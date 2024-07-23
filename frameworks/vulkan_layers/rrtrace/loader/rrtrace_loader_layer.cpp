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

#include "rrtrace_loader_helper.h"
#include "rrtrace_loader.h"

#include "vk_layer.h"

namespace rrtrace {

VkResult EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
    return VK_ERROR_UNKNOWN;
}

VkResult EnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
    return EnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}

VkResult EnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,
    VkExtensionProperties* pProperties)
{
    return VK_ERROR_UNKNOWN;
}

VkResult FillPropertyCountAndList(const VkExtensionProperties *src, uint32_t numExts, uint32_t *dstCount,
                                  VkExtensionProperties *dstProps)
{
    return VK_ERROR_UNKNOWN;
}

VkResult EnumerateInstanceExtensionProperties(const VkEnumerateInstanceExtensionPropertiesChain *pChain,
                                              const char *pLayerName, uint32_t *pPropertyCount,
                                              VkExtensionProperties *pProperties)
{
    return EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}

VkResult EnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char *pLayerName,
    uint32_t *pPropertyCount, VkExtensionProperties *pProperties)
{
    return VK_ERROR_UNKNOWN;
}

VkLayerInstanceCreateInfo *GetChainInfo(const VkInstanceCreateInfo *pCreateInfo, VkLayerFunction func)
{
    return nullptr;
}

VkLayerDeviceCreateInfo *GetChainInfo(const VkDeviceCreateInfo *pCreateInfo, VkLayerFunction func)
{
    return nullptr;
}

VkResult CreateInstance(const VkInstanceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkInstance *pInstance)
{
    return VK_ERROR_UNKNOWN;
}

void DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {}

VkResult CreateDevice(VkPhysicalDevice physDev, const VkDeviceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
    return VK_ERROR_UNKNOWN;
}

void DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {}

} // namespace rrtrace

extern "C" {
// Vk Layer Entries

RRTRACE_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
    return nullptr;
}

RRTRACE_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char *pName)
{
    return nullptr;
}

VkResult EnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties)
{
    return rrtrace::EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}

VkResult EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
    return rrtrace::EnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}

RRTRACE_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_RRTRACE_CaptureLoaderGetInstanceProcAddr(
    VkInstance instance, const char *pName)
{
    return nullptr;
}

RRTRACE_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_RRTRACE_CaptureLoaderGetDeviceProcAddr(
    VkDevice device, const char *pName)
{
    return nullptr;
}

RRTRACE_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_RRTRACE_CaptureLoader_layerGetPhysicalDeviceProcAddr(
    VkInstance instance, const char *pName)
{
    return nullptr;
}

RRTRACE_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL VK_LAYER_RRTRACE_CaptureLoaderNegotiateLoaderLayerInterfaceVersion(
    VkNegotiateLayerInterface *pVersionStruct)
{
    return VK_ERROR_UNKNOWN;
}

typedef VkResult(VKAPI_PTR *PFN_vkEnumerateInstanceExtensionPropertiesChain)(
    const VkEnumerateInstanceExtensionPropertiesChain *pChain, const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties);

RRTRACE_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL VK_LAYER_RRTRACE_CaptureLoaderEnumerateInstanceExtensionProperties(
    const VkEnumerateInstanceExtensionPropertiesChain *pChain, const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties)
{
    return VK_ERROR_UNKNOWN;
}

// GLES Layer Entries

typedef void (*eglMustCastToProperFunctionPointerType)(void);

typedef eglMustCastToProperFunctionPointerType(*PFNEGLGETNEXTLAYERPROCADDRESSPROC)(
    void *, const char *funcName);

typedef void*(*PFNEGLGETNEXTLAYERPROCADDRESSPROCOHOS)(
    void *, const char *funcName);

typedef void* (* DebugLayerInitializeType)(const void *funcTable,
    PFNEGLGETNEXTLAYERPROCADDRESSPROCOHOS next);
typedef void* (* DebugLayerGetProcAddrType)(const char *funcName,
    eglMustCastToProperFunctionPointerType next);

RRTRACE_LOADER_API
void* DebugLayerInitialize(const void *funcTable, PFNEGLGETNEXTLAYERPROCADDRESSPROCOHOS next)
{
    return nullptr;
}

RRTRACE_LOADER_API
void* DebugLayerGetProcAddr(const char *funcName, eglMustCastToProperFunctionPointerType next)
{
    return nullptr;
}

} // extern "C"
