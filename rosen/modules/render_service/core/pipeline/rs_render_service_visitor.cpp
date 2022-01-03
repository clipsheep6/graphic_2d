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

#include "pipeline/rs_render_service_visitor.h"

#include <surface.h>
#include <window_manager.h>
#include <window_option.h>
#include <window.h>

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

RSRenderServiceVisitor::RSRenderServiceVisitor() {}

RSRenderServiceVisitor::~RSRenderServiceVisitor() {}

void RSRenderServiceVisitor::PrepareBaseRenderNode(RSBaseRenderNode &node)
{
    for (auto child : node.GetChildren()) {
        auto existingChild = child.lock();
        if (!existingChild) {
            ROSEN_LOGI("RSRenderServiceVisitor::PrepareBaseRenderNode this child haven't existed");
            continue;
        }
        existingChild->Prepare(shared_from_this());
    }
}

void RSRenderServiceVisitor::ProcessBaseRenderNode(RSBaseRenderNode &node)
{
    for (auto child : node.GetChildren()) {
        auto existingChild = child.lock();
        if (!existingChild) {
            ROSEN_LOGI("RSRenderServiceVisitor::ProcessBaseRenderNode this child haven't existed");
            continue;
        }
        existingChild->Process(shared_from_this());
    }
}

void RSRenderServiceVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode &node)
{
    SortZOrder(node);
    PrepareBaseRenderNode(node);
}

void RSRenderServiceVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    ROSEN_LOGI("RsDebug RSRenderServiceVisitor::ProcessDisplayRenderNode child size:%d", node.GetChildren().size());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        ROSEN_LOGE("RSHardwareProcessor::Init ScreenManager is nullptr");
        return;
    }
    ScreenState state = screenManager->QueryScreenState(node.GetScreenId());
    processor_ = RSProcessorFactory::CreateProcessor(state);

    if (processor_ == nullptr) {
        ROSEN_LOGE("RSRenderServiceVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }

    processor_->Init(node.GetScreenId());
    ProcessBaseRenderNode(node);
    processor_->PostProcess();
}

void RSRenderServiceVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    SortZOrder(node);
}

void RSRenderServiceVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    if (!processor_) {
        ROSEN_LOGE("RSRenderServiceVisitor::ProcessSurfaceRenderNode processor is nullptr");
        return;
    }
    processor_->ProcessSurface(node);
}

void RSRenderServiceVisitor::SortZOrder(RSBaseRenderNode &node)
{
    auto children = node.GetChildren();
    auto compare = [](std::weak_ptr<RSBaseRenderNode> first, std::weak_ptr<RSBaseRenderNode> second) -> bool {
        auto node1 = RSBaseRenderNode::ReinterpretCast<RSPropertyRenderNode>(first.lock());
        auto node2 = RSBaseRenderNode::ReinterpretCast<RSPropertyRenderNode>(second.lock());
        if (node1 == nullptr || node2 == nullptr) {
            return false;
        }
        return node1->GetRenderProperties().GetPositionZ() < node2->GetRenderProperties().GetPositionZ();
    };
    std::stable_sort(children.begin(), children.end(), compare);
}
} // namespace Rosen
} // namespace OHOS
