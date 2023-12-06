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

#include "vulkan_command_buffer.h"

#include "vulkan_hilog.h"
#include "vulkan_proc_table.h"

namespace OHOS::Rosen::vulkan {

RSVulkanCommandBuffer::RSVulkanCommandBuffer(
    const RSVulkanProcTable& p_vk,
    const RSVulkanHandle<VkDevice>& device,
    const RSVulkanHandle<VkCommandPool>& pool)
    : vk(p_vk), device_(device), pool_(pool), valid_(false) {
  const VkCommandBufferAllocateInfo allocate_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = pool_,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  VkCommandBuffer buffer = VK_NULL_HANDLE;

  if (VK_CALL_LOG_ERROR(vk.AllocateCommandBuffers(device_, &allocate_info,
                                                  &buffer)) != VK_SUCCESS) {
    LOGE("Could not allocate command buffers.");
    return;
  }

  auto buffer_collect = [this](VkCommandBuffer buffer) {
    vk.FreeCommandBuffers(device_, pool_, 1, &buffer);
  };

  handle_ = {buffer, buffer_collect};

  valid_ = true;
}

RSVulkanCommandBuffer::~RSVulkanCommandBuffer() = default;

bool RSVulkanCommandBuffer::IsValid() const {
  return valid_;
}

VkCommandBuffer RSVulkanCommandBuffer::Handle() const {
  return handle_;
}

bool RSVulkanCommandBuffer::Begin() const {
  const VkCommandBufferBeginInfo info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pInheritanceInfo = nullptr,
  };

  return VK_CALL_LOG_ERROR(vk.BeginCommandBuffer(handle_, &info)) == VK_SUCCESS;
}

bool RSVulkanCommandBuffer::End() const {
  return VK_CALL_LOG_ERROR(vk.EndCommandBuffer(handle_)) == VK_SUCCESS;
}

bool RSVulkanCommandBuffer::InsertPipelineBarrier(
    VkPipelineStageFlagBits src_stage_flags,
    VkPipelineStageFlagBits dest_stage_flags,
    uint32_t /* mask of VkDependencyFlagBits */ dependency_flags,
    uint32_t memory_barrier_count,
    const VkMemoryBarrier* memory_barriers,
    uint32_t buffer_memory_barrier_count,
    const VkBufferMemoryBarrier* buffer_memory_barriers,
    uint32_t image_memory_barrier_count,
    const VkImageMemoryBarrier* image_memory_barriers) const {
  vk.CmdPipelineBarrier(handle_, src_stage_flags, dest_stage_flags,
                        dependency_flags, memory_barrier_count, memory_barriers,
                        buffer_memory_barrier_count, buffer_memory_barriers,
                        image_memory_barrier_count, image_memory_barriers);
  return true;
}

}  // namespace OHOS::Rosen::vulkan
