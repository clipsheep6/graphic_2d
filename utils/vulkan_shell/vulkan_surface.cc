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

#include "vulkan_surface.h"

#include "vulkan_application.h"
#include "vulkan_hilog.h"
#include "vulkan_native_surface.h"

namespace vulkan {

VulkanSurface::VulkanSurface(
    VulkanProcTable& p_vk,
    VulkanApplication& application,
    std::unique_ptr<VulkanNativeSurface> native_surface)
    : vk(p_vk),
      application_(application),
      native_surface_(std::move(native_surface)),
      valid_(false) {
  if (native_surface_ == nullptr || !native_surface_->IsValid()) {
    LOGE("Native surface was invalid.");
    return;
  }

  VkSurfaceKHR surface =
      native_surface_->CreateSurfaceHandle(vk, application.GetInstance());

  if (surface == VK_NULL_HANDLE) {
    LOGE("Could not create the surface handle.");
    return;
  }

  surface_ = {surface, [this](VkSurfaceKHR surface) {
                vk.DestroySurfaceKHR(application_.GetInstance(), surface,
                                     nullptr);
              }};

  valid_ = true;
}

VulkanSurface::~VulkanSurface() = default;

bool VulkanSurface::IsValid() const {
  return valid_;
}

const VulkanHandle<VkSurfaceKHR>& VulkanSurface::Handle() const {
  return surface_;
}

const VulkanNativeSurface& VulkanSurface::GetNativeSurface() const {
  return *native_surface_;
}

SkISize VulkanSurface::GetSize() const {
  return valid_ ? native_surface_->GetSize() : SkISize::Make(0, 0);
}

}  // namespace vulkan
