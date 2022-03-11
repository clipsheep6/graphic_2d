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

#ifndef RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_LISTENER_IMPL_H
#define RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_LISTENER_IMPL_H

#include <ibuffer_consumer_listener.h>
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_processor.h"

namespace OHOS {
namespace Rosen {
class RSRenderServiceListenerImpl : public IBufferConsumerListener {
public:
    RSRenderServiceListenerImpl(std::weak_ptr<RSDisplayRenderNode> displayRenderNode,
        std::shared_ptr<RSProcessor> processor);
    ~RSRenderServiceListenerImpl() override;
    void OnBufferAvailable() override;

private :
    std::weak_ptr<RSDisplayRenderNode> displayRenderNode_;
    std::shared_ptr<RSProcessor> processor_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_LISTENER_IMPL_H
