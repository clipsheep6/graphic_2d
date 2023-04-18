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
#include <EGL/egl.h>
#include <memory>

namespace OHOS {
namespace Rosen {
std::unique_ptr<RSRenderContext> RSRenderContext::Create()
{
    auto type = Setting::GetRenderBackendType();
    switch (type) {
        case RenderBackendType::GLES:
#ifdef ACE_ENABLE_GL
            LOGI("RSRenderContext::Create with gles backend");

            return std::make_unique<RSRenderContext>(std::make_shared<GLESRenderBackend>());
#endif
        case RenderBackendType::SOFTWARE:
            LOGE("RSRenderContext::Create with software backend");
            return std::make_unique<RSRenderContext>(std::make_shared<SoftwareRenderBackend>());
        default:
            return nullptr;
    }
}

RSRenderContext::RSRenderContext(std::shared_ptr<IRenderBackend> renderBackend)
    : renderBackend_(renderBackend)
{
}

RSRenderContext::~RSRenderContext()
{
    skSurface_ = nullptr;
    renderBackend_->Destroy();
}

void RSRenderContext::SetUpSurface(void* window)
{
    renderBackend_->SetUpSurface(window);
}

void RSRenderContext::DestorySurface()
{
    renderBackend_->DestorySurface();
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

void RSRenderContext::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    renderBackend_->SetUiTimeStamp(frame, uiTimestamp);
}

int32_t RSRenderContext::GetBufferAge()
{
    return renderBackend_->GetBufferAge();
}

sk_sp<SkSurface> RSRenderContext::GetSkSurface(const std::unique_ptr<RSSurfaceFrame>& frame)
{
    if (skSurface_ == nullptr) {
        skSurface_ = renderBackend_->AcquireSurface(frame);
    }
    return skSurface_;
}

SkCanvas* RSRenderContext::GetCanvas(const std::unique_ptr<RSSurfaceFrame>& frame)
{
    if (skSurface_ == nullptr) {
        skSurface_ = renderBackend_->AcquireSurface(frame);
    }
    return skSurface_->getCanvas();
}

void RSRenderContext::Init()
{
    renderBackend_->Initialize();
}

void RSRenderContext::MakeCurrent()
{
    renderBackend_->MakeCurrent();
}

void RSRenderContext::SwapBuffers()
{
    renderBackend_->SwapBuffers();
}

GrContext* RSRenderContext::GetGrContext()
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->GetGrContext();
    }
    LOGE("glesRenderBackend is nullptr!");
    return nullptr;
}

bool RSRenderContext::SetUpGrContext()
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->SetUpGrContext();
    }
    LOGE("glesRenderBackend is nullptr!");
    return false;
}

void RSRenderContext::SetCacheDir(const std::string& filePath)
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        glesRenderBackend->SetCacheDir(filePath);
    }
    LOGE("glesRenderBackend is nullptr!");
}

void RSRenderContext::ClearRedundantResources()
{
    RS_TRACE_FUNC();
    GrContext* grContext_ = GetGrContext();
    if (grContext_ != nullptr) {
        LOGD("grContext clear redundant resources");
        grContext_->flush();
        // GPU resources that haven't been used in the past 10 seconds
        grContext_->purgeResourcesNotUsedInMs(std::chrono::seconds(10));
    }
}

EGLContext RSRenderContext::GetEGLContext() const
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->GetEGLContext();
    }
    LOGE("glesRenderBackend is nullptr!");
    return EGL_NO_CONTEXT;
}

EGLDisplay RSRenderContext::GetEGLDisplay() const
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->GetEGLDisplay();
    }
    LOGE("glesRenderBackend is nullptr!");
    return EGL_NO_DISPLAY;
}

std::string RSRenderContext::CleanAllShaderCache() const
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->CleanAllShaderCache();
    }
    LOGE("glesRenderBackend is nullptr!");
    return "error";
}

void RSRenderContext::SetUniRenderMode(bool isUni)
{
    std::shared_ptr<GLESRenderBackend> glesRenderBackend = std::static_pointer_cast<GLESRenderBackend>(renderBackend_);
    if (glesRenderBackend != nullptr) {
        return glesRenderBackend->SetUniRenderMode(isUni);
    }
    LOGE("glesRenderBackend is nullptr!");
}

}
}