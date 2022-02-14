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

#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H
#include "interface_render_backend.h"
#include "canvas_context.h"
#include "render_utils.h"

namespace OHOS {
namespace Rosen {
class RenderManager {
public:
    RenderManager() {}
    virtual ~RenderManager() {}

    IRenderBackend* CreateRenderBackend();
    void DestroyRenderBackend();
    CanvasContext* CreateCanvasContext();
    void DestroyCanvasContext();
    IRenderBackend* GetRenderBackend();
    CanvasContext* GetRenderContext();
private:
    IRenderBackend* renderBackend_;
    CanvasContext* canvasContext_;
    RenderBackendType rendeBackendType_;
};
}
}
#endif