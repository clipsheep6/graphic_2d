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

#ifndef __VK_OHOS_NATIVE_BUFFER_H__
#define __VK_OHOS_NATIVE_BUFFER_H__

#include <buffer_handle.h>
#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VK_OHOS_native_buffer 1

#define VK_OHOS_NATIVE_BUFFER_EXTENSION_NUMBER 453

#define VK_OHOS_NATIVE_BUFFER_SPEC_VERSION 1
#define VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME "VK_OHOS_native_buffer"

#define VK_STRUCTURE_TYPE_NATIVE_BUFFER_OHOS                               1000453001
#define VK_STRUCTURE_TYPE_SWAPCHAIN_IMAGE_CREATE_INFO_OHOS                 1000453002
#define VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTAT30geION_PROPERTIES_OHOS 1000453003

typedef VkFlags VkSwapchainImageUsageFlagsOHOS;

typedef struct {
    VkStructureType                   sType;
    const void*                       pNext;
    BufferHandle*                     handle;
} VkNativeBufferOHOS;

typedef struct {
    VkStructureType                   sType;
    const void*                       pNext;
    VkSwapchainImageUsageFlagsOHOS    usage;
} VkSwapchainImageCreateInfoOHOS;

typedef struct {
    VkStructureType                   sType;
    const void*                       pNext;
    VkBool32                          sharedImage;
} VkPhysicalDevicePresentationPropertiesOHOS;

typedef VkResult (VKAPI_PTR *PFN_vkGetSwapchainGrallocUsageOHOS)(
    VkDevice                          device,
    VkFormat                          format,
    VkImageUsageFlags                 imageUsage,
    uint64_t*                         grallocUsage);

typedef VkResult (VKAPI_PTR *PFN_vkSetNativeFenceFdOHOS)(
    VkDevice                          device,
    int32_t                           nativeFenceFd,
    VkSemaphore                       semaphore,
    VkFence                           fence);

typedef VkResult (VKAPI_PTR *PFN_vkGetNativeFenceFdOHOS)(
    VkQueue                           queue,
    uint32_t                          waitSemaphoreCount,
    const VkSemaphore*                pWaitSemaphores,
    VkImage                           image,
    int32_t*                          pNativeFenceFd);

#ifndef VK_NO_PROTOTYPES

VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainGrallocUsageOHOS(
    VkDevice                          device,
    VkFormat                          format,
    VkImageUsageFlags                 imageUsage,
    uint64_t*                         grallocUsage
);

VKAPI_ATTR VkResult VKAPI_CALL vkSetNativeFenceFdOHOS(
    VkDevice                          device,
    int32_t                           nativeFenceFd,
    VkSemaphore                       semaphore,
    VkFence                           fence
);

VKAPI_ATTR VkResult VKAPI_CALL vkGetNativeFenceFdOHOS(
    VkQueue                           queue,
    uint32_t                          waitSemaphoreCount,
    const VkSemaphore*                pWaitSemaphores,
    VkImage                           image,
    int32_t*                          pNativeFenceFd
);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __VK_OHOS_NATIVE_BUFFER_H__ */
