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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H

#include <memory>

#include "pipeline/rs_base_render_node.h"

namespace OHOS {
namespace Rosen {
class RSDisplayRenderNode : public RSBaseRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSDisplayRenderNode>;
    using SharedPtr = std::shared_ptr<RSDisplayRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::DISPLAY_NODE;

    explicit RSDisplayRenderNode(NodeId id, const RSDisplayNodeConfig& config);
    virtual ~RSDisplayRenderNode();

    void SetScreenId(uint64_t screenId) {
        screenId_ = screenId;
    }

    uint64_t GetScreenId() const
    {
        return screenId_;
    }

    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSRenderNodeType GetType() override
    {
        return RSRenderNodeType::DISPLAY_NODE;
    }

protected:

private:
    uint64_t screenId_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DISPLAY_RENDER_NODE_H
