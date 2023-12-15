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

#include "ui/rs_effect_node.h"

#include "command/rs_effect_node_command.h"
#include "command/rs_node_command.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_node_map.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
RSEffectNode::SharedPtr RSEffectNode::Create(bool isRenderServiceNode, bool isSamelayerRender)
{
    SharedPtr node(new RSEffectNode(isRenderServiceNode, isSamelayerRender));
    RSNodeMap::MutableInstance().RegisterNode(node);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return node;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSEffectNodeCreate>(node->GetId(), isSamelayerRender);
    transactionProxy->AddCommand(command, node->IsRenderServiceNode());
    auto sameRenderSurfaceId = node->GetSameRenderSurfaceId();
    if (sameRenderSurfaceId != 0) {
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSEffectNodeSetSurface>(node->GetId(), sameRenderSurfaceId);
        transactionProxy->AddCommand(command, node->IsRenderServiceNode());
    }
    return node;
}

RSEffectNode::RSEffectNode(bool isRenderServiceNode, bool isSamelayerRender)
    : RSNode(isRenderServiceNode, isSamelayerRender) {}

} // namespace Rosen
} // namespace OHOS
