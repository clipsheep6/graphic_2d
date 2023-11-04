/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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


#ifndef NATIVE_BUFFER_UTILS
#define NATIVE_BUFFER_UTILS

#include <atomic>

#include "rs_vulkan_context.h"

namespace OHOS::Rosen {
namespace NativeBufferUtils {
void delete_vk_image(void* context);
class VulkanCleanupHelper {
public:
    VulkanCleanupHelper(RsVulkanContext& vkContext, VkImage image, VkDeviceMemory memory)
        : fDevice(vkContext.GetDevice()), fImage(image), fMemory(memory), fDestroyImage(vkContext.vkDestroyImage),
        fFreeMemory(vkContext.vkFreeMemory), fRefCnt(1) {}
    ~VulkanCleanupHelper()
    {
        fDestroyImage(fDevice, fImage, nullptr);
        fFreeMemory(fDevice, fMemory, nullptr);
    }

    VulkanCleanupHelper* Ref()
    {
        (void)fRefCnt.fetch_add(+1, std::memory_order_relaxed);
        return this;
    }

    void UnRef()
    {
        if (fRefCnt.fetch_add(-1, std::memory_order_acq_rel) == 1) {
            delete this;
        }
    }

private:
    VkDevice           fDevice;
    VkImage            fImage;
    VkDeviceMemory     fMemory;
    PFN_vkDestroyImage fDestroyImage;
    PFN_vkFreeMemory   fFreeMemory;
    mutable std::atomic<int32_t> fRefCnt;
};
}
} // OHOS::Rosen
#endif