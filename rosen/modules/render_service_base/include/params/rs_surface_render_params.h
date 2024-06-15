/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H

#include <string>

#include "common/rs_occlusion_region.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_base_render_node.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_buffer.h"
#include "sync_fence.h"
#endif
#include "surface_type.h"

namespace OHOS::Rosen {
class RSSurfaceRenderNode;
struct RSLayerInfo {
#ifndef ROSEN_CROSS_PLATFORM
    GraphicIRect srcRect;
    GraphicIRect dstRect;
    GraphicIRect boundRect;
    Drawing::Matrix matrix;
    int32_t gravity = 0;
    int32_t zOrder = 0;
    float alpha = 1.f;
    GraphicBlendType blendType;
    GraphicTransformType transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    bool operator==(const RSLayerInfo& layerInfo) const
    {
        return (srcRect == layerInfo.srcRect) && (dstRect == layerInfo.dstRect) &&
            (boundRect == layerInfo.boundRect) && (matrix == layerInfo.matrix) && (gravity == layerInfo.gravity) &&
            (zOrder == layerInfo.zOrder) && (blendType == layerInfo.blendType) &&
            (transformType == layerInfo.transformType) && (ROSEN_EQ(alpha, layerInfo.alpha));
    }
#endif
};
class RSB_EXPORT RSSurfaceRenderParams : public RSRenderParams {
public:
    explicit RSSurfaceRenderParams(NodeId id);
    ~RSSurfaceRenderParams() override = default;
    inline bool IsMainWindowType() const
    {
        return isMainWindowType_;
    }
    inline bool IsLeashWindow() const
    {
        return isLeashWindow_;
    }
    bool IsAppWindow() const
    {
        return isAppWindow_;
    }
    RSSurfaceNodeType GetSurfaceNodeType() const
    {
        return rsSurfaceNodeType_;
    }
    SelfDrawingNodeType GetSelfDrawingNodeType() const
    {
        return selfDrawingType_;
    }
    void SetAncestorDisplayNode(const RSRenderNode::WeakPtr& ancestorDisplayNode)
    {
        ancestorDisplayNode_ = ancestorDisplayNode;
    }
    RSRenderNode::WeakPtr GetAncestorDisplayNode() const
    {
        return ancestorDisplayNode_;
    }

    float GetAlpha() const
    {
        return alpha_;
    }
    bool IsSpherizeValid() const
    {
        return isSpherizeValid_;
    }
    bool NeedBilinearInterpolation() const
    {
        return needBilinearInterpolation_;
    }
    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }
    const RectI& GetAbsDrawRect() const
    {
        return absDrawRect_;
    }
    const RRect& GetRRect() const
    {
        return rrect_;
    }

    bool GetIsSecurityLayer() const
    {
        return isSecurityLayer_;
    }
    bool GetIsSkipLayer() const
    {
        return isSkipLayer_;
    }
    bool GetIsProtectedLayer() const
    {
        return isProtectedLayer_;
    }
    const std::set<NodeId>& GetSecurityLayerIds() const
    {
        return securityLayerIds_;
    }
    const std::set<NodeId>& GetSkipLayerIds() const
    {
        return skipLayerIds_;
    }
    bool HasSecurityLayer()
    {
        return securityLayerIds_.size() != 0;
    }
    bool HasSkipLayer()
    {
        return skipLayerIds_.size() != 0;
    }
    bool HasProtectedLayer()
    {
        return protectedLayerIds_.size() != 0;
    }

    std::string GetName() const
    {
        return name_;
    }

    void SetLeashWindowVisibleRegionEmptyParam(bool isLeashWindowVisibleRegionEmpty)
    {
        if (isLeashWindowVisibleRegionEmpty_ == isLeashWindowVisibleRegionEmpty) {
            return;
        }
        isLeashWindowVisibleRegionEmpty_ = isLeashWindowVisibleRegionEmpty;
        needSync_ = true;
    }

    bool GetLeashWindowVisibleRegionEmptyParam() const
    {
        return isLeashWindowVisibleRegionEmpty_;
    }

    void SetUifirstNodeEnableParam(MultiThreadCacheType isUifirst)
    {
        if (uiFirstFlag_ == isUifirst) {
            return;
        }
        uiFirstFlag_ = isUifirst;
        needSync_ = true;
    }

    MultiThreadCacheType GetUifirstNodeEnableParam() const
    {
        return uiFirstFlag_;
    }

    void SetBufferClearCacheSet(const std::set<int32_t> bufferCacheSet)
    {
        if (bufferCacheSet.size() > 0) {
            bufferCacheSet_ = bufferCacheSet;
            needSync_ = true;
        }
    }

    const std::set<int32_t> GetBufferClearCacheSet()
    {
        return bufferCacheSet_;
    }

    void SetIsParentUifirstNodeEnableParam(bool isUifirstParent)
    {
        if (uiFirstParentFlag_ == isUifirstParent) {
            return;
        }
        uiFirstParentFlag_ = isUifirstParent;
        needSync_ = true;
    }

    void SetUifirstChildrenDirtyRectParam(const RectI& rect)
    {
        childrenDirtyRect_ = rect;
        needSync_ = true;
    }

    RectI& GetUifirstChildrenDirtyRectParam()
    {
        return childrenDirtyRect_;
    }
    const RectI& GetDstRect() const
    {
        return dstRect_;
    }
    void SetSurfaceCacheContentStatic(bool contentStatic);
    bool GetSurfaceCacheContentStatic() const;
    bool GetPreSurfaceCacheContentStatic() const;

    float GetPositionZ() const;

    void SetSurfaceSubTreeDirty(bool isSubTreeDirty);
    bool GetSurfaceSubTreeDirty() const;

    bool GetParentUifirstNodeEnableParam()
    {
        return uiFirstParentFlag_;
    }

    void SetUIFirstFrameGravity(Gravity gravity)
    {
        if (uiFirstFrameGravity_ == gravity) {
            return;
        }
        uiFirstFrameGravity_ = gravity;
        needSync_ = true;
    }

    Gravity GetUIFirstFrameGravity() const
    {
        return uiFirstFrameGravity_;
    }

    void SetOcclusionVisible(bool visible);
    bool GetOcclusionVisible() const;

    void SetIsParentScaling(bool isParentScaling);
    bool IsParentScaling() const;

    void SetTransparentRegion(const Occlusion::Region& transparentRegion);
    const Occlusion::Region& GetTransparentRegion() const;

    void SetOldDirtyInSurface(const RectI& oldDirtyInSurface);
    RectI GetOldDirtyInSurface() const;

    void SetVisibleRegion(const Occlusion::Region& visibleRegion);
    Occlusion::Region GetVisibleRegion() const;

    void SetVisibleRegionInVirtual(const Occlusion::Region& visibleRegion);
    Occlusion::Region GetVisibleRegionInVirtual() const;

    void SetOccludedByFilterCache(bool val);
    bool GetOccludedByFilterCache() const;

    void SetLayerInfo(const RSLayerInfo& layerInfo);
    const RSLayerInfo& GetLayerInfo() const;
    void SetHardwareEnabled(bool enabled);
    bool GetHardwareEnabled() const;
    void SetLastFrameHardwareEnabled(bool enabled);
    bool GetLastFrameHardwareEnabled() const;
    void SetForceHardwareByUser(bool flag);
    bool GetForceHardwareByUser() const;

    void SetGpuOverDrawBufferOptimizeNode(bool overDrawNode);
    bool IsGpuOverDrawBufferOptimizeNode() const;
    void SetOverDrawBufferNodeCornerRadius(const Vector4f& radius);
    const Vector4f& GetOverDrawBufferNodeCornerRadius() const;

    void SetIsSubSurfaceNode(bool isSubSurfaceNode);
    bool IsSubSurfaceNode() const;

    void SetIsNodeToBeCaptured(bool isNodeToBeCaptured);
    bool IsNodeToBeCaptured() const;

    void SetSkipDraw(bool skip);
    bool GetSkipDraw() const;

    bool IsVisibleRegionEmpty(const Drawing::Region curSurfaceDrawRegion) const;

    void SetPreScalingMode(ScalingMode scalingMode)
    {
        preScalingMode_ = scalingMode;
    }
    ScalingMode GetPreScalingMode() const
    {
        return preScalingMode_;
    }

#ifndef ROSEN_CROSS_PLATFORM
    void SetBuffer(const sptr<SurfaceBuffer>& buffer);
    sptr<SurfaceBuffer> GetBuffer() const;
    void SetPreBuffer(const sptr<SurfaceBuffer>& preBuffer);
    sptr<SurfaceBuffer>& GetPreBuffer();
    void SetAcquireFence(const sptr<SyncFence>& acquireFence);
    sptr<SyncFence> GetAcquireFence() const;
#endif

    virtual void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    // DFX
    std::string ToString() const override;

    void SetNeedOffscreen(bool needOffscreen)
    {
        if (needOffscreen_ == needOffscreen) {
            return;
        }
        needOffscreen_ = needOffscreen;
        needSync_ = true;
    }

    bool GetNeedOffscreen() const
    {
        return needOffscreen_;
    }

protected:
private:
    bool isMainWindowType_ = false;
    bool isLeashWindow_ = false;
    bool isAppWindow_ = false;
    RSSurfaceNodeType rsSurfaceNodeType_ = RSSurfaceNodeType::DEFAULT;
    SelfDrawingNodeType selfDrawingType_ = SelfDrawingNodeType::DEFAULT;
    RSRenderNode::WeakPtr ancestorDisplayNode_;

    float alpha_ = 0;
    bool isTransparent_ = false;
    bool isSpherizeValid_ = false;
    bool isParentScaling_ = false;
    bool needBilinearInterpolation_ = false;
    MultiThreadCacheType uiFirstFlag_ = MultiThreadCacheType::NONE;
    bool uiFirstParentFlag_ = false;
    Color backgroundColor_ = RgbPalette::Transparent();

    RectI dstRect_;
    RectI oldDirtyInSurface_;
    RectI childrenDirtyRect_;
    RectI absDrawRect_;
    RRect rrect_;
    Occlusion::Region transparentRegion_;

    bool surfaceCacheContentStatic_ = false;
    bool preSurfaceCacheContentStatic_ = false;
    bool isSubTreeDirty_ = false;
    float positionZ_ = 0.0f;
    bool occlusionVisible_ = false;
    bool isLeashWindowVisibleRegionEmpty_ = false;
    Occlusion::Region visibleRegion_;
    Occlusion::Region visibleRegionInVirtual_;
    bool isOccludedByFilterCache_ = false;
    RSLayerInfo layerInfo_;
#ifndef ROSEN_CROSS_PLATFORM
    sptr<SurfaceBuffer> buffer_;
    sptr<SurfaceBuffer> preBuffer_;
    sptr<SyncFence> acquireFence_ = SyncFence::INVALID_FENCE;
#endif
    bool isHardwareEnabled_ = false;
    bool isLastFrameHardwareEnabled_ = false;
    bool isForceHardwareByUser_ = false;
    int32_t releaseInHardwareThreadTaskNum_ = 0;
    bool isSecurityLayer_ = false;
    bool isSkipLayer_ = false;
    bool isProtectedLayer_ = false;
    bool isSubSurfaceNode_ = false;
    Gravity uiFirstFrameGravity_ = Gravity::TOP_LEFT;
    bool isNodeToBeCaptured_ = false;
    std::set<NodeId> skipLayerIds_= {};
    std::set<NodeId> securityLayerIds_= {};
    std::set<NodeId> protectedLayerIds_= {};
    std::set<int32_t> bufferCacheSet_ = {};
    std::string name_= "";
    Vector4f overDrawBufferNodeCornerRadius_;
    bool isGpuOverDrawBufferOptimizeNode_ = false;
    bool isSkipDraw_ = false;
    ScalingMode preScalingMode_ = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    bool needOffscreen_ = false;

    friend class RSSurfaceRenderNode;
    friend class RSUniRenderProcessor;
    friend class RSUniRenderThread;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_SURFACE_RENDER_PARAMS_H
