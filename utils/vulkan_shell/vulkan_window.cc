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

#include "vulkan_window.h"

#include <memory>
#include <string>

#include "vulkan_application.h"
#include "vulkan_device.h"
#include "vulkan_hilog.h"
#include "vulkan_native_surface.h"
#include "vulkan_surface.h"
#include "vulkan_swapchain.h"
#include "third_party/skia/include/gpu/GrDirectContext.h"

namespace OHOS::Rosen::vulkan {

RSVulkanProcTable* RSVulkanWindow::vk;
std::unique_ptr<RSVulkanApplication> RSVulkanWindow::application_;
std::unique_ptr<RSVulkanDevice> RSVulkanWindow::logical_device_;
std::thread::id RSVulkanWindow::device_thread_;
std::vector<RSVulkanHandle<VkFence>> RSVulkanWindow::shared_fences_;
uint32_t RSVulkanWindow::shared_fence_index_;
bool RSVulkanWindow::presenting_ = false;

void RSVulkanWindow::InitializeVulkan(size_t thread_num)
{
  if (shared_fences_.size() < thread_num) {
    shared_fences_.resize(thread_num);
    shared_fence_index_ = 0;
  }

  if (logical_device_ != nullptr) {
    LOGI("Vulkan Already Initialized");
    return;
  }

  LOGI("First Initialize Vulkan");
  device_thread_ = std::this_thread::get_id();

  vk = new RSVulkanProcTable();
  if (!vk->HasAcquiredMandatoryProcAddresses()) {
    LOGE("Proc table has not acquired mandatory proc addresses.");
    return;
  }

  // Create the application instance.
  std::vector<std::string> extensions = {
      VK_KHR_SURFACE_EXTENSION_NAME,               // parent extension
      VK_OHOS_SURFACE_EXTENSION_NAME               // child extension
  };

  application_ = std::make_unique<RSVulkanApplication>(*vk, "Rosen", std::move(extensions));
  if (!application_->IsValid() || !vk->AreInstanceProcsSetup()) {
    // Make certain the application instance was created and it setup the
    // instance proc table entries.
    LOGE("Instance proc addresses have not been setup.");
    return;
  }

  // Create the device.
  logical_device_ = application_->AcquireFirstCompatibleLogicalDevice();
  if (logical_device_ == nullptr || !logical_device_->IsValid() || !vk->AreDeviceProcsSetup()) {
    // Make certain the device was created and it setup the device proc table
    // entries.
    LOGE("Device proc addresses have not been setup.");
    return;
  }
}

RSVulkanWindow::RSVulkanWindow(std::unique_ptr<RSVulkanNativeSurface> native_surface, bool is_offscreen)
    : valid_(false), is_offscreen_(is_offscreen)
{
  LOGE("VulkanWindow init enter");

  InitializeVulkan();
  if (logical_device_ == nullptr) {
    LOGE("InitializeVulkan failed");
    return;
  }

  if (!is_offscreen && (native_surface == nullptr || !native_surface->IsValid())) {
    LOGE("Native surface is invalid.");
    return;
  }

  // Create the logical surface from the native platform surface.
  if (!is_offscreen) {
    surface_ = std::make_unique<RSVulkanSurface>(*vk, *application_, std::move(native_surface));
    if (!surface_->IsValid()) {
      LOGE("Vulkan surface is invalid.");
      return;
    }
  }

  // Create the Skia GrContext.
  if (!CreateSkiaGrContext()) {
    LOGE("Could not create Skia context.");
    return;
  }

  // Create the swapchain.
  if (!is_offscreen && !RecreateSwapchain()) {
    LOGE("Could not setup the swapchain initially.");
    return;
  }
  LOGE("VulkanWindow init success");
  valid_ = true;
}

RSVulkanWindow::~RSVulkanWindow() = default;

bool RSVulkanWindow::IsValid() const {
  return valid_;
}

GrDirectContext* RSVulkanWindow::GetSkiaGrContext() {
  return skia_gr_context_.get();
}

GrVkBackendContext& RSVulkanWindow::GetSkiaBackendContext() {
  return sk_backend_context_;
}


bool RSVulkanWindow::CreateSkiaGrContext() {

  if (!CreateSkiaBackendContext(&sk_backend_context_)) {
    LOGE("CreateSkiaGrContext CreateSkiaBackendContext is false");
    return false;
  }

  sk_sp<GrDirectContext> context = GrDirectContext::MakeVulkan(sk_backend_context_);

  if (context == nullptr) {
    LOGE("CreateSkiaGrContext context is null");
    return false;
  }

  context->setResourceCacheLimits(kGrCacheMaxCount, kGrCacheMaxByteSize);

  skia_gr_context_ = context;

  return true;
}

bool RSVulkanWindow::CreateSkiaBackendContext(GrVkBackendContext* context) {
  auto getProc = vk->CreateSkiaGetProc();

  if (getProc == nullptr) {
    LOGE("CreateSkiaBackendContext getProc is null");
    return false;
  }

  uint32_t skia_features = 0;
  if (!logical_device_->GetPhysicalDeviceFeaturesSkia(&skia_features)) {
    LOGE("CreateSkiaBackendContext GetPhysicalDeviceFeaturesSkia is false");
    return false;
  }

  context->fInstance = application_->GetInstance();
  context->fPhysicalDevice = logical_device_->GetPhysicalDeviceHandle();
  context->fDevice = logical_device_->GetHandle();
  context->fQueue = logical_device_->GetQueueHandle();
  context->fGraphicsQueueIndex = logical_device_->GetGraphicsQueueIndex();
  context->fMinAPIVersion = application_->GetAPIVersion();
  uint32_t extensionFlags = kKHR_surface_GrVkExtensionFlag;
  if (!is_offscreen_) {
    extensionFlags |= kKHR_swapchain_GrVkExtensionFlag;
    extensionFlags |= surface_->GetNativeSurface().GetSkiaExtensionName();
  }
  context->fExtensions = extensionFlags;

  context->fFeatures = skia_features;
  context->fGetProc = std::move(getProc);
  context->fOwnsInstanceAndDevice = false;
  return true;
}

sk_sp<SkSurface> RSVulkanWindow::AcquireSurface(int bufferCount) {
  if (is_offscreen_ || !IsValid()) {
    LOGE("Surface is invalid or offscreen.");
    return nullptr;
  }

  auto surface_size = surface_->GetSize();

  // This check is theoretically unnecessary as the swapchain should report that
  // the surface is out-of-date and perform swapchain recreation at the new
  // configuration. However, on Android, the swapchain never reports that it is
  // of date. Hence this extra check. Platforms that don't have this issue, or,
  // cant report this information (which is optional anyway), report a zero
  // size.
  if (surface_size != SkISize::Make(0, 0) &&
      surface_size != swapchain_->GetSize()) {
    LOGE("Swapchain and surface sizes are out of sync. Recreating swapchain.");
    if (!RecreateSwapchain()) {
      LOGE("Could not recreate swapchain.");
      valid_ = false;
      return nullptr;
    }
  }

  while (true) {
    sk_sp<SkSurface> surface;
    auto acquire_result = RSVulkanSwapchain::AcquireStatus::ErrorSurfaceLost;

    std::tie(acquire_result, surface) = swapchain_->AcquireSurface(bufferCount);

    if (acquire_result == RSVulkanSwapchain::AcquireStatus::Success) {
      // Successfully acquired a surface from the swapchain. Nothing more to do.
      return surface;
    }

    if (acquire_result == RSVulkanSwapchain::AcquireStatus::ErrorSurfaceLost) {
      // Surface is lost. This is an unrecoverable error.
      LOGE("Swapchain reported surface was lost.");
      return nullptr;
    }

    if (acquire_result ==
        RSVulkanSwapchain::AcquireStatus::ErrorSurfaceOutOfDate) {
      LOGE("AcquireSurface surface out of date");
      // Surface out of date. Recreate the swapchain at the new configuration.
      if (RecreateSwapchain()) {
        // Swapchain was recreated, try surface acquisition again.
        continue;
      } else {
        // Could not recreate the swapchain at the new configuration.
        LOGE("Swapchain reported surface was out of date but "
                           "could not recreate the swapchain at the new "
                           "configuration.");
        valid_ = false;
        return nullptr;
      }
    }

    break;
  }

  LOGE("Unhandled VulkanSwapchain::AcquireResult");
  return nullptr;
}

bool RSVulkanWindow::SwapBuffers() {
  if (is_offscreen_ || !IsValid()) {
      LOGE("Window was invalid or offscreen.");
      return false;
  }
  if (device_thread_ != std::this_thread::get_id()) {
    LOGI("MT mode in VulkanWindow::SwapBuffers()");
    swapchain_->AddToPresent();
    return swapchain_->FlushCommands();
  }
  LOGI("ST mode in VulkanWindow::SwapBuffers()");
  return swapchain_->Submit();
}

void RSVulkanWindow::PresentAll() {
  //-----------------------------------------
  // create shared fences if not already
  //-----------------------------------------
  if (!shared_fences_[shared_fence_index_]) {
    const VkFenceCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    auto fence_collect = [](VkFence fence) {
      RSVulkanWindow::vk->DestroyFence(RSVulkanWindow::logical_device_->GetHandle(), fence, nullptr);
    };

    VkFence fence = VK_NULL_HANDLE;

    if (VK_CALL_LOG_ERROR(vk->CreateFence(logical_device_->GetHandle(), &create_info, nullptr, &fence)) != VK_SUCCESS) {
      return;
    }
    shared_fences_[shared_fence_index_] = {fence, fence_collect};
  }
  RSVulkanSwapchain::PresentAll(shared_fences_[shared_fence_index_]);
  shared_fence_index_++;
  if (shared_fence_index_ >= shared_fences_.size()) {
    shared_fence_index_ = 0;
  }
  presenting_ = true;
}

bool RSVulkanWindow::WaitForSharedFence() {
  if (presenting_) {
    if (shared_fences_[shared_fence_index_]) {
      VkFence fence = shared_fences_[shared_fence_index_];
      return VK_CALL_LOG_ERROR(vk->WaitForFences(
        logical_device_->GetHandle(), 1, &fence, true,
        std::numeric_limits<uint64_t>::max())) == VK_SUCCESS;
    }
  }
  return false;
}

bool RSVulkanWindow::ResetSharedFence() {
  if (presenting_) {
      presenting_ = false;
      if (shared_fences_[shared_fence_index_]) {
        VkFence fence = shared_fences_[shared_fence_index_];
        return VK_CALL_LOG_ERROR(vk->ResetFences(
            logical_device_->GetHandle(), 1, &fence)) == VK_SUCCESS;
      }
  }
  return false;
}

VkDevice RSVulkanWindow::GetDevice() {
  return logical_device_->GetHandle();
}

VkPhysicalDevice RSVulkanWindow::GetPhysicalDevice() {
  return logical_device_->GetPhysicalDeviceHandle();
}

RSVulkanProcTable &RSVulkanWindow::GetVkProcTable() {
  return *vk;
}

bool RSVulkanWindow::RecreateSwapchain() {
  if (is_offscreen_) {
      LOGE("offscreen vulkan window, don't need swapchian");
      return false;
  }
  // This way, we always lose our reference to the old swapchain. Even if we
  // cannot create a new one to replace it.
  auto old_swapchain = std::move(swapchain_);

  if (!vk->IsValid()) {
    LOGE("RecreateSwapchain vk not valid");
    return false;
  }

  if (logical_device_ == nullptr || !logical_device_->IsValid()) {
    LOGE("RecreateSwapchain logical_device_ not valid");
    return false;
  }

  if (surface_ == nullptr || !surface_->IsValid()) {
    LOGE("RecreateSwapchain surface_ not valid");
    return false;
  }

  if (skia_gr_context_ == nullptr) {
    LOGE("RecreateSwapchain skia_gr_context_ not valid");
    return false;
  }

  auto swapchain = std::make_unique<RSVulkanSwapchain>(
      *vk, *logical_device_, *surface_, skia_gr_context_.get(),
      std::move(old_swapchain), logical_device_->GetGraphicsQueueIndex());

  if (!swapchain->IsValid()) {
    LOGE("RecreateSwapchain swapchain not valid");
    return false;
  }

  swapchain_ = std::move(swapchain);
  return true;
}

}  // namespace OHOS::Rosen::vulkan 
