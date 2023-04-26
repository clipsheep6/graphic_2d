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

#ifndef SWAPCHAIN_TYPE_H
#define SWAPCHAIN_TYPE_H

#include <vulkan/vulkan.h>
#include <window.h>

namespace OHOS::SWAPCHAIN {
namespace {
constexpr uint32_t MIN_BUFFER_SIZE = 3;
constexpr uint32_t MAX_BUFFER_SIZE = 32;
}  // anonymous namespace

struct Surface {
    NativeWindow* window;
    VkSwapchainKHR swapchainHandle;
    int32_t consumerUsage;
};

struct Swapchain {
    Swapchain(Surface& surface, uint32_t numImages, VkPresentModeKHR presentMode, int preTransform)
        : surface(surface), numImages(numImages), mailboxMode(presentMode == VK_PRESENT_MODE_MAILBOX_KHR),
          preTransform(preTransform), frameTimestampsEnabled(false),
          shared(presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
                 presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {}

    Surface& surface;
    uint32_t numImages;
    bool mailboxMode;
    int preTransform;
    bool frameTimestampsEnabled;
    bool shared;

    struct Image {
        Image() : image(VK_NULL_HANDLE), buffer(nullptr), requestFence(-1), releaseFence(-1), requested(false) {}
        VkImage image;
        NativeWindowBuffer* buffer;
        int requestFence;
        int releaseFence;
        bool requested;
    } images[MAX_BUFFER_SIZE];
};
} // namespace OHOS::SWAPCHAIN
#endif // SWAPCHAIN_TYPE_H
