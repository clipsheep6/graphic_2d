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

#ifndef RENDER_SERVICE_CORE_GPU_EGL_API_H
#define RENDER_SERVICE_CORE_GPU_EGL_API_H

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES/gl.h"
#include "GLES/glext.h"
#include "GLES3/gl32.h"

namespace OHOS {
namespace Rosen {
EGLImageKHR eglCreateImageKHR(
    EGLDisplay dpy,
    EGLContext ctx,
    EGLenum target,
    EGLClientBuffer buffer,
    const EGLint *attribs);

EGLBoolean eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image);

void glEGLImageTargetTexture2DOES(GLenum target, GLeglImageOES image);

EGLSyncKHR eglCreateSyncKHR(EGLDisplay dpy, EGLenum type, const EGLint *attribs);

EGLBoolean eglDestroySyncKHR(EGLDisplay dpy, EGLSyncKHR sync);

EGLint eglWaitSyncKHR(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags);

EGLint eglDupNativeFenceFDOHOS(EGLDisplay dpy, EGLSyncKHR sync);
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_GPU_EGL_API_H
