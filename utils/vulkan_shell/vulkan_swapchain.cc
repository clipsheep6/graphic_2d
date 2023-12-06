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

#include "vulkan_swapchain.h"

#include "vulkan_backbuffer.h"
#include "vulkan_device.h"
#include "vulkan_hilog.h"
#include "vulkan_image.h"
#include "vulkan_proc_table.h"
#include "vulkan_surface.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrDirectContext.h"
#include "third_party/skia/include/gpu/vk/GrVkTypes.h"

namespace OHOS::Rosen::vulkan {

namespace {
struct FormatInfo {
  VkFormat format_;
  SkColorType color_type_;
  sk_sp<SkColorSpace> color_space_;
};
}  // namespace

static std::vector<FormatInfo> DesiredFormatInfos() {
  return { {VK_FORMAT_R8G8B8A8_UNORM, kRGBA_8888_SkColorType,
           SkColorSpace::MakeSRGB()},
          {VK_FORMAT_B8G8R8A8_UNORM, kRGBA_8888_SkColorType,
           SkColorSpace::MakeSRGB()},
          {VK_FORMAT_R8G8B8A8_SRGB, kSRGBA_8888_SkColorType,
           SkColorSpace::MakeSRGBLinear()},
          {VK_FORMAT_B8G8R8A8_SRGB, kBGRA_8888_SkColorType,
           SkColorSpace::MakeSRGBLinear()},
          {VK_FORMAT_R16G16B16A16_SFLOAT, kRGBA_F16_SkColorType,
           SkColorSpace::MakeSRGBLinear()}};
}

std::mutex RSVulkanSwapchain::map_mutex_;
std::unordered_map<std::thread::id, RSVulkanSwapchain*> RSVulkanSwapchain::to_be_present_;

RSVulkanSwapchain::RSVulkanSwapchain(const RSVulkanProcTable& p_vk,
                                 const RSVulkanDevice& device,
                                 const RSVulkanSurface& surface,
                                 GrDirectContext* skia_context,
                                 std::unique_ptr<RSVulkanSwapchain> old_swapchain,
                                 uint32_t queue_family_index)

    : vk(p_vk),
      device_(device),
      capabilities_(),
      surface_format_(),
      current_pipeline_stage_(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT),
      current_backbuffer_index_(0),
      current_image_index_(0),
      valid_(false) {
  if (!device_.IsValid() || !surface.IsValid() || skia_context == nullptr) {
    LOGE("Device or surface is invalid.");
    return;
  }

  if (!device_.GetSurfaceCapabilities(surface, &capabilities_)) {
    LOGE("Could not find surface capabilities.");
    return;
  }

  const auto format_infos = DesiredFormatInfos();
  std::vector<VkFormat> desired_formats(format_infos.size());
  for (size_t i = 0; i < format_infos.size(); ++i) {
    if (skia_context->colorTypeSupportedAsSurface(
            format_infos[i].color_type_)) {
      desired_formats[i] = format_infos[i].format_;
    } else {
      desired_formats[i] = VK_FORMAT_UNDEFINED;
    }
  }

  int format_index =
      device_.ChooseSurfaceFormat(surface, desired_formats, &surface_format_);
  if (format_index < 0) {
    LOGE("Could not choose surface format.");
    return;
  }

  VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
  if (!device_.ChoosePresentMode(surface, &present_mode)) {
    LOGE("Could not choose present mode.");
    return;
  }

  // Check if the surface can present.

  VkBool32 supported = VK_FALSE;
  if (VK_CALL_LOG_ERROR(vk.GetPhysicalDeviceSurfaceSupportKHR(
          device_.GetPhysicalDeviceHandle(),  // physical device
          queue_family_index,                 // queue family
          surface.Handle(),                   // surface to test
          &supported)) != VK_SUCCESS) {
    LOGE("Could not get physical device surface support.");
    return;
  }

  if (supported != VK_TRUE) {
    LOGE("Surface was not supported by the physical device.");
    return;
  }

  // Construct the Swapchain

  VkSwapchainKHR old_swapchain_handle = VK_NULL_HANDLE;

  if (old_swapchain != nullptr && old_swapchain->IsValid()) {
    old_swapchain_handle = old_swapchain->swapchain_;
    // The unique pointer to the swapchain will go out of scope here
    // and its handle collected after the appropriate device wait.
  }

  VkSurfaceKHR surface_handle = surface.Handle();

  const VkSwapchainCreateInfoKHR create_info = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = nullptr,
      .flags = 0,
      .surface = surface_handle,
      .minImageCount = capabilities_.minImageCount,
      .imageFormat = surface_format_.format,
      .imageColorSpace = surface_format_.colorSpace,
      .imageExtent = capabilities_.currentExtent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,  // Because of the exclusive sharing mode.
      .pQueueFamilyIndices = nullptr,
      .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
      .compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
      .presentMode = present_mode,
      .clipped = VK_FALSE,
      .oldSwapchain = old_swapchain_handle,
  };

  VkSwapchainKHR swapchain = VK_NULL_HANDLE;

  if (VK_CALL_LOG_ERROR(vk.CreateSwapchainKHR(device_.GetHandle(), &create_info,
                                              nullptr, &swapchain)) !=
      VK_SUCCESS) {
    LOGE("Could not create the swapchain.");
    return;
  }

  swapchain_ = {swapchain, [this](VkSwapchainKHR swapchain) {
                  device_.WaitIdle();
                  vk.DestroySwapchainKHR(device_.GetHandle(), swapchain, nullptr);
                }};

  if (!CreateSwapchainImages(skia_context,
                             format_infos[format_index].color_type_,
                             format_infos[format_index].color_space_)) {
    LOGE("Could not create swapchain images.");
    return;
  }

  valid_ = true;
}

RSVulkanSwapchain::~RSVulkanSwapchain() = default;

bool RSVulkanSwapchain::IsValid() const {
  return valid_;
}

std::vector<VkImage> RSVulkanSwapchain::GetImages() const {
  uint32_t count = 0;
  if (VK_CALL_LOG_ERROR(vk.GetSwapchainImagesKHR(
          device_.GetHandle(), swapchain_, &count, nullptr)) != VK_SUCCESS) {
    return {};
  }

  if (count == 0) {
    return {};
  }

  std::vector<VkImage> images;

  images.resize(count);

  if (VK_CALL_LOG_ERROR(vk.GetSwapchainImagesKHR(
          device_.GetHandle(), swapchain_, &count, images.data())) !=
      VK_SUCCESS) {
    return {};
  }

  return images;
}

SkISize RSVulkanSwapchain::GetSize() const {
  VkExtent2D extents = capabilities_.currentExtent;

  if (extents.width < capabilities_.minImageExtent.width) {
    extents.width = capabilities_.minImageExtent.width;
  } else if (extents.width > capabilities_.maxImageExtent.width) {
    extents.width = capabilities_.maxImageExtent.width;
  }

  if (extents.height < capabilities_.minImageExtent.height) {
    extents.height = capabilities_.minImageExtent.height;
  } else if (extents.height > capabilities_.maxImageExtent.height) {
    extents.height = capabilities_.maxImageExtent.height;
  }

  return SkISize::Make(extents.width, extents.height);
}

sk_sp<SkSurface> RSVulkanSwapchain::CreateSkiaSurface(
    GrDirectContext* gr_context,
    VkImage image,
    const SkISize& size,
    SkColorType color_type,
    sk_sp<SkColorSpace> color_space) const {
  if (gr_context == nullptr) {
    LOGE("gr_context == nullptr");
    return nullptr;
  }

  if (color_type == kUnknown_SkColorType) {
    // Unexpected Vulkan format.
    LOGE("color_type == kUnknown_SkColorType");
    return nullptr;
  }

  GrVkImageInfo image_info;
  image_info.fImage = image;
  image_info.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
  image_info.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.fFormat = surface_format_.format;
  image_info.fImageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                  VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  image_info.fSampleCount = 1;
  image_info.fLevelCount = 1;

  // TODO(chinmaygarde): Setup the stencil buffer and the sampleCnt.
  GrBackendRenderTarget backend_render_target(size.fWidth, size.fHeight, 0,
                                              image_info);
  SkSurfaceProps props(0, SkPixelGeometry::kUnknown_SkPixelGeometry);

  LOGI("SkSurface::MakeFromBackendRenderTarget");
  return SkSurface::MakeFromBackendRenderTarget(
      gr_context,                // context
      backend_render_target,     // backend render target
      kTopLeft_GrSurfaceOrigin,  // origin
      color_type,                // color type
      std::move(color_space),    // color space
      &props                     // surface properties
  );
}

bool RSVulkanSwapchain::CreateSwapchainImages(GrDirectContext* skia_context,
                                            SkColorType color_type,
                                            sk_sp<SkColorSpace> color_space) {
  std::vector<VkImage> images = GetImages();

  if (images.size() == 0) {
    LOGE("images.size() == 0");
    return false;
  }

  const SkISize surface_size = GetSize();

  for (const VkImage& image : images) {
    // Populate the backbuffer.
    auto backbuffer = std::make_unique<RSVulkanBackbuffer>(
        vk, device_.GetHandle(), device_.GetCommandPool());

    if (!backbuffer->IsValid()) {
      return false;
    }

    backbuffers_.emplace_back(std::move(backbuffer));

    // Populate the image.
    auto vulkan_image = std::make_unique<RSVulkanImage>(image);

    if (!vulkan_image->IsValid()) {
      LOGE("vulkan_image Not Valid()");
      return false;
    }

    images_.emplace_back(std::move(vulkan_image));

    // Populate the surface.
    auto surface = CreateSkiaSurface(skia_context, image,
                                     surface_size, color_type, color_space);

    if (surface == nullptr) {
      LOGE("surface is nullptr");
      return false;
    }

    surfaces_.emplace_back(std::move(surface));
  }

  if (backbuffers_.size() != images_.size()) {
    LOGE("backbuffers_.size() != images_.size()");
  }
  if (images_.size() != surfaces_.size()) {
    LOGE("images_.size() != surfaces_.size()");
  }

  return true;
}

RSVulkanBackbuffer* RSVulkanSwapchain::GetNextBackbuffer() {
  auto available_backbuffers = backbuffers_.size();

  if (available_backbuffers == 0) {
    return nullptr;
  }

  auto next_backbuffer_index =
      (current_backbuffer_index_ + 1) % backbuffers_.size();

  auto& backbuffer = backbuffers_[next_backbuffer_index];

  if (!backbuffer->IsValid()) {
    return nullptr;
  }

  current_backbuffer_index_ = next_backbuffer_index;
  return backbuffer.get();
}

RSVulkanSwapchain::AcquireResult RSVulkanSwapchain::AcquireSurface(int bufferCount) {
  AcquireResult error = {AcquireStatus::ErrorSurfaceLost, nullptr};

  if (!IsValid()) {
    LOGE("Swapchain was invalid.");
    return error;
  }

  // ---------------------------------------------------------------------------
  // Step 0:
  // Acquire the next available backbuffer.
  // ---------------------------------------------------------------------------
  auto backbuffer = GetNextBackbuffer();

  if (backbuffer == nullptr) {
    LOGE("Could not get the next backbuffer.");
    return error;
  }

  // -----------------------------------------------------------------------------
  // when back buffer is used in multi threading mode it need to wait shared fence
  // instead of its private fence
  // -----------------------------------------------------------------------------

  if (!backbuffer->IsMultiThreading()) {
    // ---------------------------------------------------------------------------
    // Step 1:
    // Wait for use readiness.
    // ---------------------------------------------------------------------------
    if (!backbuffer->WaitFences()) {
      LOGE("Failed waiting on fences.");
      return error;
    }

  // ---------------------------------------------------------------------------
  // Step 2:
  // Put semaphores in unsignaled state.
  // ---------------------------------------------------------------------------
    if (!backbuffer->ResetFences()) {
      LOGE("Could not reset fences.");
      return error;
    }
  } // !backbuffer->IsMultiThreading()

  // ---------------------------------------------------------------------------
  // Step 3:
  // Acquire the next image index.
  // ---------------------------------------------------------------------------
  uint32_t next_image_index = 0;

  VkResult acquire_result = VK_CALL_LOG_ERROR(
      vk.AcquireNextImageKHR(device_.GetHandle(),                   //
                             swapchain_,                            //
                             std::numeric_limits<uint64_t>::max(),  //
                             backbuffer->GetUsageSemaphore(),       //
                             VK_NULL_HANDLE,                        //
                             &next_image_index));

  if (bufferCount == 1) {
    next_image_index = 0;
  }

  switch (acquire_result) {
    case VK_SUCCESS:
      break;
    case VK_ERROR_OUT_OF_DATE_KHR:
      return {AcquireStatus::ErrorSurfaceOutOfDate, nullptr};
    case VK_ERROR_SURFACE_LOST_KHR:
      return {AcquireStatus::ErrorSurfaceLost, nullptr};
    default:
      LOGE("Unexpected result from AcquireNextImageKHR: %d", acquire_result);
      return {AcquireStatus::ErrorSurfaceLost, nullptr};
  }

  // Simple sanity checking of image index.
  if (next_image_index >= images_.size()) {
    LOGE("Image index returned was out-of-bounds.");
    return error;
  }

  auto& image = images_[next_image_index];
  if (!image->IsValid()) {
    LOGE("Image at index was invalid.");
    return error;
  }

  // ---------------------------------------------------------------------------
  // Step 4:
  // Start recording to the command buffer.
  // ---------------------------------------------------------------------------
  if (!backbuffer->GetUsageCommandBuffer().Begin()) {
    LOGE("Could not begin recording to the command buffer.");
    return error;
  }

  // ---------------------------------------------------------------------------
  // Step 5:
  // Set image layout to color attachment mode.
  // ---------------------------------------------------------------------------
  VkPipelineStageFlagBits destination_pipeline_stage =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkImageLayout destination_image_layout =
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  if (!image->InsertImageMemoryBarrier(
          backbuffer->GetUsageCommandBuffer(),   // command buffer
          current_pipeline_stage_,               // src_pipeline_bits
          destination_pipeline_stage,            // dest_pipeline_bits
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,  // dest_access_flags
          destination_image_layout               // dest_layout
          )) {
    LOGE("Could not insert image memory barrier.");
    return error;
  } else {
    current_pipeline_stage_ = destination_pipeline_stage;
  }

  // ---------------------------------------------------------------------------
  // Step 6:
  // End recording to the command buffer.
  // ---------------------------------------------------------------------------
  if (!backbuffer->GetUsageCommandBuffer().End()) {
    LOGE("Could not end recording to the command buffer.");
    return error;
  }

  // ---------------------------------------------------------------------------
  // Step 7:
  // Submit the command buffer to the device queue.
  // ---------------------------------------------------------------------------
  std::vector<VkSemaphore> wait_semaphores = {backbuffer->GetUsageSemaphore()};
  std::vector<VkSemaphore> signal_semaphores = {};
  std::vector<VkCommandBuffer> command_buffers = {
      backbuffer->GetUsageCommandBuffer().Handle()};

  if (!device_.QueueSubmit(
          {destination_pipeline_stage},  // wait_dest_pipeline_stages
          wait_semaphores,               // wait_semaphores
          signal_semaphores,             // signal_semaphores
          command_buffers,               // command_buffers
          backbuffer->GetUsageFence()    // fence
          )) {
    LOGE("Could not submit to the device queue.");
    return error;
  }

  // reset to not under multi-threading by default
  // the reality will be judged later in flush stage
  backbuffer->UnsetMultiThreading();
  // ---------------------------------------------------------------------------
  // Step 8:
  // Tell Skia about the updated image layout.
  // ---------------------------------------------------------------------------
  sk_sp<SkSurface> surface = surfaces_[next_image_index];

  if (surface == nullptr) {
    LOGE("Could not access surface at the image index.");
    return error;
  }

  GrBackendRenderTarget backendRT = surface->getBackendRenderTarget(
      SkSurface::kFlushRead_BackendHandleAccess);
  if (!backendRT.isValid()) {
    LOGE("Could not get backend render target.");
    return error;
  }
  backendRT.setVkImageLayout(destination_image_layout);

  current_image_index_ = next_image_index;

  return {AcquireStatus::Success, surface};
}

bool RSVulkanSwapchain::FlushCommands() {
  if (!IsValid()) {
    LOGE("Swapchain was invalid.");
    return false;
  }

  sk_sp<SkSurface> surface = surfaces_[current_image_index_];
  const std::unique_ptr<RSVulkanImage>& image = images_[current_image_index_];
  auto backbuffer = backbuffers_[current_backbuffer_index_].get();

  // ---------------------------------------------------------------------------
  // Step 0:
  // Make sure Skia has flushed all work for the surface to the gpu.
  // ---------------------------------------------------------------------------
  surface->flush();

  // ---------------------------------------------------------------------------
  // Step 1:
  // Start recording to the command buffer.
  // ---------------------------------------------------------------------------
  if (!backbuffer->GetRenderCommandBuffer().Begin()) {
    LOGE("Could not start recording to the command buffer.");
    return false;
  }

  // ---------------------------------------------------------------------------
  // Step 2:
  // Set image layout to present mode.
  // ---------------------------------------------------------------------------
  VkPipelineStageFlagBits destination_pipeline_stage =
      VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  VkImageLayout destination_image_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  if (!image->InsertImageMemoryBarrier(
          backbuffer->GetRenderCommandBuffer(),  // command buffer
          current_pipeline_stage_,               // src_pipeline_bits
          destination_pipeline_stage,            // dest_pipeline_bits
          VK_ACCESS_MEMORY_READ_BIT,             // dest_access_flags
          destination_image_layout               // dest_layout
          )) {
    LOGE("Could not insert memory barrier.");
    return false;
  } else {
    current_pipeline_stage_ = destination_pipeline_stage;
  }

  // ---------------------------------------------------------------------------
  // Step 3:
  // End recording to the command buffer
  // ---------------------------------------------------------------------------
  if (!backbuffer->GetRenderCommandBuffer().End()) {
    LOGE("Could not end recording to the command buffer.");
    return false;
  }
  return true;
}

void RSVulkanSwapchain::AddToPresent() {
  std::lock_guard<std::mutex> lock(map_mutex_);
  to_be_present_[std::this_thread::get_id()] = this;
}

void RSVulkanSwapchain::PresentAll(RSVulkanHandle<VkFence>& shared_fence) {
  if (to_be_present_.empty()) {
    LOGE("nothing to be presented");
    return;
  }

  std::lock_guard<std::mutex> lock(map_mutex_);
  // ---------------------------------------------------------------------------
  // Submit all the command buffer to the device queue. Tell it to signal the render
  // semaphore.
  // ---------------------------------------------------------------------------
  std::vector<VkSemaphore> wait_semaphores = {};
  std::vector<VkSemaphore> queue_signal_semaphores;
  std::vector<VkCommandBuffer> command_buffers;
  std::vector<VkSwapchainKHR> swapchains;
  std::vector<uint32_t> present_image_indices;
  queue_signal_semaphores.reserve(to_be_present_.size());
  command_buffers.reserve(to_be_present_.size());
  swapchains.reserve(to_be_present_.size());
  present_image_indices.reserve(to_be_present_.size());
  RSVulkanSwapchain* tmpSwapChain = nullptr;
  for (const auto& entry : to_be_present_) {
    auto swapchain = entry.second;
    if (!tmpSwapChain) tmpSwapChain = swapchain;
    auto backbuffer = swapchain->backbuffers_[swapchain->current_backbuffer_index_].get();
    backbuffer->SetMultiThreading();
    queue_signal_semaphores.push_back(backbuffer->GetRenderSemaphore());
    command_buffers.push_back(backbuffer->GetRenderCommandBuffer().Handle());
    swapchains.push_back(swapchain->swapchain_);
    present_image_indices.push_back(static_cast<uint32_t>(swapchain->current_image_index_));
  }

  const RSVulkanProcTable& vk = tmpSwapChain->vk;
  const RSVulkanDevice& device = tmpSwapChain->device_;

  if (!device.QueueSubmit(
      {/*Empty, No wait Semaphores. */},
      wait_semaphores,
      queue_signal_semaphores,
      command_buffers,
      shared_fence
      )) {
    LOGE("Could not submit to the device queue");
    return;
  }

  // ----------------------------------------
  //  present multiple swapchain all at once
  // ----------------------------------------
  const VkPresentInfoKHR present_info = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pNext = nullptr,
    .waitSemaphoreCount =
      static_cast<uint32_t> (queue_signal_semaphores.size()),
    .pWaitSemaphores = queue_signal_semaphores.data(),
    .swapchainCount = static_cast<uint32_t>(swapchains.size()),
    .pSwapchains = swapchains.data(),
    .pImageIndices = present_image_indices.data(),
    .pResults = nullptr,
  };

  if (VK_CALL_LOG_ERROR(vk.QueuePresentKHR(device.GetQueueHandle(),
    &present_info)) != VK_SUCCESS) {
    LOGE("Could not submit the present operation");
    return;
  }

  to_be_present_.clear();
}

bool RSVulkanSwapchain::Submit() {
  LOGI("RSVulkanSwapchain::Submit()");
  if (!IsValid()) {
    LOGE("Swapchain was invalid.");
    return false;
  }

  sk_sp<SkSurface> surface = surfaces_[current_image_index_];
  const std::unique_ptr<RSVulkanImage>& image = images_[current_image_index_];
  auto backbuffer = backbuffers_[current_backbuffer_index_].get();

  // ---------------------------------------------------------------------------
  // Step 0:
  // Make sure Skia has flushed all work for the surface to the gpu.
  // ---------------------------------------------------------------------------
  surface->flushAndSubmit(true);

  // ---------------------------------------------------------------------------
  // Step 1:
  // Start recording to the command buffer.
  // ---------------------------------------------------------------------------
  if (!backbuffer->GetRenderCommandBuffer().Begin()) {
    LOGE("Could not start recording to the command buffer.");
    return false;
  }

  // ---------------------------------------------------------------------------
  // Step 2:
  // Set image layout to present mode.
  // ---------------------------------------------------------------------------
  VkPipelineStageFlagBits destination_pipeline_stage =
      VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  VkImageLayout destination_image_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  if (!image->InsertImageMemoryBarrier(
          backbuffer->GetRenderCommandBuffer(),  // command buffer
          current_pipeline_stage_,               // src_pipeline_bits
          destination_pipeline_stage,            // dest_pipeline_bits
          VK_ACCESS_MEMORY_READ_BIT,             // dest_access_flags
          destination_image_layout               // dest_layout
          )) {
    LOGE("Could not insert memory barrier.");
    return false;
  } else {
    current_pipeline_stage_ = destination_pipeline_stage;
  }

  // ---------------------------------------------------------------------------
  // Step 3:
  // End recording to the command buffer.
  // ---------------------------------------------------------------------------
  if (!backbuffer->GetRenderCommandBuffer().End()) {
    LOGE("Could not end recording to the command buffer.");
    return false;
  }

  // ---------------------------------------------------------------------------
  // Step 4:
  // Submit the command buffer to the device queue. Tell it to signal the render
  // semaphore.
  // ---------------------------------------------------------------------------
  std::vector<VkSemaphore> wait_semaphores = {};
  std::vector<VkSemaphore> queue_signal_semaphores = {
      backbuffer->GetRenderSemaphore()};
  std::vector<VkCommandBuffer> command_buffers = {
      backbuffer->GetRenderCommandBuffer().Handle()};

  if (!device_.QueueSubmit(
          {/* Empty. No wait semaphores. */},  // wait_dest_pipeline_stages
          wait_semaphores,                     // wait_semaphores
          queue_signal_semaphores,             // signal_semaphores
          command_buffers,                     // command_buffers
          backbuffer->GetRenderFence()         // fence
          )) {
    LOGE("Could not submit to the device queue.");
    return false;
  }

  backbuffer->UnsetMultiThreading();
  // ---------------------------------------------------------------------------
  // Step 5:
  // Submit the present operation and wait on the render semaphore.
  // ---------------------------------------------------------------------------
  VkSwapchainKHR swapchain = swapchain_;
  uint32_t present_image_index = static_cast<uint32_t>(current_image_index_);
  const VkPresentInfoKHR present_info = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .waitSemaphoreCount =
          static_cast<uint32_t>(queue_signal_semaphores.size()),
      .pWaitSemaphores = queue_signal_semaphores.data(),
      .swapchainCount = 1,
      .pSwapchains = &swapchain,
      .pImageIndices = &present_image_index,
      .pResults = nullptr,
  };

  if (VK_CALL_LOG_ERROR(vk.QueuePresentKHR(device_.GetQueueHandle(),
                                           &present_info)) != VK_SUCCESS) {
    LOGE("Could not submit the present operation.");
    return false;
  }

  return true;
}

}  // namespace OHOS::Rosen::vulkan 
