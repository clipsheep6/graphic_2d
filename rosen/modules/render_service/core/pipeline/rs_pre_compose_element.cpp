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

#include "pipeline/rs_pre_compose_element.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSPreComposeElement::RSPreComposeElement(ScreenInfo& info, RenderContext* renderContext,
    std::shared_ptr<RSPaintFilterCanvas> canvas, int32_t id)
    : screenInfo_(info), renderContext_(renderContext), mainCanvas_(canvas), id_(id)
{
    ROSEN_LOGD("RSPreComposeElement()");
}

RSPreComposeElement::~RSPreComposeElement()
{
    ROSEN_LOGD("~RSPreComposeElement()");
}

void RSPreComposeElement::UpdateNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
        std::shared_ptr<RSUniRenderVisitor> visitor);
{
    displayVisitor_ = visitor;
    surfaceNodeList_ = surfaceNodeList;

    std::vector<RSBaseRenderNode::SharedPtr> allSurfaceNodes;
    for (auto surfaceNode : surfaceNodeList_) {
        surfaceNode->CollectSurface(surfaceNode, allSurfaceNodes, true, false);
    }
    swap(allSurfaceNodes_, allSurfaceNodes);

    std::unordered_set<NodeId> nodeIds;
    for (auto node : allSurfaceNodes) {
        auto surface = RSBaseRenderNode:ReinterpretCast<RSSurfaceRenderNode>(node);
        if (surface == nullptr) {
            continue;
        }
        ROSEN_LOGD("xxb node id %{public}" PRIu64 ", name %{public}s", node->GetId(), surface->GetName().c_str())
        nodeIds.insert(node->GetId());
    }
    swap(nodeIds_, nodeIds);
}

void RSPreComposeElement::UpdateNodes(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    std::vector<RSBaseRenderNode::SharedPtr> newAllSurfaces;
    for (auto node : curAllSurfaces) {
        if (nodeIds_.count(node->GetId()) == 0) {
            newAllSurfaces.emplace_back(node);
        }
    }
    swap(newAllSurfaces, curAllSurfaces);
}

void RSPreComposeElement::UpdateOcclusion(Occlusion::Region& accumulatedRegion,
    VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap)
{
    // TODO
}

bool RSPreComposeElement::IsDirty()
{
    // TODO
}

void RSPreComposeElement::GetHwcNodes(std::vector<SurfaceDirtyMgrPair>& prevHwcEnabledNodes)
{
    // TODO
}

Occlusion::Region RSPreComposeElement::GetDirtyRegion()
{
    // TODO
}

Occlusion::Region RSPreComposeElement::GetVisibleDirtyRegion()
{
    // TODO
}

void RSPreComposeElement::SetGlobalDirtyRegion(Occlusion::Region& globalRegion)
{
    // TODO
}

bool RSPreComposeElement::ProcessNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas)
{
    // TODO
}
} // namespace Rosen
} // namespace OHOS