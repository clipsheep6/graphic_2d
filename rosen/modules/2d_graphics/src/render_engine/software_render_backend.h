/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
    virtual ~SoftwareRenderBackend() {}
    void Initialize() override;
    void MakeCurrent() override;
    void SwapBuffers() override;
    void SetSurface(const sptr<Surface> surface, int width, int height) override;
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override;
private:
    // BufferRequestConfig requestConfig_ = {
    //     .width = 0x100,
    //     .height = 0x100,
    //     .strideAlignment = 0x8,
    //     .format = PIXEL_FMT_RGBA_8888,
    //     .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    //     .timeout = 0,
    // };
    // BufferFlushConfig flushConfig_ = {
    //     .damage = {
    //         .x = 0,
    //         .y = 0,
    //         .w = 0x100,
    //         .h = 0x100,
    //     },
    // };
};
}
}
#endif