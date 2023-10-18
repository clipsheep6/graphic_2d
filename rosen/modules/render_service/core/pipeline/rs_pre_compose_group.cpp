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

namespace OHOS {
namespace Rosen {
RSPreComposeGroup::RSPreComposeGroup(RenderContext *context, std::shared_ptr<RSPaintFilterCanvas> canvas)
    : renderContext_(context), mainCanvas_(canvas)
{
    ROSEN_LOGD("RSPreComposeGroup()");
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
        }
        handler_->PostSyncTask(task);
    }
    runner_->Stop();
}

void RSPreComposeGroup::UpdateLastAndCurrentVsync()
{
    if (current_->isUpdateImageEnd()) {
        swap(current_, last_);
        canStartCurrentVsync_ = true;
    } else {
        canStartCurrentVsync_ = false;
    }
    last_->StartDraw();
}

void RSPreComposeGroup::Init(ScreenInfo& info)
{
    string name = "RSPreComposerThread";
    runner_ = AppExecFwk::EventRunner::Create(name);
    handler_ = std::make_shared<AppExecFwk::EventHandler>();
    runner_->Run();
    current_ = std::make_shared<RSPreComposeElement>(info, renderContext_, mainCanvas_, elementCount_++);
    last_ = std::make_shared<RSPreComposeElement>(info, renderContext_, mainCanvas_, elementCount_++);
}

void RSPreComposeGroup::StartCurrentVsync(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
    std::shared_ptr<RSUniRenderVisitor> visitor)
{
    if (!canStartCurrentVsync_) {
        return;
    }
    current_->SetParams(surfaceNodeList, visitor);
    auto current = current_;
    if (handle_) {
        current->Reset();
        auto task = [current]() {
            ROSEN_LOGD("RSPreComposeThread Enqueue Task");
            current->Init();
            current->UpdateDirtyRegion();
            current->UpdateImage();
        }
        handle_->PostTask(task);
    }
}

void RSPreComposeGroup::UpdateNodesByLastVsync(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    last_->UpdateNodes(curAllSurfaces);
}

void RSPreComposeGroup::UpdateOcclusionByLastVsync(Occlusion::Region& accumulatedRegion,
    VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap)
{
    last_->UpdateOcclusion(accumulatedRegion, curVisVec, pidVisMap);
}

bool RSPreComposeGroup::LastVsyncIsDirty()
{
    return last_->IsDirty();
}

void RSPreComposeGroup::GetLastHwcNodes(std::vector<SurfaceDirtyMgrPair>& prevHwcEnabledNodes)
{
    last_->GetHwcNodes(prevHwcEnabledNodes);
}

Occlusion::Region RSPreComposeGroup::GetLastDirtyRegion()
{
    return last_->GetDirtyRegion();
}

Occlusion::Region RSPreComposeGroup::GetLastVisibleDirtyRegion()
{
    return last_->GetVisibleDirtyRegion();
}

void RSPreComposeGroup::SetLastVsyncGlobalDirtyRegion(Occlusion::Region& globalRegion)
{
    last_->SetGlobalDirtyRegion(globalRegion);
}

bool RSPreComposeGroup::ProcessLastVsyncNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas)
{
    last_->ProcessNode(node, canvas);
}

std::shared_ptr<RSPreComposeGroup> RSPreComposeGroup::MakeGroup(std::shared_ptr<RSDisplayRenderNode>& node)
{
    ROSEN_LOGD("xxb MakeGroup with node " PRIu64 " ", node->GetId());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        ROSEN_LOGE("RSPreComposeGroup::UpdateCurrentVsyncNodes screenManager is nullptr");
        return;
    }
    auto screenInfo = screenManager->QueryScreenInfo(node->GetScreenId());
    auto group = std::make_shared<RSPreComposeGroup>(context_, canvas_);
    group->Init(screenInfo);
    return group;
}
} // namespace Rosen
} // namespace OHOS