/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H

#include <functional>
#include <limits>
#include <memory>
#include <tuple>

#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#ifndef NEW_SKIA
#include "include/gpu/GrContext.h"
#include "refbase.h"
#endif

#include "common/rs_macros.h"
#include "common/rs_occlusion_region.h"
#include "common/rs_vector4.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "memory/rs_memory_track.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "property/rs_properties_painter.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_occlusion_data.h"

namespace OHOS {
namespace Rosen {
class RSCommand;
class RSDirtyRegionManager;
class RSB_EXPORT RSSurfaceRenderNode : public RSRenderNode, public RSSurfaceHandler {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::SURFACE_NODE;
    RSRenderNodeType GetType() const override
    {
        return Type;
    }

    explicit RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    explicit RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context = {});
    ~RSSurfaceRenderNode() override;

    void PrepareRenderBeforeChildren(RSPaintFilterCanvas& canvas);
    void PrepareRenderAfterChildren(RSPaintFilterCanvas& canvas);
    void ResetParent() override;

    bool IsAppWindow() const
    {
        return nodeType_ == RSSurfaceNodeType::APP_WINDOW_NODE;
    }

    bool IsStartingWindow() const
    {
        return nodeType_ == RSSurfaceNodeType::STARTING_WINDOW_NODE;
    }

    bool IsAbilityComponent() const
    {
        return nodeType_ == RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    }

    bool IsExtensionAbility() const
    {
        return nodeType_ == RSSurfaceNodeType::EXTENSION_ABILITY_NODE;
    }

    bool IsLeashWindow() const
    {
        return nodeType_ == RSSurfaceNodeType::LEASH_WINDOW_NODE;
    }

    // indicate if this node type can enable hardware composer
    bool IsHardwareEnabledType() const
    {
        // [PLANNING] enable hardware composer for all self-drawing node
        return nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE && name_ != "RosenWeb" && name_ != "RosenRenderWeb";
    }

    bool IsLastFrameHardwareEnabled() const
    {
        return isLastFrameHardwareEnabled_;
    }

    void MarkCurrentFrameHardwareEnabled()
    {
        isCurrentFrameHardwareEnabled_ = true;
    }

    void ResetCurrentFrameHardwareEnabledState()
    {
        isLastFrameHardwareEnabled_ = isCurrentFrameHardwareEnabled_;
        isCurrentFrameHardwareEnabled_ = false;
    }

    void ResetHardwareEnabledStates()
    {
        isLastFrameHardwareEnabled_ = false;
        isCurrentFrameHardwareEnabled_ = false;
    }

    void SetHardwareForcedDisabledState(bool forcesDisabled)
    {
        isHardwareForcedDisabled_ = forcesDisabled;
    }

    bool IsHardwareForcedDisabled() const
    {
        return isHardwareForcedDisabled_ ||
            GetDstRect().GetWidth() <= 1 || GetDstRect().GetHeight() <= 1; // avoid fallback by composer
    }

    bool IsMainWindowType() const
    {
        // a mainWindowType surfacenode will not mounted under another mainWindowType surfacenode
        // incluing app main window, starting window, and selfdrawing window
        return nodeType_ == RSSurfaceNodeType::APP_WINDOW_NODE ||
               nodeType_ == RSSurfaceNodeType::STARTING_WINDOW_NODE ||
               nodeType_ == RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    }

    bool IsSelfDrawingType() const
    {
        // self drawing surfacenode has its own buffer, and rendered in its own progress/thread
        // such as surfaceview (web/videos) and self draw windows (such as mouse pointer and boot animation)
        return nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE ||
               nodeType_ == RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    }

    RSSurfaceNodeType GetSurfaceNodeType() const
    {
        return nodeType_;
    }

    void SetSurfaceNodeType(RSSurfaceNodeType nodeType)
    {
        if (nodeType_ != RSSurfaceNodeType::ABILITY_COMPONENT_NODE) {
            nodeType_ = nodeType;
        }
    }

    void MarkUIHidden(bool isHidden);
    bool IsUIHidden() const;

    std::string GetName() const
    {
        return name_;
    }

    void SetOffSetX(int32_t offset)
    {
        offsetX_ = offset;
    }

    int32_t GetOffSetX()
    {
        return offsetX_;
    }

    void SetOffSetY(int32_t offset)
    {
        offsetY_ = offset;
    }

    int32_t GetOffSetY()
    {
        return offsetY_;
    }

    void SetOffset(int32_t offsetX, int32_t offsetY)
    {
        offsetX_ = offsetX;
        offsetY_ = offsetY;
    }

    void CollectSurface(const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec,
        bool isUniRender) override;
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas) override {}
    void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override;

    void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas) override {}
    void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas) override;

    void SetContextBounds(const Vector4f bounds);

    void OnApplyModifiers() override;

    void SetTotalMatrix(const SkMatrix& totalMatrix)
    {
        totalMatrix_ = totalMatrix;
    }
    const SkMatrix& GetTotalMatrix() const
    {
        return totalMatrix_;
    }

    // Transfer the rendering context variables (matrix, alpha, and clipRegion) from the source node (in the render thread) to the
    // target node (in the render service). Note that:
    // - All three variables are relative to their parent node.
    // - Alpha can be processed as an absolute value, as its parent (surface) node's alpha should always be 1.0f.
    // - The matrix and clipRegion should be applied according to the parent node's matrix.
    void SetContextMatrix(const std::optional<SkMatrix>& transform, bool sendMsg = true);
    void SetContextAlpha(float alpha, bool sendMsg = true);
    void SetContextClipRegion(const std::optional<SkRect>& clipRegion, bool sendMsg = true);
    std::optional<SkRect> GetContextClipRegion() const override;

    void SetSecurityLayer(bool isSecurityLayer);
    bool GetSecurityLayer() const;

    void SetFingerprint(bool hasFingerprint);
    bool GetFingerprint() const;

    std::shared_ptr<RSDirtyRegionManager> GetDirtyManager() const;

    void SetSrcRect(const RectI& rect)
    {
        srcRect_ = rect;
    }

    const RectI& GetSrcRect() const
    {
        return srcRect_;
    }

    void SetDstRect(const RectI& dstRect)
    {
        if (dstRect_ != dstRect) {
            dstRectChanged_ = true;
        }
        dstRect_ = dstRect;
    }

    const RectI& GetDstRect() const
    {
        return dstRect_;
    }

    Occlusion::Region& GetTransparentRegion()
    {
        return transparentRegion_;
    }

    Occlusion::Region& GetOpaqueRegion()
    {
        return opaqueRegion_;
    }

    Occlusion::Region& GetContainerRegion()
    {
        return containerRegion_;
    }

    void OnAlphaChanged() override {
        alphaChanged_ = true;
    }

    void SetOcclusionVisible(bool visible)
    {
        isOcclusionVisible_ = visible;
    }

    bool GetOcclusionVisible() const
    {
        return isOcclusionVisible_;
    }

    const Occlusion::Region& GetVisibleRegion() const
    {
        return visibleRegion_;
    }

    void SetAbilityBGAlpha(uint8_t alpha)
    {
        abilityBgAlpha_ = alpha;
        alphaChanged_ = true;
    }

    uint8_t GetAbilityBgAlpha() const
    {
        return abilityBgAlpha_;
    }

    void setQosCal(bool qosPidCal)
    {
        qosPidCal_ = qosPidCal;
    }

    void SetVisibleRegionRecursive(
        const Occlusion::Region& region, VisibleData& visibleVec, std::map<uint32_t, bool>& pidVisMap);

    const Occlusion::Region& GetVisibleDirtyRegion() const
    {
        return visibleDirtyRegion_;
    }

    void SetVisibleDirtyRegion(const Occlusion::Region& region)
    {
        visibleDirtyRegion_ = region;
    }

    void SetAlignedVisibleDirtyRegion(const Occlusion::Region& alignedRegion)
    {
        alignedVisibleDirtyRegion_ = alignedRegion;
    }

    const Occlusion::Region& GetAlignedVisibleDirtyRegion()
    {
        return alignedVisibleDirtyRegion_;
    }

    void SetExtraDirtyRegionAfterAlignment(const Occlusion::Region& region)
    {
        extraDirtyRegionAfterAlignment_ = region;
        extraDirtyRegionAfterAlignmentIsEmpty_ = extraDirtyRegionAfterAlignment_.IsEmpty();
    }

    void SetDirtyRegionAlignedEnable(bool enable)
    {
        isDirtyRegionAlignedEnable_ = enable;
    }

    const Occlusion::Region& GetDirtyRegionBelowCurrentLayer() const
    {
        return dirtyRegionBelowCurrentLayer_;
    }

    void SetDirtyRegionBelowCurrentLayer(Occlusion::Region& region)
    {
#ifndef ROSEN_CROSS_PLATFORM
        Occlusion::Rect dirtyRect { GetOldDirtyInSurface() };
        Occlusion::Region dirtyRegion { dirtyRect };
        dirtyRegionBelowCurrentLayer_ = dirtyRegion.And(region);
        dirtyRegionBelowCurrentLayerIsEmpty_ = dirtyRegionBelowCurrentLayer_.IsEmpty();
#endif
    }

    bool GetDstRectChanged() const
    {
        return dstRectChanged_;
    }

    void CleanDstRectChanged()
    {
        dstRectChanged_ = false;
    }

    bool GetAlphaChanged() const
    {
        return alphaChanged_;
    }

    void CleanAlphaChanged()
    {
        alphaChanged_ = false;
    }

    void SetGlobalDirtyRegion(const RectI& rect)
    {
        Occlusion::Rect tmpRect { rect.left_, rect.top_, rect.GetRight(), rect.GetBottom() };
        Occlusion::Region region { tmpRect };
        globalDirtyRegion_ = visibleRegion_.And(region);
        globalDirtyRegionIsEmpty_ = globalDirtyRegion_.IsEmpty();
    }

    const Occlusion::Region& GetGlobalDirtyRegion() const
    {
        return globalDirtyRegion_;
    }

    void SetLocalZOrder(float localZOrder);
    float GetLocalZOrder() const;

#ifndef ROSEN_CROSS_PLATFORM
    void SetColorSpace(ColorGamut colorSpace);
    ColorGamut GetColorSpace() const;
    void SetConsumer(const sptr<IConsumerSurface>& consumer);
    void SetBlendType(GraphicBlendType blendType);
    GraphicBlendType GetBlendType();
#endif

    void UpdateSurfaceDefaultSize(float width, float height);

    // Only SurfaceNode in RS calls "RegisterBufferAvailableListener"
    // to save callback method sent by RT or UI which depends on the value of "isFromRenderThread".
    void RegisterBufferAvailableListener(sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread);

    // Only SurfaceNode in RT calls "ConnectToNodeInRenderService" to send callback method to RS
    void ConnectToNodeInRenderService();

    void NotifyRTBufferAvailable();
    bool IsNotifyRTBufferAvailable() const;
    bool IsNotifyRTBufferAvailablePre() const;

    void NotifyUIBufferAvailable();
    bool IsNotifyUIBufferAvailable() const;
    void SetIsNotifyUIBufferAvailable(bool available);

    // UI Thread would not be notified when SurfaceNode created by Video/Camera in RenderService has available buffer.
    // And RenderThread does not call mainFunc_ if nothing in UI thread is changed
    // which would cause callback for "clip" on parent SurfaceNode cannot be triggered
    // for "clip" is executed in RenderThreadVisitor::ProcessSurfaceRenderNode.
    // To fix this bug, we set callback which would call RSRenderThread::RequestNextVSync() to forcedly "refresh"
    // RenderThread when SurfaceNode in RenderService has available buffer and execute RSIBufferAvailableCallback.
    void SetCallbackForRenderThreadRefresh(std::function<void(void)> callback);
    bool NeedSetCallbackForRenderThreadRefresh();

    void ParallelVisitLock()
    {
        parallelVisitMutex_.lock();
    }

    void ParallelVisitUnlock()
    {
        parallelVisitMutex_.unlock();
    }

    bool SubNodeVisible(const RectI& r) const
    {
        Occlusion::Rect nodeRect { r.left_, r.top_, r.GetRight(), r.GetBottom() };
        // if current node is in occluded region of the surface, it could be skipped in process step
        return visibleRegion_.IsIntersectWith(nodeRect);
    }

    inline bool IsTransparent() const
    {
        const uint8_t opacity = 255;
        return !(GetAbilityBgAlpha() == opacity && ROSEN_EQ(GetGlobalAlpha(), 1.0f));
    }

    inline bool IsCurrentNodeInTransparentRegion(const Occlusion::Rect& nodeRect) const
    {
        return transparentRegion_.IsIntersectWith(nodeRect);
    }

    bool SubNodeIntersectWithDirty(const RectI& r) const;

    // judge if a rect r is intersect with existing dirtyregion, include current surfacenode's dirtyregion, display
    // dirtyregion, and dirtyregion from other surfacenode because of 32/64 bits alignment.
    bool SubNodeNeedDraw(const RectI& r, PartialRenderType opDropType) const;

    bool GetZorderChanged() const
    {
        return (std::abs(GetRenderProperties().GetPositionZ() - positionZ_) > (std::numeric_limits<float>::epsilon()));
    }

    bool IsZOrderPromoted() const
    {
        return GetRenderProperties().GetPositionZ() > positionZ_;
    }

    void UpdatePositionZ()
    {
        positionZ_ = GetRenderProperties().GetPositionZ();
    }

    inline bool HasContainerWindow() const
    {
        return containerConfig_.hasContainerWindow_;
    }

    void SetContainerWindow(bool hasContainerWindow, float density)
    {
        containerConfig_.Update(hasContainerWindow, density);
    }

    bool IsOpaqueRegionChanged() const
    {
        return opaqueRegionChanged_;
    }

    // [planning] Remove this after skia is upgraded, the clipRegion is supported
    void ResetChildrenFilterRects();
    void UpdateChildrenFilterRects(const RectI& rect);
    const std::vector<RectI>& GetChildrenNeedFilterRects() const;

    // manage abilities' nodeid info
    void ResetAbilityNodeIds();
    void UpdateAbilityNodeIds(NodeId id);
    const std::vector<NodeId>& GetAbilityNodeIds() const;

    // manage appWindowNode's child hardware enabled nodes info
    void ResetChildHardwareEnabledNodes();
    void AddChildHardwareEnabledNode(WeakPtr childNode);
    std::vector<WeakPtr> GetChildHardwareEnabledNodes() const;

    bool IsFocusedWindow(pid_t focusedWindowPid)
    {
        return ExtractPid(GetNodeId()) == focusedWindowPid;
    }

    void ResetSurfaceOpaqueRegion(
        const RectI& screeninfo, const RectI& absRect, const ScreenRotation screenRotation, const bool isFocusWindow);
    Occlusion::Region ResetOpaqueRegion(
        const RectI& absRect, const ScreenRotation screenRotation, const bool isFocusWindow) const;
    Occlusion::Region SetUnfocusedWindowOpaqueRegion(const RectI& absRect, const ScreenRotation screenRotation) const;
    Occlusion::Region SetFocusedWindowOpaqueRegion(const RectI& absRect, const ScreenRotation screenRotation) const;
    Occlusion::Region SetCornerRadiusOpaqueRegion(const RectI& absRect, float radius) const;
    void ResetSurfaceContainerRegion(const RectI& screeninfo, const RectI& absRect,
        const ScreenRotation screenRotation);

    bool IsStartAnimationFinished() const;
    void SetStartAnimationFinished();
    void SetCachedImage(sk_sp<SkImage> image)
    {
        SetDirty();
        std::lock_guard<std::mutex> lock(cachedImageMutex_);
        cachedImage_ = image;
    }

    sk_sp<SkImage> GetCachedImage() const
    {
        std::lock_guard<std::mutex> lock(cachedImageMutex_);
        return cachedImage_;
    }

    void ClearCachedImage()
    {
        std::lock_guard<std::mutex> lock(cachedImageMutex_);
        cachedImage_ = nullptr;
    }

    // if surfacenode's buffer has been comsumed, it should be set dirty
    bool UpdateDirtyIfFrameBufferConsumed();

    void UpdateSrcRect(const RSPaintFilterCanvas& canvas, const SkIRect& dstRect);

    // if a surfacenode's dstrect is empty, its subnodes' prepare stage can be skipped
    bool ShouldPrepareSubnodes();

    void SetNodeCost(int32_t cost)
    {
        nodeCost_ = cost;
    }

    int32_t GetNodeCost() const
    {
        return nodeCost_;
    }

    std::string DirtyRegionDump() const;
    void SetAnimateState() {
        animateState_ = true;
    }
    void ResetAnimateState() {
        animateState_ = false;
    }
    bool GetAnimateState() const{
        return animateState_;
    }
    bool LeashWindowRelatedAppWindowOccluded();

    void OnTreeStateChanged() override;
#ifndef NEW_SKIA
    void SetGrContext(GrContext* grContext) {
        grContext_ = grContext;
    }
#endif

private:
    void ClearChildrenCache(const std::shared_ptr<RSBaseRenderNode>& node);
    bool SubNodeIntersectWithExtraDirtyRegion(const RectI& r) const;
    Vector4f GetWindowCornerRadius();

    std::mutex mutexRT_;
    std::mutex mutexUI_;
    std::mutex mutex_;
#ifndef NEW_SKIA
    GrContext* grContext_;
#endif
    std::mutex parallelVisitMutex_;

    float contextAlpha_ = 1.0f;
    std::optional<SkMatrix> contextMatrix_;
    std::optional<SkRect> contextClipRect_;

    bool isSecurityLayer_ = false;
    bool hasFingerprint_ = false;
    RectI srcRect_;
    SkMatrix totalMatrix_;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    float positionZ_ = 0.0f;
    bool qosPidCal_ = false;

    std::string name_;
    RSSurfaceNodeType nodeType_ = RSSurfaceNodeType::DEFAULT;
#ifndef ROSEN_CROSS_PLATFORM
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    GraphicBlendType blendType_ = GraphicBlendType::GRAPHIC_BLEND_SRCOVER;
#endif
    bool isNotifyRTBufferAvailablePre_ = false;
    std::atomic<bool> isNotifyRTBufferAvailable_ = false;
    std::atomic<bool> isNotifyUIBufferAvailable_ = true;
    std::atomic_bool isBufferAvailable_ = false;
    sptr<RSIBufferAvailableCallback> callbackFromRT_;
    sptr<RSIBufferAvailableCallback> callbackFromUI_;
    std::function<void(void)> callbackForRenderThreadRefresh_ = nullptr;
    std::vector<NodeId> childSurfaceNodeIds_;
    friend class RSRenderThreadVisitor;
    RectI clipRegionFromParent_;
    Occlusion::Region visibleRegion_;
    Occlusion::Region visibleDirtyRegion_;
    bool isDirtyRegionAlignedEnable_ = false;
    Occlusion::Region alignedVisibleDirtyRegion_;
    bool isOcclusionVisible_ = true;
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_ = nullptr;
    RectI dstRect_;
    bool dstRectChanged_ = false;
    uint8_t abilityBgAlpha_ = 0;
    bool alphaChanged_ = false;
    bool isUIHidden_ = false;
    Occlusion::Region globalDirtyRegion_;
    // dirtyRegion caused by surfaceNode visible region after alignment
    Occlusion::Region extraDirtyRegionAfterAlignment_;
    bool extraDirtyRegionAfterAlignmentIsEmpty_ = true;

    bool globalDirtyRegionIsEmpty_ = false;
    // if a there a dirty layer under transparent clean layer, transparent layer should refreshed
    Occlusion::Region dirtyRegionBelowCurrentLayer_;
    bool dirtyRegionBelowCurrentLayerIsEmpty_ = false;

    // opaque region of the surface
    Occlusion::Region opaqueRegion_;
    bool opaqueRegionChanged_ = false;
    // [planning] Remove this after skia is upgraded, the clipRegion is supported
    std::vector<RectI> childrenFilterRects_;
    std::vector<NodeId> abilityNodeIds_;
    // transparent region of the surface, floating window's container window is always treated as transparent
    Occlusion::Region transparentRegion_;

    Occlusion::Region containerRegion_;

    /*
        ContainerWindow configs acquired from arkui, including container window state, screen density, container border
        width, padding width, inner/outer radius, etc.
    */
    class ContarinerConfig {
    public:
        void Update(bool hasContainer, float density);
    private:
        inline int RoundFloor(float length)
        {
            // if a float value is very close to a integer (< 0.05f), return round value
            return std::abs(length - std::round(length)) < 0.05f ? std::round(length) : std::floor(length);
        }
    public:
        // temporary const value from ACE container_modal_constants.h, will be replaced by uniform interface
        const static int CONTAINER_TITLE_HEIGHT = 37;   // container title height = 37 vp
        const static int CONTENT_PADDING = 4;           // container <--> content distance 4 vp
        const static int CONTAINER_BORDER_WIDTH = 1;    // container border width 2 vp
        const static int CONTAINER_OUTER_RADIUS = 16;   // container outter radius 16 vp
        const static int CONTAINER_INNER_RADIUS = 14;   // container inner radius 14 vp

        bool hasContainerWindow_ = false;               // set to false as default, set by arkui
        float density = 2.0f;                           // The density default value is 2
        int outR = 32;                                  // outer radius (int value)
        int inR = 28;                                   // inner radius (int value)
        int bp = 10;                                    // border width + padding (int value)
        int bt = 76;                                    // border width + title (int value)
    };

    ContarinerConfig containerConfig_;

    bool startAnimationFinished_ = false;
    mutable std::mutex cachedImageMutex_;
    sk_sp<SkImage> cachedImage_;

    // used for hardware enabled nodes
    bool isCurrentFrameHardwareEnabled_ = false;
    bool isLastFrameHardwareEnabled_ = false;
    // mark if this self-drawing node is forced not to use hardware composer
    // in case where this node's parent window node is occluded or is appFreeze, this variable will be marked true
    bool isHardwareForcedDisabled_ = false;
    float localZOrder_ = 0.0f;
    std::vector<WeakPtr> childHardwareEnabledNodes_;
    int32_t nodeCost_ = 0;

    bool animateState_ = false;

    bool needDrawAnimateProperty_ = false;

    friend class RSUniRenderVisitor;
    friend class RSBaseRenderNode;
    friend class RSRenderService;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
