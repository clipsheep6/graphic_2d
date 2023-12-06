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

#include "vulkan_native_surface_ohos.h"

#include "vulkan_hilog.h"
#include "window.h"
#include "third_party/skia/include/gpu/vk/GrVkBackendContext.h"
#include <graphic_common.h>

namespace vulkan {

VulkanNativeSurfaceOHOS::VulkanNativeSurfaceOHOS(struct NativeWindow* native_window) : native_window_(native_window)
{
    if (native_window_ == nullptr) {
        return;
    }
}

VulkanNativeSurfaceOHOS::~VulkanNativeSurfaceOHOS()
{
    if (native_window_ == nullptr) {
        return;
    }
}

const char* VulkanNativeSurfaceOHOS::GetExtensionName() const
{
    return VK_OHOS_SURFACE_EXTENSION_NAME;
}

uint32_t VulkanNativeSurfaceOHOS::GetSkiaExtensionName() const
{
    return kKHR_ohos_surface_GrVkExtensionFlag;
}

VkSurfaceKHR VulkanNativeSurfaceOHOS::CreateSurfaceHandle(
    VulkanProcTable& vk, const VulkanHandle<VkInstance>& instance) const
{
    if (!vk.IsValid() || !instance) {
        LOGE("CreateSurfaceHandle vk or instance is not valid");
        return VK_NULL_HANDLE;
    }

    const VkSurfaceCreateInfoOHOS create_info = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_CREATE_INFO_OHOS,
        .pNext = nullptr,
        .flags = 0,
        .window = native_window_,
    };

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    if (VK_CALL_LOG_ERROR(vk.CreateSurfaceOHOS(instance, &create_info, nullptr, &surface)) != VK_SUCCESS) {
        LOGE("CreateSurfaceHandle CreateOHOSSurfaceKHR not success");
        return VK_NULL_HANDLE;
    }

    return surface;
}

bool VulkanNativeSurfaceOHOS::IsValid() const
{
    return native_window_ != nullptr;
}

SkISize VulkanNativeSurfaceOHOS::GetSize() const
{
    int width, height;
    int err = NativeWindowHandleOpt(native_window_, GET_BUFFER_GEOMETRY, &height, &width);
    return native_window_ == nullptr || err != OHOS::GSERROR_OK ? SkISize::Make(0, 0) : SkISize::Make(width, height);
}

} // namespace vulkan