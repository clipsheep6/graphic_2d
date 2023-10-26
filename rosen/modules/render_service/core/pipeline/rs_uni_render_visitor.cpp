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

#include "pipeline/rs_uni_render_visitor.h"

#ifdef RS_ENABLE_VK
#include <vulkan_window.h>
#endif

#include "draw/color.h"
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkRegion.h"
#include "include/core/SkTextBlob.h"
#else
#include "recording/recording_canvas.h"
#endif

#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"
#include "pipeline/parallel_render/rs_parallel_render_manager.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "system/rs_system_parameters.h"
#include "scene_board_judgement.h"
#include "hgm_core.h"
#include "benchmarks/rs_recording_thread.h"
#include "scene_board_judgement.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t PHONE_MAX_APP_WINDOW_NUM = 1;
constexpr uint32_t CACHE_MAX_UPDATE_TIME = 2;
constexpr int ROTATION_90 = 90;
constexpr int ROTATION_270 = 270;
static const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
static std::map<NodeId, uint32_t> cacheRenderNodeMap = {};
static uint32_t cacheReuseTimes = 0;
static std::mutex cacheRenderNodeMapMutex;
static std::mutex groupedTransitionNodesMutex;
using groupedTransitionNodesType = std::unordered_map<NodeId, std::pair<RSUniRenderVisitor::RenderParam,
    std::unordered_map<NodeId, RSUniRenderVisitor::RenderParam>>>;
static std::unordered_map<NodeId, std::pair<RSUniRenderVisitor::RenderParam,
    std::unordered_map<NodeId, RSUniRenderVisitor::RenderParam>>> groupedTransitionNodes = {};

bool CheckRootNodeReadyToDraw(const std::shared_ptr<RSBaseRenderNode>& child)
{
    if (child != nullptr && child->IsInstanceOf<RSRootRenderNode>()) {
        auto rootNode = child->ReinterpretCastTo<RSRootRenderNode>();
        const auto& property = rootNode->GetRenderProperties();
        if (property.GetFrameWidth() > 0 && property.GetFrameHeight() > 0 && rootNode->GetEnableRender()) {
            return true;
        }
    }
    return false;
}

bool IsFirstFrameReadyToDraw(RSSurfaceRenderNode& node)
{
    bool result = false;
    for (auto& child : node.GetSortedChildren()) {
        result = CheckRootNodeReadyToDraw(child);
        // when appWindow has abilityComponent node
        if (child != nullptr && child->IsInstanceOf<RSSurfaceRenderNode>()) {
            for (const auto& surfaceNodeChild : child->GetSortedChildren()) {
                result = CheckRootNodeReadyToDraw(surfaceNodeChild);
            }
        }
    }
    return result;
}
}

#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
constexpr uint32_t PARALLEL_RENDER_MINIMUM_RENDER_NODE_NUMBER = 50;
#endif

RSUniRenderVisitor::RSUniRenderVisitor()
    : curSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    auto mainThread = RSMainThread::Instance();
    renderEngine_ = mainThread->GetRenderEngine();
    partialRenderType_ = RSSystemProperties::GetUniPartialRenderEnabled();
    quickSkipPrepareType_ = RSSystemParameters::GetQuickSkipPrepareType();
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    auto screenNum = screenManager->GetAllScreenIds().size();
    isPartialRenderEnabled_ = (screenNum <= 1) && (partialRenderType_ != PartialRenderType::DISABLED) &&
        mainThread->IsSingleDisplay();
    dirtyRegionDebugType_ = RSSystemProperties::GetDirtyRegionDebugType();
    surfaceRegionDebugType_ = RSSystemProperties::GetSurfaceRegionDfxType();
    isRegionDebugEnabled_ = (dirtyRegionDebugType_ != DirtyRegionDebugType::DISABLED) ||
        (surfaceRegionDebugType_ != SurfaceRegionDebugType::DISABLED);
    isVisibleRegionDfxEnabled_ = (surfaceRegionDebugType_ == SurfaceRegionDebugType::VISIBLE_REGION);
    isOpaqueRegionDfxEnabled_ = (surfaceRegionDebugType_ == SurfaceRegionDebugType::OPAQUE_REGION);
    isTargetDirtyRegionDfxEnabled_ = RSSystemProperties::GetTargetDirtyRegionDfxEnabled(dfxTargetSurfaceNames_) &&
        (surfaceRegionDebugType_ == SurfaceRegionDebugType::DISABLED);
    isDirtyRegionDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::EGL_DAMAGE);
    isDisplayDirtyDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::DISPLAY_DIRTY);
    isCanvasNodeSkipDfxEnabled_ = (dirtyRegionDebugType_ == DirtyRegionDebugType::CANVAS_NODE_SKIP_RECT);
    isOcclusionEnabled_ = RSSystemProperties::GetOcclusionEnabled();
    isOpDropped_ = isPartialRenderEnabled_ &&
        (partialRenderType_ != PartialRenderType::SET_DAMAGE) && !isRegionDebugEnabled_;
    isQuickSkipPreparationEnabled_ = (quickSkipPrepareType_ != QuickSkipPrepareType::DISABLED);
    isDrawingCacheEnabled_ = RSSystemParameters::GetDrawingCacheEnabled();
    RSTagTracker::UpdateReleaseGpuResourceEnable(RSSystemProperties::GetReleaseGpuResourceEnabled());
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (RSDrivenRenderManager::GetInstance().GetDrivenRenderEnabled()) {
        drivenInfo_ = std::make_unique<DrivenInfo>();
    }
#endif
    surfaceNodePrepareMutex_ = std::make_shared<std::mutex>();
    parallelRenderType_ = ParallelRenderingType::DISABLE;
#if defined(RS_ENABLE_PARALLEL_RENDER)
    isCalcCostEnable_ = RSSystemParameters::GetCalcCostEnabled();
#endif
#if defined(RS_ENABLE_GL)
    if (renderEngine_ && renderEngine_->GetRenderContext()) {
        auto subThreadManager = RSSubThreadManager::Instance();
        subThreadManager->Start(renderEngine_->GetRenderContext().get());
    }
#endif
    isUIFirst_ = RSMainThread::Instance()->IsUIFirstOn();
    isPhone_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
}

RSUniRenderVisitor::RSUniRenderVisitor(std::shared_ptr<RSPaintFilterCanvas> canvas, uint32_t surfaceIndex)
    : RSUniRenderVisitor()
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    parallelRenderVisitorIndex_ = surfaceIndex;
#if defined(RS_ENABLE_GL)
    canvas_ = canvas;
#endif
#endif
}

RSUniRenderVisitor::RSUniRenderVisitor(const RSUniRenderVisitor& visitor) : RSUniRenderVisitor()
{
    currentVisitDisplay_ = visitor.currentVisitDisplay_;
    screenInfo_ = visitor.screenInfo_;
    displayHasSecSurface_ = visitor.displayHasSecSurface_;
    displayHasSkipSurface_ = visitor.displayHasSkipSurface_;
    parentSurfaceNodeMatrix_ = visitor.parentSurfaceNodeMatrix_;
    curAlpha_ = visitor.curAlpha_;
    dirtyFlag_ = visitor.dirtyFlag_;
    curDisplayNode_ = visitor.curDisplayNode_;
    currentFocusedNodeId_ = visitor.currentFocusedNodeId_;
    surfaceNodePrepareMutex_ = visitor.surfaceNodePrepareMutex_;
    prepareClipRect_ = visitor.prepareClipRect_;
    isOpDropped_ = visitor.isOpDropped_;
    isPartialRenderEnabled_ = visitor.isPartialRenderEnabled_;
    isHardwareForcedDisabled_ = visitor.isHardwareForcedDisabled_;
    doAnimate_ = visitor.doAnimate_;
    isDirty_ = visitor.isDirty_;
}

RSUniRenderVisitor::~RSUniRenderVisitor() {}

void RSUniRenderVisitor::CopyVisitorInfos(std::shared_ptr<RSUniRenderVisitor> visitor)
{
    std::unique_lock<std::mutex> lock(copyVisitorInfosMutex_);
    currentVisitDisplay_ = visitor->currentVisitDisplay_;
    screenInfo_ = visitor->screenInfo_;
    displayHasSecSurface_ = visitor->displayHasSecSurface_;
    displayHasSkipSurface_ = visitor->displayHasSkipSurface_;
    parentSurfaceNodeMatrix_ = visitor->parentSurfaceNodeMatrix_;
    curAlpha_ = visitor->curAlpha_;
    dirtyFlag_ = visitor->dirtyFlag_;
    curDisplayNode_ = visitor->curDisplayNode_;
    currentFocusedNodeId_ = visitor->currentFocusedNodeId_;
    surfaceNodePrepareMutex_ = visitor->surfaceNodePrepareMutex_;
    prepareClipRect_ = visitor->prepareClipRect_;
    isOpDropped_ = visitor->isOpDropped_;
    isPartialRenderEnabled_ = visitor->isPartialRenderEnabled_;
    isHardwareForcedDisabled_ = visitor->isHardwareForcedDisabled_;
    doAnimate_ = visitor->doAnimate_;
    isDirty_ = visitor->isDirty_;
}

void RSUniRenderVisitor::CopyPropertyForParallelVisitor(RSUniRenderVisitor *mainVisitor)
{
    if (!mainVisitor) {
        RS_LOGE("main thread visitor is nullptr");
        return;
    }
    doAnimate_ = mainVisitor->doAnimate_;
    isParallel_ = mainVisitor->isParallel_;
    isUpdateCachedSurface_ = mainVisitor->isUpdateCachedSurface_;
    isHardwareForcedDisabled_ = mainVisitor->isHardwareForcedDisabled_;
    isOpDropped_ = mainVisitor->isOpDropped_;
    isPartialRenderEnabled_ = mainVisitor->isPartialRenderEnabled_;
    isUIFirst_ = mainVisitor->isUIFirst_;
    isSubThread_ = true;
}

void RSUniRenderVisitor::UpdateStaticCacheSubTree(const std::shared_ptr<RSRenderNode>& cacheRootNode,
    const std::list<RSRenderNode::SharedPtr>& children)
{
    for (auto& child : children) {
        if (child == nullptr) {
            continue;
        }
        if (child->GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE) {
            child->SetDrawingCacheChanged(false);
        }
        bool isUpdated = false;
        // [planning] pay attention to outofparent case
        // since subtree is 'clean', effect node does not need fully preparation
        if (auto surfaceNode = child->ReinterpretCastTo<RSSurfaceRenderNode>()) {
            if (surfaceNode->IsHardwareEnabledType()) {
                PrepareSurfaceRenderNode(*surfaceNode);
                isUpdated = true;
            }
            if (surfaceNode->GetSecurityLayer() && curSurfaceNode_ &&
                curSurfaceNode_->GetId() == surfaceNode->GetInstanceRootNodeId()) {
                curSurfaceNode_->SetHasSecurityLayer(true);
                displayHasSecSurface_[currentVisitDisplay_] = true;
            }
            if (surfaceNode->GetSkipLayer() && curSurfaceNode_ &&
                curSurfaceNode_->GetId() == surfaceNode->GetInstanceRootNodeId() &&
                surfaceNode->GetName().find(CAPTURE_WINDOW_NAME) == std::string::npos) {
                curSurfaceNode_->SetHasSkipLayer(true);
                displayHasSkipSurface_[currentVisitDisplay_] = true;
            }
        }
        if (child->GetRenderProperties().NeedFilter()) {
            if (!isUpdated) {
                child->Update(*curSurfaceDirtyManager_, cacheRootNode, dirtyFlag_, prepareClipRect_);
            }
            UpdateForegroundFilterCacheWithDirty(*child, *curSurfaceDirtyManager_);
            if (curSurfaceNode_ && curSurfaceNode_->GetId() == child->GetInstanceRootNodeId()) {
                curSurfaceNode_->UpdateChildrenFilterRects(child->GetOldDirtyInSurface());
            }
        }
        UpdateStaticCacheSubTree(cacheRootNode, child->GetSortedChildren());
    }
}

void RSUniRenderVisitor::PrepareChildren(RSRenderNode& node)
{
    // GetSortedChildren() may remove disappearingChildren_ when transition animation end.
    // So the judgement whether node has removed child should be executed after this.
    // merge last childRect as dirty if any child has been removed
    // NOTE: removal of transition node is moved to RSMainThread::Animate
    if (curSurfaceDirtyManager_ && node.HasRemovedChild()) {
        RectI dirtyRect = prepareClipRect_.IntersectRect(node.GetChildrenRect());
        if (isSubNodeOfSurfaceInPrepare_) {
            curSurfaceDirtyManager_->MergeDirtyRect(dirtyRect);
            if (curSurfaceDirtyManager_->IsTargetForDfx()) {
                // since childRect includes multiple rects, defaultly marked as canvas_node
                curSurfaceDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
                    DirtyRegionType::REMOVE_CHILD_RECT, dirtyRect);
            }
        } else {
            curDisplayDirtyManager_->MergeDirtyRect(dirtyRect);
        }
        node.ResetHasRemovedChild();
    }

    // backup environment variables.
    auto parentNode = std::move(logicParentNode_);
    logicParentNode_ = node.weak_from_this();
    node.ResetChildrenRect();

    float alpha = curAlpha_;
    curAlpha_ *= node.GetRenderProperties().GetAlpha();
    node.SetGlobalAlpha(curAlpha_);
    auto children = node.GetSortedChildren();
    // check curSurfaceDirtyManager_ for UpdateStaticCacheSubTree calls
    if (UpdateCacheChangeStatus(node) || curSurfaceDirtyManager_ == nullptr) {
        for (auto& child : children) {
            if (UNLIKELY(child->GetSharedTransitionParam().has_value())) {
                firstVisitedCache_ = INVALID_NODEID;
                PrepareSharedTransitionNode(*child);
            }
            dirtyCoverSubTree_ = node.GetDirtyCoverSubTree();
            curDirty_ = child->IsDirty();
            child->Prepare(shared_from_this());
        }
        SetNodeCacheChangeStatus(node);
    } else {
        RS_OPTIONAL_TRACE_NAME_FMT("UpdateCacheChangeStatus quick skip node %llu", node.GetId());
        UpdateStaticCacheSubTree(node.ReinterpretCastTo<RSRenderNode>(), children);
    }
    
    if (node.GetOutOfParent() == OutOfParentType::UNKNOWN) {
        node.UpdateDirtyCoverSubTree();
    }
    curAlpha_ = alpha;
    // restore environment variables
    logicParentNode_ = std::move(parentNode);
}

bool RSUniRenderVisitor::IsDrawingCacheStatic(RSRenderNode& node)
{
    // since this function only called by drawing group root
    // if cache valid, cacheRenderNodeMapCnt > 0
    // check all dirtynodes of app instance if there's any in cache subtree
    if (curContentDirty_ || node.GetDrawingCacheChanged() || !node.IsCacheSurfaceValid() ||
        curSurfaceNode_ == nullptr || curSurfaceNode_->GetId() != node.GetInstanceRootNodeId() ||
        RSMainThread::Instance()->IsDrawingGroupChanged(node)) {
        return false;
    }
    // simplify Cache status reset
    node.GetFilterRectsInCache(allCacheFilterRects_);
    node.SetDrawingCacheChanged(false);
    if (allCacheFilterRects_.count(node.GetId())) {
        node.SetChildHasFilter(true);
        if (const auto directParent = node.GetParent().lock()) {
            directParent->SetChildHasFilter(true);
        }
    }
    return true;
}

bool RSUniRenderVisitor::UpdateCacheChangeStatus(RSRenderNode& node)
{
    node.SetChildHasFilter(false);
    if (!isDrawingCacheEnabled_) {
        return true;
    }
    node.CheckDrawingCacheType();
    if (!node.ShouldPaint() || isSurfaceRotationChanged_) {
        node.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    }
    // skip status check if there is no upper cache mark
    if (node.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE && firstVisitedCache_ == INVALID_NODEID) {
        return true;
    }
    // subroot's dirty and cached filter should be count for parent root
    if (!isDrawingCacheChanged_.empty()) {
        // Any child node dirty causes cache change
        isDrawingCacheChanged_.top() = isDrawingCacheChanged_.top() || curDirty_;
        if (!curCacheFilterRects_.empty() && !node.IsInstanceOf<RSEffectRenderNode>() &&
            (node.GetRenderProperties().GetBackgroundFilter() || node.GetRenderProperties().GetUseEffect())) {
            curCacheFilterRects_.top().emplace(node.GetId());
        }
    }
    // drawing group root node
    if (node.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE) {
        if (isPhone_ && (quickSkipPrepareType_ == QuickSkipPrepareType::STATIC_CACHE) && IsDrawingCacheStatic(node)) {
            return false;
        }
        // For rootnode, init drawing changes only if there is any content dirty
        isDrawingCacheChanged_.push(curContentDirty_);
        RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::UpdateCacheChangeStatus: cachable node %" PRIu64 ""
            "contentDirty(cacheChanged): %d", node.GetId(), static_cast<int>(isDrawingCacheChanged_.top()));
        curCacheFilterRects_.push({});
        if (firstVisitedCache_ == INVALID_NODEID) {
            firstVisitedCache_ = node.GetId();
        }
    }
    return true;
}

void RSUniRenderVisitor::DisableNodeCacheInSetting(RSRenderNode& node)
{
    // Attention: filter node should be marked. Only enable lowest suggested cached node
    if (node.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE) {
        uint32_t cacheRenderNodeMapCnt;
        {
            std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
            cacheRenderNodeMapCnt = cacheRenderNodeMap.count(node.GetId());
        }
        // if cached node is clean, keep enable
        // otherwise (uncached or dirty node) disable cache if it has outOfParent
        // [planning] visitedCacheNodeIds_ check lowest cached node may reduce
        if ((cacheRenderNodeMapCnt == 0 || (isDrawingCacheChanged_.empty() ? true : isDrawingCacheChanged_.top())) &&
            (!visitedCacheNodeIds_.empty() || node.HasChildrenOutOfRect())) {
            node.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        }
    }
    if (firstVisitedCache_ == INVALID_NODEID) {
        node.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        std::stack<bool>().swap(isDrawingCacheChanged_);
        visitedCacheNodeIds_.clear();
    }
}

void RSUniRenderVisitor::SetNodeCacheChangeStatus(RSRenderNode& node)
{
    auto directParent = node.GetParent().lock();
    if (directParent != nullptr && node.ChildHasFilter()) {
        directParent->SetChildHasFilter(true);
    }
    if (!isDrawingCacheEnabled_ ||
        node.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE) {
        return;
    }

    DisableNodeCacheInSetting(node);
    if (!curCacheFilterRects_.empty()) {
        allCacheFilterRects_.emplace(node.GetId(), curCacheFilterRects_.top());
        node.ResetFilterRectsInCache(curCacheFilterRects_.top());
        curCacheFilterRects_.pop();
    }
    // update visited cache roots including itself
    visitedCacheNodeIds_.emplace(node.GetId());
    node.SetVisitedCacheRootIds(visitedCacheNodeIds_);
    bool isDrawingCacheChanged = isDrawingCacheChanged_.empty() ? true : isDrawingCacheChanged_.top();
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::SetNodeCacheChangeStatus: node %" PRIu64 " drawingtype %d, "
        "cacheChange %d, childHasFilter: %d, outofparent: %d, visitedCacheNodeIds num: %lu",
        node.GetId(), static_cast<int>(node.GetDrawingCacheType()),
        static_cast<int>(isDrawingCacheChanged), static_cast<int>(node.ChildHasFilter()),
        static_cast<int>(node.HasChildrenOutOfRect()), visitedCacheNodeIds_.size());
    node.SetDrawingCacheChanged(isDrawingCacheChanged);
    if (curSurfaceNode_) {
        curSurfaceNode_->UpdateDrawingCacheNodes(node.ReinterpretCastTo<RSRenderNode>());
    }
    // reset counter after executing the very first marked node
    if (firstVisitedCache_ == node.GetId()) {
        std::stack<bool>().swap(isDrawingCacheChanged_);
        firstVisitedCache_ = INVALID_NODEID;
        visitedCacheNodeIds_.clear();
    } else if (!isDrawingCacheChanged_.empty()) {
        bool isChildChanged = isDrawingCacheChanged_.top();
        isDrawingCacheChanged_.pop();
        if (!isDrawingCacheChanged_.empty()) {
            isDrawingCacheChanged_.top() = isDrawingCacheChanged_.top() || isChildChanged;
        }
    }
}

void RSUniRenderVisitor::CheckColorSpace(RSSurfaceRenderNode& node)
{
    if (node.IsAppWindow()) {
        auto surfaceNodeColorSpace = node.GetColorSpace();
        if (surfaceNodeColorSpace != GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB) {
            ROSEN_LOGD("RSUniRenderVisitor::CheckColorSpace: node (%{public}s) set new colorspace %{public}d",
                node.GetName().c_str(), surfaceNodeColorSpace);
            if (std::find(colorGamutModes_.begin(), colorGamutModes_.end(),
                static_cast<ScreenColorGamut>(surfaceNodeColorSpace)) != colorGamutModes_.end()) {
                newColorSpace_ = surfaceNodeColorSpace;
            } else {
                RS_LOGD("RSUniRenderVisitor::CheckColorSpace: colorSpace is not supported on current screen");
            }
        }
    }
}

void RSUniRenderVisitor::HandleColorGamuts(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager)
{
    screenManager->GetScreenSupportedColorGamuts(node.GetScreenId(), colorGamutModes_);
    for (auto& child : node.GetCurAllSurfaces()) {
        auto surfaceNodePtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
        if (!surfaceNodePtr) {
            RS_LOGD("RSUniRenderVisitor::PrepareDisplayRenderNode ReinterpretCastTo fail");
            continue;
        }
        CheckColorSpace(*surfaceNodePtr);
    }
}

void RSUniRenderVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    currentVisitDisplay_ = node.GetScreenId();
    displayHasSecSurface_.emplace(currentVisitDisplay_, false);
    displayHasSkipSurface_.emplace(currentVisitDisplay_, false);
    dirtySurfaceNodeMap_.clear();

    RS_TRACE_NAME("RSUniRender:PrepareDisplay " + std::to_string(currentVisitDisplay_));
    curDisplayDirtyManager_ = node.GetDirtyManager();
    if (!curDisplayDirtyManager_) {
        return;
    }
    // set 1st elem for display dirty
    accumulatedDirtyRegions_.emplace_back(RectI());
    curDisplayDirtyManager_->Clear();
    curDisplayNode_ = node.shared_from_this()->ReinterpretCastTo<RSDisplayRenderNode>();

    dirtyFlag_ = isDirty_;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::PrepareDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    prepareClipRect_.SetAll(0, 0, screenInfo_.width, screenInfo_.height);
#ifndef USE_ROSEN_DRAWING
    parentSurfaceNodeMatrix_ = SkMatrix::I();
#else
    parentSurfaceNodeMatrix_ = Drawing::Matrix();
#endif
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr != nullptr) {
        geoPtr->UpdateByMatrixFromSelf();
        parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
        if (geoPtr->IsNeedClientCompose()) {
            isHardwareForcedDisabled_ = true;
        }
    }
    dirtyFlag_ = dirtyFlag_ || node.IsRotationChanged();
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
    }
#endif
    // when display is in rotation state, occlusion relationship will be ruined,
    // hence partial-render quick-reject should be disabled.
    if(node.IsRotationChanged()) {
        isOpDropped_ = false;
        RS_TRACE_NAME("ClosePartialRender 1 RotationChanged");
    }
    node.UpdateRotation();
    curAlpha_ = node.GetRenderProperties().GetAlpha();
    isParallel_ = false;
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    ParallelPrepareDisplayRenderNodeChildrens(node);
#else
    PrepareChildren(node);
#endif
    auto mirrorNode = node.GetMirrorSource().lock();
    if (mirrorNode) {
        mirroredDisplays_.insert(mirrorNode->GetScreenId());
    }

    node.GetCurAllSurfaces().clear();
    node.CollectSurface(node.shared_from_this(), node.GetCurAllSurfaces(), true, false);
    HandleColorGamuts(node, screenManager);

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_) {
        RS_OPTIONAL_TRACE_NAME("RSUniRender:DrivenRenderPrepare");
        drivenInfo_->prepareInfo.hasInvalidScene = drivenInfo_->prepareInfo.hasInvalidScene ||
            node.GetChildrenCount() >= 8 || // default value, count > 8 means invalid scene
            isHardwareForcedDisabled_ || node.GetRotation() != ScreenRotation::ROTATION_0;
        drivenInfo_->prepareInfo.screenRect = RectI(0, 0, screenInfo_.width, screenInfo_.height),
        // prepare driven render tree
        RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(drivenInfo_->prepareInfo);
        // merge dirty rect for driven render
        auto uniDrivenRenderMode = RSDrivenRenderManager::GetInstance().GetUniDrivenRenderMode();
        if (uniDrivenRenderMode == DrivenUniRenderMode::RENDER_WITH_CLIP_HOLE &&
            drivenInfo_->surfaceDirtyManager != nullptr) {
            auto drivenRenderDirtyRect = RSDrivenRenderManager::GetInstance().GetUniRenderSurfaceClipHoleRect();
            RS_OPTIONAL_TRACE_NAME("merge driven render dirty rect: " + drivenRenderDirtyRect.ToString());
            drivenInfo_->surfaceDirtyManager->MergeDirtyRect(drivenRenderDirtyRect);
        }
    }
#endif
}

void RSUniRenderVisitor::ParallelPrepareDisplayRenderNodeChildrens(RSDisplayRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    doParallelRender_ = (node.GetChildrenCount() >= PARALLEL_RENDER_MINIMUM_RENDER_NODE_NUMBER) &&
                        (!doParallelComposition_);
    isParallel_ = AdaptiveSubRenderThreadMode(doParallelRender_) && parallelRenderManager->GetParallelMode();
    isDirtyRegionAlignedEnable_ = false;
    // we will open prepare parallel after check all properties.
    if (isParallel_ &&
        RSSystemProperties::GetPrepareParallelRenderingEnabled() != ParallelRenderingType::DISABLE) {
        parallelRenderManager->CopyPrepareVisitorAndPackTask(*this, node);
        parallelRenderManager->LoadBalanceAndNotify(TaskType::PREPARE_TASK);
        parallelRenderManager->WaitPrepareEnd(*this);
    } else {
        PrepareChildren(node);
    }
#endif
}

bool RSUniRenderVisitor::CheckIfSurfaceRenderNodeStatic(RSSurfaceRenderNode& node)
{
    // dirtyFlag_ includes leashWindow dirty
    // window layout change(e.g. move or zooming) | proxyRenderNode's cmd
    if (dirtyFlag_ || node.IsDirty() || node.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        return false;
    }
    if (curDisplayDirtyManager_) {
        accumulatedDirtyRegions_[0] = curDisplayDirtyManager_->GetCurrentFrameDirtyRegion();
    }
    // if node has to be prepared, it's not static
    bool isClassifyByRootNode = (quickSkipPrepareType_ >= QuickSkipPrepareType::STATIC_APP_INSTANCE);
    NodeId rootId = node.GetInstanceRootNodeId();
    if (RSMainThread::Instance()->CheckNodeHasToBePreparedByPid(
        isClassifyByRootNode ? rootId : node.GetId(), isClassifyByRootNode)) {
        return false;
    }
    if (node.IsMainWindowType()) {
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        // [Attention] node's ability pid could be different but should have same rootId
        auto abilityNodeIds = node.GetAbilityNodeIds();
        bool result = isClassifyByRootNode
            ? RSMainThread::Instance()->CheckNodeHasToBePreparedByPid(rootId, true)
            : std::any_of(abilityNodeIds.begin(), abilityNodeIds.end(), [&](uint64_t nodeId) {
                return RSMainThread::Instance()->CheckNodeHasToBePreparedByPid(nodeId, false);
            });
        if (result) {
            return false;
        }
    }
    RS_OPTIONAL_TRACE_NAME("Skip static surface " + node.GetName() + " nodeid - pid: " +
        std::to_string(node.GetId()) + " - " + std::to_string(ExtractPid(node.GetId())));
    // static node's dirty region is empty
    curSurfaceDirtyManager_ = node.GetDirtyManager();
    if (curSurfaceDirtyManager_) {
        curSurfaceDirtyManager_->Clear();
        if (node.IsTransparent()) {
            curSurfaceDirtyManager_->UpdateVisitedDirtyRects(accumulatedDirtyRegions_);
        }
        node.UpdateFilterCacheStatusIfNodeStatic(prepareClipRect_);
    }
    node.ResetDrawingCacheStatusIfNodeStatic(allCacheFilterRects_);
    // static surface keeps same position
    curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), curDisplayNode_->GetLastFrameSurfacePos(node.GetId()));
    return true;
}

bool RSUniRenderVisitor::IsHardwareComposerEnabled()
{
    return !isHardwareForcedDisabled_;
}

void RSUniRenderVisitor::ClearTransparentBeforeSaveLayer()
{
    RS_TRACE_NAME("ClearTransparentBeforeSaveLayer");
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    for (auto& node : hardwareEnabledNodes_) {
        if (!node->ShouldPaint()) {
            continue;
        }
        auto dstRect = node->GetDstRect();
        if (dstRect.IsEmpty()) {
            continue;
        }
#ifndef USE_ROSEN_DRAWING
        canvas_->save();
        canvas_->clipRect({ static_cast<float>(dstRect.GetLeft()), static_cast<float>(dstRect.GetTop()),
                            static_cast<float>(dstRect.GetRight()), static_cast<float>(dstRect.GetBottom()) });
        canvas_->clear(SK_ColorTRANSPARENT);
        canvas_->restore();
#else
        canvas_->Save();
        canvas_->ClipRect({ static_cast<float>(dstRect.GetLeft()), static_cast<float>(dstRect.GetTop()),
                              static_cast<float>(dstRect.GetRight()), static_cast<float>(dstRect.GetBottom()) },
            Drawing::ClipOp::INTERSECT, false);
        canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        canvas_->Restore();
#endif
    }
}

void RSUniRenderVisitor::MarkSubHardwareEnableNodeState(RSSurfaceRenderNode& surfaceNode)
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }

    // hardware enabled type case: mark self
    if (surfaceNode.IsHardwareEnabledType()) {
        surfaceNode.SetHardwareForcedDisabledState(true);
        return;
    }

    if (!surfaceNode.IsAppWindow() && !surfaceNode.IsAbilityComponent() && !surfaceNode.IsLeashWindow()) {
        return;
    }

    // ability component type case: check pid
    if (surfaceNode.IsAbilityComponent()) {
        pid_t pid = ExtractPid(surfaceNode.GetId());
        for (auto& childNode : hardwareEnabledNodes_) {
            pid_t childPid = ExtractPid(childNode->GetId());
            if (pid == childPid) {
                childNode->SetHardwareForcedDisabledState(true);
            }
        }
        return;
    }
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    if (surfaceNode.IsAppWindow()) {
        hardwareEnabledNodes = surfaceNode.GetChildHardwareEnabledNodes();
    } else {
        for (auto& child : surfaceNode.GetChildren()) {
            auto appNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            if (appNode && appNode->IsAppWindow()) {
                hardwareEnabledNodes = appNode->GetChildHardwareEnabledNodes();
                break;
            }
        }
    }
    // app window type case: mark all child hardware enabled nodes
    for (auto& node : hardwareEnabledNodes) {
        auto childNode = node.lock();
        if (childNode) {
            childNode->SetHardwareForcedDisabledState(true);
        }
    }
}

void RSUniRenderVisitor::CollectAppNodeForHwc(std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (!IsHardwareComposerEnabled() || !surfaceNode || surfaceNode->GetChildHardwareEnabledNodes().empty()) {
        return;
    }

    if (isParallel_ && !isUIFirst_) {
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
        RSParallelRenderManager::Instance()->AddAppWindowNode(parallelRenderVisitorIndex_, surfaceNode);
#endif
    } else {
        if (surfaceNode->IsHardwareEnabledTopSurface()) {
            hardwareEnabledTopNodes_.emplace_back(surfaceNode);
        } else {
            appWindowNodesInZOrder_.emplace_back(surfaceNode);
        }
    }
}

void RSUniRenderVisitor::PrepareTypesOfSurfaceRenderNodeBeforeUpdate(RSSurfaceRenderNode& node)
{
    // if current surfacenode is a main window type, reset the curSurfaceDirtyManager
    // reset leash window's dirtyManager pointer to avoid curSurfaceDirtyManager mis-pointing
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        node.SetFilterCacheFullyCovered(false);
        node.ResetFilterNodes();
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        curSurfaceDirtyManager_ = node.GetDirtyManager();
        if (curSurfaceDirtyManager_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::PrepareTypesOfSurfaceRenderNodeBeforeUpdate %{public}s has no"
                " SurfaceDirtyManager", node.GetName().c_str());
            return;
        }
        curSurfaceDirtyManager_->Clear();
        if (node.IsTransparent()) {
            curSurfaceDirtyManager_->UpdateVisitedDirtyRects(accumulatedDirtyRegions_);
        }
        curSurfaceDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
        if (isTargetDirtyRegionDfxEnabled_ && CheckIfSurfaceTargetedForDFX(node.GetName())) {
            curSurfaceDirtyManager_->MarkAsTargetForDfx();
        }
    }

    // collect app window node's child hardware enabled node
    if (node.IsHardwareEnabledType() && curSurfaceNode_) {
        curSurfaceNode_->AddChildHardwareEnabledNode(node.ReinterpretCastTo<RSSurfaceRenderNode>());
        node.SetLocalZOrder(localZOrder_++);
    }
}

void RSUniRenderVisitor::PrepareTypesOfSurfaceRenderNodeAfterUpdate(RSSurfaceRenderNode& node)
{
    if (!curSurfaceDirtyManager_) {
        return;
    }
    const auto& properties = node.GetRenderProperties();
    if (properties.NeedFilter()) {
        UpdateForegroundFilterCacheWithDirty(node, *curSurfaceDirtyManager_);
        if (auto parentNode = node.GetParent().lock()) {
            parentNode->SetChildHasFilter(true);
        }
        if (curSurfaceNode_) {
            curSurfaceNode_->UpdateFilterNodes(node.shared_from_this());
        }
    }
    if (node.IsMainWindowType()) {
        bool hasFilter = node.IsTransparent() && properties.NeedFilter();
        bool hasHardwareNode = !node.GetChildHardwareEnabledNodes().empty();
        bool hasAbilityComponent = !node.GetAbilityNodeIds().empty();
        auto rsParent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock());
        if (rsParent && rsParent->IsLeashWindow()) {
            rsParent->SetHasFilter(hasFilter);
            rsParent->SetHasHardwareNode(hasHardwareNode);
            rsParent->SetHasAbilityComponent(hasAbilityComponent);
        } else {
            node.SetHasFilter(hasFilter);
            node.SetHasHardwareNode(hasHardwareNode);
            node.SetHasAbilityComponent(hasAbilityComponent);
        }
#ifndef USE_ROSEN_DRAWING
        if (hasFilter && curSurfaceDirtyManager_->IfCacheableFilterRectFullyCover(node.GetOldDirtyInSurface())) {
            node.SetFilterCacheFullyCovered(true);
            RS_LOGD("SetFilterCacheFullyCovered surfacenode %{public}" PRIu64 " [%{public}s]",
                node.GetId(), node.GetName().c_str());
        }
        node.SetFilterCacheValid();
#endif
        RS_OPTIONAL_TRACE_NAME(node.GetName() + " PreparedNodes: " +
            std::to_string(preparedCanvasNodeInCurrentSurface_));
        preparedCanvasNodeInCurrentSurface_ = 0;
    }
    if (node.GetSecurityLayer() && curSurfaceNode_ && curSurfaceNode_->GetId() == node.GetInstanceRootNodeId()) {
        curSurfaceNode_->SetHasSecurityLayer(true);
        displayHasSecSurface_[currentVisitDisplay_] = true;
    }
    if (node.GetSkipLayer() && curSurfaceNode_ && curSurfaceNode_->GetId() == node.GetInstanceRootNodeId() &&
        node.GetName().find(CAPTURE_WINDOW_NAME) == std::string::npos) {
        curSurfaceNode_->SetHasSkipLayer(true);
        displayHasSkipSurface_[currentVisitDisplay_] = true;
    }
    // accumulate all visited dirty rects including leash window's shadow dirty
    if ((node.IsMainWindowType() || node.IsLeashWindow()) && curSurfaceDirtyManager_->IsCurrentFrameDirty()) {
        accumulatedDirtyRegions_.emplace_back(curSurfaceDirtyManager_->GetCurrentFrameDirtyRegion());
    }
}

void RSUniRenderVisitor::UpdateForegroundFilterCacheWithDirty(RSRenderNode& node,
    RSDirtyRegionManager& dirtyManager)
{
#ifndef USE_ROSEN_DRAWING
    node.UpdateFilterCacheManagerWithCacheRegion(prepareClipRect_);
    node.UpdateFilterCacheWithDirty(dirtyManager, true);
#endif
}

void RSUniRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_OPTIONAL_TRACE_NAME("RSUniRender::Prepare:[" + node.GetName() + "] pid: " +
        std::to_string(ExtractPid(node.GetId())) +
        ", nodeType " + std::to_string(static_cast<uint>(node.GetSurfaceNodeType())));
    if (node.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
        needCacheImg_ = true;
        captureWindowZorder_ = static_cast<uint32_t>(node.GetRenderProperties().GetPositionZ());
    } else {
        needCacheImg_ = captureWindowZorder_ > static_cast<uint32_t>(node.GetRenderProperties().GetPositionZ());
    }
    if (node.GetFingerprint() && node.GetBuffer() != nullptr) {
        hasFingerprint_ = true;
    }

    if (curDisplayNode_ == nullptr) {
        ROSEN_LOGE("RSUniRenderVisitor::PrepareSurfaceRenderNode, curDisplayNode_ is nullptr.");
        return;
    }
    // avoid EntryView upload texture while screen rotation
    if (node.GetName() == "EntryView") {
        node.SetStaticCached(curDisplayNode_->IsRotationChanged());
    }
    node.UpdatePositionZ();
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_ && (node.GetName() == "imeWindow" || node.GetName() == "RecentView")) {
        drivenInfo_->prepareInfo.hasInvalidScene = true;
    }
#endif
    // stop traversal if node keeps static
    if (isQuickSkipPreparationEnabled_ && CheckIfSurfaceRenderNodeStatic(node)) {
        // node type is mainwindow.
        if (node.GetHasSecurityLayer() && node.GetId() == node.GetInstanceRootNodeId()) {
            displayHasSecSurface_[currentVisitDisplay_] = true;
        }
        if (node.GetHasSkipLayer() && node.GetId() == node.GetInstanceRootNodeId() &&
            node.GetName().find(CAPTURE_WINDOW_NAME) == std::string::npos) {
            displayHasSkipSurface_[currentVisitDisplay_] = true;
        }
        return;
    }
    // reset HasSecurityLayer
    node.SetHasSecurityLayer(false);
    node.SetHasSkipLayer(false);
    node.CleanDstRectChanged();
    if (node.IsHardwareEnabledTopSurface()) {
        node.ResetSubNodeShouldPaint();
        node.ResetChildHardwareEnabledNodes();
    }
    curContentDirty_ = node.IsContentDirty();
    bool dirtyFlag = dirtyFlag_;

    RectI prepareClipRect = prepareClipRect_;
    bool isQuickSkipPreparationEnabled = isQuickSkipPreparationEnabled_;

    // update geoptr with ContextMatrix
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    auto& property = node.GetMutableRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (geoPtr == nullptr) {
        return;
    }
    // before node update, prepare node's setting by types
    PrepareTypesOfSurfaceRenderNodeBeforeUpdate(node);

    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareSurfaceRenderNode %{public}s curSurfaceDirtyManager is nullptr",
            node.GetName().c_str());
        return;
    }
    auto skipNodeMap = RSMainThread::Instance()->GetCacheCmdSkippedNodes();
    if (skipNodeMap.count(node.GetId()) != 0) {
        auto parentNode = node.GetParent().lock();
        auto rsParent = (parentNode);
        dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, rsParent, dirtyFlag_, prepareClipRect_);
        dirtyFlag_ = dirtyFlag;
        RS_TRACE_NAME(node.GetName() + " PreparedNodes cacheCmdSkiped");
        return;
    }
    // Update node properties, including position (dstrect), OldDirty()
    auto parentNode = node.GetParent().lock();
    auto rsParent = (parentNode);
    auto rsSurfaceParent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentNode);
    if (node.IsAppWindow() && rsSurfaceParent && rsSurfaceParent->IsLeashWindow()
        && rsSurfaceParent->GetDstRect().IsEmpty()) {
            prepareClipRect_ = RectI {0, 0, 0, 0};
    }
    dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, rsParent, dirtyFlag_, prepareClipRect_);

    // Calculate the absolute destination rectangle of the node, initialize with absolute bounds rect
    auto dstRect = geoPtr->GetAbsRect();
    // If the screen is expanded, intersect the destination rectangle with the screen rectangle
    dstRect = dstRect.IntersectRect(RectI(curDisplayNode_->GetDisplayOffsetX(), curDisplayNode_->GetDisplayOffsetY(),
        screenInfo_.width, screenInfo_.height));
    // Remove the offset of the screen
    dstRect = RectI(dstRect.left_ - curDisplayNode_->GetDisplayOffsetX(),
        dstRect.top_ - curDisplayNode_->GetDisplayOffsetY(), dstRect.GetWidth(), dstRect.GetHeight());
    // If the node is a hardware-enabled type, intersect its destination rectangle with the prepare clip rectangle
    if (node.IsHardwareEnabledType()) {
        dstRect = dstRect.IntersectRect(prepareClipRect_);
    }
    // Set the destination rectangle of the node
    node.SetDstRect(dstRect);

    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        // record visible node position for display render node dirtyManager
        if (node.ShouldPaint()) {
            curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), node.GetOldDirty());
        }

        if (node.IsAppWindow()) {
            // if update appwindow, its children should not skip
            localZOrder_ = 0.0f;
            isQuickSkipPreparationEnabled_ = false;
            node.ResetChildHardwareEnabledNodes();
#ifndef USE_ROSEN_DRAWING
            boundsRect_ = SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight());
#else
            boundsRect_ = Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
#endif
            frameGravity_ = property.GetFrameGravity();
        }
    }

    // [planning] Remove this after skia is upgraded, the clipRegion is supported
    // reset childrenFilterRects
    node.ResetChildrenFilterRects();

    dirtyFlag_ = dirtyFlag_ || node.GetDstRectChanged();
    parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
#ifndef USE_ROSEN_DRAWING
    if (!(parentSurfaceNodeMatrix_.getSkewX() < std::numeric_limits<float>::epsilon() &&
        parentSurfaceNodeMatrix_.getSkewY() < std::numeric_limits<float>::epsilon())) {
#else
    if (!(parentSurfaceNodeMatrix_.Get(Drawing::Matrix::SKEW_X) < std::numeric_limits<float>::epsilon() &&
        parentSurfaceNodeMatrix_.Get(Drawing::Matrix::SKEW_Y) < std::numeric_limits<float>::epsilon())) {
#endif
        isSurfaceRotationChanged_ = true;
        doAnimate_ = doAnimate_ || isSurfaceRotationChanged_;
        node.SetAnimateState();
    }

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    bool isLeashWindowNode = false;
    if (drivenInfo_) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
        if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE) {
            drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty =
                drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty || dirtyFlag_;
        } else if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER) {
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
                drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || node.GetRenderProperties().NeedFilter();
        }

        if (node.GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
            isLeashWindowNode = true;
            drivenInfo_->isPrepareLeashWinSubTree = true;
        }
        if (node.IsSelfDrawingType()) {
            drivenInfo_->prepareInfo.hasInvalidScene = true;
        }
    }
#endif

    bool isSubNodeOfSurfaceInPrepare = isSubNodeOfSurfaceInPrepare_;
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        isSubNodeOfSurfaceInPrepare_ = true;
    }
    node.UpdateChildrenOutOfRectFlag(false);
    // [planning] ShouldPrepareSubnodes would be applied again if condition constraint ensures
    PrepareChildren(node);
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    rsParent = (logicParentNode_.lock());
    if (rsParent == curDisplayNode_) {
        std::unique_lock<std::mutex> lock(*surfaceNodePrepareMutex_);
        node.UpdateParentChildrenRect(logicParentNode_.lock());
    } else {
        node.UpdateParentChildrenRect(logicParentNode_.lock());
    }
#else
    node.UpdateParentChildrenRect(logicParentNode_.lock());
#endif
    // restore flags
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    dirtyFlag_ = dirtyFlag;
    isQuickSkipPreparationEnabled_ = isQuickSkipPreparationEnabled;
    prepareClipRect_ = prepareClipRect;
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        isSubNodeOfSurfaceInPrepare_ = isSubNodeOfSurfaceInPrepare;
    }

    PrepareTypesOfSurfaceRenderNodeAfterUpdate(node);
    if (node.GetDstRectChanged() || (node.GetDirtyManager() && node.GetDirtyManager()->IsCurrentFrameDirty())) {
        dirtySurfaceNodeMap_.emplace(node.GetId(), node.ReinterpretCastTo<RSSurfaceRenderNode>());
    }
    if (node.IsLeashWindow()) {
        auto matrix = geoPtr->GetAbsMatrix();
        // 1.0f means node does not have scale
#ifndef USE_ROSEN_DRAWING
        bool isScale = (matrix.getScaleX() > 0 && matrix.getScaleX() != 1.0f)
            || (matrix.getScaleY() > 0 && matrix.getScaleY() != 1.0f);
#else
        bool isScale = (matrix.Get(Drawing::Matrix::SCALE_X) > 0 && matrix.Get(Drawing::Matrix::SCALE_X) != 1.0f)
            || (matrix.Get(Drawing::Matrix::SCALE_Y) > 0 && matrix.Get(Drawing::Matrix::SCALE_Y) != 1.0f);
#endif
        if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
#ifndef USE_ROSEN_DRAWING
            isScale = isScale && matrix.getSkewX() < std::numeric_limits<float>::epsilon() &&
                matrix.getSkewX() < std::numeric_limits<float>::epsilon();
#else
            isScale = isScale && matrix.Get(Drawing::Matrix::SKEW_X) < std::numeric_limits<float>::epsilon() &&
                matrix.Get(Drawing::Matrix::SKEW_X) < std::numeric_limits<float>::epsilon();
#endif
        }
        node.SetIsScale(isScale);
    }
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_ && isLeashWindowNode) {
        drivenInfo_->isPrepareLeashWinSubTree = false;
    }
#endif
    // Due to the alpha is updated in PrepareChildren, so PrepareChildren
    // needs to be done before CheckOpaqueRegionBaseInfo
    auto screenRotation = curDisplayNode_->GetRotation();
    auto screenRect = RectI(0, 0, screenInfo_.width, screenInfo_.height);
    if (!node.CheckOpaqueRegionBaseInfo(
        screenRect, geoPtr->GetAbsRect(), screenRotation, node.IsFocusedNode(currentFocusedNodeId_))
        && node.GetSurfaceNodeType() != RSSurfaceNodeType::SELF_DRAWING_NODE) {
        node.ResetSurfaceOpaqueRegion(screenRect, geoPtr->GetAbsRect(),
            screenRotation, node.IsFocusedNode(currentFocusedNodeId_));
    }
    node.SetOpaqueRegionBaseInfo(
        screenRect, geoPtr->GetAbsRect(), screenRotation, node.IsFocusedNode(currentFocusedNodeId_));
}

void RSUniRenderVisitor::PrepareProxyRenderNode(RSProxyRenderNode& node)
{
    // alpha is not affected by dirty flag, always update
    node.SetContextAlpha(curAlpha_);
    // skip matrix & clipRegion update if not dirty
    if (!dirtyFlag_) {
        return;
    }
    auto rsParent = (logicParentNode_.lock());
    if (rsParent == nullptr) {
        return;
    }
    auto& property = rsParent->GetMutableRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());

    // Context matrix should be relative to the parent surface node, so we need to revert the parentSurfaceNodeMatrix_.
#ifndef USE_ROSEN_DRAWING
    SkMatrix invertMatrix;
    auto contextMatrix = geoPtr->GetAbsMatrix();
    if (parentSurfaceNodeMatrix_.invert(&invertMatrix)) {
        contextMatrix.preConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSUniRenderVisitor::PrepareProxyRenderNode, invert parentSurfaceNodeMatrix_ failed");
    }
#else
    Drawing::Matrix invertMatrix;
    Drawing::Matrix contextMatrix = geoPtr->GetAbsMatrix();

    if (parentSurfaceNodeMatrix_.Invert(invertMatrix)) {
        contextMatrix.PreConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSUniRenderVisitor::PrepareProxyRenderNode, invert parentSurfaceNodeMatrix_ failed");
    }
#endif
    node.SetContextMatrix(contextMatrix);

    // For now, we only set the clipRegion if the parent node has ClipToBounds set to true.
    if (!property.GetClipToBounds()) {
        node.SetContextClipRegion(std::nullopt);
    } else {
        // Maybe we should use prepareClipRect_ and make the clipRegion in device coordinate, but it will be more
        // complex to calculate the intersect, and it will make app developers confused.
        auto rect = property.GetBoundsRect();
        // Context clip region is in the parent node coordinate, so we don't need to map it.
#ifndef USE_ROSEN_DRAWING
        node.SetContextClipRegion(SkRect::MakeXYWH(rect.left_, rect.top_, rect.width_, rect.height_));
#else
        node.SetContextClipRegion(Drawing::Rect(
            rect.GetLeft(), rect.GetTop(), rect.GetWidth() + rect.GetLeft(), rect.GetHeight() + rect.GetTop()));
#endif
    }

    // prepare children
    PrepareChildren(node);
}

void RSUniRenderVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    bool dirtyFlag = dirtyFlag_;
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    RectI prepareClipRect = prepareClipRect_;

    auto rsParent = (node.GetParent().lock());
    const auto& property = node.GetRenderProperties();
    bool geoDirty = property.IsGeoDirty();
    auto geoPtr = (property.GetBoundsGeometry());

    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareRootRenderNode curSurfaceDirtyManager is nullptr");
        return;
    }
    dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, rsParent, dirtyFlag_);
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_) {
        drivenInfo_->currentRootNode = node.shared_from_this();
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
        if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE) {
            drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty = dirtyFlag_;
        } else if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER) {
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
                drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || node.GetRenderProperties().NeedFilter();
        }
    }
#endif
    if (rsParent == curSurfaceNode_) {
        const float rootWidth = property.GetFrameWidth() * property.GetScaleX();
        const float rootHeight = property.GetFrameHeight() * property.GetScaleY();
#ifndef USE_ROSEN_DRAWING
        SkMatrix gravityMatrix;
        (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
            RectF { 0.0f, 0.0f, boundsRect_.width(), boundsRect_.height() }, rootWidth, rootHeight, gravityMatrix);
#else
        Drawing::Matrix gravityMatrix;
        (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
            RectF { 0.0f, 0.0f, boundsRect_.GetWidth(), boundsRect_.GetHeight() },
            rootWidth, rootHeight, gravityMatrix);
#endif
        // Only Apply gravityMatrix when rootNode is dirty
        if (geoPtr != nullptr && (dirtyFlag || geoDirty)) {
            geoPtr->ConcatMatrix(gravityMatrix);
        }
    }

    if (geoPtr != nullptr) {
        parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    }
    node.UpdateChildrenOutOfRectFlag(false);
    PrepareChildren(node);
    node.UpdateParentChildrenRect(logicParentNode_.lock());

    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
}

void RSUniRenderVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode &node)
{
    preparedCanvasNodeInCurrentSurface_++;
    curContentDirty_ = node.IsContentDirty();
    bool dirtyFlag = dirtyFlag_;
    RectI prepareClipRect = prepareClipRect_;
    const auto& property = node.GetRenderProperties();

    auto nodeParent = node.GetParent().lock();
    while (nodeParent && nodeParent->ReinterpretCastTo<RSSurfaceRenderNode>() &&
        nodeParent->ReinterpretCastTo<RSSurfaceRenderNode>()->GetSurfaceNodeType() ==
        RSSurfaceNodeType::SELF_DRAWING_NODE) {
        nodeParent = nodeParent->GetParent().lock();
    }
    if (nodeParent) {
        node.SetIsAncestorDirty(nodeParent->IsDirty() || nodeParent->IsAncestorDirty());
        auto parentSurfaceNode = nodeParent->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (parentSurfaceNode && parentSurfaceNode->IsLeashWindow()) {
            node.SetParentLeashWindow();
        }
    }
    if (curSurfaceDirtyManager_ == nullptr || curDisplayDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareCanvasRenderNode curXDirtyManager is nullptr");
        return;
    }
    if (node.GetSharedTransitionParam().has_value()) {
        node.GetMutableRenderProperties().UpdateSandBoxMatrix(parentSurfaceNodeMatrix_);
    }
    if (isSubNodeOfSurfaceInPrepare_ && curSurfaceNode_ &&
        curSurfaceNode_->IsHardwareEnabledTopSurface() && node.ShouldPaint()) {
        curSurfaceNode_->SetSubNodeShouldPaint();
    }
    // if canvasNode is not sub node of surfaceNode, merge the dirtyRegion to curDisplayDirtyManager_
    auto dirtyManager = isSubNodeOfSurfaceInPrepare_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    if (curDirty_ || !dirtyCoverSubTree_ || property.NeedFilter()) {
        dirtyFlag_ = node.Update(*dirtyManager, nodeParent, dirtyFlag_, prepareClipRect_);
    }

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    // driven render
    bool isContentCanvasNode = false;
    bool isBeforeContentNodeDirty = false;
    if (drivenInfo_ && currentVisitDisplay_ == 0 && drivenInfo_->isPrepareLeashWinSubTree && node.IsMarkDriven()) {
        auto drivenCanvasNode = RSDrivenRenderManager::GetInstance().GetContentSurfaceNode()->GetDrivenCanvasNode();
        if (node.IsMarkDrivenRender() ||
            (!drivenInfo_->hasDrivenNodeMarkRender &&
            drivenCanvasNode != nullptr && node.GetId() == drivenCanvasNode->GetId())) {
            drivenInfo_->prepareInfo.backgroundNode = drivenInfo_->currentRootNode;
            drivenInfo_->prepareInfo.contentNode = node.shared_from_this();
            drivenInfo_->drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE;
            drivenInfo_->prepareInfo.dirtyInfo.contentDirty = false;
            drivenInfo_->surfaceDirtyManager = curSurfaceDirtyManager_;
            isContentCanvasNode = true;
            isBeforeContentNodeDirty = drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty;
            if (node.IsMarkDrivenRender()) {
                drivenInfo_->prepareInfo.dirtyInfo.type = DrivenDirtyType::MARK_DRIVEN_RENDER;
            } else {
                drivenInfo_->prepareInfo.dirtyInfo.type = DrivenDirtyType::MARK_DRIVEN;
            }
        }
    }
    if (drivenInfo_) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || dirtyFlag_;
        if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE) {
            drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty =
                drivenInfo_->prepareInfo.dirtyInfo.backgroundDirty || dirtyFlag_;
        } else if (drivenInfo_->drivenUniTreePrepareMode == DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER) {
            drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty =
                drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty || node.GetRenderProperties().NeedFilter();
        } else {
            if (node.IsContentChanged()) {
                drivenInfo_->prepareInfo.dirtyInfo.contentDirty = true;
            }
        }
        if (node.IsContentChanged()) {
            node.SetIsContentChanged(false);
        }
    }
#endif

    // Dirty Region use abstract coordinate, property of node use relative coordinate
    // BoundsRect(if exists) is mapped to absRect_ of RSObjAbsGeometry.
    if (curDirty_ || !dirtyCoverSubTree_ || property.NeedFilter()) {
        auto geoPtr = (property.GetBoundsGeometry());
        if (property.GetClipToBounds()) {
            prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->GetAbsRect());
        }
        // FrameRect(if exists) is mapped to rect using abstract coordinate explicitly by calling MapAbsRect.
        if (property.GetClipToFrame()) {
            // MapAbsRect do not handle the translation of OffsetX and OffsetY
#ifndef USE_ROSEN_DRAWING
            RectF frameRect{
                property.GetFrameOffsetX() * geoPtr->GetAbsMatrix().getScaleX(),
                property.GetFrameOffsetY() * geoPtr->GetAbsMatrix().getScaleY(),
                property.GetFrameWidth(), property.GetFrameHeight()};
#else
            RectF frameRect{
                property.GetFrameOffsetX() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_X),
                property.GetFrameOffsetY() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_Y),
                property.GetFrameWidth(), property.GetFrameHeight()};
#endif
            prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->MapAbsRect(frameRect));
        }
    }

    node.UpdateChildrenOutOfRectFlag(false);

    PrepareChildren(node);
    // attention: accumulate direct parent's childrenRect
    node.UpdateParentChildrenRect(logicParentNode_.lock());
    node.UpdateEffectRegion(effectRegion_);
    if (property.NeedFilter()) {
        // filterRects_ is used in RSUniRenderVisitor::CalcDirtyFilterRegion
        // When oldDirtyRect of node with filter has intersect with any surfaceNode or displayNode dirtyRegion,
        // the whole oldDirtyRect should be render in this vsync.
        // Partial rendering of node with filter would cause display problem.
        if (auto directParent = node.GetParent().lock()) {
            directParent->SetChildHasFilter(true);
        }
        if (curSurfaceDirtyManager_->IsTargetForDfx()) {
            curSurfaceDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
                DirtyRegionType::FILTER_RECT, node.GetOldDirtyInSurface());
        }
        if (curSurfaceNode_) {
            curSurfaceNode_->UpdateChildrenFilterRects(node.GetOldDirtyInSurface());
            curSurfaceNode_->UpdateFilterNodes(node.shared_from_this());
        }
        UpdateForegroundFilterCacheWithDirty(node, *dirtyManager);
    }
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    // skip content node and its children, calculate dirty contain background and foreground
    if (drivenInfo_ && isContentCanvasNode) {
        drivenInfo_->prepareInfo.dirtyInfo.nonContentDirty = isBeforeContentNodeDirty;
        drivenInfo_->drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER;
    }
#endif
}

void RSUniRenderVisitor::PrepareEffectRenderNode(RSEffectRenderNode& node)
{
    bool dirtyFlag = dirtyFlag_;
    RectI prepareClipRect = prepareClipRect_;
    auto effectRegion = effectRegion_;

#ifndef USE_ROSEN_DRAWING
    effectRegion_ = SkPath();
#else
    effectRegion_ = Drawing::Path();
#endif

    auto parentNode = node.GetParent().lock();
    dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, parentNode, dirtyFlag_, prepareClipRect_);

    node.UpdateChildrenOutOfRectFlag(false);
    PrepareChildren(node);
    node.UpdateParentChildrenRect(logicParentNode_.lock());
    node.SetEffectRegion(effectRegion_);

    if (node.GetRenderProperties().NeedFilter()) {
        // filterRects_ is used in RSUniRenderVisitor::CalcDirtyFilterRegion
        // When oldDirtyRect of node with filter has intersect with any surfaceNode or displayNode dirtyRegion,
        // the whole oldDirtyRect should be render in this vsync.
        // Partial rendering of node with filter would cause display problem.
        if (parentNode) {
            parentNode->SetChildHasFilter(true);
        }
        if (curSurfaceDirtyManager_->IsTargetForDfx()) {
            curSurfaceDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
                DirtyRegionType::FILTER_RECT, node.GetOldDirtyInSurface());
        }
        if (curSurfaceNode_) {
            curSurfaceNode_->UpdateChildrenFilterRects(node.GetOldDirtyInSurface());
            curSurfaceNode_->UpdateFilterNodes(node.shared_from_this());
        }
        UpdateForegroundFilterCacheWithDirty(node, *curSurfaceDirtyManager_);
    }

    effectRegion_ = effectRegion;
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
}

void RSUniRenderVisitor::CopyForParallelPrepare(std::shared_ptr<RSUniRenderVisitor> visitor)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    isPartialRenderEnabled_ = isPartialRenderEnabled_ && visitor->isPartialRenderEnabled_;
    isOpDropped_ = isOpDropped_ && visitor->isOpDropped_;
    needFilter_ = needFilter_ || visitor->needFilter_;
    for (const auto &u : visitor->displayHasSecSurface_) {
        displayHasSecSurface_[u.first] |= u.second;
    }
    for (const auto &u : visitor->displayHasSkipSurface_) {
        displayHasSkipSurface_[u.first] |= u.second;
    }
    for (const auto &u : visitor->dirtySurfaceNodeMap_) {
        dirtySurfaceNodeMap_[u.first] = u.second;
    }
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSUniRenderVisitor::DrawDirtyRectForDFX(const RectI& dirtyRect, const SkColor color,
    const SkPaint::Style fillType, float alpha, int edgeWidth = 6)
{
    if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
        ROSEN_LOGD("DrawDirtyRectForDFX dirty rect is invalid.");
        return;
    }
    ROSEN_LOGD("DrawDirtyRectForDFX current dirtyRect = %{public}s", dirtyRect.ToString().c_str());
    auto skRect = SkRect::MakeXYWH(dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
    std::string position = std::to_string(dirtyRect.left_) + ',' + std::to_string(dirtyRect.top_) + ',' +
        std::to_string(dirtyRect.width_) + ',' + std::to_string(dirtyRect.height_);
    const int defaultTextOffsetX = edgeWidth;
    const int defaultTextOffsetY = 30; // text position has 30 pixelSize under the skRect
    SkPaint rectPaint;
    // font size: 24
    sk_sp<SkTypeface> typeface = SkTypeface::MakeFromName("HarmonyOS Sans SC", SkFontStyle::Normal());
    sk_sp<SkTextBlob> SkTextBlob = SkTextBlob::MakeFromString(position.c_str(), SkFont(typeface, 24.0f, 1.0f, 0.0f));
    rectPaint.setColor(color);
    rectPaint.setAntiAlias(true);
    rectPaint.setAlphaf(alpha);
    rectPaint.setStyle(fillType);
    rectPaint.setStrokeWidth(edgeWidth);
    if (fillType == SkPaint::kFill_Style) {
        rectPaint.setStrokeJoin(SkPaint::kRound_Join);
    }
    canvas_->drawRect(skRect, rectPaint);
    canvas_->drawTextBlob(SkTextBlob, dirtyRect.left_ + defaultTextOffsetX,
        dirtyRect.top_ + defaultTextOffsetY, SkPaint());
}
#else
void RSUniRenderVisitor::DrawDirtyRectForDFX(const RectI& dirtyRect, const Drawing::Color color,
    const RSPaintStyle fillType, float alpha, int edgeWidth = 6)
{
    if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
        ROSEN_LOGD("DrawDirtyRectForDFX dirty rect is invalid.");
        return;
    }
    ROSEN_LOGD("DrawDirtyRectForDFX current dirtyRect = %{public}s", dirtyRect.ToString().c_str());
    auto rect = Drawing::Rect(dirtyRect.left_, dirtyRect.top_,
        dirtyRect.left_ + dirtyRect.width_, dirtyRect.top_ + dirtyRect.height_);
    Drawing::Pen rectPen;
    Drawing::Brush rectBrush;
    // font size: 24
    if (fillType == RSPaintStyle::STROKE) {
        rectPen.SetColor(color);
        rectPen.SetAntiAlias(true);
        rectPen.SetAlphaF(alpha);
        rectPen.SetWidth(edgeWidth);
        rectPen.SetJoinStyle(Drawing::Pen::JoinStyle::ROUND_JOIN);
        canvas_->AttachPen(rectPen);
    } else {
        rectBrush.SetColor(color);
        rectBrush.SetAntiAlias(true);
        rectBrush.SetAlphaF(alpha);
        canvas_->AttachBrush(rectBrush);
    }
    canvas_->DrawRect(rect);
    canvas_->DetachPen();
    canvas_->DetachBrush();
}
#endif

void RSUniRenderVisitor::DrawDirtyRegionForDFX(std::vector<RectI> dirtyRects)
{
    const float fillAlpha = 0.2;
    for (const auto& subRect : dirtyRects) {
#ifndef USE_ROSEN_DRAWING
        DrawDirtyRectForDFX(subRect, SK_ColorBLUE, SkPaint::kStroke_Style, fillAlpha);
#else
        DrawDirtyRectForDFX(subRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::STROKE, fillAlpha);
#endif
    }
}

void RSUniRenderVisitor::DrawCacheRegionForDFX(std::vector<RectI> cacheRects)
{
    const float fillAlpha = 0.2;
    for (const auto& subRect : cacheRects) {
#ifndef USE_ROSEN_DRAWING
        DrawDirtyRectForDFX(subRect, SK_ColorBLUE, SkPaint::kFill_Style, fillAlpha);
#else
        DrawDirtyRectForDFX(subRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::FILL, fillAlpha);
#endif
    }
}

void RSUniRenderVisitor::DrawAllSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node, const Occlusion::Region& region)
{
    const auto& visibleDirtyRects = region.GetRegionRects();
    std::vector<RectI> rects;
    for (auto& rect : visibleDirtyRects) {
        rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
    }
    DrawDirtyRegionForDFX(rects);

    // draw display dirtyregion with red color
    RectI dirtySurfaceRect = node.GetDirtyManager()->GetDirtyRegion();
    const float fillAlpha = 0.2;
#ifndef USE_ROSEN_DRAWING
    DrawDirtyRectForDFX(dirtySurfaceRect, SK_ColorRED, SkPaint::kStroke_Style, fillAlpha);
#else
    DrawDirtyRectForDFX(dirtySurfaceRect, Drawing::Color::COLOR_RED, RSPaintStyle::STROKE, fillAlpha);
#endif
}

void RSUniRenderVisitor::DrawAllSurfaceOpaqueRegionForDFX(RSDisplayRenderNode& node)
{
    for (auto& it : node.GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(it);
        if (surfaceNode->IsMainWindowType()) {
            DrawSurfaceOpaqueRegionForDFX(*surfaceNode);
        }
    }
}

void RSUniRenderVisitor::DrawTargetSurfaceDirtyRegionForDFX(RSDisplayRenderNode& node)
{
    for (auto it = node.GetCurAllSurfaces().rbegin(); it != node.GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        if (CheckIfSurfaceTargetedForDFX(surfaceNode->GetName())) {
            if (DrawDetailedTypesOfDirtyRegionForDFX(*surfaceNode)) {
                continue;
            }
            const auto& visibleDirtyRegions = surfaceNode->GetVisibleDirtyRegion().GetRegionRects();
            std::vector<RectI> rects;
            for (auto& rect : visibleDirtyRegions) {
                rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
            }
            const auto& visibleRegions = surfaceNode->GetVisibleRegion().GetRegionRects();
            auto displayDirtyRegion = node.GetDirtyManager()->GetDirtyRegion();
            for (auto& rect : visibleRegions) {
                auto visibleRect = RectI(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
                auto intersectRegion = displayDirtyRegion.IntersectRect(visibleRect);
                rects.emplace_back(intersectRegion);
            }
            DrawDirtyRegionForDFX(rects);
        }
    }
}

void RSUniRenderVisitor::DrawTargetSurfaceVisibleRegionForDFX(RSDisplayRenderNode& node)
{
    for (auto it = node.GetCurAllSurfaces().rbegin(); it != node.GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        if (CheckIfSurfaceTargetedForDFX(surfaceNode->GetName())) {
            const auto& visibleRegions = surfaceNode->GetVisibleRegion().GetRegionRects();
            std::vector<RectI> rects;
            for (auto& rect : visibleRegions) {
                rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
            }
            DrawDirtyRegionForDFX(rects);
        }
    }
}

void RSUniRenderVisitor::DrawAndTraceSingleDirtyRegionTypeForDFX(RSSurfaceRenderNode& node,
    DirtyRegionType dirtyType, bool isDrawn)
{
    auto dirtyManager = node.GetDirtyManager();
    auto matchType = DIRTY_REGION_TYPE_MAP.find(dirtyType);
    if (dirtyManager == nullptr ||  matchType == DIRTY_REGION_TYPE_MAP.end()) {
        return;
    }
    std::map<NodeId, RectI> dirtyInfo;
    float fillAlpha = 0.2;
    std::map<RSRenderNodeType, std::pair<std::string, SkColor>> nodeConfig = {
        {RSRenderNodeType::CANVAS_NODE, std::make_pair("canvas", SK_ColorRED)},
        {RSRenderNodeType::SURFACE_NODE, std::make_pair("surface", SK_ColorGREEN)},
    };

    std::string subInfo;
    for (const auto& [nodeType, info] : nodeConfig) {
        dirtyManager->GetDirtyRegionInfo(dirtyInfo, nodeType, dirtyType);
        subInfo += (" " + info.first + "node amount: " + std::to_string(dirtyInfo.size()));
        for (const auto& [nid, rect] : dirtyInfo) {
            if (isDrawn) {
#ifndef USE_ROSEN_DRAWING
                DrawDirtyRectForDFX(rect, info.second, SkPaint::kStroke_Style, fillAlpha);
#else
                DrawDirtyRectForDFX(rect, info.second, RSPaintStyle::STROKE, fillAlpha);
#endif
            }
        }
    }
    RS_TRACE_NAME("DrawAndTraceSingleDirtyRegionTypeForDFX target surface node " + node.GetName() + " - id[" +
        std::to_string(node.GetId()) + "] has dirtytype " + matchType->second + subInfo);
    ROSEN_LOGD("DrawAndTraceSingleDirtyRegionTypeForDFX target surface node %{public}s, id[%{public}" PRIu64 "]"
        "has dirtytype %{public}s%{public}s",
        node.GetName().c_str(), node.GetId(), matchType->second.c_str(), subInfo.c_str());
}

bool RSUniRenderVisitor::DrawDetailedTypesOfDirtyRegionForDFX(RSSurfaceRenderNode& node)
{
    if (dirtyRegionDebugType_ < DirtyRegionDebugType::CUR_DIRTY_DETAIL_ONLY_TRACE) {
        return false;
    }
    if (dirtyRegionDebugType_ == DirtyRegionDebugType::CUR_DIRTY_DETAIL_ONLY_TRACE) {
        auto i = DirtyRegionType::UPDATE_DIRTY_REGION;
        for (; i < DirtyRegionType::TYPE_AMOUNT; i = (DirtyRegionType)(i + 1)) {
            DrawAndTraceSingleDirtyRegionTypeForDFX(node, i, false);
        }
        return true;
    }
    const std::map<DirtyRegionDebugType, DirtyRegionType> DIRTY_REGION_DEBUG_TYPE_MAP {
        { DirtyRegionDebugType::UPDATE_DIRTY_REGION, DirtyRegionType::UPDATE_DIRTY_REGION },
        { DirtyRegionDebugType::OVERLAY_RECT, DirtyRegionType::OVERLAY_RECT },
        { DirtyRegionDebugType::FILTER_RECT, DirtyRegionType::FILTER_RECT },
        { DirtyRegionDebugType::SHADOW_RECT, DirtyRegionType::SHADOW_RECT },
        { DirtyRegionDebugType::PREPARE_CLIP_RECT, DirtyRegionType::PREPARE_CLIP_RECT },
        { DirtyRegionDebugType::REMOVE_CHILD_RECT, DirtyRegionType::REMOVE_CHILD_RECT },
        { DirtyRegionDebugType::RENDER_PROPERTIES_RECT, DirtyRegionType::RENDER_PROPERTIES_RECT },
        { DirtyRegionDebugType::CANVAS_NODE_SKIP_RECT, DirtyRegionType::CANVAS_NODE_SKIP_RECT },
    };
    auto matchType = DIRTY_REGION_DEBUG_TYPE_MAP.find(dirtyRegionDebugType_);
    if (matchType != DIRTY_REGION_DEBUG_TYPE_MAP.end()) {
        DrawAndTraceSingleDirtyRegionTypeForDFX(node, matchType->second);
    }
    return true;
}

void RSUniRenderVisitor::DrawSurfaceOpaqueRegionForDFX(RSSurfaceRenderNode& node)
{
    const auto& opaqueRegionRects = node.GetOpaqueRegion().GetRegionRects();
    for (const auto &subRect: opaqueRegionRects) {
#ifndef USE_ROSEN_DRAWING
        DrawDirtyRectForDFX(subRect.ToRectI(), SK_ColorGREEN, SkPaint::kFill_Style, 0.2f, 0);
#else
        DrawDirtyRectForDFX(subRect.ToRectI(), Drawing::Color::COLOR_GREEN,
            RSPaintStyle::FILL, 0.2f, 0);
#endif
    }
}

void RSUniRenderVisitor::ProcessChildren(RSRenderNode& node)
{
    if (DrawBlurInCache(node) || node.GetChildrenCount() == 0) {
        return;
    }
    if (isSubThread_) {
        node.SetIsUsedBySubThread(true);
        for (auto& child : node.GetSortedChildren(true)) {
            if (ProcessSharedTransitionNode(*child)) {
                dirtyCoverSubTree_ = node.GetDirtyCoverSubTree();
                child->Process(shared_from_this());
                child->SetDrawingCacheRootId(node.GetDrawingCacheRootId());
            }
        }
        // Main thread may invalidate the FullChildrenList, check if we need to clear it.
        node.ClearFullChildrenListIfNeeded(true);
        node.SetIsUsedBySubThread(false);
    } else {
        for (auto& child : node.GetSortedChildren()) {
            if (ProcessSharedTransitionNode(*child)) {
                dirtyCoverSubTree_ = node.GetDirtyCoverSubTree();
                child->Process(shared_from_this());
                child->SetDrawingCacheRootId(node.GetDrawingCacheRootId());
            }
        }
    }
}

void RSUniRenderVisitor::ProcessParallelDisplayRenderNode(RSDisplayRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_VK)
    RS_TRACE_NAME("ProcessParallelDisplayRenderNode[" + std::to_string(node.GetId()));
    RS_LOGD("RSUniRenderVisitor::ProcessParallelDisplayRenderNode node: %{public}" PRIu64 ", child size:%{public}u",
        node.GetId(), node.GetChildrenCount());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    isSecurityDisplay_ = node.GetSecurityDisplay();
    switch (screenInfo_.state) {
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
            break;
        case ScreenState::PRODUCER_SURFACE_ENABLE:
        default:
            RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode ScreenState only support HDI_OUTPUT_ENABLE");
            return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode: RSProcessor is null!");
        return;
    }

    // ParallelDisplayRenderNode cannot have mirror source.
    if (!processor_->Init(node, node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        INVALID_SCREEN_ID, renderEngine_)) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode: processor init failed!");
        return;
    }
    std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
    auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNodePtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode ReinterpretCastTo fail");
        return;
    }
    if (renderFrame_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode renderFrame_ nullptr");
        return;
    }
    std::shared_ptr<RSCanvasListener> overdrawListener = nullptr;
    AddOverDrawListener(renderFrame_, overdrawListener);

    if (canvas_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessParallelDisplayRenderNode: failed to create canvas");
        return;
    }
    canvas_->clear(SK_ColorTRANSPARENT);
    RSPropertiesPainter::SetBgAntiAlias(true);
    int saveCount = canvas_->save();
    canvas_->SetHighContrast(renderEngine_->IsHighContrastEnabled());
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr != nullptr) {
        canvas_->concat(geoPtr->GetMatrix());
    }
    for (auto& childNode : node.GetChildren()) {
        RSParallelRenderManager::Instance()->StartTiming(parallelRenderVisitorIndex_);
        childNode->Process(shared_from_this());
        RSParallelRenderManager::Instance()->StopTimingAndSetRenderTaskCost(
            parallelRenderVisitorIndex_, childNode->GetId(), TaskType::PROCESS_TASK);
    }
    canvas_->restoreToCount(saveCount);

    if (overdrawListener != nullptr) {
        overdrawListener->Draw();
    }
    DrawWatermarkIfNeed();
    RS_TRACE_BEGIN("ProcessParallelDisplayRenderNode:FlushFrame");
    renderFrame_->Flush();
    RS_TRACE_END();
    RS_LOGD("RSUniRenderVisitor::ProcessParallelDisplayRenderNode end");
#endif
}

void RSUniRenderVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_VK)
    if (node.IsParallelDisplayNode()) {
        ProcessParallelDisplayRenderNode(node);
        return;
    }
#endif
    RS_TRACE_NAME("ProcessDisplayRenderNode[" + std::to_string(node.GetScreenId()) + "]" +
        node.GetDirtyManager()->GetDirtyRegion().ToString().c_str());
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode node: %{public}" PRIu64 ", child size:%{public}u",
        node.GetId(), node.GetChildrenCount());
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
    bool isNeedCalcCost = node.GetSurfaceChangedRects().size() > 0;
#endif
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    isSecurityDisplay_ = node.GetSecurityDisplay();
    auto mirrorNode = node.GetMirrorSource().lock();
    switch (screenInfo_.state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(mirrorNode ?
                RSDisplayRenderNode::CompositeType::UNI_RENDER_MIRROR_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
            break;
        default:
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenState unsupported");
            return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();
    // in multidisplay scenario, curDisplayDirtyManager_ will be reset in another display's prepare stage
    curDisplayDirtyManager_ = node.GetDirtyManager();
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }

    if (renderEngine_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: renderEngine is null!");
        return;
    }
    if (!processor_->Init(node, node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID, renderEngine_)) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: processor init failed!");
        return;
    }
    std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
    auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNodePtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ReinterpretCastTo fail");
        return;
    }
    if (!node.IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(displayNodePtr);
        if (!node.CreateSurface(listener)) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode CreateSurface failed");
            return;
        }
    }

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    // [PLANNING]: processing of layers z-order to be implemented
    if (drivenInfo_ && !drivenInfo_->prepareInfo.hasInvalidScene) {
        drivenInfo_->currDrivenRenderMode = RSDrivenRenderManager::GetInstance().GetUniDrivenRenderMode();
        globalZOrder_ = RSDrivenRenderManager::GetInstance().GetUniRenderGlobalZOrder();
    }
#endif

    if (mirrorNode) {
        auto processor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor_);
        if (mirrorNode->GetSecurityDisplay() != isSecurityDisplay_ && processor) {
            canvas_ = processor->GetCanvas();
            if (canvas_ == nullptr) {
                RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode failed to get canvas.");
                return;
            }
            if (displayHasSecSurface_[mirrorNode->GetScreenId()]) {
#ifndef USE_ROSEN_DRAWING
                canvas_->clear(SK_ColorBLACK);
#else
                canvas_->Clear(Drawing::Color::COLOR_BLACK);
#endif
                processor_->PostProcess();
                return;
            }
#ifndef USE_ROSEN_DRAWING
            if (cacheImgForCapture_ && !displayHasSkipSurface_[mirrorNode->GetScreenId()]) {
                canvas_->save();
                // If both canvas and skImage have rotated, we need to reset the canvas
                if (resetRotate_) {
                    SkMatrix invertMatrix;
                    if (processor->GetScreenTransformMatrix().invert(&invertMatrix)) {
                        canvas_->concat(invertMatrix);
                    }
                }
                SkPaint paint;
                paint.setAntiAlias(true);
#ifdef NEW_SKIA
                canvas_->drawImage(cacheImgForCapture_, 0, 0, SkSamplingOptions(), &paint);
#else
                paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
                canvas_->drawImage(cacheImgForCapture_, 0, 0, &paint);
#endif
                canvas_->restore();
                DrawWatermarkIfNeed();
            } else {
                int saveCount = canvas_->save();
                ProcessChildren(*mirrorNode);
                DrawWatermarkIfNeed();
                canvas_->restoreToCount(saveCount);
            }
#else
            if (cacheImgForCapture_ && !displayHasSkipSurface_[mirrorNode->GetScreenId()]) {
                canvas_->Save();
                // If both canvas and skImage have rotated, we need to reset the canvas
                if (resetRotate_) {
                    Drawing::Matrix invertMatrix;
                    if (processor->GetScreenTransformMatrix().Invert(invertMatrix)) {
                        canvas_->ConcatMatrix(invertMatrix);
                    }
                }
                Drawing::Brush brush;
                brush.SetAntiAlias(true);
                canvas_->AttachBrush(brush);
                canvas_->DrawImage(*cacheImgForCapture_, 0, 0, Drawing::SamplingOptions());
                canvas_->DetachBrush();
                canvas_->Restore();
                DrawWatermarkIfNeed();
            } else {
                auto saveCount = canvas_->GetSaveCount();
                canvas_->Save();
                ProcessChildren(*mirrorNode);
                DrawWatermarkIfNeed();
                canvas_->RestoreToCount(saveCount);
            }
#endif
        } else {
            processor_->ProcessDisplaySurface(*mirrorNode);
        }
    } else if (node.GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        auto processor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor_);
        canvas_ = processor->GetCanvas();
        if (canvas_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode failed to get canvas.");
            return;
        }
        ProcessChildren(node);
        DrawWatermarkIfNeed();
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    } else if (drivenInfo_ && drivenInfo_->currDrivenRenderMode == DrivenUniRenderMode::REUSE_WITH_CLIP_HOLE) {
        RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode DrivenUniRenderMode is REUSE_WITH_CLIP_HOLE");
        node.SetGlobalZOrder(globalZOrder_);
        processor_->ProcessDisplaySurface(node);
#endif
    } else {
#ifdef RS_ENABLE_EGLQUERYSURFACE
        curDisplayDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
        if (isSurfaceRotationChanged_) {
            curDisplayDirtyManager_->MergeSurfaceRect();
            isOpDropped_ = false;
            isSurfaceRotationChanged_ = false;
        }
        if (isPartialRenderEnabled_) {
            CalcDirtyDisplayRegion(displayNodePtr);
            AddContainerDirtyToGlobalDirty(displayNodePtr);
            // Aligning displayRenderNode and surfaceRenderNode dirty region before merge dirty filter region
            if (isDirtyRegionAlignedEnable_) {
                AlignGlobalAndSurfaceDirtyRegions(displayNodePtr);
            }
            CalcDirtyFilterRegion(displayNodePtr);
            displayNodePtr->ClearCurrentSurfacePos();
        } else {
            // if isPartialRenderEnabled_ is disabled for some reason (i.e. screen rotation),
            // we should keep a fullscreen dirtyregion history to avoid dirtyregion losses.
            // isPartialRenderEnabled_ should not be disabled after current position.
            curDisplayDirtyManager_->MergeSurfaceRect();
            curDisplayDirtyManager_->UpdateDirty(isDirtyRegionAlignedEnable_);
        }
        if (isOpDropped_ && dirtySurfaceNodeMap_.empty()
            && !curDisplayDirtyManager_->IsCurrentFrameDirty() && !forceUpdateFlag_) {
            RS_LOGD("DisplayNode skip");
            RS_TRACE_NAME("DisplayNode skip");
            if (!IsHardwareComposerEnabled()) {
                return;
            }
            bool needCreateDisplayNodeLayer = false;
            for (auto& surfaceNode: hardwareEnabledNodes_) {
                if (!surfaceNode->IsHardwareForcedDisabled()) {
                    needCreateDisplayNodeLayer = true;
                    processor_->ProcessSurface(*surfaceNode);
                }
            }
            if (!RSMainThread::Instance()->WaitHardwareThreadTaskExcute()) {
                RS_LOGW("RSUniRenderVisitor::ProcessDisplayRenderNode: hardwareThread task has too many to excute");
            }
            if (needCreateDisplayNodeLayer) {
                processor_->ProcessDisplaySurface(node);
                processor_->PostProcess();
            }
            return;
        }
#endif

#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_VK)
        if (isParallel_ &&!isPartialRenderEnabled_) {
            auto parallelRenderManager = RSParallelRenderManager::Instance();
            vulkan::VulkanWindow::InitializeVulkan(
                parallelRenderManager->GetParallelThreadNumber());
            RS_OPTIONAL_TRACE_BEGIN("RSUniRender::VK::WaitFence");
            vulkan::VulkanWindow::WaitForSharedFence();
            vulkan::VulkanWindow::ResetSharedFence();
            RS_OPTIONAL_TRACE_END();
            parallelRenderManager->CopyVisitorAndPackTask(*this, node);
            parallelRenderManager->InitDisplayNodeAndRequestFrame(renderEngine_, screenInfo_);
            parallelRenderManager->LoadBalanceAndNotify(TaskType::PROCESS_TASK);
            parallelRenderManager->WaitProcessEnd();
            parallelRenderManager->CommitSurfaceNum(node.GetChildrenCount());
            vulkan::VulkanWindow::PresentAll();

            RS_OPTIONAL_TRACE_BEGIN("RSUniRender:WaitUtilUniRenderFinished");
            RSMainThread::Instance()->WaitUtilUniRenderFinished();
            RS_OPTIONAL_TRACE_END();

            parallelRenderManager->ProcessParallelDisplaySurface(*this);
            processor_->PostProcess();

            parallelRenderManager->ReleaseBuffer();

            isParallel_ = false;
            return;
        }
#endif

        auto rsSurface = node.GetRSSurface();
        if (rsSurface == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode No RSSurface found");
            return;
        }
        rsSurface->SetColorSpace(newColorSpace_);
        // we should request a framebuffer whose size is equals to the physical screen size.
        RS_TRACE_BEGIN("RSUniRender:RequestFrame");
        BufferRequestConfig bufferConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo_, true);
        if (hasFingerprint_) {
            bufferConfig.format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_1010102;
            RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode, RGBA 8888 to RGBA 1010102");
        }
        node.SetFingerprint(hasFingerprint_);
        RS_OPTIONAL_TRACE_BEGIN("RSUniRender::wait for bufferRequest cond");
        if (!RSMainThread::Instance()->WaitUntilDisplayNodeBufferReleased(node)) {
            RS_TRACE_NAME("RSUniRenderVisitor no released buffer");
        }
        RS_OPTIONAL_TRACE_END();
#ifdef NEW_RENDER_CONTEXT
        renderFrame_ = renderEngine_->RequestFrame(std::static_pointer_cast<RSRenderSurfaceOhos>(rsSurface),
            bufferConfig);
#else
        renderFrame_ = renderEngine_->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface), bufferConfig);
#endif
        RS_TRACE_END();

        if (renderFrame_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor Request Frame Failed");
            return;
        }
        std::shared_ptr<RSCanvasListener> overdrawListener = nullptr;
        AddOverDrawListener(renderFrame_, overdrawListener);

        if (canvas_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: failed to create canvas");
            return;
        }

        tryCapture(node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight());

#ifdef RS_ENABLE_VK
        canvas_->clear(SK_ColorTRANSPARENT);
#endif

        int saveLayerCnt = 0;
#ifndef USE_ROSEN_DRAWING
        SkRegion region;
#else
        Drawing::Region region;
#endif
        Occlusion::Region dirtyRegionTest;
        std::vector<RectI> rects;
        bool clipPath = false;
#ifdef RS_ENABLE_EGLQUERYSURFACE
        // Get displayNode buffer age in order to merge visible dirty region for displayNode.
        // And then set egl damage region to improve uni_render efficiency.
        if (isPartialRenderEnabled_) {
            // Early history buffer Merging will have impact on Overdraw display, so we need to
            // set the full screen dirty to avoid this impact.
            if (RSOverdrawController::GetInstance().IsEnabled()) {
                node.GetDirtyManager()->ResetDirtyAsSurfaceSize();
            }
            RS_OPTIONAL_TRACE_BEGIN("RSUniRender::GetBufferAge");
            int bufferAge = renderFrame_->GetBufferAge();
            RS_OPTIONAL_TRACE_END();
            RSUniRenderUtil::MergeDirtyHistory(displayNodePtr, bufferAge, isDirtyRegionAlignedEnable_);
            Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(
                displayNodePtr, isDirtyRegionAlignedEnable_);
            dirtyRegionTest = dirtyRegion;
            if (isDirtyRegionAlignedEnable_) {
                SetSurfaceGlobalAlignedDirtyRegion(displayNodePtr, dirtyRegion);
            } else {
                SetSurfaceGlobalDirtyRegion(displayNodePtr);
            }
            rects = GetDirtyRects(dirtyRegion);
            RectI rect = node.GetDirtyManager()->GetDirtyRegionFlipWithinSurface();
            if (!rect.IsEmpty()) {
                rects.emplace_back(rect);
            }
            if (!isDirtyRegionAlignedEnable_) {
                auto disH = screenInfo_.GetRotatedHeight();
                for (auto& r : rects) {
#ifndef USE_ROSEN_DRAWING
                    region.op(SkIRect::MakeXYWH(r.left_, disH - r.GetBottom(), r.width_, r.height_),
                        SkRegion::kUnion_Op);
#else
                    Drawing::Region tmpRegion;
                    tmpRegion.SetRect(Drawing::RectI(r.left_, disH - r.GetBottom(),
                        r.left_ + r.width_, disH - r.GetBottom() + r.height_));
                    region.Op(tmpRegion, Drawing::RegionOp::UNION);
#endif
                }
            }
            // SetDamageRegion and opDrop will be disabled for dirty region DFX visualization
            if (!isRegionDebugEnabled_) {
                renderFrame_->SetDamageRegion(rects);
            }
        }
        if (isOpDropped_ && !isDirtyRegionAlignedEnable_) {
#ifndef USE_ROSEN_DRAWING
            if (region.isEmpty()) {
                // [planning] Remove this after frame buffer can cancel
                canvas_->clipRect(SkRect::MakeEmpty());
            } else if (region.isRect()) {
                canvas_->clipRegion(region);
            } else {
                RS_TRACE_NAME("RSUniRenderVisitor: clipPath");
                clipPath = true;
                SkPath dirtyPath;
                region.getBoundaryPath(&dirtyPath);
                canvas_->clipPath(dirtyPath, true);
            }
#else
            RS_LOGD("[%{public}s:%{public}d] Drawing is not supported, canvas clipRegion", __func__, __LINE__);
#endif
        }
#endif
        RSPropertiesPainter::SetBgAntiAlias(true);
        if (!isParallel_ || isUIFirst_) {
#ifndef USE_ROSEN_DRAWING
            int saveCount = canvas_->save();
            canvas_->SetHighContrast(renderEngine_->IsHighContrastEnabled());
            auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
            if (geoPtr != nullptr) {
                // enable cache if screen rotation
                canvas_->SetCacheType(RSSystemProperties::GetCacheEnabledForRotation() ?
                    RSPaintFilterCanvas::CacheType::ENABLED : RSPaintFilterCanvas::CacheType::DISABLED);
            }

            bool needOffscreen = clipPath;
            if (needOffscreen) {
                ClearTransparentBeforeSaveLayer(); // clear transparent before concat display node's matrix
            }
            if (geoPtr != nullptr) {
                canvas_->concat(geoPtr->GetMatrix());
            }
            if (needOffscreen) {
                // we are doing rotation animation, try offscreen render if capable
                displayNodeMatrix_ = canvas_->getTotalMatrix();
                PrepareOffscreenRender(node);
                ProcessChildren(node);
                FinishOffscreenRender();
            } else {
                // render directly
                ProcessChildren(node);
            }
            canvas_->restoreToCount(saveCount);
#else
            int saveCount = canvas_->GetSaveCount();
            canvas_->Save();
            canvas_->SetHighContrast(renderEngine_->IsHighContrastEnabled());
            auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
            if (geoPtr != nullptr) {
                // enable cache if screen rotation
                canvas_->SetCacheType(RSSystemProperties::GetCacheEnabledForRotation() ?
                    RSPaintFilterCanvas::CacheType::ENABLED : RSPaintFilterCanvas::CacheType::DISABLED);
            }

            bool needOffscreen = clipPath;
            if (needOffscreen) {
                ClearTransparentBeforeSaveLayer(); // clear transparent before concat display node's matrix
            }
            if (geoPtr != nullptr) {
                canvas_->ConcatMatrix(geoPtr->GetMatrix());
            }
            if (needOffscreen) {
                // we are doing rotation animation, try offscreen render if capable
                displayNodeMatrix_ = canvas_->GetTotalMatrix();
                PrepareOffscreenRender(node);
                ProcessChildren(node);
                FinishOffscreenRender();
            } else {
                // render directly
                ProcessChildren(node);
            }

            canvas_->RestoreToCount(saveCount);
#endif
        }
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
        if ((isParallel_ && !isUIFirst_ && ((rects.size() > 0) || !isPartialRenderEnabled_)) && isCalcCostEnable_) {
            auto parallelRenderManager = RSParallelRenderManager::Instance();
            parallelRenderManager->CopyCalcCostVisitorAndPackTask(*this, node, isNeedCalcCost,
                doAnimate_, isOpDropped_);
            if (parallelRenderManager->IsNeedCalcCost()) {
                parallelRenderManager->LoadBalanceAndNotify(TaskType::CALC_COST_TASK);
                parallelRenderManager->WaitCalcCostEnd();
                parallelRenderManager->UpdateNodeCost(node);
            }
        }
        if (isParallel_ && !isUIFirst_ && ((rects.size() > 0) || !isPartialRenderEnabled_)) {
            ClearTransparentBeforeSaveLayer();
            auto parallelRenderManager = RSParallelRenderManager::Instance();
            parallelRenderManager->SetFrameSize(screenInfo_.width, screenInfo_.height);
            parallelRenderManager->CopyVisitorAndPackTask(*this, node);
            parallelRenderManager->LoadBalanceAndNotify(TaskType::PROCESS_TASK);
            parallelRenderManager->MergeRenderResult(*canvas_);
            parallelRenderManager->CommitSurfaceNum(node.GetChildrenCount());
            parallelRenderManager->ProcessFilterSurfaceRenderNode();
        }
#endif
        if (saveLayerCnt > 0) {
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
#ifdef NEW_RENDER_CONTEXT
            RSTagTracker tagTracker(
                renderEngine_->GetDrawingContext()->GetDrawingContext(),
                RSTagTracker::TAGTYPE::TAG_RESTORELAYER_DRAW_NODE);
#else
            RSTagTracker tagTracker(
                renderEngine_->GetRenderContext()->GetGrContext(), RSTagTracker::TAGTYPE::TAG_RESTORELAYER_DRAW_NODE);
#endif
#endif
            RS_TRACE_NAME("RSUniRender:RestoreLayer");
            canvas_->restoreToCount(saveLayerCnt);
#endif
        }
        if (UNLIKELY(!unpairedTransitionNodes_.empty())) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode  unpairedTransitionNodes_ is not empty.");
            ProcessUnpairedSharedTransitionNode();
        }
        if (overdrawListener != nullptr) {
            overdrawListener->Draw();
        }
        DrawWatermarkIfNeed();
        // the following code makes DirtyRegion visible, enable this method by turning on the dirtyregiondebug property
        if (isPartialRenderEnabled_) {
            if (isDirtyRegionDfxEnabled_) {
                DrawAllSurfaceDirtyRegionForDFX(node, dirtyRegionTest);
            }
            if (isTargetDirtyRegionDfxEnabled_) {
                DrawTargetSurfaceDirtyRegionForDFX(node);
            }
            if (isDisplayDirtyDfxEnabled_) {
                DrawDirtyRegionForDFX(node.GetDirtyManager()->GetMergedDirtyRegions());
            }
        }

        if (isOpaqueRegionDfxEnabled_) {
            DrawAllSurfaceOpaqueRegionForDFX(node);
        }
        if (isVisibleRegionDfxEnabled_) {
            DrawTargetSurfaceVisibleRegionForDFX(node);
        }

        if (isDrawingCacheEnabled_ && RSSystemParameters::GetDrawingCacheEnabledDfx()) {
            DrawCacheRegionForDFX(cacheRenderNodeMapRects_);
        }
        endCapture();
        RSMainThread::Instance()->RemoveTask(CLEAR_GPU_CACHE);
        RS_TRACE_BEGIN("RSUniRender:FlushFrame");
        renderFrame_->Flush();
        RS_TRACE_END();
        RS_OPTIONAL_TRACE_BEGIN("RSUniRender:WaitUtilUniRenderFinished");
        RSMainThread::Instance()->WaitUtilUniRenderFinished();
        RS_OPTIONAL_TRACE_END();
        UpdateHardwareEnabledInfoBeforeCreateLayer();
        AssignGlobalZOrderAndCreateLayer(appWindowNodesInZOrder_);
        node.SetGlobalZOrder(globalZOrder_++);
        processor_->ProcessDisplaySurface(node);
        AssignGlobalZOrderAndCreateLayer(hardwareEnabledTopNodes_);
    }

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (drivenInfo_ && !drivenInfo_->prepareInfo.hasInvalidScene) {
        RS_TRACE_NAME("RSUniRender:DrivenRenderProcess");
        // process driven render tree
        drivenInfo_->processInfo = { processor_, newColorSpace_, node.GetGlobalZOrder() };
        RSDrivenRenderManager::GetInstance().DoProcessRenderTask(drivenInfo_->processInfo);
    }
#endif
    processor_->PostProcess();
    {
        std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
        EraseIf(groupedTransitionNodes, [](auto& iter) -> bool {
            auto& [id, pair] = iter;
            if (pair.second.empty()) {
                return true;
            }
            const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
            auto node = nodeMap.GetRenderNode<RSRenderNode>(iter.first);
            return node ? (!node->IsOnTheTree()) : true;
        });
    }
    RSMainThread::Instance()->ClearGpuCache();
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode end");
}

void RSUniRenderVisitor::DrawSurfaceLayer(const std::shared_ptr<RSDisplayRenderNode>& displayNode,
    const std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes) const
{
#if defined(RS_ENABLE_GL)
    auto subThreadManager = RSSubThreadManager::Instance();
    subThreadManager->StartFilterThread(renderEngine_->GetRenderContext().get());
    subThreadManager->SubmitSubThreadTask(displayNode, subThreadNodes);
#endif
}

void RSUniRenderVisitor::UpdateHardwareEnabledInfoBeforeCreateLayer()
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    if (hardwareEnabledNodes_.empty()) {
        return;
    }
    if (isParallel_ && !isUIFirst_) {
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>().swap(appWindowNodesInZOrder_);
        auto subThreadNum = RSParallelRenderManager::Instance()->GetParallelThreadNumber();
        auto appWindowNodesMap = RSParallelRenderManager::Instance()->GetAppWindowNodes();
        std::vector<std::shared_ptr<RSSurfaceRenderNode>> appWindowNodes;
        for (uint32_t i = 0; i < subThreadNum; i++) {
            appWindowNodes = appWindowNodesMap[i];
            appWindowNodesInZOrder_.insert(appWindowNodesInZOrder_.end(), appWindowNodes.begin(), appWindowNodes.end());
        }
#endif
    }
    globalZOrder_ = 0.0f;
}

void RSUniRenderVisitor::AssignGlobalZOrderAndCreateLayer(
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& nodesInZOrder)
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    if (hardwareEnabledNodes_.empty()) {
        return;
    }
    for (auto& appWindowNode : nodesInZOrder) {
        // first, sort app window node's child surfaceView by local zOrder
        auto childHardwareEnabledNodes = appWindowNode->GetChildHardwareEnabledNodes();
        for (auto iter = childHardwareEnabledNodes.begin(); iter != childHardwareEnabledNodes.end();) {
            auto childNode = iter->lock();
            if (!childNode || !childNode->IsOnTheTree()) {
                iter = childHardwareEnabledNodes.erase(iter);
                continue;
            }
            if (childNode->GetBuffer() != nullptr && !childNode->IsHardwareForcedDisabled()) {
                // SetGlobalZOrder here to ensure zOrder committed to composer is continuous
                childNode->SetGlobalZOrder(globalZOrder_++);
                RS_LOGD("createLayer: %{public}" PRIu64 "", childNode->GetId());
                processor_->ProcessSurface(*childNode);
            }
            ++iter;
        }
    }
}

void RSUniRenderVisitor::AddOverDrawListener(std::unique_ptr<RSRenderFrame>& renderFrame,
    std::shared_ptr<RSCanvasListener>& overdrawListener)
{
#if defined(NEW_RENDER_CONTEXT)
    if (renderFrame == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: renderFrame is nullptr");
        return;
    }
    auto renderSurface = renderFrame->GetSurface();
    if (renderSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: renderSurface is nullptr");
        return;
    }
#if !defined(USE_ROSEN_DRAWING)
    RS_OPTIONAL_TRACE_BEGIN("RSUniRender::GetSurface");
    auto skSurface = renderSurface->GetSurface();
    RS_OPTIONAL_TRACE_END();
    if (skSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: skSurface is null");
        return;
    }
    if (skSurface->getCanvas() == nullptr) {
        ROSEN_LOGE("skSurface.getCanvas is null.");
        return;
    }
#else
    RS_OPTIONAL_TRACE_BEGIN("RSUniRender::GetSurface");
    auto drSurface = renderSurface->GetSurface();
    RS_OPTIONAL_TRACE_END();
    if (drSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: drSurface is null");
        return;
    }
    if (drSurface->GetCanvas() == nullptr) {
        ROSEN_LOGE("drSurface.getCanvas is null.");
        return;
    }
#endif
#else
    if (renderFrame->GetFrame() == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: RSSurfaceFrame is nullptr");
        return;
    }
#if !defined(USE_ROSEN_DRAWING)
    RS_TRACE_BEGIN("RSUniRender::GetSurface");
    auto skSurface = renderFrame->GetFrame()->GetSurface();
    RS_TRACE_END();
    if (skSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: skSurface is null");
        return;
    }
    if (skSurface->getCanvas() == nullptr) {
        ROSEN_LOGE("skSurface.getCanvas is null.");
        return;
    }
#else
    RS_OPTIONAL_TRACE_BEGIN("RSUniRender::GetSurface");
    auto drSurface = renderFrame->GetFrame()->GetSurface();
    RS_OPTIONAL_TRACE_END();
    if (drSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::AddOverDrawListener: drSurface is null");
        return;
    }
    if (drSurface->GetCanvas() == nullptr) {
        ROSEN_LOGE("drSurface.getCanvas is null.");
        return;
    }
#endif
#endif
    // if listenedCanvas is nullptr, that means disabled or listen failed
    std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;

    if (RSOverdrawController::GetInstance().IsEnabled()) {
        auto &oc = RSOverdrawController::GetInstance();
#ifndef USE_ROSEN_DRAWING
        listenedCanvas = std::make_shared<RSListenedCanvas>(skSurface.get());
#else
        listenedCanvas = std::make_shared<RSListenedCanvas>(*drSurface.get());
#endif
        overdrawListener = oc.CreateListener<RSGPUOverdrawCanvasListener>(listenedCanvas.get());
        if (overdrawListener == nullptr) {
            overdrawListener = oc.CreateListener<RSCPUOverdrawCanvasListener>(listenedCanvas.get());
        }

        if (overdrawListener != nullptr) {
            listenedCanvas->SetListener(overdrawListener);
        } else {
            // create listener failed
            listenedCanvas = nullptr;
        }
    }

    if (listenedCanvas != nullptr) {
        canvas_ = listenedCanvas;
    } else {
#ifndef USE_ROSEN_DRAWING
        canvas_ = std::make_shared<RSPaintFilterCanvas>(skSurface.get());
#else
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
#endif
    }
}

void RSUniRenderVisitor::CalcDirtyDisplayRegion(std::shared_ptr<RSDisplayRenderNode>& node)
{
    RS_OPTIONAL_TRACE_FUNC();
    auto displayDirtyManager = node->GetDirtyManager();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !IsHardwareEnabledNodeNeedCalcGlobalDirty(surfaceNode)) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        if (isUIFirst_ && surfaceDirtyManager->IsCurrentFrameDirty()) {
            dirtySurfaceNodeMap_.emplace(surfaceNode->GetId(), surfaceNode->ReinterpretCastTo<RSSurfaceRenderNode>());
        }
        RectI surfaceDirtyRect = surfaceDirtyManager->GetCurrentFrameDirtyRegion();
        if (surfaceNode->IsTransparent()) {
            // Handles the case of transparent surface, merge transparent dirty rect
            RectI transparentDirtyRect = surfaceNode->GetDstRect().IntersectRect(surfaceDirtyRect);
            if (!transparentDirtyRect.IsEmpty()) {
                RS_OPTIONAL_TRACE_NAME_FMT("CalcDirtyDisplayRegion merge transparent dirty rect %s rect %s",
                    surfaceNode->GetName().c_str(), transparentDirtyRect.ToString().c_str());
                displayDirtyManager->MergeDirtyRect(transparentDirtyRect);
            }
        }

        if (surfaceNode->GetZorderChanged()) {
            // Zorder changed case, merge surface dest Rect
            RS_LOGD("CalcDirtyDisplayRegion merge GetZorderChanged %{public}s rect %{public}s",
                surfaceNode->GetName().c_str(), surfaceNode->GetDstRect().ToString().c_str());
            displayDirtyManager->MergeDirtyRect(surfaceNode->GetDstRect());
        }

        RectI lastFrameSurfacePos = node->GetLastFrameSurfacePos(surfaceNode->GetId());
        RectI currentFrameSurfacePos = node->GetCurrentFrameSurfacePos(surfaceNode->GetId());
        if (lastFrameSurfacePos != currentFrameSurfacePos) {
            RS_LOGD("CalcDirtyDisplayRegion merge surface pos changed %{public}s lastFrameRect %{public}s"
                " currentFrameRect %{public}s", surfaceNode->GetName().c_str(), lastFrameSurfacePos.ToString().c_str(),
                currentFrameSurfacePos.ToString().c_str());
            if (!lastFrameSurfacePos.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(lastFrameSurfacePos);
            }
            if (!currentFrameSurfacePos.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(currentFrameSurfacePos);
            }
        }

        bool isShadowDisappear = !surfaceNode->GetRenderProperties().IsShadowValid() && surfaceNode->IsShadowValidLastFrame();
        if (surfaceNode->GetRenderProperties().IsShadowValid() || isShadowDisappear) {
            RectI shadowDirtyRect = surfaceNode->GetOldDirtyInSurface().IntersectRect(surfaceDirtyRect);
            // There are two situation here:
            // 1. SurfaceNode first has shadow or shadow radius is larger than the last frame,
            // surfaceDirtyRect == surfaceNode->GetOldDirtyInSurface()
            // 2. SurfaceNode remove shadow or shadow radius is smaller than the last frame,
            // surfaceDirtyRect > surfaceNode->GetOldDirtyInSurface()
            // So we should always merge surfaceDirtyRect here.
            if (!shadowDirtyRect.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(surfaceDirtyRect);
                RS_LOGD("CalcDirtyDisplayRegion merge ShadowValid %{public}s rect %{public}s",
                    surfaceNode->GetName().c_str(), surfaceDirtyRect.ToString().c_str());
            }
            if (isShadowDisappear) {
                surfaceNode->SetShadowValidLastFrame(false);
            }
        }
    }
    std::vector<RectI> surfaceChangedRects = node->GetSurfaceChangedRects();
    for (auto& surfaceChangedRect : surfaceChangedRects) {
        RS_LOGD("CalcDirtyDisplayRegion merge Surface closed %{public}s", surfaceChangedRect.ToString().c_str());
        if (!surfaceChangedRect.IsEmpty()) {
            displayDirtyManager->MergeDirtyRect(surfaceChangedRect);
        }
    }
}

void RSUniRenderVisitor::MergeDirtyRectIfNeed(std::shared_ptr<RSSurfaceRenderNode> appNode,
    std::shared_ptr<RSSurfaceRenderNode> hwcNode)
{
    if ((hwcNode->IsLastFrameHardwareEnabled() || hwcNode->IsCurrentFrameBufferConsumed()) &&
        appNode && appNode->GetDirtyManager()) {
        appNode->GetDirtyManager()->MergeDirtyRect(hwcNode->GetDstRect());
        dirtySurfaceNodeMap_.emplace(appNode->GetId(), appNode);
    }
}

RectI RSUniRenderVisitor::UpdateHardwareEnableList(std::vector<RectI>& filterRects,
    std::vector<SurfaceDirtyMgrPair>& validHwcNodes)
{
    if (validHwcNodes.empty() || filterRects.empty()) {
        return RectI();
    }
    // remove invisible surface since occlusion
    // check intersected parts
    RectI filterDirty;
    for (auto iter = validHwcNodes.begin(); iter != validHwcNodes.end(); ++iter) {
        auto childNode = iter->first;
        auto childDirtyRect = childNode->GetDstRect();
        bool isIntersected = false;
        // remove invisible surface since occlusion
        for (auto& filterRect : filterRects) {
            if (!childDirtyRect.IntersectRect(filterRect).IsEmpty()) {
                filterDirty = filterDirty.JoinRect(filterRect);
                isIntersected = true;
            }
        }
        if (isIntersected) {
            childNode->SetHardwareForcedDisabledStateByFilter(true);
            auto node = iter->second;
            MergeDirtyRectIfNeed(iter->second, childNode);
            iter = validHwcNodes.erase(iter);
            iter--;
        }
    }
    return filterDirty;
}

void RSUniRenderVisitor::UpdateHardwareNodeStatusBasedOnFilter(std::shared_ptr<RSSurfaceRenderNode>& node,
    std::vector<SurfaceDirtyMgrPair>& prevHwcEnabledNodes,
    std::shared_ptr<RSDirtyRegionManager>& displayDirtyManager)
{
    if (!IsHardwareComposerEnabled()) {
        return;
    }
    if (node == nullptr || !node->IsAppWindow() || node->GetDirtyManager() == nullptr ||
        displayDirtyManager == nullptr) {
        return;
    }
    auto dirtyManager = node->GetDirtyManager();
    auto filterRects = node->GetChildrenNeedFilterRects();
    // collect valid hwc surface which is not intersected with filterRects
    std::vector<SurfaceDirtyMgrPair> curHwcEnabledNodes;
    // remove invisible surface since occlusion
    auto visibleRegion = node->GetVisibleRegion();
    for (auto subNode : node->GetChildHardwareEnabledNodes()) {
        if (auto childNode = subNode.lock()) {
            // recover disabled state before update
            childNode->SetHardwareForcedDisabledStateByFilter(false);
            if (visibleRegion.IsIntersectWith(Occlusion::Rect(childNode->GetOldDirtyInSurface()))) {
                curHwcEnabledNodes.emplace_back(std::make_pair(subNode, node));
            }
        }
    }
    // Within App: disable hwc if intersect with filterRects
    dirtyManager->MergeDirtyRect(UpdateHardwareEnableList(filterRects, curHwcEnabledNodes));
    // Among App: disable lower hwc layers if intersect with upper transparent appWindow
    if (node->IsTransparent()) {
        if (node->GetRenderProperties().NeedFilter()) {
            // Attention: if transparent appwindow needs filter, only need to check itself
            filterRects = {node->GetDstRect()};
        }
        // In case of transparent window, filterRects need hwc surface's content
        RectI globalTransDirty = UpdateHardwareEnableList(filterRects, prevHwcEnabledNodes);
        displayDirtyManager->MergeDirtyRect(globalTransDirty);
        dirtyManager->MergeDirtyRect(globalTransDirty);
    }
    // erase from curHwcEnabledNodes if app node has no container window and its hwc node intersects with hwc below
    if (!node->HasContainerWindow() && !curHwcEnabledNodes.empty() && !prevHwcEnabledNodes.empty()) {
        for (auto iter = curHwcEnabledNodes.begin(); iter != curHwcEnabledNodes.end(); ++iter) {
            for (auto& prevNode : prevHwcEnabledNodes) {
                if (!iter->first->GetDstRect().IntersectRect(prevNode.first->GetDstRect()).IsEmpty()) {
                    iter->first->SetHardwareForcedDisabledStateByFilter(true);
                    MergeDirtyRectIfNeed(iter->second, iter->first);
                    iter = curHwcEnabledNodes.erase(iter);
                    iter--;
                    break;
                }
            }
        }
    }
    if (!curHwcEnabledNodes.empty()) {
        prevHwcEnabledNodes.insert(prevHwcEnabledNodes.end(), curHwcEnabledNodes.begin(), curHwcEnabledNodes.end());
    }
}

void RSUniRenderVisitor::CalcDirtyRegionForFilterNode(const RectI& filterRect,
    std::shared_ptr<RSSurfaceRenderNode>& currentSurfaceNode,
    std::shared_ptr<RSDisplayRenderNode>& displayNode)
{
    auto displayDirtyManager = displayNode->GetDirtyManager();
    auto currentSurfaceDirtyManager = currentSurfaceNode->GetDirtyManager();
    if (displayDirtyManager == nullptr || currentSurfaceDirtyManager == nullptr) {
        return;
    }

    RectI displayDirtyRect = displayDirtyManager->GetCurrentFrameDirtyRegion();
    RectI currentSurfaceDirtyRect = currentSurfaceDirtyManager->GetCurrentFrameDirtyRegion();

    if (!displayDirtyRect.IntersectRect(filterRect).IsEmpty() ||
        !currentSurfaceDirtyRect.IntersectRect(filterRect).IsEmpty()) {
        currentSurfaceDirtyManager->MergeDirtyRect(filterRect);
    }

    if (currentSurfaceNode->IsTransparent()) {
        if (!displayDirtyRect.IntersectRect(filterRect).IsEmpty()) {
            displayDirtyManager->MergeDirtyRect(filterRect);
            return;
        }
        // If currentSurfaceNode is transparent and displayDirtyRect is not intersect with filterRect,
        // We should check whether window below currentSurfaceNode has dirtyRect intersect with filterRect.
        for (auto belowSurface = displayNode->GetCurAllSurfaces().begin();
            belowSurface != displayNode->GetCurAllSurfaces().end(); ++belowSurface) {
            auto belowSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*belowSurface);
            if (belowSurfaceNode == currentSurfaceNode) {
                break;
            }
            if (belowSurfaceNode == nullptr || !belowSurfaceNode->IsAppWindow()) {
                continue;
            }
            auto belowSurfaceDirtyManager = belowSurfaceNode->GetDirtyManager();
            RectI belowDirtyRect =
                belowSurfaceDirtyManager ? belowSurfaceDirtyManager->GetCurrentFrameDirtyRegion() : RectI{0, 0, 0, 0};
            if (belowDirtyRect.IsEmpty()) {
                continue;
            }
            // To minimize dirtyRect, only filterRect has intersect with both visibleRegion and dirtyRect
            // of window below, we add filterRect to displayDirtyRect and currentSurfaceDirtyRect.
            if (belowSurfaceNode->GetVisibleRegion().IsIntersectWith(filterRect) &&
                !belowDirtyRect.IntersectRect(filterRect).IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(filterRect);
                currentSurfaceDirtyManager->MergeDirtyRect(filterRect);
                break;
            }
        }
    }
}

void RSUniRenderVisitor::CalcDirtyFilterRegion(std::shared_ptr<RSDisplayRenderNode>& displayNode)
{
    if (displayNode == nullptr || displayNode->GetDirtyManager() == nullptr) {
        return;
    }
    auto displayDirtyManager = displayNode->GetDirtyManager();
    std::vector<SurfaceDirtyMgrPair> prevHwcEnabledNodes;
    for (auto it = displayNode->GetCurAllSurfaces().begin(); it != displayNode->GetCurAllSurfaces().end(); ++it) {
        auto currentSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (currentSurfaceNode == nullptr) {
            continue;
        }
        auto currentSurfaceDirtyManager = currentSurfaceNode->GetDirtyManager();
        // [planning] Update hwc surface dirty status at the same time
        UpdateHardwareNodeStatusBasedOnFilter(currentSurfaceNode, prevHwcEnabledNodes, displayDirtyManager);

        if (currentSurfaceNode->GetVisibleRegion().IsEmpty()) {
            continue;
        }
        // child node (component) has filter
        auto filterRects = currentSurfaceNode->GetChildrenNeedFilterRects();
        if (currentSurfaceNode->IsAppWindow() && !filterRects.empty()) {
            needFilter_ = needFilter_ || !currentSurfaceNode->IsStaticCached();
            for (auto& filterRect : filterRects) {
                CalcDirtyRegionForFilterNode(
                    filterRect, currentSurfaceNode, displayNode);
            }
        }
        // surfaceNode self has filter
        if (currentSurfaceNode->GetRenderProperties().NeedFilter()) {
            needFilter_ = needFilter_ || !currentSurfaceNode->IsStaticCached();
            CalcDirtyRegionForFilterNode(
                currentSurfaceNode->GetOldDirtyInSurface(), currentSurfaceNode, displayNode);
        }
    }
}

void RSUniRenderVisitor::AddContainerDirtyToGlobalDirty(std::shared_ptr<RSDisplayRenderNode>& node) const
{
    RS_OPTIONAL_TRACE_FUNC();
    auto displayDirtyManager = node->GetDirtyManager();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        if (surfaceDirtyManager == nullptr) {
            continue;
        }
        RectI surfaceDirtyRect = surfaceDirtyManager->GetCurrentFrameDirtyRegion();

        if (surfaceNode->HasContainerWindow()) {
            // If a surface's dirty is intersect with container region (which can be considered transparent)
            // should be added to display dirty region.
            // Note: we use containerRegion rather transparentRegion to bypass inner corner dirty problem.
            auto containerRegion = surfaceNode->GetContainerRegion();
            auto surfaceDirtyRegion = Occlusion::Region{Occlusion::Rect{surfaceDirtyRect}};
            auto containerDirtyRegion = containerRegion.And(surfaceDirtyRegion);
            if (!containerDirtyRegion.IsEmpty()) {
                RS_LOGD("CalcDirtyDisplayRegion merge containerDirtyRegion %{public}s region %{public}s",
                    surfaceNode->GetName().c_str(), containerDirtyRegion.GetRegionInfo().c_str());
                // plan: we can use surfacenode's absrect as containerRegion's bound
                const auto& rect = containerRegion.GetBoundRef();
                displayDirtyManager->MergeDirtyRect(
                    RectI{ rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
            }
        } else {
            // warning: if a surfacenode has transparent region and opaque region, and its dirty pattern appears in
            // transparent region and opaque region in adjacent frame, may cause displaydirty region incomplete after
            // merge history (as surfacenode's dirty region merging opaque region will enlarge surface dirty region
            // which include transparent region but not counted in display dirtyregion)
            if (!IsHardwareEnabledNodeNeedCalcGlobalDirty(surfaceNode)) {
                continue;
            }
            auto transparentRegion = surfaceNode->GetTransparentRegion();
            Occlusion::Rect tmpRect = Occlusion::Rect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
                surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
            Occlusion::Region surfaceDirtyRegion { tmpRect };
            Occlusion::Region transparentDirtyRegion = transparentRegion.And(surfaceDirtyRegion);
            if (!transparentDirtyRegion.IsEmpty()) {
                RS_LOGD("CalcDirtyDisplayRegion merge TransparentDirtyRegion %{public}s region %{public}s",
                    surfaceNode->GetName().c_str(), transparentDirtyRegion.GetRegionInfo().c_str());
                const std::vector<Occlusion::Rect>& rects = transparentDirtyRegion.GetRegionRects();
                for (const auto& rect : rects) {
                    displayDirtyManager->MergeDirtyRect(
                        RectI{ rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
                }
            }
        }
    }
}

void RSUniRenderVisitor::SetSurfaceGlobalDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node)
{
    RS_OPTIONAL_TRACE_FUNC();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsMainWindowType()) {
            continue;
        }
        // set display dirty region to surfaceNode
        surfaceNode->SetGlobalDirtyRegion(node->GetDirtyManager()->GetDirtyRegion());
        surfaceNode->SetDirtyRegionAlignedEnable(false);
    }
    Occlusion::Region curVisibleDirtyRegion;
    for (auto& it : node->GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(it);
        if (surfaceNode == nullptr || !surfaceNode->IsMainWindowType()) {
            continue;
        }
        // set display dirty region to surfaceNode
        surfaceNode->SetDirtyRegionBelowCurrentLayer(curVisibleDirtyRegion);
        auto visibleDirtyRegion = surfaceNode->GetVisibleDirtyRegion();
        curVisibleDirtyRegion = curVisibleDirtyRegion.Or(visibleDirtyRegion);
    }
}

void RSUniRenderVisitor::SetSurfaceGlobalAlignedDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node,
    const Occlusion::Region alignedDirtyRegion)
{
    RS_TRACE_FUNC();
    if (!isDirtyRegionAlignedEnable_) {
        return;
    }
    // calculate extra dirty region after 32 bits alignment
    Occlusion::Region dirtyRegion = alignedDirtyRegion;
    auto globalRectI = node->GetDirtyManager()->GetDirtyRegion();
    Occlusion::Rect globalRect {globalRectI.left_, globalRectI.top_, globalRectI.GetRight(), globalRectI.GetBottom()};
    Occlusion::Region globalRegion{globalRect};
    dirtyRegion.SubSelf(globalRegion);
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsMainWindowType()) {
            continue;
        }
        surfaceNode->SetGlobalDirtyRegion(node->GetDirtyManager()->GetDirtyRegion());
        Occlusion::Region visibleRegion = surfaceNode->GetVisibleRegion();
        Occlusion::Region surfaceAlignedDirtyRegion = surfaceNode->GetAlignedVisibleDirtyRegion();
        if (dirtyRegion.IsEmpty()) {
            surfaceNode->SetExtraDirtyRegionAfterAlignment(dirtyRegion);
        } else {
            auto extraDirtyRegion = (dirtyRegion.Sub(surfaceAlignedDirtyRegion)).And(visibleRegion);
            surfaceNode->SetExtraDirtyRegionAfterAlignment(extraDirtyRegion);
        }
        surfaceNode->SetDirtyRegionAlignedEnable(true);
    }
    Occlusion::Region curVisibleDirtyRegion;
    for (auto& it : node->GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(it);
        if (surfaceNode == nullptr || !surfaceNode->IsMainWindowType()) {
            continue;
        }
        surfaceNode->SetDirtyRegionBelowCurrentLayer(curVisibleDirtyRegion);
        auto alignedVisibleDirtyRegion = surfaceNode->GetAlignedVisibleDirtyRegion();
        curVisibleDirtyRegion.OrSelf(alignedVisibleDirtyRegion);
    }
}

void RSUniRenderVisitor::AlignGlobalAndSurfaceDirtyRegions(std::shared_ptr<RSDisplayRenderNode>& node)
{
    node->GetDirtyManager()->UpdateDirtyByAligned();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsMainWindowType()) {
            continue;
        }
        surfaceNode->GetDirtyManager()->UpdateDirtyByAligned();
    }
}

#ifdef RS_ENABLE_EGLQUERYSURFACE
std::vector<RectI> RSUniRenderVisitor::GetDirtyRects(const Occlusion::Region &region)
{
    const std::vector<Occlusion::Rect>& rects = region.GetRegionRects();
    std::vector<RectI> retRects;
    for (const Occlusion::Rect& rect : rects) {
        // origin transformation
        retRects.emplace_back(RectI(rect.left_, screenInfo_.GetRotatedHeight() - rect.bottom_,
            rect.right_ - rect.left_, rect.bottom_ - rect.top_));
    }
    RS_LOGD("GetDirtyRects size %{public}d %{public}s", region.GetSize(), region.GetRegionInfo().c_str());
    return retRects;
}
#endif

void RSUniRenderVisitor::CheckAndSetNodeCacheType(RSRenderNode& node)
{
    if (node.IsStaticCached()) {
        if (node.GetCacheType() != CacheType::CONTENT) {
            node.SetCacheType(CacheType::CONTENT);
            RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
        }

        if (!node.GetCompletedCacheSurface(threadIndex_, true) && UpdateCacheSurface(node)) {
            node.UpdateCompletedCacheSurface();
        }
    } else if (isDrawingCacheEnabled_ && GenerateNodeContentCache(node)) {
        UpdateCacheRenderNodeMapWithBlur(node);
    } else {
        if (node.GetCacheType() != CacheType::NONE) {
            node.SetCacheType(CacheType::NONE);
            if (node.GetCompletedCacheSurface(threadIndex_, false)) {
                RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
            }
        }
    }
}

bool RSUniRenderVisitor::UpdateCacheSurface(RSRenderNode& node)
{
    RS_TRACE_NAME_FMT("UpdateCacheSurface: [%llu]", node.GetId());
    CacheType cacheType = node.GetCacheType();
    if (cacheType == CacheType::NONE) {
        return false;
    }

    if (!node.GetCacheSurface(threadIndex_, true)) {
        RSRenderNode::ClearCacheSurfaceFunc func = std::bind(&RSUniRenderUtil::ClearNodeCacheSurface,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
        node.InitCacheSurface(canvas_ ? canvas_->recordingContext() : nullptr, func, threadIndex_);
#else
        node.InitCacheSurface(canvas_ ? canvas_->getGrContext() : nullptr, func, threadIndex_);
#endif
#else
        node.InitCacheSurface(canvas_ ? canvas_->GetGPUContext().get() : nullptr, func, threadIndex_);
#endif
    }
    auto surface = node.GetCacheSurface(threadIndex_, true);
    if (!surface) {
        RS_LOGE("Get CacheSurface failed");
        return false;
    }
    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(surface.get());
    if (!cacheCanvas) {
        return false;
    }

    // copy current canvas properties into cacheCanvas
    if (renderEngine_) {
        cacheCanvas->SetHighContrast(renderEngine_->IsHighContrastEnabled());
    }
    if (canvas_) {
        cacheCanvas->CopyConfiguration(*canvas_);
    }
    // Using filter cache in multi-thread environment may cause GPU memory leak or invalid textures, so we explicitly
    // disable it in sub-thread.
    cacheCanvas->SetDisableFilterCache(isSubThread_);

    // When drawing CacheSurface, all child node should be drawn.
    // So set isOpDropped_ = false here.
    bool isOpDropped = isOpDropped_;
    isOpDropped_ = false;
    isUpdateCachedSurface_ = true;

#ifndef USE_ROSEN_DRAWING
    cacheCanvas->clear(SK_ColorTRANSPARENT);
#else
    cacheCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
#endif

    swap(cacheCanvas, canvas_);
    // When cacheType == CacheType::ANIMATE_PROPERTY,
    // we should draw AnimateProperty on cacheCanvas
    if (cacheType == CacheType::ANIMATE_PROPERTY) {
        if (node.GetRenderProperties().IsShadowValid()
            && !node.GetRenderProperties().IsSpherizeValid()) {
#ifndef USE_ROSEN_DRAWING
            canvas_->save();
            canvas_->translate(node.GetShadowRectOffsetX(), node.GetShadowRectOffsetY());
#else
            canvas_->Save();
            canvas_->Translate(node.GetShadowRectOffsetX(), node.GetShadowRectOffsetY());
#endif
        }
        node.ProcessAnimatePropertyBeforeChildren(*canvas_);
    }
    node.SetDrawingCacheRootId(node.GetId());
    node.ProcessRenderContents(*canvas_);
    ProcessChildren(node);

    if (cacheType == CacheType::ANIMATE_PROPERTY) {
        if (node.GetRenderProperties().IsShadowValid()
            && !node.GetRenderProperties().IsSpherizeValid()) {
#ifndef USE_ROSEN_DRAWING
            canvas_->restore();
#else
            canvas_->Restore();
#endif
        }
        node.ProcessAnimatePropertyAfterChildren(*canvas_);
    }
    swap(cacheCanvas, canvas_);

    isUpdateCachedSurface_ = false;
    isOpDropped_ = isOpDropped;

    // To get all FreezeNode
    // execute: "param set rosen.dumpsurfacetype.enabled 2 && setenforce 0"
    // To get specific FreezeNode
    // execute: "param set rosen.dumpsurfacetype.enabled 1 && setenforce 0 && "
    // "param set rosen.dumpsurfaceid "NodeId" "
    // Png file could be found in /data
    RSBaseRenderUtil::WriteCacheRenderNodeToPng(node);
    return true;
}

void RSUniRenderVisitor::DrawSpherize(RSRenderNode& node)
{
    if (node.GetCacheType() != CacheType::ANIMATE_PROPERTY) {
        node.SetCacheType(CacheType::ANIMATE_PROPERTY);
        RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
    }
    if (!node.GetCompletedCacheSurface(threadIndex_, true) && UpdateCacheSurface(node)) {
        node.UpdateCompletedCacheSurface();
    }
    node.ProcessTransitionBeforeChildren(*canvas_);
    RSPropertiesPainter::DrawSpherize(
        node.GetRenderProperties(), *canvas_, node.GetCompletedCacheSurface(threadIndex_, true));
    node.ProcessTransitionAfterChildren(*canvas_);
}

bool RSUniRenderVisitor::DrawBlurInCache(RSRenderNode& node)
{
    if (LIKELY(curCacheFilterRects_.empty())) {
        return false;
    }
    if (curCacheFilterRects_.top().count(node.GetId())) {
        if (curGroupedNodes_.empty()) {
            // draw filter before drawing cached surface
            curCacheFilterRects_.top().erase(node.GetId());
            if (curCacheFilterRects_.empty() || !node.ChildHasFilter()) {
                // no filter to draw, return
                return true;
            }
        } else if (node.GetRenderProperties().GetBackgroundFilter() || node.GetRenderProperties().GetUseEffect()) {
            // clear hole while generating cache surface
#ifndef USE_ROSEN_DRAWING
            SkAutoCanvasRestore arc(canvas_.get(), true);
            canvas_->clipRect(RSPropertiesPainter::Rect2SkRect(node.GetRenderProperties().GetBoundsRect()));
            canvas_->clear(SK_ColorTRANSPARENT);
#else
            Drawing::AutoCanvasRestore arc(*canvas_, true);
            canvas_->ClipRect(RSPropertiesPainter::Rect2DrawingRect(node.GetRenderProperties().GetBoundsRect()),
                Drawing::ClipOp::INTERSECT, false);
            canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
#endif
        }
    } else if (curGroupedNodes_.empty() && !node.ChildHasFilter()) {
        // no filter to draw, return
        return true;
    }
    return false;
}

void RSUniRenderVisitor::DrawChildCanvasRenderNode(RSRenderNode& node)
{
     if (node.GetCacheType() == CacheType::NONE) {
         if (node.IsPureContainer()) {
             processedPureContainerNode_++;
             node.ApplyBoundsGeometry(*canvas_);
             ProcessChildren(node);
             node.RSRenderNode::ProcessTransitionAfterChildren(*canvas_);
             return;
         } else if (node.IsContentNode()) {
             node.ApplyBoundsGeometry(*canvas_);
             node.ApplyAlpha(*canvas_);
             node.ProcessRenderContents(*canvas_);
             ProcessChildren(node);
             node.RSRenderNode::ProcessTransitionAfterChildren(*canvas_);
             return;
         }
    }
    DrawChildRenderNode(node);
}

void RSUniRenderVisitor::DrawChildRenderNode(RSRenderNode& node)
{
    CacheType cacheType = node.GetCacheType();
    node.ProcessTransitionBeforeChildren(*canvas_);
    switch (cacheType) {
        case CacheType::NONE: {
            auto preCache = canvas_->GetCacheType();
            if (node.HasCacheableAnim() && isDrawingCacheEnabled_) {
                canvas_->SetCacheType(RSPaintFilterCanvas::CacheType::ENABLED);
            }
            node.ProcessAnimatePropertyBeforeChildren(*canvas_);
            node.ProcessRenderContents(*canvas_);
            ProcessChildren(node);
            node.ProcessAnimatePropertyAfterChildren(*canvas_);
            if (node.HasCacheableAnim() && isDrawingCacheEnabled_) {
                canvas_->SetCacheType(preCache);
            }
            break;
        }
        case CacheType::CONTENT: {
            node.ProcessAnimatePropertyBeforeChildren(*canvas_);
            node.DrawCacheSurface(*canvas_, threadIndex_, false);
            node.ProcessAnimatePropertyAfterChildren(*canvas_);
            cacheRenderNodeMapRects_.push_back(node.GetOldDirtyInSurface());
            break;
        }
        case CacheType::ANIMATE_PROPERTY: {
            node.DrawCacheSurface(*canvas_, threadIndex_, false);
            break;
        }
        default:
            break;
    }
    node.ProcessTransitionAfterChildren(*canvas_);
}

bool RSUniRenderVisitor::CheckIfSurfaceRenderNodeNeedProcess(RSSurfaceRenderNode& node)
{
    if (isSubThread_) {
        return true;
    }
    if (isSecurityDisplay_ && node.GetSkipLayer()) {
        RS_PROCESS_TRACE(isPhone_, false, node.GetName() + " SkipLayer Skip");
        return false;
    }
    if (!node.ShouldPaint()) {
        MarkSubHardwareEnableNodeState(node);
        RS_OPTIONAL_TRACE_NAME(node.GetName() + " Node should not paint Skip");
        RS_LOGD("RSUniRenderVisitor::IfSurfaceRenderNodeNeedProcess node: %{public}" PRIu64 " invisible",
            node.GetId());
        return false;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && isOcclusionEnabled_ && !isSecurityDisplay_) {
        MarkSubHardwareEnableNodeState(node);
        RS_PROCESS_TRACE(isPhone_, false, node.GetName() + " Occlusion Skip");
        return false;
    }
    if (node.IsAbilityComponent() && node.GetDstRect().IsEmpty() && curGroupedNodes_.empty()) {
        RS_PROCESS_TRACE(isPhone_, false, node.GetName() + " Empty AbilityComponent Skip");
        return false;
    }
    std::shared_ptr<RSSurfaceRenderNode> appNode;
    if (node.LeashWindowRelatedAppWindowOccluded(appNode)) {
        if (appNode != nullptr) {
            MarkSubHardwareEnableNodeState(*appNode);
        }
        RS_PROCESS_TRACE(isPhone_, false, node.GetName() + " App Occluded Leashwindow Skip");
        return false;
    }
    return true;
}

void RSUniRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (isUIFirst_ && isSubThread_) {
        if (auto parentNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(node.GetParent().lock()) ||
            (SceneBoardJudgement::IsSceneBoardEnabled() && (node.IsLeashWindow() || (node.IsAppWindow() &&
            !RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock()))))) {
            UpdateCacheSurface(node);
            return;
        }
    }
    if (RSSystemProperties::GetProxyNodeDebugEnabled() && node.contextClipRect_.has_value() && canvas_ != nullptr) {
        // draw transparent red rect to indicate valid clip area
        {
            RSAutoCanvasRestore acr(canvas_);
#ifndef USE_ROSEN_DRAWING
            canvas_->concat(node.contextMatrix_.value_or(SkMatrix::I()));
            SkPaint paint;
            paint.setARGB(0x80, 0xFF, 0, 0); // transparent red
            canvas_->drawRect(node.contextClipRect_.value(), paint);
#else
            canvas_->ConcatMatrix(node.contextMatrix_.value_or(Drawing::Matrix()));
            Drawing::Brush brush;
            brush.SetARGB(0xFF, 0, 0, 0x80); // transparent red
            canvas_->AttachBrush(brush);
            canvas_->DrawRect(node.contextClipRect_.value());
            canvas_->DetachBrush();
#endif
        }
        // make this node context transparent
        canvas_->MultiplyAlpha(0.5);
    }
    RS_PROCESS_TRACE(isPhone_,
        node.IsFocusedNode(currentFocusedNodeId_) || node.IsFocusedNode(focusedLeashWindowId_),
        "RSUniRender::Process:[" + node.GetName() + "] " +
        node.GetDstRect().ToString() + " Alpha: " + std::to_string(node.GetGlobalAlpha()).substr(0, 4));
    RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node:%{public}" PRIu64 ",child size:%{public}u,"
        "name:%{public}s,OcclusionVisible:%{public}d",
        node.GetId(), node.GetChildrenCount(), node.GetName().c_str(), node.GetOcclusionVisible());
#ifdef RS_ENABLE_GL
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
        auto grContext = renderEngine_->GetDrawingContext()->GetDrawingContext();
#else
        auto grContext = canvas_ != nullptr ? static_cast<GrDirectContext*>(canvas_->recordingContext()) : nullptr;
#endif
        RSTagTracker tagTracker(grContext, node.GetId(), RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE);
        node.SetGrContext(grContext);
#else
    Drawing::GPUContext* gpuContext = renderEngine_->GetRenderContext()->GetDrGPUContext();
    node.SetDrawingGPUContext(gpuContext);
#endif
#endif
    if (!CheckIfSurfaceRenderNodeNeedProcess(node)) {
        node.UpdateFilterCacheStatusWithVisible(false);
        return;
    } else {
        node.UpdateFilterCacheStatusWithVisible(true);
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    if (node.IsMainWindowType()) {
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        CollectAppNodeForHwc(curSurfaceNode_);
    }
    // skip clean surface node
    if (isOpDropped_ && node.IsAppWindow() && !dirtyCoverSubTree_ &&
        !node.SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_)) {
        RS_PROCESS_TRACE(isPhone_, false, node.GetName() + " QuickReject Skip");
        RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode skip: %{public}s", node.GetName().c_str());
        return;
    }
#endif
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode node:%{public}" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }

#ifdef RS_ENABLE_EGLQUERYSURFACE
    // when display is in rotation state, occlusion relationship will be ruined,
    // hence visibleRegions cannot be used.
    if (isOpDropped_ && node.IsAppWindow()) {
        const auto& visibleRegions = node.GetVisibleRegion().GetRegionRects();
        if (visibleRegions.size() == 1) {
#ifndef USE_ROSEN_DRAWING
            canvas_->SetVisibleRect(SkRect::MakeLTRB(
                visibleRegions[0].left_, visibleRegions[0].top_, visibleRegions[0].right_, visibleRegions[0].bottom_));
#else
            canvas_->SetVisibleRect(Drawing::Rect(
                visibleRegions[0].left_, visibleRegions[0].top_, visibleRegions[0].right_, visibleRegions[0].bottom_));
#endif
        }
    }
#endif

    // when surfacenode named "CapsuleWindow", cache the current canvas as SkImage for screen recording
    if (node.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos &&
        canvas_->GetSurface() != nullptr && needCacheImg_) {
#ifndef USE_ROSEN_DRAWING
        int angle = RSUniRenderUtil::GetRotationFromMatrix(canvas_->getTotalMatrix());
        resetRotate_ = angle != 0 && angle % 90 == 0;
        cacheImgForCapture_ = canvas_->GetSurface()->makeImageSnapshot();
#else
        int angle = RSUniRenderUtil::GetRotationFromMatrix(canvas_->GetTotalMatrix());
        resetRotate_ = angle != 0 && angle % 90 == 0;
        cacheImgForCapture_ = canvas_->GetSurface()->GetImageSnapshot();
#endif
    }

    RSAutoCanvasRestore acr(canvas_);
    auto bgAntiAliasState = RSPropertiesPainter::GetBgAntiAlias();
    if (doAnimate_ && (!ROSEN_EQ(geoPtr->GetScaleX(), 1.f) || !ROSEN_EQ(geoPtr->GetScaleY(), 1.f))) {
        // disable background antialias when surfacenode has scale animation
        RSPropertiesPainter::SetBgAntiAlias(false);
    }

    canvas_->MultiplyAlpha(property.GetAlpha());

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    // [planning] surfaceNode use frame instead
    // This is for SELF_DRAWING_NODE like RosenRenderTexture
    // BoundsRect of RosenRenderTexture is the size of video, not the size of the component.
    // The size of RosenRenderTexture is the paintRect (always be set to FrameRect) which is not passed to RenderNode
    // because RSSurfaceRenderNode is designed only affected by BoundsRect.
    // When RosenRenderTexture has child node, child node is layouted
    // according to paintRect of RosenRenderTexture, not the BoundsRect.
    // So when draw SELF_DRAWING_NODE, we should save canvas
    // to avoid child node being layout according to the BoundsRect of RosenRenderTexture.
    // Temporarily, we use parent of SELF_DRAWING_NODE which has the same paintRect with its child instead.
    // to draw child node of SELF_DRAWING_NODE
#ifndef USE_ROSEN_DRAWING
    if (isSelfDrawingSurface && !property.IsSpherizeValid()) {
        canvas_->save();
    }

    canvas_->concat(geoPtr->GetMatrix());
#else
    if (isSelfDrawingSurface && !property.IsSpherizeValid()) {
        canvas_->Save();
    }

    canvas_->ConcatMatrix(geoPtr->GetMatrix());
#endif
    bool isSubNodeOfSurfaceInProcess = isSubNodeOfSurfaceInProcess_;
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        isSubNodeOfSurfaceInProcess_ = true;
    }
    if (property.IsSpherizeValid()) {
        DrawSpherize(node);
    } else {
        if (isSelfDrawingSurface) {
            RSUniRenderUtil::FloorTransXYInCanvasMatrix(*canvas_);
        }
        if (isUIFirst_ && node.GetCacheType() == CacheType::ANIMATE_PROPERTY) {
            RSUniRenderUtil::HandleSubThreadNode(node, *canvas_);
            if (node.IsMainWindowType() || node.IsLeashWindow()) {
                isSubNodeOfSurfaceInProcess_ = isSubNodeOfSurfaceInProcess;
            }
            return;
        }
        node.ProcessRenderBeforeChildren(*canvas_);
        if (isUIFirst_ && RSUniRenderUtil::HandleSubThreadNode(node, *canvas_)) {
            node.ProcessRenderAfterChildren(*canvas_);
            if (node.IsMainWindowType() || node.IsLeashWindow()) {
                isSubNodeOfSurfaceInProcess_ = isSubNodeOfSurfaceInProcess;
            }
            return;
        }
        if (node.GetBuffer() != nullptr) {
            if (node.IsHardwareEnabledType()) {
                // since node has buffer, hwc disabledState could be reset by filter or surface cached
                bool backgroundTransparent =
                    static_cast<uint8_t>(node.GetRenderProperties().GetBackgroundColor().GetAlpha()) < UINT8_MAX;
                bool rotationDisabled = false;
                if (node.IsRosenWeb()) {
                    int rotation = RSUniRenderUtil::GetRotationFromMatrix(node.GetTotalMatrix());
                    if (rotation == ROTATION_90 || rotation == ROTATION_270) {
                        rotationDisabled = true;
                    }
                }
                node.SetHardwareForcedDisabledState(
                    (node.IsHardwareForcedDisabledByFilter() || canvas_->GetAlpha() < 1.f ||
                    backgroundTransparent || rotationDisabled) &&
                    (!node.IsHardwareEnabledTopSurface() || node.HasSubNodeShouldPaint()));
                node.SetHardwareDisabledByCache(isUpdateCachedSurface_);
            }
            // if this window is in freeze state, disable hardware composer for its child surfaceView
            if (IsHardwareComposerEnabled() && !node.IsHardwareForcedDisabled() && node.IsHardwareEnabledType()) {
#ifndef USE_ROSEN_DRAWING
                if (!node.IsHardwareEnabledTopSurface()) {
                    canvas_->clear(SK_ColorTRANSPARENT);
                }
#else
                canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
#endif
                node.SetGlobalAlpha(canvas_->GetAlpha());
                ParallelRenderEnableHardwareComposer(node);

                {
#ifndef USE_ROSEN_DRAWING
                    SkAutoCanvasRestore acr(canvas_.get(), true);

                    if (displayNodeMatrix_.has_value()) {
                        auto& displayNodeMatrix = displayNodeMatrix_.value();
                        canvas_->concat(displayNodeMatrix);
                    }
                    node.SetTotalMatrix(canvas_->getTotalMatrix());

                    auto dstRect = node.GetDstRect();
                    SkIRect dst = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom() };
#else
                    Drawing::AutoCanvasRestore acr(*canvas_.get(), true);

                    if (displayNodeMatrix_.has_value()) {
                        auto& displayNodeMatrix = displayNodeMatrix_.value();
                        displayNodeMatrix.PreConcat(canvas_->GetTotalMatrix());
                        canvas_->SetMatrix(displayNodeMatrix);
                    }
                    node.SetTotalMatrix(canvas_->GetTotalMatrix());

                    auto dstRect = node.GetDstRect();
                    Drawing::RectI dst = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(),
                        dstRect.GetBottom() };
#endif
                    node.UpdateSrcRect(*canvas_, dst);
                }
                RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode src:%{public}s, dst:%{public}s name:%{public}s"
                    " id:%{public}" PRIu64 "", node.GetSrcRect().ToString().c_str(),
                    node.GetDstRect().ToString().c_str(), node.GetName().c_str(), node.GetId());
            } else {
                node.SetGlobalAlpha(1.0f);
                auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false, threadIndex_);
                renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
            }
        }

        if (isSelfDrawingSurface) {
#ifndef USE_ROSEN_DRAWING
            canvas_->restore();
#else
            canvas_->Restore();
#endif
        }

        if (node.IsAppWindow()) {
            if (!node.IsNotifyUIBufferAvailable() && IsFirstFrameReadyToDraw(node)) {
                node.NotifyUIBufferAvailable();
            }
            CheckAndSetNodeCacheType(node);
            DrawChildRenderNode(node);
        } else {
            ProcessChildren(node);
        }
        node.ProcessRenderAfterChildren(*canvas_);
    }

    RSPropertiesPainter::SetBgAntiAlias(bgAntiAliasState);
    if (node.IsAppWindow()) {
#ifndef USE_ROSEN_DRAWING
        canvas_->SetVisibleRect(SkRect::MakeLTRB(0, 0, 0, 0));
#else
        canvas_->SetVisibleRect(Drawing::Rect(0, 0, 0, 0));
#endif

        // count processed canvas node
        RS_OPTIONAL_TRACE_NAME_FMT("%s PureContainerNode/ProcessedNodes: %u/%u", node.GetName().c_str(),
            processedPureContainerNode_, processedCanvasNodeInCurrentSurface_);
        processedCanvasNodeInCurrentSurface_ = 0; // reset
        processedPureContainerNode_ = 0;
    }
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        isSubNodeOfSurfaceInProcess_ = isSubNodeOfSurfaceInProcess;
        // release full children list used by sub thread
    }
}

void RSUniRenderVisitor::ProcessProxyRenderNode(RSProxyRenderNode& node)
{
    if (RSSystemProperties::GetProxyNodeDebugEnabled() && node.contextClipRect_.has_value() &&
        node.target_.lock() != nullptr) {
        // draw transparent green rect to indicate clip area of proxy node
#ifndef USE_ROSEN_DRAWING
        SkPaint paint;
        paint.setARGB(0x80, 0, 0xFF, 0); // transparent green
        canvas_->drawRect(node.contextClipRect_.value(), paint);
#else
        Drawing::Brush brush;
        brush.SetARGB(0, 0xFF, 0, 0x80); // transparent green
        canvas_->AttachBrush(brush);
        canvas_->DrawRect(node.contextClipRect_.value());
        canvas_->DetachBrush();
#endif
    }
    ProcessChildren(node);
}

void RSUniRenderVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode node: %{public}" PRIu64 ", child size:%{public}u", node.GetId(),
        node.GetChildrenCount());
    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessRootRenderNode, canvas is nullptr");
        return;
    }

    ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
    int saveCount;
    if (colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
        colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE) {
        RS_LOGD("RsDebug RSBaseRenderEngine::SetColorFilterModeToPaint mode:%{public}d",
            static_cast<int32_t>(colorFilterMode));
#ifndef USE_ROSEN_DRAWING
        SkPaint paint;
        RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
#ifdef RS_ENABLE_GL
#ifdef NEW_RENDER_CONTEXT
            RSTagTracker tagTracker(
                renderEngine_->GetDrawingContext()->GetDrawingContext(),
                RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#else
            RSTagTracker tagTracker(
                renderEngine_->GetRenderContext()->GetGrContext(), RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#endif
#endif
        saveCount = canvas_->saveLayer(nullptr, &paint);
    } else {
        saveCount = canvas_->save();
    }
    ProcessCanvasRenderNode(node);
    canvas_->restoreToCount(saveCount);
#else
        Drawing::Brush brush;
        RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, brush);
        Drawing::SaveLayerOps saveLayerOps(nullptr, &brush);
        saveCount = canvas_->GetSaveCount();
        canvas_->SaveLayer(saveLayerOps);
    } else {
        saveCount = canvas_->GetSaveCount();
        canvas_->Save();
    }
    ProcessCanvasRenderNode(node);
    canvas_->RestoreToCount(saveCount);
#endif
}

bool RSUniRenderVisitor::GenerateNodeContentCache(RSRenderNode& node)
{
    // Node cannot have cache.
    uint32_t cacheRenderNodeMapCnt;
    if (node.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE) {
        {
            std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
            cacheRenderNodeMapCnt = cacheRenderNodeMap.count(node.GetId());
        }
        if (cacheRenderNodeMapCnt > 0) {
            node.SetCacheType(CacheType::NONE);
            RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
            {
                std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
                cacheRenderNodeMap.erase(node.GetId());
            }
            {
                std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
                groupedTransitionNodes.erase(node.GetId());
            }
        }
        return false;
    }

    // The node goes down the tree to clear the cache.
    {
        std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
        cacheRenderNodeMapCnt = cacheRenderNodeMap.count(node.GetId());
    }
    if (node.GetCacheType() == CacheType::NONE && cacheRenderNodeMapCnt > 0) {
        {
            std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
            cacheRenderNodeMap.erase(node.GetId());
        }
        {
            std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
            groupedTransitionNodes.erase(node.GetId());
        }
        return false;
    }
    return true;
}

bool RSUniRenderVisitor::InitNodeCache(RSRenderNode& node)
{
    if (node.GetDrawingCacheType() == RSDrawingCacheType::FORCED_CACHE ||
        node.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE) {
        uint32_t cacheRenderNodeMapCnt;
        {
            std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
            cacheRenderNodeMapCnt = cacheRenderNodeMap.count(node.GetId());
        }
        if (cacheRenderNodeMapCnt == 0 || node.NeedInitCacheSurface()) {
#ifndef USE_ROSEN_DRAWING
            RenderParam val { node.shared_from_this(), canvas_->GetCanvasStatus() };
#else
            RenderParam val { node.shared_from_this(), canvas_->GetAlpha(), canvas_->GetTotalMatrix() };
#endif
            curGroupedNodes_.push(val);
            {
                std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
                groupedTransitionNodes[node.GetId()] = { val, {} };
            }
            node.SetCacheType(CacheType::CONTENT);
            RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
            if (UpdateCacheSurface(node)) {
                node.UpdateCompletedCacheSurface();
                ChangeCacheRenderNodeMap(node);
                cacheReuseTimes = 0;
                node.ResetDrawingCacheNeedUpdate();
            }
            curGroupedNodes_.pop();
            return true;
        }
    }
    return false;
}

void RSUniRenderVisitor::ChangeCacheRenderNodeMap(RSRenderNode& node, const uint32_t count)
{
    std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
    cacheRenderNodeMap[node.GetId()] = count;
}

void RSUniRenderVisitor::UpdateCacheRenderNodeMapWithBlur(RSRenderNode& node)
{
    curCacheFilterRects_.push(allCacheFilterRects_[node.GetId()]);
    auto canvasType = canvas_->GetCacheType();
    canvas_->SetCacheType(RSPaintFilterCanvas::CacheType::OFFSCREEN);
    UpdateCacheRenderNodeMap(node);
    canvas_->SetCacheType(canvasType);
    RS_TRACE_NAME_FMT("Draw cache with blur [%llu]", node.GetId());
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore arc(canvas_.get(), true);
#else
    Drawing::AutoCanvasRestore arc(*canvas_, true);
#endif
    auto nodeType = node.GetCacheType();
    node.SetCacheType(CacheType::NONE);
    bool isOpDropped = isOpDropped_;
    isOpDropped_ = false;
    DrawChildRenderNode(node);
    isOpDropped_ = isOpDropped;
    node.SetCacheType(nodeType);
    curCacheFilterRects_.pop();
}

void RSUniRenderVisitor::UpdateCacheRenderNodeMap(RSRenderNode& node)
{
    if (InitNodeCache(node)) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::UpdateCacheRenderNodeMap, generate the node cache for the first"
            "time, NodeId: %" PRIu64 " ", node.GetId());
        return;
    }
    uint32_t updateTimes = 0;
    if (node.GetDrawingCacheType() == RSDrawingCacheType::FORCED_CACHE) {
        // Regardless of the number of consecutive refreshes, the current cache is forced to be updated.
        if (node.GetDrawingCacheChanged()) {
#ifndef USE_ROSEN_DRAWING
            RenderParam val { node.shared_from_this(), canvas_->GetCanvasStatus() };
#else
            RenderParam val { node.shared_from_this(), canvas_->GetAlpha(), canvas_->GetTotalMatrix() };
#endif
            curGroupedNodes_.push(val);
            {
                std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
                groupedTransitionNodes[node.GetId()] = { val, {} };
            }
            {
                std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
                updateTimes = cacheRenderNodeMap[node.GetId()] + 1;
            }
            node.SetCacheType(CacheType::CONTENT);
            if (UpdateCacheSurface(node)) {
                node.UpdateCompletedCacheSurface();
                ChangeCacheRenderNodeMap(node, updateTimes);
                cacheReuseTimes = 0;
                node.ResetDrawingCacheNeedUpdate();
            }
            curGroupedNodes_.pop();
            return;
        }
    }
    if (node.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE) {
        // If the number of consecutive refreshes exceeds CACHE_MAX_UPDATE_TIME times, the cache is cleaned,
        // otherwise the cache is updated.
        if (node.GetDrawingCacheChanged()) {
            {
                std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
                updateTimes = cacheRenderNodeMap[node.GetId()] + 1;
            }
            if (updateTimes >= CACHE_MAX_UPDATE_TIME) {
                node.SetCacheType(CacheType::NONE);
                RSUniRenderUtil::ClearCacheSurface(node, threadIndex_);
                cacheRenderNodeMap.erase(node.GetId());
                groupedTransitionNodes.erase(node.GetId());
                cacheReuseTimes = 0;
                return;
            }
#ifndef USE_ROSEN_DRAWING
            RenderParam val { node.shared_from_this(), canvas_->GetCanvasStatus() };
#else
            RenderParam val { node.shared_from_this(), canvas_->GetAlpha(), canvas_->GetTotalMatrix() };
#endif
            curGroupedNodes_.push(val);
            {
                std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
                groupedTransitionNodes[node.GetId()] = { val, {} };
            }
            node.SetCacheType(CacheType::CONTENT);
            if (UpdateCacheSurface(node)) {
                node.UpdateCompletedCacheSurface();
                ChangeCacheRenderNodeMap(node, updateTimes);
                cacheReuseTimes = 0;
                node.ResetDrawingCacheNeedUpdate();
            }
            curGroupedNodes_.pop();
            return;
        }
    }
    // The cache is not refreshed continuously.
    ChangeCacheRenderNodeMap(node);
    cacheReuseTimes++;
    RS_OPTIONAL_TRACE_NAME("RSUniRenderVisitor::UpdateCacheRenderNodeMap ,NodeId: " + std::to_string(node.GetId()) +
        " ,CacheRenderNodeMapCnt: " + std::to_string(cacheReuseTimes));
}

void RSUniRenderVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    processedCanvasNodeInCurrentSurface_++;
    if (!node.ShouldPaint()) {
        return;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    if ((isOpDropped_ && (curSurfaceNode_ != nullptr)) || isCanvasNodeSkipDfxEnabled_) {
        // If all the child nodes have drawing areas that do not exceed the current node, then current node
        // can be directly skipped if not intersect with any dirtyregion.
        // Otherwise, its childrenRect_ should be considered.
        RectI dirtyRect = node.HasChildrenOutOfRect() ?
            node.GetOldDirtyInSurface().JoinRect(node.GetChildrenRect()) : node.GetOldDirtyInSurface();
        if (isSubNodeOfSurfaceInProcess_ && !dirtyRect.IsEmpty() && !node.IsAncestorDirty() && !dirtyCoverSubTree_ &&
            !curSurfaceNode_->SubNodeNeedDraw(dirtyRect, partialRenderType_) && !node.IsParentLeashWindow()) {
            if (isCanvasNodeSkipDfxEnabled_) {
                curSurfaceNode_->GetDirtyManager()->UpdateDirtyRegionInfoForDfx(
                    node.GetId(), node.GetType(), DirtyRegionType::CANVAS_NODE_SKIP_RECT, dirtyRect);
            } else {
                return;
            }
        }
    }
#endif
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    // clip hole for driven render
    if (drivenInfo_ && !drivenInfo_->prepareInfo.hasInvalidScene &&
        drivenInfo_->currDrivenRenderMode != DrivenUniRenderMode::RENDER_WITH_NORMAL) {
        // skip render driven node sub tree
        if (RSDrivenRenderManager::GetInstance().ClipHoleForDrivenNode(*canvas_, node)) {
            return;
        }
    }
#endif
    // in case preparation'update is skipped
#ifndef USE_ROSEN_DRAWING
    canvas_->save();
#else
    canvas_->Save();
#endif
    if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        RSUniRenderUtil::FloorTransXYInCanvasMatrix(*canvas_);
    }
    if (node.GetSharedTransitionParam().has_value()) {
        // draw self and children in sandbox which will not be affected by parent's transition
        const auto& sandboxMatrix = node.GetRenderProperties().GetSandBoxMatrix();
        if (sandboxMatrix) {
#ifndef USE_ROSEN_DRAWING
            canvas_->setMatrix(*sandboxMatrix);
#else
            canvas_->SetMatrix(*sandboxMatrix);
#endif
        }
    }
    const auto& property = node.GetRenderProperties();
    if (property.IsSpherizeValid()) {
        DrawSpherize(node);
        return;
    }
    if (auto drawingNode = node.ReinterpretCastTo<RSCanvasDrawingRenderNode>()) {
#ifndef USE_ROSEN_DRAWING
        auto clearFunc = [id = threadIndex_](sk_sp<SkSurface> surface) {
            // The second param is null, 0 is an invalid value.
            RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, id, 0);
        };
#else
        auto clearFunc = [id = threadIndex_](std::shared_ptr<Drawing::Surface> surface) {
            // The second param is null, 0 is an invalid value.
            RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, id, 0);
        };
#endif
        drawingNode->SetSurfaceClearFunc({ threadIndex_, clearFunc });
    }
    CheckAndSetNodeCacheType(node);
    DrawChildCanvasRenderNode(node);
#ifndef USE_ROSEN_DRAWING
    canvas_->restore();
#else
    canvas_->Restore();
#endif
}

void RSUniRenderVisitor::ProcessEffectRenderNode(RSEffectRenderNode& node)
{
    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniRenderVisitor::ProcessEffectRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessEffectRenderNode, canvas is nullptr");
        return;
    }
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(canvas_.get(), true);
#else
    Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
#endif
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSUniRenderVisitor::PrepareOffscreenRender(RSRenderNode& node)
{
    RS_TRACE_NAME("PrepareOffscreenRender");
    // cleanup
    canvasBackup_ = nullptr;
    offscreenSurface_ = nullptr;
    // check offscreen size and hardware renderer
    int32_t offscreenWidth = node.GetRenderProperties().GetFrameWidth();
    int32_t offscreenHeight = node.GetRenderProperties().GetFrameHeight();
    if (offscreenWidth <= 0 || offscreenHeight <= 0) {
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, offscreenWidth or offscreenHeight is invalid");
        return;
    }
#ifndef USE_ROSEN_DRAWING
    if (canvas_->GetSurface() == nullptr) {
        canvas_->clipRect(SkRect::MakeWH(offscreenWidth, offscreenHeight));
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, current surface is nullptr (software renderer?)");
        return;
    }
    // create offscreen surface and canvas
    offscreenSurface_ = canvas_->GetSurface()->makeSurface(offscreenWidth, offscreenHeight);
    if (offscreenSurface_ == nullptr) {
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, offscreenSurface is nullptr");
        canvas_->clipRect(SkRect::MakeWH(offscreenWidth, offscreenHeight));
        return;
    }
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface_.get());

    // copy current canvas properties into offscreen canvas
    offscreenCanvas->CopyConfiguration(*canvas_);

    // backup current canvas and replace with offscreen canvas
    canvasBackup_ = std::exchange(canvas_, offscreenCanvas);
#else
    if (canvas_->GetSurface() == nullptr) {
        canvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        RS_LOGD("RSUniRenderVisitor::PrepareOffscreenRender, current surface is nullptr (software renderer?)");
        return;
    }
#endif
}

void RSUniRenderVisitor::FinishOffscreenRender()
{
    if (canvasBackup_ == nullptr) {
        RS_LOGD("RSUniRenderVisitor::FinishOffscreenRender, canvasBackup_ is nullptr");
        return;
    }
    RS_TRACE_NAME("RSUniRenderVisitor::OffscreenRender finish");
    // flush offscreen canvas, maybe unnecessary
#ifndef USE_ROSEN_DRAWING
    canvas_->flush();
    // draw offscreen surface to current canvas
    SkPaint paint;
    paint.setAntiAlias(true);
#ifdef NEW_SKIA
    canvasBackup_->drawImage(offscreenSurface_->makeImageSnapshot(), 0, 0, SkSamplingOptions(), &paint);
#else
    canvasBackup_->drawImage(offscreenSurface_->makeImageSnapshot(), 0, 0, &paint);
#endif
#else
    canvas_->Flush();
    // draw offscreen surface to current canvas
    Drawing::Brush paint;
    paint.SetAntiAlias(true);
    canvasBackup_->AttachBrush(paint);
    Drawing::SamplingOptions sampling =
        Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    canvasBackup_->DrawImage(*offscreenSurface_->GetImageSnapshot().get(), 0, 0, sampling);
    canvasBackup_->DetachBrush();
#endif
    // restore current canvas and cleanup
    offscreenSurface_ = nullptr;
    canvas_ = std::move(canvasBackup_);
}

bool RSUniRenderVisitor::AdaptiveSubRenderThreadMode(bool doParallel)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    doParallel = (doParallel && (parallelRenderType_ != ParallelRenderingType::DISABLE));
    if (!doParallel) {
        return doParallel;
    }
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    switch (parallelRenderType_) {
        case ParallelRenderingType::AUTO:
            parallelRenderManager->SetParallelMode(doParallel);
            break;
        case ParallelRenderingType::DISABLE:
            parallelRenderManager->SetParallelMode(false);
            break;
        case ParallelRenderingType::ENABLE:
            parallelRenderManager->SetParallelMode(true);
            break;
    }
    return doParallel;
#else
    return false;
#endif
}
void RSUniRenderVisitor::ParallelRenderEnableHardwareComposer(RSSurfaceRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined (RS_ENABLE_GL)
    if (isParallel_ && !isUIFirst_) {
        const auto& property = node.GetRenderProperties();
        auto dstRect = node.GetDstRect();
        RectF clipRect = {dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetWidth(), dstRect.GetHeight()};
        RSParallelRenderManager::Instance()->AddSelfDrawingSurface(parallelRenderVisitorIndex_,
            property.GetCornerRadius().IsZero(), clipRect, property.GetCornerRadius());
    }
#endif
}

void RSUniRenderVisitor::ClosePartialRenderWhenAnimatingWindows(std::shared_ptr<RSDisplayRenderNode>& node)
{
    if (!doAnimate_) {
        return;
    }
    if (appWindowNum_ > PHONE_MAX_APP_WINDOW_NUM) {
        node->GetDirtyManager()->MergeSurfaceRect();
    } else {
        isPartialRenderEnabled_ = false;
        isOpDropped_ = false;
        RS_TRACE_NAME("ClosePartialRender 0 Window Animation");
    }
}

void RSUniRenderVisitor::SetHardwareEnabledNodes(
    const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hardwareEnabledNodes)
{
    hardwareEnabledNodes_ = hardwareEnabledNodes;
}

bool RSUniRenderVisitor::DoDirectComposition(std::shared_ptr<RSBaseRenderNode> rootNode)
{
    if (!IsHardwareComposerEnabled() || rootNode->GetSortedChildren().empty()) {
        RS_LOGD("RSUniRenderVisitor::DoDirectComposition HardwareComposer disabled");
        return false;
    }
    RS_TRACE_NAME("DoDirectComposition");
    auto child = rootNode->GetSortedChildren().front();
    if (child == nullptr || !child->IsInstanceOf<RSDisplayRenderNode>()) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition child type not match");
        return false;
    }
    auto displayNode = child->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNode ||
        displayNode->GetCompositeType() != RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition displayNode state error");
        return false;
    }
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    screenInfo_ = screenManager->QueryScreenInfo(displayNode->GetScreenId());
    if (screenInfo_.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: ScreenState error!");
        return false;
    }
    processor_ = RSProcessorFactory::CreateProcessor(displayNode->GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: RSProcessor is null!");
        return false;
    }

    if (renderEngine_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: renderEngine is null!");
        return false;
    }
    if (!processor_->Init(*displayNode, displayNode->GetDisplayOffsetX(), displayNode->GetDisplayOffsetY(),
        INVALID_SCREEN_ID, renderEngine_)) {
        RS_LOGE("RSUniRenderVisitor::DoDirectComposition: processor init failed!");
        return false;
    }
    processor_->ProcessDisplaySurface(*displayNode);
    for (auto& node: hardwareEnabledNodes_) {
        if (!node->IsHardwareForcedDisabled()) {
            processor_->ProcessSurface(*node);
        }
    }
    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExcute()) {
        RS_LOGW("RSUniRenderVisitor::DoDirectComposition: hardwareThread task has too many to excute");
    }
    processor_->PostProcess();
    RS_LOGD("RSUniRenderVisitor::DoDirectComposition end");
    return true;
}

void RSUniRenderVisitor::DrawWatermarkIfNeed()
{
    if (RSMainThread::Instance()->GetWatermarkFlag()) {
#ifndef USE_ROSEN_DRAWING
        sk_sp<SkImage> skImage = RSMainThread::Instance()->GetWatermarkImg();
        if (!skImage) {
            RS_LOGE("RSUniRenderVisitor::DrawWatermarkIfNeed: no image!");
            return;
        }
        SkPaint rectPaint;
        auto skSrcRect = SkRect::MakeWH(skImage->width(), skImage->height());
        auto skDstRect = SkRect::MakeWH(screenInfo_.width, screenInfo_.height);
#ifdef NEW_SKIA
        canvas_->drawImageRect(
            skImage, skSrcRect, skDstRect, SkSamplingOptions(), &rectPaint, SkCanvas::kStrict_SrcRectConstraint);
#else
        canvas_->drawImageRect(skImage, skSrcRect, skDstRect, &rectPaint);
#endif
#else
        std::shared_ptr<Drawing::Image> drImage = RSMainThread::Instance()->GetWatermarkImg();
        if (drImage == nullptr) {
            return;
        }
        Drawing::Brush rectPaint;
        canvas_->AttachBrush(rectPaint);
        auto srcRect = Drawing::Rect(0, 0, drImage->GetWidth(), drImage->GetHeight());
        auto dstRect = Drawing::Rect(0, 0, screenInfo_.width, screenInfo_.height);
        canvas_->DrawImageRect(*drImage, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        canvas_->DetachBrush();
#endif
    }
}


void RSUniRenderVisitor::SetAppWindowNum(uint32_t num)
{
    appWindowNum_ = num;
}

bool RSUniRenderVisitor::ParallelComposition(const std::shared_ptr<RSBaseRenderNode> rootNode)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined (RS_ENABLE_GL)
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    doParallelComposition_ = true;
    doParallelComposition_ = AdaptiveSubRenderThreadMode(doParallelComposition_) &&
                             parallelRenderManager->GetParallelMode();
    if (doParallelComposition_) {
        parallelRenderManager->PackParallelCompositionTask(shared_from_this(), rootNode);
        parallelRenderManager->LoadBalanceAndNotify(TaskType::COMPOSITION_TASK);
        parallelRenderManager->WaitCompositionEnd();
    } else {
        return false;
    }
    return true;
#else
    return false;
#endif
}

void RSUniRenderVisitor::PrepareSharedTransitionNode(RSBaseRenderNode& node)
{
    // Sanity check done by caller, transitionParam should always has value.
    auto& transitionParam = node.GetSharedTransitionParam();

    auto pairedNode = transitionParam->second.lock();
    if (pairedNode == nullptr) {
        // paired node is already destroyed, clear transition param and prepare directly
        node.SetSharedTransitionParam(std::nullopt);
        return;
    }

    auto& pairedParam = pairedNode->GetSharedTransitionParam();
    if (!pairedParam.has_value() || pairedParam->first != transitionParam->first) {
        // if 1. paired node is not a transition node or 2. paired node is not paired with this node, then clear
        // transition param and prepare directly
        node.SetSharedTransitionParam(std::nullopt);
        return;
    }

    // hack to ensure that dirty region will include the whole shared-transition nodes, and won't be clipped by parent
    // clip rect.
    prepareClipRect_.SetAll(0, 0, INT_MAX, INT_MAX);
}

bool RSUniRenderVisitor::ProcessSharedTransitionNode(RSBaseRenderNode& node)
{
    // Sanity check done by caller, transitionParam should always has value.
    auto& transitionParam = node.GetSharedTransitionParam();
    if (LIKELY(!transitionParam.has_value())) {
        // non-transition node, prepare directly
        return true;
    }

    // Note: Sanity checks for shared transition nodes are already done in prepare phase, no need to do it again.
    // use transition key (in node id) as map index.
    auto key = transitionParam->first;
    // paired node is already visited, process both nodes in order.
    if (auto existingNodeIter = unpairedTransitionNodes_.find(key);
        existingNodeIter != unpairedTransitionNodes_.end()) {
        RSAutoCanvasRestore acr(canvas_);
        // restore render context and process the paired node.
#ifndef USE_ROSEN_DRAWING
        auto& [node, canvasStatus] = existingNodeIter->second;
        canvas_->SetCanvasStatus(canvasStatus);
#else
        auto& [node, alpha, mat] = existingNodeIter->second;
        canvas_->SetAlpha(alpha);
        canvas->SetMatrix(mat.value());
#endif
        node->Process(shared_from_this());
        unpairedTransitionNodes_.erase(existingNodeIter);
        return true;
    }
    groupedTransitionNodesType nodes;
    {
        std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
        nodes = groupedTransitionNodes;
    }
    for (auto& [unused, pair] : nodes) {
        if (auto existingNodeIter = pair.second.find(key); existingNodeIter != pair.second.end()) {
            RSAutoCanvasRestore acr(canvas_);
            // restore render context and process the paired node.
#ifndef USE_ROSEN_DRAWING
            auto& [unused2, PreCanvasStatus] = pair.first;
            auto& [child, canvasStatus] = existingNodeIter->second;
            canvas_->SetCanvasStatus(canvasStatus);
            canvas_->MultiplyAlpha(PreCanvasStatus.alpha_);
            canvas_->concat(PreCanvasStatus.matrix_);
#else
            auto& [unused2, alpha1, mat1] = pair.first;
            auto& [child, alhpa2, mat2] = existingNodeIter->second;
            canvas->SetAlpha(alpha1);
            canvas_->SetMatrix(mat1.value());
            canvas_->MultipleyAlpha(alpha2);
            canvas_->ConcatMatrix(mat2.value());
#endif
            child->Process(shared_from_this());
            return true;
        }
    }

    auto pairedNode = transitionParam->second.lock();
    if (pairedNode == nullptr || pairedNode->GetGlobalAlpha() <= 0.0f) {
        // visitor may never visit the paired node, ignore the transition logic and process directly.
        return true;
    }

    if (!curGroupedNodes_.empty()) {
        // if in node group cache, add this node and render params (alpha and matrix) into groupedTransitionNodes.
#ifndef USE_ROSEN_DRAWING
        auto& [child, currentStatus] = curGroupedNodes_.top();
        auto canvasStatus = canvas_->GetCanvasStatus();
        if (!ROSEN_EQ(currentStatus.alpha_, 0.f)) {
            canvasStatus.alpha_ /= currentStatus.alpha_;
        } else {
            RS_LOGE("RSUniRenderVisitor::ProcessSharedTransitionNode: alpha_ is zero");
        }
        if (!currentStatus.matrix_.invert(&canvasStatus.matrix_)) {
            RS_LOGE("RSUniRenderVisitor::ProcessSharedTransitionNode invert failed");
        }
        RenderParam value { node.shared_from_this(), canvasStatus };
#else
        auto& [child, alpha, matrix] = curGroupedNodes_.top();
        if (!matrix->Invert(matrix)) {
            RS_LOGE("RSUniRenderVisitor::ProcessSharedTransitionNode invert failed");
        }
        RenderParam value { node.shared_from_this(), canvas_->GetAlpha() / alpha, matrix };
#endif
        {
            std::lock_guard<std::mutex> lock(groupedTransitionNodesMutex);
            groupedTransitionNodes[child->GetId()].second.emplace(key, std::move(value));
        }
        return false;
    }

    // all sanity checks passed, add this node and render params (alpha and matrix) into unpairedTransitionNodes_.
#ifndef USE_ROSEN_DRAWING
    RenderParam value { node.shared_from_this(), canvas_->GetCanvasStatus() };
#else
    RenderParam value { node.shared_from_this(), canvas_->GetAlpha(), canvas_->GetTotalMatrix() };
#endif
    unpairedTransitionNodes_.emplace(key, std::move(value));

    // skip processing the current node and all its children.
    return false;
}

void RSUniRenderVisitor::ProcessUnpairedSharedTransitionNode()
{
    // Do cleanup for unpaired transition nodes.
    for (auto& [key, params] : unpairedTransitionNodes_) {
        RSAutoCanvasRestore acr(canvas_);
        auto& [node, canvasStatus] = params;
        // restore render context and process the unpaired node.
        canvas_->SetCanvasStatus(canvasStatus);
        node->Process(shared_from_this());
        // clear transition param
        node->SetSharedTransitionParam(std::nullopt);
    }
    unpairedTransitionNodes_.clear();
}

void RSUniRenderVisitor::tryCapture(float width, float height)
{
    if (!RSSystemProperties::GetRecordingEnabled()) {
        return;
    }
    recordingCanvas_ = std::make_unique<RSRecordingCanvas>(width, height);
    RS_TRACE_NAME("RSUniRender:Recording begin");
#ifdef RS_ENABLE_GL
#ifdef NEW_SKIA
    recordingCanvas_->SetGrRecordingContext(canvas_->recordingContext());
#else
    recordingCanvas_->SetGrContext(canvas_->getGrContext()); // SkImage::MakeFromCompressed need GrContext
#endif
#endif
    canvas_->addCanvas(recordingCanvas_.get());
    RSRecordingThread::Instance().CheckAndRecording();
}
 
void RSUniRenderVisitor::endCapture() const
{
    if (!RSRecordingThread::Instance().GetRecordingEnabled()) {
        return;
    }
    auto drawCmdList = recordingCanvas_->GetDrawCmdList();
    RS_TRACE_NAME("RSUniRender:RecordingToFile curFrameNum = " +
                   std::to_string(RSRecordingThread::Instance().GetCurDumpFrame()));
    RSRecordingThread::Instance().RecordingToFile(drawCmdList);
}
} // namespace Rosen
} // namespace OHOS
