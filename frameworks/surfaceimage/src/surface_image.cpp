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

#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#include "surface_image.h"

#include "securec.h"
#include "sandbox_utils.h"
#include "surface_utils.h"

#include <atomic>
#include <sync_fence.h>
#include <unistd.h>
#include <window.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

namespace OHOS {
namespace {
// Get a uniqueID in a process
static int GetProcessUniqueId()
{
    static std::atomic<int> g_counter { 0 };
    return g_counter.fetch_add(1);
}
}

SurfaceImage::SurfaceImage(uint32_t textureId, uint32_t textureTarget)
    : ConsumerSurface("SurfaceImage-" + std::to_string(GetRealPid()) + "-" + std::to_string(GetProcessUniqueId())),
      textureId_(textureId),
      textureTarget_(textureTarget),
      updateSurfaceImage_(false),
      eglDisplay_(EGL_NO_DISPLAY),
      eglContext_(EGL_NO_CONTEXT),
      currentSurfaceImage_(0),
      currentSurfaceBuffer_(nullptr),
      currentSurfaceBufferFence_(-1), // -1 invalid
      currentTimeStamp_(0)
{
    InitSurfaceImage();
}

SurfaceImage::~SurfaceImage()
{
    for (auto it = imageCacheSeqs_.begin(); it != imageCacheSeqs_.end(); it++) {
        if (it->second.eglImage_ != EGL_NO_IMAGE_KHR) {
            eglDestroyImageKHR(eglDisplay_, it->second.eglImage_);
            it->second.eglImage_ = EGL_NO_IMAGE_KHR;
        }
    }
}

void SurfaceImage::InitSurfaceImage()
{
    std::string name = "SurfaceImage-" + std::to_string(GetRealPid()) + "-" + std::to_string(GetProcessUniqueId());
    auto ret = ConsumerSurface::Init();
    BLOGI("surfaceimage init");
    if (ret != SURFACE_ERROR_OK) {
        BLOGE("init surfaceimage failed");
    }
    surfaceImageName_ = name;
}

SurfaceError SurfaceImage::SetDefaultSize(int32_t width, int32_t height)
{
    return ConsumerSurface::SetDefaultWidthAndHeight(width, height);
}

void SurfaceImage::UpdateSurfaceInfo(uint32_t seqNum, sptr<SurfaceBuffer> buffer, int32_t fence,
                                     int64_t timestamp, Rect damage)
{
    currentSurfaceImage_ = seqNum;
    currentSurfaceBuffer_ = buffer;
    currentSurfaceBufferFence_ = fence;
    currentTimeStamp_ = timestamp;
    currentCrop_ = damage;
    currentTransformType_ = ConsumerSurface::GetTransform();
}

SurfaceError SurfaceImage::UpdateSurfaceImage()
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);

    // validate egl state
    SurfaceError ret = ValidateEglState();
    if (ret != SURFACE_ERROR_OK) {
        return ret;
    }

    // acquire buffer
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence;
    int64_t timestamp;
    Rect damage;

    ret = AcquireBuffer(buffer, fence, timestamp, damage);
    if (ret != SURFACE_ERROR_OK) {
        if (ret == SURFACE_ERROR_NO_BUFFER) {
            glBindTexture(textureTarget_, textureId_);
            BLOGE("AcquireBuffer no buffer");
        } else {
            BLOGE("AcquireBuffer failed");
        }
        return ret;
    }

    uint32_t seqNum = buffer->GetSeqNum();
    BLOGD("seqNum %{public}d", seqNum);
    EGLImageKHR img = imageCacheSeqs_[seqNum].eglImage_;
    glBindTexture(textureTarget_, textureId_);
    glEGLImageTargetTexture2DOES(textureTarget_, static_cast<GLeglImageOES>(img));

    while (glGetError() != GL_NO_ERROR) {
        BLOGE("glEGLImageTargetTexture2DOES error");
        ret = SURFACE_ERROR_API_FAILED;
    }

    if (ret == SURFACE_ERROR_OK) {
        ret = WaitReleaseEGLSync(eglDisplay_);
    }

    if (ret != SURFACE_ERROR_OK) {
        ReleaseBuffer(buffer, -1);
        return ret;
    }
    
    if (seqNum != currentSurfaceImage_ && currentSurfaceBuffer_ != nullptr) {
        ret = ReleaseBuffer(currentSurfaceBuffer_, -1);
        if (ret != SURFACE_ERROR_OK) {
            BLOGE("release currentSurfaceBuffer_ failed %{public}d", ret);
        }
    }
    UpdateSurfaceInfo(seqNum, buffer, fence, timestamp, damage);
    auto utils = SurfaceUtils::GetInstance();
    utils->ComputeTransformMatrix(currentTransformMatrix_, TRANSFORM_MATRIX_ELE_COUNT,
        currentSurfaceBuffer_, currentTransformType_, currentCrop_);

    ret = WaitOnFence();
    return ret;
}

SurfaceError SurfaceImage::AttachContext(uint32_t textureId)
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    if (isAttached) {
        BLOGI("SurfaceImage is already attached");
        return SURFACE_ERROR_OK;
    }
    if (imageCacheSeqs_[currentSurfaceImage_].eglImage_ == EGL_NO_IMAGE_KHR) {
        BLOGE("AttachContext failed, no eglImage");
        return SURFACE_ERROR_ERROR;
    }

    EGLImageKHR img = imageCacheSeqs_[currentSurfaceImage_].eglImage_;
    EGLDisplay disp = eglGetCurrentDisplay();
    EGLContext context = eglGetCurrentContext();
    if (disp == EGL_NO_DISPLAY || context == EGL_NO_CONTEXT) {
        BLOGE("AttachContext failed, EGLDisplay or EGLContext is invalid");
        return SURFACE_ERROR_INIT;
    }

    glBindTexture(textureTarget_, textureId);
    textureId_ = textureId;
    glEGLImageTargetTexture2DOES(textureTarget_, static_cast<GLeglImageOES>(img));
    eglDisplay_ = disp;
    eglContext_ = context;

    isAttached = true;
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::DetachContext()
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    if (!isAttached) {
        BLOGI("SurfaceImage is already detached");
        return SURFACE_ERROR_OK;
    }
    EGLDisplay disp = eglGetCurrentDisplay();
    EGLContext context = eglGetCurrentContext();

    if ((eglDisplay_ != disp && eglDisplay_ != EGL_NO_DISPLAY) || (disp == EGL_NO_DISPLAY)) {
        BLOGE("EGLDisplay is invalid, errno : 0x%{public}x", eglGetError());
        return SURFACE_ERROR_INIT;
    }
    if ((eglContext_ != context && eglContext_ != EGL_NO_CONTEXT) || (context == EGL_NO_CONTEXT)) {
        BLOGE("EGLContext is invalid, errno : 0x%{public}x", eglGetError());
        return SURFACE_ERROR_INIT;
    }

    SurfaceError ret = WaitReleaseEGLSync(eglDisplay_);
    if (ret == SURFACE_ERROR_OK) {
        glDeleteTextures(1, &textureId_);
    } else {
        return ret;
    }

    eglDisplay_ = EGL_NO_DISPLAY;
    eglContext_ = EGL_NO_CONTEXT;

    isAttached = false;
    return SURFACE_ERROR_OK;
}

int64_t SurfaceImage::GetTimeStamp()
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    return currentTimeStamp_;
}

SurfaceError SurfaceImage::GetTransformMatrix(float matrix[16])
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    auto ret = memcpy_s(matrix, sizeof(currentTransformMatrix_),
                        currentTransformMatrix_, sizeof(currentTransformMatrix_));
    if (ret != EOK) {
        BLOGE("GetTransformMatrix: currentTransformMatrix_ memcpy_s failed");
        return SURFACE_ERROR_ERROR;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
                                         int64_t &timestamp, Rect &damage)
{
    SurfaceError ret = ConsumerSurface::AcquireBuffer(buffer, fence, timestamp, damage);
    if (ret != SURFACE_ERROR_OK) {
        BLOGE("AcquireBuffer error");
        return ret;
    }
    // get seq num
    uint32_t seqNum = buffer->GetSeqNum();

    if (buffer != nullptr) {
        if (imageCacheSeqs_[seqNum].eglImage_ != EGL_NO_IMAGE_KHR) {
            eglDestroyImageKHR(eglDisplay_, imageCacheSeqs_[seqNum].eglImage_);
            imageCacheSeqs_[seqNum].eglImage_ = EGL_NO_IMAGE_KHR;
        }
    }

    // create image
    if (imageCacheSeqs_[seqNum].eglImage_ == EGL_NO_IMAGE_KHR) {
        EGLImageKHR eglImage = CreateEGLImage(eglDisplay_, buffer);
        if (eglImage == EGL_NO_IMAGE_KHR) {
            return SURFACE_ERROR_INIT;
        }
        imageCacheSeqs_[seqNum].eglImage_ = eglImage;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence)
{
    SurfaceError error = ConsumerSurface::ReleaseBuffer(buffer, fence);
    if (error != SURFACE_ERROR_OK) {
        BLOGE("ReleaseBuffer error");
        return error;
    }
    uint32_t seqNum = buffer->GetSeqNum();

    imageCacheSeqs_[seqNum].eglSync_ = EGL_NO_SYNC_KHR;
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::ValidateEglState()
{
    EGLDisplay disp = eglGetCurrentDisplay();
    EGLContext context = eglGetCurrentContext();

    if ((eglDisplay_ != disp && eglDisplay_ != EGL_NO_DISPLAY) || (disp == EGL_NO_DISPLAY)) {
        BLOGE("EGLDisplay is invalid, errno : 0x%{public}x", eglGetError());
        return SURFACE_ERROR_INIT;
    }
    if ((eglContext_ != context && eglContext_ != EGL_NO_CONTEXT) || (context == EGL_NO_CONTEXT)) {
        BLOGE("EGLContext is invalid, errno : 0x%{public}x", eglGetError());
        return SURFACE_ERROR_INIT;
    }

    eglDisplay_ = disp;
    eglContext_ = context;
    return SURFACE_ERROR_OK;
}

EGLImageKHR SurfaceImage::CreateEGLImage(EGLDisplay disp, const sptr<SurfaceBuffer>& buffer)
{
    sptr<SurfaceBuffer> bufferImpl = buffer;
    NativeWindowBuffer* nBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&bufferImpl);
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED,
        EGL_TRUE,
        EGL_NONE,
    };

    EGLImageKHR img = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nBuffer, attrs);
    if (img == EGL_NO_IMAGE_KHR) {
        EGLint error = eglGetError();
        BLOGE("failed, error %{public}d", error);
        eglTerminate(disp);
    }
    DestroyNativeWindowBuffer(nBuffer);
    return img;
}

SurfaceError SurfaceImage::WaitReleaseEGLSync(EGLDisplay disp)
{
    // check fence extension
    EGLSyncKHR fence = imageCacheSeqs_[currentSurfaceImage_].eglSync_;
    if (fence != EGL_NO_SYNC_KHR) {
        EGLint ret = eglClientWaitSyncKHR(disp, fence, 0, 1000000000);
        if (ret == EGL_FALSE) {
            BLOGE("eglClientWaitSyncKHR error 0x%{public}x", eglGetError());
            return SURFACE_ERROR_ERROR;
        } else if (ret == EGL_TIMEOUT_EXPIRED_KHR) {
            BLOGE("eglClientWaitSyncKHR timeout");
            return SURFACE_ERROR_ERROR;
        }
        eglDestroySyncKHR(disp, fence);
    }
    fence = eglCreateSyncKHR(disp, EGL_SYNC_FENCE_KHR, NULL);
    if (fence == EGL_NO_SYNC_KHR) {
        BLOGE("eglCreateSyncKHR error 0x%{public}x", eglGetError());
        return SURFACE_ERROR_ERROR;
    }
    glFlush();
    imageCacheSeqs_[currentSurfaceImage_].eglSync_ = fence;
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::WaitOnFence()
{
    // check ret error code
    EGLDisplay disp = eglGetCurrentDisplay();
    EGLContext context = eglGetCurrentContext();
    if ((eglDisplay_ != disp && eglDisplay_ != EGL_NO_DISPLAY) || (disp == EGL_NO_DISPLAY)) {
        BLOGE("EGLDisplay is invalid, errno : 0x%{public}x", eglGetError());
        return SURFACE_ERROR_INIT;
    }
    if ((eglContext_ != context && eglContext_ != EGL_NO_CONTEXT) || (context == EGL_NO_CONTEXT)) {
        BLOGE("EGLContext is invalid, errno : 0x%{public}x", eglGetError());
        return SURFACE_ERROR_INIT;
    }

    // check EGL_FENCE_KHR
    if (currentSurfaceBufferFence_ != -1) {
        BLOGE("currentSurfaceBufferFence_ fd - %{public}d", currentSurfaceBufferFence_);
        sptr<SyncFence> fence = new SyncFence(currentSurfaceBufferFence_);
        fence->Wait(-1);
    }
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::SetOnBufferAvailableListener(void *context, OnBufferAvailableListener listener)
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    if (listener == nullptr) {
        BLOGE("listener is nullptr");
        return SURFACE_ERROR_ERROR;
    }

    listener_ = listener;
    context_ = context;
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceImage::UnsetOnBufferAvailableListener()
{
    std::lock_guard<std::mutex> lockGuard(opMutex_);
    listener_ = nullptr;
    context_ = nullptr;
    return SURFACE_ERROR_OK;
}

SurfaceImageListener::~SurfaceImageListener()
{
    BLOGE("~SurfaceImageListener");
    surfaceImage_ = nullptr;
}

void SurfaceImageListener::OnBufferAvailable()
{
    BLOGD("SurfaceImageListener::OnBufferAvailable");
    auto surfaceImage = surfaceImage_.promote();
    if (surfaceImage == nullptr) {
        BLOGE("surfaceImage promote failed");
        return;
    }

    // check here maybe a messagequeue, flag instead now
    surfaceImage->OnUpdateBufferAvailableState(true);
    if (surfaceImage->listener_ != nullptr) {
        surfaceImage->listener_(surfaceImage->context_);
    }
}
} // namespace OHOS
