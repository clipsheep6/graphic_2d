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

#ifndef FLUTTER_VULKAN_VULKAN_IMAGE_H_
#define FLUTTER_VULKAN_VULKAN_IMAGE_H_

#include "vulkan_handle.h"

namespace vulkan {

class VulkanProcTable;
class VulkanCommandBuffer;

class VulkanImage {
 public:
  VulkanImage(VulkanHandle<VkImage> image);

  ~VulkanImage();

  bool IsValid() const;

  bool InsertImageMemoryBarrier(const VulkanCommandBuffer& command_buffer,
                                VkPipelineStageFlagBits src_pipline_bits,
                                VkPipelineStageFlagBits dest_pipline_bits,
                                VkAccessFlagBits dest_access_flags,
                                VkImageLayout dest_layout);

 private:
  VulkanHandle<VkImage> handle_;
  VkImageLayout layout_;
  uint32_t /* mask of VkAccessFlagBits */ access_flags_;
  bool valid_;
};

}  // namespace vulkan

#endif  // FLUTTER_VULKAN_VULKAN_IMAGE_H_
