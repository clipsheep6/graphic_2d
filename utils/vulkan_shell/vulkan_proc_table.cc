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

#include "vulkan_proc_table.h"

#include <dlfcn.h>

#include "vulkan_hilog.h"

#define ACQUIRE_PROC(name, context)                   \
    if (!(name = AcquireProc("vk" #name, context))) { \
        LOGE("Could not acquire proc: vk" #name);     \
    }

namespace OHOS::Rosen::vulkan {

RSVulkanProcTable::RSVulkanProcTable() : handle_(nullptr), acquiredMandatoryProcAddresses_(false)
{
    acquiredMandatoryProcAddresses_ = OpenLibraryHandle() && SetupLoaderProcAddresses();
}

RSVulkanProcTable::~RSVulkanProcTable()
{
    CloseLibraryHandle();
}

bool RSVulkanProcTable::HasAcquiredMandatoryProcAddresses() const
{
    return acquiredMandatoryProcAddresses_;
}

bool RSVulkanProcTable::IsValid() const
{
    return instance_ && device_;
}

bool RSVulkanProcTable::AreInstanceProcsSetup() const
{
    return instance_;
}

bool RSVulkanProcTable::AreDeviceProcsSetup() const
{
    return device_;
}

bool RSVulkanProcTable::SetupLoaderProcAddresses()
{
    if (handle_ == nullptr) {
        return true;
    }

    GetInstanceProcAddr =
#if VULKAN_LINK_STATICALLY
        GetInstanceProcAddr = &vkGetInstanceProcAddr;
#else  // VULKAN_LINK_STATICALLY
        reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(handle_, "vkGetInstanceProcAddr"));
    GetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(dlsym(handle_, "vkGetDeviceProcAddr"));
    EnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
        dlsym(handle_, "vkEnumerateInstanceExtensionProperties"));
    CreateInstance = reinterpret_cast<PFN_vkCreateInstance>(dlsym(handle_, "vkCreateInstance"));
#endif // VULKAN_LINK_STATICALLY

    if (!GetInstanceProcAddr) {
        LOGE("Could not acquire vkGetInstanceProcAddr.");
        return false;
    }

    RSVulkanHandle<VkInstance> null_instance(VK_NULL_HANDLE, nullptr);

    ACQUIRE_PROC(EnumerateInstanceLayerProperties, null_instance);

    return true;
}

bool RSVulkanProcTable::SetupInstanceProcAddresses(const RSVulkanHandle<VkInstance>& handle)
{
    ACQUIRE_PROC(CreateDevice, handle);
    ACQUIRE_PROC(DestroyDevice, handle);
    ACQUIRE_PROC(DestroyInstance, handle);
    ACQUIRE_PROC(EnumerateDeviceLayerProperties, handle);
    ACQUIRE_PROC(EnumeratePhysicalDevices, handle);
    ACQUIRE_PROC(GetPhysicalDeviceFeatures, handle);
    ACQUIRE_PROC(GetPhysicalDeviceQueueFamilyProperties, handle);
    ACQUIRE_PROC(GetPhysicalDeviceSurfaceCapabilitiesKHR, handle);
    ACQUIRE_PROC(GetPhysicalDeviceSurfaceFormatsKHR, handle);
    ACQUIRE_PROC(GetPhysicalDeviceSurfacePresentModesKHR, handle);
    ACQUIRE_PROC(GetPhysicalDeviceSurfaceSupportKHR, handle);
    ACQUIRE_PROC(DestroySurfaceKHR, handle);
    ACQUIRE_PROC(CreateSurfaceOHOS, handle);
    ACQUIRE_PROC(GetPhysicalDeviceMemoryProperties, handle);
    ACQUIRE_PROC(GetPhysicalDeviceMemoryProperties2, handle);

    // The debug report functions are optional. We don't want proc acquisition to
    // fail here because the optional methods were not present (since ACQUIRE_PROC
    // returns false on failure). Wrap the optional proc acquisitions in an
    // anonymous lambda and invoke it. We don't really care about the result since
    // users of Debug reporting functions check for their presence explicitly.

    instance_ = { handle, nullptr };
    return true;
}

bool RSVulkanProcTable::SetupDeviceProcAddresses(const RSVulkanHandle<VkDevice>& handle)
{
    ACQUIRE_PROC(AllocateCommandBuffers, handle);
    ACQUIRE_PROC(AllocateMemory, handle);
    ACQUIRE_PROC(BeginCommandBuffer, handle);
    ACQUIRE_PROC(BindImageMemory, handle);
    ACQUIRE_PROC(BindImageMemory2, handle);
    ACQUIRE_PROC(CmdPipelineBarrier, handle);
    ACQUIRE_PROC(CreateCommandPool, handle);
    ACQUIRE_PROC(CreateFence, handle);
    ACQUIRE_PROC(CreateImage, handle);
    ACQUIRE_PROC(CreateImageView, handle);
    ACQUIRE_PROC(CreateSemaphore, handle);
    ACQUIRE_PROC(DestroyCommandPool, handle);
    ACQUIRE_PROC(DestroyFence, handle);
    ACQUIRE_PROC(DestroyImage, handle);
    ACQUIRE_PROC(DestroySemaphore, handle);
    ACQUIRE_PROC(DeviceWaitIdle, handle);
    ACQUIRE_PROC(EndCommandBuffer, handle);
    ACQUIRE_PROC(FreeCommandBuffers, handle);
    ACQUIRE_PROC(FreeMemory, handle);
    ACQUIRE_PROC(GetDeviceQueue, handle);
    ACQUIRE_PROC(GetImageMemoryRequirements, handle);
    ACQUIRE_PROC(QueueSubmit, handle);
    ACQUIRE_PROC(QueueWaitIdle, handle);
    ACQUIRE_PROC(ResetCommandBuffer, handle);
    ACQUIRE_PROC(ResetFences, handle);
    ACQUIRE_PROC(WaitForFences, handle);
    ACQUIRE_PROC(AcquireNextImageKHR, handle);
    ACQUIRE_PROC(CreateSwapchainKHR, handle);
    ACQUIRE_PROC(DestroySwapchainKHR, handle);
    ACQUIRE_PROC(GetSwapchainImagesKHR, handle);
    ACQUIRE_PROC(QueuePresentKHR, handle);
    ACQUIRE_PROC(GetNativeBufferPropertiesOHOS, handle);
    ACQUIRE_PROC(QueueSignalReleaseImageOHOS, handle);

    device_ = { handle, nullptr };
    return true;
}

bool RSVulkanProcTable::OpenLibraryHandle()
{
    LOGI("VulkanProcTable OpenLibararyHandle.");
#if VULKAN_LINK_STATICALLY
    static char kDummyLibraryHandle = '\0';
    handle_ = reinterpret_cast<decltype(handle_)>(&kDummyLibraryHandle);
    return true;
#else  // VULKAN_LINK_STATICALLY
    LOGI("VulkanProcTable OpenLibararyHandle: dlopen libvulkan.so.");
    dlerror(); // clear existing errors on thread.
    handle_ = dlopen("/system/lib64/libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (handle_ == nullptr) {
        LOGE("Could not open the vulkan library: %s", dlerror());
        return false;
    }
    return true;
#endif // VULKAN_LINK_STATICALLY
}

bool RSVulkanProcTable::CloseLibraryHandle()
{
#if VULKAN_LINK_STATICALLY
    handle_ = nullptr;
    return true;
#else
    if (handle_ != nullptr) {
        dlerror(); // clear existing errors on thread.
        if (dlclose(handle_) != 0) {
            LOGE("Could not close the vulkan library handle. This "
                 "indicates a leak.");
            LOGE("%s", dlerror());
        }
        handle_ = nullptr;
    }
    return handle_ == nullptr;
#endif
}

PFN_vkVoidFunction RSVulkanProcTable::AcquireProc(
    const char* proc_name, const RSVulkanHandle<VkInstance>& instance) const
{
    if (proc_name == nullptr || !GetInstanceProcAddr) {
        return nullptr;
    }

    // A VK_NULL_HANDLE as the instance is an acceptable parameter.
    return GetInstanceProcAddr(instance, proc_name);
}

PFN_vkVoidFunction RSVulkanProcTable::AcquireProc(const char* proc_name, const RSVulkanHandle<VkDevice>& device) const
{
    if (proc_name == nullptr || !device || !GetDeviceProcAddr) {
        return nullptr;
    }

    return GetDeviceProcAddr(device, proc_name);
}

GrVkGetProc RSVulkanProcTable::CreateSkiaGetProc() const
{
    if (!IsValid()) {
        return nullptr;
    }

    return [this](const char* proc_name, VkInstance instance, VkDevice device) {
        if (device != VK_NULL_HANDLE) {
            auto result = AcquireProc(proc_name, { device, nullptr });
            if (result != nullptr) {
                return result;
            }
        }

        return AcquireProc(proc_name, { instance, nullptr });
    };
}

} // namespace OHOS::Rosen::vulkan
