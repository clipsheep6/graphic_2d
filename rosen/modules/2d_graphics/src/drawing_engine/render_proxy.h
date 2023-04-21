/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and	 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef Render_PROXY_H
#define Render_PROXY_H
#include "rs_render_context.h"


namespace OHOS {
namespace Rosen {
class RenderProxy {
public:
    RenderProxy();
    virtual ~RenderProxy();
    sk_sp<SkSurface> GetSkSurface(const std::unique_ptr<RSSurfaceFrame>& frame);
    SkCanvas* GetCanvas(const std::unique_ptr<RSSurfaceFrame>& frame);
    void SetUpSurface(void* window);
    void MakeCurrent();
    void InitRSRenderContext();
    void SwapBuffers();
    void Destroy();
    void DestorySurface();
    void RenderFrame();
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height);
    void DamageFrame(const std::vector<RectI> &rects);
    void SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp);
    int32_t GetBufferAge();
    GrContext* GetGrContext();
    bool SetUpGrContext();
    void SetCacheDir(const std::string& filePath);
    void ClearRedundantResources();
    EGLContext GetEGLContext() const;
    EGLDisplay GetEGLDisplay() const;
    std::string CleanAllShaderCache() const;
    void SetUniRenderMode(bool isUni);
private:
    std::unique_ptr<RSRenderContext> renderContext_;
};
}
} // namespace Rosen
#endif