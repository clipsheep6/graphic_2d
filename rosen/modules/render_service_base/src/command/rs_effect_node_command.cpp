/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "command/rs_effect_node_command.h"

#include "pipeline/rs_effect_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

void EffectNodeCommandHelper::Create(RSContext& context, NodeId id, bool isSameLayerRender)
{
    auto node = std::make_shared<RSEffectRenderNode>(id, context.weak_from_this(), isSameLayerRender);
    context.GetMutableNodeMap().RegisterRenderNode(node);
}

void EffectNodeCommandHelper::SetSameRenderSurface(RSContext& context, NodeId id, SurfaceId surfaceId)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSEffectRenderNode>(id)) {
        node->SetSameRenderSurfaceId(surfaceId);
        context.GetGlobalRootRenderNode()->AddChild(node);
    }
}

} // namespace Rosen
} // namespace OHOS
