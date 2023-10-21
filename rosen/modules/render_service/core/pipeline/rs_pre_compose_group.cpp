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

#include "pipeline/rs_pre_compose_group.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "rs_base_render_util.h"

namespace OHOS {
namespace Rosen {
RSPreComposeGroup::RSPreComposeGroup()
{
    ROSEN_LOGD("RSPreComposeGroup()");
    regionManager_ = std::make_shared<RSPreComposeRegionManager>();
}

RSPreComposeGroup::~RSPreComposeGroup()
{
    ROSEN_LOGD("~RSPreComposeGroup()");
    auto last = last_;
    auto current = current_;
    if (handler_) {
        auto task = [last, current]() {
            ROSEN_LOGD("RSPreComposeThread() Deinit Element");
            last->Deinit();
            current->Deinit();
        };
        handler_->PostSyncTask(task);
    }
    runner_->Stop();
}

void RSPreComposeGroup::UpdateLastAndCurrentVsync()
{
    if (current_->isUpdateImageEnd()) {
        std::swap(current_, last_);
        canStartCurrentVsync_ = true;
    } else {
        canStartCurrentVsync_ = false;
    }
    last_->StartCalculateAndDrawImage();
}

void RSPreComposeGroup::Init(ScreenInfo& info)
{
    std::string name = "RSPreComposerThread";
    runner_ = AppExecFwk::EventRunner::Create(name);
    handler_ = std::make_shared<AppExecFwk::EventHandler>();
    runner_->Run();
    current_ = std::make_shared<RSPreComposeElement>(info, elementCount_++, regionManager_);
    last_ = std::make_shared<RSPreComposeElement>(info, elementCount_++, regionManager_);
}

void RSPreComposeGroup::StartCurrentVsync(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
    std::shared_ptr<RSUniRenderVisitor> visitor, uint64_t focusNodeId, uint64_t leashFocusId)
{
    if (!canStartCurrentVsync_) {
        return;
    }
    current_->Reset();
    current_->SetParams(surfaceNodeList, visitor, focusNodeId, leashFocusId);
    auto current = current_;
    if (handler_) {
        auto task = [current]() {
            ROSEN_LOGD("RSPreComposeThread Enqueue Task");
            current->Init();
            current->UpdateDirtyRegion();
            current->UpdateImage();
        };
        handler_->PostTask(task);
    }
}

void RSPreComposeGroup::UpdateNodesByLastVsync(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    last_->UpdateNodes(curAllSurfaces);
}

void RSPreComposeGroup::UpdateOcclusionByLastVsync(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    Occlusion::Region& accumulatedRegion, VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap)
{
    last_->UpdateOcclusion(surfaceNode, accumulatedRegion, curVisVec, pidVisMap);
}

bool RSPreComposeGroup::LastVsyncIsDirty()
{
    return last_->IsDirty();
}

void RSPreComposeGroup::GetLastHwcNodes(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    std::vector<RSUniRenderVisitor::SurfaceDirtyMgrPair>& prevHwcEnabledNodes)
{
    last_->GetHwcNodes(surfaceNode, prevHwcEnabledNodes);
}

Occlusion::Region RSPreComposeGroup::GetLastVisibleDirtyRegion()
{
    return last_->GetDirtyVisibleRegion();
}

Occlusion::Region RSPreComposeGroup::GetLastVisibleDirtyRegionWithGpuNodes()
{
    return last_->GetVisibleDirtyRegionWithGpuNodes();
}

bool RSPreComposeGroup::ProcessLastVsyncNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas,
    uint32_t threadIndex)
{
    return last_->ProcessNode(node, canvas, threadIndex);
}
} // namespace Rosen
} // namespace OHOS