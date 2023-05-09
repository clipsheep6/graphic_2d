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

#ifndef RENDER_SERVICE_BASE_RENDER_SURFACE_OHOS_VK_H
#define RENDER_SERVICE_BASE_RENDER_SURFACE_OHOS_VK_H

#include "render_surface_ohos.h"

#include <vulkan_window.h>

namespace OHOS {
namespace Rosen {
class RenderSurfaceOhosVk : public RenderSurfaceOhos {
public:
    explicit RenderSurfaceOhosVk(const sptr<Surface>& producer) : RenderSurfaceOhos(producer)
    {
    }

    ~RenderSurfaceOhosVk();

    std::shared_ptr<RenderSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp = 0, bool useAFBC = true) override;
    
    bool FlushFrame(uint64_t uiTimestamp = 0) override;

    void SetUiTimeStamp(uint64_t uiTimestamp = 0) override;

    void ClearBuffer() override;

    sk_sp<SkSurface> AcquireSurface() override;

private:
    struct NativeWindow* nativeWindow_ = nullptr;
    std::shared_ptr<vulkan::VulkanWindow> vulkanWindow_ = nullptr;
};
}
} // namespace Rosen
#endif