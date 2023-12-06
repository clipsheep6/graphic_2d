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

#ifndef FLUTTER_VULKAN_VULKAN_BACKBUFFER_H_
#define FLUTTER_VULKAN_VULKAN_BACKBUFFER_H_

#include <array>

#include "vulkan_command_buffer.h"
#include "vulkan_handle.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace vulkan {

class VulkanBackbuffer {
 public:
  VulkanBackbuffer(const VulkanProcTable& vk,
                   const VulkanHandle<VkDevice>& device,
                   const VulkanHandle<VkCommandPool>& pool);

  ~VulkanBackbuffer();

  bool IsValid() const;

  bool WaitFences();

  bool ResetFences();

  const VulkanHandle<VkFence>& GetUsageFence() const;

  const VulkanHandle<VkFence>& GetRenderFence() const;

  const VulkanHandle<VkSemaphore>& GetUsageSemaphore() const;

  const VulkanHandle<VkSemaphore>& GetRenderSemaphore() const;

  VulkanCommandBuffer& GetUsageCommandBuffer();

  VulkanCommandBuffer& GetRenderCommandBuffer();

  void SetMultiThreading() { multi_threading_ = true; }

  void UnsetMultiThreading() { multi_threading_ = false; }

  bool IsMultiThreading() { return multi_threading_; }

 private:
  const VulkanProcTable& vk;
  const VulkanHandle<VkDevice>& device_;
  std::array<VulkanHandle<VkSemaphore>, 2> semaphores_;
  std::array<VulkanHandle<VkFence>, 2> use_fences_;
  VulkanCommandBuffer usage_command_buffer_;
  VulkanCommandBuffer render_command_buffer_;
  bool valid_;

  bool CreateSemaphores();

  bool CreateFences();

  bool multi_threading_ = false;
};

}  // namespace vulkan

#endif  // FLUTTER_VULKAN_VULKAN_BACKBUFFER_H_
