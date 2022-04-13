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

#include "pipeline/rs_uni_render_listener.h"

#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSUniRenderListener::~RSUniRenderListener() {}

RSUniRenderListener::RSUniRenderListener(std::weak_ptr<RSDisplayRenderNode> displayRenderNode)
    : displayRenderNode_(displayRenderNode) {}

void RSUniRenderListener::OnBufferAvailable()
{
    auto node = displayRenderNode_.lock();
    if (node == nullptr) {
        ROSEN_LOGE("RSUniRenderListener::OnBufferAvailable node is nullptr");
        return;
    }
    ROSEN_LOGI("RsDebug RSUniRenderListener::OnBufferAvailable node id:%llu", node->GetId());

    if (!node->IsOnTheTree()) {
        RSMainThread::Instance()->PostTask([node]() {
            ROSEN_LOGI("RsDebug RSUniRenderListener::OnBufferAvailable node id:%llu: is not on the tree",
                node->GetId());
            auto& surfaceConsumer = node->GetConsumer();
            if (surfaceConsumer == nullptr) {
                ROSEN_LOGE("RsDebug RSUniRenderListener::OnBufferAvailable: consumer is null!");
                return;
            }
            sptr<SurfaceBuffer> buffer;
            int32_t fence = -1;
            int64_t timestamp = 0;
            Rect damage;
            auto ret = surfaceConsumer->AcquireBuffer(buffer, fence, timestamp, damage);
            if (buffer == nullptr || ret != SURFACE_ERROR_OK) {
                ROSEN_LOGE("RsDebug RSUniRenderListener::OnBufferAvailable: AcquireBuffer failed!");
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
    }
    RSMainThread::Instance()->RequestNextVSync();
}
}
}

