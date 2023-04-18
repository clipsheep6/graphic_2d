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

namespace OHOS {
namespace Rosen {
RenderProxy::RenderProxy()
{
    renderContext_ = RSRenderContext::Create();
}

RenderProxy::~RenderProxy()
{
}

void RenderProxy::InitRSRenderContext()
{
    renderContext_->Init();
}
sk_sp<SkSurface> RenderProxy::GetSkSurface(const std::unique_ptr<RSSurfaceFrame>& frame)
{
    return renderContext_->GetSkSurface(frame);
}
SkCanvas* RenderProxy::GetCanvas(const std::unique_ptr<RSSurfaceFrame>& frame)
{
    return renderContext_->GetCanvas(frame);
}

void RenderProxy::SetUpSurface(void* window)
{
    return renderContext_->SetUpSurface(window);
}

void RenderProxy::MakeCurrent()
{
    renderContext_->MakeCurrent();
}

void RenderProxy::SwapBuffers()
{
    renderContext_->SwapBuffers();
}

void RenderProxy::Destroy()
{
}

void RenderProxy::DestorySurface()
{
    renderContext_->DestorySurface();
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

void RenderProxy::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    renderContext_->SetUiTimeStamp(frame, uiTimestamp);
}

int32_t RenderProxy::GetBufferAge()
{
    return renderContext_->GetBufferAge();
}

GrContext* RenderProxy::GetGrContext()
{
    return renderContext_->GetGrContext();
}

bool RenderProxy::SetUpGrContext()
{
    return renderContext_->SetUpGrContext();
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

void RenderProxy::SetUniRenderMode(bool isUni)
{
    renderContext_->SetUniRenderMode(isUni);
}
}
}