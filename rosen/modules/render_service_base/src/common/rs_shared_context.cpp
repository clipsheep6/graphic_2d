/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "common/rs_shared_context.h"

#ifdef ROSEN_IOS
#include "render_context/render_context_egl_defines.h"
#else
#include "EGL/egl.h"
#endif
#include "tools/gpu/GrContextFactory.h"

#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS::Rosen {
std::shared_ptr<RSSharedContext> RSSharedContext::MakeSharedGLContext(EGLContext context)
{
    RS_TRACE_NAME_FMT("MakeSharedGLContext create egl ");
    if (context == EGL_NO_CONTEXT) {
        RS_LOGE("eglGetCurrentContext failed err:%{public}d", eglGetError());
        return nullptr;
    }
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        RS_LOGE("eglGetDisplay failed err:%{public}d", eglGetError());
        return nullptr;
    }
    EGLint attributes[] = {
        // clang-format off
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_ALPHA_SIZE,      8,
        EGL_DEPTH_SIZE,      0,
        EGL_STENCIL_SIZE,    0,
        EGL_NONE,            // termination sentinel
        // clang-format on
    };
    EGLint config_count = 0;
    EGLConfig egl_config = nullptr;
    if (eglChooseConfig(display, attributes, &egl_config, 1, &config_count) != EGL_TRUE) {
        RS_LOGE("Get EGLConfig failed err:%{public}d", eglGetError());
        return nullptr;
    }
    EGLint contextAttr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLContext shareContext = eglCreateContext(display, egl_config, context, contextAttr);
    if (shareContext == EGL_NO_CONTEXT) {
        RS_LOGE("eglCreateContext failed err:%{public}d", eglGetError());
        return nullptr;
    }
    const EGLint attribs[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
    EGLSurface surface = eglCreatePbufferSurface(display, egl_config, attribs);
    if (surface == EGL_NO_SURFACE) {
        RS_LOGE("eglCreatePbufferSurface failed err:%{public}d", eglGetError());
        eglDestroyContext(display, context);
        return nullptr;
    }
    RS_LOGI("create egl success");
    return std::shared_ptr<RSSharedContext>(new RSSharedContext(display, shareContext, surface));
}

RSSharedContext::~RSSharedContext()
{
    if (surface_ != EGL_NO_SURFACE) {
        eglDestroySurface(display_, surface_);
    }

    if (context_ != EGL_NO_CONTEXT) {
        eglDestroyContext(display_, context_);
    }
}

void RSSharedContext::MakeCurrent()
{
    if (eglMakeCurrent(display_, surface_, surface_, context_) != EGL_TRUE) {
        RS_LOGE("eglMakeCurrent failed");
    }
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
sk_sp<GrDirectContext> RSSharedContext::MakeGrContext()
#else
sk_sp<GrContext> RSSharedContext::MakeGrContext()
#endif
{
    GrContextOptions options;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
#ifdef NEW_SKIA
    return GrDirectContext::MakeGL(GrGLMakeNativeInterface(), options);
#else
    return GrContext::MakeGL(GrGLMakeNativeInterface(), options);
#endif
}
#else // USE_ROSEN_DRAWING
std::shared_ptr<Drawing::GPUContext> RSSharedContext::MakeDrContext()
{
    Drawing::GPUContextOptions options;
    std::shared_ptr<Drawing::GPUContext> GPUContext = std::make_shared<Drawing::GPUContext>();
    GPUContext->BuildFromGL(options);
    return GPUContext;
}
#endif
} // namespace OHOS::Rosen
