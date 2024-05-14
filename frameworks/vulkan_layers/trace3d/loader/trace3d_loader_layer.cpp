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

//
// Desc: This file is a part of the Trace3D Capture Gles/Vk Layer Loader
//

#include "trace3d_loader_helper.h"
#include "trace3d_loader.h"

#include "vk_layer.h"

#define TRACE3D_CAPTURE_LOADER_LAYER_NAME  "VK_LAYER_TRACE3D_CaptureLoader"

#define TRACE3D_CAPTURE_LOADER_DESCRIPTION "Trace3D Capture Layer Loader"
#define TRACE3D_CAPTURE_LOADER_VERSION_MAJOR_VALUE 0
#define TRACE3D_CAPTURE_LOADER_VERSION_MINOR_VALUE 1
#define TRACE3D_CAPTURE_LOADER_VERSION_PATCH_VALUE 0

#define TRACE3D_VERSION_STR_EXPAND(x) #x
#define TRACE3D_VERSION_STR(x) TRACE3D_VERSION_STR_EXPAND(x)

static const VkLayerProperties layerProps = {
    TRACE3D_CAPTURE_LOADER_LAYER_NAME,
    VK_HEADER_VERSION_COMPLETE,
    VK_MAKE_VERSION(TRACE3D_CAPTURE_LOADER_VERSION_MAJOR_VALUE, TRACE3D_CAPTURE_LOADER_VERSION_MINOR_VALUE,
        TRACE3D_CAPTURE_LOADER_VERSION_PATCH_VALUE),
    TRACE3D_CAPTURE_LOADER_DESCRIPTION
    " Version " TRACE3D_VERSION_STR(TRACE3D_CAPTURE_LOADER_VERSION_MAJOR_VALUE) "." TRACE3D_VERSION_STR(
        TRACE3D_CAPTURE_LOADER_VERSION_MINOR_VALUE) "." TRACE3D_VERSION_STR(TRACE3D_CAPTURE_LOADER_VERSION_PATCH_VALUE)
};

namespace trace3d {

VkResult EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
    VkResult vkresult = VK_SUCCESS;
    if (pProperties == nullptr) {
        if (pPropertyCount != nullptr)
            *pPropertyCount = 1;
    }
    else if ((pPropertyCount != nullptr) && (*pPropertyCount >= 1)) {
        memcpy(pProperties, &layerProps, sizeof(layerProps));
        *pPropertyCount = 1;
    }
    else
        vkresult = VK_INCOMPLETE;
    return vkresult;
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
    VkResult vkresult = VK_SUCCESS;
    if (pLayerName && strcmp(pLayerName, layerProps.layerName) == 0) {
        if (pPropertyCount != nullptr)
            *pPropertyCount = 0;
    }
    else
        vkresult = VK_ERROR_LAYER_NOT_PRESENT;
    return vkresult;
}

VkResult FillPropertyCountAndList(const VkExtensionProperties *src, uint32_t numExts, uint32_t *dstCount,
                                  VkExtensionProperties *dstProps) {
    if (dstCount && !dstProps) {
        // just returning the number of extensions
        *dstCount = numExts;
        return VK_SUCCESS;
    } else if (dstCount && dstProps) {
        uint32_t dstSpace = *dstCount;

        // return the number of extensions.
        *dstCount = (numExts < dstSpace) ? numExts : dstSpace;

        // copy as much as there's space for, up to how many there are
        if (src)
            memcpy(dstProps, src, sizeof(VkExtensionProperties) * ((numExts < dstSpace) ? numExts : dstSpace));

        // if there was enough space, return success, else incomplete
        if (dstSpace >= numExts)
            return VK_SUCCESS;
    }
    // both parameters were NULL, return incomplete
    return VK_INCOMPLETE;
}

VkResult EnumerateInstanceExtensionProperties(const VkEnumerateInstanceExtensionPropertiesChain *pChain,
                                              const char *pLayerName, uint32_t *pPropertyCount,
                                              VkExtensionProperties *pProperties) {
    VkResult vkresult = VK_ERROR_LAYER_NOT_PRESENT;

    if (pChain && !pLayerName) {
        // first fetch the list of extensions ourselves
        uint32_t numExts = 0;

        vkresult = pChain->CallDown(pLayerName, &numExts, NULL);

        if (vkresult == VK_SUCCESS) {
            std::vector<VkExtensionProperties> exts;
            exts.resize(numExts);
            vkresult = pChain->CallDown(pLayerName, &numExts, exts.data());

            if (vkresult == VK_SUCCESS) {
                vkresult = FillPropertyCountAndList(exts.data(), exts.size(), pPropertyCount, pProperties);
            }
        }
    } else
        vkresult = EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);

    return vkresult;
}

typedef const void *DispatchKey;

static inline DispatchKey GetDispatchKey(const void *handle) {
    const DispatchKey *dispatch_key = reinterpret_cast<const DispatchKey *>(handle);
    return (*dispatch_key);
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
    VkResult vkresult = VK_ERROR_LAYER_NOT_PRESENT;

    if (pLayerName && strcmp(pLayerName, layerProps.layerName) == 0) {
        if (pPropertyCount != nullptr)
            *pPropertyCount = 0;

        vkresult = VK_SUCCESS;
    }
    else {
        DispatchKey physKey = GetDispatchKey(physicalDevice);

        auto it = instanceMap.find(physKey);
        if (it != instanceMap.end()) {
            auto &instInfo = it->second;

            if (instInfo.enumerateDeviceExtensionProperties)
                vkresult = instInfo.enumerateDeviceExtensionProperties(physicalDevice, nullptr, pPropertyCount, pProperties);
        }
    }

    TRACE3D_LOGI("%s(physicalDevice:%p, pLayerName:%p) --> vkresult:%d\n",
        __FUNCTION__, physicalDevice, pLayerName, vkresult);
    return vkresult;
}

VkLayerInstanceCreateInfo *GetChainInfo(const VkInstanceCreateInfo *pCreateInfo, VkLayerFunction func) {
    auto chainInfo = static_cast<const VkLayerInstanceCreateInfo *>(pCreateInfo->pNext);
    while (chainInfo != nullptr) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerInstanceCreateInfo *>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerInstanceCreateInfo *>(chainInfo->pNext);
    }
    TRACE3D_LOGE("%s", "ERROR: Find VkLayerInstanceCreateInfo Failed");
    return nullptr;
}

VkLayerDeviceCreateInfo *GetChainInfo(const VkDeviceCreateInfo *pCreateInfo, VkLayerFunction func) {
    auto chainInfo = static_cast<const VkLayerDeviceCreateInfo *>(pCreateInfo->pNext);
    while (chainInfo != nullptr) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerDeviceCreateInfo *>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerDeviceCreateInfo *>(chainInfo->pNext);
    }
    TRACE3D_LOGE("%s", "ERROR: Find VkLayerDeviceCreateInfo Failed");
    return nullptr;
}

VkResult CreateInstance(const VkInstanceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkInstance *pInstance)
{
    VkResult result = VK_ERROR_INITIALIZATION_FAILED;
    VkLayerInstanceCreateInfo *chainInfo = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    if (chainInfo == nullptr || chainInfo->u.pLayerInfo == nullptr) {
        TRACE3D_LOGE("%s:%d() --> vkresult:%d\n", __FUNCTION__, __LINE__, result);
        return result;
    }
    auto fpGetInstanceProcAddr = chainInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    auto fpCreateInstance = (PFN_vkCreateInstance)fpGetInstanceProcAddr(nullptr, "vkCreateInstance");

    if (!fpCreateInstance) {
        TRACE3D_LOGE("%s:%d() --> vkresult:%d\n", __FUNCTION__, __LINE__, result);
        return result;
    }

    chainInfo->u.pLayerInfo = chainInfo->u.pLayerInfo->pNext;

    result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS) {
        return result;
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

    TRACE3D_LOGI("%s:%d() --> instance:%p, instanceKey:%p, vkresult:%d\n",
        __FUNCTION__, __LINE__, instance, instanceKey, result);
    return result;
}

void DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey instanceKey = GetDispatchKey(instance);

    auto it = instanceMap.find(instanceKey);
    if (it != instanceMap.end()) {
        auto &instInfo = it->second;

        TRACE3D_LOGI("%s(instance:%p) instanceKey:%p, destroyInstance:%p\n",
            __FUNCTION__, instance, instanceKey, instInfo.destroyInstance);
        if (instInfo.destroyInstance)
            instInfo.destroyInstance(instance, pAllocator);
        instanceMap.erase(instanceKey);
    }
}

VkResult CreateDevice(VkPhysicalDevice physDev, const VkDeviceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
    VkResult result = VK_ERROR_INITIALIZATION_FAILED;
    const DispatchKey physKey = GetDispatchKey(physDev);

    const InstanceInfo instanceInfo = instanceMap[physKey];
    const VkInstance instance = instanceInfo.instance;

    VkLayerDeviceCreateInfo *linkInfo = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    if (linkInfo == nullptr || linkInfo->u.pLayerInfo == nullptr || instance == nullptr) {
        TRACE3D_LOGE("%s:%d(physDev:%p) physKey:%p --> vkresult:%d\n",
            __FUNCTION__, __LINE__, physDev, physKey, result);
        return result;
    }
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = linkInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = linkInfo->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice =
        (PFN_vkCreateDevice)fpGetInstanceProcAddr(instance, "vkCreateDevice");
    
    if (!fpCreateDevice) {
        TRACE3D_LOGE("%s:%d(physDev:%p) physKey:%p, instance:%p --> vkresult:%d\n",
            __FUNCTION__, __LINE__, physDev, physKey, instance, result);
        return result;
    }

    linkInfo->u.pLayerInfo = linkInfo->u.pLayerInfo->pNext;

    result = fpCreateDevice(physDev, pCreateInfo, pAllocator, pDevice);
    if (result != VK_SUCCESS) {
        TRACE3D_LOGE("%s:%d(physDev:%p) physKey:%p, instance:%p --> device:%p, vkresult:%d\n",
            __FUNCTION__, __LINE__, physDev, physKey, instance, *pDevice, result);
        return result;
    }
    VkDevice device = *pDevice;
    DispatchKey deviceKey = GetDispatchKey(device);

    PFN_vkDestroyDevice fpDestroyDevice = (PFN_vkDestroyDevice)fpGetDeviceProcAddr(device, "vkDestroyDevice");

    deviceMap[deviceKey] = DeviceInfo{device, fpGetDeviceProcAddr, fpDestroyDevice};

    TRACE3D_LOGI("%s(physDev:%p) physKey:%p, instance:%p --> device:%p, deviceKey:%p, vkresult:%d\n",
        __FUNCTION__, physDev, physKey, instance, device, deviceKey, result);
    return VK_SUCCESS;
}

void DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey deviceKey = GetDispatchKey(device);

    auto it = deviceMap.find(deviceKey);
    if (it != deviceMap.end()) {
        auto &devInfo = it->second;

        TRACE3D_LOGI("%s(device:%p) deviceKey:%p, destroyDevice:%p\n",
            __FUNCTION__, device, deviceKey, devInfo.destroyDevice);
        if (devInfo.destroyDevice)
            devInfo.destroyDevice(device, pAllocator);
        deviceMap.erase(deviceKey);
    }
}

} // namespace trace3d

extern "C" {

// Vk Layer Entries
//

PFN_vkGetInstanceProcAddr trace3dVkGetInstanceProcAddr = nullptr;
PFN_vkGetDeviceProcAddr trace3dVkGetDeviceProcAddr = nullptr;
PFN_vkGetInstanceProcAddr trace3dVkGetPhysicalDeviceProcAddr = nullptr;

TRACE3D_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *pName) {
    return trace3dVkGetInstanceProcAddr ? trace3dVkGetInstanceProcAddr(instance, pName) : nullptr;
}

TRACE3D_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char *pName) {
    return trace3dVkGetDeviceProcAddr ? trace3dVkGetDeviceProcAddr(device, pName) : nullptr;
}

VkResult EnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties)
{
    return trace3d::EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}

VkResult EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
    return trace3d::EnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}

TRACE3D_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_TRACE3D_CaptureLoaderGetInstanceProcAddr(
    VkInstance instance, const char *pName)
{
    PFN_vkVoidFunction procAddr = nullptr;

    if (strcmp("vkGetInstanceProcAddr", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)VK_LAYER_TRACE3D_CaptureLoaderGetInstanceProcAddr;
    }
    else if (strcmp("vkEnumerateInstanceExtensionProperties", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceExtensionProperties);
    }
    else if (strcmp("vkEnumerateInstanceLayerProperties", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceLayerProperties);
    }
    else if (strcmp("vkCreateInstance", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(trace3d::CreateInstance);
    }
    else if (strcmp("vkDestroyInstance", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(trace3d::DestroyInstance);
    }
    else if (strcmp("vkCreateDevice", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(trace3d::CreateDevice);
    }
    else if (strcmp("vkEnumerateDeviceExtensionProperties", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(trace3d::EnumerateDeviceExtensionProperties);
    }
    else {
        if (instance == VK_NULL_HANDLE) {
            TRACE3D_LOGE("%s(instance:%p, pName:'%s') ERROR: instance is null --> %p\n", __FUNCTION__, instance, pName, procAddr);
            return procAddr;
        }
        trace3d::DispatchKey instanceKey = trace3d::GetDispatchKey(instance);

        auto it = trace3d::instanceMap.find(instanceKey);
        if (it != trace3d::instanceMap.end()) {
            procAddr = it->second.getInstanceProcAddr(instance, pName);
        }
    }
    return procAddr;
}

TRACE3D_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_TRACE3D_CaptureLoaderGetDeviceProcAddr(VkDevice device, const char *pName)
{
    PFN_vkVoidFunction procAddr = nullptr;

    if (device == VK_NULL_HANDLE) {
        TRACE3D_LOGE("%s(device:%p, pName:'%s') ERROR: device is null--> %p\n", __FUNCTION__, device, pName, procAddr);
        return procAddr;
    }

    if (strcmp("vkGetDeviceProcAddr", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)VK_LAYER_TRACE3D_CaptureLoaderGetDeviceProcAddr;
    }
    else if (strcmp("vkDestroyDevice", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)trace3d::DestroyDevice;
    } else {
        trace3d::DispatchKey deviceKey = trace3d::GetDispatchKey(device);

        auto it = trace3d::deviceMap.find(deviceKey);
        if (it != trace3d::deviceMap.end()) {
            procAddr = it->second.getDeviceProcAddr(device, pName);
        }
    }
    return procAddr;
}

TRACE3D_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_TRACE3D_CaptureLoader_layerGetPhysicalDeviceProcAddr(
    VkInstance instance, const char *pName)
{
    PFN_vkVoidFunction procAddr = nullptr;

    if (strcmp("vk_layerGetPhysicalDeviceProcAddr", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)VK_LAYER_TRACE3D_CaptureLoader_layerGetPhysicalDeviceProcAddr;
    }
    else {
        procAddr = VK_LAYER_TRACE3D_CaptureLoaderGetInstanceProcAddr(instance, pName);
    }
    TRACE3D_LOGI("%s(instance:%p, pName:'%s') --> %p\n", __FUNCTION__, instance, pName, procAddr);
    return procAddr;
}

TRACE3D_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL VK_LAYER_TRACE3D_CaptureLoaderNegotiateLoaderLayerInterfaceVersion(
    VkNegotiateLayerInterface *pVersionStruct)
{
    VkResult vkresult = VK_ERROR_INITIALIZATION_FAILED;

    TRACE3D_LOGI("%s(pVerStruct:%p)...\n", __FUNCTION__);

    void *libHandle = trace3d::CaptureInit();
    if (libHandle) {
        auto *pfnNegotiate = (PFN_vkNegotiateLoaderLayerInterfaceVersion)dlsym(libHandle,
            "VK_LAYER_TRACE3D_CaptureNegotiateLoaderLayerInterfaceVersion");
        if (pfnNegotiate) {
            vkresult = pfnNegotiate(pVersionStruct);
    
            if (vkresult == VK_SUCCESS) {
                trace3dVkGetInstanceProcAddr = pVersionStruct->pfnGetInstanceProcAddr;
                trace3dVkGetDeviceProcAddr = pVersionStruct->pfnGetDeviceProcAddr;
                trace3dVkGetPhysicalDeviceProcAddr = pVersionStruct->pfnGetPhysicalDeviceProcAddr;
            }
        }
    }
    else {
        if (pVersionStruct && pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT) {
            trace3dVkGetInstanceProcAddr = VK_LAYER_TRACE3D_CaptureLoaderGetInstanceProcAddr;
            trace3dVkGetDeviceProcAddr = VK_LAYER_TRACE3D_CaptureLoaderGetDeviceProcAddr;
            trace3dVkGetPhysicalDeviceProcAddr = VK_LAYER_TRACE3D_CaptureLoader_layerGetPhysicalDeviceProcAddr;

            pVersionStruct->pfnGetInstanceProcAddr = trace3dVkGetInstanceProcAddr;
            pVersionStruct->pfnGetDeviceProcAddr = trace3dVkGetDeviceProcAddr;
            pVersionStruct->pfnGetPhysicalDeviceProcAddr = trace3dVkGetPhysicalDeviceProcAddr;

            if (pVersionStruct->loaderLayerInterfaceVersion > CURRENT_LOADER_LAYER_INTERFACE_VERSION) {
                pVersionStruct->loaderLayerInterfaceVersion = CURRENT_LOADER_LAYER_INTERFACE_VERSION;
            }
            vkresult = VK_SUCCESS;
        }
    }

    TRACE3D_LOGI("%s(pVerStruct:%p) ifaceVer:%d, libHandle:%p --> vkresult:%d (pVerStruct:{ sType:%d, ifaceVer:%d, pfnGIPA:%p, pfnGDPA:%p, pfnGPDPA:%p })\n", __FUNCTION__,
        pVersionStruct, (pVersionStruct ? (int)pVersionStruct->loaderLayerInterfaceVersion : 0), libHandle, vkresult,
        (pVersionStruct ? (int)pVersionStruct->sType : -1),
        (pVersionStruct ? (int)pVersionStruct->loaderLayerInterfaceVersion : 0),
        (pVersionStruct ? pVersionStruct->pfnGetInstanceProcAddr : nullptr),
        (pVersionStruct ? pVersionStruct->pfnGetDeviceProcAddr : nullptr),
        (pVersionStruct ? pVersionStruct->pfnGetPhysicalDeviceProcAddr : nullptr)
    );
    return vkresult;
}

typedef VkResult(VKAPI_PTR *PFN_vkEnumerateInstanceExtensionPropertiesChain)(const VkEnumerateInstanceExtensionPropertiesChain *pChain,
    const char *pLayerName, uint32_t *pPropertyCount, VkExtensionProperties *pProperties);

TRACE3D_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL VK_LAYER_TRACE3D_CaptureLoaderEnumerateInstanceExtensionProperties(
    const VkEnumerateInstanceExtensionPropertiesChain *pChain, const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties)
{
    VkResult vkresult = VK_INCOMPLETE;

    void *libHandle = trace3d::CaptureInit();
    if (libHandle) {
        PFN_vkEnumerateInstanceExtensionPropertiesChain pfnEnumerate = (PFN_vkEnumerateInstanceExtensionPropertiesChain)dlsym(
            libHandle, "VK_LAYER_TRACE3D_CaptureEnumerateInstanceExtensionProperties");
        if (pfnEnumerate) {
            vkresult = pfnEnumerate(pChain, pLayerName, pPropertyCount, pProperties);
        }
    }
    else {
        vkresult = trace3d::EnumerateInstanceExtensionProperties(pChain, pLayerName, pPropertyCount, pProperties);
    }

    TRACE3D_LOGI("%s(pChain:%p, pLayerName:'%s', pPropCount:%p, pProps:%p) --> vkresult:%d (*pPropCount:%u)",
        __FUNCTION__, pChain, pLayerName, pPropertyCount, pProperties, vkresult,
        pPropertyCount ? *pPropertyCount : 0);
    return vkresult;
}

// GLES Layer Entries
//
typedef void (*__eglMustCastToProperFunctionPointerType)(void);

typedef __eglMustCastToProperFunctionPointerType(*PFNEGLGETNEXTLAYERPROCADDRESSPROC)(
    void *, const char *funcName);

typedef void*(*PFNEGLGETNEXTLAYERPROCADDRESSPROCOHOS)(
    void *, const char *funcName);

typedef void* (* DebugLayerInitializeType)(const void *funcTable,
    PFNEGLGETNEXTLAYERPROCADDRESSPROCOHOS next);
typedef void* (* DebugLayerGetProcAddrType)(const char *funcName,
    __eglMustCastToProperFunctionPointerType next);

DebugLayerInitializeType trace3dInitialize = nullptr;
DebugLayerGetProcAddrType trace3dLayerGetProcAddr = nullptr;

TRACE3D_LOADER_API
void* DebugLayerInitialize(const void *funcTable, PFNEGLGETNEXTLAYERPROCADDRESSPROCOHOS next)
{
    void *ret = nullptr; 
    void *libHandle = trace3d::CaptureInit();

    if (libHandle) {
        if (!trace3dInitialize)
            trace3dInitialize = (DebugLayerInitializeType)dlsym(libHandle, "DebugLayerInitialize");
        if (trace3dInitialize && !trace3dLayerGetProcAddr) {
            trace3dLayerGetProcAddr = (DebugLayerGetProcAddrType)dlsym(libHandle, "DebugLayerGetProcAddr");
            if (!trace3dLayerGetProcAddr)
                trace3dInitialize = nullptr;
            else {
                TRACE3D_LOGI("%s:%d init:%p, getProcAddr:%p\n", __FUNCTION__, __LINE__,
                    trace3dInitialize, trace3dLayerGetProcAddr);
            }
        }

        if (trace3dInitialize)
            ret = trace3dInitialize(funcTable, next);
    }
    TRACE3D_LOGI("%s:%d funcTable:%p next:%p libHandle:%p\n", __FUNCTION__, __LINE__, funcTable, next, libHandle);
    return ret;
}

TRACE3D_LOADER_API
void* DebugLayerGetProcAddr(const char *funcName, __eglMustCastToProperFunctionPointerType next)
{
    void *trace3dPtr = (void *)next;

    if (trace3dLayerGetProcAddr)
        trace3dPtr = trace3dLayerGetProcAddr(funcName, next);
    return static_cast<void *>(trace3dPtr);
}

} // extern "C"
