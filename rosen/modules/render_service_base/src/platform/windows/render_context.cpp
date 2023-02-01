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

#include "render_context.h"

namespace OHOS {
namespace Rosen {
RenderContext::RenderContext()
{
    (void)nativeWindow_;
    (void)config_;
    (void)colorSpace_;
}

RenderContext::~RenderContext()
{
}

void RenderContext::CreateCanvas(int width, int height)
{
}

sk_sp<SkSurface> RenderContext::AcquireSurface(int width, int height)
{
    return {};
}

void RenderContext::SetColorSpace(ColorGamut colorSpace)
{
}

void RenderContext::InitializeEglContext()
{
}

bool RenderContext::SetUpGrContext()
{
    return {};
}

EGLSurface RenderContext::CreateEGLSurface(EGLNativeWindowType eglNativeWindow)
{
    return {};
}

void RenderContext::DestroyEGLSurface(EGLSurface surface)
{
}

void RenderContext::MakeCurrent(EGLSurface surface, EGLContext context) const
{
}

void RenderContext::SwapBuffers(EGLSurface surface) const
{
}

void RenderContext::RenderFrame()
{
}

EGLint RenderContext::QueryEglBufferAge()
{
    return {};
}

void RenderContext::DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height)
{
}

void RenderContext::DamageFrame(const std::vector<RectI> &rects)
{
}

void RenderContext::ClearRedundantResources()
{
}
} // namespace Rosen
} // namespace OHOS
