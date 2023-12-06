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

namespace OHOS::Rosen::vulkan {

RSVulkanSwapchain::RSVulkanSwapchain(const RSVulkanProcTable& p_vk,
                                 const RSVulkanDevice& device,
                                 const RSVulkanSurface& surface,
                                 GrDirectContext* skia_context,
                                 std::unique_ptr<RSVulkanSwapchain> old_swapchain,
                                 uint32_t queue_family_index) {}

RSVulkanSwapchain::~RSVulkanSwapchain() = default;

bool RSVulkanSwapchain::IsValid() const {
  return false;
}

RSVulkanSwapchain::AcquireResult RSVulkanSwapchain::AcquireSurface() {
  return {AcquireStatus::ErrorSurfaceLost, nullptr};
}

bool RSVulkanSwapchain::Submit() {
  return false;
}

SkISize RSVulkanSwapchain::GetSize() const {
  return SkISize::Make(0, 0);
}

}  // namespace OHOS::Rosen::vulkan 
