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

#include "rs_uni_render_processor.h"

#include "rs_trace.h"
#include "string_utils.h"

#include "platform/common/rs_log.h"

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
#include "pipeline/driven_render/rs_driven_surface_render_node.h"
#endif

namespace OHOS {
namespace Rosen {
RSUniRenderProcessor::RSUniRenderProcessor()
    : uniComposerAdapter_(std::make_unique<RSUniRenderComposerAdapter>())
{
}

RSUniRenderProcessor::~RSUniRenderProcessor() noexcept
{
}

bool RSUniRenderProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
                                std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId, renderEngine)) {
        return false;
    }
    // In uni render mode, we can handle screen rotation in the rendering process,
    // so we do not need to handle rotation in composer adapter any more,
    // just pass the buffer to composer straightly.
    screenInfo_.rotation = ScreenRotation::ROTATION_0;
    return uniComposerAdapter_->Init(screenInfo_, offsetX, offsetY, mirrorAdaptiveCoefficient_);
}

void RSUniRenderProcessor::PostProcess()
{
    uniComposerAdapter_->CommitLayers(layers_);
    MultiLayersPerf(layerNum);
    RS_LOGD("RSUniRenderProcessor::PostProcess layers_:%zu", layers_.size());
}

void RSUniRenderProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessSurface: failed to createLayer for node(id: %" PRIu64 ")", node.GetId());
        return;
    }
    node.MarkCurrentFrameHardwareEnabled();
    layers_.emplace_back(layer);
}

void RSUniRenderProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessDisplaySurface: failed to createLayer for node(id: %" PRIu64 ")",
            node.GetId());
        return;
    }
    if (node.GetFingerprint()) {
        layer->SetLayerMaskInfo(HdiLayerInfo::LayerMask::LAYER_MASK_HBM_SYNC);
        RS_LOGD("RSUniRenderProcessor::ProcessDisplaySurface, set layer mask hbm sync");
    } else {
        layer->SetLayerMaskInfo(HdiLayerInfo::LayerMask::LAYER_MASK_NORMAL);
    }
    layers_.emplace_back(layer);
    for (auto surface : node.GetCurAllSurfaces()) {
        auto surfaceNode = RSRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surface);
        if (!surfaceNode || !surfaceNode->GetOcclusionVisible() || surfaceNode->IsLeashWindow()) {
            continue;
        }
        layerNum++;
    }
}

void RSUniRenderProcessor::ProcessDrivenSurface(RSDrivenSurfaceRenderNode& node)
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessDrivenSurface: failed to createLayer for node(id: %" PRIu64 ")",
            node.GetId());
        return;
    }
    layers_.emplace_back(layer);
#endif
}
} // namespace Rosen
} // namespace OHOS
