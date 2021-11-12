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

#include "producer_surface.h"

#include "buffer_log.h"
#include "buffer_manager.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "ProducerSurface" };
}

ProducerSurface::ProducerSurface(sptr<IBufferProducer>& producer)
{
    producer_ = producer;
    auto sret = producer_->GetName(name_);
    if (sret != SURFACE_ERROR_OK) {
        BLOGNE("GetName failed, %{public}s", SurfaceErrorStr(sret).c_str());
    }
    BLOGND("ctor");
}

ProducerSurface::~ProducerSurface()
{
    BLOGND("dtor");
    if (IsRemote()) {
        for (auto it = bufferProducerCache_.begin(); it != bufferProducerCache_.end(); it++) {
            if (it->second->GetVirAddr() != nullptr) {
                BufferManager::GetInstance()->Unmap(it->second);
            }
        }
    }

    producer_ = nullptr;
}

SurfaceError ProducerSurface::Init()
{
    return SURFACE_ERROR_OK;
}

bool ProducerSurface::IsConsumer() const
{
    return false;
}

sptr<IBufferProducer> ProducerSurface::GetProducer() const
{
    return producer_;
}

SurfaceError ProducerSurface::RequestBuffer(sptr<SurfaceBuffer>& buffer,
                                            int32_t &fence, BufferRequestConfig &config)
{
    fence = -1;
    return RequestBufferNoFence(buffer, config);
}

SurfaceError ProducerSurface::RequestBufferNoFence(sptr<SurfaceBuffer>& buffer,
                                                   BufferRequestConfig &config)
{
    int32_t releaseFence = -1;
    auto sret = RequestBufferWithFence(buffer, releaseFence, config);
    if (sret == SURFACE_ERROR_OK && releaseFence >= 0) {
        BLOGI("closing fence: %{public}d", releaseFence);
        close(releaseFence);
    }
    return sret;
}

SurfaceError ProducerSurface::RequestBufferWithFence(sptr<SurfaceBuffer>& buffer,
                                                  int32_t &fence, BufferRequestConfig &config)
{
    IBufferProducer::RequestBufferReturnValue retval;
    std::shared_ptr<BufferExtraData> bedataimpl = std::make_shared<BufferExtraDataImpl>();
    SurfaceError ret = GetProducer()->RequestBuffer(config, bedataimpl, retval);
    if (ret != SURFACE_ERROR_OK) {
        BLOGN_FAILURE("Producer report %{public}s", SurfaceErrorStr(ret).c_str());
        return ret;
    }

    // add cache
    if (retval.buffer != nullptr && IsRemote()) {
        ret = BufferManager::GetInstance()->Map(retval.buffer);
        if (ret != SURFACE_ERROR_OK) {
            BLOGN_FAILURE_ID(retval.sequence, "Map failed");
        } else {
            BLOGN_SUCCESS_ID(retval.sequence, "Map");
        }
    }

    buffer = retval.buffer;
    fence = retval.fence;
    if (buffer != nullptr) {
        bufferProducerCache_[retval.sequence] = buffer;
    } else {
        buffer = bufferProducerCache_[retval.sequence];
    }

    ret = BufferManager::GetInstance()->InvalidateCache(buffer);
    if (ret != SURFACE_ERROR_OK) {
        BLOGNW("Warning [%{public}d], InvalidateCache failed", retval.sequence);
    }

    if (buffer != nullptr) {
        SurfaceBufferImpl::SetExtraData(buffer, bedataimpl);
    }

    for (auto it = retval.deletingBuffers.begin(); it != retval.deletingBuffers.end(); it++) {
        if (IsRemote() && bufferProducerCache_[*it]->GetVirAddr() != nullptr) {
            BufferManager::GetInstance()->Unmap(bufferProducerCache_[*it]);
        }
        bufferProducerCache_.erase(*it);
    }
    return SURFACE_ERROR_OK;
}

SurfaceError ProducerSurface::CancelBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (buffer == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    std::shared_ptr<BufferExtraData> bedataimpl = std::make_shared<BufferExtraDataImpl>();
    SurfaceBufferImpl::GetExtraData(buffer, bedataimpl);
    return GetProducer()->CancelBuffer(SurfaceBufferImpl::GetSeqNum(buffer), bedataimpl);
}

SurfaceError ProducerSurface::FlushBuffer(sptr<SurfaceBuffer>& buffer,
    int32_t fence, BufferFlushConfig &config)
{
    if (buffer == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    std::shared_ptr<BufferExtraData> bedataimpl = std::make_shared<BufferExtraDataImpl>();
    SurfaceBufferImpl::GetExtraData(buffer, bedataimpl);
    return GetProducer()->FlushBuffer(SurfaceBufferImpl::GetSeqNum(buffer), bedataimpl, fence, config);
}

SurfaceError ProducerSurface::FlushBufferNoFence(sptr<SurfaceBuffer>& buffer,
                                                 BufferFlushConfig &config)
{
    return FlushBuffer(buffer, -1, config);
}

SurfaceError ProducerSurface::AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
                                            int64_t &timestamp, Rect &damage)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

SurfaceError ProducerSurface::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

uint32_t     ProducerSurface::GetQueueSize()
{
    return producer_->GetQueueSize();
}

SurfaceError ProducerSurface::SetQueueSize(uint32_t queueSize)
{
    return producer_->SetQueueSize(queueSize);
}

SurfaceError ProducerSurface::GetName(std::string &name)
{
    auto sret = producer_->GetName(name);
    name_ = name;
    return sret;
}

SurfaceError ProducerSurface::SetDefaultWidthAndHeight(int32_t width, int32_t height)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

int32_t ProducerSurface::GetDefaultWidth()
{
    return producer_->GetDefaultWidth();
}

int32_t ProducerSurface::GetDefaultHeight()
{
    return producer_->GetDefaultHeight();
}

SurfaceError ProducerSurface::SetDefaultUsage(uint32_t usage)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

uint32_t ProducerSurface::GetDefaultUsage()
{
    return producer_->GetDefaultUsage();
}

SurfaceError ProducerSurface::SetUserData(const std::string &key, const std::string &val)
{
    if (userData_.size() >= SURFACE_MAX_USER_DATA_COUNT) {
        return SURFACE_ERROR_OUT_OF_RANGE;
    }
    userData_[key] = val;
    return SURFACE_ERROR_OK;
}

std::string ProducerSurface::GetUserData(const std::string &key)
{
    if (userData_.find(key) != userData_.end()) {
        return userData_[key];
    }

    return "";
}

SurfaceError ProducerSurface::RegisterConsumerListener(sptr<IBufferConsumerListener>& listener)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

SurfaceError ProducerSurface::RegisterConsumerListener(IBufferConsumerListenerClazz *listener)
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

SurfaceError ProducerSurface::UnregisterConsumerListener()
{
    return SURFACE_ERROR_NOT_SUPPORT;
}

bool ProducerSurface::IsRemote()
{
    return producer_->AsObject()->IsProxyObject();
}

SurfaceError ProducerSurface::CleanCache()
{
    return producer_->CleanCache();
}
} // namespace OHOS
