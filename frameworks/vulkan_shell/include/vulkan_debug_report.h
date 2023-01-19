// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_VULKAN_VULKAN_DEBUG_REPORT_H_
#define FLUTTER_VULKAN_VULKAN_DEBUG_REPORT_H_

#include "vulkan_handle.h"
#include "vulkan_interface.h"
#include "vulkan_proc_table.h"

namespace OHOS {
namespace Rosen {
namespace vulkan {

class VulkanDebugReport {
public:
    static std::string DebugExtensionName();

    VulkanDebugReport(const VulkanProcTable& vk, const VulkanHandle<VkInstance>& application);

    ~VulkanDebugReport();

    bool IsValid() const;

private:
    const VulkanProcTable& vk;
    const VulkanHandle<VkInstance>& application_;
    VulkanHandle<VkDebugReportCallbackEXT> handle_;
    bool valid_;
};

} // namespace vulkan
} // namespace Rosen
} // namespace OHOS
#endif // FLUTTER_VULKAN_VULKAN_DEBUG_REPORT_H_
