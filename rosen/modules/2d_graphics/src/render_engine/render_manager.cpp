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

#include "render_manager.h"
#include "render_utils.h"
#include "gles_render_backend.h"
#include "software_render_backend.h"

namespace OHOS {
namespace Rosen {
IRenderBackend* RenderManager::CreateRenderBackend()
{
    switch (rendeBackendType_) {
        case RenderBackendType::GLES:
            renderBackend_ = new GLESRenderBackend();
            return renderBackend_;
        case RenderBackendType::VULKAN:
            return renderBackend_;
        case RenderBackendType::SOFTWARE:
            renderBackend_ = new SoftwareRenderBackend();
            return renderBackend_;
        default:
            return nullptr;
    }
}

void RenderManager::DestroyRenderBackend()
{
    SAFE_DELETE(renderBackend_);
}

CanvasContext* RenderManager::CreateCanvasContext()
{
    canvasContext_ = new CanvasContext(rendeBackendType_);
    return canvasContext_;
}

void RenderManager::DestroyCanvasContext()
{
    SAFE_DELETE(canvasContext_);
}

IRenderBackend* RenderManager::GetRenderBackend()
{
    return renderBackend_;
}

CanvasContext* RenderManager::GetRenderContext()
{
    return canvasContext_;
}
}
}