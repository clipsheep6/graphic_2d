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

#include "ui/rs_surface_node.h"

#include <algorithm>
#include <string>

#include "command/rs_surface_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface_converter.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#ifdef ACE_ENABLE_GL
#include "render_context/render_context.h"
#endif

namespace OHOS {
namespace Rosen {

RSSurfaceNode::SharedPtr RSSurfaceNode::Create(const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow)
{
    SharedPtr node(new RSSurfaceNode(surfaceNodeConfig, isWindow));

    RSNodeMap::Instance().RegisterNode(node);

    // create node in RS
    RSSurfaceRenderNodeConfig config = { .id = node->GetId() };
    if (!node->CreateNodeAndSurface(config)) {
        return nullptr;
    }

    // create node in RT
    if (!isWindow) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(node->GetId());
        RSTransactionProxy::GetInstance().AddCommand(command, isWindow);
    }
    ROSEN_LOGI("RsDebug RSSurfaceNode::Create id:%llu", node->GetId());
    return node;
}

bool RSSurfaceNode::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint64(GetId()) && parcel.WriteString(name_) && parcel.WriteBool(IsRenderServiceNode());
}

RSSurfaceNode* RSSurfaceNode::Unmarshalling(Parcel& parcel)
{
    uint64_t id = UINT64_MAX;
    std::string name;
    bool isRenderServiceNode = false;
    if (!(parcel.ReadUint64(id) && parcel.ReadString(name) && parcel.ReadBool(isRenderServiceNode))) {
        return nullptr;
    }
    RSSurfaceNodeConfig config = { name };

    RSSurfaceNode* surfaceNode = new RSSurfaceNode(config, isRenderServiceNode);
    surfaceNode->SetId(id);

    return surfaceNode;
}

bool RSSurfaceNode::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    surface_ = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())
                   ->CreateNodeAndSurface(config);
    return (surface_ != nullptr);
}

#ifdef ROSEN_OHOS
sptr<OHOS::Surface> RSSurfaceNode::GetSurface() const
{
    if (surface_ == nullptr) {
        ROSEN_LOGE("No surface");
        return nullptr;
    }
    auto ohosSurface = RSSurfaceConverter::ConvertToOhosSurface(surface_);
    return ohosSurface;
}
#endif

RSSurfaceNode::RSSurfaceNode(bool isRenderServiceNode) : RSPropertyNode(isRenderServiceNode) {}

RSSurfaceNode::RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode)
    : RSPropertyNode(isRenderServiceNode), name_(config.SurfaceNodeName)
{}

RSSurfaceNode::~RSSurfaceNode() {}

} // namespace Rosen
} // namespace OHOS
