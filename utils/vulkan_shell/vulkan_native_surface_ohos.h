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

#ifndef FLUTTER_VULKAN_VULKAN_NATIVE_SURFACE_OHOS_H_
#define FLUTTER_VULKAN_VULKAN_NATIVE_SURFACE_OHOS_H_

#include "vulkan_native_surface.h"
#include "window.h"

namespace vulkan {

class VulkanNativeSurfaceOHOS : public VulkanNativeSurface {
 public:
  /// Create a native surface from the valid NativeWindow reference. Ownership
  /// of the NativeWindow is assumed by this instance.
  VulkanNativeSurfaceOHOS(struct NativeWindow* native_window);

  ~VulkanNativeSurfaceOHOS();

  const char* GetExtensionName() const override;

  uint32_t GetSkiaExtensionName() const override;

  VkSurfaceKHR CreateSurfaceHandle(
      VulkanProcTable& vk,
      const VulkanHandle<VkInstance>& instance) const override;

  bool IsValid() const override;

  SkISize GetSize() const override;

 private:
  struct NativeWindow* native_window_;
};

}  // namespace vulkan

#endif  // FLUTTER_VULKAN_VULKAN_NATIVE_SURFACE_OHOS_H_
