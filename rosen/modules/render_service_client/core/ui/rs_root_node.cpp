/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ui/rs_root_node.h"

#include "command/rs_root_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSNode> RSRootNode::Create(bool isRenderServiceNode)
{
    std::shared_ptr<RSRootNode> node(new RSRootNode(isRenderServiceNode));
    RSNodeMap::MutableInstance().RegisterNode(node);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return node;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSRootNodeCreate>(node->GetId());
    transactionProxy->AddCommand(command, node->IsRenderServiceNode());
    if (node->NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSRootNodeCreate>(node->GetId());
        transactionProxy->AddCommand(extraCommand, !node->IsRenderServiceNode());
    }
    return node;
}

RSRootNode::RSRootNode(bool isRenderServiceNode) : RSCanvasNode(isRenderServiceNode) {}

void RSRootNode::AttachRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode) const
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSRootNodeAttachRSSurfaceNode>(GetId(),
        surfaceNode->GetId());
    transactionProxy->AddCommand(command, false);

    if (isUniRenderEnabled_) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<RSRootNodeAttachToUniSurfaceNode>(GetId(),
            surfaceNode->GetId());
        transactionProxy->AddCommand(extraCommand, true);
    }
}

void RSRootNode::SetEnableRender(bool flag) const
{
    if (!isUniRenderEnabled_) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSRootNodeSetEnableRender>(GetId(), flag);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode());
        }
    }
}

void RSRootNode::OnBoundsSizeChanged() const
{
    // the following property is only used in RenderThreadVisitor
    if (IsRenderServiceNode()) {
        return;
    }
    // Planning: we should use frame size instead of bounds size to calculate the surface size.
    auto bounds = GetStagingProperties().GetBounds();
    std::unique_ptr<RSCommand> command = std::make_unique<RSRootNodeUpdateSurfaceSize>(GetId(), bounds.z_, bounds.w_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, false);
    }
}
} // namespace Rosen
} // namespace OHOS
