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

#include "render_proxy.h"

#include "drawing_utils.h"
#include <EGL/egl.h>

namespace OHOS {
namespace Rosen {
RenderProxy::RenderProxy()
{
    LOGD("Create a Render Proxy!");
    renderContext_ = RSRenderContext::Create();
}

RenderProxy::~RenderProxy()
{
    LOGD("RenderProxy::~RenderProxy");
    renderContext_ = nullptr;
}

void RenderProxy::InitRSRenderContext()
{
    renderContext_->Init();
}

void RenderProxy::RenderFrame()
{
    renderContext_->RenderFrame();
}

void RenderProxy::DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height)
{
    renderContext_->DamageFrame(left, top, width, height);
}

void RenderProxy::DamageFrame(const std::vector<RectI> &rects)
{
    renderContext_->DamageFrame(rects);
}

int32_t RenderProxy::GetBufferAge()
{
    return renderContext_->GetBufferAge();
}

void RenderProxy::SwapBuffers()
{
    renderContext_->SwapBuffers();
}

sk_sp<SkSurface> RenderProxy::AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame)
{
    return renderContext_->AcquireSurface(frame);
}

void RenderProxy::Destroy()
{
    renderContext_->Destroy();
    renderContext_ = nullptr;
}

void RenderProxy::MakeCurrent(EGLSurface surface, EGLContext context)
{
    renderContext_->MakeCurrent(surface, context);
}

EGLSurface RenderProxy::CreateEGLSurface(EGLNativeWindowType window)
{
    return renderContext_->CreateEGLSurface(window);
}

void RenderProxy::DestroyEGLSurface(EGLSurface surface)
{
    renderContext_->DestroyEGLSurface(surface);
}

GrContext* RenderProxy::GetGrContext()
{
    return renderContext_->GetGrContext();
}

void RenderProxy::SetCacheDir(const std::string& filePath)
{
    renderContext_->SetCacheDir(filePath);
}

void RenderProxy::ClearRedundantResources()
{
    renderContext_->ClearRedundantResources();
}

EGLContext RenderProxy::GetEGLContext() const
{
    return renderContext_->GetEGLContext();
}

EGLDisplay RenderProxy::GetEGLDisplay() const
{
    return renderContext_->GetEGLDisplay();
}

std::string RenderProxy::CleanAllShaderCache() const
{
    return renderContext_->CleanAllShaderCache();
}

std::string RenderProxy::GetShaderCacheSize() const
{
    return renderContext_->GetShaderCacheSize();
}

void RenderProxy::SetUniRenderMode(bool isUni)
{
    renderContext_->SetUniRenderMode(isUni);
}

void RenderProxy::SetColorSpace(ColorGamut colorSpace)
{
    renderContext_->SetColorSpace(colorSpace);
}

void RenderProxy::ShareMakeCurrent(EGLContext shareContext)
{
    renderContext_->ShareMakeCurrent(shareContext);
}

void RenderProxy::MakeSelfCurrent()
{
    renderContext_->MakeSelfCurrent();
}

EGLContext RenderProxy::CreateShareContext()
{
    return renderContext_->CreateShareContext();
}

#ifdef RS_ENABLE_VK
void RenderProxy::InitializeVulkan(uint32_t num)
{
    renderContext_->InitializeVulkan(num);
}

bool RenderProxy::SetUpVulkanWindow(std::unique_ptr<vulkan::VulkanNativeSurfaceOHOS> vulkanSurface)
{
    return renderContext_->SetUpVulkanWindow(std::move(vulkanSurface));
}

void RenderProxy::WaitForSharedFence()
{
    renderContext_->WaitForSharedFence();
}

void RenderProxy::ResetSharedFence()
{
    renderContext_->ResetSharedFence();
}

void RenderProxy::PresentAll()
{
    renderContext_->PresentAll();
}

#endif
}
}