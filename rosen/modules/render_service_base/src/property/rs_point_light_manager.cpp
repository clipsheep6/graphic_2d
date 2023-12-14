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

#include "property/rs_point_light_manager.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
constexpr int TWO = 2;

RSPointLightManager* RSPointLightManager::Instance()
{
    static RSPointLightManager instance;
    return &instance;
}

void RSPointLightManager::RegisterLightSource(const std::shared_ptr<RSRenderNode>& renderNode)
{
    NodeId nodeId = renderNode->GetId();
    lightSourceNodeMap_.emplace(nodeId, renderNode->weak_from_this());
}

void RSPointLightManager::RegisterIlluminated(const std::shared_ptr<RSRenderNode>& renderNode)
{
    NodeId nodeId = renderNode->GetId();
    illuminatedNodeMap_.emplace(nodeId, renderNode->weak_from_this());
}

void RSPointLightManager::UnRegisterLightSource(const std::shared_ptr<RSRenderNode>& renderNode)
{
    NodeId nodeId = renderNode->GetId();
    lightSourceNodeMap_.erase(nodeId);
}
void RSPointLightManager::UnRegisterIlluminated(const std::shared_ptr<RSRenderNode>& renderNode)
{
    NodeId nodeId = renderNode->GetId();
    illuminatedNodeMap_.erase(nodeId);
}
void RSPointLightManager::AddDirtyLightSource(std::weak_ptr<RSRenderNode> renderNode)
{
    dirtyLightSourceList_.emplace_back(renderNode);
}
void RSPointLightManager::AddDirtyIlluminated(std::weak_ptr<RSRenderNode> renderNode)
{
    dirtyIlluminatedList_.emplace_back(renderNode);
}
void RSPointLightManager::ClearDirtyList()
{
    dirtyLightSourceList_.clear();
    dirtyIlluminatedList_.clear();
}
void RSPointLightManager::PrepareLight()
{
    if (lightSourceNodeMap_.empty() || illuminatedNodeMap_.empty()) {
        ClearDirtyList();
        return;
    }
    if ((dirtyIlluminatedList_.empty() && dirtyLightSourceList_.empty())) {
        return;
    }
    for (const auto& illuminatedWeakPtr : dirtyIlluminatedList_) {
        auto illuminatedNodePtr = illuminatedWeakPtr.lock();
        if (illuminatedNodePtr) {
            illuminatedNodePtr->GetRenderProperties().GetIlluminated()->ClearLightSource();
        }
    }
    PrepareLight(lightSourceNodeMap_, dirtyIlluminatedList_, false);
    PrepareLight(illuminatedNodeMap_, dirtyLightSourceList_, true);
    ClearDirtyList();
}
void RSPointLightManager::PrepareLight(std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>>& map,
    std::vector<std::weak_ptr<RSRenderNode>>& dirtyList, bool isLightSourceDirty)
{
    EraseIf(map, [this, isLightSourceDirty, &dirtyList](const auto& pair) -> bool {
        auto mapElm = pair.second.lock();
        if (!mapElm) {
            return true;
        }
        if (!mapElm->IsOnTheTree()) { // skip check when node is not on the tree
            return false;
        }
        for (const auto& weakPtr : dirtyList) {
            auto dirtyNodePtr = weakPtr.lock();
            if (!dirtyNodePtr) {
                continue;
            }
            std::shared_ptr<RSRenderNode> lightSourceNode = isLightSourceDirty ? dirtyNodePtr : mapElm;
            std::shared_ptr<RSRenderNode> illuminatedNode = isLightSourceDirty ? mapElm : dirtyNodePtr;
            CheckIlluminated(lightSourceNode, illuminatedNode);
        }
        return false;
    });
}
void RSPointLightManager::CheckIlluminated(
    const std::shared_ptr<RSRenderNode>& lightSourceNode, const std::shared_ptr<RSRenderNode>& illuminatedNode)
{
    const auto& geoPtr = (illuminatedNode->GetRenderProperties().GetBoundsGeometry());
    if (!geoPtr || geoPtr->IsEmpty()) {
        return;
    }
    auto lightSourcePtr = lightSourceNode->GetRenderProperties().GetLightSource();
    RectI effectAbsRect = geoPtr->GetAbsRect();
    int radius = static_cast<int>(lightSourcePtr->GetLightRadius());
    effectAbsRect.SetAll(effectAbsRect.left_ - radius, effectAbsRect.top_ - radius, effectAbsRect.width_ + TWO * radius,
        effectAbsRect.height_ + TWO * radius);
    const auto& lightSourceAbsPosition = lightSourcePtr->GetAbsLightPosition();
    auto lightAbsPositionX = static_cast<int>(lightSourceAbsPosition[0]);
    auto lightAbsPositionY = static_cast<int>(lightSourceAbsPosition[1]);
    auto illuminatedRootNodeId = illuminatedNode->GetInstanceRootNodeId();
    auto lightSourceRootNodeId = lightSourceNode->GetInstanceRootNodeId();
    if (effectAbsRect.Intersect(lightAbsPositionX, lightAbsPositionY) &&
        illuminatedRootNodeId == lightSourceRootNodeId) {
        illuminatedNode->GetRenderProperties().GetIlluminated()->AddLightSource(lightSourcePtr);
        illuminatedNode->SetDirty();
    }
}
} // namespace Rosen
} // namespace OHOS