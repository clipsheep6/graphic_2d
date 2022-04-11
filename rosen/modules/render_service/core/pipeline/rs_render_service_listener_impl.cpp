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

#include "pipeline/rs_render_service_listener_impl.h"

#include "platform/common/rs_log.h"
#include "pipeline/rs_main_thread.h"

namespace OHOS {
namespace Rosen {
RSRenderServiceListenerImpl::~RSRenderServiceListenerImpl() {}

RSRenderServiceListenerImpl::RSRenderServiceListenerImpl(std::weak_ptr<RSDisplayRenderNode> displayRenderNode,
    std::shared_ptr<RSProcessor> processor)
    : displayRenderNode_(displayRenderNode), processor_(processor) {}

void RSRenderServiceListenerImpl::OnBufferAvailable()
{
    auto node = displayRenderNode_.lock();
    if (node == nullptr) {
        ROSEN_LOGE("RSRenderServiceListenerImpl::OnBufferAvailable node is nullptr");
        return;
    }
    ROSEN_LOGI("RsDebug RSRenderServiceListenerImpl::OnBufferAvailable node id:%llu", node->GetId());

    if (!node->IsOnTheTree()) {
        RSMainThread::Instance()->PostTask([node]() {
            ROSEN_LOGI("RsDebug RSRenderServiceListenerImpl::OnBufferAvailable node id:%llu: is not on the tree",
                node->GetId());
            auto& surfaceConsumer = node->GetConsumer();
            if (surfaceConsumer == nullptr) {
                ROSEN_LOGE("RsDebug RSRenderServiceListenerImpl::OnBufferAvailable: consumer is null!");
                return;
            }
            sptr<SurfaceBuffer> buffer;
            int32_t fence = -1;
            int64_t timestamp = 0;
            Rect damage;
            auto ret = surfaceConsumer->AcquireBuffer(buffer, fence, timestamp, damage);
            if (buffer == nullptr || ret != SURFACE_ERROR_OK) {
                ROSEN_LOGE("RsDebug RSRenderServiceListenerImpl::OnBufferAvailable: AcquireBuffer failed!");
                return;
            }

            if (node->GetBuffer() != nullptr && node->GetBuffer() != buffer) {
                (void)surfaceConsumer->ReleaseBuffer(node->GetBuffer(), -1);
            }
            node->SetBuffer(buffer);
            sptr<SyncFence> acquireFence = new SyncFence(fence);
            node->SetFence(acquireFence);
        });
    } else {
        node->IncreaseAvailableBuffer();
        if (processor_ != nullptr) {
            processor_->ProcessSurface(*node);
            processor_->PostProcess();
        }
    }
}
}
}

