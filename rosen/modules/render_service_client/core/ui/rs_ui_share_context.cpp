/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "rs_ui_share_context.h"
#include "pipeline/rs_render_thread.h"

namespace OHOS {
namespace Rosen {
RSUIShareContext& RSUIShareContext::GetInstance()
{
    static RSUIShareContext singleton;
    return singleton;
}

EGLContext RSUIShareContext::GetRsRenderContext() const
{
#ifdef NEW_RENDER_CONTEXT
    auto context = RSRenderThread::Instance().GetRenderContext();
    if (!context) {
        return EGL_NO_CONTEXT;
    }
    auto frame = context->GetRSRenderSurfaceFrame();
    if (frame == nullptr || frame->eglState == nullptr) {
        return EGL_NO_CONTEXT;
    }
    return frame->eglState->eglContext;
#else
    auto context = RSRenderThread::Instance().GetRenderContext();
    if (!context) {
        return EGL_NO_CONTEXT;
    }
    return context->GetEGLContext();
#endif
}
}
}
