
#ifndef __VK_OHOS_NATIVE_BUFFER_H__
#define __VK_OHOS_NATIVE_BUFFER_H__

#include  <buffer_handle.h>
#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VK_OHOS_native_buffer 1

#define VK_OHOS_NATIVE_BUFFER_EXTENSION_NUMBER 11
/*
 * NOTE ON VK_ANDROID_NATIVE_BUFFER_SPEC_VERSION 6
 *
 * This version of the extension transitions from gralloc0 to gralloc1 usage
 * flags (int -> 2x uint64_t). The WSI implementation will temporarily continue
 * to fill out deprecated fields in VkNativeBufferANDROID, and will call the
 * deprecated vkGetSwapchainGrallocUsageANDROID if the new
 * vkGetSwapchainGrallocUsage2ANDROID is not supported. This transitionary
 * backwards-compatibility support is temporary, and will likely be removed
 * (along with all gralloc0 support) in a future release.
 */
/*
 * NOTE ON VK_ANDROID_NATIVE_BUFFER_SPEC_VERSION 8
 *
 * This version of the extension doesn't introduce new types or structs, but is
 * to accommodate the new struct VkBindImageMemorySwapchainInfoKHR added in
 * VK_KHR_swapchain spec version 69. When VkBindImageMemorySwapchainInfoKHR is
 * chained in the pNext chain of VkBindImageMemoryInfo, a VkNativeBufferANDROID
 * that holds the correct gralloc handle according to the imageIndex specified
 * in VkBindImageMemorySwapchainInfoKHR will be additionally chained to the
 * pNext chain of VkBindImageMemoryInfo and passed down to the driver.
 */
#define VK_OHOS_NATIVE_BUFFER_SPEC_VERSION 8
#define VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME "VK_OHOS_native_buffer"

#define VK_OHOS_NATIVE_BUFFER_ENUM(type, id) \
    ((type)(1000000000 +                        \
            (1000 * (VK_OHOS_NATIVE_BUFFER_EXTENSION_NUMBER - 1)) + (id)))
#define VK_STRUCTURE_TYPE_NATIVE_BUFFER_OHOS \
    VK_OHOS_NATIVE_BUFFER_ENUM(VkStructureType, 0)
#define VK_STRUCTURE_TYPE_SWAPCHAIN_IMAGE_CREATE_INFO_OHOS \
    VK_OHOS_NATIVE_BUFFER_ENUM(VkStructureType, 1)
#define VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTATION_PROPERTIES_OHOS \
    VK_OHOS_NATIVE_BUFFER_ENUM(VkStructureType, 2)

/* clang-format off */
typedef enum VkSwapchainImageUsageFlagBitsOHOS {
    VK_SWAPCHAIN_IMAGE_USAGE_SHARED_BIT_OHOS = 0x00000001,
    VK_SWAPCHAIN_IMAGE_USAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkSwapchainImageUsageFlagBitsOHOS;
typedef VkFlags VkSwapchainImageUsageFlagsOHOS;

/*
 * struct VkNativeBufferUsage2ANDROID
 *
 * consumer: gralloc1 consumer usage flag
 * producer: gralloc1 producer usage flag
 */
typedef struct {
    uint64_t                          consumer;
    uint64_t                          producer;
} VkNativeBufferUsage2OHOS;

/*
 * struct VkNativeBufferANDROID
 *
 * sType: VK_STRUCTURE_TYPE_NATIVE_BUFFER_ANDROID
 * pNext: NULL or a pointer to a structure extending this structure
 * handle: buffer handle returned from gralloc alloc()
 * stride: stride returned from gralloc alloc()
 * format: gralloc format requested when the buffer was allocated
 * usage: gralloc usage requested when the buffer was allocated
 * usage2: gralloc usage requested when the buffer was allocated
 */
typedef struct {
    VkStructureType                   sType;
    const void*                       pNext;
    BufferHandle*                     handle;
    int                               stride;
    int                               format;
    int                               usage; /* DEPRECATED in SPEC_VERSION 6 */
    VkNativeBufferUsage2OHOS       usage2; /* ADDED in SPEC_VERSION 6 */
} VkNativeBufferOHOS;

/*
 * struct VkSwapchainImageCreateInfoANDROID
 *
 * sType: VK_STRUCTURE_TYPE_SWAPCHAIN_IMAGE_CREATE_INFO_ANDROID
 * pNext: NULL or a pointer to a structure extending this structure
 * usage: is a bitmask of VkSwapchainImageUsageFlagsANDROID
 */
typedef struct {
    VkStructureType                   sType;
    const void*                       pNext;
    VkSwapchainImageUsageFlagsOHOS usage;
} VkSwapchainImageCreateInfoOHOS;

/*
 * struct VkPhysicalDevicePresentationPropertiesANDROID
 *
 * sType: VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTATION_PROPERTIES_ANDROID
 * pNext: NULL or a pointer to a structure extending this structure
 * sharedImage: specifies if the image can be shared with the display system
 */
typedef struct {
    VkStructureType                   sType;
    const void*                       pNext;
    VkBool32                          sharedImage;
} VkPhysicalDevicePresentationPropertiesOHOS;

/* DEPRECATED in SPEC_VERSION 6 */
typedef VkResult (VKAPI_PTR *PFN_vkGetSwapchainGrallocUsageOHOS)(
    VkDevice                          device,
    VkFormat                          format,
    VkImageUsageFlags                 imageUsage,
    int*                              grallocUsage);

/* ADDED in SPEC_VERSION 6 */
typedef VkResult (VKAPI_PTR *PFN_vkGetSwapchainGrallocUsage2OHOS)(
    VkDevice                          device,
    VkFormat                          format,
    VkImageUsageFlags                 imageUsage,
    VkSwapchainImageUsageFlagsOHOS swapchainImageUsage,
    uint64_t*                         grallocConsumerUsage,
    uint64_t*                         grallocProducerUsage);

typedef VkResult (VKAPI_PTR *PFN_vkAcquireImageOHOS)(
    VkDevice                          device,
    VkImage                           image,
    int                               nativeFenceFd,
    VkSemaphore                       semaphore,
    VkFence                           fence);

typedef VkResult (VKAPI_PTR *PFN_vkQueueSignalReleaseImageOHOS)(
    VkQueue                           queue,
    uint32_t                          waitSemaphoreCount,
    const VkSemaphore*                pWaitSemaphores,
    VkImage                           image,
    int*                              pNativeFenceFd);

#ifndef VK_NO_PROTOTYPES

/* DEPRECATED in SPEC_VERSION 6 */
VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainGrallocUsageOHOS(
    VkDevice                          device,
    VkFormat                          format,
    VkImageUsageFlags                 imageUsage,
    int*                              grallocUsage
);

/* ADDED in SPEC_VERSION 6 */
VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainGrallocUsage2OHOS(
    VkDevice                          device,
    VkFormat                          format,
    VkImageUsageFlags                 imageUsage,
    VkSwapchainImageUsageFlagsOHOS swapchainImageUsage,
    uint64_t*                         grallocConsumerUsage,
    uint64_t*                         grallocProducerUsage
);

VKAPI_ATTR VkResult VKAPI_CALL vkAcquireImageOHOS(
    VkDevice                          device,
    VkImage                           image,
    int                               nativeFenceFd,
    VkSemaphore                       semaphore,
    VkFence                           fence
);

VKAPI_ATTR VkResult VKAPI_CALL vkQueueSignalReleaseImageOHOS(
    VkQueue                           queue,
    uint32_t                          waitSemaphoreCount,
    const VkSemaphore*                pWaitSemaphores,
    VkImage                           image,
    int*                              pNativeFenceFd
);

#endif
/* clang-format on */

#ifdef __cplusplus
}
#endif

#endif /* __VK_OHOS_NATIVE_BUFFER_H__ */
