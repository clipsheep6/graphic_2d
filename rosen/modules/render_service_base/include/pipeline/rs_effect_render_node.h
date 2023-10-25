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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_EFFECT_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_EFFECT_RENDER_NODE_H

#include "common/rs_macros.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSEffectRenderNode : public RSRenderNode {
public:
    using WeakPtr = std::weak_ptr<RSEffectRenderNode>;
    using SharedPtr = std::shared_ptr<RSEffectRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::EFFECT_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    explicit RSEffectRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {});
    ~RSEffectRenderNode() override;

    void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override;

    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;
#ifndef USE_ROSEN_DRAWING
    void SetEffectRegion(const std::optional<SkPath>& region);
#else
    void SetEffectRegion(const std::optional<Drawing::Path>& region);
#endif

protected:
    RectI GetFilterRect() const override;

private:
#ifndef USE_ROSEN_DRAWING
    std::optional<SkPath> effectRegion_ = std::nullopt;
#else
    std::optional<Drawing::Path> effectRegion_ = std::nullopt;
#endif
    friend class RSEffectDataGenerateDrawable;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_EFFECT_RENDER_NODE_H
