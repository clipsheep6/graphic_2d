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

#ifndef FLUTTER_VULKAN_VULKAN_SWAPCHAIN_H_
#define FLUTTER_VULKAN_VULKAN_SWAPCHAIN_H_

#include <memory>
#include <utility>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "vulkan_handle.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace vulkan {

class VulkanProcTable;
class VulkanDevice;
class VulkanSurface;
class VulkanBackbuffer;
class VulkanImage;

class VulkanSwapchain {
 public:
  VulkanSwapchain(const VulkanProcTable& vk,
                  const VulkanDevice& device,
                  const VulkanSurface& surface,
                  GrDirectContext* skia_context,
                  std::unique_ptr<VulkanSwapchain> old_swapchain,
                  uint32_t queue_family_index);

  ~VulkanSwapchain();

  bool IsValid() const;

  enum class AcquireStatus {
    /// A valid SkSurface was acquired successfully from the swapchain.
    Success,
    /// The underlying surface of the swapchain was permanently lost. This is an
    /// unrecoverable error. The entire surface must be recreated along with the
    /// swapchain.
    ErrorSurfaceLost,
    /// The swapchain surface is out-of-date with the underlying surface. The
    /// swapchain must be recreated.
    ErrorSurfaceOutOfDate,
  };

  using AcquireResult = std::pair<AcquireStatus, sk_sp<SkSurface>>;

  /// Acquire an SkSurface from the swapchain for the caller to render into for
  /// later submission via |Submit|. There must not be consecutive calls to
  /// |AcquireFrame| without and interleaving |Submit|.
  AcquireResult AcquireSurface(int bufferCount = -1);

  /// Submit a previously acquired. There must not be consecutive calls to
  /// |Submit| without and interleaving |AcquireFrame|.
  bool Submit();

  SkISize GetSize() const;

  bool FlushCommands();

  void AddToPresent();

  static void PresentAll(VulkanHandle<VkFence>& shared_fence);

 private:
  const VulkanProcTable& vk;
  const VulkanDevice& device_;
  VkSurfaceCapabilitiesKHR capabilities_;
  VkSurfaceFormatKHR surface_format_;
  VulkanHandle<VkSwapchainKHR> swapchain_;
  std::vector<std::unique_ptr<VulkanBackbuffer>> backbuffers_;
  std::vector<std::unique_ptr<VulkanImage>> images_;
  std::vector<sk_sp<SkSurface>> surfaces_;
  VkPipelineStageFlagBits current_pipeline_stage_;
  size_t current_backbuffer_index_;
  size_t current_image_index_;
  bool valid_;
  static std::mutex map_mutex_;
  static std::unordered_map<std::thread::id, VulkanSwapchain*> to_be_present_;

  std::vector<VkImage> GetImages() const;

  bool CreateSwapchainImages(GrDirectContext* skia_context,
                             SkColorType color_type,
                             sk_sp<SkColorSpace> color_space);

  sk_sp<SkSurface> CreateSkiaSurface(GrDirectContext* skia_context,
                                     VkImage image,
                                     const SkISize& size,
                                     SkColorType color_type,
                                     sk_sp<SkColorSpace> color_space) const;

  VulkanBackbuffer* GetNextBackbuffer();

};

}  // namespace vulkan

#endif  // FLUTTER_VULKAN_VULKAN_SWAPCHAIN_H_
