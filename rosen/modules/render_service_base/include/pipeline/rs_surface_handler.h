/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_HANDLER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_HANDLER_H

#include <atomic>
#include <mutex>
#include <vector>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "surface_type.h"
#ifndef ROSEN_CROSS_PLATFORM
#include <iconsumer_surface.h>
#include <surface.h>
#include "sync_fence.h"
#endif

namespace OHOS {
namespace Rosen {
using OnDeleteBufferFunc = std::function<void(int32_t)>;
class RSB_EXPORT RSSurfaceHandler {
public:
    // indicates which node this handler belongs to.
    explicit RSSurfaceHandler(NodeId id) : id_(id) {}
    virtual ~RSSurfaceHandler() noexcept = default;

    struct SurfaceBufferEntry {
#ifndef ROSEN_CROSS_PLATFORM
        ~SurfaceBufferEntry() noexcept
        {
            if (buffer != nullptr && bufferDeleteCb_ != nullptr) {
                bufferDeleteCb_(buffer->GetSeqNum());
            }
        }

        void RegisterDeleteBufferListener(OnDeleteBufferFunc bufferDeleteCb)
        {
            if (bufferDeleteCb_ == nullptr) {
                bufferDeleteCb_ = bufferDeleteCb;
            }
        }
#endif
        void Reset()
        {
#ifndef ROSEN_CROSS_PLATFORM
            if (buffer == nullptr) {
                return;
            }
            if (bufferDeleteCb_) {
                bufferDeleteCb_(buffer->GetSeqNum());
            }
            buffer = nullptr;
            acquireFence = SyncFence::INVALID_FENCE;
            releaseFence = SyncFence::INVALID_FENCE;
            damageRect = Rect {0, 0, 0, 0};
#endif
            timestamp = 0;
        }
#ifndef ROSEN_CROSS_PLATFORM
        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
        Rect damageRect = {0, 0, 0, 0};
        OnDeleteBufferFunc bufferDeleteCb_ = nullptr;
#endif
        int64_t timestamp = 0;
    };

    void IncreaseAvailableBuffer();
    void ReduceAvailableBuffer();

    NodeId GetNodeId() const
    {
        return id_;
    }

    void SetDefaultWidthAndHeight(int32_t width, int32_t height)
    {
#ifndef ROSEN_CROSS_PLATFORM
        if (consumer_ != nullptr) {
            consumer_->SetDefaultWidthAndHeight(width, height);
        }
#endif
    }

#ifndef ROSEN_CROSS_PLATFORM
    void SetConsumer(const sptr<IConsumerSurface>& consumer);

    const sptr<IConsumerSurface>& GetConsumer() const
    {
        return consumer_;
    }

    void SetBuffer(
        const sptr<SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence,
        const Rect& damage,
        const int64_t timestamp)
    {
        std::lock_guard<std::mutex> lock(bufMutex_);
        preBuffer_.Reset();
        preBuffer_ = buffer_;
        buffer_.buffer = buffer;
        buffer_.acquireFence = acquireFence;
        buffer_.damageRect = damage;
        buffer_.timestamp = timestamp;
    }

    void SetBuffers(std::vector<Rect> damages)
    {
        damages_ = damages;
    }

    const sptr<SurfaceBuffer>& GetBuffer() const
    {
        std::lock_guard<std::mutex> lock(bufMutex_);
        return buffer_.buffer;
    }

    std::vector<Rect> GetBuffers()
    {
        return damages_;
    }

    const sptr<SyncFence>& GetAcquireFence() const
    {
        std::lock_guard<std::mutex> lock(bufMutex_);
        return buffer_.acquireFence;
    }

    const Rect& GetDamageRegion() const
    {
        std::lock_guard<std::mutex> lock(bufMutex_);
        return buffer_.damageRect;
    }

    void SetCurrentReleaseFence(sptr<SyncFence> fence)
    {
        buffer_.releaseFence = fence;
    }

    void SetReleaseFence(sptr<SyncFence> fence)
    {
        // The fence which get from hdi is preBuffer's releaseFence now.
        preBuffer_.releaseFence = std::move(fence);
    }

#endif

    SurfaceBufferEntry& GetPreBuffer()
    {
        return preBuffer_;
    }

    int32_t GetAvailableBufferCount() const
    {
        return bufferAvailableCount_;
    }

    int64_t GetTimestamp() const
    {
        std::lock_guard<std::mutex> lock(bufMutex_);
        return buffer_.timestamp;
    }

    void CleanCache()
    {
        std::lock_guard<std::mutex> lock(bufMutex_);
        buffer_.Reset();
        preBuffer_.Reset();
    }

    void ResetBufferAvailableCount()
    {
        bufferAvailableCount_ = 0;
    }

    void SetGlobalZOrder(float globalZOrder);
    float GetGlobalZOrder() const;

    bool HasConsumer() const
    {
#ifndef ROSEN_CROSS_PLATFORM
        return consumer_ != nullptr;
#else
        return false;
#endif
    }
    inline bool IsCurrentFrameBufferConsumed()
    {
        return isCurrentFrameBufferConsumed_;
    }
    inline void ResetCurrentFrameBufferConsumed()
    {
        isCurrentFrameBufferConsumed_ = false;
    }
    inline void SetCurrentFrameBufferConsumed()
    {
        isCurrentFrameBufferConsumed_ = true;
    }

#ifndef ROSEN_CROSS_PLATFORM
    void RegisterDeleteBufferListener(OnDeleteBufferFunc bufferDeleteCb)
    {
        std::lock_guard<std::mutex> lock(bufMutex_);
        if (bufferDeleteCb != nullptr) {
            buffer_.RegisterDeleteBufferListener(bufferDeleteCb);
            preBuffer_.RegisterDeleteBufferListener(bufferDeleteCb);
        }
    }
#endif

protected:
#ifndef ROSEN_CROSS_PLATFORM
    sptr<IConsumerSurface> consumer_;
#endif
    bool isCurrentFrameBufferConsumed_ = false;

private:
    NodeId id_ = 0;
    mutable std::mutex bufMutex_;
    SurfaceBufferEntry buffer_; // GUARDED BY bufMutex_
    SurfaceBufferEntry preBuffer_; // GUARDED BY bufMutex_
    float globalZOrder_ = 0.0f;
    std::atomic<int> bufferAvailableCount_ = 0;
    std::vector<Rect> damages_;
};
}
}
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_HANDLER_H
