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

#ifndef RS_VULKAN_VULKAN_PROC_TABLE_H_
#define RS_VULKAN_VULKAN_PROC_TABLE_H_

#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/gpu/vk/GrVkBackendContext.h"
#include "vulkan_handle.h"
#include "vulkan_interface.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanProcTable {
public:
    template<class T>
    class Proc {
    public:
        using Proto = T;

        Proc(T proc = nullptr) : proc_(proc) {}

        ~Proc()
        {
            proc_ = nullptr;
        }

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

        operator bool() const
        {
            return proc_ != nullptr;
        }

        operator T() const
        {
            return proc_;
        }

    private:
        T proc_;
    };

    RSVulkanProcTable();

    ~RSVulkanProcTable();

    bool HasAcquiredMandatoryProcAddresses() const;

    bool IsValid() const;

    bool AreInstanceProcsSetup() const;

    bool AreDeviceProcsSetup() const;

    bool SetupInstanceProcAddresses(const RSVulkanHandle<VkInstance>& instance);

    bool SetupDeviceProcAddresses(const RSVulkanHandle<VkDevice>& device);

    GrVkGetProc CreateSkiaGetProc() const;

#define DEFINE_PROC(name) Proc<PFN_vk##name> name

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
    DEFINE_PROC(DestroyCommandPool);
    DEFINE_PROC(DestroyDebugReportCallbackEXT);
    DEFINE_PROC(DestroyDevice);
    DEFINE_PROC(DestroyFence);
    DEFINE_PROC(DestroyImage);
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

#undef DEFINE_PROC

private:
    void* handle_;
    bool acquiredMandatoryProcAddresses_;
    RSVulkanHandle<VkInstance> instance_;
    RSVulkanHandle<VkDevice> device_;

    bool OpenLibraryHandle();
    bool SetupLoaderProcAddresses();
    bool CloseLibraryHandle();
    PFN_vkVoidFunction AcquireProc(const char* procName, const RSVulkanHandle<VkInstance>& instance) const;
    PFN_vkVoidFunction AcquireProc(const char* procName, const RSVulkanHandle<VkDevice>& device) const;
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_PROC_TABLE_H_
