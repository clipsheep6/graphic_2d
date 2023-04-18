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

#include "gles_render_backend.h"

#include "egl_manager.h"
#include "drawing_utils.h"

#include "rs_trace.h"

#include "memory/rs_tag_tracker.h"
#include "utils/log.h"
namespace OHOS {
namespace Rosen {
GLESRenderBackend::GLESRenderBackend() noexcept
{
    eglManager_ = std::make_unique<EGLManager>();
}

GLESRenderBackend::~GLESRenderBackend()
{
}

void GLESRenderBackend::Initialize()
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr, can not InitDrawContext");
        return;
    }
    eglManager_->Initialize();
}

bool GLESRenderBackend::SetUpGrContext()
{
    if (grContext_ != nullptr) {
        LOGD("grContext has already created!!");
        return true;
    }

    sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
    if (glInterface.get() == nullptr) {
        LOGE("SetUpGrContext failed to make native interface");
        return false;
    }

    GrContextOptions options;
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;

    mHandler_ = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    if (isUniRenderMode_) {
        cacheDir_ = UNIRENDER_CACHE_DIR;
    }
    mHandler_->ConfigureContext(&options, glesVersion, size, cacheDir_, isUniRenderMode_);

    sk_sp<GrContext> grContext(GrContext::MakeGL(std::move(glInterface), options));
    if (grContext == nullptr) {
        LOGE("SetUpGrContext grContext is null");
        return false;
    }
    grContext_ = std::move(grContext);
    return true;
}

void GLESRenderBackend::MakeCurrent()
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr, can not MakeCurrent");
        return;
    }
    eglManager_->MakeCurrent();
}

void GLESRenderBackend::SwapBuffers()
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr, can not SwapBuffers");
        return;
    }
    eglManager_->SwapBuffers();
}

void GLESRenderBackend::SetUpSurface(void* window)
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr, can not CreateSurface");
    }
    eglManager_->SetUpEGLSurface((EGLNativeWindowType)window);
}

void GLESRenderBackend::DestorySurface()
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr, can not DestorySurface");
    }
    eglManager_->DestroySurface();
}

void GLESRenderBackend::DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height)
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr");
        return;
    }
    eglManager_->DamageFrame(left, top, width, height);  
}

void GLESRenderBackend::DamageFrame(const std::vector<RectI> &rects)
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr");
        return;
    }
    eglManager_->DamageFrame(rects);  
}

void GLESRenderBackend::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    if (eglManager_ == nullptr) {
        LOGE("eglManager_ is nullptr");
        return;
    }
    eglManager_->SetUiTimeStamp(frame, uiTimestamp);
}

int32_t GLESRenderBackend::GetBufferAge()
{
    return static_cast<int32_t>(eglManager_->QueryEglBufferAge());
}

void GLESRenderBackend::Destroy()
{
    grContext_ = nullptr;
    skSurface_ = nullptr;
}

void GLESRenderBackend::RenderFrame()
{
    RS_TRACE_FUNC();
    // flush commands
    if (skSurface_->getCanvas() != nullptr) {
        LOGD("RenderFrame: Canvas");
        skSurface_->getCanvas()->flush();
    } else {
        LOGW("canvas is nullptr!!!");
    }
}

sk_sp<SkSurface> GLESRenderBackend::AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame)
{
    if (!SetUpGrContext()) {
        LOGE("GrContext is not ready!!!");
        return nullptr;
    }

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType;

    GrBackendRenderTarget backendRenderTarget(frame->GetWidth(), frame->GetHeight(), 0, 8, framebufferInfo);
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;

    sk_sp<SkColorSpace> skColorSpace = nullptr;

    switch (colorSpace_) {
        // [planning] in order to stay consistant with the colorspace used before, we disabled
        // COLOR_GAMUT_SRGB to let the branch to default, then skColorSpace is set to nullptr
        case COLOR_GAMUT_DISPLAY_P3:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDCIP3);
            break;
        case COLOR_GAMUT_ADOBE_RGB:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kAdobeRGB);
            break;
        case COLOR_GAMUT_BT2020:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kRec2020);
            break;
        default:
            break;
    }
    RSTagTracker tagTracker(GetGrContext(), RSTagTracker::TAGTYPE::TAG_ACQUIRE_SURFACE);
    skSurface_ = SkSurface::MakeFromBackendRenderTarget(
        GetGrContext(), backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, skColorSpace, &surfaceProps);
    if (skSurface_ == nullptr) {
        LOGW("skSurface is nullptr");
        return nullptr;
    }

    LOGD("CreateCanvas successfully!!!");
    return skSurface_;
}

}
}