// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_VULKAN_VULKAN_HANDLE_H_
#define FLUTTER_VULKAN_VULKAN_HANDLE_H_

#include <functional>

#include "vulkan_interface.h"

namespace OHOS {
namespace Rosen {
namespace vulkan {

template<class T>
class VulkanHandle {
public:
    using Handle = T;
    using Disposer = std::function<void(Handle)>;

    VulkanHandle() : handle_(VK_NULL_HANDLE) {}

    VulkanHandle(Handle handle, Disposer disposer = nullptr) : handle_(handle), disposer_(disposer) {}

    VulkanHandle(VulkanHandle&& other) : handle_(other.handle_), disposer_(other.disposer_)
    {
        other.handle_ = VK_NULL_HANDLE;
        other.disposer_ = nullptr;
    }

    ~VulkanHandle()
    {
        DisposeIfNecessary();
    }

    VulkanHandle& operator=(VulkanHandle&& other)
    {
        if (handle_ != other.handle_) {
            DisposeIfNecessary();
        }

        handle_ = other.handle_;
        disposer_ = other.disposer_;

        other.handle_ = VK_NULL_HANDLE;
        other.disposer_ = nullptr;

        return *this;
    }

    operator bool() const
    {
        return handle_ != VK_NULL_HANDLE;
    }

    operator Handle() const
    {
        return handle_;
    }

    /// Relinquish responsibility of collecting the underlying handle when this
    /// object is collected. It is the responsibility of the caller to ensure that
    /// the lifetime of the handle extends past the lifetime of this object.
    void ReleaseOwnership()
    {
        disposer_ = nullptr;
    }

    void Reset()
    {
        DisposeIfNecessary();
    }

private:
    Handle handle_;
    Disposer disposer_;

    void DisposeIfNecessary()
    {
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

} // namespace vulkan
} // namespace Rosen
} // namespace OHOS

#endif // FLUTTER_VULKAN_VULKAN_HANDLE_H_
