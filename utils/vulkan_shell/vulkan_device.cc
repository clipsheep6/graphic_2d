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

#include "vulkan_device.h"

#include <limits>
#include <map>
#include <vector>

#include "vulkan_hilog.h"
#include "vulkan_proc_table.h"
#include "vulkan_surface.h"
#include "vulkan_utilities.h"
#include "third_party/skia/include/gpu/vk/GrVkBackendContext.h"

namespace OHOS::Rosen::vulkan {

constexpr auto kVulkanInvalidGraphicsQueueIndex =
    std::numeric_limits<uint32_t>::max();

static uint32_t FindQueueIndex(const std::vector<VkQueueFamilyProperties>& properties, VkQueueFlagBits flagBits) {
  for (uint32_t i = 0, count = static_cast<uint32_t>(properties.size()); i < count; i++) {
    if (properties[i].queueFlags & flagBits) {
      return i;
    }
  }
  return kVulkanInvalidGraphicsQueueIndex;
}

RSVulkanDevice::RSVulkanDevice(RSVulkanProcTable& p_vk, RSVulkanHandle<VkPhysicalDevice> physical_device)
    : vk(p_vk), physicalDevice_(std::move(physical_device)),
      graphicQueueIndex_(std::numeric_limits<uint32_t>::max()),
      computeQueueIndex_(std::numeric_limits<uint32_t>::max()), valid_(false)
{
  if (!physicalDevice_ || !vk.AreInstanceProcsSetup()) {
    return;
  }

  std::vector<VkQueueFamilyProperties> properties = GetQueueFamilyProperties();
  graphicQueueIndex_ = FindQueueIndex(properties, VK_QUEUE_GRAPHICS_BIT);
  computeQueueIndex_ = FindQueueIndex(properties, VK_QUEUE_COMPUTE_BIT);

  if (graphicQueueIndex_ == kVulkanInvalidGraphicsQueueIndex) {
    LOGE("Could not find the graphics queue index.");
    return;
  }

  const float priorities[1] = {1.0f};

  std::vector<VkDeviceQueueCreateInfo> queue_create { {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = graphicQueueIndex_,
      .queueCount = 1,
      .pQueuePriorities = priorities,
  } };

  if (computeQueueIndex_ != kVulkanInvalidGraphicsQueueIndex && computeQueueIndex_ != graphicQueueIndex_) {
    queue_create.push_back({
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = graphicQueueIndex_,
        .queueCount = 1,
        .pQueuePriorities = priorities,
    });
  }

  const char* extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  auto enabled_layers = DeviceLayersToEnable(vk, physicalDevice_);

  const char* layers[enabled_layers.size()];

  for (size_t i = 0; i < enabled_layers.size(); i++) {
    layers[i] = enabled_layers[i].c_str();
  }

  const VkDeviceCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueCreateInfoCount = queue_create.size(),
      .pQueueCreateInfos = queue_create.data(),
      .enabledLayerCount = static_cast<uint32_t>(enabled_layers.size()),
      .ppEnabledLayerNames = layers,
      .enabledExtensionCount = sizeof(extensions) / sizeof(const char*),
      .ppEnabledExtensionNames = extensions,
      .pEnabledFeatures = nullptr,
  };

  VkDevice device = VK_NULL_HANDLE;

  if (VK_CALL_LOG_ERROR(vk.CreateDevice(physicalDevice_, &create_info, nullptr,
                                        &device)) != VK_SUCCESS) {
    LOGE("Could not create device.");
    return;
  }

  device_ = {device,
              [this](VkDevice device) { vk.DestroyDevice(device, nullptr); }};

  if (!vk.SetupDeviceProcAddresses(device_)) {
    LOGE("Could not setup device proc addresses.");
    return;
  }

  VkQueue queue = VK_NULL_HANDLE;

  vk.GetDeviceQueue(device_, graphicQueueIndex_, 0, &queue);

  if (queue == VK_NULL_HANDLE) {
    LOGE("Could not get the device queue handle.");
    return;
  }

  queue_ = queue;

  const VkCommandPoolCreateInfo command_pool_create_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = 0,
  };

  VkCommandPool command_pool = VK_NULL_HANDLE;
  if (VK_CALL_LOG_ERROR(vk.CreateCommandPool(device_, &command_pool_create_info,
                                             nullptr, &command_pool)) !=
      VK_SUCCESS) {
    LOGE("Could not create the command pool.");
    return;
  }

  commandPool_ = {command_pool, [this](VkCommandPool pool) {
                    vk.DestroyCommandPool(device_, pool, nullptr);
                  }};

  valid_ = true;
}

RSVulkanDevice::~RSVulkanDevice() {
  WaitIdle();
}

bool RSVulkanDevice::IsValid() const {
  return valid_;
}

bool RSVulkanDevice::WaitIdle() const {
  return VK_CALL_LOG_ERROR(vk.DeviceWaitIdle(device_)) == VK_SUCCESS;
}

const RSVulkanHandle<VkDevice>& RSVulkanDevice::GetHandle() const {
  return device_;
}

void RSVulkanDevice::ReleaseDeviceOwnership() {
  device_.ReleaseOwnership();
}

const RSVulkanHandle<VkPhysicalDevice>& RSVulkanDevice::GetPhysicalDeviceHandle()
    const {
  return physicalDevice_;
}

const RSVulkanHandle<VkQueue>& RSVulkanDevice::GetQueueHandle() const {
  return queue_;
}

const RSVulkanHandle<VkCommandPool>& RSVulkanDevice::GetCommandPool() const {
  return commandPool_;
}

uint32_t RSVulkanDevice::GetGraphicsQueueIndex() const {
  return graphicQueueIndex_;
}

bool RSVulkanDevice::GetSurfaceCapabilities(
    const RSVulkanSurface& surface,
    VkSurfaceCapabilitiesKHR* capabilities) const {
  if (!surface.IsValid() || capabilities == nullptr) {
    LOGE("GetSurfaceCapabilities surface is not valid or capabilities is null");
    return false;
  }

  bool success =
      VK_CALL_LOG_ERROR(vk.GetPhysicalDeviceSurfaceCapabilitiesKHR(
          physicalDevice_, surface.Handle(), capabilities)) == VK_SUCCESS;

  if (!success) {
    LOGE("GetPhysicalDeviceSurfaceCapabilitiesKHR not success");
    return false;
  }

  // Check if the physical device surface capabilities are valid. If so, there
  // is nothing more to do.
  if (capabilities->currentExtent.width != 0xFFFFFFFF &&
      capabilities->currentExtent.height != 0xFFFFFFFF) {
    return true;
  }

  // Ask the native surface for its size as a fallback.
  SkISize size = surface.GetSize();

  if (size.width() == 0 || size.height() == 0) {
    LOGE("GetSurfaceCapabilities surface size is 0");
    return false;
  }

  capabilities->currentExtent.width = size.width();
  capabilities->currentExtent.height = size.height();
  return true;
}

bool RSVulkanDevice::GetPhysicalDeviceFeatures(
    VkPhysicalDeviceFeatures* features) const {
  if (features == nullptr || !physicalDevice_) {
    return false;
  }
  vk.GetPhysicalDeviceFeatures(physicalDevice_, features);
  return true;
}

bool RSVulkanDevice::GetPhysicalDeviceFeaturesSkia(uint32_t* sk_features) const {
  if (sk_features == nullptr) {
    return false;
  }

  VkPhysicalDeviceFeatures features;

  if (!GetPhysicalDeviceFeatures(&features)) {
    return false;
  }

  uint32_t flags = 0;

  if (features.geometryShader) {
    flags |= kGeometryShader_GrVkFeatureFlag;
  }
  if (features.dualSrcBlend) {
    flags |= kDualSrcBlend_GrVkFeatureFlag;
  }
  if (features.sampleRateShading) {
    flags |= kSampleRateShading_GrVkFeatureFlag;
  }

  *sk_features = flags;
  return true;
}

std::vector<VkQueueFamilyProperties> RSVulkanDevice::GetQueueFamilyProperties()
    const {
  uint32_t count = 0;

  vk.GetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &count, nullptr);

  std::vector<VkQueueFamilyProperties> properties;
  properties.resize(count, {});

  vk.GetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &count,
                                            properties.data());

  return properties;
}

int RSVulkanDevice::ChooseSurfaceFormat(const RSVulkanSurface& surface,
                                      std::vector<VkFormat> desired_formats,
                                      VkSurfaceFormatKHR* format) const {
  if (!surface.IsValid() || format == nullptr) {
    LOGE("ChooseSurfaceFormat surface not valid or format == null");
    return -1;
  }

  uint32_t format_count = 0;
  if (VK_CALL_LOG_ERROR(vk.GetPhysicalDeviceSurfaceFormatsKHR(
          physicalDevice_, surface.Handle(), &format_count, nullptr)) !=
      VK_SUCCESS) {
    LOGE("ChooseSurfaceFormat sGetPhysicalDeviceSurfaceFormatsKHR not success");
    return -1;
  }

  if (format_count == 0) {
    LOGE("ChooseSurfaceFormat format count = 0");
    return -1;
  }

  VkSurfaceFormatKHR formats[format_count];
  if (VK_CALL_LOG_ERROR(vk.GetPhysicalDeviceSurfaceFormatsKHR(
          physicalDevice_, surface.Handle(), &format_count, formats)) !=
      VK_SUCCESS) {
    LOGE("ChooseSurfaceFormat sGetPhysicalDeviceSurfaceFormatsKHR not success 2");
    return -1;
  }

  std::map<VkFormat, VkSurfaceFormatKHR> supported_formats;
  for (uint32_t i = 0; i < format_count; i++) {
    supported_formats[formats[i].format] = formats[i];
  }

  // Try to find the first supported format in the list of desired formats.
  for (size_t i = 0; i < desired_formats.size(); ++i) {
    auto found = supported_formats.find(desired_formats[i]);
    if (found != supported_formats.end()) {
      *format = found->second;
      return static_cast<int>(i);
    }
  }
  LOGE("ChooseSurfaceFormat failded");
  return -1;
}

bool RSVulkanDevice::ChoosePresentMode(const RSVulkanSurface& surface,
                                     VkPresentModeKHR* present_mode) const {
  if (!surface.IsValid() || present_mode == nullptr) {
    LOGE("ChoosePresentMode surface not valid or presentmode is null");
    return false;
  }

  // https://github.com/LunarG/VulkanSamples/issues/98 indicates that
  // VK_PRESENT_MODE_FIFO_KHR is preferable on mobile platforms. The problems
  // mentioned in the ticket w.r.t the application being faster that the refresh
  // rate of the screen should not be faced by any Flutter platforms as they are
  // powered by Vsync pulses instead of depending the the submit to block.
  // However, for platforms that don't have VSync providers setup, it is better
  // to fall back to FIFO. For platforms that do have VSync providers, there
  // should be little difference. In case there is a need for a mode other than
  // FIFO, availability checks must be performed here before returning the
  // result. FIFO is always present.
  *present_mode = VK_PRESENT_MODE_FIFO_KHR;
  return true;
}

bool RSVulkanDevice::QueueSubmit(
    std::vector<VkPipelineStageFlags> wait_dest_pipeline_stages,
    const std::vector<VkSemaphore>& wait_semaphores,
    const std::vector<VkSemaphore>& signal_semaphores,
    const std::vector<VkCommandBuffer>& command_buffers,
    const RSVulkanHandle<VkFence>& fence) const {
  if (wait_semaphores.size() != wait_dest_pipeline_stages.size()) {
    return false;
  }

  const VkSubmitInfo submit_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = static_cast<uint32_t>(wait_semaphores.size()),
      .pWaitSemaphores = wait_semaphores.data(),
      .pWaitDstStageMask = wait_dest_pipeline_stages.data(),
      .commandBufferCount = static_cast<uint32_t>(command_buffers.size()),
      .pCommandBuffers = command_buffers.data(),
      .signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size()),
      .pSignalSemaphores = signal_semaphores.data(),
  };

  if (VK_CALL_LOG_ERROR(vk.QueueSubmit(queue_, 1, &submit_info, fence)) !=
      VK_SUCCESS) {
    return false;
  }

  return true;
}

}  // namespace OHOS::Rosen::vulkan
