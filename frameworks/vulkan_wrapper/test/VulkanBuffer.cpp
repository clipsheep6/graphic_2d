/*
* Vulkan buffer class
*
* Encapsulates a Vulkan buffer
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "VulkanBuffer.h"

namespace vks
{    

VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset)
{
    return vkMapMemory(device, memory, offset, size, 0, &mapped);
}

void Buffer::unmap()
{
    if (mapped)
    {
        vkUnmapMemory(device, memory);
        mapped = nullptr;
    }
}

VkResult Buffer::bind(VkDeviceSize offset)
{
    return vkBindBufferMemory(device, buffer, memory, offset);
}

void Buffer::setupDescriptor(VkDeviceSize size, VkDeviceSize offset)
{
    descriptor.offset = offset;
    descriptor.buffer = buffer;
    descriptor.range = size;
}

VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(device, 1, &mappedRange);
}

VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(device, 1, &mappedRange);
}

void Buffer::destroy()
{
    if (buffer)
    {
        vkDestroyBuffer(device, buffer, nullptr);
    }
    if (memory)
    {
        vkFreeMemory(device, memory, nullptr);
    }
}
}//namespace vks
