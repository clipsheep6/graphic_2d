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

#include "canvas_context.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {

CanvasContext::CanvasContext(RenderBackendType rendeBackendType)
{
    rendeBackendType_ = rendeBackendType;
}

CanvasContext::~CanvasContext()
{
    grContext_ = nullptr;
    skSurface_ = nullptr;
    skCanvas_ = nullptr;
}

void CanvasContext::RenderFrame()
{
    if (rendeBackendType_ == RenderBackendType::GLES) {
        // flush commands
        if (skSurface_->getCanvas() != nullptr) {
            LOGW("RenderFrame: Canvas is %{public}p", skSurface_->getCanvas());
            skSurface_->getCanvas()->flush();
        } else {
            LOGW("canvas is nullptr!!!");
        }
    }
}

bool CanvasContext::SetUpGrContext()
{
    if (grContext_ != nullptr) {
        LOGW("grContext has already created!!");
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

    sk_sp<GrContext> grContext(GrContext::MakeGL(std::move(glInterface), options));
    if (grContext == nullptr) {
        LOGE("SetUpGrContext grContext is null");
        return false;
    }

    grContext_ = std::move(grContext);
    return true;
}

void CanvasContext::CreateCanvas(int width, int height)
{
    if (rendeBackendType_ == RenderBackendType::GLES) {
        if (!SetUpGrContext()) {
            LOGE("GrContext is not ready!!!");
            return;
        }

        GrGLFramebufferInfo framebufferInfo;
        framebufferInfo.fFBOID = 0;
        framebufferInfo.fFormat = 0x8058; //GL_RGBA8

        SkColorType colorType = kRGBA_8888_SkColorType;

        GrBackendRenderTarget backendRenderTarget(width, height, 0, 8, framebufferInfo);
        SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;

        skSurface_ = SkSurface::MakeFromBackendRenderTarget(
            GetGrContext(), backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, &surfaceProps);
        if (skSurface_ == nullptr) {
            LOGW("skSurface is nullptr");
            return;
        }

        LOGE("CreateCanvas successfully!!! (%{public}p)", skSurface_->getCanvas());

        skCanvas_ = skSurface_->getCanvas();
    }

    if (rendeBackendType_ == RenderBackendType::SOFTWARE) {
        auto addr = static_cast<uint32_t*>(buffer_->GetVirAddr());
        if (addr == nullptr) {
            LOGE("buffer addr is invalid");
            return;
        }
        SkImageInfo info =
            SkImageInfo::Make(buffer_->GetWidth(), buffer_->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
        auto uniqueCanvasPtr = SkCanvas::MakeRasterDirect(info, addr, buffer_->GetSize() / buffer_->GetHeight());
        // skCanvas_ = std::move(uniqueCanvasPtr);
        skCanvas_ = uniqueCanvasPtr.get();
    }
}
}
}