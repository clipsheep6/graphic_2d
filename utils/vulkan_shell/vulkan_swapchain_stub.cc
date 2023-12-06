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

namespace vulkan {

VulkanSwapchain::VulkanSwapchain(const VulkanProcTable& p_vk,
                                 const VulkanDevice& device,
                                 const VulkanSurface& surface,
                                 GrDirectContext* skia_context,
                                 std::unique_ptr<VulkanSwapchain> old_swapchain,
                                 uint32_t queue_family_index) {}

VulkanSwapchain::~VulkanSwapchain() = default;

bool VulkanSwapchain::IsValid() const {
  return false;
}

VulkanSwapchain::AcquireResult VulkanSwapchain::AcquireSurface() {
  return {AcquireStatus::ErrorSurfaceLost, nullptr};
}

bool VulkanSwapchain::Submit() {
  return false;
}

SkISize VulkanSwapchain::GetSize() const {
  return SkISize::Make(0, 0);
}

}  // namespace vulkan
