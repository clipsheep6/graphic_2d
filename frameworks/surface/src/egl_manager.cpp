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

#include "egl_manager.h"

#include <fcntl.h>
#include <mutex>
#include <unistd.h>
#include "window.h"
#include "buffer_log.h"
// #include "shared/platform.h"

#define CHECK_NOT_INIT(ret)                 \
    if (!initFlag_) {                       \
        BLOGE("EglManager is not init.");   \
        return ret;                         \
    }

#define CHECK_NOT_INIT_VOID()               \
    if (!initFlag_) {                       \
        BLOGE("EglManager is not init.");   \
        return;                             \
    }

#define CHECK_PARAM_NULLPTR(param)          \
    if (param == nullptr) {                 \
        BLOGE("param is nullptr.");         \
        return SURFACE_ERROR_INIT;          \
    }

#define CHECK_PARAM_NULLPTR_RET(param, ret) \
    if (param == nullptr) {                 \
        BLOGE("param is nullptr.");         \
        return ret;                         \
    }

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "EglManager" };
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char *CHARACTER_STRING_WHITESPACE = " ";
constexpr const char *EGL_CREATE_IMAGE_KHR = "eglCreateImageKHR";
constexpr const char *EGL_DESTROY_IMAGE_KHR = "eglDestroyImageKHR";
constexpr const char *EGL_IMAGE_TARGET_TEXTURE2DOES = "glEGLImageTargetTexture2DOES";
constexpr const char *EGL_KHR_FENCE_SYNC = "EGL_KHR_fence_sync";
constexpr const char *EGL_CREATE_SYNC_KHR = "eglCreateSyncKHR";
constexpr const char *EGL_DESTROY_SYNC_KHR = "eglDestroySyncKHR";
constexpr const char *EGL_CLIENT_WAIT_SYNC_KHR = "eglClientWaitSyncKHR";
constexpr const char *EGL_DUP_NATIVE_FENCE_FD_ANDROID = "eglDupNativeFenceFDANDROID";
constexpr const char *EGL_KHR_WAIT_SYNC = "EGL_KHR_wait_sync";
constexpr const char *EGL_WAIT_SYNC_KHR = "eglWaitSyncKHR";
constexpr const char *GL_OES_EGL_IMAGE = "GL_OES_EGL_image";

} // namespace

EglManager::EglManager()
{
    BLOGD("EglManager");
}

EglManager::~EglManager()
{
    BLOGD("~EglManager");
    Deinit();
}

sptr<EglManager> EglManager::GetInstance()
{
    if (instance_ == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance_ == nullptr) {
            instance_ = new EglManager();
        }
    }
    return instance_;
}

bool EglManager::IsInit() const
{
    return initFlag_;
}

SurfaceError EglManager::Init(EGLContext context)
{
    BLOGD("Init with no surface");
    if (initFlag_) {
        BLOGW("already init.");
        return SURFACE_ERROR_OK;
    }

    if (GbmInit() != SURFACE_ERROR_OK) {
        BLOGE("GbmInit failed.");
        return SURFACE_ERROR_INIT;
    }

    if (EglInit(context) != SURFACE_ERROR_OK) {
        BLOGE("EglInit failed.");
        return SURFACE_ERROR_INIT;
    }

    initFlag_ = true;
    return SURFACE_ERROR_OK;
}

SurfaceError EglManager::Init(EGLContext context, sptr<Surface> &surface)
{
    BLOGD("Init with  surface");
    if (initFlag_) {
        BLOGW("already init.");
        return SURFACE_ERROR_OK;
    }

    if (GbmInit() != SURFACE_ERROR_OK) {
        BLOGE("GbmInit failed.");
        return SURFACE_ERROR_INIT;
    }

    if (EglInit(context, surface) != SURFACE_ERROR_OK) {
        BLOGE("EglInit failed.");
        return SURFACE_ERROR_INIT;
    }

    initFlag_ = true;
    return SURFACE_ERROR_OK;
}

SurfaceError EglManager::GbmInit()
{
    device_ = EGL_DEFAULT_DISPLAY;
    return SURFACE_ERROR_OK;
}

namespace {
static bool CheckEglExtension(const char *extensions, const char *extension)
{
	size_t extlen = strlen(extension);
	const char *end = extensions + strlen(extensions);

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
}

SurfaceError EglManager::EglCheckExt()
{
    const char *eglExtensions = eglQueryString(display_, EGL_EXTENSIONS);
    CHECK_PARAM_NULLPTR(eglExtensions);


    EGLint ret, count;
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    ret = eglChooseConfig(display_, config_attribs, &conf_, 1, &count);
    if (!(ret && count >= 1)) {
        BLOGE("Failed to eglChooseConfig");
        return SURFACE_ERROR_INIT;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError EglManager::EglFuncInit()
{
    const char *eglExtensions = eglQueryString(display_, EGL_EXTENSIONS);
    CHECK_PARAM_NULLPTR(eglExtensions);

    createImage_ = (EglCreateImageFunc)eglGetProcAddress(EGL_CREATE_IMAGE_KHR);
    CHECK_PARAM_NULLPTR(createImage_);

    destroyImage_ = (EglDestroyImageFunc)eglGetProcAddress(EGL_DESTROY_IMAGE_KHR);
    CHECK_PARAM_NULLPTR(destroyImage_);

    imageTargetTexture2d_ = (EglImageTargetTexture2DFunc)eglGetProcAddress(EGL_IMAGE_TARGET_TEXTURE2DOES);
    CHECK_PARAM_NULLPTR(imageTargetTexture2d_);
    
    if (CheckEglExtension(eglExtensions, EGL_KHR_FENCE_SYNC)) {
        createSync_ = (EglCreateSyncFunc)eglGetProcAddress(EGL_CREATE_SYNC_KHR);
        CHECK_PARAM_NULLPTR(createSync_);
        
        destroySync_ = (EglDestroySyncFunc)eglGetProcAddress(EGL_DESTROY_SYNC_KHR);
        CHECK_PARAM_NULLPTR(destroySync_);

        clientWaitSync_ = (EglClientWaitSyncFunc)eglGetProcAddress(EGL_CLIENT_WAIT_SYNC_KHR);
        CHECK_PARAM_NULLPTR(clientWaitSync_);

        dupNativeFenceFd_ = (EglDupNativeFenceFdFunc)eglGetProcAddress(EGL_DUP_NATIVE_FENCE_FD_ANDROID);
        CHECK_PARAM_NULLPTR(dupNativeFenceFd_);
    }

    if (CheckEglExtension(eglExtensions, EGL_KHR_WAIT_SYNC)) {
        waitSync_ = (EglWaitSyncFunc)eglGetProcAddress(EGL_WAIT_SYNC_KHR);
        CHECK_PARAM_NULLPTR(waitSync_);
    }
    return SURFACE_ERROR_OK;
}

SurfaceError EglManager::EglInit(EGLContext ctx)
{
    BLOGD("EglInit with no surface");
    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) {
        BLOGE("Failed to create EGLDisplay gl errno : 0x%{public}x", eglGetError());
        return SURFACE_ERROR_INIT;
    }

    EGLint major, minor;
    if (eglInitialize(display_, &major, &minor) == EGL_FALSE) {
        BLOGE("Failed to initialize EGLDisplay");
        return SURFACE_ERROR_INIT;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        BLOGE("Failed to bind OpenGL ES API");
        return SURFACE_ERROR_INIT;
    }

    if (EglCheckExt() != SURFACE_ERROR_OK) {
        BLOGE("EglCheckExt failed");
        return SURFACE_ERROR_INIT;
    }

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM,
        EGL_NONE
    };

    if (ctx == EGL_NO_CONTEXT) {
        context_ = eglCreateContext(display_, conf_, EGL_NO_CONTEXT, context_attribs);
        if (context_ == EGL_NO_CONTEXT) {
            BLOGE("Failed to create EGLContext");
            return SURFACE_ERROR_INIT;
        }
        ctxReleaseFlg_ = true;
    } else {
        context_ = ctx;
    }

    eglMakeCurrent(display_, eglSurface_, eglSurface_, context_);

    const char *glExtensions = (const char *) glGetString(GL_EXTENSIONS);
    CHECK_PARAM_NULLPTR(glExtensions);

    if (!CheckEglExtension(glExtensions, GL_OES_EGL_IMAGE)) {
        BLOGE("GL_OES_EGL_image not supported");
        return SURFACE_ERROR_INIT;
    }

    if (EglFuncInit() != SURFACE_ERROR_OK) {
        BLOGE("EglFuncInit failed");
        return SURFACE_ERROR_INIT;
    }

    return SURFACE_ERROR_OK;
}

SurfaceError EglManager::EglInit(EGLContext ctx, sptr<Surface> &surface)
{
    BLOGD("EglInit with surface");
    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) {
        BLOGE("Failed to create EGLDisplay egl errno : 0x%{public}x", eglGetError());
        return SURFACE_ERROR_INIT;
    }

    EGLint major, minor;
    if (eglInitialize(display_, &major, &minor) == EGL_FALSE) {
        BLOGE("Failed to initialize EGLDisplay");
        return SURFACE_ERROR_INIT;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        BLOGE("Failed to bind OpenGL ES API");
        return SURFACE_ERROR_INIT;
    }

    if (EglCheckExt() != SURFACE_ERROR_OK) {
        BLOGE("EglCheckExt failed");
        return SURFACE_ERROR_INIT;
    }

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM,
        EGL_NONE
    };

    if (ctx == EGL_NO_CONTEXT) {
        context_ = eglCreateContext(display_, conf_, EGL_NO_CONTEXT, context_attribs);
        if (context_ == EGL_NO_CONTEXT) {
            BLOGE("Failed to create EGLContext");
            return SURFACE_ERROR_INIT;
        }
        ctxReleaseFlg_ = true;
    } else {
        context_ = ctx;
    }

    EGLSurface eglSurface = CreateEglSurface(surface);

    eglMakeCurrent(display_, eglSurface, eglSurface, context_);
    const char *glExtensions = (const char *) glGetString(GL_EXTENSIONS);
    CHECK_PARAM_NULLPTR(glExtensions);

    if (!CheckEglExtension(glExtensions, GL_OES_EGL_IMAGE)) {
        BLOGE("GL_OES_EGL_image not supported");
        return SURFACE_ERROR_INIT;
    }

    if (EglFuncInit() != SURFACE_ERROR_OK) {
        BLOGE("EglFuncInit failed");
        return SURFACE_ERROR_INIT;
    }

    return SURFACE_ERROR_OK;
}

void EglManager::SwapBuffer()
{
    EglMakeCurrent();
    glFlush();
	eglSwapBuffers(display_, eglSurface_);
}

EGLSurface EglManager::CreateEglSurface(sptr<Surface> &surface)
{
    BLOGD("CreateEglSurface");
    EGLSurface eglSurface = EGL_NO_SURFACE;

    EGLint ret, count;
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    ret = eglChooseConfig(display_, config_attribs, &conf_, 1, &count);
    if (!(ret && count >= 1)) {
        BLOGE("Failed to eglChooseConfig");
        return eglSurface;
    }

    if (surface == nullptr) {
        BLOGE("the surface is nullptr");
        return eglSurface;
    }
    NativeWindow* window = CreateNativeWindowFromSurface(&surface);
    if (window == nullptr) {
        BLOGE("can not create NativeWindow frome surface");
        return eglSurface;
    }

    EGLint winAttribs[] = {EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE};
    eglSurface = eglCreateWindowSurface(display_, conf_, (EGLNativeWindowType)window, winAttribs);
    if (eglSurface == EGL_NO_SURFACE) {
        BLOGE("eglCreateWindowSurface failed  eglError: 0x%{public}x", eglGetError());
    }
    eglSurface_ = eglSurface;
    return eglSurface;
}

void EglManager::Deinit()
{
    initFlag_ = false;

    if (drmFd_ >= 0) {
        close(drmFd_);
    }

    if (context_ != EGL_NO_CONTEXT && ctxReleaseFlg_) {
        eglDestroyContext(display_, context_);
    }

    if (display_ != EGL_NO_DISPLAY) {
        eglTerminate(display_);
    }
}

EGLDisplay EglManager::GetEGLDisplay() const
{
    CHECK_NOT_INIT(EGL_NO_DISPLAY);
    return display_;
}

EGLContext EglManager::GetEGLContext() const
{
    CHECK_NOT_INIT(EGL_NO_CONTEXT);
    return context_;
}

EGLSurface EglManager::GetEglSurface() const
{
    CHECK_NOT_INIT(EGL_NO_SURFACE);
    return eglSurface_;
}

EGLImageKHR EglManager::EglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attribList)
{
    CHECK_NOT_INIT(EGL_NO_IMAGE);
    return createImage_(display_, ctx, target, buffer, attribList);
}

EGLImageKHR EglManager::EglCreateImage(EGLenum target, EGLClientBuffer buffer, const EGLint *attribList)
{
    CHECK_NOT_INIT(EGL_NO_IMAGE);
    return createImage_(display_, context_, target, buffer, attribList);
}

EGLBoolean EglManager::EglDestroyImage(EGLImageKHR image)
{
    CHECK_NOT_INIT(EGL_FALSE);
    return destroyImage_(display_, image);
}

void EglManager::EglImageTargetTexture2D(GLenum target, GLeglImageOES image)
{
    CHECK_NOT_INIT_VOID();
    return imageTargetTexture2d_(target, image);
}

EGLSyncKHR EglManager::EglCreateSync(EGLenum type, const EGLint *attribList)
{
    CHECK_PARAM_NULLPTR_RET(createSync_, EGL_NO_SYNC_KHR);
    return createSync_(display_, type, attribList);
}

EGLint EglManager::EglWaitSync(EGLSyncKHR sync, EGLint flags)
{
    CHECK_PARAM_NULLPTR_RET(waitSync_, EGL_FALSE);
    return waitSync_(display_, sync, flags);
}

EGLint EglManager::EglClientWaitSync(EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout)
{
    CHECK_PARAM_NULLPTR_RET(clientWaitSync_, EGL_FALSE);
    return clientWaitSync_(display_, sync, flags, timeout);
}

EGLBoolean EglManager::EglDestroySync(EGLSyncKHR sync)
{
    CHECK_PARAM_NULLPTR_RET(destroySync_, EGL_FALSE);
    return destroySync_(display_, sync);
}

EGLint EglManager::EglDupNativeFenceFd(EGLSyncKHR sync)
{
    CHECK_PARAM_NULLPTR_RET(dupNativeFenceFd_, EGL_DONT_CARE);
    return dupNativeFenceFd_(display_, sync);
}

EGLBoolean EglManager::EglMakeCurrent()
{
    CHECK_NOT_INIT(EGL_FALSE);
    return eglMakeCurrent(display_, eglSurface_, eglSurface_, context_);
}

EGLBoolean EglManager::EglMakeCurrent(EGLSurface draw, EGLSurface read)
{
    CHECK_NOT_INIT(EGL_FALSE);
    return eglMakeCurrent(display_, draw, read, context_);
}

EGLBoolean EglManager::EglMakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx)
{
    CHECK_NOT_INIT(EGL_FALSE);
    return eglMakeCurrent(display_, draw, read, ctx);
}

} // namespace OHOS
