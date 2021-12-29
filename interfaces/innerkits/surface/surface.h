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

#ifndef INTERFACES_INNERKITS_SURFACE_SURFACE_H
#define INTERFACES_INNERKITS_SURFACE_SURFACE_H

#include <refbase.h>

#include "ibuffer_consumer_listener.h"
#include "ibuffer_producer.h"
#include "surface_buffer.h"
#include "surface_type.h"

namespace OHOS {
class Surface : public RefBase {
public:
    static sptr<Surface> CreateSurfaceAsConsumer(std::string name = "noname", bool isShared = false);
    static sptr<Surface> CreateSurfaceAsProducer(sptr<IBufferProducer>& producer);

    virtual ~Surface() = default;

    virtual bool IsConsumer() const = 0;
    virtual sptr<IBufferProducer> GetProducer() const = 0;

    // use RequestBufferNoFence or RequestBufferWithFence
    __attribute__((deprecated))
    virtual SurfaceError RequestBuffer(sptr<SurfaceBuffer>& buffer,
                                       int32_t &fence, BufferRequestConfig &config) = 0;

    virtual SurfaceError RequestBufferNoFence(sptr<SurfaceBuffer>& buffer,
                                              BufferRequestConfig &config) = 0;

    // need close fence, or destroy fence
    virtual SurfaceError RequestBufferWithFence(sptr<SurfaceBuffer>& buffer,
                                             int32_t &fence, BufferRequestConfig &config) = 0;

    virtual SurfaceError CancelBuffer(sptr<SurfaceBuffer>& buffer) = 0;

    virtual SurfaceError FlushBuffer(sptr<SurfaceBuffer>& buffer,
                                     int32_t fence, BufferFlushConfig &config) = 0;

    virtual SurfaceError FlushBufferNoFence(sptr<SurfaceBuffer>& buffer,
                                            BufferFlushConfig &config) = 0;

    virtual SurfaceError AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
                                       int64_t &timestamp, Rect &damage) = 0;
    virtual SurfaceError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence) = 0;

    virtual SurfaceError AttachBuffer(sptr<SurfaceBuffer>& buffer) = 0;

    virtual SurfaceError DetachBuffer(sptr<SurfaceBuffer>& buffer) = 0;

    virtual uint32_t     GetQueueSize() = 0;
    virtual SurfaceError SetQueueSize(uint32_t queueSize) = 0;

    virtual SurfaceError SetDefaultWidthAndHeight(int32_t width, int32_t height) = 0;
    virtual int32_t GetDefaultWidth() = 0;
    virtual int32_t GetDefaultHeight() = 0;

    virtual SurfaceError SetDefaultUsage(uint32_t usage) = 0;
    virtual uint32_t GetDefaultUsage() = 0;

    virtual SurfaceError SetUserData(const std::string &key, const std::string &val) = 0;
    virtual std::string GetUserData(const std::string &key) = 0;

    virtual SurfaceError GetName(std::string &name) = 0;

    virtual SurfaceError RegisterConsumerListener(sptr<IBufferConsumerListener>& listener) = 0;
    virtual SurfaceError RegisterConsumerListener(IBufferConsumerListenerClazz *listener) = 0;
    virtual SurfaceError RegisterReleaseListener(OnReleaseFunc func) = 0;
    virtual SurfaceError UnregisterConsumerListener() = 0;

    virtual uint64_t GetUniqueId() const = 0;

    virtual void Dump(std::string &result) const = 0;

    virtual SurfaceError CleanCache() = 0;

protected:
    Surface() = default;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_H
