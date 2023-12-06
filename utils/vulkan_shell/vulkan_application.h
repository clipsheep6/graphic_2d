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

#ifndef FLUTTER_VULKAN_VULKAN_APPLICATION_H_
#define FLUTTER_VULKAN_VULKAN_APPLICATION_H_

#include <memory>
#include <string>
#include <vector>

#include "vulkan_debug_report.h"
#include "vulkan_handle.h"

namespace vulkan {

static const int kGrCacheMaxCount = 8192;
static const size_t kGrCacheMaxByteSize = 512 * (1 << 20);

class VulkanDevice;
class VulkanProcTable;

/// Applications using Vulkan acquire a VulkanApplication that attempts to
/// create a VkInstance (with debug reporting optionally enabled).
class VulkanApplication {
 public:
  VulkanApplication(VulkanProcTable& vk,
                    const std::string& application_name,
                    std::vector<std::string> enabled_extensions,
                    uint32_t application_version = VK_MAKE_VERSION(1, 0, 0),
                    uint32_t api_version = VK_MAKE_VERSION(1, 0, 0));

  ~VulkanApplication();

  bool IsValid() const;

  uint32_t GetAPIVersion() const;

  const VulkanHandle<VkInstance>& GetInstance() const;

  void ReleaseInstanceOwnership();

  std::unique_ptr<VulkanDevice> AcquireFirstCompatibleLogicalDevice() const;

 private:
  VulkanProcTable& vk;
  VulkanHandle<VkInstance> instance_;
  uint32_t api_version_;
  std::unique_ptr<VulkanDebugReport> debug_report_;
  bool valid_;

  std::vector<VkPhysicalDevice> GetPhysicalDevices() const;
  std::vector<VkExtensionProperties> GetSupportedInstanceExtensions(
      const VulkanProcTable& vk) const;
  bool ExtensionSupported(
      const std::vector<VkExtensionProperties>& supported_extensions,
      std::string extension_name);
};

}  // namespace vulkan

#endif  // FLUTTER_VULKAN_VULKAN_APPLICATION_H_
