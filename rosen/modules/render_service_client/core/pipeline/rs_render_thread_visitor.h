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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_RENDER_THREAD_VISITOR_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_RENDER_THREAD_VISITOR_H

#include <memory>
#include <set>

#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "transaction/rs_transaction_proxy.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
class RSDirtyRegionManager;
class RSProperties;
class RSRenderNode;

class RSRenderThreadVisitor : public RSNodeVisitor {
public:
    RSRenderThreadVisitor();
    virtual ~RSRenderThreadVisitor();

    virtual void PrepareBaseRenderNode(RSBaseRenderNode& node) override;
    virtual void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override {}
    virtual void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override;
    virtual void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    virtual void PrepareRootRenderNode(RSRootRenderNode& node) override;

    virtual void ProcessBaseRenderNode(RSBaseRenderNode& node) override;
    virtual void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override {}
    virtual void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;
    virtual void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
    virtual void ProcessRootRenderNode(RSRootRenderNode& node) override;

private:
    void DrawRectOnCanvas(const RectI& dirtyRect, const SkColor color, const SkPaint::Style fillType, float alpha);
    void DrawDirtyRegion();

    RSDirtyRegionManager dirtyManager_;
    bool dirtyFlag_ = false;
    bool isIdle_ = true;
    RSPaintFilterCanvas* canvas_;
    uint64_t uiTimestamp_ = 0;

    void ClipHoleForSurfaceNode(RSSurfaceRenderNode& node);

    std::vector<NodeId> childSurfaceNodeIds_;
    SkMatrix parentSurfaceNodeMatrix_;

    void SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId followNodeId) const;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_RENDER_THREAD_VISITOR_H
