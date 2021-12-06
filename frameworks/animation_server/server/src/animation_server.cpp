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

#include "animation_server.h"

#include <cassert>
#include <chrono>
#include <fstream>
#include <sys/time.h>
#include <securec.h>

#include <graphic_bytrace.h>
#include <gslogger.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("AnimationServer");
} // namespace

GSError AnimationServer::Init()
{
    handler = AppExecFwk::EventHandler::Current();
    vhelper = VsyncHelper::FromHandler(handler);
    auto wm = WindowManager::GetInstance();
    auto wret = wm->Init();
    if (wret != WM_OK) {
        GSLOG2HI(ERROR) << "WindowManager::Init failed: " << WMErrorStr(wret);
        return static_cast<enum GSError>(wret);
    }

    WindowManagerServiceClient::GetInstance()->Init();
    WindowManagerServiceClient::GetInstance()->GetService()->OnAdjacentModeChange(this);

    auto splitOption = WindowOption::Get();
    splitOption->SetWindowType(WINDOW_TYPE_NORMAL);
    wret = wm->CreateWindow(splitWindow, splitOption);
    if (wret != WM_OK || splitWindow == nullptr) {
        GSLOG2HI(ERROR) << "WindowManager::CreateWindow failed: " << WMErrorStr(wret);
        return static_cast<enum GSError>(wret);
    }
    splitWindow->Hide();

    auto option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_ANIMATION);
    wret = wm->CreateWindow(window, option);
    if (wret != WM_OK || window == nullptr) {
        GSLOG2HI(ERROR) << "WindowManager::CreateWindow failed: " << WMErrorStr(wret);
        return static_cast<enum GSError>(wret);
    }

    window->Hide();
    auto producer = window->GetProducer();
    eglSurface = EglRenderSurface::CreateEglSurfaceAsProducer(producer);
    return GSERROR_OK;
}

GSError AnimationServer::StartRotationAnimation(int32_t did, int32_t degree)
{
    if (isAnimationRunning == false) {
        struct Animation animation {
            .degree = degree,
            .retval = new Promise<GSError>(),
        };
        handler->PostTask(std::bind(&AnimationServer::StartAnimation, this, std::ref(animation)));
        return animation.retval->Await();
    }
    return GSERROR_ANIMATION_RUNNING;
}

GSError AnimationServer::SplitModeCreateBackground()
{
    SplitWindowUpdate();
}

GSError AnimationServer::SplitModeCreateMiddleLine()
{
    SplitWindowUpdate(50);
}

void AnimationServer::StartAnimation(struct Animation &animation)
{
    if (isAnimationRunning) {
        animation.retval->Resolve(GSERROR_ANIMATION_RUNNING);
        return;
    }

    ScopedBytrace(__func__);
    isAnimationRunning = true;
    GSLOG2HI(INFO) << "Animation Start";
    window->Hide();
    auto wm = WindowManager::GetInstance();
    screenshotPromise = new Promise<struct AnimationScreenshotInfo>();
    wm->ListenNextScreenShot(0, this);
    auto asinfo = screenshotPromise->Await();
    if (asinfo.wmimage.wret) {
        GSLOG2HI(ERROR) << "OnScreenShot failed: " << WMErrorStr(asinfo.wmimage.wret);
        animation.retval->Resolve(static_cast<enum GSError>(asinfo.wmimage.wret));
        isAnimationRunning = false;
        return;
    }

    assert(asinfo.wmimage.width == window->GetWidth());
    assert(asinfo.wmimage.height == window->GetHeight());
    window->Show();

    auto sret = eglSurface->InitContext();
    if (sret != SURFACE_ERROR_OK) {
        GSLOG2HI(ERROR) << "EGLSurface InitContext failed: " << sret;
        animation.retval->Resolve(static_cast<enum GSError>(sret));
        isAnimationRunning = false;
        return;
    }

    ranimation = std::make_unique<RotationAnimation>();
    constexpr int32_t rotationAnimationDuration = 500;
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    struct RotationAnimationParam param = {
        .data = asinfo.ptr,
        .width = window->GetWidth(),
        .height = window->GetHeight(),
        .startTime = now,
        .duration = rotationAnimationDuration,
        .degree = animation.degree,
    };
    auto gret = ranimation->Init(param);
    if (gret != GSERROR_OK) {
        GSLOG2HI(ERROR) << "RotationAnimation Init failed: " << GSErrorStr(gret);
        animation.retval->Resolve(gret);
        isAnimationRunning = false;
        return;
    }

    animation.retval->Resolve(static_cast<enum GSError>(RequestNextVsync()));
}

void AnimationServer::AnimationSync(int64_t time, void *data)
{
    ScopedBytrace sb(__func__);
    if (ranimation->Draw()) {
        eglSurface->SwapBuffers();
        RequestNextVsync();
        sb.End();
    } else {
        sb.End();
        GSLOG2HI(INFO) << "Animation End";
        window->Hide();
        isAnimationRunning = false;
    }
}

void AnimationServer::SplitWindowUpdate(int32_t midline)
{
    sptr<SurfaceBuffer> buffer;
    BufferRequestConfig rconfig = {
        .width = surface->GetDefaultWidth(),
        .height = surface->GetDefaultHeight(),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = surface->GetDefaultUsage(),
        .timeout = 0,
    };
    if (data != nullptr) {
        rconfig = *reinterpret_cast<BufferRequestConfig *>(data);
    }

    SurfaceError ret = surface->RequestBufferNoFence(buffer, rconfig);
    if (ret == SURFACE_ERROR_NO_BUFFER) {
        return;
    } else if (ret != SURFACE_ERROR_OK || buffer == nullptr) {
        return;
    }

    auto addr = static_cast<uint32_t *>(buffer->GetVirAddr());
    if (addr == nullptr) {
        surface->CancelBuffer(buffer);
        return;
    }

    for (uint32_t j = 0; j < rconfig.height; j++) {
        constexpr int32_t percent = 100;
        int32_t diff = j - rconfig.height * midline / percent;
        if (diff < 0) {
            diff = -diff;
        }

        if (diff < rconfig.height / percent / 0x2) {
            for (uint32_t i = 0; i < rconfig.width; i++) {
                addr[j * rconfig.width + i] = 0xffaaaaaa;
            }
        }
        for (uint32_t i = 0; i < rconfig.width; i++) {
            addr[j * rconfig.width + i] = 0xff000000;
        }
    }

    BufferFlushConfig fconfig = {
        .damage = {
            .w = rconfig.width,
            .h = rconfig.height,
        },
    };
    surface->FlushBuffer(buffer, -1, fconfig);
}

VsyncError AnimationServer::RequestNextVsync()
{
    struct FrameCallback cb = {
        .callback_ = std::bind(&AnimationServer::AnimationSync, this, SYNC_FUNC_ARG),
    };
    return vhelper->RequestFrameCallback(cb);
}

void AnimationServer::OnScreenShot(const struct WMImageInfo &info)
{
    int32_t length = info.size;
    struct AnimationScreenshotInfo ainfo = {
        .wmimage = info,
        .ptr = nullptr,
    };
    if (info.wret != WM_OK) {
        screenshotPromise->Resolve(ainfo);
        return;
    }

    ainfo.ptr = std::make_shared<Array>();
    ainfo.ptr->ptr = std::make_unique<uint8_t[]>(length);
    ainfo.wmimage.data = ainfo.ptr.get();
    if (memcpy_s(ainfo.ptr->ptr.get(), length, info.data, info.size) != EOK) {
        GSLOG2HI(ERROR) << "memcpy_s failed: " << strerror(errno);
        ainfo.wmimage.wret = static_cast<enum WMError>(GSERROR_INTERNEL);
        screenshotPromise->Resolve(ainfo);
        return;
    }

    screenshotPromise->Resolve(ainfo);
}

void AnimationServer::OnAdjacentModeChange(int32_t wid, int32_t x, int32_t y, int32_t width, int32_t height, AdjacentModeStatus status)
{
    if (status ==  ADJACENT_MODE_STATUS_DESTROY) {
        splitWindow->Hide();
    }
}
} // namespace OHOS
