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

#ifndef RS_SURFACE_OHOS_VULKAN_H
#define RS_SURFACE_OHOS_VULKAN_H

// default enable native buffer


#ifndef ENABLE_NATIVEBUFFER
#include <vulkan_window.h>
#else // ENABLE_NATIVEBUFFER
#include <list>
#include <unordered_map>
#include "native_window.h"
#include "vulkan/vulkan_core.h"
#include "rs_vulkan_context.h"
#include "sync_fence.h"
#endif // ENABLE_NATIVEBUFFER

#include <surface.h>
#include "platform/ohos/rs_surface_ohos.h"
#include "rs_surface_frame_ohos_vulkan.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceOhosVulkan : public RSSurfaceOhos {
public:
    explicit RSSurfaceOhosVulkan(const sptr<Surface>& producer);
    ~RSSurfaceOhosVulkan() override;

    bool IsValid() const override
    {
        return producer_ != nullptr;
    }

    std::unique_ptr<RSSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC = true) override;
    bool FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp) override;
    void SetSurfaceBufferUsage(uint64_t usage) override;
    void SetSurfacePixelFormat(int32_t pixelFormat) override;
    void ClearBuffer() override;
    void ResetBufferAge() override;
    void SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp) override;
#ifdef ENABLE_NATIVEBUFFER
    void SetSkContext(sk_sp<GrDirectContext> skContext)
    {
        mSkContext = skContext;
    }
#endif
private:
    struct NativeWindow* mNativeWindow = nullptr;
    int mWidth = -1;
    int mHeight = -1;
#ifdef ENABLE_NATIVEBUFFER
    uint32_t mPresentCount = 0;
    std::list<NativeWindowBuffer*> mSurfaceList;
    std::unordered_map<NativeWindowBuffer*, NativeBufferUtils::NativeSurfaceInfo> mSurfaceMap;
    sk_sp<GrDirectContext> mSkContext = nullptr;
#else // ENABLE_NATIVEBUFFER
    vulkan::VulkanWindow* mVulkanWindow = nullptr;
#endif  // ENABLE_NATIVEBUFFER
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_SURFACE_OHOS_VULKAN_H
