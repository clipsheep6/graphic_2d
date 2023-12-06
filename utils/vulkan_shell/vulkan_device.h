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

#ifndef RS_VULKAN_VULKAN_DEVICE_H_
#define RS_VULKAN_VULKAN_DEVICE_H_

#include <vector>

#include "vulkan_handle.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanProcTable;
class RSVulkanSurface;

class RSVulkanDevice {
 public:
  RSVulkanDevice(RSVulkanProcTable& vk,
               RSVulkanHandle<VkPhysicalDevice> physical_device);

  ~RSVulkanDevice();

  bool IsValid() const;

  const RSVulkanHandle<VkDevice>& GetHandle() const;

  const RSVulkanHandle<VkPhysicalDevice>& GetPhysicalDeviceHandle() const;

  const RSVulkanHandle<VkQueue>& GetQueueHandle() const;

  const RSVulkanHandle<VkCommandPool>& GetCommandPool() const;

  uint32_t GetGraphicsQueueIndex() const;

  void ReleaseDeviceOwnership();

  bool GetSurfaceCapabilities(const RSVulkanSurface& surface,
                              VkSurfaceCapabilitiesKHR* capabilities) const;

  bool GetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures* features) const;

  bool GetPhysicalDeviceFeaturesSkia(
      uint32_t* /* mask of GrVkFeatureFlags */ features) const;

  int ChooseSurfaceFormat(const RSVulkanSurface& surface,
                          std::vector<VkFormat> desired_formats,
                          VkSurfaceFormatKHR* format) const;

  bool ChoosePresentMode(const RSVulkanSurface& surface,
                         VkPresentModeKHR* present_mode) const;

  bool QueueSubmit(std::vector<VkPipelineStageFlags> wait_dest_pipeline_stages,
                   const std::vector<VkSemaphore>& wait_semaphores,
                   const std::vector<VkSemaphore>& signal_semaphores,
                   const std::vector<VkCommandBuffer>& command_buffers,
                   const RSVulkanHandle<VkFence>& fence) const;

  bool WaitIdle() const;
  RSVulkanProcTable& vk;
  RSVulkanHandle<VkPhysicalDevice> physical_device_;
  RSVulkanHandle<VkDevice> device_;

 private:
  RSVulkanHandle<VkQueue> queue_;
  RSVulkanHandle<VkCommandPool> command_pool_;
  uint32_t graphics_queue_index_;
  uint32_t compute_queue_index_;
  bool valid_;

  std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties() const;
};

}  // namespace OHOS::Rosen::vulkan 

#endif  // RS_VULKAN_VULKAN_DEVICE_H_
