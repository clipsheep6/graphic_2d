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

#ifndef RS_VULKAN_VULKAN_APPLICATION_H_
#define RS_VULKAN_VULKAN_APPLICATION_H_

#include <memory>
#include <string>
#include <vector>

#include "vulkan_debug_report.h"
#include "vulkan_handle.h"

namespace OHOS::Rosen::vulkan {

static const int K_GR_CACHE_MAX_COUNT = 8192;
static const size_t K_GR_CACHE_MAX_BYTE_SIZE = 512 * (1 << 20);

class RSVulkanDevice;
class RSVulkanProcTable;

/// Applications using Vulkan acquire a RSVulkanApplication that attempts to
/// create a VkInstance (with debug reporting optionally enabled).
class RSVulkanApplication {
public:
    RSVulkanApplication(RSVulkanProcTable& vk, const std::string& applicationName,
        std::vector<std::string> enabledExtensions, uint32_t applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        uint32_t apiVersion = VK_MAKE_VERSION(1, 0, 0));

    ~RSVulkanApplication();

    bool IsValid() const;

    uint32_t GetAPIVersion() const;

    const RSVulkanHandle<VkInstance>& GetInstance() const;

    void ReleaseInstanceOwnership();

    std::unique_ptr<RSVulkanDevice> AcquireFirstCompatibleLogicalDevice() const;

private:
    RSVulkanProcTable& vk;
    RSVulkanHandle<VkInstance> instance_;
    uint32_t apiVersion_;
    std::unique_ptr<RSVulkanDebugReport> debugReport_;
    bool valid_;

    std::vector<VkPhysicalDevice> GetPhysicalDevices() const;
    std::vector<VkExtensionProperties> GetSupportedInstanceExtensions(const RSVulkanProcTable& vk) const;
    bool ExtensionSupported(const std::vector<VkExtensionProperties>& supportedExtensions, std::string extensionName);
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_APPLICATION_H_
