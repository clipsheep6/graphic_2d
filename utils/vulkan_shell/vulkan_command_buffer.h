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

#ifndef RS_VULKAN_VULKAN_COMMAND_BUFFER_H_
#define RS_VULKAN_VULKAN_COMMAND_BUFFER_H_

#include "vulkan_handle.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanProcTable;

class RSVulkanCommandBuffer {
 public:
  RSVulkanCommandBuffer(const RSVulkanProcTable& vk,
                      const RSVulkanHandle<VkDevice>& device,
                      const RSVulkanHandle<VkCommandPool>& pool);

  ~RSVulkanCommandBuffer();

  bool IsValid() const;

  VkCommandBuffer Handle() const;

  bool Begin() const;

  bool End() const;

  bool InsertPipelineBarrier(
      VkPipelineStageFlagBits src_stage_flags,
      VkPipelineStageFlagBits dest_stage_flags,
      uint32_t /* mask of VkDependencyFlagBits */ dependency_flags,
      uint32_t memory_barrier_count,
      const VkMemoryBarrier* memory_barriers,
      uint32_t buffer_memory_barrier_count,
      const VkBufferMemoryBarrier* buffer_memory_barriers,
      uint32_t image_memory_barrier_count,
      const VkImageMemoryBarrier* image_memory_barriers) const;

 private:
  const RSVulkanProcTable& vk;
  const RSVulkanHandle<VkDevice>& device_;
  const RSVulkanHandle<VkCommandPool>& pool_;
  RSVulkanHandle<VkCommandBuffer> handle_;
  bool valid_;
};

}  // namespace OHOS::Rosen::vulkan 

#endif  // RS_VULKAN_VULKAN_COMMAND_BUFFER_H_
