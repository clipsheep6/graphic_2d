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

#include "gles_render_backend.h"
#include "egl_manager.h"
#include "render_utils.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {

GLESRenderBackend::GLESRenderBackend() noexcept
{
    eglManager_ = new EGLManager();
}

GLESRenderBackend::~GLESRenderBackend()
{
    SAFE_DELETE(eglManager_);
}

void GLESRenderBackend::Initialize()
{
    CHECKSTATE(eglManager_);
    eglManager_->Initialize();
}

void GLESRenderBackend::MakeCurrent()
{
    CHECKSTATE(eglManager_);
    eglManager_->MakeCurrent();
}

void GLESRenderBackend::SwapBuffers()
{
    CHECKSTATE(eglManager_);
    eglManager_->SwapBuffers();
}

void GLESRenderBackend::SetSurface(const sptr<Surface> surface, int width, int height)
{

}

void GLESRenderBackend::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    CHECKSTATE(eglManager_);
    eglManager_->SetDamageRegion(left, top, width, height);  
}
}
}