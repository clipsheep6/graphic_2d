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

#ifndef RS_VULKAN_VULKAN_SWAPCHAIN_H_
#define RS_VULKAN_VULKAN_SWAPCHAIN_H_

#include <memory>
#include <utility>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "vulkan_handle.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanProcTable;
class RSVulkanDevice;
class RSVulkanSurface;
class RSVulkanBackbuffer;
class RSVulkanImage;

class RSVulkanSwapchain {
 public:
  RSVulkanSwapchain(const RSVulkanProcTable& vk,
                  const RSVulkanDevice& device,
                  const RSVulkanSurface& surface,
                  GrDirectContext* skia_context,
                  std::unique_ptr<RSVulkanSwapchain> old_swapchain,
                  uint32_t queue_family_index);

  ~RSVulkanSwapchain();

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

  static void PresentAll(RSVulkanHandle<VkFence>& shared_fence);

 private:
  const RSVulkanProcTable& vk;
  const RSVulkanDevice& device_;
  VkSurfaceCapabilitiesKHR capabilities_;
  VkSurfaceFormatKHR surface_format_;
  RSVulkanHandle<VkSwapchainKHR> swapchain_;
  std::vector<std::unique_ptr<RSVulkanBackbuffer>> backbuffers_;
  std::vector<std::unique_ptr<RSVulkanImage>> images_;
  std::vector<sk_sp<SkSurface>> surfaces_;
  VkPipelineStageFlagBits current_pipeline_stage_;
  size_t current_backbuffer_index_;
  size_t current_image_index_;
  bool valid_;
  static std::mutex map_mutex_;
  static std::unordered_map<std::thread::id, RSVulkanSwapchain*> to_be_present_;

  std::vector<VkImage> GetImages() const;

  bool CreateSwapchainImages(GrDirectContext* skia_context,
                             SkColorType color_type,
                             sk_sp<SkColorSpace> color_space);

  sk_sp<SkSurface> CreateSkiaSurface(GrDirectContext* skia_context,
                                     VkImage image,
                                     const SkISize& size,
                                     SkColorType color_type,
                                     sk_sp<SkColorSpace> color_space) const;

  RSVulkanBackbuffer* GetNextBackbuffer();

};

}  // namespace OHOS::Rosen::vulkan 

#endif  // RS_VULKAN_VULKAN_SWAPCHAIN_H_
