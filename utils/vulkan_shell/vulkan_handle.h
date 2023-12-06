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

#ifndef FLUTTER_VULKAN_VULKAN_HANDLE_H_
#define FLUTTER_VULKAN_VULKAN_HANDLE_H_

#include <functional>

#include "vulkan_interface.h"

namespace vulkan {

template <class T>
class VulkanHandle {
 public:
  using Handle = T;
  using Disposer = std::function<void(Handle)>;

  VulkanHandle() : handle_(VK_NULL_HANDLE) {}

  VulkanHandle(Handle handle, Disposer disposer = nullptr)
      : handle_(handle), disposer_(disposer) {}

  VulkanHandle(VulkanHandle&& other)
      : handle_(other.handle_), disposer_(other.disposer_) {
    other.handle_ = VK_NULL_HANDLE;
    other.disposer_ = nullptr;
  }

  ~VulkanHandle() { DisposeIfNecessary(); }

  VulkanHandle& operator=(VulkanHandle&& other) {
    if (handle_ != other.handle_) {
      DisposeIfNecessary();
    }

    handle_ = other.handle_;
    disposer_ = other.disposer_;

    other.handle_ = VK_NULL_HANDLE;
    other.disposer_ = nullptr;

    return *this;
  }

  operator bool() const { return handle_ != VK_NULL_HANDLE; }

  operator Handle() const { return handle_; }

  /// Relinquish responsibility of collecting the underlying handle when this
  /// object is collected. It is the responsibility of the caller to ensure that
  /// the lifetime of the handle extends past the lifetime of this object.
  void ReleaseOwnership() { disposer_ = nullptr; }

  void Reset() { DisposeIfNecessary(); }

 private:
  Handle handle_;
  Disposer disposer_;

  void DisposeIfNecessary() {
    if (handle_ == VK_NULL_HANDLE) {
      return;
    }
    if (disposer_) {
      disposer_(handle_);
    }
    handle_ = VK_NULL_HANDLE;
    disposer_ = nullptr;
  }
};

}  // namespace vulkan

#endif  // FLUTTER_VULKAN_VULKAN_HANDLE_H_
