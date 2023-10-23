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

#include "pipeline/rs_pre_compose_manager.h"
#include <mutex>
#include "screen_manager/rs_screen_manager.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

using namespace std;
namespace OHOS {
namespace Rosen {
sptr<RSPreComposeManager> RSPreComposeManager::GetInstance()
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    if (instance_ == nullptr) {
        instance_ = new RSPreComposeManager();
    }
    return instance_;
}

void RSPreComposeManager::UpdateLastAndCurrentVsync()
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGE("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->UpdateLastAndCurrentVsync();
}

void RSPreComposeManager::StartCurrentVsync(std::shared_ptr<RSDisplayRenderNode>& node,
    std::shared_ptr<RSUniRenderVisitor> visitor, uint64_t focusNodeId)
{
    if (rsPreComposeGroup_ == nullptr) {
        rsPreComposeGroup_ = MakeGroup(node);
    }
    uint64_t leashFocusId = 0;
    auto surfaceNodeList = GetSurfaceNodesFromDisplay(node, focusNodeId, leashFocusId);
    rsPreComposeGroup_->StartCurrentVsync(surfaceNodeList, visitor, focusNodeId, leashFocusId);
}

std::list<std::shared_ptr<RSSurfaceRenderNode>> RSPreComposeManager::GetSurfaceNodesFromDisplay(
    std::shared_ptr<RSDisplayRenderNode>& displayNode, uint64_t focusNodeId, uint64_t& leashFocusId)
{
    if (displayNode == nullptr) {
        ROSEN_LOGE("DisplayNode is nullptr");
        return {};
    }
    bool isRotation = displayNode->IsRotationChanged();
    if (isRotation) {
        ROSEN_LOGD("DisplayNode is rotation and PreComposer disable this vsync");
        return {};
    }
    std::string logInfo = "";
    std::list<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    if (RSSystemProperties::IsSceneBoardEnabled()) {
        std::vector<RSBaseRenderNode::SharedPtr> curAllSurfacesVec;
        displayNode->CollectSurface(displayNode, curAllSurfacesVec, true, true);
        std::copy(curAllSurfacesVec.begin(), curAllSurfacesVec.end(), std::back_inserter(curAllSurfaces));
    } else {
        curAllSurfaces = displayNode->GetSortedChildren();
    }
    std::list<std::shared_ptr<RSSurfaceRenderNode>> surfaceNodeList;
    bool isPrecomposeNode = false;
    bool isFocusNode = false;
    for (auto iter = curAllSurfaces.begin(); iter != curAllSurfaces.end(); iter++) {
        auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*iter);
        if (node == nullptr) {
            ROSEN_LOGE("current node is not surfaceNode");
            continue;
        }
        std::string surfaceName = node->GetName();
        isPrecomposeNode |= (surfaceName.substr(0, 3) != "SCB");
        if (node->GetId() == focusNodeId) {
            isFocusNode = true;
        }
        if (!isFocusNode && node->IsLeashWindow()) {
            for (auto& child : node->GetSortedChildren()) {
                if (child && child->GetId() == focusNodeId) {
                    isFocusNode = true;
                    leashFocusId = node->GetId();
                }
            }
        }
        if (isFocusNode) {
            break;
        }
        if (isPrecomposeNode) {
            logInfo += "node:[ " + node->GetName() + ", " + std::to_string(node->GetId()) + " ]" +
                "( " + std::to_string(node->HasFilter()) + ", " + std::to_string(node->HasAbilityComponent()) + " ); ";
            surfaceNodeList.push_back(node);
        }
    }
    ROSEN_LOGD("surfaceNode in PreCompose is %{public}s", logInfo.c_str());
    return surfaceNodeList;
}

void RSPreComposeManager::UpdateNodesByLastVsync(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGE("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->UpdateNodesByLastVsync(curAllSurfaces);
}

void RSPreComposeManager::UpdateOcclusionByLastVsync(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    Occlusion::Region& accumulatedRegion, VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGE("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->UpdateOcclusionByLastVsync(surfaceNode, accumulatedRegion, curVisVec, pidVisMap);
}

bool RSPreComposeManager::LastVsyncIsDirty()
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGE("rsPreComposeGroup_ is nullptr");
        return false;
    }
    return rsPreComposeGroup_->LastVsyncIsDirty();
}

void RSPreComposeManager::GetLastHwcNodes(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    std::vector<RSUniRenderVisitor::SurfaceDirtyMgrPair>& prevHwcEnabledNodes)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGE("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->GetLastHwcNodes(surfaceNode, prevHwcEnabledNodes);
}

Occlusion::Region RSPreComposeManager::GetLastVisibleDirtyRegion()
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGE("rsPreComposeGroup_ is nullptr");
        return {};
    }
    return rsPreComposeGroup_->GetLastVisibleDirtyRegion();
}

void RSPreComposeManager::SetBufferAge(int32_t bufferAge)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGE("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->SetBufferAge(bufferAge);
}

Occlusion::Region RSPreComposeManager::GetLastVisibleDirtyRegionWithGpuNodes()
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGE("rsPreComposeGroup_ is nullptr");
        return {};
    }
    return rsPreComposeGroup_->GetLastVisibleDirtyRegionWithGpuNodes();
}

bool RSPreComposeManager::ProcessLastVsyncNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas,
    uint32_t threadIndex)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGE("rsPreComposeGroup_ is nullptr");
        return false;
    }
    return rsPreComposeGroup_->ProcessLastVsyncNode(node, canvas, threadIndex);
}

std::shared_ptr<RSPreComposeGroup> RSPreComposeManager::MakeGroup(std::shared_ptr<RSDisplayRenderNode>& node)
{
    ROSEN_LOGD("xxb MakeGroup with node %" PRIu64 " ", node->GetId());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        ROSEN_LOGE("RSPreComposeManager::UpdateCurrentVsyncNodes screenManager is nullptr");
        return nullptr;
    }
    auto screenInfo = screenManager->QueryScreenInfo(node->GetScreenId());
    auto group = std::make_shared<RSPreComposeGroup>();
    group->Init(screenInfo);
    return group;
}
} // namespace Rosen
} // namespace OHOS