// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_VULKAN_VULKAN_INTERFACE_H_
#define FLUTTER_VULKAN_VULKAN_INTERFACE_H_

#include <string>



#ifdef RS_ENABLE_VK
#ifndef VK_USE_PLATFORM_OHOS
#define VK_USE_PLATFORM_OHOS
#endif  // VK_USE_PLATFORM_OHOS
#endif  // RS_ENABLE_VK

#if !VULKAN_LINK_STATICALLY
#define VK_NO_PROTOTYPES 1
#endif  // !VULKAN_LINK_STATICALLY

#include "vulkan/vulkan.h"

#ifndef NDEBUG

#ifdef RS_ENABLE_VK
#define VK_CALL_LOG_ERROR(expression)                            \
  ({                                                             \
    __typeof__(expression) _rc = (expression);                   \
    if (_rc != VK_SUCCESS) {                                     \
      LOGE("Vulkan call '" #expression "' failed with error %s", \
           vulkan::VulkanResultToString(_rc));                   \
    }                                                            \
    _rc;                                                         \
  })
#else
#define VK_CALL_LOG_ERROR(expression)                      \
  ({                                                       \
    __typeof__(expression) _rc = (expression);             \
    if (_rc != VK_SUCCESS) {                               \
      FML_DLOG(INFO) << "Vulkan call '" << #expression     \
                     << "' failed with error "             \
                     << vulkan::VulkanResultToString(_rc); \
    }                                                      \
    _rc;                                                   \
  })
#endif

#else  // NDEBUG

#define VK_CALL_LOG_ERROR(expression) (expression)

#endif  // NDEBUG

namespace vulkan {

std::string VulkanResultToString(VkResult result);

}  // namespace vulkan

#endif  // FLUTTER_VULKAN_VULKAN_INTERFACE_H_
