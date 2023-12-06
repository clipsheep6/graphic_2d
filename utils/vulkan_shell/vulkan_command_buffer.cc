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
    const RSVulkanProcTable& p_vk, const RSVulkanHandle<VkDevice>& device, const RSVulkanHandle<VkCommandPool>& pool)
    : vk(p_vk), device_(device), pool_(pool), valid_(false)
{
    const VkCommandBufferAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = pool_,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer buffer = VK_NULL_HANDLE;

    if (VK_CALL_LOG_ERROR(vk.AllocateCommandBuffers(device_, &allocateInfo, &buffer)) != VK_SUCCESS) {
        LOGE("Could not allocate command buffers.");
        return;
    }

    auto buffer_collect = [this](VkCommandBuffer buffer) { vk.FreeCommandBuffers(device_, pool_, 1, &buffer); };

    handle_ = { buffer, buffer_collect };

    valid_ = true;
}

RSVulkanCommandBuffer::~RSVulkanCommandBuffer() = default;

bool RSVulkanCommandBuffer::IsValid() const
{
    return valid_;
}

VkCommandBuffer RSVulkanCommandBuffer::Handle() const
{
    return handle_;
}

bool RSVulkanCommandBuffer::Begin() const
{
    const VkCommandBufferBeginInfo info {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr,
    };
    return VK_CALL_LOG_ERROR(vk.BeginCommandBuffer(handle_, &info)) == VK_SUCCESS;
}

bool RSVulkanCommandBuffer::End() const
{
    return VK_CALL_LOG_ERROR(vk.EndCommandBuffer(handle_)) == VK_SUCCESS;
}

bool RSVulkanCommandBuffer::InsertPipelineBarrier(VkPipelineStageFlagBits srcStageFlags,
    VkPipelineStageFlagBits destStageFlags, uint32_t /* mask of VkDependencyFlagBits */ dependencyFlags,
    uint32_t memoryBarrierCount, const VkMemoryBarrier* memoryBarriers, uint32_t bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* bufferMemoryBarriers, uint32_t imageMemoryBarrierCount,
    const VkImageMemoryBarrier* imageMemoryBarriers) const
{
    vk.CmdPipelineBarrier(handle_, srcStageFlags, destStageFlags, dependencyFlags, memoryBarrierCount, memoryBarriers,
        bufferMemoryBarrierCount, bufferMemoryBarriers, imageMemoryBarrierCount, imageMemoryBarriers);
    return true;
}

} // namespace OHOS::Rosen::vulkan
