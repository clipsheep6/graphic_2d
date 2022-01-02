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

#include "egl_data_impl.h"

#include "buffer_log.h"
#include <display_gralloc.h>
#include <drm_fourcc.h>
#include "window.h"
namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "EglDataImpl" };
} // namespace

EglDataImpl::EglDataImpl()
{
    BLOGD("ctor");
    sEglManager_ = EglManager::GetInstance();
    if (sEglManager_ == nullptr) {
        BLOGE("EglManager::GetInstance Failed.");
    }
}

EglDataImpl::~EglDataImpl()
{
    BLOGD("dtor");
    if (glFbo_) {
        glDeleteFramebuffers(1, &glFbo_);
    }

    if (glTexture_) {
        glDeleteTextures(1, &glTexture_);
    }

    if (eglImage_ && sEglManager_) {
        sEglManager_->EglDestroyImage(eglImage_);
    }
    sEglManager_ = nullptr;
}

GLuint EglDataImpl::GetFrameBufferObj() const
{
    BLOGD("glFbo_:%{public}d", glFbo_);
    return glFbo_;
}

SurfaceError EglDataImpl::CreateEglData(sptr<SurfaceBufferImpl> &buffer)
{
    struct NativeWindowBuffer* nBuffer =  CreateNativeWindowBufferFromSurfaceBuffer(&buffer);

    eglImage_ = sEglManager_->EglCreateImage(EGL_NO_CONTEXT, EGL_NATIV_BUFFER_OHOS, nBuffer, NULL);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        BLOGE("##createImage failed.");
        return SURFACE_ERROR_ERROR;
    }
    sEglManager_->EglMakeCurrent();

    glGenTextures(1, &glTexture_);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, glTexture_);

    sEglManager_->EglImageTargetTexture2D(GL_TEXTURE_EXTERNAL_OES, eglImage_);

    glGenFramebuffers(1, &glFbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, glFbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_EXTERNAL_OES, glTexture_, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        BLOGE("FBO creation failed");
        return SURFACE_ERROR_ERROR;
    }
    return SURFACE_ERROR_OK;
}

} // namespace OHOS
