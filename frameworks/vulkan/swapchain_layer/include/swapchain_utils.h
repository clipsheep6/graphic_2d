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

#ifndef SWAPCHAIN_UTILS_H
#define SWAPCHAIN_UTILS_H

#include "surface_type.h"
#include <vulkan/vulkan.h>
#include "swapchain_layer_log.h"
#include "swapchain_type.h"

namespace OHOS::SWAPCHAIN {
bool IsFencePending(int fd)
{
    if (fd < 0) {
        return false;
    }
    errno = 0;
    sptr<SyncFence> syncFence = new SyncFence(fd);
    return syncFence->Wait(0) == -1 && errno == ETIME;
}

uint32_t ToUint32(uint64_t val)
{
    if (val > UINT32_MAX) {
        SWLOGE("%{public}" PRIu64 " is too large to convert to uint32", val);
    }
    return static_cast<uint32_t>(val);
}

VkSurfaceKHR HandleFromSurface(Surface* surface)
{
    return VkSurfaceKHR(reinterpret_cast<uint64_t>(surface));
}

Surface* SurfaceFromHandle(VkSurfaceKHR handle)
{
    return reinterpret_cast<Surface*>(handle);
}

Swapchain* SwapchainFromHandle(VkSwapchainKHR handle)
{
    return reinterpret_cast<Swapchain*>(handle);
}

VkSwapchainKHR HandleFromSwapchain(Swapchain* swapchain)
{
    return VkSwapchainKHR(reinterpret_cast<uint64_t>(swapchain));
}

VKAPI_ATTR void* DefaultAllocate(void*, size_t size, size_t alignment, VkSystemAllocationScope)
{
    void* ptr = nullptr;
    int ret = posix_memalign(&ptr, std::max(alignment, sizeof(void*)), size);
    return ret == 0 ? ptr : nullptr;
}

VKAPI_ATTR void* DefaultReallocate(void*, void* ptr, size_t size, size_t alignment, VkSystemAllocationScope)
{
    if (size == 0) {
        free(ptr);
        return nullptr;
    }

    size_t oldSize = ptr ? malloc_usable_size(ptr) : 0;
    if (size <= oldSize) {
        return ptr;
    }

    void* newPtr = nullptr;
    if (posix_memalign(&newPtr, std::max(alignment, sizeof(void*)), size) != 0) {
        return nullptr;
    }

    if (ptr != nullptr) {
        auto ret = memcpy_s(newPtr, size, ptr, oldSize);
        if (ret != EOK) {
            free(newPtr);
            return nullptr;
        }
        free(ptr);
    }
    return newPtr;
}

VKAPI_ATTR void DefaultFree(void*, void* ptr)
{
    free(ptr);
}

const VkAllocationCallbacks& GetDefaultAllocator()
{
    static const VkAllocationCallbacks defaultAllocCallbacks = {
        .pUserData = nullptr,
        .pfnAllocation = DefaultAllocate,
        .pfnReallocation = DefaultReallocate,
        .pfnFree = DefaultFree,
    };

    return defaultAllocCallbacks;
}

GraphicPixelFormat GetPixelFormat(VkFormat format)
{
    GraphicPixelFormat nativeFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
    switch (format) {
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:
            nativeFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
            break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            nativeFormat = GRAPHIC_PIXEL_FMT_RGB_565;
            break;
        default:
            SWLOGE("unsupported swapchain format %{public}d", format);
            break;
    }
    return nativeFormat;
}

GraphicColorDataSpace GetColorDataspace(VkColorSpaceKHR colorspace)
{
    switch (colorspace) {
        case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
            return GRAPHIC_BT709_SRGB_FULL;
        case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT:
            return GRAPHIC_DCI_P3_SRGB_FULL;
        case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
            return GRAPHIC_BT709_LINEAR_EXTENDED;
        case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
            return GRAPHIC_BT709_SRGB_EXTENDED;
        case VK_COLOR_SPACE_DCI_P3_LINEAR_EXT:
            return GRAPHIC_DCI_P3_LINEAR_FULL;
        case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:
            return GRAPHIC_DCI_P3_GAMMA26_FULL;
        case VK_COLOR_SPACE_BT709_LINEAR_EXT:
            return GRAPHIC_BT709_LINEAR_FULL;
        case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:
            return GRAPHIC_BT709_SRGB_FULL;
        case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
            return GRAPHIC_BT2020_LINEAR_FULL;
        case VK_COLOR_SPACE_HDR10_ST2084_EXT:
            return GRAPHIC_BT2020_ST2084_FULL;
        case VK_COLOR_SPACE_DOLBYVISION_EXT:
            return GRAPHIC_BT2020_ST2084_FULL;
        case VK_COLOR_SPACE_HDR10_HLG_EXT:
            return GRAPHIC_BT2020_HLG_FULL;
        case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:
            return static_cast<GraphicColorDataSpace>(GRAPHIC_GAMUT_ADOBE_RGB |
                                                      GRAPHIC_TRANSFORM_FUNC_LINEAR | GRAPHIC_PRECISION_FULL);
        case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:
            return GRAPHIC_ADOBE_RGB_GAMMA22_FULL;
        default:
            return GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    }
}

int TranslateVulkanToNativeTransform(VkSurfaceTransformFlagBitsKHR transform)
{
    switch (transform) {
        case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR:
            return GRAPHIC_ROTATE_90;
        case VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR:
            return GRAPHIC_ROTATE_180;
        case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR:
            return GRAPHIC_ROTATE_270;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR:
            return GRAPHIC_FLIP_H;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR:
            return GRAPHIC_FLIP_H_ROT90;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR:
            return GRAPHIC_FLIP_H_ROT180;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR:
            return GRAPHIC_FLIP_H_ROT270;
        case VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR:
        case VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR:
        default:
            return GRAPHIC_ROTATE_NONE;
    }
}
} // namespace OHOS::SWAPCHAIN
#endif // SWAPCHAIN_UTILS_H
