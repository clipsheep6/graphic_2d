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

#ifndef RS_RENDER_CONTEXT_H
#define RS_RENDER_CONTEXT_H
#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/gl/GrGLInterface.h"

#include "software_render_backend.h"
#include "surface.h"
#include "gles_render_backend.h"
#include <mutex>

namespace OHOS {
namespace Rosen {
class RSRenderContext {
public:
    RSRenderContext() = delete;
    explicit RSRenderContext(std::shared_ptr<IRenderBackend> renderBackend);
    static std::unique_ptr<RSRenderContext> Create();
    virtual ~RSRenderContext();
    void RenderFrame();
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height);
    void DamageFrame(const std::vector<RectI> &rects);
    void SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp);
    int32_t GetBufferAge();
    sk_sp<SkSurface> GetSkSurface(const std::unique_ptr<RSSurfaceFrame>& frame);
    SkCanvas* GetCanvas(const std::unique_ptr<RSSurfaceFrame>& frame);
    void Init();
    void MakeCurrent();
    void SetUpSurface(void* window);
    void DestorySurface();
    void SwapBuffers();
    GrContext* GetGrContext();
    bool SetUpGrContext();
    void SetCacheDir(const std::string& filePath);
    void ClearRedundantResources();
    EGLContext GetEGLContext() const;
    EGLDisplay GetEGLDisplay() const;
    std::string CleanAllShaderCache() const;
    void SetUniRenderMode(bool isUni);
private:
    std::shared_ptr<IRenderBackend> renderBackend_;
    sk_sp<SkSurface> skSurface_ = nullptr;
};
}
}
#endif