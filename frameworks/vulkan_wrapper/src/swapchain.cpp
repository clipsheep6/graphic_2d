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
#include "swapchain.h"
#include "driver.h"
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <new>
#include <malloc.h>
#include <native_window.h>
#include <window.h>
#include <native_buffer.h>
#include <wrapper_log.h>
#include <graphic_common.h>
#include "display_type.h"
#include "vk_ohos_native_buffer.h"

namespace vulkan {
namespace driver {

VKAPI_ATTR void* DefaultAllocate(void*,
                                 size_t size,
                                 size_t alignment,
                                 VkSystemAllocationScope) {
    void* ptr = nullptr;
    // Vulkan requires 'alignment' to be a power of two, but posix_memalign
    // additionally requires that it be at least sizeof(void*).
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

struct Surface {
    NativeWindow* window;
    VkSwapchainKHR swapchain_handle;
    uint64_t consumer_usage;
};

struct Swapchain {
    Swapchain(Surface& surface_,
              uint32_t num_images_,
              VkPresentModeKHR present_mode,
              int pre_transform_)
        : surface(surface_),
          num_images(num_images_),
          mailbox_mode(present_mode == VK_PRESENT_MODE_MAILBOX_KHR),
          pre_transform(pre_transform_),
          frame_timestamps_enabled(false),
          shared(present_mode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
                 present_mode ==
                     VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {
        //OHNativeWindow* window = surface.window;

    }

    Surface& surface;
    uint32_t num_images;
    bool mailbox_mode;
    int pre_transform;
    bool frame_timestamps_enabled;
    bool shared;

    struct Image {
        Image()
            : image(VK_NULL_HANDLE),
              request_fence(-1),
              release_fence(-1),
              requested(false) {}
        VkImage image;
        NativeWindowBuffer* buffer;
        int request_fence;
        int release_fence;
        bool requested;
    } images[32];
};

VkSurfaceKHR HandleFromSurface(Surface* surface) {
    return VkSurfaceKHR(reinterpret_cast<uint64_t>(surface));
}

Surface* SurfaceFromHandle(VkSurfaceKHR handle) {
    return reinterpret_cast<Surface*>(handle);
}

Swapchain* SwapchainFromHandle(VkSwapchainKHR handle) {
    return reinterpret_cast<Swapchain*>(handle);
}

VkSwapchainKHR HandleFromSwapchain(Swapchain* swapchain) {
    return VkSwapchainKHR(reinterpret_cast<uint64_t>(swapchain));
}

// input  : VkSwapchainCreateInfoKHR (native window)
// output : VkSurfaceKHR(swapchain_handle is null)
VKAPI_ATTR 
VkResult CreateOHOSSurfaceOpenHarmony(VkInstance instance, 
                                      const VkOHOSSurfaceCreateInfoOpenHarmony* pCreateInfo, 
                                      const VkAllocationCallbacks* allocator, 
                                      VkSurfaceKHR* out_surface) {

    if (!allocator) {
        allocator = &vulkan::driver::GetDefaultAllocator();
    }
    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Surface),
                                         alignof(Surface),
                                         VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (!mem) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    Surface* surface = new (mem) Surface;

    surface->window = pCreateInfo->window;
    surface->swapchain_handle = VK_NULL_HANDLE;
    int32_t res = NativeWindowHandleOpt(pCreateInfo->window, GET_USAGE, &(surface->consumer_usage));

    if (surface->consumer_usage == 0) {
        WLOGE("native window get usage failed, error num : %{public}d", res);
        surface->~Surface();
        allocator->pfnFree(allocator->pUserData, surface);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    // TODO: find connect method

    *out_surface = HandleFromSurface(surface);
    return VK_SUCCESS;
}


VKAPI_ATTR
void DestroySurfaceKHR(VkInstance instance,
                       VkSurfaceKHR surface_handle,
                       const VkAllocationCallbacks* allocator) {

    Surface* surface = SurfaceFromHandle(surface_handle);
    if (!surface) {
        return;
    }
    // disconnect [TODO]

    surface->~Surface();
    if (!allocator) {
        allocator = &vulkan::driver::GetDefaultAllocator();
    }
    allocator->pfnFree(allocator->pUserData, surface);
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice /*pdev*/,
                                            uint32_t /*queue_family*/,
                                            VkSurfaceKHR /*surface_handle*/,
                                            VkBool32* supported) {
    *supported = VK_TRUE;
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice /*pdev*/,
    VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR* capabilities) {

    int err;
    NativeWindow* window = SurfaceFromHandle(surface)->window;

    int width, height;
    err = NativeWindowHandleOpt(window, GET_BUFFER_GEOMETRY,&height, &width);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("NATIVE_WINDOW_DEFAULT_WIDTH query failed: %{public}s (%{public}d)",
              strerror(-err), err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    // int transform_hint = 0; // TODO


    int max_buffer_count = window->surface->GetQueueSize();

    capabilities->minImageCount = std::min(max_buffer_count, 3);
    capabilities->maxImageCount = static_cast<uint32_t>(max_buffer_count);

    capabilities->currentExtent =
        VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    // TODO(http://b/134182502): Figure out what the max extent should be.
    capabilities->minImageExtent = VkExtent2D{1, 1};
    capabilities->maxImageExtent = VkExtent2D{4096, 4096};

    capabilities->maxImageArrayLayers = 1;

    // capabilities->supportedTransforms = kSupportedTransforms;
    // capabilities->currentTransform =
    //     TranslateNativeToVulkanTransform(transform_hint);


    capabilities->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

    capabilities->supportedUsageFlags =
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

    return VK_SUCCESS;
}


VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice pdev,
                                            VkSurfaceKHR surface_handle,
                                            uint32_t* count,
                                            VkSurfaceFormatKHR* formats) {

    bool wide_color_support = false;
    //Surface& surface = *SurfaceFromHandle(surface_handle);

    WLOGE("wide_color_support is: %d", wide_color_support);

    // [todo] Hardwarebuffer

    std::vector<VkSurfaceFormatKHR> all_formats = {
        {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};

    VkResult result = VK_SUCCESS;
    if (formats) {
        uint32_t transfer_count = all_formats.size();
        if (transfer_count > *count) {
            transfer_count = *count;
            result = VK_INCOMPLETE;
        }
        std::copy(all_formats.begin(), all_formats.begin() + transfer_count,
                  formats);
        *count = transfer_count;
    } else {
        *count = all_formats.size();
    }

    return result;
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceFormats2KHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    uint32_t* pSurfaceFormatCount,
    VkSurfaceFormat2KHR* pSurfaceFormats)
{
    if (!pSurfaceFormats) {
        return GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,
                                                  pSurfaceInfo->surface,
                                                  pSurfaceFormatCount, nullptr);
    } else {
        // temp vector for forwarding; we'll marshal it into the pSurfaceFormats
        // after the call.
        std::vector<VkSurfaceFormatKHR> surface_formats(*pSurfaceFormatCount);
        VkResult result = GetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice, pSurfaceInfo->surface, pSurfaceFormatCount,
            surface_formats.data());

        if (result == VK_SUCCESS || result == VK_INCOMPLETE) {
            // marshal results individually due to stride difference.
            // completely ignore any chained extension structs.
            uint32_t formats_to_marshal = *pSurfaceFormatCount;
            for (uint32_t i = 0u; i < formats_to_marshal; i++) {
                pSurfaceFormats[i].surfaceFormat = surface_formats[i];
            }
        }

        return result;
    }
}


VKAPI_ATTR
VkResult GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice pdev, 
    VkSurfaceKHR surface, 
    uint32_t* count, 
    VkPresentModeKHR* modes)
{

    std::vector<VkPresentModeKHR> present_modes;

    if (surface == VK_NULL_HANDLE) {
        //[TODO]
        present_modes.push_back(VK_PRESENT_MODE_MAILBOX_KHR);
        present_modes.push_back(VK_PRESENT_MODE_FIFO_KHR);
    } else {
        present_modes.push_back(VK_PRESENT_MODE_FIFO_KHR);
    }
    // NativeWindow* window = SurfaceFromHandle(surface)->window;

    VkPhysicalDevicePresentationPropertiesOpenHarmony present_properties;
    QueryPresentationProperties(pdev, &present_properties);
    if (present_properties.sharedImage) {
        present_modes.push_back(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR);
        present_modes.push_back(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR);
    }

    uint32_t num_modes = uint32_t(present_modes.size());

    VkResult result = VK_SUCCESS;
    if (modes) {
        if (*count < num_modes)
            result = VK_INCOMPLETE;
        *count = std::min(*count, num_modes);
        std::copy_n(present_modes.data(), *count, modes);
    } else {
        *count = num_modes;
    }
    return result;

}

VKAPI_ATTR 
VkResult GetDeviceGroupPresentCapabilitiesKHR(VkDevice device, 
    VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities)
{
    memset(pDeviceGroupPresentCapabilities->presentMask, 0,
           sizeof(pDeviceGroupPresentCapabilities->presentMask));

    // assume device group of size 1
    pDeviceGroupPresentCapabilities->presentMask[0] = 1 << 0;
    pDeviceGroupPresentCapabilities->modes =
        VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR;

    return VK_SUCCESS;

}


VKAPI_ATTR
VkResult GetDeviceGroupSurfacePresentModesKHR(
    VkDevice,
    VkSurfaceKHR,
    VkDeviceGroupPresentModeFlagsKHR* pModes) {

    *pModes = VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR;
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult GetSwapchainImagesKHR(VkDevice,
                               VkSwapchainKHR swapchain_handle,
                               uint32_t* count,
                               VkImage* images) {

    Swapchain& swapchain = *SwapchainFromHandle(swapchain_handle);

    VkResult result = VK_SUCCESS;
    if (images) {
        uint32_t n = swapchain.num_images;
        if (*count < swapchain.num_images) {
            n = *count;
            result = VK_INCOMPLETE;
        }
        for (uint32_t i = 0; i < n; i++)
            images[i] = swapchain.images[i].image;
        *count = n;
    } else {
        *count = swapchain.num_images;
    }
    return result;
}

VKAPI_ATTR
VkResult GetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice,
                                               VkSurfaceKHR surface,
                                               uint32_t* pRectCount,
                                               VkRect2D* pRects) {
    if (!pRects) {
        *pRectCount = 1;
    } else {
        uint32_t count = std::min(*pRectCount, 1u);
        bool incomplete = *pRectCount < 1;

        *pRectCount = count;

        if (incomplete) {
            return VK_INCOMPLETE;
        }

        int err;
        NativeWindow* window = SurfaceFromHandle(surface)->window;

        int width = 0, height = 0;
        err = NativeWindowHandleOpt(window, GET_BUFFER_GEOMETRY,&height, &width);
        if (err != OHOS::GSERROR_OK) {
            WLOGE("NATIVE_WINDOW_DEFAULT_WIDTH query failed: %{public}s (%{public}d)",
                  strerror(-err), err);
        }
        pRects[0].offset.x = 0;
        pRects[0].offset.y = 0;
        pRects[0].extent = VkExtent2D{static_cast<uint32_t>(width),
                                      static_cast<uint32_t>(height)};
    }
    return VK_SUCCESS;
}


PixelFormat GetPixelFormat(VkFormat format) {
    PixelFormat native_format = PIXEL_FMT_RGBA_8888;
    switch (format) {
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:
            native_format = PIXEL_FMT_RGBA_8888;
            break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            native_format = PIXEL_FMT_RGB_565;
            break;
        default:
            WLOGE("unsupported swapchain format %{public}d", format);
            break;
    }
    return native_format;
}

ColorDataSpace GetColorDataspace(VkColorSpaceKHR colorspace) {
    switch (colorspace) {
        case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
            return BT709_SRGB_FULL;
        case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT:
            return DCI_P3_GAMMA26_FULL;
        case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
            return BT709_LINEAR_EXTENDED;
        case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
            return BT709_SRGB_EXTENDED;
        case VK_COLOR_SPACE_DCI_P3_LINEAR_EXT:
            return DCI_P3_LINEAR_FULL;
        case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:
            return DCI_P3_GAMMA26_FULL;
        case VK_COLOR_SPACE_BT709_LINEAR_EXT:
            return BT709_LINEAR_FULL;
        case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:
            return BT709_SRGB_FULL;
        case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
            return BT2020_LINEAR_FULL;
        case VK_COLOR_SPACE_HDR10_ST2084_EXT:
            return BT2020_ST2084_FULL;
        case VK_COLOR_SPACE_DOLBYVISION_EXT:
            return BT2020_ST2084_FULL;
        case VK_COLOR_SPACE_HDR10_HLG_EXT:
            return BT2020_HLG_FULL;
        case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:
            return static_cast<ColorDataSpace>(
                GAMUT_ADOBE_RGB | TRANSFORM_FUNC_LINEAR | PRECISION_FULL);
        case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:
            return ADOBE_RGB_GAMMA22_FULL;

        // Pass through is intended to allow app to provide data that is passed
        // to the display system without modification.
        // case VK_COLOR_SPACE_PASS_THROUGH_EXT:
        //     return HAL_DATASPACE_ARBITRARY; // ????

        default:
            return COLOR_DATA_SPACE_UNKNOWN;
    }
}

static bool IsFencePending(int fd) {
    return false;
    if (fd < 0) {
        return false;
    }

    errno = 0; // ?????
    //return sync_wait(fd, 0 /* timeout */) == -1 && errno == ETIME;
}

void ReleaseSwapchainImage(VkDevice device,
                           NativeWindow* window,
                           int release_fence,
                           Swapchain::Image& image,
                           bool defer_if_pending) {

    if (image.requested) {
        if (release_fence >= 0) {
            if (image.request_fence >= 0) {
                close(image.request_fence);
            }
        } else {
            release_fence = image.request_fence;
        }
        image.request_fence = -1;

        if (window) {
            NativeWindowCancelBuffer(window, image.buffer);
        } else {
            if (release_fence >= 0) {
                //sync_wait(release_fence, -1 /* forever */);
                close(release_fence);
            }
        }
        release_fence = -1;
        image.requested = false;
    }

    if (defer_if_pending && IsFencePending(image.release_fence)) {
        return;
    }

    if (image.release_fence >= 0) {
        close(image.release_fence);
        image.release_fence = -1;
    }

    if (image.image) {
        vulkan::driver::DestroyImage(device, image.image, nullptr);
        image.image = VK_NULL_HANDLE;
    }
}

void ReleaseSwapchain(VkDevice device, Swapchain* swapchain) {
    if (swapchain->surface.swapchain_handle != HandleFromSwapchain(swapchain)) {
        return;
    }
    for (uint32_t i = 0; i < swapchain->num_images; i++) {
        if (!swapchain->images[i].requested)
            ReleaseSwapchainImage(device, nullptr, -1, swapchain->images[i],
                                  true);
    }
    swapchain->surface.swapchain_handle = VK_NULL_HANDLE;
}

int TranslateVulkanToNativeTransform(VkSurfaceTransformFlagBitsKHR transform) {
    // TODO
    return ROTATE_NONE;
}

static void DestroySwapchainInternal(VkDevice device,
                                     VkSwapchainKHR swapchain_handle,
                                     const VkAllocationCallbacks* allocator) {
    Swapchain* swapchain = SwapchainFromHandle(swapchain_handle);
    if (!swapchain) {
        return;
    }

    bool active = swapchain->surface.swapchain_handle == swapchain_handle;
    NativeWindow* window = active ? swapchain->surface.window : nullptr;

    for (uint32_t i = 0; i < swapchain->num_images; i++) {
        ReleaseSwapchainImage(device, window, -1, swapchain->images[i], false);
    }

    if (active) {
        swapchain->surface.swapchain_handle = VK_NULL_HANDLE;
    }

    if (!allocator) {
        allocator = &vulkan::driver::GetDefaultAllocator();
    }

    swapchain->~Swapchain();
    allocator->pfnFree(allocator->pUserData, swapchain);
}


VKAPI_ATTR
VkResult CreateSwapchainKHR(VkDevice device,
                            const VkSwapchainCreateInfoKHR* create_info,
                            const VkAllocationCallbacks* allocator,
                            VkSwapchainKHR* swapchain_handle) {

    int err;
    VkResult result = VK_SUCCESS;

    if (!allocator) {
        allocator = &vulkan::driver::GetDefaultAllocator();
    }

    // VkFormat -> PIXEL_FORMAT // ?????

    PixelFormat pixel_format = GetPixelFormat(create_info->imageFormat);
    ColorDataSpace color_data_space = GetColorDataspace(create_info->imageColorSpace);
    if (color_data_space == COLOR_DATA_SPACE_UNKNOWN) {
        WLOGI("CreateSwapchainKHR(VkSwapchainCreateInfoKHR.imageColorSpace = %{public}d) "
            "failed: Unsupported color space", create_info->imageColorSpace);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // surface(native_window, but swapchain_handle is null)
    Surface& surface = *SurfaceFromHandle(create_info->surface);
    if (surface.swapchain_handle != create_info->oldSwapchain) {
        WLOGD("Can't create a swapchain for VkSurfaceKHR 0x%" PRIx64
              " because it already has active swapchain 0x%" PRIx64
              " but VkSwapchainCreateInfo::oldSwapchain=0x%" PRIx64,
              reinterpret_cast<uint64_t>(create_info->surface),
              reinterpret_cast<uint64_t>(surface.swapchain_handle),
              reinterpret_cast<uint64_t>(create_info->oldSwapchain));
        return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;
    }
    if (create_info->oldSwapchain != VK_NULL_HANDLE) {
        ReleaseSwapchain(device, SwapchainFromHandle(create_info->oldSwapchain));
    }

    // [TODO]disconnet and connect
    // -- Configure the native window --
    NativeWindow* window = surface.window;
    err = NativeWindowHandleOpt(window, SET_FORMAT, pixel_format);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffers_format(%d) failed: %s (%d)",
              pixel_format, strerror(-err), err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    // [TODO]color_data_space not ready

    err = NativeWindowHandleOpt(window, static_cast<int>(create_info->imageExtent.width),
        static_cast<int>(create_info->imageExtent.height));
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffers_dimensions(%{public}d,%{public}d) failed: %{public}s (%{public}d)",
              create_info->imageExtent.width, create_info->imageExtent.height,
              strerror(-err), err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    // [TODO]Transform and scaling mode

    VkSwapchainImageUsageFlagsOpenHarmony swapchain_image_usage = 0;
    if (create_info->presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
        create_info->presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {
        swapchain_image_usage |= VK_SWAPCHAIN_IMAGE_USAGE_SHARED_BIT_OPENHARMONY;
        // [TODO]shared buffer
        if (err != OHOS::GSERROR_OK) {
            WLOGE("native_window_set_shared_buffer_mode failed: %{public}s (%{public}d)", strerror(-err), err);
            return VK_ERROR_SURFACE_LOST_KHR;
        }
    }

    //[TODO] auto refresh
    // [TODO] get min undequeued buffers and set buffer_count

    uint32_t num_images = 3;

    if (swapchain_image_usage & VK_SWAPCHAIN_IMAGE_USAGE_SHARED_BIT_OPENHARMONY) {
        num_images = 1;
    }

    // get consumer and producer usage

    uint64_t native_usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA;
    err = NativeWindowHandleOpt(window, SET_USAGE, native_usage);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffer_count(%{public}d) failed: %{public}s (%{public}d)", num_images,
              strerror(-err), err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    // [TODO] transform hint

    // -- Allocate our Swapchain object --
    // After this point, we must deallocate the swapchain on error.

    void* mem = allocator->pfnAllocation(allocator->pUserData,
                                         sizeof(Swapchain), alignof(Swapchain),
                                         VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (!mem)
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    Swapchain* swapchain = new (mem)
        Swapchain(surface, num_images, create_info->presentMode,
                  TranslateVulkanToNativeTransform(create_info->preTransform));
    // -- Dequeue all buffers and create a VkImage for each --
    // Any failures during or after this must cancel the dequeued buffers.

    VkSwapchainImageCreateInfoOpenHarmony swapchain_image_create = {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_IMAGE_CREATE_INFO_OPENHARMONY,
#pragma clang diagnostic pop
        .pNext = nullptr,
    };
    VkNativeBufferOpenHarmony image_native_buffer = {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
        .sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_OPENHARMONY,
#pragma clang diagnostic pop
        .pNext = &swapchain_image_create,
    };
    VkImageCreateInfo image_create = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = &image_native_buffer,
        .flags = 0u,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = create_info->imageFormat,
        .extent = {0, 0, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = create_info->imageUsage,
        .sharingMode = create_info->imageSharingMode,
        .queueFamilyIndexCount = create_info->queueFamilyIndexCount,
        .pQueueFamilyIndices = create_info->pQueueFamilyIndices,
    };

    if (create_info->oldSwapchain != VK_NULL_HANDLE) {
        WLOGI("recreate swapchain ,clean buffer queue");
        window->surface->CleanCache();
    }

    for (uint32_t i = 0; i < num_images; i++) {
        Swapchain::Image& img = swapchain->images[i];

        NativeWindowBuffer* buffer;
        err = NativeWindowRequestBuffer(window, &buffer, &img.request_fence);
        if (err != OHOS::GSERROR_OK) {
            WLOGE("dequeueBuffer[%{public}u] failed: %{public}s (%{public}d)", i, strerror(-err), err);
            switch (-err) {
                case ENOMEM:
                    result = VK_ERROR_OUT_OF_DEVICE_MEMORY;
                    break;
                default:
                    result = VK_ERROR_SURFACE_LOST_KHR;
                    break;
            }
            break;
        }
        img.buffer = buffer;
        img.requested = true;

        image_create.extent =
            VkExtent3D{static_cast<uint32_t>(img.buffer->sfbuffer->GetSurfaceBufferWidth()),
                       static_cast<uint32_t>(img.buffer->sfbuffer->GetSurfaceBufferHeight()),
                       1};
        image_native_buffer.handle = img.buffer->sfbuffer->GetBufferHandle();
        
        // [TODO] set producer and consumer usage


        result =
            vulkan::driver::CreateImage(device, &image_create, nullptr, &img.image);
        if (result != VK_SUCCESS) {
            WLOGD("vkCreateImage w/ native buffer failed: %{public}u", result);
            break;
        }
    }

    // -- Cancel all buffers, returning them to the queue --
    // If an error occurred before, also destroy the VkImage and release the
    // buffer reference. Otherwise, we retain a strong reference to the buffer.
    WLOGD("swapchain init shared %{public}d", swapchain->shared);
    for (uint32_t i = 0; i < num_images; i++) {
        Swapchain::Image& img = swapchain->images[i];
        if (img.requested) {
            if (!swapchain->shared) {
                NativeWindowCancelBuffer(window, img.buffer);
                img.request_fence = -1;
                img.requested = false;
            }
        }
    }

    if (result != VK_SUCCESS) {
        DestroySwapchainInternal(device, HandleFromSwapchain(swapchain),
                                 allocator);
        return result;
    }

    // [TODO] set target stat

    surface.swapchain_handle = HandleFromSwapchain(swapchain);
    *swapchain_handle = surface.swapchain_handle;
    return VK_SUCCESS;
}

VKAPI_ATTR
void DestroySwapchainKHR(VkDevice device,
                         VkSwapchainKHR swapchain_handle,
                         const VkAllocationCallbacks* allocator) {
    DestroySwapchainInternal(device, swapchain_handle, allocator);
}


VKAPI_ATTR
VkResult AcquireNextImageKHR(VkDevice device,
                             VkSwapchainKHR swapchain_handle,
                             uint64_t timeout,
                             VkSemaphore semaphore,
                             VkFence vk_fence,
                             uint32_t* image_index) {

    Swapchain& swapchain = *SwapchainFromHandle(swapchain_handle);
    NativeWindow* window = swapchain.surface.window;
    VkResult result;
    int err;

    if (swapchain.surface.swapchain_handle != swapchain_handle)
        return VK_ERROR_OUT_OF_DATE_KHR;

    if (swapchain.shared) {
        // In shared mode, we keep the buffer dequeued all the time, so we don't
        // want to dequeue a buffer here. Instead, just ask the driver to ensure
        // the semaphore and fence passed to us will be signalled.
        *image_index = 0;
        result = vulkan::driver::SetNativeFenceFdOpenHarmony(
                device, -1, semaphore, vk_fence);
        return result;
    }

    // [TODO] timeout

    NativeWindowBuffer* buffer;
    int fence_fd;
    err = NativeWindowRequestBuffer(window, &buffer, &fence_fd);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("dequeueBuffer failed: %{public}s (%{public}d)", strerror(-err), err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    uint32_t idx;
    for (idx = 0; idx < swapchain.num_images; idx++) {
        if (swapchain.images[idx].buffer->sfbuffer == buffer->sfbuffer) {
            swapchain.images[idx].requested = true;
            swapchain.images[idx].request_fence = fence_fd;
            break;
        }
    }
    if (idx == swapchain.num_images) {
        WLOGE("dequeueBuffer returned unrecognized buffer");
        NativeWindowCancelBuffer(window, buffer);
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    int fence_clone = -1;
    if (fence_fd != -1) {
        fence_clone = dup(fence_fd);
        if (fence_clone == -1) {
            WLOGE("dup(fence) failed, stalling until signalled: %{public}s (%{public}d)",
                  strerror(errno), errno);
            //sync_wait(fence_fd, -1 /* forever */);
        }
    }

    result = vulkan::driver::SetNativeFenceFdOpenHarmony(
        device, fence_clone, semaphore, vk_fence);
    if (result != VK_SUCCESS) {
        NativeWindowCancelBuffer(window, buffer);
        swapchain.images[idx].requested = false;
        swapchain.images[idx].request_fence = -1;
        return result;
    }

    *image_index = idx;
    return VK_SUCCESS;
}

VKAPI_ATTR 
VkResult AcquireNextImage2KHR(VkDevice device, 
    const VkAcquireNextImageInfoKHR* pAcquireInfo, 
    uint32_t* pImageIndex)
{
    return AcquireNextImageKHR(device, 
                               pAcquireInfo->swapchain, 
                               pAcquireInfo->timeout,
                               pAcquireInfo->semaphore,
                               pAcquireInfo->fence,
                               pImageIndex);
}


VKAPI_ATTR
VkResult GetSwapchainStatusKHR(
    VkDevice,
    VkSwapchainKHR swapchain_handle) {

    Swapchain& swapchain = *SwapchainFromHandle(swapchain_handle);
    VkResult result = VK_SUCCESS;

    if (swapchain.surface.swapchain_handle != swapchain_handle) {
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    return result;
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    VkSurfaceCapabilities2KHR* pSurfaceCapabilities) {

    VkResult result = GetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice, pSurfaceInfo->surface,
        &pSurfaceCapabilities->surfaceCapabilities);

    VkSurfaceCapabilities2KHR* caps = pSurfaceCapabilities;
    while (caps->pNext) {
        caps = reinterpret_cast<VkSurfaceCapabilities2KHR*>(caps->pNext);

        switch (caps->sType) {
            case VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR: {
                VkSharedPresentSurfaceCapabilitiesKHR* shared_caps =
                    reinterpret_cast<VkSharedPresentSurfaceCapabilitiesKHR*>(
                        caps);
                // Claim same set of usage flags are supported for
                // shared present modes as for other modes.
                shared_caps->sharedPresentSupportedUsageFlags =
                    pSurfaceCapabilities->surfaceCapabilities
                        .supportedUsageFlags;
            } break;

            default:
                // Ignore all other extension structs
                break;
        }
    }

    return result;
}

// static void InterceptBindImageMemory2(
//     uint32_t bind_info_count,
//     const VkBindImageMemoryInfo* bind_infos,
//     std::vector<VkNativeBufferOpenHarmony>* out_native_buffers,
//     std::vector<VkBindImageMemoryInfo>* out_bind_infos) {
//     out_native_buffers->clear();
//     out_bind_infos->clear();

//     if (!bind_info_count)
//         return;

//     std::unordered_set<uint32_t> intercepted_indexes;

//     for (uint32_t idx = 0; idx < bind_info_count; idx++) {
//         auto info = reinterpret_cast<const VkBindImageMemorySwapchainInfoKHR*>(
//             bind_infos[idx].pNext);
//         while (info &&
//                info->sType !=
//                    VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_SWAPCHAIN_INFO_KHR) {
//             info = reinterpret_cast<const VkBindImageMemorySwapchainInfoKHR*>(
//                 info->pNext);
//         }

//         if (!info)
//             continue;

        
//         const Swapchain* swapchain = SwapchainFromHandle(info->swapchain);
      
//         NativeWindowBuffer* buffer =
//             swapchain->images[info->imageIndex].buffer;
//         VkNativeBufferOpenHarmony native_buffer = {
//             .sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_OPENHARMONY,
//             .pNext = bind_infos[idx].pNext,
//             .handle = buffer->sfbuffer->GetBufferHandle(),
//         };
//         // Reserve enough space to avoid letting re-allocation invalidate the
//         // addresses of the elements inside.
//         out_native_buffers->reserve(bind_info_count);
//         out_native_buffers->emplace_back(native_buffer);

//         // Reserve the space now since we know how much is needed now.
//         out_bind_infos->reserve(bind_info_count);
//         out_bind_infos->emplace_back(bind_infos[idx]);
//         out_bind_infos->back().pNext = &out_native_buffers->back();

//         intercepted_indexes.insert(idx);
//     }

//     if (intercepted_indexes.empty())
//         return;

//     for (uint32_t idx = 0; idx < bind_info_count; idx++) {
//         if (intercepted_indexes.count(idx))
//             continue;
//         out_bind_infos->emplace_back(bind_infos[idx]);
//     }
// }

// VKAPI_ATTR 
// VkResult BindImageMemory2(
//     VkDevice device, 
//     uint32_t bindInfoCount, 
//     const VkBindImageMemoryInfo* pBindInfos)
// {
//     std::vector<VkNativeBufferOpenHarmony> out_native_buffers;
//     std::vector<VkBindImageMemoryInfo> out_bind_infos;
//     InterceptBindImageMemory2(bindInfoCount, pBindInfos, &out_native_buffers,
//                               &out_bind_infos);
//     return vulkan::driver::BindImageMemory2(device, bindInfoCount,
//         out_bind_infos.empty() ? pBindInfos : out_bind_infos.data());

// }

// VKAPI_ATTR
// VkResult BindImageMemory2KHR(VkDevice device,
//                              uint32_t bindInfoCount,
//                              const VkBindImageMemoryInfo* pBindInfos) {

//     std::vector<VkNativeBufferOpenHarmony> out_native_buffers;
//     std::vector<VkBindImageMemoryInfo> out_bind_infos;
//     InterceptBindImageMemory2(bindInfoCount, pBindInfos, &out_native_buffers,
//                               &out_bind_infos);
//     return vulkan::driver::BindImageMemory2KHR(
//         device, bindInfoCount,
//         out_bind_infos.empty() ? pBindInfos : out_bind_infos.data());
// }

static VkResult WorstPresentResult(VkResult a, VkResult b) {
    static const VkResult kWorstToBest[] = {
        VK_ERROR_DEVICE_LOST,
        VK_ERROR_SURFACE_LOST_KHR,
        VK_ERROR_OUT_OF_DATE_KHR,
        VK_ERROR_OUT_OF_DEVICE_MEMORY,
        VK_ERROR_OUT_OF_HOST_MEMORY,
        VK_SUBOPTIMAL_KHR,
    };
    for (auto result : kWorstToBest) {
        if (a == result || b == result)
            return result;
    }
    return a != VK_SUCCESS ? a : b;
}



VKAPI_ATTR 
VkResult QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* present_info)
{
    VkDevice device = nullptr;//[TODO] = GetData(queue).driver_device;
    //const auto& dispatch = GetData(queue).driver;
    VkResult final_result = VK_SUCCESS;

    // Look at the pNext chain for supported extension structs:
    const VkPresentRegionsKHR* present_regions = nullptr;
    const VkPresentRegionsKHR* next =
        reinterpret_cast<const VkPresentRegionsKHR*>(present_info->pNext);
    while (next) {
        switch (next->sType) {
            case VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR:
                present_regions = next;
                break;
            default:
                WLOGE("QueuePresentKHR ignoring unrecognized pNext->sType = %{public}x",
                      next->sType);
                break;
        }
        next = reinterpret_cast<const VkPresentRegionsKHR*>(next->pNext);
    }

    const VkPresentRegionKHR* regions =
        (present_regions) ? present_regions->pRegions : nullptr;

    const VkAllocationCallbacks* allocator = &vulkan::driver::GetDefaultAllocator();
    struct Region::Rect* rects = nullptr;
    uint32_t nrects = 0;

    for (uint32_t sc = 0; sc < present_info->swapchainCount; sc++) {
        Swapchain& swapchain =
            *SwapchainFromHandle(present_info->pSwapchains[sc]);
        uint32_t image_idx = present_info->pImageIndices[sc];
        Swapchain::Image& img = swapchain.images[image_idx];
        const VkPresentRegionKHR* region =
            (regions && !swapchain.mailbox_mode) ? &regions[sc] : nullptr;
        VkResult swapchain_result = VK_SUCCESS;
        VkResult result;
        Region native_region;
        memset(&native_region, 0, sizeof(Region));
        
        int err;

        int fence = -1;
        result = vulkan::driver::GetNativeFenceFdOpenHarmony(
            queue, present_info->waitSemaphoreCount,
            present_info->pWaitSemaphores, img.image, &fence);
        if (result != VK_SUCCESS) {
            WLOGE("QueueSignalReleaseImageOHOS failed: %{public}d", result);
            swapchain_result = result;
        }
        if (img.release_fence >= 0)
            close(img.release_fence);
        img.release_fence = fence < 0 ? -1 : dup(fence);

        if (swapchain.surface.swapchain_handle ==
            present_info->pSwapchains[sc]) {
            NativeWindow* window = swapchain.surface.window;
            if (swapchain_result == VK_SUCCESS) {
                if (region) {
                    // Process the incremental-present hint for this swapchain:
                    uint32_t rcount = region->rectangleCount;
                    if (rcount > nrects) {
                        struct Region::Rect* new_rects =
                            static_cast<struct Region::Rect*>(
                                allocator->pfnReallocation(
                                    allocator->pUserData, rects,
                                    sizeof(Region::Rect) * rcount,
                                    alignof(Region::Rect),
                                    VK_SYSTEM_ALLOCATION_SCOPE_COMMAND));
                        if (new_rects) {
                            rects = new_rects;
                            nrects = rcount;
                        } else {
                            rcount = 0;  // Ignore the hint for this swapchain
                        }
                    }
                    for (uint32_t r = 0; r < rcount; ++r) {
                        if (region->pRectangles[r].layer > 0) {
                            WLOGD(
                                "vkQueuePresentKHR ignoring invalid layer "
                                "(%{public}u); using layer 0 instead",
                                region->pRectangles[r].layer);
                        }
                        int x = region->pRectangles[r].offset.x;
                        int y = region->pRectangles[r].offset.y;
                        int width = static_cast<int>(
                            region->pRectangles[r].extent.width);
                        int height = static_cast<int>(
                            region->pRectangles[r].extent.height);
                        struct Region::Rect* cur_rect = &rects[r];
                        cur_rect->x = x;
                        cur_rect->y = y;
                        cur_rect->w = width;
                        cur_rect->h = height;
                    }
                    native_region.rects = rects;
                    native_region.rectNumber = rcount;
                }

                err = NativeWindowFlushBuffer(window, img.buffer, fence, native_region);
                // queueBuffer always closes fence, even on error
                if (err != OHOS::GSERROR_OK) {
                    WLOGE("queueBuffer failed: %{public}s (%{public}d)", strerror(-err), err);
                    swapchain_result = WorstPresentResult(
                        swapchain_result, VK_ERROR_SURFACE_LOST_KHR);
                } else {
                    if (img.request_fence >= 0) {
                        close(img.request_fence);
                        img.request_fence = -1;
                    }
                    img.requested = false;
                }

                // If the swapchain is in shared mode, immediately dequeue the
                // buffer so it can be presented again without an intervening
                // call to AcquireNextImageKHR. We expect to get the same buffer
                // back from every call to dequeueBuffer in this mode.
                if (swapchain.shared && swapchain_result == VK_SUCCESS) {
                    NativeWindowBuffer* buffer;
                    int fence_fd;
                    err = NativeWindowRequestBuffer(window, &buffer, &fence_fd);
                    if (err != OHOS::GSERROR_OK) {
                        WLOGE("dequeueBuffer failed: %{public}s (%{public}d)", strerror(-err), err);
                        swapchain_result = WorstPresentResult(swapchain_result,
                            VK_ERROR_SURFACE_LOST_KHR);
                    } else if (img.buffer != buffer) {
                        WLOGE("got wrong image back for shared swapchain");
                        swapchain_result = WorstPresentResult(swapchain_result,
                            VK_ERROR_SURFACE_LOST_KHR);
                    } else {
                        img.request_fence = fence_fd;
                        img.requested = true;
                    }
                }
            }
            if (swapchain_result != VK_SUCCESS) {
                ReleaseSwapchain(device, &swapchain);
            }

        } else {
            WLOGE("QueuePresentKHR swapchain_handle != pSwapchains[%{public}d]", sc);
            ReleaseSwapchainImage(device, nullptr, fence, img, true);
            swapchain_result = VK_ERROR_OUT_OF_DATE_KHR;
        }

        if (present_info->pResults)
            present_info->pResults[sc] = swapchain_result;

        if (swapchain_result != final_result)
            final_result = WorstPresentResult(final_result, swapchain_result);
    }
    if (rects) {
        allocator->pfnFree(allocator->pUserData, rects);
    }

    return final_result;

}


}  // namespace driver
}  // namespace vulkan
