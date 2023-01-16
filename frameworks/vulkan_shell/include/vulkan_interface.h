// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_VULKAN_VULKAN_INTERFACE_H_
#define FLUTTER_VULKAN_VULKAN_INTERFACE_H_

#include <string>

#ifndef VK_USE_PLATFORM_OHOS_KHR
#define VK_USE_PLATFORM_OHOS_KHR
#endif // VK_USE_PLATFORM_OHOS_KHR

#if !VULKAN_LINK_STATICALLY
#define VK_NO_PROTOTYPES
#endif // !VULKAN_LINK_STATICALLY

#include <vulkan/vulkan.h>

#ifndef NDEBUG

#define VK_CALL_LOG_ERROR(expression)                                                                      \
    ({                                                                                                     \
        __typeof__(expression) _rc = (expression);                                                         \
        if (_rc != VK_SUCCESS) {                                                                           \
            LOGE("Vulkan call '" #expression "' failed with error %s", vulkan::VulkanResultToString(_rc)); \
        }                                                                                                  \
        _rc;                                                                                               \
    })

#else // NDEBUG

#define VK_CALL_LOG_ERROR(expression) (expression)

#endif // NDEBUG

namespace OHOS {
namespace Rosen {
namespace vulkan {

std::string VulkanResultToString(VkResult result);

} // namespace vulkan
} // namespace Rosen
} // namespace OHOS
#endif // FLUTTER_VULKAN_VULKAN_INTERFACE_H_
