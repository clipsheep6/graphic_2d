// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vulkan_window.h"

#include <memory>
#include <string>

#include "vulkan_application.h"
#include "vulkan_device.h"
#include "vulkan_native_surface.h"
#include "vulkan_shell_hilog.h"
#include "vulkan_surface.h"
#include "vulkan_swapchain.h"

namespace OHOS {
namespace Rosen {
namespace vulkan {

VulkanProcTable* VulkanWindow::vk;
std::unique_ptr<VulkanApplication> VulkanWindow::application_;
std::unique_ptr<VulkanDevice> VulkanWindow::logical_device_;

void VulkanWindow::InitializeVulkan()
{
    if (logical_device_ != nullptr) {
        LOGI("Vulkan Already Initialized");
        return;
    }
    LOGI("First Initialize Vulkan");
    vk = new VulkanProcTable();
    if (!vk->HasAcquiredMandatoryProcAddresses()) {
        LOGE("Proc table has not acquired mandatory proc addresses.");
        return;
    }

    // Create the application instance.
    std::vector<std::string> extensions = {
        VK_KHR_SURFACE_SPEC_VERSION,               // parent extension
        VK_OPENHARMONY_OHOS_SURFACE_EXTENSION_NAME // child extension
    };

    application_ = std::make_unique<VulkanApplication>(*vk, "Rosen", std::move(extensions));
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

VulkanWindow::VulkanWindow(std::unique_ptr<VulkanNativeSurface> native_surface, bool is_offscreen)
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
        surface_ = std::make_unique<VulkanSurface>(*vk, *application_, std::move(native_surface));
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

VulkanWindow::~VulkanWindow() = default;

bool VulkanWindow::IsValid() const
{
    return valid_;
}

GrContext* VulkanWindow::GetSkiaGrContext()
{
    return skia_gr_context_.get();
}

bool VulkanWindow::CreateSkiaGrContext()
{
    GrVkBackendContext backend_context;

    if (!CreateSkiaBackendContext(&backend_context)) {
        LOGE("CreateSkiaGrContext CreateSkiaBackendContext is false");
        return false;
    }

    sk_sp<GrContext> context = GrContext::MakeVulkan(backend_context);

    if (context == nullptr) {
        LOGE("CreateSkiaGrContext context is null");
        return false;
    }

    context->setResourceCacheLimits(kGrCacheMaxCount, kGrCacheMaxByteSize);

    skia_gr_context_ = context;

    return true;
}

bool VulkanWindow::CreateSkiaBackendContext(GrVkBackendContext* context)
{
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

sk_sp<SkSurface> VulkanWindow::AcquireSurface()
{
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
    if (surface_size != SkISize::Make(0, 0) && surface_size != swapchain_->GetSize()) {
        LOGE("Swapchain and surface sizes are out of sync. Recreating swapchain.");
        if (!RecreateSwapchain()) {
            LOGE("Could not recreate swapchain.");
            valid_ = false;
            return nullptr;
        }
    }

    while (true) {
        sk_sp<SkSurface> surface;
        auto acquire_result = VulkanSwapchain::AcquireStatus::ErrorSurfaceLost;

        std::tie(acquire_result, surface) = swapchain_->AcquireSurface();

        if (acquire_result == VulkanSwapchain::AcquireStatus::Success) {
            // Successfully acquired a surface from the swapchain. Nothing more to do.
            return surface;
        }

        if (acquire_result == VulkanSwapchain::AcquireStatus::ErrorSurfaceLost) {
            // Surface is lost. This is an unrecoverable error.
            LOGE("Swapchain reported surface was lost.");
            return nullptr;
        }

        if (acquire_result == VulkanSwapchain::AcquireStatus::ErrorSurfaceOutOfDate) {
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

bool VulkanWindow::SwapBuffers()
{
    if (is_offscreen_ || !IsValid()) {
        LOGE("Window was invalid or offscreen.");
        return false;
    }

    return swapchain_->Submit();
}

bool VulkanWindow::RecreateSwapchain()
{
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

    auto swapchain = std::make_unique<VulkanSwapchain>(*vk, *logical_device_, *surface_, skia_gr_context_.get(),
        std::move(old_swapchain), logical_device_->GetGraphicsQueueIndex());
    if (!swapchain->IsValid()) {
        LOGE("RecreateSwapchain swapchain not valid");
        return false;
    }

    swapchain_ = std::move(swapchain);
    return true;
}

} // namespace vulkan
} // namespace Rosen
} // namespace OHOS