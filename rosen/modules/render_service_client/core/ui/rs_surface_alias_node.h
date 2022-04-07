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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_ALIAS_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_ALIAS_NODE_H

#include "ui/rs_surface_node.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSSurfaceAliasNode : public RSSurfaceNode {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceAliasNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceAliasNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::SURFACE_ALIAS_NODE;

    virtual ~RSSurfaceAliasNode() = default;
    static SharedPtr Create();
    static SharedPtr Create(NodeId targetNodeId, std::string targetNodeName = {});
    void SetTargetNodeId(NodeId id);
    void SetTargetNodeName(const std::string& name);

    // surface alias node should only be used to set properties, not to draw
    sptr<OHOS::Surface> GetSurface() const override {
        return nullptr;
    }

    RSUINodeType GetType() const override
    {
        return RSUINodeType::SURFACE_ALIAS_NODE;
    }

protected:
    explicit RSSurfaceAliasNode();
    RSSurfaceAliasNode(const RSSurfaceAliasNode&) = delete;
    RSSurfaceAliasNode(const RSSurfaceAliasNode&&) = delete;
    RSSurfaceAliasNode& operator=(const RSSurfaceAliasNode&) = delete;
    RSSurfaceAliasNode& operator=(const RSSurfaceAliasNode&&) = delete;

private:
    NodeId targetNodeId_ {0};

    friend class RSUIDirector;
    friend class RSAnimation;
    friend class RSPathAnimation;
    template<typename T>
    friend class RSPropertyAnimation;
    friend class RSSurfaceExtractor;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_ALIAS_NODE_H
