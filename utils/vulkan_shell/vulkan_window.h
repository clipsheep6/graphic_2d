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

#ifndef RS_VULKAN_VULKAN_WINDOW_H_
#define RS_VULKAN_VULKAN_WINDOW_H_

#include <memory>
#include <tuple>
#include <utility>
#include <vector>
#include <thread>

#include <native_window.h>

#include "vulkan_proc_table.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/GrDirectContext.h"
#include "third_party/skia/include/gpu/vk/GrVkBackendContext.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanNativeSurface;
class RSVulkanDevice;
class RSVulkanSurface;
class RSVulkanSwapchain;
class RSVulkanImage;
class RSVulkanApplication;
class RSVulkanBackbuffer;

class RSVulkanWindow {
 public:
  typedef std::shared_ptr<RSVulkanWindow> Ptr;
  RSVulkanWindow(std::unique_ptr<RSVulkanNativeSurface> native_surface,
               bool is_offscreen = false);

  ~RSVulkanWindow();

  bool IsValid() const;

  GrDirectContext* GetSkiaGrContext();
  GrVkBackendContext &GetSkiaBackendContext();

  sk_sp<SkSurface> AcquireSurface(int bufferCount = -1);

  bool SwapBuffers();

  bool FlushCommands();
  static void PresentAll();
  static void InitializeVulkan(size_t thread_num = 0);
  static bool WaitForSharedFence();
  static bool ResetSharedFence();
  static VkDevice GetDevice();
  static VkPhysicalDevice GetPhysicalDevice();
  static RSVulkanProcTable &GetVkProcTable();

  static std::unique_ptr<RSVulkanDevice> logical_device_;
  static RSVulkanProcTable* vk;

 public:
  bool valid_;
  static std::unique_ptr<RSVulkanApplication> application_;
  bool is_offscreen_ = false;
  static std::thread::id device_thread_;
  static std::vector<RSVulkanHandle<VkFence>> shared_fences_;
  static uint32_t shared_fence_index_;
  static bool presenting_;
  std::unique_ptr<RSVulkanSurface> surface_;
  std::unique_ptr<RSVulkanSwapchain> swapchain_;
  sk_sp<GrDirectContext> skia_gr_context_;

  GrVkBackendContext sk_backend_context_;

  bool CreateSkiaGrContext();

  bool CreateSkiaBackendContext(GrVkBackendContext* context);

  bool RecreateSwapchain();
};

}  // namespace OHOS::Rosen::vulkan 

#endif  // RS_VULKAN_VULKAN_WINDOW_H_
