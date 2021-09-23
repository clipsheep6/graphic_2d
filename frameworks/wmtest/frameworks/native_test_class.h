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

#ifndef FRAMEWORKS_WM_SRC_TEST_DRAW_NATIVE_TEST_H
#define FRAMEWORKS_WM_SRC_TEST_DRAW_NATIVE_TEST_H

#include <cstdint>
#include <functional>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <refbase.h>
#include <window_manager.h>
#include <egl_surface.h>

namespace OHOS {
typedef struct {
    GLuint program;
    GLuint pos;
    GLuint color;
    GLuint offsetUniform;
} GlContext;

class NativeTestFactory {
public:
    static sptr<Window> CreateWindow(WindowType, sptr<Surface> csurface = nullptr);
};

using DrawFunc = std::function<void(void *, uint32_t, uint32_t, uint32_t)>;
using DrawFuncEgl = std::function<void(GlContext *, sptr<EglRenderSurface> &psurface)>;

class NativeTestSync : public RefBase {
public:
    static sptr<NativeTestSync> CreateSync(DrawFunc drawFunc, sptr<Surface> &psurface, void *data = nullptr);
    static sptr<NativeTestSync> CreateSyncEgl(DrawFuncEgl drawFunc,
        sptr<EglRenderSurface> &psurface, void *data = nullptr);

private:
    void Sync(int64_t, void *);
    void SyncEgl(int64_t, void *);
    bool GLContextInit();

    sptr<Surface> surface = nullptr;
    sptr<EglRenderSurface> eglsurface = nullptr;
    DrawFunc draw = nullptr;
    DrawFuncEgl drawEgl = nullptr;
    uint32_t count = 0;
    GlContext glCtx;
    bool bInit = false;
    SurfaceError sret = SURFACE_ERROR_OK;
};

class NativeTestDraw {
public:
    static void FlushDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void FlushDrawEgl(GlContext *ctx, sptr<EglRenderSurface> &eglsurface);
    static void ColorDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void BlackDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void RainbowDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void BoxDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_SRC_TEST_DRAW_NATIVE_TEST_H
