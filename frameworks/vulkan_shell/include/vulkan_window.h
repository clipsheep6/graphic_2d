// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_VULKAN_VULKAN_WINDOW_H_
#define FLUTTER_VULKAN_VULKAN_WINDOW_H_

#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "include/core/SkRefCnt.h"
#include "include/core/SkSize.h"
#include "include/core/SkSurface.h"
#include "vulkan_proc_table.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/vk/GrVkBackendContext.h"

namespace OHOS {
namespace Rosen {
namespace vulkan {

class VulkanNativeSurface;
class VulkanDevice;
class VulkanSurface;
class VulkanSwapchain;
class VulkanImage;
class VulkanApplication;
class VulkanBackbuffer;

class VulkanWindow {
public:
    VulkanWindow(std::unique_ptr<VulkanNativeSurface> native_surface, bool is_offscreen = false);

    ~VulkanWindow();

    bool IsValid() const;

    GrContext* GetSkiaGrContext();

    sk_sp<SkSurface> AcquireSurface();

    bool SwapBuffers();

private:
    bool valid_;
    static VulkanProcTable* vk;
    static std::unique_ptr<VulkanApplication> application_;
    static std::unique_ptr<VulkanDevice> logical_device_;
    std::unique_ptr<VulkanSurface> surface_;
    std::unique_ptr<VulkanSwapchain> swapchain_;
    sk_sp<GrContext> skia_gr_context_;

    bool is_offscreen_ = false;

    static void InitializeVulkan();

    bool CreateSkiaGrContext();

    bool CreateSkiaBackendContext(GrVkBackendContext* context);

    bool RecreateSwapchain();
};

} // namespace vulkan
} // namespace Rosen
} // namespace OHOS
#endif // FLUTTER_VULKAN_VULKAN_WINDOW_H_
