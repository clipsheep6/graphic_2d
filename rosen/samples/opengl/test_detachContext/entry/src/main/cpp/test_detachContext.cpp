/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi/native_api.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <GLES3/gl32.h>
#include <bits/alltypes.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <GLES3/gl3.h>
#include <hilog/log.h>
#include <native_window/external_window.h>
#include <GLES2/gl2ext.h>
#include "native_image/native_image.h"
#include <unistd.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "native_window/external_window.h"
#include "native_buffer/native_buffer.h"

static const int LOG_PRINT_DOMAIN = 0xFF01;
using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
constexpr const char* EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char* EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
static const int EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char* CHARACTER_STRING_WHITESPACE = " ";
constexpr const char* EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";
EGLDisplay gEglDisplay = EGL_NO_DISPLAY;
EGLContext gEglContext = EGL_NO_CONTEXT;
EGLConfig gConfig;
static const int GSERROR_OK = 0;
static const int SURFACE_ERROR_OK = 0;
static const int GRAPHIC_PIXEL_FMT_RGBA_8888 = 12;
static const int SURFACE_ERROR_ERROR = 50002000;
static const int DEFAULT_POSITION = 0x100;
static const int EGL_DEFAULT_SIZE = 8;

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

static EGLDisplay GetPlatformEglDisplay(EGLenum platform, void* nativeDisplay, const EGLint* attribList)
{
    static GetPlatformDisplayExt eglGetPlatformDisplayExt = NULL;

    if (!eglGetPlatformDisplayExt) {
        const char* extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (extensions &&
            (CheckEglExtension(extensions, EGL_EXT_PLATFORM_WAYLAND) ||
                CheckEglExtension(extensions, EGL_KHR_PLATFORM_WAYLAND))) {
            eglGetPlatformDisplayExt = (GetPlatformDisplayExt)eglGetProcAddress(EGL_GET_PLATFORM_DISPLAY_EXT);
        }
    }

    if (eglGetPlatformDisplayExt) {
        return eglGetPlatformDisplayExt(platform, nativeDisplay, attribList);
    }

    return eglGetDisplay((EGLNativeDisplayType)nativeDisplay);
}

static void AddBuffer(OHNativeWindow* nativeWindow)
{
    int code = SET_USAGE;
    int32_t usage = NATIVEBUFFER_USAGE_CPU_READ | NATIVEBUFFER_USAGE_CPU_WRITE | NATIVEBUFFER_USAGE_MEM_DMA;
    int32_t ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, usage);
    if (ret != GSERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "NativeWindowHandleOpt SET_USAGE faile");
    }
    code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, width, height);
    if (ret != GSERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore",
            "NativeWindowHandleOpt SET_BUFFER_GEOMETRY failed");
    }
    code = SET_STRIDE;
    int32_t stride = 0x8;
    ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, stride);
    if (ret != GSERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "NativeWindowHandleOpt SET_STRIDE failed");
    }
    code = SET_FORMAT;
    int32_t format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, format);
    if (ret != GSERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "NativeWindowHandleOpt SET_FORMAT failed");
    }

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    if (ret != GSERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "NativeWindowHandleOpt SET_FORMAT failed");
    }

    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = DEFAULT_POSITION;
    rect->y = DEFAULT_POSITION;
    rect->w = DEFAULT_POSITION;
    rect->h = DEFAULT_POSITION;
    region->rects = rect;
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
    if (ret != GSERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "NativeWindowHandleOpt SET_FORMAT failed");
    }
    delete region;
}

static int32_t GetData(OH_NativeImage* image, OHNativeWindow* nativeWindow)
{
    AddBuffer(nativeWindow);
    int32_t ret = OH_NativeImage_UpdateSurfaceImage(image);
    if (ret != SURFACE_ERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "OH_NativeImage_UpdateSurfaceImage failed");
        return -1;
    }

    int64_t timeStamp = OH_NativeImage_GetTimestamp(image);
    if (timeStamp == SURFACE_ERROR_ERROR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "OH_NativeImage_GetTimestamp failed");
        return -1;
    }

    float matrix[16]; // 16 is 4x4 matrix
    ret = OH_NativeImage_GetTransformMatrix(image, matrix);
    if (ret != SURFACE_ERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "OH_NativeImage_GetTransformMatrix failed");
        return -1;
    }
    return SURFACE_ERROR_OK;
}

static void InitEglContext()
{
    if (gEglContext != EGL_NO_DISPLAY) {
        return;
    }
    gEglDisplay = GetPlatformEglDisplay(EGL_PLATFORM_OHOS_KHR, EGL_DEFAULT_DISPLAY, NULL);
    if (gEglDisplay == EGL_NO_DISPLAY) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore",
            "Failed to create EGLDisplay gl errno : %{publuc}d", eglGetError());
        return;
    }

    EGLint major;
    EGLint minor;
    if (eglInitialize(gEglDisplay, &major, &minor) == EGL_FALSE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Failed to initialize EGLDisplay");
        return;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Failed to bind OpenGL ES API");
        return;
    }

    unsigned int ret;
    EGLint count;
    EGLint configAttribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, EGL_DEFAULT_SIZE,
        EGL_GREEN_SIZE, EGL_DEFAULT_SIZE, EGL_BLUE_SIZE, EGL_DEFAULT_SIZE, EGL_ALPHA_SIZE,
        EGL_DEFAULT_SIZE, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };

    ret = eglChooseConfig(gEglDisplay, configAttribs, &gConfig, 1, &count);
    if (!(ret && static_cast<unsigned int>(count) >= 1)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Failed to eglChooseConfig");
        return;
    }

    static const EGLint CONTEXT_ATTRIBS[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };

    gEglContext = eglCreateContext(gEglDisplay, gConfig, EGL_NO_CONTEXT, CONTEXT_ATTRIBS);
    if (gEglContext == EGL_NO_CONTEXT) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "eglMakeCurrent failed");
        return;
    }

    eglMakeCurrent(gEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, gEglContext);

    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "EGLCore",  "Create EGL context successfully");
}

static void Deinit()
{
    if (gEglDisplay == EGL_NO_DISPLAY) {
        return;
    }
    eglDestroyContext(gEglDisplay, gEglContext);
    eglMakeCurrent(gEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(gEglDisplay);
    eglReleaseThread();

    gEglDisplay = EGL_NO_DISPLAY;
    gEglContext = EGL_NO_CONTEXT;
}

static int32_t TestNativeImage()
{
    InitEglContext();
    GLuint textureId;
    glGenTextures(1, &textureId);

    OH_NativeImage* image = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);

    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(image);
    if (nativeWindow == nullptr) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "OH_NativeImage_AcquireNativeWindow failed");
        return -1;
    }

    int32_t ret = GetData(image, nativeWindow);
    if (ret != SURFACE_ERROR_OK) {
        return -1;
    }

    GLuint textureId2;
    glGenTextures(1, &textureId2);
    ret = OH_NativeImage_AttachContext(image, textureId2);
    if (ret != SURFACE_ERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore",
            "OH_NativeImage_AttachContext textureId2 failed");
        return -1;
    }

    ret = GetData(image, nativeWindow);
    if (ret != SURFACE_ERROR_OK) {
        return -1;
    }

    ret = OH_NativeImage_DetachContext(image);
    if (ret != SURFACE_ERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "OH_NativeImage_DetachContext failed");
        return -1;
    }

    ret = OH_NativeImage_AttachContext(image, textureId2);
    if (ret != SURFACE_ERROR_OK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore",
            "OH_NativeImage_AttachContext after OH_NativeImage_DetachContext failed");
        return -1;
    }

    ret = GetData(image, nativeWindow);
    if (ret != SURFACE_ERROR_OK) {
        return -1;
    }

    OH_NativeImage_Destroy(&image);
    if (image != nullptr) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "OH_NativeImage_Destroy failed");
        return -1;
    }

    Deinit();
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "EGLCore", "sample end successfully");
    return 0;
}


static napi_value DetachContext(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    if (TestNativeImage() == 0) {
        napi_create_int32(env, 0, &result);
    } else {
        napi_create_int32(env, -1, &result);
    }
    return result;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        { "testDetachContext", nullptr, DetachContext, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version =1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void)
{
    napi_module_register(&demoModule);
}
