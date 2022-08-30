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
#include "vulkan/vk_ohos_native_buffer.h"

namespace vulkan {
namespace driver {
namespace {
constexpr int32_t MIN_BUFFER_SIZE = 3;
}
struct Surface {
    NativeWindow* window;
    VkSwapchainKHR swapchainHandle;
    uint64_t consumerUsage;
};

struct Swapchain {
    Swapchain(Surface& surface, uint32_t numImages, VkPresentModeKHR presentMode, int preTransform)
        : surface(surface), numImages(numImages), mailboxMode(presentMode == VK_PRESENT_MODE_MAILBOX_KHR),
          preTransform(preTransform), frameTimestampsEnabled(false),
          shared(presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
                 presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {}

    Surface& surface;
    uint32_t numImages;
    bool mailboxMode;
    int preTransform;
    bool frameTimestampsEnabled;
    bool shared;

    struct Image {
        Image() : image(VK_NULL_HANDLE), requestFence(-1), releaseFence(-1), requested(false) {}
        VkImage image;
        NativeWindowBuffer* buffer;
        int requestFence;
        int releaseFence;
        bool requested;
    } images[32];
};

VKAPI_ATTR void* DefaultAllocate(void*, size_t size, size_t alignment, VkSystemAllocationScope)
{
    void* ptr = nullptr;
    // Vulkan requires 'alignment' to be a power of two, but posix_memalign
    // additionally requires that it be at least sizeof(void*).
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

    if (ptr) {
        memcpy(newPtr, ptr, std::min(oldSize, size));
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
    static const VkAllocationCallbacks kDefaultAllocCallbacks = {
        .pUserData = nullptr,
        .pfnAllocation = DefaultAllocate,
        .pfnReallocation = DefaultReallocate,
        .pfnFree = DefaultFree,
    };

    return kDefaultAllocCallbacks;
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

VKAPI_ATTR VkResult CreateOHOSSurfaceOpenHarmony(VkInstance instance,
    const VkOHOSSurfaceCreateInfoOpenHarmony* pCreateInfo,
    const VkAllocationCallbacks* allocator,
    VkSurfaceKHR* outSurface)
{
    if (!allocator) {
        allocator = &vulkan::driver::GetDefaultAllocator();
    }
    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Surface), alignof(Surface),
                                         VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (!mem) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    Surface* surface = new (mem) Surface;
    surface->window = pCreateInfo->window;
    surface->swapchainHandle = VK_NULL_HANDLE;
    int32_t res = NativeWindowHandleOpt(pCreateInfo->window, GET_USAGE, &(surface->consumerUsage));

    if (surface->consumerUsage == 0) {
        WLOGE("native window get usage failed, error num : %{public}d", res);
        surface->~Surface();
        allocator->pfnFree(allocator->pUserData, surface);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    *outSurface = HandleFromSurface(surface);
    return VK_SUCCESS;
}


VKAPI_ATTR void DestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surfaceHandle,
    const VkAllocationCallbacks* allocator)
{
    Surface* surface = SurfaceFromHandle(surfaceHandle);
    if (!surface) {
        return;
    }

    surface->~Surface();
    if (!allocator) {
        allocator = &vulkan::driver::GetDefaultAllocator();
    }
    allocator->pfnFree(allocator->pUserData, surface);
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice, uint32_t, VkSurfaceKHR, VkBool32* supported)
{
    *supported = VK_TRUE;
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice, VkSurfaceKHR surface,
                                                 VkSurfaceCapabilitiesKHR* capabilities)
{
    int err;
    int width = 0;
    int height = 0;
    int maxBufferCount = 10;
    if (surface != VK_NULL_HANDLE) {
        NativeWindow* window = SurfaceFromHandle(surface)->window;
        err = NativeWindowHandleOpt(window, GET_BUFFER_GEOMETRY, &height, &width);
        if (err != OHOS::GSERROR_OK) {
            WLOGE("NATIVE_WINDOW_DEFAULT_WIDTH query failed: (%{public}d)", err);
            return VK_ERROR_SURFACE_LOST_KHR;
        }
        maxBufferCount = window->surface->GetQueueSize();
    }

    capabilities->minImageCount = std::min(maxBufferCount, MIN_BUFFER_SIZE);
    capabilities->maxImageCount = static_cast<uint32_t>(maxBufferCount);
    capabilities->currentExtent = VkExtent2D {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    capabilities->minImageExtent = VkExtent2D {1, 1};
    capabilities->maxImageExtent = VkExtent2D {4096, 4096};
    capabilities->maxImageArrayLayers = 1;
    capabilities->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    capabilities->supportedUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
                                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    return VK_SUCCESS;
}


VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice pdev, VkSurfaceKHR surfaceHandle, uint32_t* count,
                                            VkSurfaceFormatKHR* formats)
{
    std::vector<VkSurfaceFormatKHR> allFormats = {{VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};
    VkResult result = VK_SUCCESS;
    if (formats) {
        uint32_t transferCount = allFormats.size();
        if (transferCount > *count) {
            transferCount = *count;
            result = VK_INCOMPLETE;
        }
        std::copy(allFormats.begin(), allFormats.begin() + transferCount, formats);
        *count = transferCount;
    } else {
        *count = allFormats.size();
    }

    return result;
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice,
                                             const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                             uint32_t* pSurfaceFormatCount, VkSurfaceFormat2KHR* pSurfaceFormats)
{
    if (!pSurfaceFormats) {
        return GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, pSurfaceInfo->surface, pSurfaceFormatCount, nullptr);
    }
    // temp vector for forwarding. We'll marshal it into the pSurfaceFormats after the call.
    std::vector<VkSurfaceFormatKHR> surfaceFormats(*pSurfaceFormatCount);
    VkResult result = GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, pSurfaceInfo->surface, pSurfaceFormatCount,
                                                         surfaceFormats.data());
    if (result == VK_SUCCESS || result == VK_INCOMPLETE) {
        // marshal results individually due to stride difference.
        // completely ignore any chained extension structs.
        uint32_t formatsToMarshal = *pSurfaceFormatCount;
        for (uint32_t i = 0u; i < formatsToMarshal; i++) {
            pSurfaceFormats[i].surfaceFormat = surfaceFormats[i];
        }
    }
    return result;
}


VKAPI_ATTR
VkResult GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice pdev, VkSurfaceKHR surface,  uint32_t* count,
    VkPresentModeKHR* modes)
{
    std::vector<VkPresentModeKHR> presentModes;
    presentModes.push_back(VK_PRESENT_MODE_MAILBOX_KHR);
    presentModes.push_back(VK_PRESENT_MODE_FIFO_KHR);

    VkPhysicalDevicePresentationPropertiesOpenHarmony present_properties;
    QueryPresentationProperties(pdev, &present_properties);
    if (present_properties.sharedImage) {
        presentModes.push_back(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR);
        presentModes.push_back(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR);
    }

    uint32_t numModes = uint32_t(presentModes.size());
    VkResult result = VK_SUCCESS;
    if (modes) {
        if (*count < numModes) {
            result = VK_INCOMPLETE;
        }
        *count = std::min(*count, numModes);
        std::copy_n(presentModes.data(), *count, modes);
    } else {
        *count = numModes;
    }
    return result;
}

VKAPI_ATTR VkResult GetDeviceGroupPresentCapabilitiesKHR(VkDevice device,
    VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities)
{
    memset(pDeviceGroupPresentCapabilities->presentMask, 0, sizeof(pDeviceGroupPresentCapabilities->presentMask));

    // assume device group of size 1
    pDeviceGroupPresentCapabilities->presentMask[0] = 1 << 0;
    pDeviceGroupPresentCapabilities->modes = VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult GetDeviceGroupSurfacePresentModesKHR(VkDevice, VkSurfaceKHR,
    VkDeviceGroupPresentModeFlagsKHR* pModes)
{
    *pModes = VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR;
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult GetSwapchainImagesKHR(VkDevice, VkSwapchainKHR swapchainHandle, uint32_t* count, VkImage* images)
{
    Swapchain& swapchain = *SwapchainFromHandle(swapchainHandle);

    VkResult result = VK_SUCCESS;
    if (images) {
        uint32_t n = swapchain.numImages;
        if (*count < swapchain.numImages) {
            n = *count;
            result = VK_INCOMPLETE;
        }
        for (uint32_t i = 0; i < n; i++) {
            images[i] = swapchain.images[i].image;
        }
        *count = n;
    } else {
        *count = swapchain.numImages;
    }
    return result;
}

VKAPI_ATTR VkResult GetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice, VkSurfaceKHR surface,
    uint32_t* pRectCount, VkRect2D* pRects)
{
    if (!pRects) {
        *pRectCount = 1;
        return VK_SUCCESS;
    }

    uint32_t count = std::min(*pRectCount, 1u);
    bool incomplete = *pRectCount < 1;
    *pRectCount = count;

    if (incomplete) {
        return VK_INCOMPLETE;
    }

    NativeWindow* window = SurfaceFromHandle(surface)->window;

    int width = 0;
    int height = 0;
    int err = NativeWindowHandleOpt(window, GET_BUFFER_GEOMETRY, &height, &width);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("NATIVE_WINDOW_DEFAULT_WIDTH query failed: (%{public}d)", err);
    }
    pRects[0].offset.x = 0;
    pRects[0].offset.y = 0;
    pRects[0].extent = VkExtent2D {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    return VK_SUCCESS;
}


PixelFormat GetPixelFormat(VkFormat format)
{
    PixelFormat nativeFormat = PIXEL_FMT_RGBA_8888;
    switch (format) {
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:
            nativeFormat = PIXEL_FMT_RGBA_8888;
            break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            nativeFormat = PIXEL_FMT_RGB_565;
            break;
        default:
            WLOGE("unsupported swapchain format %{public}d", format);
            break;
    }
    return nativeFormat;
}

ColorDataSpace GetColorDataspace(VkColorSpaceKHR colorspace)
{
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
        default:
            return COLOR_DATA_SPACE_UNKNOWN;
    }
}

static bool IsFencePending(int fd)
{
    return false;
}

void ReleaseSwapchainImage(VkDevice device, NativeWindow* window, int releaseFence, Swapchain::Image& image,
                           bool deferIfPending)
{
    if (image.requested) {
        if (releaseFence >= 0) {
            if (image.requestFence >= 0) {
                close(image.requestFence);
            }
        } else {
            releaseFence = image.requestFence;
        }
        image.requestFence = -1;

        if (window) {
            NativeWindowCancelBuffer(window, image.buffer);
        } else {
            if (releaseFence >= 0) {
                close(releaseFence);
            }
        }
        releaseFence = -1;
        image.requested = false;
    }

    if (deferIfPending && IsFencePending(image.releaseFence)) {
        return;
    }

    if (image.releaseFence >= 0) {
        close(image.releaseFence);
        image.releaseFence = -1;
    }

    if (image.image) {
        vulkan::driver::DestroyImage(device, image.image, nullptr);
        image.image = VK_NULL_HANDLE;
    }
}

void ReleaseSwapchain(VkDevice device, Swapchain* swapchain)
{
    if (swapchain->surface.swapchainHandle != HandleFromSwapchain(swapchain)) {
        return;
    }

    for (uint32_t i = 0; i < swapchain->numImages; i++) {
        if (!swapchain->images[i].requested)
            ReleaseSwapchainImage(device, nullptr, -1, swapchain->images[i], true);
    }
    swapchain->surface.swapchainHandle = VK_NULL_HANDLE;
}

int TranslateVulkanToNativeTransform(VkSurfaceTransformFlagBitsKHR transform)
{
    return ROTATE_NONE;
}

static void DestroySwapchainInternal(VkDevice device, VkSwapchainKHR swapchainHandle,
                                     const VkAllocationCallbacks* allocator)
{
    Swapchain* swapchain = SwapchainFromHandle(swapchainHandle);
    if (!swapchain) {
        return;
    }

    bool active = swapchain->surface.swapchainHandle == swapchainHandle;
    NativeWindow* window = active ? swapchain->surface.window : nullptr;

    for (uint32_t i = 0; i < swapchain->numImages; i++) {
        ReleaseSwapchainImage(device, window, -1, swapchain->images[i], false);
    }

    if (active) {
        swapchain->surface.swapchainHandle = VK_NULL_HANDLE;
    }

    if (!allocator) {
        allocator = &vulkan::driver::GetDefaultAllocator();
    }

    swapchain->~Swapchain();
    allocator->pfnFree(allocator->pUserData, swapchain);
}

VKAPI_ATTR
VkResult ConfigWindowInfo(Surface* surface, PixelFormat& pixelFormat, const VkSwapchainCreateInfoKHR* createInfo,
                          uint32_t* numImages)
{
    // Configure the native window
    NativeWindow* window = surface->window;
    int err = NativeWindowHandleOpt(window, SET_FORMAT, pixelFormat);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffers_format(%{public}d) failed: (%{public}d)", pixelFormat, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    err = NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, static_cast<int>(createInfo->imageExtent.width),
                                static_cast<int>(createInfo->imageExtent.height));
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffers_dimensions(%{public}d,%{public}d) failed:(%{public}d)",
              createInfo->imageExtent.width, createInfo->imageExtent.height, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    VkSwapchainImageUsageFlagsOpenHarmony swapchainImageUsage = 0;
    if (createInfo->presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
        createInfo->presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {
        swapchainImageUsage |= VK_SWAPCHAIN_IMAGE_USAGE_SHARED_BIT_OPENHARMONY;
        if (err != OHOS::GSERROR_OK) {
            WLOGE("native_window_set_shared_buffer_mode failed: (%{public}d)", err);
            return VK_ERROR_SURFACE_LOST_KHR;
        }
    }

    // auto refresh: get min undequeued buffers and set buffer_count
    if (swapchainImageUsage & VK_SWAPCHAIN_IMAGE_USAGE_SHARED_BIT_OPENHARMONY) {
        *numImages = 1;
    }
    // get consumer and producer usage
    uint64_t nativeUsage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA;
    err = NativeWindowHandleOpt(window, SET_USAGE, nativeUsage);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffer_count(%{public}d) failed: (%{public}d)",
              *numImages, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult CreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo,
    const VkAllocationCallbacks* allocator,VkSwapchainKHR* swapchainHandle)
{
    int err;
    VkResult result = VK_SUCCESS;

    if (!allocator) {
        allocator = &vulkan::driver::GetDefaultAllocator();
    }

    PixelFormat pixel_format = GetPixelFormat(createInfo->imageFormat);
    ColorDataSpace color_data_space = GetColorDataspace(createInfo->imageColorSpace);
    if (color_data_space == COLOR_DATA_SPACE_UNKNOWN) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    Surface& surface = *SurfaceFromHandle(createInfo->surface);
    if (surface.swapchainHandle != createInfo->oldSwapchain) {
        return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;
    }
    if (createInfo->oldSwapchain != VK_NULL_HANDLE) {
        ReleaseSwapchain(device, SwapchainFromHandle(createInfo->oldSwapchain));
    }

    NativeWindow* window = surface.window;
    err = NativeWindowHandleOpt(window, SET_FORMAT, pixel_format);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffers_format(%{public}d) failed: (%{public}d)", pixel_format, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    err = NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY,
                                static_cast<int>(createInfo->imageExtent.width),
                                static_cast<int>(createInfo->imageExtent.height));
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffers_dimensions(%{public}d,%{public}d) failed: (%{public}d)",
            createInfo->imageExtent.width, createInfo->imageExtent.height, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    VkSwapchainImageUsageFlagsOpenHarmony swapchain_image_usage = 0;
    if (createInfo->presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
        createInfo->presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {
        swapchain_image_usage |= VK_SWAPCHAIN_IMAGE_USAGE_SHARED_BIT_OPENHARMONY;
        if (err != OHOS::GSERROR_OK) {
            WLOGE("native_window_set_shared_buffer_mode failed: (%{public}d)", err);
            return VK_ERROR_SURFACE_LOST_KHR;
        }
    }

    uint32_t num_images = MIN_BUFFER_SIZE;

    if (swapchain_image_usage & VK_SWAPCHAIN_IMAGE_USAGE_SHARED_BIT_OPENHARMONY) {
        num_images = 1;
    }

    uint64_t native_usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA;
    err = NativeWindowHandleOpt(window, SET_USAGE, native_usage);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffer_count(%{public}d) failed: (%{public}d)", num_images, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Swapchain), alignof(Swapchain),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (!mem) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    Swapchain* swapchain = new (mem) Swapchain(surface, num_images, createInfo->presentMode,
        TranslateVulkanToNativeTransform(createInfo->preTransform));

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
        .format = createInfo->imageFormat,
        .extent = {0, 0, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = createInfo->imageUsage,
        .sharingMode = createInfo->imageSharingMode,
        .queueFamilyIndexCount = createInfo->queueFamilyIndexCount,
        .pQueueFamilyIndices = createInfo->pQueueFamilyIndices,
    };

    if (createInfo->oldSwapchain != VK_NULL_HANDLE) {
        WLOGI("recreate swapchain ,clean buffer queue");
        window->surface->CleanCache();
    }

    for (uint32_t i = 0; i < num_images; i++) {
        Swapchain::Image& img = swapchain->images[i];

        NativeWindowBuffer* buffer;
        err = NativeWindowRequestBuffer(window, &buffer, &img.requestFence);
        if (err != OHOS::GSERROR_OK) {
            WLOGE("dequeueBuffer[%{public}u] failed: (%{public}d)", i, err);
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
        image_create.extent = VkExtent3D {static_cast<uint32_t>(img.buffer->sfbuffer->GetSurfaceBufferWidth()),
                                          static_cast<uint32_t>(img.buffer->sfbuffer->GetSurfaceBufferHeight()), 1};
        image_native_buffer.handle = img.buffer->sfbuffer->GetBufferHandle();

        result = vulkan::driver::CreateImage(device, &image_create, nullptr, &img.image);
        if (result != VK_SUCCESS) {
            WLOGD("vkCreateImage w/ native buffer failed: %{public}u", result);
            break;
        }
    }
    WLOGD("swapchain init shared %{public}d", swapchain->shared);
    for (uint32_t i = 0; i < num_images; i++) {
        Swapchain::Image& img = swapchain->images[i];
        if (img.requested) {
            if (!swapchain->shared) {
                NativeWindowCancelBuffer(window, img.buffer);
                img.requestFence = -1;
                img.requested = false;
            }
        }
    }

    if (result != VK_SUCCESS) {
        DestroySwapchainInternal(device, HandleFromSwapchain(swapchain), allocator);
        return result;
    }

    surface.swapchainHandle = HandleFromSwapchain(swapchain);
    *swapchainHandle = surface.swapchainHandle;
    return VK_SUCCESS;
}

VKAPI_ATTR
void DestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchainHandle,
                         const VkAllocationCallbacks* allocator)
{
    DestroySwapchainInternal(device, swapchainHandle, allocator);
}


VKAPI_ATTR VkResult AcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchainHandle, uint64_t timeout,
    VkSemaphore semaphore, VkFence vkFence, uint32_t* imageIndex)
{
    Swapchain& swapchain = *SwapchainFromHandle(swapchainHandle);
    NativeWindow* nativeWindow = swapchain.surface.window;
    VkResult result;

    if (swapchain.surface.swapchainHandle != swapchainHandle) {
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    if (swapchain.shared) {
        *imageIndex = 0;
        result = vulkan::driver::SetNativeFenceFdOpenHarmony(device, -1, semaphore, vkFence);
        return result;
    }

    NativeWindowBuffer* nativeWindowBuffer;
    int fence;
    int32_t ret = NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fence);
    if (ret != OHOS::GSERROR_OK) {
        WLOGE("dequeueBuffer failed: (%{public}d)", ret);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    uint32_t index = 0;
    while (index < swapchain.numImages) {
        if (swapchain.images[index].buffer->sfbuffer == nativeWindowBuffer->sfbuffer) {
            swapchain.images[index].requested = true;
            swapchain.images[index].requestFence = fence;
            break;
        }
        index++;
    }

    if (index == swapchain.numImages) {
        WLOGE("dequeueBuffer returned unrecognized buffer");
        if (NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer) != OHOS::GSERROR_OK) {
            WLOGE("NativeWindowCancelBuffer failed: (%{public}d)", ret);
        }
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    int fenceFd = dup(fence);
    if (fenceFd == -1) {
        WLOGE("dup(fence) failed, stalling until signalled: (%{public}d)", errno);
    }

    result = vulkan::driver::SetNativeFenceFdOpenHarmony(device, fenceFd, semaphore, vkFence);
    if (result != VK_SUCCESS) {
        if (NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer) != OHOS::GSERROR_OK) {
            WLOGE("NativeWindowCancelBuffer failed: (%{public}d)", ret);
        }
        swapchain.images[index].requested = false;
        swapchain.images[index].requestFence = -1;
        return result;
    }

    *imageIndex = index;
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult AcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex)
{
    return AcquireNextImageKHR(device, pAcquireInfo->swapchain, pAcquireInfo->timeout,
                               pAcquireInfo->semaphore, pAcquireInfo->fence, pImageIndex);
}


VKAPI_ATTR
VkResult GetSwapchainStatusKHR(VkDevice, VkSwapchainKHR swapchainHandle)
{
    Swapchain& swapchain = *SwapchainFromHandle(swapchainHandle);
    VkResult result = VK_SUCCESS;
    if (swapchain.surface.swapchainHandle != swapchainHandle) {
        return VK_ERROR_OUT_OF_DATE_KHR;
    }
    return result;
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice,
                                                  const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                                  VkSurfaceCapabilities2KHR* pSurfaceCapabilities)
{
    VkResult result = GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, pSurfaceInfo->surface,
                                                              &pSurfaceCapabilities->surfaceCapabilities);
    VkSurfaceCapabilities2KHR* caps = pSurfaceCapabilities;
    while (caps->pNext) {
        caps = reinterpret_cast<VkSurfaceCapabilities2KHR*>(caps->pNext);
        switch (caps->sType) {
            case VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR: {
                VkSharedPresentSurfaceCapabilitiesKHR* shared_caps =
                    reinterpret_cast<VkSharedPresentSurfaceCapabilitiesKHR*>(caps);
                // Claim same set of usage flags are supported for shared present modes as for other modes.
                shared_caps->sharedPresentSupportedUsageFlags =
                    pSurfaceCapabilities->surfaceCapabilities.supportedUsageFlags;
                break;
            }
            default:
                // Ignore all other extension structs
                break;
        }
    }
    return result;
}

static VkResult WorstPresentResult(VkResult a, VkResult b)
{
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

static inline const VkPresentRegionKHR* GetPresentRegion(
    const VkPresentRegionKHR* regions, const Swapchain& swapchain, uint32_t index)
{
    return (regions && !swapchain.mailboxMode) ? &regions[index] : nullptr;
}

static inline VkResult GetReleaseFence( VkQueue queue, const VkPresentInfoKHR* presentInfo,
    Swapchain::Image& img, int32_t &fence)
{
    VkResult result = vulkan::driver::GetNativeFenceFdOpenHarmony(queue, presentInfo->waitSemaphoreCount,
        presentInfo->pWaitSemaphores, img.image, &fence);
    if (img.releaseFence >= 0) {
        close(img.releaseFence);
        img.releaseFence = -1;
    }
    if (fence >= 0) {
        img.releaseFence = dup(fence);
    }

    return result;
}

static inline struct Region::Rect* GetRegionRect(
    const VkAllocationCallbacks* defaultAllocator, struct Region::Rect** rects, uint32_t rectangleCount)
{
    return static_cast<struct Region::Rect*>(
                defaultAllocator->pfnReallocation(
                    defaultAllocator->pUserData, *rects,
                    sizeof(Region::Rect) * rectangleCount,
                    alignof(Region::Rect), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND));
}

static inline void InitRegionRect(const VkRectLayerKHR* layer, struct Region::Rect* rect)
{
    int32_t x = layer->offset.x;
    int32_t y = layer->offset.y;
    int32_t width = static_cast<int32_t>(layer->extent.width);
    int32_t height = static_cast<int32_t>(layer->extent.height);
    rect->x = x;
    rect->y = y;
    rect->w = width;
    rect->h = height;
}

static inline const VkPresentRegionKHR* GetPresentRegions(const VkPresentInfoKHR* presentInfo)
{
    const VkPresentRegionsKHR* presentRegions = nullptr;
    const VkPresentRegionsKHR* nextRegions = reinterpret_cast<const VkPresentRegionsKHR*>(presentInfo->pNext);
    while (nextRegions != nullptr) {
        if (nextRegions->sType == VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR) {
            presentRegions = nextRegions;
        }
        nextRegions = reinterpret_cast<const VkPresentRegionsKHR*>(nextRegions->pNext);
    }
    
    if (presentRegions == nullptr) {
        return nullptr;
    } else {
        return presentRegions->pRegions;
    }
}

VKAPI_ATTR VkResult QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* presentInfo)
{
    VkDevice device = nullptr;
    VkResult ret = VK_SUCCESS;

    const VkPresentRegionKHR* regions = GetPresentRegions(presentInfo);
    const VkAllocationCallbacks* defaultAllocator = &vulkan::driver::GetDefaultAllocator();
    struct Region::Rect* rects = nullptr;
    uint32_t restsNumber = 0;

    for (uint32_t i = 0; i < presentInfo->swapchainCount; i++) {
        VkResult scResult = VK_SUCCESS;
        int err = 0;

        Swapchain& swapchain = *(reinterpret_cast<Swapchain*>(presentInfo->pSwapchains[i]));
        Swapchain::Image& img = swapchain.images[presentInfo->pImageIndices[i]];
        const VkPresentRegionKHR* region = GetPresentRegion(regions, swapchain, i);
        
        Region localRegion;
        memset(&localRegion, 0, sizeof(Region));

        int32_t fence = -1;
        scResult = GetReleaseFence(queue, presentInfo, img, fence);
        if (swapchain.surface.swapchainHandle == presentInfo->pSwapchains[i]) {
            NativeWindow* window = swapchain.surface.window;
            if (scResult == VK_SUCCESS) {
                if (region != nullptr) {
                    uint32_t rectangleCount = region->rectangleCount;
                    if (rectangleCount > restsNumber) {
                        struct Region::Rect* tmpRects = GetRegionRect(defaultAllocator, &rects, rectangleCount);
                        if (tmpRects != nullptr) {
                            rects = tmpRects;
                            restsNumber = rectangleCount;
                        } else {
                            rectangleCount = 0;
                        }
                    }
                    for (uint32_t r = 0; r < rectangleCount; ++r) {
                        InitRegionRect(&region->pRectangles[r], &rects[r]);
                    }
                    localRegion.rects = rects;
                    localRegion.rectNumber = rectangleCount;
                }

                // the acquire fence will be close by BufferQueue module
                err = NativeWindowFlushBuffer(window, img.buffer, fence, localRegion);
                if (err != OHOS::GSERROR_OK) {
                    WLOGE("queueBuffer failed: (%{public}d)", err);
                    scResult = WorstPresentResult(scResult, VK_ERROR_SURFACE_LOST_KHR);
                } else {
                    if (img.requestFence >= 0) {
                        close(img.requestFence);
                        img.requestFence = -1;
                    }
                    img.requested = false;
                }

                if (swapchain.shared && scResult == VK_SUCCESS) {
                    NativeWindowBuffer* buffer = nullptr;
                    int releaseFence = -1;
                    err = NativeWindowRequestBuffer(window, &buffer, &releaseFence);
                    if (err != OHOS::GSERROR_OK) {
                        WLOGE("NativeWindowRequestBuffer failed: (%{public}d)", err);
                        scResult = WorstPresentResult(scResult, VK_ERROR_SURFACE_LOST_KHR);
                    } else if (img.buffer != buffer) {
                        WLOGE("img.buffer != buffer");
                        scResult = WorstPresentResult(scResult, VK_ERROR_SURFACE_LOST_KHR);
                    } else {
                        img.requestFence = releaseFence;
                        img.requested = true;
                    }
                }
            }
            if (scResult != VK_SUCCESS) {
                ReleaseSwapchain(device, &swapchain);
            }

        } else {
            WLOGE("QueuePresentKHR swapchainHandle != pSwapchains[%{public}d]", i);
            ReleaseSwapchainImage(device, nullptr, fence, img, true);
            scResult = VK_ERROR_OUT_OF_DATE_KHR;
        }

        if (presentInfo->pResults)
            presentInfo->pResults[i] = scResult;

        if (scResult != ret)
            ret = WorstPresentResult(ret, scResult);
    }
    if (rects != nullptr) {
        defaultAllocator->pfnFree(defaultAllocator->pUserData, rects);
    }

    return ret;

}
}  // namespace driver
}  // namespace vulkan
