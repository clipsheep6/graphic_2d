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

#include "buffer_log.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "SurfaceBufferImpl" };
}

SurfaceBufferImpl::SurfaceBufferImpl()
{
    {
        static std::mutex mutex;
        mutex.lock();

        static int sequence_number_ = 0;
        sequenceNumber = sequence_number_++;

        mutex.unlock();
    }
    handle_ = nullptr;
    eglData_ = nullptr;
    BLOGD("ctor +[%{public}d]", sequenceNumber);
}

SurfaceBufferImpl::SurfaceBufferImpl(int seqNum)
{
    sequenceNumber = seqNum;
    handle_ = nullptr;
    BLOGD("ctor =[%{public}d]", sequenceNumber);
}

SurfaceBufferImpl::~SurfaceBufferImpl()
{
    BLOGD("dtor ~[%{public}d]", sequenceNumber);
    if (handle_) {
        FreeBufferHandle(handle_);
    }
    eglData_ = nullptr;
}

BufferHandle *SurfaceBufferImpl::GetBufferHandle() const
{
    return handle_;
}

int32_t SurfaceBufferImpl::GetWidth() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->width;
}

int32_t SurfaceBufferImpl::GetHeight() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->height;
}

int32_t SurfaceBufferImpl::GetFormat() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->format;
}

int64_t SurfaceBufferImpl::GetUsage() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->usage;
}

uint64_t SurfaceBufferImpl::GetPhyAddr() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return 0;
    }
    return handle_->phyAddr;
}

int32_t SurfaceBufferImpl::GetKey() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->key;
}

void *SurfaceBufferImpl::GetVirAddr() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return nullptr;
    }
    return handle_->virAddr;
}

int32_t SurfaceBufferImpl::GetFileDescriptor() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->fd;
}

uint32_t SurfaceBufferImpl::GetSize() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return 0;
    }
    return handle_->size;
}

SurfaceError SurfaceBufferImpl::SetInt32(uint32_t key, int32_t val)
{
    ExtraData int32 = {
        .value = val,
        .type = EXTRA_DATA_TYPE_INT32,
    };
    return SetData(key, int32);
}

SurfaceError SurfaceBufferImpl::GetInt32(uint32_t key, int32_t &val)
{
    ExtraData int32;
    SurfaceError ret = GetData(key, int32);
    if (ret == SURFACE_ERROR_OK) {
        if (int32.type == EXTRA_DATA_TYPE_INT32) {
            auto pVal = std::any_cast<int32_t>(&int32.value);
            if (pVal != nullptr) {
                val = *pVal;
            } else {
                BLOGE("unexpected: INT32, any_cast failed");
            }
        } else {
            return SURFACE_ERROR_TYPE_ERROR;
        }
    }
    return ret;
}

SurfaceError SurfaceBufferImpl::SetInt64(uint32_t key, int64_t val)
{
    ExtraData int64 = {
        .value = val,
        .type = EXTRA_DATA_TYPE_INT64,
    };
    return SetData(key, int64);
}

SurfaceError SurfaceBufferImpl::GetInt64(uint32_t key, int64_t &val)
{
    ExtraData int64;
    SurfaceError ret = GetData(key, int64);
    if (ret == SURFACE_ERROR_OK) {
        if (int64.type == EXTRA_DATA_TYPE_INT64) {
            auto pVal = std::any_cast<int64_t>(&int64.value);
            if (pVal != nullptr) {
                val = *pVal;
            } else {
                BLOGE("unexpected: INT64, any_cast failed");
            }
        } else {
            return SURFACE_ERROR_TYPE_ERROR;
        }
    }
    return ret;
}

SurfaceError SurfaceBufferImpl::SetData(uint32_t key, ExtraData data)
{
    if (data.type <= EXTRA_DATA_TYPE_MIN || data.type >= EXTRA_DATA_TYPE_MAX) {
        BLOGW("Invalid, data.type is out of range");
        return SURFACE_ERROR_INVALID_PARAM;
    }

    if (extraDatas_.size() > SURFACE_MAX_USER_DATA_COUNT) {
        BLOGW("SurfaceBuffer has too many extra data, cannot save one more!!!");
        return SURFACE_ERROR_OUT_OF_RANGE;
    }

    ExtraData mapData;
    GetData(key, mapData);

    mapData = data;

    extraDatas_[key] = mapData;
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceBufferImpl::GetData(uint32_t key, ExtraData &data)
{
    auto it = extraDatas_.find(key);
    if (it == extraDatas_.end()) {
        return SURFACE_ERROR_NO_ENTRY;
    }

    data = it->second;
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceBufferImpl::ExtraGet(std::string key, int32_t &value) const
{
    return bedata_->ExtraGet(key, value);
}

SurfaceError SurfaceBufferImpl::ExtraGet(std::string key, int64_t &value) const
{
    return bedata_->ExtraGet(key, value);
}

SurfaceError SurfaceBufferImpl::ExtraGet(std::string key, double &value) const
{
    return bedata_->ExtraGet(key, value);
}

SurfaceError SurfaceBufferImpl::ExtraGet(std::string key, std::string &value) const
{
    return bedata_->ExtraGet(key, value);
}

SurfaceError SurfaceBufferImpl::ExtraSet(std::string key, int32_t value)
{
    return bedata_->ExtraSet(key, value);
}

SurfaceError SurfaceBufferImpl::ExtraSet(std::string key, int64_t value)
{
    return bedata_->ExtraSet(key, value);
}

SurfaceError SurfaceBufferImpl::ExtraSet(std::string key, double value)
{
    return bedata_->ExtraSet(key, value);
}

SurfaceError SurfaceBufferImpl::ExtraSet(std::string key, std::string value)
{
    return bedata_->ExtraSet(key, value);
}

int32_t SurfaceBufferImpl::GetSeqNum(const sptr<SurfaceBuffer> &buffer)
{
    if (buffer == nullptr) {
        return -1;
    }
    auto bi = reinterpret_cast<const SurfaceBufferImpl *>(buffer.GetRefPtr());
    return bi->sequenceNumber;
}

void SurfaceBufferImpl::SetExtraData(sptr<SurfaceBuffer> &buffer, const std::shared_ptr<BufferExtraData> &bedata)
{
    if (buffer == nullptr) {
        return;
    }
    auto bi = reinterpret_cast<SurfaceBufferImpl *>(buffer.GetRefPtr());
    bi->bedata_ = bedata;
}

void SurfaceBufferImpl::GetExtraData(const sptr<SurfaceBuffer> &buffer, std::shared_ptr<BufferExtraData> &bedata)
{
    if (buffer == nullptr) {
        return;
    }
    auto bi = reinterpret_cast<const SurfaceBufferImpl *>(buffer.GetRefPtr());
    bedata = bi->bedata_;
}

void SurfaceBufferImpl::SetBufferHandle(sptr<SurfaceBuffer> &buffer, BufferHandle *handle)
{
    if (buffer == nullptr) {
        return;
    }
    auto bi = reinterpret_cast<SurfaceBufferImpl *>(buffer.GetRefPtr());
    bi->handle_ = handle;
}

void SurfaceBufferImpl::WriteToMessageParcel(const sptr<SurfaceBuffer> &buffer, MessageParcel &parcel)
{
    if (buffer == nullptr) {
        return;
    }
    auto bi = reinterpret_cast<const SurfaceBufferImpl *>(buffer.GetRefPtr());

    if (bi->handle_ == nullptr) {
        BLOGE("Failure, Reason: handle_ is nullptr");
        return;
    }

    bool ret = WriteBufferHandle(parcel, *bi->handle_);
    if (ret == false) {
        BLOGE("Failure, Reason: WriteBufferHandle return false");
    }

    parcel.WriteInt32(bi->extraDatas_.size());
    for (const auto &[k, v] : bi->extraDatas_) {
        parcel.WriteUint32(k);
        parcel.WriteInt32(v.type);
        if (v.type == EXTRA_DATA_TYPE_INT32) {
            auto pVal = std::any_cast<int32_t>(&v.value);
            if (pVal != nullptr) {
                parcel.WriteInt32(*pVal);
            } else {
                BLOGE("unexpected: INT32, any_cast failed");
            }
        }
        if (v.type == EXTRA_DATA_TYPE_INT64) {
            auto pVal = std::any_cast<int64_t>(&v.value);
            if (pVal != nullptr) {
                parcel.WriteInt64(*pVal);
            } else {
                BLOGE("unexpected: INT64, any_cast failed");
            }
        }
    }
}

sptr<EglData> SurfaceBufferImpl::GetEglData(const sptr<SurfaceBuffer> &buffer)
{
    if (buffer == nullptr) {
        return;
    }
    auto bi = reinterpret_cast<const SurfaceBufferImpl *>(buffer.GetRefPtr());
    return bi->eglData_;
}

void SurfaceBufferImpl::SetEglData(sptr<SurfaceBuffer> &buffer, const sptr<EglData>& data)
{
    if (buffer == nullptr) {
        return;
    }
    auto bi = reinterpret_cast<const SurfaceBufferImpl *>(buffer.GetRefPtr());
    bi->eglData_ = data;
}
} // namespace OHOS
