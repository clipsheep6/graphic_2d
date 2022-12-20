/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RS_PARALLEL_PACK_VISITOR_H
#define RS_PARALLEL_PACK_VISITOR_H

#include <vector>
#include <string>
#include "platform/common/rs_system_properties.h"
#include "visitor/rs_node_visitor.h"
namespace OHOS {
namespace Rosen {
class RSUniRenderVisitor;
class RSParallelPackVisitor : public RSNodeVisitor {
public:
    RSParallelPackVisitor() {}
    explicit RSParallelPackVisitor(RSUniRenderVisitor &visitor);
    ~RSParallelPackVisitor() = default;

    void PrepareBaseRenderNode(RSBaseRenderNode& node) override;
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override;
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
    void PrepareRootRenderNode(RSRootRenderNode& node) override {}
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override;

    void ProcessBaseRenderNode(RSBaseRenderNode& node) override;
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override;
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
    void ProcessRootRenderNode(RSRootRenderNode& node) override {}
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
private:
    bool isOpDropped_ = false;
    bool isPartialRenderEnabled_ = false;
    bool isSecurityDisplay_ = false;
    bool doAnimate_ = false;
    PartialRenderType partialRenderType_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_PACK_VISITOR_H