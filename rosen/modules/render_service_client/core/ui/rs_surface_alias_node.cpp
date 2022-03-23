/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ui/rs_surface_alias_node.h"

#include "command/rs_surface_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
RSSurfaceAliasNode::SharedPtr RSSurfaceAliasNode::Create()
{
    SharedPtr node(new RSSurfaceAliasNode());
    RSNodeMap::MutableInstance().RegisterNode(node);

    ROSEN_LOGD("RsDebug RSSurfaceAliasNode::Create id:%llu", node->GetId());
    return node;
}

RSSurfaceAliasNode::SharedPtr RSSurfaceAliasNode::Create(NodeId targetNodeId, std::string targetNodeName)
{
    SharedPtr node(new RSSurfaceAliasNode());
    RSNodeMap::MutableInstance().RegisterNode(node);

    node->SetTargetNodeId(targetNodeId);
    node->SetTargetNodeName(targetNodeName);

    ROSEN_LOGD("RsDebug RSSurfaceAliasNode::Create id:%llu, target id:%llu", node->GetId(), targetNodeId);
    return node;
}

RSSurfaceAliasNode::RSSurfaceAliasNode() : RSSurfaceNode() {}

void RSSurfaceAliasNode::SetTargetNodeId(NodeId id)
{
    if (targetNodeId_ != 0) {
        ROSEN_LOGE("RSSurfaceAliasNode::SetTargetNodeId, target node id already set");
        return;
    }
    targetNodeId_ = id;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeAddAlias>(GetId(), targetNodeId_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    // this command should be inserted to the front of transaction queue, before any SetXXX commands
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), true);
    }
}

void RSSurfaceAliasNode::SetTargetNodeName(const std::string& name)
{
    name_ = name;
}
} // namespace Rosen
} // namespace OHOS
