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

#include "surface_buffer_impl_refactor.h"

#include <mutex>

#include <message_parcel.h>
#include <securec.h>
#include <sys/mman.h>

#include "buffer_extra_data_impl.h"
#include "buffer_log.h"

#define CHECK_INIT() \
    do { \
        if (displayGralloc_ == nullptr) { \
            GSError ret = InitDisplayGralloc(); \
            if (ret != GSERROR_OK) { \
                return ret; \
            } \
        } \
    } while (0)
#define CHECK_HANDLE() \
    do { \
        if (handle_ == nullptr) { \
            return GSERROR_INVALID_ARGUMENTS; \
        } \
    } while (0)
namespace OHOS {
namespace {
GSError GenerateError(GSError err, DispErrCode code)
{
    switch (code) {
        case DISPLAY_SUCCESS: return static_cast<GSError>(err + 0);
        case DISPLAY_FAILURE: return static_cast<GSError>(err + LOWERROR_FAILURE);
        case DISPLAY_FD_ERR: return static_cast<GSError>(err + EBADF);
        case DISPLAY_PARAM_ERR: return static_cast<GSError>(err + EINVAL);
        case DISPLAY_NULL_PTR: return static_cast<GSError>(err + EINVAL);
        case DISPLAY_NOT_SUPPORT: return static_cast<GSError>(err + EOPNOTSUPP);
        case DISPLAY_NOMEM: return static_cast<GSError>(err + ENOMEM);
        case DISPLAY_SYS_BUSY: return static_cast<GSError>(err + EBUSY);
        case DISPLAY_NOT_PERM: return static_cast<GSError>(err + EPERM);
        default: break;
    }
    return static_cast<GSError>(err + LOWERROR_INVALID);
}

inline GSError GenerateError(GSError err, int32_t code)
{
    return GenerateError(err, static_cast<DispErrCode>(code));
}
}
SurfaceBufferImplRefactor::SurfaceBufferImplRefactor()
{
    {
        static std::mutex mutex;
        mutex.lock();

        static int sequence_numbe = 0;
        sequenceNumber_ = sequence_numbe++;

        mutex.unlock();
    }
    bedata_ = new BufferExtraDataImpl;
    handle_ = nullptr;
    eglData_ = nullptr;
    BLOGD("ctor +[%{public}d]", sequenceNumber_);
}

SurfaceBufferImplRefactor::SurfaceBufferImplRefactor(int seqNum)
{
    sequenceNumber_ = seqNum;
    handle_ = nullptr;
    eglData_ = nullptr;
    BLOGD("ctor =[%{public}d]", sequenceNumber_);
}

SurfaceBufferImplRefactor::~SurfaceBufferImplRefactor()
{
    BLOGD("dtor ~[%{public}d] handle_ %{public}p", sequenceNumber_, handle_);
    FreeBufferHandleLocked();
    eglData_ = nullptr;
    displayGralloc_ = nullptr;
}

SurfaceBufferImplRefactor *SurfaceBufferImplRefactor::FromBase(const sptr<SurfaceBuffer>& buffer)
{
    return static_cast<SurfaceBufferImplRefactor*>(buffer.GetRefPtr());
}

GSError SurfaceBufferImplRefactor::InitDisplayGralloc()
{
    if (displayGralloc_ != nullptr) {
        BLOGD("BufferManager has been initialized successfully.");
        return GSERROR_OK;
    }

    displayGralloc_.reset(::OHOS::HDI::Display::V1_0::IDisplayGralloc::Get());
    if (displayGralloc_ == nullptr) {
        BLOGE("IDisplayGralloc::Get return nullptr.");
        return GSERROR_INTERNEL;
    }
    return GSERROR_OK;
}

GSError SurfaceBufferImplRefactor::Alloc(const BufferRequestConfig &config)
{
    CHECK_INIT();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ != nullptr) {
            FreeBufferHandleLocked();
        }
    }

    BufferHandle *handle = nullptr;
    int32_t allocWidth = config.width;
    int32_t allocHeight = config.height;
    AllocInfo info = {allocWidth, allocHeight, config.usage, (PixelFormat)config.format};
    auto dret = displayGralloc_->AllocMem(info, handle);
    if (dret == DISPLAY_SUCCESS) {
        std::lock_guard<std::mutex> lock(mutex_);
        surfaceBufferColorGamut_ = config.colorGamut;
        transform_ = config.transform;
        handle_ = handle;
        BLOGI("buffer handle %{public}p w: %{public}d h: %{public}d t: %{public}d", handle_,
            allocWidth, allocHeight, config.transform);
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}
GSError SurfaceBufferImplRefactor::Map()
{
    CHECK_INIT();

    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        CHECK_HANDLE();
        handle = handle_;
    }
    void *virAddr = displayGralloc_->Mmap(*handle);
    if (virAddr == nullptr || virAddr == MAP_FAILED) {
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}
GSError SurfaceBufferImplRefactor::Unmap()
{
    CHECK_INIT();

    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        CHECK_HANDLE();
        handle = handle_;
    }

    auto dret = displayGralloc_->Unmap(*handle);
    if (dret == DISPLAY_SUCCESS) {
        handle_->virAddr = nullptr;
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}
GSError SurfaceBufferImplRefactor::FlushCache()
{
    CHECK_INIT();

    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        CHECK_HANDLE();
        handle = handle_;
    }

    if (handle->virAddr == nullptr) {
        return GSERROR_API_FAILED;
    }

    auto dret = displayGralloc_->FlushCache(*handle);
    if (dret == DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}
GSError SurfaceBufferImplRefactor::InvalidateCache()
{
    CHECK_INIT();

    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        CHECK_HANDLE();
        handle = handle_;
    }

    auto dret = displayGralloc_->InvalidateCache(*handle);
    if (dret == DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

void SurfaceBufferImplRefactor::FreeBufferHandleLocked()
{
    if (handle_) {
        if (handle_->virAddr != nullptr) {
            BLOGD("FreeBufferHandle [%{public}d] virAddr %{public}p", sequenceNumber_, handle_->virAddr);
            displayGralloc_->Unmap(*handle_);
        }
        FreeBufferHandle(handle_);
    }
    handle_ = nullptr;
}

BufferHandle *SurfaceBufferImplRefactor::GetBufferHandle() const
{
    return handle_;
}

GSError SurfaceBufferImplRefactor::SetSurfaceBufferWidth(int32_t width)
{
    //surfaceBufferWidth_ = width;
    return GSERROR_OK;
}

GSError SurfaceBufferImplRefactor::SetSurfaceBufferHeight(int32_t height)
{
    //surfaceBufferHeight_ = height;
    return GSERROR_OK;
}

int32_t SurfaceBufferImplRefactor::GetSurfaceBufferHeight() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_HANDLE();
    return handle_->height;
}

int32_t SurfaceBufferImplRefactor::GetSurfaceBufferWidth() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_HANDLE();
    return handle_->width;
}

GSError SurfaceBufferImplRefactor::SetSurfaceBufferColorGamut(ColorGamut colorGamut)
{
    surfaceBufferColorGamut_ = colorGamut;
    return GSERROR_OK;
}

ColorGamut SurfaceBufferImplRefactor::GetSurfaceBufferColorGamut() const
{
    return surfaceBufferColorGamut_;
}

GSError SurfaceBufferImplRefactor::SetSurfaceBufferTransform(TransformType transform)
{
    transform_ = transform;
    return GSERROR_OK;
}

TransformType SurfaceBufferImplRefactor::GetSurfaceBufferTransform() const
{
    return transform_;
}

int32_t SurfaceBufferImplRefactor::GetWidth() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->width;
}

int32_t SurfaceBufferImplRefactor::GetHeight() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->height;
}

int32_t SurfaceBufferImplRefactor::GetStride() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->stride;
}

int32_t SurfaceBufferImplRefactor::GetFormat() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->format;
}

int64_t SurfaceBufferImplRefactor::GetUsage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->usage;
}

uint64_t SurfaceBufferImplRefactor::GetPhyAddr() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return 0;
    }
    return handle_->phyAddr;
}

int32_t SurfaceBufferImplRefactor::GetKey() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->key;
}

void *SurfaceBufferImplRefactor::GetVirAddr() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return nullptr;
    }
    return handle_->virAddr;
}

int32_t SurfaceBufferImplRefactor::GetFileDescriptor() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->fd;
}

uint32_t SurfaceBufferImplRefactor::GetSize() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return 0;
    }
    return handle_->size;
}

void SurfaceBufferImplRefactor::SetExtraData(const sptr<BufferExtraData> &bedata)
{
    bedata_ = bedata;
}

void SurfaceBufferImplRefactor::GetExtraData(sptr<BufferExtraData> &bedata) const
{
    bedata = bedata_;
}

void SurfaceBufferImplRefactor::SetBufferHandle(BufferHandle *handle)
{
    handle_ = handle;
}

void SurfaceBufferImplRefactor::WriteToMessageParcel(MessageParcel &parcel)
{
    if (handle_ == nullptr) {
        BLOGE("Failure, Reason: handle_ is nullptr");
        return;
    }

    bool ret = WriteBufferHandle(parcel, *handle_);
    if (ret == false) {
        BLOGE("Failure, Reason: WriteBufferHandle return false");
    }
}

int32_t SurfaceBufferImplRefactor::GetSeqNum() const
{
    return sequenceNumber_;
}

sptr<EglData> SurfaceBufferImplRefactor::GetEglData() const
{
    return eglData_;
}

void SurfaceBufferImplRefactor::SetEglData(const sptr<EglData>& data)
{
    eglData_ = data;
}
} // namespace OHOS
