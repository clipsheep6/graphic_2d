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

#ifndef GLES_RENDER_BACKEND_H
#define GLES_RENDER_BACKEND_H
#include "interface_render_backend.h"
#include "egl_manager.h"

namespace OHOS {
namespace Rosen {
class GLESRenderBackend : public IRenderBackend {
public:
    GLESRenderBackend() noexcept;
    virtual ~GLESRenderBackend();
    void Initialize() override;
    void MakeCurrent() override;
    void SwapBuffers() override;
    void SetSurface(const sptr<Surface> surface, int width, int height) override;
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override;
private:
    EGLManager* eglManager_ = nullptr;
};
}
}
#endif