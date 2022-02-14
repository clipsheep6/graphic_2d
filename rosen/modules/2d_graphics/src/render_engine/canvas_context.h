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

#ifndef CANVAS_CONTEXT_H
#define CANVAS_CONTEXT_H
#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/gl/GrGLInterface.h"

#include "render_utils.h"
#include <surface.h>

namespace OHOS {
namespace Rosen {
class CanvasContext {
public:
    CanvasContext() = delete;
    CanvasContext(RenderBackendType rendeBackendType);
    virtual ~CanvasContext();
    void RenderFrame();
    void CreateCanvas(int width, int height);
    SkCanvas* GetCanvas() const
    {
        return skCanvas_;
    }
private:
    bool SetUpGrContext();
    GrContext* GetGrContext() const
    {
        return grContext_.get();
    }

    sk_sp<GrContext> grContext_ = nullptr;
    sk_sp<SkSurface> skSurface_ = nullptr;
    SkCanvas* skCanvas_ = nullptr;

    sptr<SurfaceBuffer> buffer_;

    RenderBackendType rendeBackendType_;
};
}
}
#endif