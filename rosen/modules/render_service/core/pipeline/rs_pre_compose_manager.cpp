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

namespace OHOS {
namespace Rosen {
static sptr<RSPreComposeManager> RSPreComposeManager::GetInstance()
{
    static std::mutex mutex;
    lock_guard<std::mutex> lock(mutex);
    if (instance_ = nullptr) {
        instance_ = new RSPreComposeManager();
    }
    return instance_;
}

void RSPreComposeManager::UpdateLastAndCurrentVsync()
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGD("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->UpdateLastAndCurrentVsync();
}

void RSPreComposeManager::Init(RenderContext *context, std::shared_ptr<RSPaintFilterCanvas> canvas,
    std::shared_ptr<RSDisplayRenderNode>& node)
{
    if (rsPreComposeGroup_ == nullptr) {
        context_ = context;
        canvas_ = canvas;
        rsPreComposeGroup_ = MakeGroup(node);
    }
}

void RSPreComposeManager::StartCurrentVsync(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
    std::shared_ptr<RSUniRenderVisitor> visitor)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGD("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->StartCurrentVsync(surfaceNodeList, visitor);
}

void RSPreComposeManager::UpdateNodesByLastVsync(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGD("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->UpdateNodesByLastVsync(curAllSurfaces);
}

void RSPreComposeManager::UpdateOcclusionByLastVsync(Occlusion::Region& accumulatedRegion,
    VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGD("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->UpdateOcclusionByLastVsync(accumulatedRegion, curVisVec, pidVisMap);
}

bool RSPreComposeManager::LastVsyncIsDirty()
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGD("rsPreComposeGroup_ is nullptr");
        return false;
    }
    return rsPreComposeGroup_->LastVsyncIsDirty();
}

void RSPreComposeManager::GetLastHwcNodes(std::vector<SurfaceDirtyMgrPair>& prevHwcEnabledNodes)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGD("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->GetLastHwcNodes(prevHwcEnabledNodes);
}

Occlusion::Region RSPreComposeManager::GetLastDirtyRegion()
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGD("rsPreComposeGroup_ is nullptr");
        return {};
    }
    return rsPreComposeGroup_->GetLastDirtyRegion();
}

Occlusion::Region RSPreComposeManager::GetLastVisibleDirtyRegion()
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGD("rsPreComposeGroup_ is nullptr");
        return {};
    }
    return rsPreComposeGroup_->GetLastVisibleDirtyRegion();
}

void RSPreComposeManager::SetLastVsyncGlobalDirtyRegion(Occlusion::Region& globalRegion)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGD("rsPreComposeGroup_ is nullptr");
        return;
    }
    rsPreComposeGroup_->SetLastVsyncGlobalDirtyRegion(globalRegion);
}

bool RSPreComposeManager::ProcessLastVsyncNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas)
{
    if (rsPreComposeGroup_ == nullptr) {
        ROSEN_LOGD("rsPreComposeGroup_ is nullptr");
        return false;
    }
    rsPreComposeGroup_->ProcessLastVsyncNode(node, canvas);
}

std::shared_ptr<RSPreComposeGroup> RSPreComposeManager::MakeGroup(std::shared_ptr<RSDisplayRenderNode>& node)
{
    ROSEN_LOGD("xxb MakeGroup with node " PRIu64 " ", node->GetId());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        ROSEN_LOGE("RSPreComposeManager::UpdateCurrentVsyncNodes screenManager is nullptr");
        return;
    }
    auto screenInfo = screenManager->QueryScreenInfo(node->GetScreenId());
    auto group = std::make_shared<RSPreComposeGroup>(context_, canvas_);
    group->Init(screenInfo);
    return group;
}
} // namespace Rosen
} // namespace OHOS