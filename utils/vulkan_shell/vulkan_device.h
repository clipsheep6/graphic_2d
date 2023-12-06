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

#ifndef FLUTTER_VULKAN_VULKAN_DEVICE_H_
#define FLUTTER_VULKAN_VULKAN_DEVICE_H_

#include <vector>

#include "vulkan_handle.h"

namespace vulkan {

class VulkanProcTable;
class VulkanSurface;

class VulkanDevice {
 public:
  VulkanDevice(VulkanProcTable& vk,
               VulkanHandle<VkPhysicalDevice> physical_device);

  ~VulkanDevice();

  bool IsValid() const;

  const VulkanHandle<VkDevice>& GetHandle() const;

  const VulkanHandle<VkPhysicalDevice>& GetPhysicalDeviceHandle() const;

  const VulkanHandle<VkQueue>& GetQueueHandle() const;

  const VulkanHandle<VkCommandPool>& GetCommandPool() const;

  uint32_t GetGraphicsQueueIndex() const;

  void ReleaseDeviceOwnership();

  bool GetSurfaceCapabilities(const VulkanSurface& surface,
                              VkSurfaceCapabilitiesKHR* capabilities) const;

  bool GetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures* features) const;

  bool GetPhysicalDeviceFeaturesSkia(
      uint32_t* /* mask of GrVkFeatureFlags */ features) const;

  int ChooseSurfaceFormat(const VulkanSurface& surface,
                          std::vector<VkFormat> desired_formats,
                          VkSurfaceFormatKHR* format) const;

  bool ChoosePresentMode(const VulkanSurface& surface,
                         VkPresentModeKHR* present_mode) const;

  bool QueueSubmit(std::vector<VkPipelineStageFlags> wait_dest_pipeline_stages,
                   const std::vector<VkSemaphore>& wait_semaphores,
                   const std::vector<VkSemaphore>& signal_semaphores,
                   const std::vector<VkCommandBuffer>& command_buffers,
                   const VulkanHandle<VkFence>& fence) const;

  bool WaitIdle() const;
  VulkanProcTable& vk;
  VulkanHandle<VkPhysicalDevice> physical_device_;
  VulkanHandle<VkDevice> device_;

 private:
  VulkanHandle<VkQueue> queue_;
  VulkanHandle<VkCommandPool> command_pool_;
  uint32_t graphics_queue_index_;
#ifdef RS_ENABLE_VK
  uint32_t compute_queue_index_;
#endif
  bool valid_;

  std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties() const;
};

}  // namespace vulkan

#endif  // FLUTTER_VULKAN_VULKAN_DEVICE_H_
