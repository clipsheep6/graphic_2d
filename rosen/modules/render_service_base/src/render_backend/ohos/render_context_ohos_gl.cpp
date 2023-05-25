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

#include "ohos/render_context_ohos_gl.h"

#include <EGL/egl.h>

#include "rs_trace.h"
#include "window.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
constexpr const char* EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char* EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char* CHARACTER_STRING_WHITESPACE = " ";
constexpr const char* EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";
constexpr const char* EGL_KHR_SURFACELESS_CONTEXT = "EGL_KHR_surfaceless_context";

// use functor to call gel*KHR API
static PFNEGLSETDAMAGEREGIONKHRPROC GetEGLSetDamageRegionKHRFunc()
{
    static auto func = reinterpret_cast<PFNEGLSETDAMAGEREGIONKHRPROC>(eglGetProcAddress("eglSetDamageRegionKHR"));
    return func;
}

static bool CheckEglExtension(const char* extensions, const char* extension)
{
    size_t extlen = strlen(extension);
    const char* end = extensions + strlen(extensions);

    while (extensions < end) {
        size_t n = 0;
        /* Skip whitespaces, if any */
        if (*extensions == CHARACTER_WHITESPACE) {
            extensions++;
            continue;
        }

        n = strcspn(extensions, CHARACTER_STRING_WHITESPACE);

        /* Compare strings */
        if (n == extlen && strncmp(extension, extensions, n) == 0) {
            return true; /* Found */
        }
        extensions += n;
    }
    /* Not found */
    return false;
}

static EGLDisplay GetPlatformEglDisplay(EGLenum platform, void* native_display, const EGLint* attrib_list)
{
    static GetPlatformDisplayExt eglGetPlatformDisplayExt = NULL;

    if (!eglGetPlatformDisplayExt) {
        const char* extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (extensions &&
            (CheckEglExtension(extensions, EGL_EXT_PLATFORM_WAYLAND) ||
                CheckEglExtension(extensions, EGL_KHR_PLATFORM_WAYLAND))) {
            eglGetPlatformDisplayExt = reinterpret_cast<GetPlatformDisplayExt>(
                eglGetProcAddress(EGL_GET_PLATFORM_DISPLAY_EXT));
        }
    }

    if (eglGetPlatformDisplayExt) {
        return eglGetPlatformDisplayExt(platform, native_display, attrib_list);
    }

    return eglGetDisplay(static_cast<EGLNativeDisplayType>(native_display));
}

RenderContextOhosGl::RenderContextOhosGl() noexcept: nativeWindow_(nullptr), eglDisplay_(EGL_NO_DISPLAY),
    eglContext_(EGL_NO_CONTEXT), eglSurface_(EGL_NO_SURFACE), config_(nullptr)
{}

RenderContextOhosGl::~RenderContextOhosGl()
{
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        return;
    }

    eglDestroyContext(eglDisplay_, eglContext_);
    if (pbufferSurface_ != EGL_NO_SURFACE) {
        eglDestroySurface(eglDisplay_, pbufferSurface_);
    }
    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(eglDisplay_);
    eglReleaseThread();

    eglDisplay_ = EGL_NO_DISPLAY;
    eglContext_ = EGL_NO_CONTEXT;
    eglSurface_ = EGL_NO_SURFACE;
    pbufferSurface_ = EGL_NO_SURFACE;
}

void RenderContextOhosGl::Init()
{
    if (IsContextReady()) {
        LOGD("Egl context has ready initialized");
        return;
    }

    eglDisplay_ = GetPlatformEglDisplay(EGL_PLATFORM_OHOS_KHR, EGL_DEFAULT_DISPLAY, NULL);
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        LOGW("Failed to create EGLDisplay gl errno : %{public}x", eglGetError());
        return;
    }

    EGLint major, minor;
    if (eglInitialize(eglDisplay_, &major, &minor) == EGL_FALSE) {
        LOGE("Failed to initialize EGLDisplay");
        return;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        LOGE("Failed to bind OpenGL ES API");
        return;
    }

    unsigned int ret;
    EGLint count;
    EGLint config_attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };

    ret = eglChooseConfig(eglDisplay_, config_attribs, &config_, 1, &count);
    if (!(ret && static_cast<unsigned int>(count) >= 1)) {
        LOGE("Failed to eglChooseConfig");
        return;
    }

    static const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };

    eglContext_ = eglCreateContext(eglDisplay_, config_, EGL_NO_CONTEXT, context_attribs);
    if (eglContext_ == EGL_NO_CONTEXT) {
        LOGE("Failed to create egl context %{public}x", eglGetError());
        return;
    }
    CreatePbufferSurface();
    if (!eglMakeCurrent(eglDisplay_, pbufferSurface_, pbufferSurface_, eglContext_)) {
        LOGE("Failed to make current on surface, error is %{public}x", eglGetError());
        return;
    }
    frame_->eglDisplay_ = eglDisplay_;
    frame_->eglContext_ = eglContext_;
    LOGD("Init context successfully, version %{public}d.%{public}d", major, minor);
}

bool RenderContextOhosGl::IsContextReady()
{
    return eglContext_ != EGL_NO_DISPLAY;
}

void RenderContextOhosGl::MakeCurrent(void* curSurface, void* curContext)
{
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    if (curSurface != nullptr) {
        surface = static_cast<EGLSurface>(curSurface);
    }

    if (curContext != nullptr) {
        context = static_cast<EGLContext>(curContext);
    }

    if (surface == EGL_NO_SURFACE) {
        surface = pbufferSurface_;
    }
    if (context == EGL_NO_CONTEXT) {
        context = eglContext_;
    }
    if (!eglMakeCurrent(eglDisplay_, surface, surface, context)) {
        LOGE("Failed to make current on surface, error is %{public}x", eglGetError());
    }
    eglSurface_ = surface;
}

void* RenderContextOhosGl::CreateContext(bool share)
{
    static const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE};
    EGLContext context = EGL_NO_CONTEXT;
    if (!share) {
        context = eglCreateContext(eglDisplay_, config_, EGL_NO_CONTEXT, context_attribs);
    } else {
        std::unique_lock<std::mutex> lock(shareContextMutex_);
        context = eglCreateContext(eglDisplay_, config_, eglContext_, context_attribs);
    }
    return static_cast<void*>(context);
}

bool RenderContextOhosGl::CreateSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (frame->nativeWindow_ != nullptr) {
        LOGD("native window has been created");
        return true;
    }

    frame->nativeWindow_ = CreateNativeWindowFromSurface(&(frame->producer_));
    nativeWindow_ = frame->nativeWindow_;
    if (!IsContextReady()) {
        LOGE("EGL context has not initialized");
        return false;
    }
    
    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    EGLSurface surface = eglCreateWindowSurface(eglDisplay_, config_, nativeWindow_, NULL);
    if (surface == EGL_NO_SURFACE) {
        LOGE("Failed to create eglsurface!!! %{public}x", eglGetError());
        return false;
    }

    NativeWindowHandleOpt(nativeWindow_, SET_FORMAT, frame->pixelFormat_);
#ifdef RS_ENABLE_AFBC
    if (RSSystemProperties::GetAFBCEnabled()) {
        int32_t format = 0;
        NativeWindowHandleOpt(nativeWindow_, GET_FORMAT, &format);
        if (format == PIXEL_FMT_RGBA_8888 && useAFBC) {
            bufferUsage_ =
                (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
        }
    }
#endif
    NativeWindowHandleOpt(nativeWindow_, SET_USAGE, frame->bufferUsage_);
    NativeWindowHandleOpt(nativeWindow_, SET_BUFFER_GEOMETRY, frame->width_, frame->height_);
    NativeWindowHandleOpt(nativeWindow_, SET_COLOR_GAMUT, frame->colorSpace_);
    NativeWindowHandleOpt(nativeWindow_, SET_UI_TIMESTAMP, frame->uiTimestamp_);

    eglSurface_ = surface;
    if (!eglMakeCurrent(eglDisplay_, eglSurface_, eglSurface_, eglContext_)) {
        LOGE("Failed to make current on surface, error is %{public}x", eglGetError());
        return false;
    }
    LOGD("Create egl surface successfully");
    return true;
}

void RenderContextOhosGl::DestroySurface()
{
    if (!eglDestroySurface(eglDisplay_, eglSurface_)) {
        LOGE("Failed to DestroyEGLSurface surface, error is %{public}x", eglGetError());
    }
}

void RenderContextOhosGl::DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height,
    const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if ((eglDisplay_ == nullptr) || (eglSurface_ == nullptr)) {
        LOGE("eglDisplay or eglSurface is nullptr");
        return;
    }

    EGLint rect[4];
    rect[0] = left;
    rect[1] = top;
    rect[2] = width;
    rect[3] = height;

    EGLBoolean ret = GetEGLSetDamageRegionKHRFunc()(eglDisplay_, eglSurface_, rect, 1);
    if (ret == EGL_FALSE) {
        LOGE("eglSetDamageRegionKHR is failed");
    }
}

void RenderContextOhosGl::DamageFrame(const std::vector<RectI> &rects)
{
    if ((eglDisplay_ == nullptr) || (eglSurface_ == nullptr)) {
        LOGE("eglDisplay or eglSurface is nullptr");
        return;
    }

    size_t size = rects.size();
    if (size == 0) {
        LOGE("invalid rects size");
        return;
    }

    EGLint eglRect[size * 4]; // 4 is size of RectI.
    int index = 0;
    for (const RectI& rect : rects) {
        eglRect[index * 4] = rect.left_; // 4 is size of RectI.
        eglRect[index * 4 + 1] = rect.top_; // 4 is size of RectI.
        eglRect[index * 4 + 2] = rect.width_; // 4 is size of RectI, 2 is the index of the width_ subscript.
        eglRect[index * 4 + 3] = rect.height_; // 4 is size of RectI, 3 is the index of the height_ subscript.
        index++;
    }

    EGLBoolean ret = GetEGLSetDamageRegionKHRFunc()(eglDisplay_, eglSurface_, eglRect, size);
    if (ret == EGL_FALSE) {
        LOGE("eglSetDamageRegionKHR is failed");
    }
}

int32_t RenderContextOhosGl::GetBufferAge()
{
    if ((eglDisplay_ == nullptr) || (eglSurface_ == nullptr)) {
        LOGE("eglDisplay or eglSurface is nullptr");
        return EGL_UNKNOWN;
    }
    EGLint bufferAge = EGL_UNKNOWN;
    EGLBoolean ret = eglQuerySurface(eglDisplay_, eglSurface_, EGL_BUFFER_AGE_KHR, &bufferAge);
    if (ret == EGL_FALSE) {
        LOGE("eglQuerySurface is failed");
        return EGL_UNKNOWN;
    }
    return static_cast<int32_t>(bufferAge);
}

void RenderContextOhosGl::SwapBuffers(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    RS_TRACE_FUNC();
    if (!eglSwapBuffers(eglDisplay_, eglSurface_)) {
        LOGE("Failed to SwapBuffers on surface, error is %{public}x", eglGetError());
    } else {
        LOGD("SwapBuffers successfully");
    }
}

void RenderContextOhosGl::Destroy()
{
}

void RenderContextOhosGl::CreatePbufferSurface()
{
    const char* extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);

    if ((extensions != nullptr) &&
       (!CheckEglExtension(extensions, EGL_KHR_SURFACELESS_CONTEXT)) &&
       (pbufferSurface_ == EGL_NO_SURFACE)) {
        EGLint attribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
        pbufferSurface_ = eglCreatePbufferSurface(eglDisplay_, config_, attribs);
        if (pbufferSurface_ == EGL_NO_SURFACE) {
            LOGE("Failed to create pbuffer surface");
            return;
        }
    }
}

}
}