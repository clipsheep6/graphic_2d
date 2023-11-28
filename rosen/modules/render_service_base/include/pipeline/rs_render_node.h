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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H

#include <atomic>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include <bitset>

#include "animation/rs_animation_manager.h"
#include "animation/rs_frame_rate_range.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "memory/rs_dfx_string.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_single_frame_composer.h"
#include "property/rs_properties.h"
#include "property/rs_property_drawable.h"

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#else
#include "draw/surface.h"
#include "image/gpu_context.h"
#endif

#ifndef USE_ROSEN_DRAWING
class SkCanvas;
#endif
namespace OHOS {
namespace Rosen {
class DrawCmdList;
class RSContext;
class RSNodeVisitor;
class RSCommand;
class RSPropertyDrawable;
class RSB_EXPORT RSRenderNode : public std::enable_shared_from_this<RSRenderNode>  {
public:

    using WeakPtr = std::weak_ptr<RSRenderNode>;
    using SharedPtr = std::shared_ptr<RSRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::RS_NODE;
    virtual RSRenderNodeType GetType() const
    {
        return Type;
    }

    explicit RSRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {});
    explicit RSRenderNode(NodeId id, bool isOnTheTree, const std::weak_ptr<RSContext>& context = {});
    RSRenderNode(const RSRenderNode&) = delete;
    RSRenderNode(const RSRenderNode&&) = delete;
    RSRenderNode& operator=(const RSRenderNode&) = delete;
    RSRenderNode& operator=(const RSRenderNode&&) = delete;
    virtual ~RSRenderNode();

    void AddChild(SharedPtr child, int index = -1);
    void SetContainBootAnimation(bool isContainBootAnimation);
    bool GetContainBootAnimation() const;
    virtual void SetBootAnimation(bool isBootAnimation);
    virtual bool GetBootAnimation() const;

    void MoveChild(SharedPtr child, int index);
    void RemoveChild(SharedPtr child, bool skipTransition = false);
    void ClearChildren();
    void RemoveFromTree(bool skipTransition = false);

    // Add/RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens)
    void AddCrossParentChild(const SharedPtr& child, int32_t index = -1);
    void RemoveCrossParentChild(const SharedPtr& child, const WeakPtr& newParent);

    virtual void CollectSurface(const std::shared_ptr<RSRenderNode>& node,
                                std::vector<RSRenderNode::SharedPtr>& vec,
                                bool isUniRender,
                                bool onlyFirstLevel);
    virtual void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor);
    virtual void Process(const std::shared_ptr<RSNodeVisitor>& visitor);
    bool IsDirty() const;
    // attention: current all base node's dirty ops causing content dirty
    // if there is any new dirty op, check it
    bool IsContentDirty() const;
    void SetContentDirty();
    bool IsOnlyBasicGeoTransfrom() const
    {
        return isOnlyBasicGeoTransform_;
    }

    WeakPtr GetParent() const;

    inline NodeId GetId() const
    {
        return id_;
    }

    inline const std::list<WeakPtr> &GetChildren() const noexcept
    {
        return children_;
    }

    inline const std::map<NodeId, std::vector<WeakPtr>> &GetSubSurfaceNodes() const
    {
        return subSurfaceNodes_;
    }
    bool IsFirstLevelSurfaceNode();
    bool SubSurfaceNodeNeedDraw(PartialRenderType opDropType);
    void AddSubSurfaceNode(SharedPtr child, SharedPtr parent);
    void RemoveSubSurfaceNode(SharedPtr child, SharedPtr parent);
    inline bool GetSubSurfaceEnabled() const
    {
        return isSubSurfaceEnabled_;
    }

    // flag: isOnTheTree; instanceRootNodeId: displaynode or leash/appnode attached to
    // firstLevelNodeId: surfacenode for uiFirst to assign task; cacheNodeId: drawing cache rootnode attached to
    virtual void SetIsOnTheTree(bool flag, NodeId instanceRootNodeId = INVALID_NODEID,
        NodeId firstLevelNodeId = INVALID_NODEID, NodeId cacheNodeId = INVALID_NODEID);
    bool IsOnTheTree() const;

    // return children and disappeared children, not guaranteed to be sorted by z-index
    const std::list<SharedPtr>& GetChildren(bool inSubThread = false);
    // return children and disappeared children, sorted by z-index
    const std::list<SharedPtr>& GetSortedChildren(bool inSubThread = false);
    uint32_t GetChildrenCount() const;
    void ClearFullChildrenListIfNeeded(bool inSubThread = false);

    void DumpTree(int32_t depth, std::string& ou) const;
    void DumpNodeInfo(DfxString& log);

    virtual bool HasDisappearingTransition(bool recursive = true) const;

    void SetTunnelHandleChange(bool change);
    bool GetTunnelHandleChange() const;

    // type-safe reinterpret_cast
    template<typename T>
    bool IsInstanceOf() const
    {
        constexpr auto targetType = static_cast<uint32_t>(T::Type);
        return (static_cast<uint32_t>(GetType()) & targetType) == targetType;
    }
    template<typename T>
    static std::shared_ptr<T> ReinterpretCast(std::shared_ptr<RSRenderNode> node)
    {
        return node ? node->ReinterpretCastTo<T>() : nullptr;
    }
    template<typename T>
    std::shared_ptr<T> ReinterpretCastTo()
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<T>(shared_from_this()) : nullptr;
    }

    bool HasChildrenOutOfRect() const;
    void UpdateChildrenOutOfRectFlag(bool flag);

    void ResetHasRemovedChild();
    bool HasRemovedChild() const;
    void ResetChildrenRect();
    RectI GetChildrenRect() const;

    bool ChildHasFilter() const;
    void SetChildHasFilter(bool childHasFilter);

    NodeId GetInstanceRootNodeId() const;
    const std::shared_ptr<RSRenderNode> GetInstanceRootNode() const;
    NodeId GetFirstLevelNodeId() const;

    // accumulate all valid children's area
    void UpdateChildrenRect(const RectI& subRect);
    void SetDirty();

    virtual void AddDirtyType(RSModifierType type)
    {
#ifndef USE_ROSEN_DRAWING
        dirtyTypes_.emplace(type);
#else
        dirtyTypes_.set(static_cast<int>(type), true);
#endif
    }

    std::tuple<bool, bool, bool> Animate(int64_t timestamp);

    bool IsClipBound() const;
    // clipRect has value in UniRender when calling PrepareCanvasRenderNode, else it is nullopt
    bool Update(RSDirtyRegionManager& dirtyManager, const std::shared_ptr<RSRenderNode>& parent, bool parentDirty,
        std::optional<RectI> clipRect = std::nullopt);
#ifndef USE_ROSEN_DRAWING
    virtual std::optional<SkRect> GetContextClipRegion() const { return std::nullopt; }
#else
    virtual std::optional<Drawing::Rect> GetContextClipRegion() const { return std::nullopt; }
#endif

    RSProperties& GetMutableRenderProperties();
    const RSProperties& GetRenderProperties() const;
    void UpdateRenderStatus(RectI& dirtyRegion, bool isPartialRenderEnabled);
    bool IsRenderUpdateIgnored() const;

    // used for animation test
    RSAnimationManager& GetAnimationManager();

    void ApplyBoundsGeometry(RSPaintFilterCanvas& canvas);
    void ApplyAlpha(RSPaintFilterCanvas& canvas);
    virtual void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas);

    virtual void ProcessRenderContents(RSPaintFilterCanvas& canvas) {}

    virtual void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas);
    virtual void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) {}
    virtual void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas);

    void RenderTraceDebug() const;
    bool ShouldPaint() const;

    RectI GetOldDirty() const;
    RectI GetOldDirtyInSurface() const;
    bool IsDirtyRegionUpdated() const;
    void CleanDirtyRegionUpdated();

    void AddModifier(const std::shared_ptr<RSRenderModifier>& modifier, bool isSingleFrameComposer = false);
    void RemoveModifier(const PropertyId& id);
    std::shared_ptr<RSRenderModifier> GetModifier(const PropertyId& id);
    void ApplyChildrenModifiers();

    bool IsShadowValidLastFrame() const;
    void SetShadowValidLastFrame(bool isShadowValidLastFrame)
    {
        isShadowValidLastFrame_ = isShadowValidLastFrame;
    }

    // update parent's children rect including childRect and itself
    void UpdateParentChildrenRect(std::shared_ptr<RSRenderNode> parentNode) const;
    void UpdateFilterCacheManagerWithCacheRegion(
        RSDirtyRegionManager& dirtyManager, const std::optional<RectI>& clipRect = std::nullopt) const;

    void SetStaticCached(bool isStaticCached);
    bool IsStaticCached() const;

    bool NeedInitCacheSurface() const;
    bool NeedInitCacheCompletedSurface() const;
    inline bool IsPureContainer() const
    {
        return (drawCmdModifiers_.empty() && !renderProperties_.isDrawn_ && !renderProperties_.alphaNeedApply_);
    }

    bool IsContentNode() const
    {
        return ((drawCmdModifiers_.size() == 1 && drawCmdModifiers_.count(RSModifierType::CONTENT_STYLE)) ||
            drawCmdModifiers_.empty()) && !renderProperties_.isDrawn_;
    }

#ifndef USE_ROSEN_DRAWING
    using ClearCacheSurfaceFunc = std::function<void(sk_sp<SkSurface>&&, sk_sp<SkSurface>&&, uint32_t, uint32_t)>;
#ifdef NEW_SKIA
    void InitCacheSurface(GrRecordingContext* grContext, ClearCacheSurfaceFunc func = nullptr,
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);
#else
    void InitCacheSurface(GrContext* grContext, ClearCacheSurfaceFunc func = nullptr,
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);
#endif
#else
    using ClearCacheSurfaceFunc =
        std::function<void(std::shared_ptr<Drawing::Surface>&&,
        std::shared_ptr<Drawing::Surface>&&, uint32_t, uint32_t)>;
    void InitCacheSurface(Drawing::GPUContext* grContext, ClearCacheSurfaceFunc func = nullptr,
        uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);
#endif

    Vector2f GetOptionalBufferSize() const;

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> GetCacheSurface() const
#else
    std::shared_ptr<Drawing::Surface> GetCacheSurface() const
#endif
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        return cacheSurface_;
    }

// use for uni render visitor
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> GetCacheSurface(uint32_t threadIndex, bool needCheckThread, bool releaseAfterGet = false);
#else
    std::shared_ptr<Drawing::Surface> GetCacheSurface(uint32_t threadIndex, bool needCheckThread,
        bool releaseAfterGet = false);
#endif

    void UpdateCompletedCacheSurface();
    void SetTextureValidFlag(bool isValid);
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> GetCompletedCacheSurface(uint32_t threadIndex = UNI_MAIN_THREAD_INDEX,
        bool needCheckThread = true, bool releaseAfterGet = false);
#else
    std::shared_ptr<Drawing::Surface> GetCompletedCacheSurface(uint32_t threadIndex = UNI_MAIN_THREAD_INDEX,
        bool needCheckThread = true, bool releaseAfterGet = false);
#endif
    void ClearCacheSurfaceInThread();
    void ClearCacheSurface(bool isClearCompletedCacheSurface = true);
    bool IsCacheSurfaceValid() const;

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    void UpdateBackendTexture();
#endif

    void DrawCacheSurface(RSPaintFilterCanvas& canvas, uint32_t threadIndex = UNI_MAIN_THREAD_INDEX,
        bool isUIFirst = false);

    void SetCacheType(CacheType cacheType);
    CacheType GetCacheType() const;

    void SetCacheSurfaceNeedUpdated(bool isCacheSurfaceNeedUpdate)
    {
        isCacheSurfaceNeedUpdate_ = isCacheSurfaceNeedUpdate;
    }

    bool GetCacheSurfaceNeedUpdated() const
    {
        return isCacheSurfaceNeedUpdate_;
    }

    int GetShadowRectOffsetX() const;
    int GetShadowRectOffsetY() const;

    void SetDrawingCacheType(RSDrawingCacheType cacheType);
    RSDrawingCacheType GetDrawingCacheType() const;
    void ResetFilterRectsInCache(const std::unordered_set<NodeId>& curRects);
    void GetFilterRectsInCache(std::unordered_map<NodeId, std::unordered_set<NodeId>>& allRects) const;
    void SetDrawingCacheChanged(bool cacheChanged);
    bool GetDrawingCacheChanged() const;
    void ResetDrawingCacheNeedUpdate();
    void SetVisitedCacheRootIds(const std::unordered_set<NodeId>& visitedNodes);
    const std::unordered_set<NodeId>& GetVisitedCacheRootIds() const;
    // manage cache root nodeid
    void SetDrawingCacheRootId(NodeId id);
    NodeId GetDrawingCacheRootId() const;
    // record cache geodirty for preparation optimization
    void SetCacheGeoPreparationDelay(bool val);
    void ResetCacheGeoPreparationDelay();
    bool GetCacheGeoPreparationDelay() const;

    // driven render ///////////////////////////////////
    void SetIsMarkDriven(bool isMarkDriven);
    bool IsMarkDriven() const;
    void SetIsMarkDrivenRender(bool isMarkDrivenRender);
    bool IsMarkDrivenRender() const;

    void SetItemIndex(int index);
    int GetItemIndex() const;

    void SetPaintState(bool paintState);
    bool GetPaintState() const;

    void SetIsContentChanged(bool isChanged);
    bool IsContentChanged() const;

    bool HasAnimation() const;

    bool HasFilter() const;
    void SetHasFilter(bool hasFilter);

    std::recursive_mutex& GetSurfaceMutex() const;

    bool HasHardwareNode() const;
    void SetHasHardwareNode(bool hasHardwareNode);

    bool HasAbilityComponent() const;
    void SetHasAbilityComponent(bool hasAbilityComponent);
    bool QuerySubAssignable(bool isRotation) const;

    uint32_t GetCacheSurfaceThreadIndex() const;

    uint32_t GetCompletedSurfaceThreadIndex() const;

    bool IsMainThreadNode() const;
    void SetIsMainThreadNode(bool isMainThreadNode);

    bool IsScale() const;
    void SetIsScale(bool isScale);

    void SetPriority(NodePriorityType priority);
    NodePriorityType GetPriority();

    bool IsAncestorDirty() const;
    void SetIsAncestorDirty(bool isAncestorDirty);

    bool IsParentLeashWindow() const;
    void SetParentLeashWindow();

    bool IsParentScbScreen() const;
    void SetParentScbScreen();

    bool HasCachedTexture() const;

    void SetDrawRegion(const std::shared_ptr<RectF>& rect);
    const std::shared_ptr<RectF>& GetDrawRegion() const;
    void SetOutOfParent(OutOfParentType outOfParent);
    OutOfParentType GetOutOfParent() const;

#ifndef USE_ROSEN_DRAWING
    void UpdateEffectRegion(std::optional<SkPath>& region);
#else
    void UpdateEffectRegion(std::optional<Drawing::Path>& region);
#endif
    void UpdateFilterCacheWithDirty(RSDirtyRegionManager& dirtyManager, bool isForeground=true) const;

    void CheckGroupableAnimation(const PropertyId& id, bool isAnimAdd);
    bool IsForcedDrawInGroup() const;
    bool IsSuggestedDrawInGroup() const;
    void CheckDrawingCacheType();
    bool HasCacheableAnim() const { return hasCacheableAnim_; }
    enum NodeGroupType {
        NONE = 0,
        GROUPED_BY_ANIM,
        GROUPED_BY_UI,
        GROUPED_BY_USER,
    };
    void MarkNodeGroup(NodeGroupType type, bool isNodeGroup);
    NodeGroupType GetNodeGroupType();

    void MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer, pid_t pid = 0);
    virtual bool GetNodeIsSingleFrameComposer() const;

    /////////////////////////////////////////////

    // shared transition params, in format <InNodeId, target weakPtr>, nullopt means no transition
    using SharedTransitionParam = std::pair<NodeId, std::weak_ptr<RSRenderNode>>;
    void SetSharedTransitionParam(const std::optional<SharedTransitionParam>&& sharedTransitionParam);
    const std::optional<SharedTransitionParam>& GetSharedTransitionParam() const;

    void SetGlobalAlpha(float alpha);
    float GetGlobalAlpha() const;
    virtual void OnAlphaChanged() {}

    void SetRSFrameRateRange(FrameRateRange range);
    const FrameRateRange& GetRSFrameRateRange();
    void UpdateUIFrameRateRange(const FrameRateRange& range);
    const FrameRateRange& GetUIFrameRateRange() const;

    void ResetRSFrameRateRange();
    void ResetUIFrameRateRange();

    void MarkNonGeometryChanged();
    const std::vector<HgmModifierProfile>& GetHgmModifierProfileList();
    void SetRSFrameRateRangeByPreferred(int32_t preferred);
    bool ApplyModifiers();

    virtual RectI GetFilterRect() const;
    void SetIsUsedBySubThread(bool isUsedBySubThread);
    bool GetIsUsedBySubThread() const;

    bool IsCalPreferredNode() {
        return isCalPreferredNode_;
    }
    void SetCalPreferredNode(bool isCalPreferredNode) {
        isCalPreferredNode_ = isCalPreferredNode;
    }

protected:
    virtual void OnApplyModifiers() {}

    enum class NodeDirty {
        CLEAN = 0,
        DIRTY,
    };
    void SetClean();

    void DumpNodeType(std::string& out) const;

    const std::weak_ptr<RSContext> GetContext() const
    {
        return context_;
    }
    virtual void OnTreeStateChanged();

    static void SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId);
    void AddGeometryModifier(const std::shared_ptr<RSRenderModifier>& modifier);
    RSPaintFilterCanvas::SaveStatus renderNodeSaveCount_;
    std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>> drawCmdModifiers_;
    std::shared_ptr<RSSingleFrameComposer> singleFrameComposer_ = nullptr;
    bool isNodeSingleFrameComposer_ = false;
    // if true, it means currently it's in partial render mode and this node is intersect with dirtyRegion
    bool isRenderUpdateIgnored_ = false;
    bool isShadowValidLastFrame_ = false;

    bool IsSelfDrawingNode() const;
    bool isOnTheTree_ = false;
    NodeId drawingCacheRootId_ = INVALID_NODEID;

#ifndef USE_ROSEN_DRAWING
    std::unordered_set<RSModifierType> dirtyTypes_;
#else
    std::bitset<static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE)> dirtyTypes_;
#endif
    bool isFullChildrenListValid_ = false;
    bool isBootAnimation_ = false;
    RSProperties renderProperties_;
    void IterateOnDrawableRange(
        Slot::RSPropertyDrawableSlot begin, Slot::RSPropertyDrawableSlot end, RSPaintFilterCanvas& canvas);

private:
    NodeId id_;
    NodeId instanceRootNodeId_ = INVALID_NODEID;
    NodeId firstLevelNodeId_ = INVALID_NODEID;

    WeakPtr parent_;
    void SetParent(WeakPtr parent);
    void ResetParent();
    virtual void OnResetParent() {}

    std::list<WeakPtr> children_;
    std::list<std::pair<SharedPtr, uint32_t>> disappearingChildren_;

    std::list<SharedPtr> fullChildrenList_;
    bool isChildrenSorted_ = false;
    void GenerateFullChildrenList(bool inSubThread);
    void SortChildren(bool inSubThread);

    const std::weak_ptr<RSContext> context_;
    NodeDirty dirtyStatus_ = NodeDirty::CLEAN;
    bool isContentDirty_ = false;
    bool isOnlyBasicGeoTransform_ = true;
    friend class RSRenderPropertyBase;
    friend class RSRenderTransition;
    std::atomic<bool> isTunnelHandleChange_ = false;
    // accumulate all children's region rect for dirty merging when any child has been removed
    bool hasRemovedChild_ = false;
    bool hasChildrenOutOfRect_ = false;
    RectI childrenRect_;
    bool childHasFilter_ = false;  // only collect children filter status

    void InternalRemoveSelfFromDisappearingChildren();
    void FallbackAnimationsToRoot();
    void FilterModifiersByPid(pid_t pid);
    inline void AddActiveNode();

    void UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager, bool geoDirty, std::optional<RectI> clipRect);
    void AddModifierProfile(const std::shared_ptr<RSRenderModifier>& modifier, float width, float height);
    void UpdateFullScreenFilterCacheRect(RSDirtyRegionManager& dirtyManager, bool isForeground) const;

    void UpdateShouldPaint(); // update node should paint state in apply modifier stage
    bool shouldPaint_ = true;

    bool isDirtyRegionUpdated_ = false;
    bool isContainBootAnimation_ = false;
    bool isLastVisible_ = false;
    bool fallbackAnimationOnDestroy_ = true;
    uint32_t disappearingTransitionCount_ = 0;
    RectI oldDirty_;
    RectI oldDirtyInSurface_;
    RSAnimationManager animationManager_;
    std::map<PropertyId, std::shared_ptr<RSRenderModifier>> modifiers_;
    // bounds and frame modifiers must be unique
    std::shared_ptr<RSRenderModifier> boundsModifier_;
    std::shared_ptr<RSRenderModifier> frameModifier_;

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> GetCompletedImage(RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst);
    sk_sp<SkSurface> cacheSurface_ = nullptr;
    sk_sp<SkSurface> cacheCompletedSurface_ = nullptr;
#else
    std::shared_ptr<Drawing::Image> GetCompletedImage(
        RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst);
    std::shared_ptr<Drawing::Surface> cacheSurface_ = nullptr;
    std::shared_ptr<Drawing::Surface> cacheCompletedSurface_ = nullptr;
#endif
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
#ifndef USE_ROSEN_DRAWING
    GrBackendTexture cacheBackendTexture_;
    GrBackendTexture cacheCompletedBackendTexture_;
#else
    Drawing::BackendTexture cacheBackendTexture_;
    Drawing::BackendTexture cacheCompletedBackendTexture_;
#endif
    bool isTextureValid_ = false;
#endif
    std::atomic<bool> isCacheSurfaceNeedUpdate_ = false;
    std::atomic<bool> isStaticCached_ = false;
    CacheType cacheType_ = CacheType::NONE;
    // drawing group cache
    RSDrawingCacheType drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;
    bool isDrawingCacheChanged_ = false;
    bool drawingCacheNeedUpdate_ = false;
    // since cache preparation optimization would skip child's dirtyFlag(geoDirty) update
    // it should be recorded and update if marked dirty again
    bool cacheGeoPreparationDelay_ = false;

    std::unordered_set<NodeId> curCacheFilterRects_ = {};
    std::unordered_set<NodeId> visitedCacheRoots_ = {};

    mutable std::recursive_mutex surfaceMutex_;
    std::mutex mutex_;
    ClearCacheSurfaceFunc clearCacheSurfaceFunc_ = nullptr;
    uint32_t cacheSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    uint32_t completedSurfaceThreadIndex_ = UNI_MAIN_THREAD_INDEX;
    bool isMainThreadNode_ = true;
    bool isScale_ = false;
    bool hasFilter_ = false;
    bool hasHardwareNode_ = false;
    bool hasAbilityComponent_ = false;
    bool isAncestorDirty_ = false;
    bool isParentLeashWindow_ = false;
    bool isParentScbScreen_ = false;
    NodePriorityType priority_ = NodePriorityType::MAIN_PRIORITY;

    // driven render
    int itemIndex_ = -1;
    bool isMarkDriven_ = false;
    bool isMarkDrivenRender_ = false;
    bool paintState_ = false;
    bool isContentChanged_ = false;
    OutOfParentType outOfParent_ = OutOfParentType::UNKNOWN;
    float globalAlpha_ = 1.0f;
    std::optional<SharedTransitionParam> sharedTransitionParam_;

    std::shared_ptr<RectF> drawRegion_ = nullptr;
    NodeGroupType nodeGroupType_ = NodeGroupType::NONE;

    // shadowRectOffset means offset between shadowRect and absRect of node
    int shadowRectOffsetX_ = 0;
    int shadowRectOffsetY_ = 0;
    // Only use in RSRenderNode::DrawCacheSurface to calculate scale factor
    float boundsWidth_ = 0.0f;
    float boundsHeight_ = 0.0f;
    bool hasCacheableAnim_ = false;
    bool geometryChangeNotPerceived_ = false;

    std::atomic_bool isUsedBySubThread_ = false;

    FrameRateRange rsRange_ = {0, 0, 0};
    FrameRateRange uiRange_ = {0, 0, 0};

    int64_t lastTimestamp_ = -1;
    int64_t lastApplyTimestamp_ = -1;
    float timeDelta_ = -1;
    std::unordered_map<PropertyId, std::variant<float, Vector2f, Vector4f>> propertyValueMap_;
    std::vector<HgmModifierProfile> hgmModifierProfileList_;

    std::vector<std::unique_ptr<RSPropertyDrawable>> propertyDrawablesVec_;
    uint8_t drawableVecStatus_ = 0;
    void UpdateDrawableVec();
    bool isCalPreferredNode_ = true;

    bool isSubSurfaceEnabled_ = false;
    std::map<NodeId, std::vector<WeakPtr>> subSurfaceNodes_;
    pid_t appPid_ = 0;

    friend class RSAliasDrawable;
    friend class RSMainThread;
    friend class RSModifierDrawable;
    friend class RSProxyRenderNode;
    friend class RSRenderNodeMap;
    friend class RSRenderTransition;
};
// backward compatibility
using RSBaseRenderNode = RSRenderNode;
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_NODE_H
