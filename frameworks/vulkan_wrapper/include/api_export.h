
#include "driver.h"
#include "swapchain.h"


__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    return vulkan::driver::CreateInstance(pCreateInfo, pAllocator, pInstance);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    vulkan::driver::DestroyInstance(instance, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) {
    return vulkan::driver::EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
}

__attribute__((visibility("default")))
VKAPI_ATTR PFN_vkVoidFunction vkGetDeviceProcAddr(VkDevice device, const char* pName) {
    return vulkan::driver::GetDeviceProcAddr(device, pName);
}

__attribute__((visibility("default")))
VKAPI_ATTR PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
    return vulkan::driver::GetInstanceProcAddr(instance, pName);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties) {
    vulkan::driver::GetPhysicalDeviceProperties(physicalDevice, pProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties) {
    vulkan::driver::GetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
    //vulkan::driver::GetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures) {
    //vulkan::driver::GetPhysicalDeviceFeatures(physicalDevice, pFeatures);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties) {
    //vulkan::driver::GetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) {
    //return vulkan::driver::GetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    //return vulkan::driver::CreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    //vulkan::driver::DestroyDevice(device, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkEnumerateInstanceVersion(uint32_t* pApiVersion) {
    // return vulkan::driver::EnumerateInstanceVersion(pApiVersion);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    // return vulkan::driver::EnumerateInstanceLayerProperties(pPropertyCount, pProperties);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
    // return vulkan::driver::EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    // return vulkan::driver::EnumerateDeviceLayerProperties(physicalDevice, pPropertyCount, pProperties);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
    // return vulkan::driver::EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount, pProperties);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) {
    // vulkan::driver::GetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    // return vulkan::driver::QueueSubmit(queue, submitCount, pSubmits, fence);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkQueueWaitIdle(VkQueue queue) {
    // return vulkan::driver::QueueWaitIdle(queue);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkDeviceWaitIdle(VkDevice device) {
    // return vulkan::driver::DeviceWaitIdle(device);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) {
    // return vulkan::driver::AllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::FreeMemory(device, memory, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) {
    return VK_SUCCESS;
    // return vulkan::driver::MapMemory(device, memory, offset, size, flags, ppData);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
    // vulkan::driver::UnmapMemory(device, memory);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) {
    // return vulkan::driver::FlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) {
    // return vulkan::driver::InvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes) {
    // vulkan::driver::GetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements) {
    // vulkan::driver::GetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
    // return vulkan::driver::BindBufferMemory(device, buffer, memory, memoryOffset);
    return VK_SUCCESS;
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements) {
    // vulkan::driver::GetImageMemoryRequirements(device, image, pMemoryRequirements);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
    // return vulkan::driver::BindImageMemory(device, image, memory, memoryOffset);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements) {
    // vulkan::driver::GetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties) {
    // vulkan::driver::GetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence) {
    // return vulkan::driver::QueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) {
    // return vulkan::driver::CreateFence(device, pCreateInfo, pAllocator, pFence);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyFence(device, fence, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences) {
    // return vulkan::driver::ResetFences(device, fenceCount, pFences);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetFenceStatus(VkDevice device, VkFence fence) {
    // return vulkan::driver::GetFenceStatus(device, fence);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout) {
    // return vulkan::driver::WaitForFences(device, fenceCount, pFences, waitAll, timeout);
    return VK_SUCCESS;
    
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) {
    // return vulkan::driver::CreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroySemaphore(device, semaphore, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent) {
    // return vulkan::driver::CreateEvent(device, pCreateInfo, pAllocator, pEvent);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyEvent(device, event, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetEventStatus(VkDevice device, VkEvent event) {
    // return vulkan::driver::GetEventStatus(device, event);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkSetEvent(VkDevice device, VkEvent event) {
    // return vulkan::driver::SetEvent(device, event);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkResetEvent(VkDevice device, VkEvent event) {
    // return vulkan::driver::ResetEvent(device, event);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool) {
    // return vulkan::driver::CreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyQueryPool(device, queryPool, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) {
    // return vulkan::driver::GetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
    return VK_SUCCESS;

}

__attribute__((visibility("default")))
VKAPI_ATTR void vkResetQueryPool(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount) {
    // vulkan::driver::ResetQueryPool(device, queryPool, firstQuery, queryCount);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyBuffer(device, buffer, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateBufferView(device, pCreateInfo, pAllocator, pView);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyBufferView(device, bufferView, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateImage(device, pCreateInfo, pAllocator, pImage);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyImage(device, image, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout) {
    // vulkan::driver::GetImageSubresourceLayout(device, image, pSubresource, pLayout);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateImageView(device, pCreateInfo, pAllocator, pView);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyImageView(device, imageView, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyShaderModule(device, shaderModule, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache) {
    return VK_SUCCESS;
    // return vulkan::driver::CreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyPipelineCache(device, pipelineCache, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData) {
    return VK_SUCCESS;
    // return vulkan::driver::GetPipelineCacheData(device, pipelineCache, pDataSize, pData);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches) {
    return VK_SUCCESS;
    // return vulkan::driver::MergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyPipeline(device, pipeline, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout) {
    return VK_SUCCESS;
    // return vulkan::driver::CreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyPipelineLayout(device, pipelineLayout, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateSampler(device, pCreateInfo, pAllocator, pSampler);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroySampler(device, sampler, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyDescriptorPool(device, descriptorPool, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags) {
    return VK_SUCCESS;
    // return vulkan::driver::ResetDescriptorPool(device, descriptorPool, flags);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) {
   return VK_SUCCESS;
    //  return vulkan::driver::AllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) {
    return VK_SUCCESS;
    // return vulkan::driver::FreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies) {
    // vulkan::driver::UpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyFramebuffer(device, framebuffer, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyRenderPass(device, renderPass, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity) {
    // vulkan::driver::GetRenderAreaGranularity(device, renderPass, pGranularity);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyCommandPool(device, commandPool, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags) {
    return VK_SUCCESS;
    // return vulkan::driver::ResetCommandPool(device, commandPool, flags);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) {
    return VK_SUCCESS;
    // return vulkan::driver::AllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
    // vulkan::driver::FreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) {
    return VK_SUCCESS;
    // return vulkan::driver::BeginCommandBuffer(commandBuffer, pBeginInfo);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkEndCommandBuffer(VkCommandBuffer commandBuffer) {
    return VK_SUCCESS;
    // return vulkan::driver::EndCommandBuffer(commandBuffer);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) {
    return VK_SUCCESS;
    // return vulkan::driver::ResetCommandBuffer(commandBuffer, flags);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) {
    // vulkan::driver::CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports) {
    // vulkan::driver::CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors) {
    // vulkan::driver::CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth) {
    // vulkan::driver::CmdSetLineWidth(commandBuffer, lineWidth);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) {
    // vulkan::driver::CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4]) {
    // vulkan::driver::CmdSetBlendConstants(commandBuffer, blendConstants);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds) {
    // vulkan::driver::CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask) {
    // vulkan::driver::CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask) {
    // vulkan::driver::CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference) {
    // vulkan::driver::CmdSetStencilReference(commandBuffer, faceMask, reference);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) {
    // vulkan::driver::CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) {
    // vulkan::driver::CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) {
    // vulkan::driver::CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    // vulkan::driver::CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    // vulkan::driver::CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
    // vulkan::driver::CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
    // vulkan::driver::CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    // vulkan::driver::CmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset) {
    // // vulkan::driver::CmdDispatchIndirect(commandBuffer, buffer, offset);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions) {
    // // vulkan::driver::CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions) {
    // vulkan::driver::CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter) {
    // vulkan::driver::CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions) {
    // vulkan::driver::CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions) {
    // vulkan::driver::CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData) {
    // vulkan::driver::CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data) {
    // vulkan::driver::CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
    // vulkan::driver::CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
    // vulkan::driver::CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects) {
    // vulkan::driver::CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions) {
    // vulkan::driver::CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) {
    // vulkan::driver::CmdSetEvent(commandBuffer, event, stageMask);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) {
    // vulkan::driver::CmdResetEvent(commandBuffer, event, stageMask);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
    // vulkan::driver::CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
    // vulkan::driver::CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags) {
    // vulkan::driver::CmdBeginQuery(commandBuffer, queryPool, query, flags);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query) {
    // vulkan::driver::CmdEndQuery(commandBuffer, queryPool, query);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount) {
    // vulkan::driver::CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query) {
    // vulkan::driver::CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags) {
    // vulkan::driver::CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues) {
    // vulkan::driver::CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) {
    // vulkan::driver::CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents) {
    // vulkan::driver::CmdNextSubpass(commandBuffer, contents);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {
    // vulkan::driver::CmdEndRenderPass(commandBuffer);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
    // vulkan::driver::CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateOHOSSurfaceOpenHarmony(VkInstance instance, const VkOHOSSurfaceCreateInfoOpenHarmony* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    return vulkan::driver::CreateOHOSSurfaceOpenHarmony(instance, pCreateInfo, pAllocator, pSurface);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroySurfaceKHR(instance, surface, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported) {
    return VK_SUCCESS;
    // return vulkan::driver::GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) {
    return VK_SUCCESS;
    // return vulkan::driver::GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats) {
    return VK_SUCCESS;
    // return vulkan::driver::GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes) {
    return VK_SUCCESS;
    // return vulkan::driver::GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroySwapchainKHR(device, swapchain, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages) {
    return VK_SUCCESS;
    // return vulkan::driver::GetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) {
    return VK_SUCCESS;
    // return vulkan::driver::AcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) {
    return VK_SUCCESS;
    // return vulkan::driver::QueuePresentKHR(queue, pPresentInfo);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures) {
    // vulkan::driver::GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties) {
    // vulkan::driver::GetPhysicalDeviceProperties2(physicalDevice, pProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2* pFormatProperties) {
    // vulkan::driver::GetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo, VkImageFormatProperties2* pImageFormatProperties) {
    return VK_SUCCESS;
    // return vulkan::driver::GetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties) {
    // vulkan::driver::GetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {
    // vulkan::driver::GetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceSparseImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount, VkSparseImageFormatProperties2* pProperties) {
    // vulkan::driver::GetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkTrimCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags) {
    // vulkan::driver::TrimCommandPool(device, commandPool, flags);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceExternalBufferProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo, VkExternalBufferProperties* pExternalBufferProperties) {
    // vulkan::driver::GetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceExternalSemaphoreProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo, VkExternalSemaphoreProperties* pExternalSemaphoreProperties) {
    // vulkan::driver::GetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetPhysicalDeviceExternalFenceProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo, VkExternalFenceProperties* pExternalFenceProperties) {
    // vulkan::driver::GetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkEnumeratePhysicalDeviceGroups(VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    return VK_SUCCESS;
    // return vulkan::driver::EnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetDeviceGroupPeerMemoryFeatures(VkDevice device, uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) {
    // vulkan::driver::GetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkBindBufferMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos) {
    return VK_SUCCESS;
    // return vulkan::driver::BindBufferMemory2(device, bindInfoCount, pBindInfos);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkBindImageMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos) {
    return VK_SUCCESS;
    // return vulkan::driver::BindImageMemory2(device, bindInfoCount, pBindInfos);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask) {
    // vulkan::driver::CmdSetDeviceMask(commandBuffer, deviceMask);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetDeviceGroupPresentCapabilitiesKHR(VkDevice device, VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities) {
    return VK_SUCCESS;
    // return vulkan::driver::GetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface, VkDeviceGroupPresentModeFlagsKHR* pModes) {
    return VK_SUCCESS;
    // return vulkan::driver::GetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkAcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex) {
    return VK_SUCCESS;
    // return vulkan::driver::AcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    // vulkan::driver::CmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pRectCount, VkRect2D* pRects) {
    return VK_SUCCESS;
    // return vulkan::driver::GetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, pRectCount, pRects);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateDescriptorUpdateTemplate(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroyDescriptorUpdateTemplate(VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkUpdateDescriptorSetWithTemplate(VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void* pData) {
    // vulkan::driver::UpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetBufferMemoryRequirements2(VkDevice device, const VkBufferMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) {
    // vulkan::driver::GetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetImageMemoryRequirements2(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) {
    // vulkan::driver::GetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetImageSparseMemoryRequirements2(VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    // vulkan::driver::GetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateSamplerYcbcrConversion(VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkDestroySamplerYcbcrConversion(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks* pAllocator) {
    // vulkan::driver::DestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue) {
    // vulkan::driver::GetDeviceQueue2(device, pQueueInfo, pQueue);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkGetDescriptorSetLayoutSupport(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport) {
    // vulkan::driver::GetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkCreateRenderPass2(VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) {
    return VK_SUCCESS;
    // return vulkan::driver::CreateRenderPass2(device, pCreateInfo, pAllocator, pRenderPass);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdBeginRenderPass2(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, const VkSubpassBeginInfo* pSubpassBeginInfo) {
    // vulkan::driver::CmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdNextSubpass2(VkCommandBuffer commandBuffer, const VkSubpassBeginInfo* pSubpassBeginInfo, const VkSubpassEndInfo* pSubpassEndInfo) {
    // vulkan::driver::CmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdEndRenderPass2(VkCommandBuffer commandBuffer, const VkSubpassEndInfo* pSubpassEndInfo) {
    // vulkan::driver::CmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkGetSemaphoreCounterValue(VkDevice device, VkSemaphore semaphore, uint64_t* pValue) {
    return VK_SUCCESS;
    // return vulkan::driver::GetSemaphoreCounterValue(device, semaphore, pValue);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkWaitSemaphores(VkDevice device, const VkSemaphoreWaitInfo* pWaitInfo, uint64_t timeout) {
    return VK_SUCCESS;
    // return vulkan::driver::WaitSemaphores(device, pWaitInfo, timeout);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkResult vkSignalSemaphore(VkDevice device, const VkSemaphoreSignalInfo* pSignalInfo) {
    return VK_SUCCESS;
    // return vulkan::driver::SignalSemaphore(device, pSignalInfo);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdDrawIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride) {
    // vulkan::driver::CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

__attribute__((visibility("default")))
VKAPI_ATTR void vkCmdDrawIndexedIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride) {
    // vulkan::driver::CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

__attribute__((visibility("default")))
VKAPI_ATTR uint64_t vkGetBufferOpaqueCaptureAddress(VkDevice device, const VkBufferDeviceAddressInfo* pInfo) {
    return VK_SUCCESS;
    // return vulkan::driver::GetBufferOpaqueCaptureAddress(device, pInfo);
}

__attribute__((visibility("default")))
VKAPI_ATTR VkDeviceAddress vkGetBufferDeviceAddress(VkDevice device, const VkBufferDeviceAddressInfo* pInfo) {
    return VK_SUCCESS;
    // return vulkan::driver::GetBufferDeviceAddress(device, pInfo);
}

__attribute__((visibility("default")))
VKAPI_ATTR uint64_t vkGetDeviceMemoryOpaqueCaptureAddress(VkDevice device, const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    return VK_SUCCESS;
    // return vulkan::driver::GetDeviceMemoryOpaqueCaptureAddress(device, pInfo);
}
