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

#ifndef RS_VULKAN_CONTEXT_H
#define RS_VULKAN_CONTEXT_H

#include <mutex>
#include "include/gpu/vk/GrVkExtensions.h"
#include "vulkan/vulkan_core.h"

#if !VULKAN_LINK_STATICALLY
#define VK_NO_PROTOTYPES 1
#endif

#include "vulkan/vulkan.h"
#include "include/gpu/vk/GrVkBackendContext.h"
#include "include/gpu/GrDirectContext.h"

namespace OHOS {
namespace Rosen {

class RsVulkanContext {
public:
    template <class T>
    class Proc {
    public:
        using Proto = T;
        explicit Proc(T proc = nullptr) : proc_(proc) {}
        ~Proc() { proc_ = nullptr; }

        Proc operator=(T proc)
        {
            proc_ = proc;
            return *this;
        }

        Proc operator=(PFN_vkVoidFunction proc)
        {
            proc_ = reinterpret_cast<Proto>(proc);
            return *this;
        }

        operator bool() const { return proc_ != nullptr; }
        operator T() const { return proc_; }
    private:
        T proc_;
    };

    static RsVulkanContext& GetSingleton();

    RsVulkanContext();
    ~RsVulkanContext();
    bool CreateInstance();
    bool SelectPhysicalDevice();
    bool CreateDevice();
    bool CreateSkiaBackendContext(GrVkBackendContext* context);

    bool IsValid() const;
    GrVkGetProc CreateSkiaGetProc() const;

#define DEFINE_PROC(name) Proc<PFN_vk##name> vk##name

    DEFINE_PROC(AcquireNextImageKHR);
    DEFINE_PROC(AllocateCommandBuffers);
    DEFINE_PROC(AllocateMemory);
    DEFINE_PROC(BeginCommandBuffer);
    DEFINE_PROC(BindImageMemory);
    DEFINE_PROC(BindImageMemory2);
    DEFINE_PROC(CmdPipelineBarrier);
    DEFINE_PROC(CreateCommandPool);
    DEFINE_PROC(CreateDebugReportCallbackEXT);
    DEFINE_PROC(CreateDevice);
    DEFINE_PROC(CreateFence);
    DEFINE_PROC(CreateImage);
    DEFINE_PROC(CreateImageView);
    DEFINE_PROC(CreateInstance);
    DEFINE_PROC(CreateSemaphore);
    DEFINE_PROC(CreateSwapchainKHR);
    DEFINE_PROC(DestoryCommandPool);
    DEFINE_PROC(DestoryDebugReportCallbackEXT);
    DEFINE_PROC(DestroyDevice);
    DEFINE_PROC(DestroyFence);
    DEFINE_PROC(DestroyImage);
    DEFINE_PROC(DestroyImageView);
    DEFINE_PROC(DestroyInstance);
    DEFINE_PROC(DestroySemaphore);
    DEFINE_PROC(DestroySurfaceKHR);
    DEFINE_PROC(DestroySwapchainKHR);
    DEFINE_PROC(DeviceWaitIdle);
    DEFINE_PROC(EndCommandBuffer);
    DEFINE_PROC(EnumerateDeviceLayerProperties);
    DEFINE_PROC(EnumerateInstanceExtensionProperties);
    DEFINE_PROC(EnumerateInstanceLayerProperties);
    DEFINE_PROC(EnumeratePhysicalDevices);
    DEFINE_PROC(FreeCommandBuffers);
    DEFINE_PROC(FreeMemory);
    DEFINE_PROC(GetDeviceProcAddr);
    DEFINE_PROC(GetDeviceQueue);
    DEFINE_PROC(GetImageMemoryRequirements);
    DEFINE_PROC(GetInstanceProcAddr);
    DEFINE_PROC(GetPhysicalDeviceFeatures);
    DEFINE_PROC(GetPhysicalDeviceQueueFamilyProperties);
    DEFINE_PROC(QueueSubmit);
    DEFINE_PROC(QueueWaitIdle);
    DEFINE_PROC(ResetCommandBuffer);
    DEFINE_PROC(ResetFences);
    DEFINE_PROC(WaitForFences);
    DEFINE_PROC(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    DEFINE_PROC(GetPhysicalDeviceSurfaceFormatsKHR);
    DEFINE_PROC(GetPhysicalDeviceSurfacePresentModesKHR);
    DEFINE_PROC(GetPhysicalDeviceSurfaceSupportKHR);
    DEFINE_PROC(GetSwapchainImagesKHR);
    DEFINE_PROC(QueuePresentKHR);
    DEFINE_PROC(CreateSurfaceOHOS);
    DEFINE_PROC(GetPhysicalDeviceMemoryProperties);
    DEFINE_PROC(GetPhysicalDeviceMemoryProperties2);
    DEFINE_PROC(GetNativeBufferPropertiesOHOS);
    DEFINE_PROC(QueueSignalReleaseImageOHOS);
    DEFINE_PROC(ImportSemaphoreFdKHR);
    DEFINE_PROC(GetPhysicalDeviceFeatures2);
#undef DEFINE_PROC

    VkPhysicalDevice GetPhysicalDevice() const
    {
        return physicalDevice_;
    }

    vkDevice GetDevice() const
    {
        return device_;
    }

    vkQueue GetQueue() const
    {
        return queue_;
    }

    inline const GrVkBackendContext& GetGrVkBackendContext() const noexcept
    {
        return backendContext_;
    }

    sk_sp<GrDirectContext> CreateSkContext();
    static thread_local sk_sp<GrDirectContext> skContext_;
    sk_sp<GrDirectContext> GetSkContext();

    static VKAPI_ATTR VkResult HookedVkQueueSubmit(VkQueue queue, uint32_t submitCount,
        const VkSubmitInfo* pSubmits, VkFence fence);

    static VKAPI_ATTR VkResult HookedVkQueueSignalReleaseImageOHOS(VkQueue queue, uint32_t waitSemaphoreCount,
        const VkSemaphore* pWaitSemaphores, VkImage image, int32_t* pNativeFenceFd);

private:
    std::mutex vkMutex_;
    std::mutex graphicsQueueMutex_;
    void* handle_;
    bool acquiredMandatoryProcAddresses_;
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamilyIndex_ = UINT32_MAX;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue queue_ = VK_NULL_HANDLE;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2_;
    VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeature_;
    GrVkExtensions skVkExtensions_;
    GrVkBackendContext backendContext_;

    RsVulkanContext(const RsVulkanContext &) = delete;
    RsVulkanContext &operator=(const RsVulkanContext &) = delete;

    RsVulkanContext(RsVulkanContext &&) = delete;
    RsVulkanContext &operator=(RsVulkanContext &&) = delete;

    bool OpenLibraryHandle();
    bool SetupLoaderProcAddresses();
    bool CloseLibraryHandle();
    bool SetupDeviceProcAddresses();
    bool GetGraphicsQueueFamilyIndex();

    PFN_vkVoidFunction AcquireProc(
        const char* proc_name,
        const VkInstance& instance) const;
    PFN_vkVoidFunction AcquireProc(const char* proc_name, const VkDevice& device) const;
};

} // namespace Rosen
} // namespace OHOS

#endif