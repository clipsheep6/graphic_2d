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

#include "rs_render_context.h"
#include "rs_trace.h"

#include "drawing_utils.h"
#include "software_render_backend.h"
#include "gles_render_backend.h"
#ifdef RS_ENABLE_VK
#include "vulkan_render_backend.h"
#endif
#include <EGL/egl.h>
#include <memory>

namespace OHOS {
namespace Rosen {
RSRenderContext::RSRenderContext(std::shared_ptr<IRenderBackend> renderBackend)
    : renderBackend_(renderBackend)
{
}

RSRenderContext::~RSRenderContext()
{
    LOGD("RSRenderContext::~RSRenderContext");
    renderBackend_->Destroy();
    renderBackend_ = nullptr;
}

std::unique_ptr<RSRenderContext> RSRenderContext::Create()
{
    auto type = Setting::GetRenderBackendType();
    switch (type) {
        case RenderBackendType::GLES:
            LOGD("RSRenderContext::Create with gles backend");
            return std::make_unique<RSRenderContext>(std::make_shared<GLESRenderBackend>());
#ifdef RS_ENABLE_VK
        case RenderBackendType::VULKAN:
            LOGD("RSRenderContext::Create with vk backend");
            return std::make_unique<RSRenderContext>(std::make_shared<VulkanRenderBackend>());
#endif
        case RenderBackendType::SOFTWARE:
            LOGD("RSRenderContext::Create with software backend");
            return std::make_unique<RSRenderContext>(std::make_shared<SoftwareRenderBackend>());
        default:
            LOGD("RSRenderContext::Create with unknown renderbackend type");
            return nullptr;
    }
}

void RSRenderContext::Init()
{
    renderBackend_->Initialize();
}

void RSRenderContext::RenderFrame()
{
    renderBackend_->RenderFrame();
}

void RSRenderContext::DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height)
{
    renderBackend_->DamageFrame(left, top, width, height);
}

void RSRenderContext::DamageFrame(const std::vector<RectI> &rects)
{
    renderBackend_->DamageFrame(rects);
}

int32_t RSRenderContext::GetBufferAge()
{
    return renderBackend_->GetBufferAge();
}

sk_sp<SkSurface> RSRenderContext::AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame)
{
    return renderBackend_->AcquireSurface(frame);
}

void RSRenderContext::SwapBuffers()
{
    renderBackend_->SwapBuffers();
}

void RSRenderContext::Destroy()
{
    renderBackend_->Destroy();
    renderBackend_ = nullptr;
}

void RSRenderContext::MakeCurrent(EGLSurface surface, EGLContext context)
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        glesRenderBackend->MakeCurrent(surface, context);
    } else {
        LOGE("RSRenderContext::MakeCurrent glesRenderBackend is nullptr!");
    }
}

EGLSurface RSRenderContext::CreateEGLSurface(EGLNativeWindowType window)
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->CreateEGLSurface(window);
    }
    LOGE("RSRenderContext::CreateEGLSurface glesRenderBackend is nullptr!");
    return EGL_NO_SURFACE;
}

void RSRenderContext::DestroyEGLSurface(EGLSurface surface)
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        glesRenderBackend->DestroyEGLSurface(surface);
    } else {
        LOGE("RSRenderContext::DestroyEGLSurface glesRenderBackend is nullptr!");
    }
}

GrContext* RSRenderContext::GetGrContext()
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->GetGrContext();
    }
    LOGE("RSRenderContext::GetGrContext glesRenderBackend is nullptr!");
    return nullptr;
}

void RSRenderContext::SetCacheDir(const std::string& filePath)
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        glesRenderBackend->SetCacheDir(filePath);
    } else {
        LOGE("RSRenderContext::SetCacheDir glesRenderBackend is nullptr!");
    }
}

void RSRenderContext::ClearRedundantResources()
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        glesRenderBackend->ClearRedundantResources();
    } else {
        LOGE("RSRenderContext::ClearRedundantResources glesRenderBackend is nullptr!");
    }
}

EGLContext RSRenderContext::GetEGLContext() const
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->GetEGLContext();
    }
    LOGE("RSRenderContext::GetEGLContext glesRenderBackend is nullptr!");
    return EGL_NO_CONTEXT;
}

EGLDisplay RSRenderContext::GetEGLDisplay() const
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->GetEGLDisplay();
    }
    LOGE("RSRenderContext::GetEGLDisplay glesRenderBackend is nullptr!");
    return EGL_NO_DISPLAY;
}

std::string RSRenderContext::GetShaderCacheSize() const
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->GetShaderCacheSize();
    }
    LOGE("RSRenderContext::GetShaderCacheSize glesRenderBackend is nullptr!");
    return "error";
}

std::string RSRenderContext::CleanAllShaderCache() const
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->CleanAllShaderCache();
    }
    LOGE("RSRenderContext::CleanAllShaderCache glesRenderBackend is nullptr!");
    return "error";
}

void RSRenderContext::SetUniRenderMode(bool isUni)
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        glesRenderBackend->SetUniRenderMode(isUni);
    } else { 
        LOGE("RSRenderContext::SetUniRenderMode glesRenderBackend is nullptr!");
    }
}

void RSRenderContext::SetColorSpace(ColorGamut colorSpace)
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        glesRenderBackend->SetColorSpace(colorSpace);
    } else { 
        LOGE("RSRenderContext::SetColorSpace glesRenderBackend is nullptr!");
    }
}

void RSRenderContext::ShareMakeCurrent(EGLContext shareContext)
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        glesRenderBackend->ShareMakeCurrent(shareContext);
    } else { 
        LOGE("RSRenderContext::ShareMakeCurrent glesRenderBackend is nullptr!");
    }
}

void RSRenderContext::MakeSelfCurrent()
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        glesRenderBackend->MakeSelfCurrent();
    } else { 
        LOGE("RSRenderContext::MakeSelfCurrent glesRenderBackend is nullptr!");
    }
}

EGLContext RSRenderContext::CreateShareContext()
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->CreateShareContext();
    }
    LOGE("RSRenderContext::CreateShareContext glesRenderBackend is nullptr!");
    return EGL_NO_CONTEXT;
}

#ifdef RS_ENABLE_VK
void RSRenderContext::InitializeVulkan(uint32_t num)
{
    std::shared_ptr<VulkanRenderBackend> vkRenderBackend = std::static_pointer_cast<VulkanRenderBackend>(renderBackend_);
    if (vkRenderBackend != nullptr) {
        vkRenderBackend->InitializeVulkan(num);
    } else {
        LOGE("RSRenderContext::InitializeVulkan vkRenderBackend is nullptr!");
    }
}

bool RSRenderContext::SetUpVulkanWindow(std::unique_ptr<vulkan::VulkanNativeSurfaceOHOS> vulkanSurface)
{
    std::shared_ptr<VulkanRenderBackend> vkRenderBackend = std::static_pointer_cast<VulkanRenderBackend>(renderBackend_);
    if (vkRenderBackend != nullptr) {
        return vkRenderBackend->SetUpVulkanWindow(std::move(vulkanSurface));
    }
    LOGE("RSRenderContext::SetUpVulkanWindow vkRenderBackend is nullptr!");
    return false;
}

void RSRenderContext::WaitForSharedFence()
{
    std::shared_ptr<VulkanRenderBackend> vkRenderBackend = std::static_pointer_cast<VulkanRenderBackend>(renderBackend_);
    if (vkRenderBackend != nullptr) {
        return vkRenderBackend->WaitForSharedFence();
    } else {
        LOGE("RSRenderContext::WaitForSharedFence vkRenderBackend is nullptr!");
    }
}

void RSRenderContext::ResetSharedFence()
{
    std::shared_ptr<VulkanRenderBackend> vkRenderBackend = std::static_pointer_cast<VulkanRenderBackend>(renderBackend_);
    if (vkRenderBackend != nullptr) {
        return vkRenderBackend->ResetSharedFence();
    } else {
        LOGE("RSRenderContext::ResetSharedFence vkRenderBackend is nullptr!");
    }
}

void RSRenderContext::PresentAll()
{
    std::shared_ptr<VulkanRenderBackend> vkRenderBackend = std::static_pointer_cast<VulkanRenderBackend>(renderBackend_);
    if (vkRenderBackend != nullptr) {
        return vkRenderBackend->PresentAll();
    } else {
        LOGE("RSRenderContext::PresentAll vkRenderBackend is nullptr!");
    }
}

#endif
}
}