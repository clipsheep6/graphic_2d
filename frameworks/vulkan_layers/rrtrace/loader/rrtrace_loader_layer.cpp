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

#define RRTRACE_CAPTURE_LOADER_LAYER_NAME  "VK_LAYER_RRTRACE_CaptureLoader"

#define RRTRACE_CAPTURE_LOADER_DESCRIPTION "RrTrace Capture Layer Loader Verison 0.1.0"
#define RRTRACE_CAPTURE_LOADER_VERSION_MAJOR_VALUE 0
#define RRTRACE_CAPTURE_LOADER_VERSION_MINOR_VALUE 1
#define RRTRACE_CAPTURE_LOADER_VERSION_PATCH_VALUE 0

static const VkLayerProperties layerProps = {
    RRTRACE_CAPTURE_LOADER_LAYER_NAME,
    VK_HEADER_VERSION_COMPLETE,
    VK_MAKE_VERSION(RRTRACE_CAPTURE_LOADER_VERSION_MAJOR_VALUE,
                    RRTRACE_CAPTURE_LOADER_VERSION_MINOR_VALUE,
                    RRTRACE_CAPTURE_LOADER_VERSION_PATCH_VALUE),
    RRTRACE_CAPTURE_LOADER_DESCRIPTION
};

namespace rrtrace {

VkResult EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
    VkResult Result = VK_SUCCESS;
    if (pProperties == nullptr) {
        if (pPropertyCount != nullptr) {
            *pPropertyCount = 1;
        }
    } else if ((pPropertyCount != nullptr) && (*pPropertyCount >= 1)) {
        errno_t ret = memcpy_s(pProperties, sizeof(VkLayerProperties), &layerProps, sizeof(layerProps));
        if (ret != EOK) {
            RRTRACE_LOGE("%s:%d ERROR: memcpy_s! Error code: %d\n", __FUNCTION__, __LINE__, ret);
        }
        *pPropertyCount = 1;
    } else {
        Result = VK_INCOMPLETE;
    }
    return Result;
}

VkResult EnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
    (void)physicalDevice;
    return EnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}

VkResult EnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,
    VkExtensionProperties* pProperties)
{
    VkResult Result = VK_SUCCESS;
    if (pLayerName && strcmp(pLayerName, layerProps.layerName) == 0) {
        if (pPropertyCount != nullptr)
            *pPropertyCount = 0;
    } else {
        Result = VK_ERROR_LAYER_NOT_PRESENT;
    }
    return Result;
}

VkResult FillPropertyCountAndList(const VkExtensionProperties *src, uint32_t numExts, uint32_t *dstCount,
                                  VkExtensionProperties *dstProps)
{
    if (dstCount && !dstProps) {
        // just returning the number of extensions
        *dstCount = numExts;
        return VK_SUCCESS;
    } else if (dstCount && dstProps) {
        uint32_t dstSpace = *dstCount;

        // return the number of extensions.
        *dstCount = (numExts < dstSpace) ? numExts : dstSpace;

        // copy as much as there's space for, up to how many there are
        if (src) {
            errno_t ret = memcpy_s(dstProps, sizeof(VkExtensionProperties) * dstSpace, src,
                sizeof(VkExtensionProperties) * ((numExts < dstSpace) ? numExts : dstSpace));
            if (ret != EOK) {
                RRTRACE_LOGE("%s:%d ERROR: memcpy_s! Error code: %d\n", __FUNCTION__, __LINE__, ret);
            }
        }

        // if there was enough space, return success, else incomplete
        if (dstSpace >= numExts) {
            return VK_SUCCESS;
        }
    }
    // both parameters were NULL, return incomplete
    return VK_INCOMPLETE;
}

VkResult EnumerateInstanceExtensionProperties(const VkEnumerateInstanceExtensionPropertiesChain *pChain,
                                              const char *pLayerName, uint32_t *pPropertyCount,
                                              VkExtensionProperties *pProperties)
{
    VkResult Result = VK_ERROR_LAYER_NOT_PRESENT;

    if (pChain && !pLayerName) {
        // first fetch the list of extensions ourselves
        uint32_t numExts = 0;

        Result = pChain->CallDown(pLayerName, &numExts, NULL);
        if (Result == VK_SUCCESS) {
            std::vector<VkExtensionProperties> exts;
            exts.resize(numExts);
            Result = pChain->CallDown(pLayerName, &numExts, exts.data());
            if (Result == VK_SUCCESS) {
                Result = FillPropertyCountAndList(exts.data(), exts.size(), pPropertyCount, pProperties);
            }
        }
    } else {
        Result = EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
    }

    return Result;
}

typedef const void *DispatchKey;

static inline DispatchKey GetDispatchKey(const void *handle)
{
    const DispatchKey *dispatchKey = reinterpret_cast<const DispatchKey *>(handle);
    return (*dispatchKey);
}

struct InstanceInfo {
    VkInstance instance{nullptr};
    PFN_vkGetInstanceProcAddr getInstanceProcAddr{nullptr};
    PFN_vkEnumerateDeviceExtensionProperties enumerateDeviceExtensionProperties{nullptr};
    PFN_vkDestroyInstance destroyInstance{nullptr};
};

struct DeviceInfo {
    VkDevice device{nullptr};
    PFN_vkGetDeviceProcAddr getDeviceProcAddr{nullptr};
    PFN_vkDestroyDevice destroyDevice{nullptr};
};

std::map<DispatchKey, InstanceInfo> instanceMap;
std::map<DispatchKey, DeviceInfo> deviceMap;

VkResult EnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char *pLayerName,
    uint32_t *pPropertyCount, VkExtensionProperties *pProperties)
{
    VkResult Result = VK_ERROR_LAYER_NOT_PRESENT;

    if (pLayerName && strcmp(pLayerName, layerProps.layerName) == 0) {
        if (pPropertyCount != nullptr)
            *pPropertyCount = 0;

        Result = VK_SUCCESS;
    } else {
        DispatchKey physKey = GetDispatchKey(physicalDevice);

        auto it = instanceMap.find(physKey);
        if (it != instanceMap.end()) {
            auto &instInfo = it->second;

            if (instInfo.enumerateDeviceExtensionProperties)
                Result = instInfo.enumerateDeviceExtensionProperties(physicalDevice, nullptr,
                    pPropertyCount, pProperties);
        }
    }

    RRTRACE_LOGI("%s(physicalDevice:%p, pLayerName:%p) --> vkresult:%d\n",
        __FUNCTION__, physicalDevice, pLayerName, Result);
    return Result;
}

VkLayerInstanceCreateInfo *GetChainInfo(const VkInstanceCreateInfo *pCreateInfo, VkLayerFunction func)
{
    auto chainInfo = static_cast<const VkLayerInstanceCreateInfo *>(pCreateInfo->pNext);
    while (chainInfo != nullptr) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerInstanceCreateInfo *>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerInstanceCreateInfo *>(chainInfo->pNext);
    }
    RRTRACE_LOGE("%s", "ERROR: Find VkLayerInstanceCreateInfo Failed");
    return nullptr;
}

VkLayerDeviceCreateInfo *GetChainInfo(const VkDeviceCreateInfo *pCreateInfo, VkLayerFunction func)
{
    auto chainInfo = static_cast<const VkLayerDeviceCreateInfo *>(pCreateInfo->pNext);
    while (chainInfo != nullptr) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerDeviceCreateInfo *>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerDeviceCreateInfo *>(chainInfo->pNext);
    }
    RRTRACE_LOGE("%s", "ERROR: Find VkLayerDeviceCreateInfo Failed");
    return nullptr;
}

VkResult CreateInstance(const VkInstanceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkInstance *pInstance)
{
    VkResult Result = VK_ERROR_INITIALIZATION_FAILED;
    VkLayerInstanceCreateInfo *chainInfo = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);
    if (chainInfo == nullptr || chainInfo->u.pLayerInfo == nullptr) {
        RRTRACE_LOGE("%s:%d() --> vkresult:%d\n", __FUNCTION__, __LINE__, Result);
        return Result;
    }
    auto fpGetInstanceProcAddr = chainInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    auto fpCreateInstance = (PFN_vkCreateInstance)fpGetInstanceProcAddr(nullptr, "vkCreateInstance");
    if (!fpCreateInstance) {
        RRTRACE_LOGE("%s:%d() --> vkresult:%d\n", __FUNCTION__, __LINE__, Result);
        return Result;
    }

    chainInfo->u.pLayerInfo = chainInfo->u.pLayerInfo->pNext;

    Result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (Result != VK_SUCCESS) {
        return Result;
    }
    VkInstance instance = *pInstance;
    DispatchKey instanceKey = GetDispatchKey(instance);

    auto fpEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)fpGetInstanceProcAddr(
        instance, "vkEnumerateDeviceExtensionProperties");
    auto fpDestroyInstance = (PFN_vkDestroyInstance)fpGetInstanceProcAddr(instance, "vkDestroyInstance");

    instanceMap[GetDispatchKey(*pInstance)] = InstanceInfo{ instance,
        fpGetInstanceProcAddr,
        fpEnumerateDeviceExtensionProperties,
        fpDestroyInstance } ;

    RRTRACE_LOGI("%s:%d() --> instance:%p, instanceKey:%p, vkresult:%d\n",
        __FUNCTION__, __LINE__, instance, instanceKey, Result);
    return Result;
}

void DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey instanceKey = GetDispatchKey(instance);

    auto it = instanceMap.find(instanceKey);
    if (it != instanceMap.end()) {
        auto &instInfo = it->second;

        RRTRACE_LOGI("%s(instance:%p) instanceKey:%p, destroyInstance:%p\n",
            __FUNCTION__, instance, instanceKey, instInfo.destroyInstance);
        if (instInfo.destroyInstance)
            instInfo.destroyInstance(instance, pAllocator);
        instanceMap.erase(instanceKey);
    }
}

VkResult CreateDevice(VkPhysicalDevice physDev, const VkDeviceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
    VkResult Result = VK_ERROR_INITIALIZATION_FAILED;
    const DispatchKey physKey = GetDispatchKey(physDev);

    const InstanceInfo instanceInfo = instanceMap[physKey];
    const VkInstance instance = instanceInfo.instance;

    VkLayerDeviceCreateInfo *linkInfo = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    if (linkInfo == nullptr || linkInfo->u.pLayerInfo == nullptr || instance == nullptr) {
        RRTRACE_LOGE("%s:%d(physDev:%p) physKey:%p --> vkresult:%d\n",
            __FUNCTION__, __LINE__, physDev, physKey, Result);
        return Result;
    }
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = linkInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = linkInfo->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice =
        (PFN_vkCreateDevice)fpGetInstanceProcAddr(instance, "vkCreateDevice");
    if (!fpCreateDevice) {
        RRTRACE_LOGE("%s:%d(physDev:%p) physKey:%p, instance:%p --> vkresult:%d\n",
            __FUNCTION__, __LINE__, physDev, physKey, instance, Result);
        return Result;
    }

    linkInfo->u.pLayerInfo = linkInfo->u.pLayerInfo->pNext;

    Result = fpCreateDevice(physDev, pCreateInfo, pAllocator, pDevice);
    if (Result != VK_SUCCESS) {
        RRTRACE_LOGE("%s:%d(physDev:%p) physKey:%p, instance:%p --> device:%p, vkresult:%d\n",
            __FUNCTION__, __LINE__, physDev, physKey, instance, *pDevice, Result);
        return Result;
    }
    VkDevice device = *pDevice;
    DispatchKey deviceKey = GetDispatchKey(device);

    PFN_vkDestroyDevice fpDestroyDevice = (PFN_vkDestroyDevice)fpGetDeviceProcAddr(device, "vkDestroyDevice");

    deviceMap[deviceKey] = DeviceInfo{device, fpGetDeviceProcAddr, fpDestroyDevice};

    RRTRACE_LOGI("%s(physDev:%p) physKey:%p, instance:%p --> device:%p, deviceKey:%p, vkresult:%d\n",
        __FUNCTION__, physDev, physKey, instance, device, deviceKey, Result);
    return VK_SUCCESS;
}

void DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey deviceKey = GetDispatchKey(device);

    auto it = deviceMap.find(deviceKey);
    if (it != deviceMap.end()) {
        auto &devInfo = it->second;

        RRTRACE_LOGI("%s(device:%p) deviceKey:%p, destroyDevice:%p\n",
            __FUNCTION__, device, deviceKey, devInfo.destroyDevice);
        if (devInfo.destroyDevice)
            devInfo.destroyDevice(device, pAllocator);
        deviceMap.erase(deviceKey);
    }
}

} // namespace rrtrace

extern "C" {
// Vk Layer Entries

PFN_vkGetInstanceProcAddr rrtraceVkGetInstanceProcAddr = nullptr;
PFN_vkGetDeviceProcAddr rrtraceVkGetDeviceProcAddr = nullptr;
PFN_vkGetInstanceProcAddr rrtraceVkGetPhysicalDeviceProcAddr = nullptr;

RRTRACE_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
    return rrtraceVkGetInstanceProcAddr ? rrtraceVkGetInstanceProcAddr(instance, pName) : nullptr;
}

RRTRACE_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char *pName)
{
    return rrtraceVkGetDeviceProcAddr ? rrtraceVkGetDeviceProcAddr(device, pName) : nullptr;
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
    PFN_vkVoidFunction procAddr = nullptr;

    if (strcmp("vkGetInstanceProcAddr", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)VK_LAYER_RRTRACE_CaptureLoaderGetInstanceProcAddr;
    } else if (strcmp("vkEnumerateInstanceExtensionProperties", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceExtensionProperties);
    } else if (strcmp("vkEnumerateInstanceLayerProperties", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceLayerProperties);
    } else if (strcmp("vkCreateInstance", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(rrtrace::CreateInstance);
    } else if (strcmp("vkDestroyInstance", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(rrtrace::DestroyInstance);
    } else if (strcmp("vkCreateDevice", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(rrtrace::CreateDevice);
    } else if (strcmp("vkEnumerateDeviceExtensionProperties", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(rrtrace::EnumerateDeviceExtensionProperties);
    } else {
        if (instance == VK_NULL_HANDLE) {
            RRTRACE_LOGE("%s(instance:%p, pName:'%s') ERROR: instance is null --> %p\n",
                __FUNCTION__, instance, pName, procAddr);
            return procAddr;
        }
        rrtrace::DispatchKey instanceKey = rrtrace::GetDispatchKey(instance);

        auto it = rrtrace::instanceMap.find(instanceKey);
        if (it != rrtrace::instanceMap.end()) {
            procAddr = it->second.getInstanceProcAddr(instance, pName);
        }
    }
    return procAddr;
}

RRTRACE_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_RRTRACE_CaptureLoaderGetDeviceProcAddr(
    VkDevice device, const char *pName)
{
    PFN_vkVoidFunction procAddr = nullptr;

    if (device == VK_NULL_HANDLE) {
        RRTRACE_LOGE("%s(device:%p, pName:'%s') ERROR: device is null--> %p\n", __FUNCTION__, device, pName, procAddr);
        return procAddr;
    }

    if (strcmp("vkGetDeviceProcAddr", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)VK_LAYER_RRTRACE_CaptureLoaderGetDeviceProcAddr;
    } else if (strcmp("vkDestroyDevice", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)rrtrace::DestroyDevice;
    } else {
        rrtrace::DispatchKey deviceKey = rrtrace::GetDispatchKey(device);

        auto it = rrtrace::deviceMap.find(deviceKey);
        if (it != rrtrace::deviceMap.end()) {
            procAddr = it->second.getDeviceProcAddr(device, pName);
        }
    }
    return procAddr;
}

RRTRACE_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_RRTRACE_CaptureLoader_layerGetPhysicalDeviceProcAddr(
    VkInstance instance, const char *pName)
{
    PFN_vkVoidFunction procAddr = nullptr;

    if (strcmp("vk_layerGetPhysicalDeviceProcAddr", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)VK_LAYER_RRTRACE_CaptureLoader_layerGetPhysicalDeviceProcAddr;
    } else {
        procAddr = VK_LAYER_RRTRACE_CaptureLoaderGetInstanceProcAddr(instance, pName);
    }
    RRTRACE_LOGI("%s(instance:%p, pName:'%s') --> %p\n", __FUNCTION__, instance, pName, procAddr);
    return procAddr;
}

RRTRACE_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL VK_LAYER_RRTRACE_CaptureLoaderNegotiateLoaderLayerInterfaceVersion(
    VkNegotiateLayerInterface *pVersionStruct)
{
    VkResult Result = VK_ERROR_INITIALIZATION_FAILED;

    RRTRACE_LOGI("%s(pVerStruct:%p)...\n", __FUNCTION__, pVersionStruct);

    void *libHandle = rrtrace::CaptureInit();
    if (libHandle) {
        auto *pfnNegotiate = (PFN_vkNegotiateLoaderLayerInterfaceVersion)dlsym(libHandle,
            "VK_LAYER_RRTRACE_CaptureNegotiateLoaderLayerInterfaceVersion");
        if (pfnNegotiate) {
            Result = pfnNegotiate(pVersionStruct);
            if (Result == VK_SUCCESS) {
                rrtraceVkGetInstanceProcAddr = pVersionStruct->pfnGetInstanceProcAddr;
                rrtraceVkGetDeviceProcAddr = pVersionStruct->pfnGetDeviceProcAddr;
                rrtraceVkGetPhysicalDeviceProcAddr = pVersionStruct->pfnGetPhysicalDeviceProcAddr;
            }
        }
    } else {
        if (pVersionStruct && pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT) {
            rrtraceVkGetInstanceProcAddr = VK_LAYER_RRTRACE_CaptureLoaderGetInstanceProcAddr;
            rrtraceVkGetDeviceProcAddr = VK_LAYER_RRTRACE_CaptureLoaderGetDeviceProcAddr;
            rrtraceVkGetPhysicalDeviceProcAddr = VK_LAYER_RRTRACE_CaptureLoader_layerGetPhysicalDeviceProcAddr;

            pVersionStruct->pfnGetInstanceProcAddr = rrtraceVkGetInstanceProcAddr;
            pVersionStruct->pfnGetDeviceProcAddr = rrtraceVkGetDeviceProcAddr;
            pVersionStruct->pfnGetPhysicalDeviceProcAddr = rrtraceVkGetPhysicalDeviceProcAddr;

            if (pVersionStruct->loaderLayerInterfaceVersion > CURRENT_LOADER_LAYER_INTERFACE_VERSION) {
                pVersionStruct->loaderLayerInterfaceVersion = CURRENT_LOADER_LAYER_INTERFACE_VERSION;
            }
            Result = VK_SUCCESS;
        }
    }

    RRTRACE_LOGI("%s(pVerStruct:%p) ifaceVer:%d, libHandle:%p --> vkresult:%d"
        "(pVerStruct:{ sType:%d, ifaceVer:%d, pfnGIPA:%p, pfnGDPA:%p, pfnGPDPA:%p })\n",
        __FUNCTION__,
        pVersionStruct, (pVersionStruct ? (int)pVersionStruct->loaderLayerInterfaceVersion : 0), libHandle, Result,
        (pVersionStruct ? (int)pVersionStruct->sType : -1),
        (pVersionStruct ? (int)pVersionStruct->loaderLayerInterfaceVersion : 0),
        (pVersionStruct ? pVersionStruct->pfnGetInstanceProcAddr : nullptr),
        (pVersionStruct ? pVersionStruct->pfnGetDeviceProcAddr : nullptr),
        (pVersionStruct ? pVersionStruct->pfnGetPhysicalDeviceProcAddr : nullptr)
    );
    return Result;
}

typedef VkResult(VKAPI_PTR *PFN_vkEnumerateInstanceExtensionPropertiesChain)(
    const VkEnumerateInstanceExtensionPropertiesChain *pChain, const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties);

RRTRACE_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL VK_LAYER_RRTRACE_CaptureLoaderEnumerateInstanceExtensionProperties(
    const VkEnumerateInstanceExtensionPropertiesChain *pChain, const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties)
{
    VkResult Result = VK_INCOMPLETE;

    void *libHandle = rrtrace::CaptureInit();
    if (libHandle) {
        PFN_vkEnumerateInstanceExtensionPropertiesChain pfnEnumerate =
            (PFN_vkEnumerateInstanceExtensionPropertiesChain)dlsym(
                libHandle, "VK_LAYER_RRTRACE_CaptureEnumerateInstanceExtensionProperties");
        if (pfnEnumerate) {
            Result = pfnEnumerate(pChain, pLayerName, pPropertyCount, pProperties);
        }
    } else {
        Result = rrtrace::EnumerateInstanceExtensionProperties(pChain, pLayerName, pPropertyCount, pProperties);
    }

    RRTRACE_LOGI("%s(pChain:%p, pLayerName:'%s', pPropCount:%p, pProps:%p) --> vkresult:%d (*pPropCount:%u)",
        __FUNCTION__, pChain, pLayerName, pPropertyCount, pProperties, Result,
        pPropertyCount ? *pPropertyCount : 0);
    return Result;
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

DebugLayerInitializeType g_rrtraceInitialize = nullptr;
DebugLayerGetProcAddrType g_rrtraceLayerGetProcAddr = nullptr;

RRTRACE_LOADER_API
void* DebugLayerInitialize(const void *funcTable, PFNEGLGETNEXTLAYERPROCADDRESSPROCOHOS next)
{
    void *ret = nullptr;
    void *libHandle = rrtrace::CaptureInit();

    if (libHandle) {
        if (!g_rrtraceInitialize) {
            g_rrtraceInitialize = (DebugLayerInitializeType)dlsym(libHandle, "DebugLayerInitialize");
        }
        if (g_rrtraceInitialize && !g_rrtraceLayerGetProcAddr) {
            g_rrtraceLayerGetProcAddr = (DebugLayerGetProcAddrType)dlsym(libHandle, "DebugLayerGetProcAddr");
            if (!g_rrtraceLayerGetProcAddr) {
                g_rrtraceInitialize = nullptr;
            } else {
                RRTRACE_LOGI("%s:%d init:%p, getProcAddr:%p\n", __FUNCTION__, __LINE__,
                    g_rrtraceInitialize, g_rrtraceLayerGetProcAddr);
            }
        }

        if (g_rrtraceInitialize) {
            ret = g_rrtraceInitialize(funcTable, next);
        }
    }
    RRTRACE_LOGI("%s:%d funcTable:%p next:%p libHandle:%p\n", __FUNCTION__, __LINE__, funcTable, next, libHandle);
    return ret;
}

RRTRACE_LOADER_API
void* DebugLayerGetProcAddr(const char *funcName, eglMustCastToProperFunctionPointerType next)
{
    void *rrtracePtr = (void *)next;

    if (g_rrtraceLayerGetProcAddr) {
        rrtracePtr = g_rrtraceLayerGetProcAddr(funcName, next);
    }
    return static_cast<void *>(rrtracePtr);
}

} // extern "C"
