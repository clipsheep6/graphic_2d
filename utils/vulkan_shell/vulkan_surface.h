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

#ifndef FLUTTER_VULKAN_VULKAN_SURFACE_H_
#define FLUTTER_VULKAN_VULKAN_SURFACE_H_

#include "vulkan_handle.h"
#include "third_party/skia/include/core/SkSize.h"

namespace vulkan {

class VulkanProcTable;
class VulkanApplication;
class VulkanNativeSurface;

class VulkanSurface {
 public:
  VulkanSurface(VulkanProcTable& vk,
                VulkanApplication& application,
                std::unique_ptr<VulkanNativeSurface> native_surface);

  ~VulkanSurface();

  bool IsValid() const;

  /// Returns the current size of the surface or (0, 0) if invalid.
  SkISize GetSize() const;

  const VulkanHandle<VkSurfaceKHR>& Handle() const;

  const VulkanNativeSurface& GetNativeSurface() const;

 private:
  VulkanProcTable& vk;
  VulkanApplication& application_;
  std::unique_ptr<VulkanNativeSurface> native_surface_;
  VulkanHandle<VkSurfaceKHR> surface_;
  bool valid_;
};

}  // namespace vulkan

#endif  // FLUTTER_VULKAN_VULKAN_SURFACE_H_
