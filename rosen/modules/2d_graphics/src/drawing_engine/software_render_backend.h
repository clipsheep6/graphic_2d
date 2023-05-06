/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef SOFTWARE_RENDER_BACKEND_H
#define SOFTWARE_RENDER_BACKEND_H
#include "interface_render_backend.h"

namespace OHOS {
namespace Rosen {
class SoftwareRenderBackend : public IRenderBackend {
public:
    SoftwareRenderBackend() {}
    ~SoftwareRenderBackend() override {}
    void Initialize() override {};
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height) override {};
    void DamageFrame(const std::vector<RectI> &rects) override {};
    int32_t GetBufferAge() override { return 0; }
    void Destroy() override {};
    void RenderFrame() override {};
    void SwapBuffers() override {};
    sk_sp<SkSurface> AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame = nullptr) override;
private:
    sk_sp<SkSurface> skSurface_ = nullptr;
};
}
}
#endif