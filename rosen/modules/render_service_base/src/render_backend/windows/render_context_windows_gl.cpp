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

#include "windows/render_context_windows_gl.h"

#include <glfw_render_context.h>

namespace OHOS {
namespace Rosen {
RenderContextWindowsGl::~RenderContextWindowsGl()
{
}

RenderContextWindowsGl::Init()
{
}

bool RenderContextWindowsGl::SetUpGrContext()
{
    if (grContext_ != nullptr) {
        return true;
    }

    MakeCurrent();
    sk_sp<const GrGLInterface> glinterface{GrGLCreateNativeInterface()};
    if (glinterface == nullptr) {
        ROSEN_LOGE("glinterface is nullptr");
        return false;
    }

    GrContextOptions options;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
#if !defined(NEW_SKIA)
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    const auto &grContext = GrContext::MakeGL(glinterface, options);
#else
    const auto &grContext = GrDirectContext::MakeGL(glinterface, options);
#endif
    if (grContext == nullptr) {
        ROSEN_LOGE("grContext is nullptr");
        return false;
    }

    grContext_ = grContext;
    return true;
}

void RenderContextWindowsGl::MakeCurrent(void* curSurface = nullptr, void* curContext = nullptr)
{
    GlfwRenderContext::GetGlobal()->MakeCurrent();
}

void RenderContextWindowsGl::SwapBuffers()
{
#ifdef USE_GLFW_WINDOW
    GlfwRenderContext::GetGlobal()->SwapBuffers();
#endif
}

}
}
#endif