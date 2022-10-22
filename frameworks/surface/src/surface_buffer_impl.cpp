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

#include "surface_buffer_impl.h"

#include <mutex>

#include <message_parcel.h>
#include <securec.h>
#include <sys/mman.h>
#include "buffer_log.h"
#include "buffer_manager.h"
#include "buffer_extra_data_impl.h"
#include "native_buffer.h"
#include "v1_0/include/idisplay_buffer.h"

namespace OHOS {
namespace {
GSError GenerateError(GSError err, GraphicDispErrCode code)
{
    switch (code) {
        case GRAPHIC_DISPLAY_SUCCESS: return static_cast<GSError>(err + 0);
        case GRAPHIC_DISPLAY_FAILURE: return static_cast<GSError>(err + LOWERROR_FAILURE);
        case GRAPHIC_DISPLAY_FD_ERR: return static_cast<GSError>(err + EBADF);
        case GRAPHIC_DISPLAY_PARAM_ERR: return static_cast<GSError>(err + EINVAL);
        case GRAPHIC_DISPLAY_NULL_PTR: return static_cast<GSError>(err + EINVAL);
        case GRAPHIC_DISPLAY_NOT_SUPPORT: return static_cast<GSError>(err + EOPNOTSUPP);
        case GRAPHIC_DISPLAY_NOMEM: return static_cast<GSError>(err + ENOMEM);
        case GRAPHIC_DISPLAY_SYS_BUSY: return static_cast<GSError>(err + EBUSY);
        case GRAPHIC_DISPLAY_NOT_PERM: return static_cast<GSError>(err + EPERM);
        default: break;
    }
    return static_cast<GSError>(err + LOWERROR_INVALID);
}

inline GSError GenerateError(GSError err, int32_t code)
{
    return GenerateError(err, static_cast<GraphicDispErrCode>(code));
}

using namespace OHOS::HDI::Display::Buffer::V1_0;
using IDisplayBufferSptr = std::shared_ptr<IDisplayBuffer>;
static IDisplayBufferSptr displayBuffer_ = nullptr;                                         
IDisplayBufferSptr GetDisplayBuffer()
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    if (displayBuffer_ != nullptr) {
        return displayBuffer_;
    }

    displayBuffer_.reset(IDisplayBuffer::Get());
    if (displayBuffer_ == nullptr) {
        BLOGE("IDisplayGralloc::Get return nullptr.");
        return nullptr;
    }
    return displayBuffer_;
}

}


SurfaceBufferImpl::SurfaceBufferImpl()
{
    {
        static std::mutex mutex;
        mutex.lock();

        static uint32_t sequence_number_ = 0;
        sequenceNumber_ = sequence_number_++;

        mutex.unlock();
    }
    bedata_ = new BufferExtraDataImpl;
    BLOGD("ctor +[%{public}u]", sequenceNumber_);
}

SurfaceBufferImpl::SurfaceBufferImpl(uint32_t seqNum)
{
    sequenceNumber_ = seqNum;
    bedata_ = new BufferExtraDataImpl;
    BLOGD("ctor =[%{public}u]", sequenceNumber_);
}

SurfaceBufferImpl::~SurfaceBufferImpl()
{
    BLOGD("dtor ~[%{public}u] handle_ %{public}p", sequenceNumber_, handle_);
    FreeBufferHandleLocked();
}

SurfaceBufferImpl *SurfaceBufferImpl::FromBase(const sptr<SurfaceBuffer>& buffer)
{
    return static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
}

GSError SurfaceBufferImpl::Alloc(const BufferRequestConfig &config)
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
        return GSERROR_INTERNAL;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ != nullptr) {
            FreeBufferHandleLocked();
        }
    }
    
    GSError ret = CheckBufferConfig(config.width, config.height, config.format, config.usage);
    if (ret != GSERROR_OK) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    BufferHandle *handle = nullptr;
    uint64_t usage = BufferUsageToGrallocUsage(config.usage);
    
    AllocInfo info = {config.width, config.height, usage, config.format};
    auto dret = displayBuffer_->AllocMem(info, handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        std::lock_guard<std::mutex> lock(mutex_);
        surfaceBufferColorGamut_ = static_cast<GraphicColorGamut>(config.colorGamut);
        transform_ = static_cast<GraphicTransformType>(config.transform);
        surfaceBufferWidth_ = config.width;
        surfaceBufferHeight_ = config.height;
        handle_ = handle;
        BLOGD("buffer handle %{public}p w: %{public}d h: %{public}d t: %{public}d", handle_,
            handle_->width, handle_->height, config.transform);
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}
GSError SurfaceBufferImpl::Map()
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
        return GSERROR_INTERNAL;
    }

    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("handle is nullptr");
            return GSERROR_INVALID_OPERATING;
        } else if (handle_->virAddr != nullptr) {
            BLOGD("handle_->virAddr has been maped");
            return GSERROR_OK;
        }
        handle = handle_;
    }
    void *virAddr = displayBuffer_->Mmap(*handle);
    if (virAddr == nullptr || virAddr == MAP_FAILED) {
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}
GSError SurfaceBufferImpl::Unmap()
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
        return GSERROR_INTERNAL;
    }
    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("handle is nullptr");
            return GSERROR_INVALID_OPERATING;
        } else if (handle_->virAddr == nullptr) {
            BLOGW("handle has been unmaped");
            return GSERROR_OK;
        }
        handle = handle_;
    }

    auto dret = displayBuffer_->Unmap(*handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        handle_->virAddr = nullptr;
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}
GSError SurfaceBufferImpl::FlushCache()
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
        return GSERROR_INTERNAL;
    }
    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("handle is nullptr");
            return GSERROR_INVALID_OPERATING;
        }
        handle = handle_;
    }

    if (handle->virAddr == nullptr) {
        return GSERROR_API_FAILED;
    }

    auto dret = displayBuffer_->FlushCache(*handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError SurfaceBufferImpl::InvalidateCache()
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
        return GSERROR_INTERNAL;
    }
    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("handle is nullptr");
            return GSERROR_INVALID_OPERATING;
        }
        handle = handle_;
    }

    auto dret = displayBuffer_->InvalidateCache(*handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

void SurfaceBufferImpl::FreeBufferHandleLocked()
{
    if (handle_) {
        if (handle_->virAddr != nullptr) {
            displayBuffer_->Unmap(*handle_);
        }
        FreeBufferHandle(handle_);
    }
    handle_ = nullptr;
}

BufferHandle *SurfaceBufferImpl::GetBufferHandle() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return handle_;
}

void SurfaceBufferImpl::SetSurfaceBufferColorGamut(const GraphicColorGamut& colorGamut)
{
    std::lock_guard<std::mutex> lock(mutex_);
    surfaceBufferColorGamut_ = colorGamut;
}

const GraphicColorGamut& SurfaceBufferImpl::GetSurfaceBufferColorGamut() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return surfaceBufferColorGamut_;
}

void SurfaceBufferImpl::SetSurfaceBufferTransform(const GraphicTransformType& transform)
{
    std::lock_guard<std::mutex> lock(mutex_);
    transform_ = transform;
}

const GraphicTransformType& SurfaceBufferImpl::GetSurfaceBufferTransform() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return transform_;
}

int32_t SurfaceBufferImpl::GetSurfaceBufferWidth() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return surfaceBufferWidth_;
}

int32_t SurfaceBufferImpl::GetSurfaceBufferHeight() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return surfaceBufferHeight_;
}

void SurfaceBufferImpl::SetSurfaceBufferWidth(int32_t width)
{
    std::lock_guard<std::mutex> lock(mutex_);
    surfaceBufferWidth_ = width;
}

void SurfaceBufferImpl::SetSurfaceBufferHeight(int32_t height)
{
    std::lock_guard<std::mutex> lock(mutex_);
    surfaceBufferHeight_ = height;
}

int32_t SurfaceBufferImpl::GetWidth() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->width;
}

int32_t SurfaceBufferImpl::GetHeight() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->height;
}

int32_t SurfaceBufferImpl::GetStride() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->stride;
}

int32_t SurfaceBufferImpl::GetFormat() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->format;
}

int64_t SurfaceBufferImpl::GetUsage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->usage;
}

uint64_t SurfaceBufferImpl::GetPhyAddr() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return 0;
    }
    return handle_->phyAddr;
}

int32_t SurfaceBufferImpl::GetKey() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->key;
}

void *SurfaceBufferImpl::GetVirAddr()
{
    GSError ret = this->Map();
    if (ret != GSERROR_OK) {
        BLOGW("Map failed");
        return nullptr;
    }
    return handle_->virAddr;
}

int32_t SurfaceBufferImpl::GetFileDescriptor() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->fd;
}

uint32_t SurfaceBufferImpl::GetSize() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return 0;
    }
    return handle_->size;
}

void SurfaceBufferImpl::SetExtraData(const sptr<BufferExtraData> &bedata)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bedata_ = bedata;
}

const sptr<BufferExtraData>& SurfaceBufferImpl::GetExtraData() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return bedata_;
}

void SurfaceBufferImpl::SetBufferHandle(BufferHandle *handle)
{
    std::lock_guard<std::mutex> lock(mutex_);
    handle_ = handle;
}

GSError SurfaceBufferImpl::WriteToMessageParcel(MessageParcel &parcel)
{
    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("Failure, Reason: handle_ is nullptr");
            return GSERROR_NOT_INIT;
        }
        handle = handle_;
    }

    bool ret = WriteBufferHandle(parcel, *handle);
    if (ret == false) {
        BLOGE("Failure, Reason: WriteBufferHandle return false");
        return GSERROR_API_FAILED;
    }

    return GSERROR_OK;
}

GSError SurfaceBufferImpl::ReadFromMessageParcel(MessageParcel &parcel)
{
    std::lock_guard<std::mutex> lock(mutex_);
    FreeBufferHandleLocked();
    handle_ = ReadBufferHandle(parcel);
    if (handle_ == nullptr) {
        return GSERROR_API_FAILED;
    }

    return GSERROR_OK;
}

OH_NativeBuffer* SurfaceBufferImpl::SurfaceBufferToNativeBuffer()
{
    return reinterpret_cast<OH_NativeBuffer *>(this);
}

uint32_t SurfaceBufferImpl::GetSeqNum() const
{
    return sequenceNumber_;
}

sptr<EglData> SurfaceBufferImpl::GetEglData() const
{
    return eglData_;
}

void SurfaceBufferImpl::SetEglData(const sptr<EglData>& data)
{
    eglData_ = data;
}

GSError SurfaceBufferImpl::CheckBufferConfig(int32_t width, int32_t height,
                                             int32_t format, int32_t usage)
{
    if (width <= 0 || height <= 0) {
        BLOGE("width or height is greater than 0, now is w %{public}d h %{public}d", width, height);
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (format < 0 || format > GRAPHIC_PIXEL_FMT_BUTT) {
        BLOGE("format [0, %{public}d], now is %{public}d", GRAPHIC_PIXEL_FMT_BUTT, format);
        return GSERROR_INVALID_ARGUMENTS;
    }

    return GSERROR_OK;
}

uint64_t SurfaceBufferImpl::BufferUsageToGrallocUsage(uint64_t bufferUsage)
{
    uint64_t grallocUsage = 0;
    if (bufferUsage & BUFFER_USAGE_CPU_READ) {
        grallocUsage |= HBM_USE_CPU_READ;
    }
    if (bufferUsage & BUFFER_USAGE_CPU_WRITE) {
        grallocUsage |= HBM_USE_CPU_WRITE;
    }
    if (bufferUsage & BUFFER_USAGE_MEM_MMZ) {
        grallocUsage |= HBM_USE_MEM_MMZ;
    }
    if (bufferUsage & BUFFER_USAGE_MEM_DMA) {
        grallocUsage |= HBM_USE_MEM_DMA;
    }
    if (bufferUsage & BUFFER_USAGE_MEM_SHARE) {
        grallocUsage |= HBM_USE_MEM_SHARE;
    }
    if (bufferUsage & BUFFER_USAGE_MEM_MMZ_CACHE) {
        grallocUsage |= HBM_USE_MEM_MMZ_CACHE;
    }
    if (bufferUsage & BUFFER_USAGE_ASSIGN_SIZE) {
        grallocUsage |= HBM_USE_ASSIGN_SIZE;
    }
    if (bufferUsage & BUFFER_USAGE_MEM_FB) {
        grallocUsage |= HBM_USE_MEM_FB;
    }
    if (bufferUsage & BUFFER_USAGE_HW_RENDER) {
        grallocUsage |= HBM_USE_HW_RENDER;
    }
    if (bufferUsage & BUFFER_USAGE_HW_TEXTURE) {
        grallocUsage |= HBM_USE_HW_TEXTURE;
    }
    if (bufferUsage & BUFFER_USAGE_HW_COMPOSER) {
        grallocUsage |= HBM_USE_HW_COMPOSER;
    }
    if (bufferUsage & BUFFER_USAGE_PROTECTED) {
        grallocUsage |= HBM_USE_PROTECTED;
    }
    if (bufferUsage & BUFFER_USAGE_CAMERA_READ) {
        grallocUsage |= HBM_USE_CAMERA_READ;
    }
    if (bufferUsage & BUFFER_USAGE_CAMERA_WRITE) {
        grallocUsage |= HBM_USE_CAMERA_WRITE;
    }
    if (bufferUsage & BUFFER_USAGE_VIDEO_ENCODER) {
        grallocUsage |= HBM_USE_VIDEO_ENCODER;
    }
    if (bufferUsage & BUFFER_USAGE_VIDEO_DECODER) {
        grallocUsage |= HBM_USE_VIDEO_DECODER;
    }
    return grallocUsage;
}
} // namespace OHOS
