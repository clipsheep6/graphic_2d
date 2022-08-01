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

#include "egl_api.h"

namespace OHOS {
namespace Rosen {
EGLImageKHR eglCreateImageKHR(
    EGLDisplay dpy,
    EGLContext ctx,
    EGLenum target,
    EGLClientBuffer buffer,
    const EGLint *attribs)
{
    static auto func = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(eglGetProcAddress("eglCreateImageKHR"));
    if (func == nullptr) {
        return EGL_NO_IMAGE_KHR;
    }

    return func(dpy, ctx, target, buffer, attribs);
}

EGLBoolean eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image)
{
    static auto func = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(eglGetProcAddress("eglDestroyImageKHR"));
    if (func == nullptr) {
        return EGL_FALSE;
    }

    return func(dpy, image);
}

void glEGLImageTargetTexture2DOES(GLenum target, GLeglImageOES image)
{
    static auto func = reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(
        eglGetProcAddress("glEGLImageTargetTexture2DOES"));
    if (func == nullptr) {
        return;
    }

    func(target, image);
}

EGLSyncKHR eglCreateSyncKHR(EGLDisplay dpy, EGLenum type, const EGLint *attribs)
{
    static auto func = reinterpret_cast<PFNEGLCREATESYNCKHRPROC>(
        eglGetProcAddress("eglCreateSyncKHR"));
    if (func == nullptr) {
        return EGL_NO_SYNC_KHR;
    }

    return func(dpy, type, attribs);
}

EGLBoolean eglDestroySyncKHR(EGLDisplay dpy, EGLSyncKHR sync)
{
    static auto func = reinterpret_cast<PFNEGLDESTROYSYNCKHRPROC>(
        eglGetProcAddress("eglDestroySyncKHR"));
    if (func == nullptr) {
        return EGL_FALSE;
    }

    return func(dpy, sync);
}

EGLint eglWaitSyncKHR(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags)
{
    static auto func = reinterpret_cast<PFNEGLWAITSYNCKHRPROC>(
        eglGetProcAddress("eglWaitSyncKHR"));
    if (func == nullptr) {
        return EGL_FALSE;
    }

    return func(dpy, sync, flags);
}

EGLint eglDupNativeFenceFDOHOS(EGLDisplay dpy, EGLSyncKHR sync)
{
    static auto func = reinterpret_cast<PFNEGLDUPNATIVEFENCEFDOHOSPROC>(
        eglGetProcAddress("eglDupNativeFenceFDOHOS"));
    if (func == nullptr) {
        return -1; // return invalid fd.
    }

    return func(dpy, sync);
}
} // namespace Rosen
} // namespace OHOS
