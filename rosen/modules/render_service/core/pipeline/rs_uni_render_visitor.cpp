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
#include <memory>
#include "rs_trace.h"
#include "screen_manager/rs_screen_manager.h"

#ifdef RS_ENABLE_OLD_VK
#include <vulkan_window.h>
#endif

#include "common/rs_common_def.h"
#include "common/rs_common_hook.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "luminance/rs_luminance_control.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_realtime_refresh_rate_manager.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uifirst_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "system/rs_system_parameters.h"
#include "hgm_core.h"
#include "metadata_helper.h"
#include <v1_0/buffer_handle_meta_key_type.h>
#include <v1_0/cm_color_space.h>

#include "pipeline/round_corner_display/rs_round_corner_display.h"
#include "pipeline/round_corner_display/rs_message_bus.h"

#include "rs_profiler.h"
#ifdef RS_PROFILER_ENABLED
#include "rs_profiler_capture_recorder.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t VISIBLEAREARATIO_FORQOS = 3;
constexpr int MAX_ALPHA = 255;
constexpr int TRACE_LEVEL_THREE = 3;
constexpr float EPSILON_SCALE = 0.00001f;
static const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
constexpr const char* RELIABLE_GESTURE_BACK_SURFACE_NAME = "SCBGestureBack";
static std::map<NodeId, uint32_t> cacheRenderNodeMap = {};
static uint32_t cacheReuseTimes = 0;
static std::mutex cacheRenderNodeMapMutex;

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

bool CheckScbReadyToDraw(const std::shared_ptr<RSBaseRenderNode>& child)
{
    if (child != nullptr && child->IsInstanceOf<RSCanvasRenderNode>()) {
        auto canvasRenderNode = child->ReinterpretCastTo<RSCanvasRenderNode>();
        const auto& property = canvasRenderNode->GetRenderProperties();
        if (property.GetFrameWidth() > 0 && property.GetFrameHeight() > 0) {
            return true;
        }
    }
    return false;
}

bool IsFirstFrameReadyToDraw(const RSSurfaceRenderNode& node)
{
    bool result = false;
    auto sortedChildren = node.GetSortedChildren();
    if (node.IsScbScreen() || node.IsSCBNode()) {
        for (const auto& child : *sortedChildren) {
            result = CheckScbReadyToDraw(child);
        }
        return result;
    }
    for (auto& child : *sortedChildren) {
        result = CheckRootNodeReadyToDraw(child);
        // when appWindow has abilityComponent node
        if (child != nullptr && child->IsInstanceOf<RSSurfaceRenderNode>()) {
            for (const auto& surfaceNodeChild : *child->GetSortedChildren()) {
                result = CheckRootNodeReadyToDraw(surfaceNodeChild);
            }
        }
    }
    return result;
}

std::string VisibleDataToString(const VisibleData& val)
{
    std::stringstream ss;
    ss << "VisibleData[name, nodeId, visibleLevel]:";
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (const auto& v : val) {
        auto surfaceNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(v.first);
        auto name = surfaceNode ? surfaceNode->GetName() : "";
        ss << "[" << name << ", " << v.first << ", " << v.second << "], ";
    }
    return ss.str();
}
} // namespace

RSUniRenderVisitor::RSUniRenderVisitor()
    : curSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    PartialRenderOptionInit();
    auto mainThread = RSMainThread::Instance();
    renderEngine_ = mainThread->GetRenderEngine();
    hasMirrorDisplay_ = mainThread->HasMirrorDisplay();
    // when occlusion enabled is false, subTree do not skip, but not influence visible region
    isOcclusionEnabled_ = RSSystemProperties::GetOcclusionEnabled();
    isDrawingCacheEnabled_ = RSSystemParameters::GetDrawingCacheEnabled();
#ifdef DDGR_ENABLE_FEATURE_OPINC
    autoCacheEnable_ = RSSystemProperties::IsDdgrOpincEnable();
#endif
    RSTagTracker::UpdateReleaseResourceEnabled(RSSystemProperties::GetReleaseResourceEnabled());
    isScreenRotationAnimating_ = RSSystemProperties::GetCacheEnabledForRotation();
    isSkipCanvasNodeOutOfScreen_ = RSSystemParameters::GetSkipCanvasNodeOutofScreenEnabled();
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (renderEngine_ && renderEngine_->GetRenderContext()) {
        auto subThreadManager = RSSubThreadManager::Instance();
        subThreadManager->Start(renderEngine_->GetRenderContext().get());
    }
#endif
    isUIFirstDebugEnable_ = RSSystemProperties::GetUIFirstDebugEnabled();
    isPrevalidateHwcNodeEnable_ = RSSystemParameters::GetPrevalidateHwcNodeEnabled() &&
        RSUniHwcPrevalidateUtil::GetInstance().IsLoadSuccess();
}

void RSUniRenderVisitor::PartialRenderOptionInit()
{
    partialRenderType_ = RSSystemProperties::GetUniPartialRenderEnabled();
    isPartialRenderEnabled_ = (partialRenderType_ != PartialRenderType::DISABLED);
    dirtyRegionDebugType_ = RSSystemProperties::GetDirtyRegionDebugType();
    surfaceRegionDebugType_ = RSSystemProperties::GetSurfaceRegionDfxType();
    isTargetDirtyRegionDfxEnabled_ = RSSystemProperties::GetTargetDirtyRegionDfxEnabled(dfxTargetSurfaceNames_) &&
        (surfaceRegionDebugType_ == SurfaceRegionDebugType::DISABLED);
    isRegionDebugEnabled_ = (dirtyRegionDebugType_ != DirtyRegionDebugType::DISABLED) ||
        (surfaceRegionDebugType_ != SurfaceRegionDebugType::DISABLED) || (dfxTargetSurfaceNames_.size() > 0);
    isVisibleRegionDfxEnabled_ = (surfaceRegionDebugType_ == SurfaceRegionDebugType::VISIBLE_REGION);
    isOpaqueRegionDfxEnabled_ = (surfaceRegionDebugType_ == SurfaceRegionDebugType::OPAQUE_REGION);
    isAllSurfaceVisibleDebugEnabled_ = RSSystemProperties::GetAllSurfaceVisibleDebugEnabled();
    isDirtyRegionDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::EGL_DAMAGE);
    isDisplayDirtyDfxEnabled_ = !isTargetDirtyRegionDfxEnabled_ &&
        (dirtyRegionDebugType_ == DirtyRegionDebugType::DISPLAY_DIRTY);
    isCanvasNodeSkipDfxEnabled_ = (dirtyRegionDebugType_ == DirtyRegionDebugType::CANVAS_NODE_SKIP_RECT);
    isOpDropped_ = isPartialRenderEnabled_ &&
        (partialRenderType_ != PartialRenderType::SET_DAMAGE) && !isRegionDebugEnabled_;
    isVirtualDirtyDfxEnabled_ = RSSystemProperties::GetVirtualDirtyDebugEnabled();
    isVirtualDirtyEnabled_ = RSSystemProperties::GetVirtualDirtyEnabled() &&
        (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL);
    isExpandScreenDirtyEnabled_ = RSSystemProperties::GetExpandScreenDirtyEnabled();
}

RSUniRenderVisitor::RSUniRenderVisitor(const RSUniRenderVisitor& visitor) : RSUniRenderVisitor()
{
    currentVisitDisplay_ = visitor.currentVisitDisplay_;
    screenInfo_ = visitor.screenInfo_;
    displayHasSecSurface_ = visitor.displayHasSecSurface_;
    displayHasSkipSurface_ = visitor.displayHasSkipSurface_;
    displayHasProtectedSurface_ = visitor.displayHasProtectedSurface_;
    displaySpecailSurfaceChanged_ = visitor.displaySpecailSurfaceChanged_;
    hasCaptureWindow_ = visitor.hasCaptureWindow_;
    parentSurfaceNodeMatrix_ = visitor.parentSurfaceNodeMatrix_;
    curAlpha_ = visitor.curAlpha_;
    dirtyFlag_ = visitor.dirtyFlag_;
    curDisplayNode_ = visitor.curDisplayNode_;
    currentFocusedNodeId_ = visitor.currentFocusedNodeId_;
    prepareClipRect_ = visitor.prepareClipRect_;
    isOpDropped_ = visitor.isOpDropped_;
    isPartialRenderEnabled_ = visitor.isPartialRenderEnabled_;
    isAllSurfaceVisibleDebugEnabled_ = visitor.isAllSurfaceVisibleDebugEnabled_;
    isHardwareForcedDisabled_ = visitor.isHardwareForcedDisabled_;
    doAnimate_ = visitor.doAnimate_;
    isDirty_ = visitor.isDirty_;
}

RSUniRenderVisitor::~RSUniRenderVisitor() {}

void RSUniRenderVisitor::MergeRemovedChildDirtyRegion(RSRenderNode& node, bool needMap)
{
    if (!node.HasRemovedChild()) {
        return;
    }
    RectI dirtyRect = node.GetChildrenRect();
    auto dirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    if (dirtyManager == nullptr || dirtyRect.IsEmpty()) {
        node.ResetHasRemovedChild();
        return;
    }

    // [planning] merge removed child's rect instead
    if (needMap && curSurfaceNode_) {
        if (auto& geoPtr = node.GetRenderProperties().GetBoundsGeometry()) {
            dirtyRect = geoPtr->MapAbsRect(dirtyRect.ConvertTo<float>());
        }
    } else {
        dirtyRect = prepareClipRect_.IntersectRect(node.GetChildrenRect());
    }
    dirtyManager->MergeDirtyRect(dirtyRect);
    RS_OPTIONAL_TRACE_NAME_FMT("MergeRemovedChildDirtyRegion NodeId:%" PRIu64 ", dirty rect:%s",
        node.GetId(), dirtyRect.ToString().c_str());
    if (dirtyManager->IsTargetForDfx()) {
        // since childRect includes multiple rects, defaultly marked as canvas_node
        dirtyManager->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
            DirtyRegionType::REMOVE_CHILD_RECT, dirtyRect);
    }
    node.ResetHasRemovedChild();
}

void RSUniRenderVisitor::CheckColorSpace(RSSurfaceRenderNode& node)
{
    const sptr<SurfaceBuffer>& buffer = node.GetRSSurfaceHandler()->GetBuffer();
    if (buffer != nullptr) {
        using namespace HDI::Display::Graphic::Common::V1_0;
        CM_ColorSpaceInfo colorSpaceInfo;
        if (MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo) != GSERROR_OK) {
            RS_LOGD("RSUniRenderVisitor::CheckColorSpace: get color space info failed.");
            return;
        }

        if (colorSpaceInfo.primaries != COLORPRIMARIES_SRGB && !IsHardwareComposerEnabled()) {
            newColorSpace_ = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
            RS_LOGD("RSUniRenderVisitor::CheckColorSpace: node(%{public}s) set new colorspace primaries %{public}d",
                    node.GetName().c_str(), colorSpaceInfo.primaries);
        }
        return;
    }

    if (node.IsAppWindow()) {
        auto colorspace = node.GetColorSpace();
        if (colorspace != GRAPHIC_COLOR_GAMUT_SRGB) {
            newColorSpace_ = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
            RS_LOGD("RSUniRenderVisitor::CheckColorSpace: node(%{public}s) set new colorgamut %{public}d",
                    node.GetName().c_str(), colorspace);
        }
    }
}

void RSUniRenderVisitor::HandleColorGamuts(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager)
{
    RSScreenType screenType = BUILT_IN_TYPE_SCREEN;
    if (screenManager->GetScreenType(node.GetScreenId(), screenType) != SUCCESS) {
        RS_LOGD("RSUniRenderVisitor::HandleColorGamuts get screen type failed.");
        return;
    }

    if (screenType == VIRTUAL_TYPE_SCREEN) {
        ScreenColorGamut screenColorGamut;
        if (screenManager->GetScreenColorGamut(node.GetScreenId(), screenColorGamut) != SUCCESS) {
            RS_LOGD("RSUniRenderVisitor::HandleColorGamuts get screen color gamut failed.");
            return;
        }
        newColorSpace_ = static_cast<GraphicColorGamut>(screenColorGamut);
    }

    auto stagingDisplayParams = static_cast<RSDisplayRenderParams*>(node.GetStagingRenderParams().get());
    if (stagingDisplayParams) {
        stagingDisplayParams->SetNewColorSpace(newColorSpace_);
    }
}

void RSUniRenderVisitor::CheckPixelFormat(RSSurfaceRenderNode& node)
{
    if (node.GetHDRPresent()) {
        RS_LOGD("SetHDRPresent true, surfaceNode: %{public}" PRIu64 "", node.GetId());
        hasHdrpresent_ = true;
    }
    if (hasFingerprint_) {
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat hasFingerprint is true.");
        return;
    }
    const sptr<SurfaceBuffer>& buffer = node.GetRSSurfaceHandler()->GetBuffer();
    if (buffer == nullptr) {
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat node(%{public}s) did not have buffer.", node.GetName().c_str());
        return;
    }

    if (node.GetFingerprint()) {
        hasFingerprint_ = true;
        newPixelFormat_ = GRAPHIC_PIXEL_FMT_RGBA_1010102;
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat newPixelFormat_ is set 1010102 for fingerprint.");
        return;
    }

    auto bufferPixelFormat = buffer->GetFormat();
    if ((bufferPixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010102 ||
        bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCBCR_P010 ||
        bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCRCB_P010) && !IsHardwareComposerEnabled()) {
        newPixelFormat_ = GRAPHIC_PIXEL_FMT_RGBA_1010102;
        RS_LOGD("RSUniRenderVisitor::CheckPixelFormat pixelformat is set to 1010102 for 10bit buffer");
    }
}

void RSUniRenderVisitor::HandlePixelFormat(RSDisplayRenderNode& node, const sptr<RSScreenManager>& screenManager)
{
    if (!RSSystemProperties::GetHDRImageEnable()) {
        hasHdrpresent_ = false;
    }
    RS_LOGD("SetHDRPresent: [%{public}d] prepare", hasHdrpresent_);
    auto stagingDisplayParams = static_cast<RSDisplayRenderParams*>(node.GetStagingRenderParams().get());
    if (!stagingDisplayParams) {
        RS_LOGD("RSUniRenderVisitor::HandlePixelFormat get StagingRenderParams failed.");
        return;
    }
    ScreenId screenId = stagingDisplayParams->GetScreenId();
    RSLuminanceControl::Get().SetHdrStatus(screenId, hasHdrpresent_);
    bool isHdrOn = RSLuminanceControl::Get().IsHdrOn(screenId);
    float brightnessRatio = RSLuminanceControl::Get().GetHdrBrightnessRatio(screenId, 0);
    if (!hasHdrpresent_ && !hasUniRenderHdrSurface_) {
        isHdrOn = false;
        node.SetBrightnessRatio(brightnessRatio);
        RS_LOGD("no hdr content in uniRender, brightness ratio: %{public}f handled in composer", brightnessRatio);
    } else {
        // 1.0f means that dss composer don't brightness discount.
        node.SetBrightnessRatio(1.0f);
        RS_LOGD("hdr content in uniRender, brightness ratio: %{public}f handled in uniRender", brightnessRatio);
    }
    node.SetHDRPresent(isHdrOn);
    RSScreenType screenType = BUILT_IN_TYPE_SCREEN;
    if (screenManager->GetScreenType(node.GetScreenId(), screenType) != SUCCESS) {
        RS_LOGD("RSUniRenderVisitor::HandlePixelFormat get screen type failed.");
        return;
    }

    if (screenType == VIRTUAL_TYPE_SCREEN) {
        if (screenManager->GetPixelFormat(node.GetScreenId(), newPixelFormat_) != SUCCESS) {
            RS_LOGD("RSUniRenderVisitor::HandlePixelFormat get screen color gamut failed.");
        }
    }
    stagingDisplayParams->SetNewPixelFormat(newPixelFormat_);
}

void RSUniRenderVisitor::ResetCurSurfaceInfoAsUpperSurfaceParent(RSSurfaceRenderNode& node)
{
    // record current frame mainwindow or leashwindow node
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        curMainAndLeashWindowNodesIds_.push(node.GetId());
        curAllMainAndLeashWindowNodesIds_.emplace_back(node.GetId());
        curDisplayNode_->RecordMainAndLeashSurfaces(node.shared_from_this());
    }
    // only reset for instance node
    if (curSurfaceNode_ == nullptr || curSurfaceNode_->GetId() != node.GetId()) {
        return;
    }
    if (auto directParent = node.GetParent().lock()) {
        if (auto parentInstance = directParent->GetInstanceRootNode()) {
            // in case leashwindow is not directParent
            auto surfaceParent = parentInstance->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (surfaceParent && (surfaceParent->IsLeashWindow() || surfaceParent->IsMainWindowType())) {
                curSurfaceNode_ = surfaceParent;
                curSurfaceDirtyManager_ = surfaceParent->GetDirtyManager();
                filterInGlobal_ = surfaceParent->IsTransparent();
                return;
            }
            curSurfaceNode_ = nullptr;
            curSurfaceDirtyManager_ = nullptr;
            filterInGlobal_ = true;
        }
    }
}

bool RSUniRenderVisitor::IsHardwareComposerEnabled()
{
    return !isHardwareForcedDisabled_;
}

void RSUniRenderVisitor::UpdateSecuritySkipAndProtectedLayersRecord(RSSurfaceRenderNode& node)
{
    if (node.GetHasSecurityLayer()) {
        displayHasSecSurface_[currentVisitDisplay_] = true;
    }
    if (node.GetHasSkipLayer() && node.GetName().find(CAPTURE_WINDOW_NAME) == std::string::npos) {
        displayHasSkipSurface_[currentVisitDisplay_] = true;
    }
    if (node.GetHasProtectedLayer()) {
        displayHasProtectedSurface_[currentVisitDisplay_] = true;
    }
    if (node.IsSpecialLayerChanged()) {
        displaySpecailSurfaceChanged_[currentVisitDisplay_] = true;
    }
}

void RSUniRenderVisitor::UpdateSurfaceRenderNodeRotate(RSSurfaceRenderNode& node)
{
    if (!node.IsMainWindowType()) {
        return;
    }
    if (RSUniRenderUtil::GetRotationDegreeFromMatrix(
        node.GetRenderProperties().GetBoundsGeometry()->GetAbsMatrix()) % RS_ROTATION_90 != 0) {
        node.SetIsRotating(true);
    }
}

bool RSUniRenderVisitor::IsSubTreeOccluded(RSRenderNode& node) const
{
    if (!isOcclusionEnabled_) {
        return false;
    }
    // step1. apply occlusion info for surfacenode and skip fully covered subtree
    if (node.GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(node);
        if (surfaceNode.IsMainWindowType()) {
            RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::IsSubTreeOccluded node[%s]"
                "name:[%s] visibleRegionIsEmpty[%d]",
                std::to_string(node.GetId()).c_str(), surfaceNode.GetName().c_str(),
                surfaceNode.GetVisibleRegion().IsEmpty());
            auto isOccluded = hasMirrorDisplay_ ?
                surfaceNode.GetVisibleRegionInVirtual().IsEmpty() : surfaceNode.GetVisibleRegion().IsEmpty();
            if (isOccluded && curSurfaceDirtyManager_) {
                curSurfaceDirtyManager_->Clear();
            }
            surfaceNode.AccmulateDirtyInOcclusion(isOccluded);
            return isOccluded;
        }
    }
    // step2.1 For partial visible surface, intersected region->rects in surface
    // step2.2 check if clean subtree in occlusion rects
    return false;
}

void RSUniRenderVisitor::ResetDisplayDirtyRegion()
{
    if (!curDisplayDirtyManager_) {
        return;
    }
    bool ret = CheckScreenPowerChange() || CheckColorFilterChange() ||
        CheckCurtainScreenUsingStatusChange() || IsFirstFrameOfPartialRender() ||
        IsWatermarkFlagChanged() || IsDisplayZoomIn();
    if (ret) {
        curDisplayDirtyManager_->ResetDirtyAsSurfaceSize();
        RS_LOGD("RSUniRenderVisitor::ResetDisplayDirtyRegion on");
    }
}

bool RSUniRenderVisitor::CheckScreenPowerChange() const
{
    if (!RSMainThread::Instance()->GetScreenPowerOnChanged()) {
        return false;
    }
    RS_LOGD("RSUniRenderVisitor::CheckScreenPowerChange changed");
    return true;
}

bool RSUniRenderVisitor::CheckCurtainScreenUsingStatusChange() const
{
    if (!RSMainThread::Instance()->IsCurtainScreenUsingStatusChanged()) {
        return false;
    }
    RS_LOGD("RSUniRenderVisitor::CheckCurtainScreenUsingStatusChange changed");
    return true;
}

bool RSUniRenderVisitor::IsFirstFrameOfPartialRender() const
{
    if (!RSMainThread::Instance()->IsFirstFrameOfPartialRender()) {
        return false;
    }
    RS_LOGD("FirstFrameOfPartialRender");
    return true;
}

bool RSUniRenderVisitor::IsWatermarkFlagChanged() const
{
    if (RSMainThread::Instance()->IsWatermarkFlagChanged()) {
        RS_LOGD("FirstOrLastFrameOfWatermark");
        return true;
    } else {
        return false;
    }
}

bool RSUniRenderVisitor::IsDisplayZoomIn() const
{
    if (!curDisplayNode_) {
        return false;
    }
    auto scale = curDisplayNode_->GetRenderProperties().GetScale();
    return scale.x_ > 1.f || scale.y_ > 1.f;
}

void RSUniRenderVisitor::QuickPrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    // 0. init display info
    RS_TRACE_NAME("RSUniRender:QuickPrepareDisplayRenderNode " + std::to_string(node.GetScreenId()));
    if (!InitDisplayInfo(node)) {
        return;
    }
    SendRcdMessage(node);
    ancestorNodeHasAnimation_ = false;
    displayNodeRotationChanged_ = node.IsRotationChanged();
    dirtyFlag_ = isDirty_ || displayNodeRotationChanged_;
    prepareClipRect_ = screenRect_;
    hasAccumulatedClip_ = false;

    curAlpha_ = 1.0f;
    globalZOrder_ = 0.0f;
    hasSkipLayer_ = false;
    node.UpdateRotation();
    if (!(RSMainThread::Instance()->IsRequestedNextVSync() || RSMainThread::Instance()->GetNextDVsyncAnimateFlag())) {
        RS_OPTIONAL_TRACE_NAME_FMT("do not request next vsync");
        needRequestNextVsync_ = false;
    }
    RSUifirstManager::Instance().SetRotationChanged(displayNodeRotationChanged_ || isScreenRotationAnimating_);
    if (node.IsSubTreeDirty() || node.IsRotationChanged()) {
        QuickPrepareChildren(node);
    }
    PostPrepare(node);
    UpdateHwcNodeEnable();
    UpdateSurfaceDirtyAndGlobalDirty();
    UpdateSurfaceOcclusionInfo();
    if (needRecalculateOcclusion_) {
        // Callback for registered self drawing surfacenode
        RSMainThread::Instance()->SurfaceOcclusionCallback();
    }
    //UIFirst layer must be above displayNode, so use zorder + 1
    RSUifirstManager::Instance().UpdateUIFirstLayerInfo(screenInfo_, globalZOrder_ + 1);
    curDisplayNode_->UpdatePartialRenderParams();
    RSDisplayRenderNode::ScreenRenderParams screenRenderParams;
    screenRenderParams.screenInfo = std::move(screenInfo_);
    screenRenderParams.displayHasSecSurface = std::move(displayHasSecSurface_);
    screenRenderParams.displayHasSkipSurface = std::move(displayHasSkipSurface_);
    screenRenderParams.displayHasProtectedSurface = std::move(displayHasProtectedSurface_);
    screenRenderParams.displaySpecailSurfaceChanged = std::move(displaySpecailSurfaceChanged_);
    screenRenderParams.hasCaptureWindow = std::move(hasCaptureWindow_);
    curDisplayNode_->UpdateScreenRenderParams(screenRenderParams);
    curDisplayNode_->UpdateOffscreenRenderParams(curDisplayNode_->IsRotationChanged());
    HandleColorGamuts(node, screenManager_);
    HandlePixelFormat(node, screenManager_);
    if (UNLIKELY(!SharedTransitionParam::unpairedShareTransitions_.empty())) {
        ProcessUnpairedSharedTransitionNode();
    }
    node.RenderTraceDebug();
}

void RSUniRenderVisitor::CheckFilterCacheNeedForceClearOrSave(RSRenderNode& node)
{
    if (!node.HasBlurFilter()) {
        return;
    }
    bool rotationChanged = curDisplayNode_ ?
        curDisplayNode_->IsRotationChanged() || curDisplayNode_->IsLastRotationChanged() : false;
    bool hdrChanged = curDisplayNode_ ?
        RSLuminanceControl::Get().IsDimmingOn(curDisplayNode_->GetScreenId()) : false;
    bool rotationStatusChanged = curDisplayNode_ ?
        curDisplayNode_->GetPreRotationStatus() != curDisplayNode_->GetCurRotationStatus() : false;
    node.CheckBlurFilterCacheNeedForceClearOrSave(rotationChanged || hdrChanged, rotationStatusChanged);
}

void RSUniRenderVisitor::QuickPrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRender::QuickPrepare:[%s] nodeId[%" PRIu64 "]"
        "pid[%d] nodeType[%u] subTreeDirty[%d]", node.GetName().c_str(), node.GetId(), ExtractPid(node.GetId()),
        static_cast<uint>(node.GetSurfaceNodeType()), node.IsSubTreeDirty());
    RS_LOGD("RSUniRender::QuickPrepareSurfaceRenderNode:[%{public}s] nodeid:[%{public}" PRIu64 "]"
        "pid:[%{public}d] nodeType:[%{public}d] subTreeDirty[%{public}d]", node.GetName().c_str(), node.GetId(),
        ExtractPid(node.GetId()), static_cast<int>(node.GetSurfaceNodeType()), node.IsSubTreeDirty());

    // 0. init curSurface info and check node info
    auto curCornerRadius = curCornerRadius_;
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    if (!BeforeUpdateSurfaceDirtyCalc(node)) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode BeforeUpdateSurfaceDirtyCalc fail");
        RSUifirstManager::Instance().DisableUifirstNode(node);
        node.OpincSetInAppStateEnd(unchangeMarkInApp_);
        return;
    }

    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode %{public}s curSurfaceDirtyManager is nullptr",
            node.GetName().c_str());
        return;
    }

    // 1. Update matrix and collect dirty region
    auto dirtyFlag = dirtyFlag_;
    auto prepareClipRect = prepareClipRect_;
    bool hasAccumulatedClip = hasAccumulatedClip_;
    auto prevAlpha = curAlpha_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);
    node.SetGlobalAlpha(curAlpha_);
    CheckFilterCacheNeedForceClearOrSave(node);
    node.CheckContainerDirtyStatusAndUpdateDirty(curContainerDirty_);
    dirtyFlag_ = node.UpdateDrawRectAndDirtyRegion(
        *curSurfaceDirtyManager_, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix);
    parentSurfaceNodeMatrix_ = node.GetRenderProperties().GetBoundsGeometry()->GetAbsMatrix();
    if (!AfterUpdateSurfaceDirtyCalc(node)) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareSurfaceRenderNode AfterUpdateSurfaceDirtyCalc fail");
        RSUifirstManager::Instance().DisableUifirstNode(node);
        node.OpincSetInAppStateEnd(unchangeMarkInApp_);
        return;
    }
    hasAccumulatedClip_ = node.SetAccumulatedClipFlag(hasAccumulatedClip_);
    bool isSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_, IsSubTreeOccluded(node)) ||
        ForcePrepareSubTree();
    isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*curSurfaceDirtyManager_, curDirty_, dirtyFlag_, prepareClipRect_);

    // Update whether leash window's visible region is empty after prepare children
    UpdateLeashWindowVisibleRegionEmpty(node);

    if (node.IsLeashWindow() && RSSystemProperties::GetGpuOverDrawBufferOptimizeEnabled()) {
        CheckIsGpuOverDrawBufferOptimizeNode(node);
    }
    PostPrepare(node, !isSubTreeNeedPrepare);
    curAlpha_ = prevAlpha;
    prepareClipRect_ = prepareClipRect;
    hasAccumulatedClip_ = hasAccumulatedClip;
    dirtyFlag_ = dirtyFlag;

    PrepareForUIFirstNode(node);
    PrepareForCapsuleWindowNode(node);
    node.OpincSetInAppStateEnd(unchangeMarkInApp_);
    ResetCurSurfaceInfoAsUpperSurfaceParent(node);
    curCornerRadius_ = curCornerRadius;
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    node.RenderTraceDebug();
    node.SetNeedOffscreen(isScreenRotationAnimating_);
}

void RSUniRenderVisitor::PrepareForCapsuleWindowNode(RSSurfaceRenderNode& node)
{
    if (UNLIKELY(!curDisplayNode_)) {
        RS_LOGE("RSUniRenderVisitor::PrepareForCapsuleWindowNode curDisplayNode is nullptr");
        return;
    }
    if (node.GetName().find("CapsuleWindow") == std::string::npos) {
        return;
    }
    node.SetRootIdOfCaptureWindow(FindInstanceChildOfDisplay(node.GetParent().lock()));
}

void RSUniRenderVisitor::PrepareForUIFirstNode(RSSurfaceRenderNode& node)
{
    MultiThreadCacheType lastFlag = node.GetLastFrameUifirstFlag();
    RSUifirstManager::Instance().UpdateUifirstNodes(node, ancestorNodeHasAnimation_ || node.GetCurFrameHasAnimation());
    RSUifirstManager::Instance().UpdateUIFirstNodeUseDma(node, globalSurfaceBounds_);
    if (node.GetLastFrameUifirstFlag() == MultiThreadCacheType::LEASH_WINDOW &&
        !node.IsHardwareForcedDisabled()) {
        auto geo = node.GetRenderProperties().GetBoundsGeometry();
        UpdateSrcRect(node, geo->GetAbsMatrix(), geo->GetAbsRect());
        UpdateHwcNodeByTransform(node);
    }
    if (RSUifirstManager::Instance().GetUseDmaBuffer(node.GetName()) &&
        (node.GetLastFrameUifirstFlag() != lastFlag ||
        !node.IsHardwareForcedDisabled() != node.GetIsLastFrameHwcEnabled())) {
        curSurfaceDirtyManager_->MergeDirtyRect(node.GetOldDirty());
    }
}

void RSUniRenderVisitor::UpdateNodeVisibleRegion(RSSurfaceRenderNode& node)
{
    Occlusion::Rect selfDrawRect = node.GetSurfaceOcclusionRect(true);
    Occlusion::Region selfDrawRegion { selfDrawRect };
    if (needRecalculateOcclusion_) {
        Occlusion::Region subResult = selfDrawRegion.Sub(accumulatedOcclusionRegion_);
        node.SetVisibleRegion(subResult);
        Occlusion::Region subResultWithoutSkipLayer = selfDrawRegion.Sub(occlusionRegionWithoutSkipLayer_);
        node.SetVisibleRegionInVirtual(subResultWithoutSkipLayer);
    }
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_THREE,
        "RSUniRenderVisitor::UpdateNodeVisibleRegion name[%s] visibleRegion[%s]",
        node.GetName().c_str(), node.GetVisibleRegion().GetRegionInfo().c_str());
}

void RSUniRenderVisitor::CalculateOcclusion(RSSurfaceRenderNode& node)
{
    if (!curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::CalculateOcclusion curDisplayNode is nullptr");
        return;
    }
    // CheckAndUpdateOpaqueRegion only in mainWindow
    auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.GetParent().lock());
    auto isFocused = node.IsFocusedNode(currentFocusedNodeId_) ||
        (parent && parent->IsLeashWindow() && parent->IsFocusedNode(focusedLeashWindowId_));
    node.CheckAndUpdateOpaqueRegion(screenRect_, curDisplayNode_->GetRotation(), isFocused);
    if (!needRecalculateOcclusion_) {
        needRecalculateOcclusion_ = node.CheckIfOcclusionChanged();
    }
    // Update node visibleRegion
    hasSkipLayer_ = hasSkipLayer_ || node.GetSkipLayer();
    UpdateNodeVisibleRegion(node);
    auto mainThread = RSMainThread::Instance();
    node.SetOcclusionInSpecificScenes(mainThread->GetDeviceType() == DeviceType::PC &&
        mainThread->IsPCThreeFingerScenesListScene());
    // check current surface Participate In Occlusion
    if (node.CheckParticipateInOcclusion() && !isAllSurfaceVisibleDebugEnabled_) {
        accumulatedOcclusionRegion_.OrSelf(node.GetOpaqueRegion());
        std::unordered_set<NodeId> currentBlackList = GetCurrentBlackList();
        if (IsValidInVirtualScreen(node) && currentBlackList.find(node.GetId()) == currentBlackList.end()) {
            occlusionRegionWithoutSkipLayer_.OrSelf(node.GetOpaqueRegion());
        }
    }
    node.SetOcclusionInSpecificScenes(false);
    CollectOcclusionInfoForWMS(node);
}

void RSUniRenderVisitor::CollectOcclusionInfoForWMS(RSSurfaceRenderNode& node)
{
    if (!node.IsMainWindowType()) {
        return;
    }
    // collect mainWindow occlusion visibleLevel
    Occlusion::Region selfDrawRegion { node.GetSurfaceOcclusionRect(true) };
    auto visibleLevel = GetRegionVisibleLevel(node.GetVisibleRegion(), selfDrawRegion);
    // collect surface node visibleLevel for dynamic Vsync Rate.
    CollectVSyncRate(node, visibleLevel);

    // wms default all visible about sefdrawing node and AbilityComponent node
    auto instanceNode = node.GetInstanceRootNode() ?
        node.GetInstanceRootNode()->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
    if (instanceNode == nullptr) {
        return;
    }
    if ((node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE &&
        !instanceNode->GetVisibleRegion().IsEmpty()) || node.IsAbilityComponent()) {
        dstCurVisVec_.emplace_back(std::make_pair(node.GetId(),
            WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE));
        return;
    }
    dstCurVisVec_.emplace_back(std::make_pair(node.GetId(),
        node.GetVisibleLevelForWMS(visibleLevel)));
}

void RSUniRenderVisitor::CollectVSyncRate(RSSurfaceRenderNode& node, RSVisibleLevel visibleLevel)
{
    if (!node.GetQosCal()) {
        return;
    }
    auto nodeId = node.GetId();
    if (RSMainThread::Instance()->IsSystemAnimatedScenesListEmpty()) {
        RSVisibleLevel levelForVsync = node.IsSCBNode() ? visibleLevel : RSVisibleLevel::RS_ALL_VISIBLE;
        visMapForVSyncRate_[nodeId] = levelForVsync;
        RS_OPTIONAL_TRACE_NAME_FMT("CollectVSyncRate name=%s id=%" PRIu64 " visLevel=%d", node.GetName().c_str(),
            nodeId, levelForVsync);
        RS_LOGD("CollectVSyncRate name=%{public}s id=%{public}" PRIu64 " visLevel=%{public}d",
            node.GetName().c_str(), nodeId, levelForVsync);
    } else {
        if (!(node.GetDstRect().IsEmpty() || node.IsLeashWindow())) {
            RSVisibleLevel levelForVsync = RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE;
            visMapForVSyncRate_[nodeId] = levelForVsync;
            RS_OPTIONAL_TRACE_NAME_FMT("CollectVSyncRate name=%s id=%" PRIu64 " visLevel=%d", node.GetName().c_str(),
                nodeId, levelForVsync);
            RS_LOGD("CollectVSyncRate name=%{public}s id=%{public}" PRIu64 " visLevel=%{public}d",
                node.GetName().c_str(), nodeId, levelForVsync);
        }
    }
}

void RSUniRenderVisitor::SurfaceOcclusionCallbackToWMS()
{
    if (RSSystemParameters::GetOcclusionCallBackToWMSDebugType()) {
        allDstCurVisVec_.clear();
        const auto& curAllSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
        for (const auto& surfacePtr : curAllSurfaces) {
            if (surfacePtr == nullptr) {
                continue;
            }
            const auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(*surfacePtr);
            if (surfaceNode.IsMainWindowType()) {
                allDstCurVisVec_.emplace_back(std::make_pair(surfacePtr->GetId(),
                    WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE));
            }
        }
    }
    if (allDstCurVisVec_ != allLastVisVec_) {
        RSMainThread::Instance()->SurfaceOcclusionChangeCallback(allDstCurVisVec_);
        RS_LOGI("RSUniRenderVisitor::SurfaceOcclusionCallbackToWMS %{public}s",
            VisibleDataToString(allDstCurVisVec_).c_str());
        allLastVisVec_ = std::move(allDstCurVisVec_);
        vSyncRatesChanged_ = true;
    }
}

std::unordered_set<NodeId> RSUniRenderVisitor::GetCurrentBlackList() const
{
    if (!screenManager_ || !curDisplayNode_) {
        return std::unordered_set<NodeId>();
    }

    std::unordered_set<NodeId> currentBlackList;
    if (screenManager_->GetCastScreenEnableSkipWindow(curDisplayNode_->GetScreenId())) {
        screenManager_->GetCastScreenBlackList(currentBlackList);
    } else {
        currentBlackList = screenManager_->GetVirtualScreenBlackList(curDisplayNode_->GetScreenId());
    }
    return currentBlackList;
}

RSVisibleLevel RSUniRenderVisitor::GetRegionVisibleLevel(const Occlusion::Region& visibleRegion,
    const Occlusion::Region& selfDrawRegion)
{
    if (visibleRegion.IsEmpty()) {
        return RSVisibleLevel::RS_INVISIBLE;
    } else if (visibleRegion.Area() == selfDrawRegion.Area()) {
        return RSVisibleLevel::RS_ALL_VISIBLE;
    } else if (static_cast<uint>(visibleRegion.Area()) <
        (static_cast<uint>(selfDrawRegion.Area()) >> VISIBLEAREARATIO_FORQOS)) {
        return RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE;
    }
    return RSVisibleLevel::RS_SEMI_NONDEFAULT_VISIBLE;
}

void RSUniRenderVisitor::QuickPrepareEffectRenderNode(RSEffectRenderNode& node)
{
    // 0. check current node need to tranverse
    auto dirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    if (!dirtyManager) {
        RS_LOGE("RSUniRenderVisitor::QuickPrepareEffectRenderNode dirtyManager is nullptr");
        return;
    }
    auto dirtyFlag = dirtyFlag_;
    auto prevAlpha = curAlpha_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);
    UpdateRotationStatusForEffectNode(node);
    CheckFilterCacheNeedForceClearOrSave(node);
    RectI prepareClipRect = prepareClipRect_;
    bool hasAccumulatedClip = hasAccumulatedClip_;
    dirtyFlag_ =
        node.UpdateDrawRectAndDirtyRegion(*dirtyManager, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix_);

    // 1. Recursively traverse child nodes
    hasAccumulatedClip_ = node.SetAccumulatedClipFlag(hasAccumulatedClip_);
    bool isSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_) || ForcePrepareSubTree();
    isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*dirtyManager, curDirty_, dirtyFlag_, prepareClipRect_);

    PostPrepare(node, !isSubTreeNeedPrepare);
    prepareClipRect_ = prepareClipRect;
    hasAccumulatedClip_ = hasAccumulatedClip;
    dirtyFlag_ = dirtyFlag;
    curAlpha_ = prevAlpha;
    node.RenderTraceDebug();
}

void RSUniRenderVisitor::QuickPrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    // 0. check current node need to traverse
    auto dirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    auto dirtyFlag = dirtyFlag_;
    auto prevAlpha = curAlpha_;
    auto curCornerRadius = curCornerRadius_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);
    CheckFilterCacheNeedForceClearOrSave(node);
    if (isDrawingCacheEnabled_) {
        node.UpdateDrawingCacheInfoBeforeChildren(isScreenRotationAnimating_);
    }
    node.OpincQuickMarkStableNode(unchangeMarkInApp_, unchangeMarkEnable_);

    RectI prepareClipRect = prepareClipRect_;
    bool hasAccumulatedClip = hasAccumulatedClip_;

    if (!dirtyManager) {
        return;
    }
    dirtyFlag_ =
        node.UpdateDrawRectAndDirtyRegion(*dirtyManager, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix_);
    // update prepare clip before children
    UpdatePrepareClip(node);
    node.UpdateCurCornerRadius(curCornerRadius_, curSurfaceNode_ != nullptr);

    // 1. Recursively traverse child nodes if above curSurfaceNode and subnode need draw
    hasAccumulatedClip_ = node.SetAccumulatedClipFlag(hasAccumulatedClip_);
    bool isSubTreeNeedPrepare = !curSurfaceNode_ || node.IsSubTreeNeedPrepare(filterInGlobal_) ||
        ForcePrepareSubTree() || node.OpincForcePrepareSubTree();
    isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*dirtyManager, curDirty_, dirtyFlag_, prepareClipRect_);

    PostPrepare(node, !isSubTreeNeedPrepare);
    prepareClipRect_ = prepareClipRect;
    hasAccumulatedClip_ = hasAccumulatedClip;
    dirtyFlag_ = dirtyFlag;
    curAlpha_ = prevAlpha;
    curCornerRadius_ = curCornerRadius;
    node.OpincUpdateRootFlag(unchangeMarkEnable_);
    node.RenderTraceDebug();
}

void RSUniRenderVisitor::UpdateRotationStatusForEffectNode(RSEffectRenderNode& node)
{
     // folding/expanding screen force invalidate cache.
    node.SetFoldStatusChanged(doAnimate_ &&
        curDisplayNode_->GetScreenId() != node.GetCurrentAttachedScreenId());
    node.SetCurrentAttachedScreenId(curDisplayNode_->GetScreenId());
    node.SetRotationChanged(curDisplayNode_->IsRotationChanged());
}

void RSUniRenderVisitor::UpdatePrepareClip(RSRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto& geoPtr = property.GetBoundsGeometry();
    if (geoPtr == nullptr) {
        return;
    }
    // Dirty Region use abstract coordinate, property of node use relative coordinate
    // BoundsRect(if exists) is mapped to absRect_ of RSObjAbsGeometry.
    if (property.GetClipToBounds()) {
        prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->GetAbsRect());
    }
    // FrameRect(if exists) is mapped to rect using abstract coordinate explicitly by calling MapAbsRect.
    if (property.GetClipToFrame()) {
        // MapAbsRect do not handle the translation of OffsetX and OffsetY
        RectF frameRect{
            property.GetFrameOffsetX() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_X),
            property.GetFrameOffsetY() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_Y),
            property.GetFrameWidth(), property.GetFrameHeight()};
        prepareClipRect_ = prepareClipRect_.IntersectRect(geoPtr->MapAbsRect(frameRect));
    }
}

bool RSUniRenderVisitor::IsLeashAndHasMainSubNode(RSRenderNode& node) const
{
    if (node.GetType() != RSRenderNodeType::SURFACE_NODE) {
        return false;
    }
    const auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(node);
    if (!surfaceNode.IsLeashWindow()) {
        return false;
    }
    // check leashWindow surface has first level mainwindow node
    auto children = node.GetSortedChildren();
    auto iter = std::find_if((*children).begin(), (*children).end(),
        [](const std::shared_ptr<RSRenderNode>& node) {
        if (node->GetType() == RSRenderNodeType::SURFACE_NODE) {
            const auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(*node);
            return surfaceNode.IsMainWindowType();
        }
        return false;
    });
    return iter != (*children).end();
}

bool RSUniRenderVisitor::NeedPrepareChindrenInReverseOrder(RSRenderNode& node) const
{
    if (!curSurfaceNode_ && node.GetType() != RSRenderNodeType::RS_NODE) {
        return true;
    }
    return IsLeashAndHasMainSubNode(node);
}

void RSUniRenderVisitor::QuickPrepareChildren(RSRenderNode& node)
{
    MergeRemovedChildDirtyRegion(node, true);
    if (node.LastFrameSubTreeSkipped() && curSurfaceDirtyManager_) {
        node.ForceMergeSubTreeDirtyRegion(*curSurfaceDirtyManager_, prepareClipRect_);
    }
    bool animationBackup = ancestorNodeHasAnimation_;
    ancestorNodeHasAnimation_ = ancestorNodeHasAnimation_ || node.GetCurFrameHasAnimation();
    node.ResetChildRelevantFlags();
    node.ResetChildUifirstSupportFlag();
    auto children = node.GetSortedChildren();
    if (NeedPrepareChindrenInReverseOrder(node)) {
        std::for_each((*children).rbegin(), (*children).rend(), [this](const std::shared_ptr<RSRenderNode>& node) {
            auto containerDirty = curContainerDirty_;
            curDirty_ = node->IsDirty();
            curContainerDirty_ = curContainerDirty_ || node->IsDirty();
            node->QuickPrepare(shared_from_this());
            curContainerDirty_ = containerDirty;
        });
    } else {
        std::for_each((*children).begin(), (*children).end(), [this](const std::shared_ptr<RSRenderNode>& node) {
            curDirty_ = node->IsDirty();
            node->QuickPrepare(shared_from_this());
        });
    }
    ancestorNodeHasAnimation_ = animationBackup;
    node.ResetGeoUpdateDelay();
}

bool RSUniRenderVisitor::InitDisplayInfo(RSDisplayRenderNode& node)
{
    // 1 init curDisplay and curDisplayDirtyManager
    currentVisitDisplay_ = node.GetScreenId();
    displayHasSecSurface_.emplace(currentVisitDisplay_, false);
    displayHasSkipSurface_.emplace(currentVisitDisplay_, false);
    displayHasProtectedSurface_.emplace(currentVisitDisplay_, false);
    displaySpecailSurfaceChanged_.emplace(currentVisitDisplay_, false);
    hasCaptureWindow_.emplace(currentVisitDisplay_, false);
    curDisplayDirtyManager_ = node.GetDirtyManager();
    curDisplayNode_ = node.shared_from_this()->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!curDisplayDirtyManager_ || !curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::InitDisplayInfo dirtyMgr or node ptr is nullptr");
        return false;
    }
    curDisplayDirtyManager_->Clear();
    transparentCleanFilter_.clear();
    transparentDirtyFilter_.clear();

    // 2 init screenManager info
    screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        RS_LOGE("RSUniRenderVisitor::InitDisplayInfo screenManager_ is nullptr");
        return false;
    }
    screenInfo_ = screenManager_->QueryScreenInfo(node.GetScreenId());
    curDisplayDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
    screenRect_ = RectI{0, 0, screenInfo_.width, screenInfo_.height};

    // 3 init Occlusion info
    needRecalculateOcclusion_ = false;
    accumulatedOcclusionRegion_.Reset();
    occlusionRegionWithoutSkipLayer_.Reset();
    if (!curMainAndLeashWindowNodesIds_.empty()) {
        std::queue<NodeId>().swap(curMainAndLeashWindowNodesIds_);
    }

    // 4. check isHardwareForcedDisabled
    auto& geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGE("RSUniRenderVisitor::InitDisplayInfo geoPtr is nullptr");
        return false;
    }
    if (geoPtr->IsNeedClientCompose()) {
        isHardwareForcedDisabled_ = true;
    }

    // 5. check compositeType
    auto mirrorNode = node.GetMirrorSource().lock();
    switch (screenInfo_.state) {
        case ScreenState::SOFTWARE_OUTPUT_ENABLE:
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
            return false;
    }

    return true;
}

bool RSUniRenderVisitor::BeforeUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node)
{
    // 1. init and record surface info
    if (node.GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos) {
        hasCaptureWindow_[currentVisitDisplay_] = true;
    }
    UpdateSecuritySkipAndProtectedLayersRecord(node);
    node.UpdateUIFirstFrameGravity();
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        // UpdateCurCornerRadius must process before curSurfaceNode_ update
        node.UpdateCurCornerRadius(curCornerRadius_, curSurfaceNode_ != nullptr);
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        curSurfaceDirtyManager_ = node.GetDirtyManager();
        if (!curSurfaceDirtyManager_ || !curSurfaceNode_) {
            RS_LOGE("RSUniRenderVisitor::BeforeUpdateSurfaceDirtyCalc %{public}s has invalid"
                " SurfaceDirtyManager or node ptr", node.GetName().c_str());
            return false;
        }
        curSurfaceDirtyManager_->Clear();
        curSurfaceDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
        filterInGlobal_ = curSurfaceNode_->IsTransparent();
        // update surfaceNode contentDirty and subTreeDirty flag for UIFirst purging policy
        RSMainThread::Instance()->CheckAndUpdateInstanceContentStaticStatus(curSurfaceNode_);
        curSurfaceNode_->UpdateSurfaceCacheContentStaticFlag();
        curSurfaceNode_->UpdateSurfaceSubTreeDirtyFlag();
        curSurfaceNode_->SetLeashWindowVisibleRegionEmpty(false);
    }
    // 2. update surface info and CheckIfOcclusionReusable
    node.SetAncestorDisplayNode(curDisplayNode_); // set for boot animation
    node.UpdateAncestorDisplayNodeInRenderParams();
    node.CleanDstRectChanged();
    // [planning] check node isDirty can be optimized.
    needRecalculateOcclusion_ = needRecalculateOcclusion_ || node.IsDirty() ||
        node.CheckIfOcclusionReusable(preMainAndLeashWindowNodesIds_);
    if (autoCacheEnable_ && node.IsAppWindow()) {
        node.OpincSetInAppStateStart(unchangeMarkInApp_);
    }
    // 3. check color space pixelFormat and update RelMatrix
    CheckColorSpace(node);
    CheckPixelFormat(node);
    if (node.GetRSSurfaceHandler()->GetBuffer() != nullptr) {
        node.SetBufferRelMatrix(RSUniRenderUtil::GetMatrixOfBufferToRelRect(node));
    }
    node.setQosCal((RSMainThread::Instance()->GetDeviceType() == DeviceType::PC) &&
        RSSystemParameters::GetVSyncControlEnabled());
    return true;
}

bool RSUniRenderVisitor::AfterUpdateSurfaceDirtyCalc(RSSurfaceRenderNode& node)
{
    // 1. Update surfaceNode info and AppWindow gravity
    const auto& property = node.GetRenderProperties();
    if (node.IsAppWindow()) {
        boundsRect_ = Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        frameGravity_ = property.GetFrameGravity();
    }
    auto& geoPtr = property.GetBoundsGeometry();
    if (geoPtr == nullptr) {
        return false;
    }
    UpdateDstRect(node, geoPtr->GetAbsRect(), prepareClipRect_);
    node.UpdatePositionZ();
    if (node.IsHardwareEnabledType() && node.GetZorderChanged() && curSurfaceNode_) {
        curSurfaceNode_->SetNeedCollectHwcNode(true);
    }
    UpdateSurfaceRenderNodeScale(node);
    UpdateSurfaceRenderNodeRotate(node);
    if (node.IsMainWindowType() || node.IsLeashWindow()) {
        curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), node.GetOldDirtyInSurface());
        curDisplayNode_->AddSurfaceNodePosByDescZOrder(node.GetId(), node.GetOldDirtyInSurface());
    }
    // 2. Update Occlusion info before children preparation
    if (node.IsMainWindowType()) {
        node.SetGlobalCornerRadius(curCornerRadius_);
        CalculateOcclusion(node);
        if (node.GetFirstLevelNodeId() == node.GetId()) {
            globalSurfaceBounds_.emplace_back(node.GetAbsDrawRect());
        }
    }
    // 3. Update HwcNode Info for appNode
    UpdateHwcNodeInfoForAppNode(node);
    return true;
}

void RSUniRenderVisitor::UpdateLeashWindowVisibleRegionEmpty(RSSurfaceRenderNode& node)
{
    if (!node.IsLeashWindow()) {
        return;
    }
    bool isVisibleRegionEmpty = true;
    for (const auto& child : *node.GetSortedChildren()) {
        const auto childSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
        if (childSurfaceNode && childSurfaceNode->IsAppWindow()) {
            // leash window is visible when child app has visible region
            if (!childSurfaceNode->GetVisibleRegion().IsEmpty()) {
                isVisibleRegionEmpty = false;
            }
            RS_OPTIONAL_TRACE_NAME_FMT("%s's visible region is empty", childSurfaceNode->GetName().c_str());
        } else {
            // leash window is visible when child is not app window node
            isVisibleRegionEmpty = false;
        }
        if (!isVisibleRegionEmpty) {
            break;
        }
    }
    node.SetLeashWindowVisibleRegionEmpty(isVisibleRegionEmpty);
}

void RSUniRenderVisitor::UpdateHwcNodeInfoForAppNode(RSSurfaceRenderNode& node)
{
    // app node
    if (node.GetNeedCollectHwcNode()) {
        node.ResetChildHardwareEnabledNodes();
    }
    // hwc node
    if (node.IsHardwareEnabledType() && curSurfaceNode_) {
        if (curSurfaceNode_->GetNeedCollectHwcNode()) {
            curSurfaceNode_->AddChildHardwareEnabledNode(node.ReinterpretCastTo<RSSurfaceRenderNode>());
        }
        if (!node.GetHardWareDisabledByReverse()) {
            node.SetHardwareForcedDisabledState(false);
        }
        node.SetHardwareForcedDisabledByVisibility(false);
        node.SetForceHardware(displayNodeRotationChanged_ || isScreenRotationAnimating_);
        if ((!node.GetForceHardware() && !IsHardwareComposerEnabled()) ||
            curSurfaceNode_->GetVisibleRegion().IsEmpty() || !node.GetRSSurfaceHandler()->GetBuffer()) {
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disabled by param/invisible/no buffer",
                node.GetName().c_str(), node.GetId());
            node.SetHardwareForcedDisabledByVisibility(true);
            hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(node.GetId(),
                HwcDisabledReasons::DISABLED_BY_INVALID_PARAM, node.GetName());
            return;
        }
        auto geo = node.GetRenderProperties().GetBoundsGeometry();
        UpdateSrcRect(node, geo->GetAbsMatrix(), geo->GetAbsRect());
        UpdateHwcNodeByTransform(node);
        UpdateHwcNodeEnableByBackgroundAlpha(node);
        UpdateHwcNodeEnableByBufferSize(node);
        UpdateHwcNodeEnableBySrcRect(node);
    }
}

void RSUniRenderVisitor::UpdateSrcRect(RSSurfaceRenderNode& node,
    const Drawing::Matrix& absMatrix, const RectI& absRect)
{
    auto canvas = std::make_unique<Rosen::Drawing::Canvas>(screenInfo_.phyWidth, screenInfo_.phyHeight);
    canvas->ConcatMatrix(absMatrix);

    const auto& dstRect = node.GetDstRect();
    Drawing::RectI dst = { std::round(dstRect.GetLeft()), std::round(dstRect.GetTop()), std::round(dstRect.GetRight()),
                           std::round(dstRect.GetBottom()) };
    node.UpdateSrcRect(*canvas.get(), dst);
    if (node.GetRSSurfaceHandler()->GetBuffer()) {
        RSUniRenderUtil::UpdateRealSrcRect(node, absRect);
    }
}

void RSUniRenderVisitor::UpdateDstRect(RSSurfaceRenderNode& node, const RectI& absRect, const RectI& clipRect)
{
    auto dstRect = absRect;
    // If the screen is expanded, intersect the destination rectangle with the screen rectangle
    dstRect = dstRect.IntersectRect(RectI(curDisplayNode_->GetDisplayOffsetX(), curDisplayNode_->GetDisplayOffsetY(),
        screenInfo_.width, screenInfo_.height));
    // Remove the offset of the screen
    dstRect.left_ = dstRect.left_ - curDisplayNode_->GetDisplayOffsetX();
    dstRect.top_ = dstRect.top_ - curDisplayNode_->GetDisplayOffsetY();
    // If the node is a hardware-enabled type, intersect its destination rectangle with the prepare clip rectangle
    if (node.IsHardwareEnabledType()) {
        dstRect = dstRect.IntersectRect(clipRect);
    }
    dstRect.left_ = static_cast<int>(std::round(dstRect.left_ * screenInfo_.GetRogWidthRatio()));
    dstRect.top_ = static_cast<int>(std::round(dstRect.top_ * screenInfo_.GetRogHeightRatio()));
    dstRect.width_ = static_cast<int>(std::round(dstRect.width_ * screenInfo_.GetRogWidthRatio()));
    dstRect.height_ = static_cast<int>(std::round(dstRect.height_ * screenInfo_.GetRogHeightRatio()));
    // Set the destination rectangle of the node
    node.SetDstRect(dstRect);
}

void RSUniRenderVisitor::UpdateHwcNodeByTransform(RSSurfaceRenderNode& node)
{
    if (!node.GetRSSurfaceHandler() || !node.GetRSSurfaceHandler()->GetBuffer()) {
        return;
    }
    node.SetForceHardware(displayNodeRotationChanged_ || isScreenRotationAnimating_);
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo_);
    RSUniRenderUtil::LayerRotate(node, screenInfo_);
    RSUniRenderUtil::LayerCrop(node, screenInfo_);
    const auto nodeParams = static_cast<RSSurfaceRenderParams*>(node.GetStagingRenderParams().get());
    ScalingMode scalingMode = nodeParams->GetPreScalingMode();
    const auto& buffer = node.GetRSSurfaceHandler()->GetBuffer();
    const auto& surface = node.GetRSSurfaceHandler()->GetConsumer();
    if (surface == nullptr) {
        RS_LOGE("surface is nullptr");
        return;
    }

    if (surface->GetScalingMode(buffer->GetSeqNum(), scalingMode) == GSERROR_OK) {
        nodeParams->SetPreScalingMode(scalingMode);
    }
    if (scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        RSUniRenderUtil::LayerScaleDown(node);
    } else if (scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT) {
        RSUniRenderUtil::LayerScaleFit(node);
    }
    node.SetCalcRectInPrepare(true);
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByBackgroundAlpha(RSSurfaceRenderNode& node)
{
    if (node.IsHardwareForcedDisabled()) {
        return;
    }
    bool bgTransport = !node.GetAncoForceDoDirect() &&
        (static_cast<uint8_t>(node.GetRenderProperties().GetBackgroundColor().GetAlpha()) < UINT8_MAX);
    if (bgTransport) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disabled by background color alpha < 1",
            node.GetName().c_str(), node.GetId());
        // use in skip updating hardware state for hwcnode with background alpha in specific situation
        if (!RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag()) {
            node.SetHardwareForcedDisabledState(true);
        }
        node.SetNodeHasBackgroundColorAlpha(true);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(node.GetId(),
            HwcDisabledReasons::DISABLED_BY_BACKGROUND_ALPHA, node.GetName());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByBufferSize(RSSurfaceRenderNode& node)
{
    if (!node.IsRosenWeb() || node.IsHardwareForcedDisabled()) {
        return;
    }
    if (!node.GetRSSurfaceHandler() || !node.GetRSSurfaceHandler()->GetBuffer()) {
        return;
    }
    const auto& property = node.GetRenderProperties();
    auto gravity = property.GetFrameGravity();
    if (gravity != Gravity::TOP_LEFT) {
        return;
    }
    auto surfaceHandler = node.GetRSSurfaceHandler();
    auto consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return;
    }

    auto buffer = surfaceHandler->GetBuffer();
    const auto bufferWidth = buffer->GetSurfaceBufferWidth();
    const auto bufferHeight = buffer->GetSurfaceBufferHeight();
    auto boundsWidth = property.GetBoundsWidth();
    auto boundsHeight = property.GetBoundsHeight();

    auto transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    if (consumer->GetSurfaceBufferTransformType(buffer, &transformType) != GSERROR_OK) {
        RS_LOGE("RSUniRenderVisitor::UpdateHwcNodeEnableByBufferSize GetSurfaceBufferTransformType failed");
    }
    if (transformType == GraphicTransformType::GRAPHIC_ROTATE_270 ||
        transformType == GraphicTransformType::GRAPHIC_ROTATE_90) {
        std::swap(boundsWidth, boundsHeight);
    }
    if ((bufferWidth < boundsWidth) || (bufferHeight < boundsHeight)) {
        RS_OPTIONAL_TRACE_NAME_FMT(
            "hwc debug: name:%s id:%llu buffer:[%d, %d] bounds:[%f, %f] disabled by buffer nonmatching",
            node.GetName().c_str(), node.GetId(), bufferWidth, bufferHeight, boundsWidth, boundsHeight);
        node.SetHardwareForcedDisabledState(true);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(
            node.GetId(), HwcDisabledReasons::DISABLED_BY_BUFFER_NONMATCH, node.GetName());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableBySrcRect(RSSurfaceRenderNode& node)
{
    if (node.IsHardwareForcedDisabled()) {
        return;
    }
    bool hasRotation = false;
    const auto consumer = node.GetRSSurfaceHandler()->GetConsumer();
    if (consumer != nullptr) {
        auto rotation = RSBaseRenderUtil::GetRotateTransform(consumer->GetTransform());
        hasRotation = rotation == GRAPHIC_ROTATE_90 || rotation == GRAPHIC_ROTATE_270;
    }
    node.UpdateHwcDisabledBySrcRect(hasRotation);
    if (node.IsHardwareDisabledBySrcRect()) {
        node.SetHardwareForcedDisabledState(true);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(node.GetId(),
            HwcDisabledReasons::DISABLED_BY_SRC_PIXEL, node.GetName());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(std::vector<RectI>& hwcRects,
    std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    if (hwcNode->IsHardwareForcedDisabled()) {
        return;
    }
    auto dst = hwcNode->GetDstRect();
    if (hwcNode->GetAncoForceDoDirect()) {
        hwcRects.emplace_back(dst);
        return;
    }
    for (auto rect : hwcRects) {
        if (dst.Intersect(rect) && !RsCommonHook::Instance().GetHardwareEnabledByHwcnodeBelowSelfInAppFlag()) {
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disabled by hwc node above",
                hwcNode->GetName().c_str(), hwcNode->GetId());
            hwcNode->SetHardwareForcedDisabledState(true);
            hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
                HwcDisabledReasons::DISABLED_BY_HWC_NODE_ABOVE, hwcNode->GetName());
            return;
        }
    }
    hwcRects.emplace_back(dst);
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByRotateAndAlpha(std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    Drawing::Matrix totalMatrix;
    float alpha = 1.f;
    RSUniRenderUtil::AccumulateMatrixAndAlpha(hwcNode, totalMatrix, alpha);
    hwcNode->SetTotalMatrix(totalMatrix);
    hwcNode->SetGlobalAlpha(alpha);
    if (!ROSEN_EQ(alpha, 1.f)) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disabled by accumulated alpha:%.2f",
            hwcNode->GetName().c_str(), hwcNode->GetId(), alpha);
        hwcNode->SetHardwareForcedDisabledState(true);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
            HwcDisabledReasons::DISABLED_BY_ACCUMULATED_ALPHA, hwcNode->GetName());
        return;
    }
    // [planning] degree only multiples of 90 now
    int degree = RSUniRenderUtil::GetRotationDegreeFromMatrix(totalMatrix);
    bool hasRotate = degree % RS_ROTATION_90 != 0 || RSUniRenderUtil::Is3DRotation(totalMatrix);
    if (hasRotate) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disabled by rotation:%d",
            hwcNode->GetName().c_str(), hwcNode->GetId(), degree);
        hwcNode->SetHardwareForcedDisabledState(true);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
            HwcDisabledReasons::DISABLED_BY_ROTATION, hwcNode->GetName());
        return;
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnable()
{
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    std::for_each(curMainAndLeashSurfaces.rbegin(), curMainAndLeashSurfaces.rend(),
        [this](RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (!surfaceNode) {
            return;
        }
        UpdateHwcNodeEnableByGlobalFilter(surfaceNode);
        surfaceNode->ResetNeedCollectHwcNode();
        const auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            return;
        }
        std::vector<RectI> hwcRects;
        for (auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
                continue;
            }
            UpdateHwcNodeEnableByRotateAndAlpha(hwcNodePtr);
            UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(hwcRects, hwcNodePtr);
        }
    });
    PrevalidateHwcNode();
}

void RSUniRenderVisitor::PrevalidateHwcNode()
{
    if (!isPrevalidateHwcNodeEnable_) {
        return;
    }
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    std::vector<RequestLayerInfo> prevalidLayers;
    uint32_t curFps = OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(curDisplayNode_->GetScreenId());
    uint32_t zOrder = static_cast<uint32_t>(globalZOrder_);
    // add surfaceNode layer
    RSUniHwcPrevalidateUtil::GetInstance().CollectSurfaceNodeLayerInfo(
        prevalidLayers, curMainAndLeashSurfaces, curFps, zOrder, screenInfo_);
    std::vector<RequestLayerInfo> uiFirstLayers;
    // collect uifirst layer
    // zOrder + 1.f is displayNode, UIFirst layer must be above displayNode(zorder + 2.f)
    RSUniHwcPrevalidateUtil::GetInstance().CollectUIFirstLayerInfo(
        uiFirstLayers, curFps, static_cast<float>(zOrder) + 2.f, screenInfo_);
    if (prevalidLayers.size() == 0 && uiFirstLayers.size() == 0) {
        RS_LOGD("RSUniRenderVisitor::PrevalidateHwcNode no hardware layer");
        return;
    }
    // add display layer
    RequestLayerInfo displayLayer;
    if (RSUniHwcPrevalidateUtil::GetInstance().CreateDisplayNodeLayerInfo(
        zOrder++, curDisplayNode_, screenInfo_, curFps, displayLayer)) {
        prevalidLayers.emplace_back(displayLayer);
    }
    // add uiFirst layer
    prevalidLayers.insert(prevalidLayers.end(), uiFirstLayers.begin(), uiFirstLayers.end());
    // add rcd layer
    RequestLayerInfo rcdLayer;
    if (RSUniHwcPrevalidateUtil::GetInstance().CreateRCDLayerInfo(
        RSRcdRenderManager::GetInstance().GetBackgroundSurfaceNode(), screenInfo_, curFps, rcdLayer)) {
        prevalidLayers.emplace_back(rcdLayer);
    }
    if (RSUniHwcPrevalidateUtil::GetInstance().CreateRCDLayerInfo(
        RSRcdRenderManager::GetInstance().GetContentSurfaceNode(), screenInfo_, curFps, rcdLayer)) {
        prevalidLayers.emplace_back(rcdLayer);
    }
    std::map<uint64_t, RequestCompositionType> strategy;
    if (!RSUniHwcPrevalidateUtil::GetInstance().PreValidate(screenInfo_.id, prevalidLayers, strategy)) {
        RS_LOGD("RSUniRenderVisitor::PrevalidateHwcNode preValidate failed");
        return;
    }
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto it : strategy) {
        if (it.second == RequestCompositionType::DEVICE) {
            continue;
        }
        auto node = nodeMap.GetRenderNode<RSSurfaceRenderNode>(it.first);
        if (node == nullptr || node->GetForceHardware() || node->GetProtectedLayer()) {
            continue;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disabled by prevalidate",
            node->GetName().c_str(), node->GetId());
        node->SetHardwareForcedDisabledState(true);
        node->GetRSSurfaceHandler()->SetGlobalZOrder(-1.f);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(node->GetId(),
            HwcDisabledReasons::DISABLED_BY_PREVALIDATE, node->GetName());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeDirtyRegionAndCreateLayer(std::shared_ptr<RSSurfaceRenderNode>& node)
{
    const auto& hwcNodes = node->GetChildHardwareEnabledNodes();
    if (hwcNodes.empty()) {
        return;
    }
    std::shared_ptr<RSSurfaceRenderNode> pointWindow;
    for (auto hwcNode : hwcNodes) {
        auto hwcNodePtr = hwcNode.lock();
        if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
            continue;
        }
        auto surfaceHandler = hwcNodePtr->GetMutableRSSurfaceHandler();
        if (node->IsHardwareEnabledTopSurface()) {
            pointWindow = hwcNodePtr;
            continue;
        }
        UpdateHwcNodeDirtyRegionForApp(node, hwcNodePtr);
        hwcNodePtr->SetCalcRectInPrepare(false);
        hwcNodePtr->SetHardWareDisabledByReverse(false);
        surfaceHandler->SetGlobalZOrder(hwcNodePtr->IsHardwareForcedDisabled() && !hwcNodePtr->GetProtectedLayer()
            ? -1.f : globalZOrder_++);
        auto transform = RSUniRenderUtil::GetLayerTransform(*hwcNodePtr, screenInfo_);
        hwcNodePtr->UpdateHwcNodeLayerInfo(transform);
        if (hwcNodePtr->GetProtectedLayer()) {
            hwcNodePtr->SetForceClientForDRMOnly(displayNodeRotationChanged_ || isScreenRotationAnimating_);
        } else {
            hwcNodePtr->SetForceClientForDRMOnly(false);
        }
        // HDR in UniRender
        if (hwcNodePtr->IsHardwareForcedDisabled() && RSMainThread::CheckIsHdrSurface(*hwcNodePtr)) {
            hasUniRenderHdrSurface_ = true;
        }
    }
    curDisplayNode_->SetDisplayGlobalZOrder(globalZOrder_);
    if (pointWindow) {
        UpdatePointWindowDirtyStatus(pointWindow);
    }
}

void RSUniRenderVisitor::UpdatePointWindowDirtyStatus(std::shared_ptr<RSSurfaceRenderNode>& pointWindow)
{
    std::shared_ptr<RSSurfaceHandler> pointSurfaceHandler = pointWindow->GetMutableRSSurfaceHandler();
    if (pointSurfaceHandler) {
        // globalZOrder_ + 2 is displayNode layer, point window must be at the top.
        pointSurfaceHandler->SetGlobalZOrder(globalZOrder_ + 2);
        pointWindow->SetHardwareForcedDisabledState(!IsHardwareComposerEnabled() || !pointWindow->ShouldPaint());
        auto transform = RSUniRenderUtil::GetLayerTransform(*pointWindow, screenInfo_);
        pointWindow->UpdateHwcNodeLayerInfo(transform);
        auto stagingDisplayParams = static_cast<RSDisplayRenderParams*>(
            curDisplayNode_->GetStagingRenderParams().get());
        auto dirtyManager = pointWindow->GetDirtyManager();
        if (dirtyManager && stagingDisplayParams && !pointWindow->IsHardwareForcedDisabled()) {
            if (!dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty()) {
                dirtyManager->SetCurrentFrameDirtyRect(RectI());
                stagingDisplayParams->SetIsMouseDirty(true);
            } else {
                stagingDisplayParams->SetIsMouseDirty(false);
            }
        }
    }
}

void RSUniRenderVisitor::UpdateHwcNodeDirtyRegionForApp(std::shared_ptr<RSSurfaceRenderNode>& appNode,
    std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    // if current frame hwc enable status not equal with last frame
    // or current frame do gpu composition and has buffer consumed,
    // we need merge hwc node dst rect to dirty region.
    if (!hwcNode->IsHardwareForcedDisabled() != hwcNode->GetIsLastFrameHwcEnabled()) {
        appNode->GetDirtyManager()->MergeDirtyRect(hwcNode->GetDstRect());
        return;
    }
    if (hwcNode->IsHardwareForcedDisabled() && hwcNode->GetRSSurfaceHandler()->IsCurrentFrameBufferConsumed()) {
        appNode->GetDirtyManager()->MergeDirtyRect(hwcNode->GetOldDirtyInSurface());
    }
    if (hasMirrorDisplay_ && hwcNode->GetRSSurfaceHandler()->IsCurrentFrameBufferConsumed()) {
        // merge hwc node dst rect for virtual screen dirty, in case the main display node skip
        curDisplayDirtyManager_->MergeHwcDirtyRect(hwcNode->GetDstRect());
    }
}

void RSUniRenderVisitor::UpdateSurfaceDirtyAndGlobalDirty()
{
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    // this is used to record mainAndLeash surface accumulatedDirtyRegion by Pre-order traversal
    Occlusion::Region accumulatedDirtyRegion;
    bool hasMainAndLeashSurfaceDirty = false;
    hasUniRenderHdrSurface_ = false;
    std::vector<RectI> hwcRects;
    std::for_each(curMainAndLeashSurfaces.rbegin(), curMainAndLeashSurfaces.rend(),
        [this, &accumulatedDirtyRegion,
         &hasMainAndLeashSurfaceDirty, &hwcRects](RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (!surfaceNode) {
            RS_LOGE("RSUniRenderVisitor::UpdateSurfaceDirtyAndGlobalDirty surfaceNode is nullptr");
            return;
        }
        auto dirtyManager = surfaceNode->GetDirtyManager();
        RSMainThread::Instance()->GetContext().AddPendingSyncNode(nodePtr);
        auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        if (!hwcNodes.empty() && RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag() &&
            RsCommonHook::Instance().GetHardwareEnabledByHwcnodeBelowSelfInAppFlag()) {
            UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
        }
        // disable hwc node with corner radius if intersects with hwc node below
        UpdateChildHwcNodeEnableByHwcNodeBelow(hwcRects, surfaceNode);
        // 0. update hwc node dirty region and create layer
        UpdateHwcNodeDirtyRegionAndCreateLayer(surfaceNode);
        // 1. calculate abs dirtyrect and update partialRenderParams
        // currently only sync visible region info
        surfaceNode->UpdatePartialRenderParams();
        if (dirtyManager && dirtyManager->IsCurrentFrameDirty()) {
            hasMainAndLeashSurfaceDirty = true;
        }
        // 2. check surface node dirtyrect need merge into displayDirtyManager
        CheckMergeSurfaceDirtysForDisplay(surfaceNode);
        // 3. check merge transparent filter when it intersects with pre-dirty
        CheckMergeDisplayDirtyByTransparentFilter(surfaceNode, accumulatedDirtyRegion);
        // 4. check whether the composition path of the frame before and after the mouse is consistent
        CheckMergeTopSurfaceForDisplay(surfaceNode);
    });
    curDisplayNode_->SetMainAndLeashSurfaceDirty(hasMainAndLeashSurfaceDirty);
    CheckAndUpdateFilterCacheOcclusion(curMainAndLeashSurfaces);
    CheckMergeDebugRectforRefreshRate(curMainAndLeashSurfaces);
    CheckMergeGlobalFilterForDisplay(accumulatedDirtyRegion);
    ResetDisplayDirtyRegion();
    curDisplayNode_->ClearCurrentSurfacePos();
    std::swap(preMainAndLeashWindowNodesIds_, curMainAndLeashWindowNodesIds_);

#ifdef RS_PROFILER_ENABLED
    RS_PROFILER_SET_DIRTY_REGION(accumulatedDirtyRegion);
#endif
}

void RSUniRenderVisitor::UpdateChildHwcNodeEnableByHwcNodeBelow(std::vector<RectI>& hwcRects,
    std::shared_ptr<RSSurfaceRenderNode>& appNode)
{
    const auto& hwcNodes = appNode->GetChildHardwareEnabledNodes();
    bool hasCornerRadius = !appNode->GetRenderProperties().GetCornerRadius().IsZero();
    for (auto hwcNode : hwcNodes) {
        auto hwcNodePtr = hwcNode.lock();
        if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
            continue;
        }
        UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, hwcNodePtr, hasCornerRadius);
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByHwcNodeBelowSelf(std::vector<RectI>& hwcRects,
    std::shared_ptr<RSSurfaceRenderNode>& hwcNode, bool hasCornerRadius)
{
    if (hwcNode->IsHardwareForcedDisabled()) {
        return;
    }
    auto dst = hwcNode->GetDstRect();
    if (hwcNode->GetAncoForceDoDirect() || !hasCornerRadius) {
        hwcRects.emplace_back(dst);
        return;
    }
    for (auto rect : hwcRects) {
        if (dst.Intersect(rect)) {
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disabled by corner radius + hwc node below",
                hwcNode->GetName().c_str(), hwcNode->GetId());
            hwcNode->SetHardwareForcedDisabledState(true);
            hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
                HwcDisabledReasons::DISABLED_BY_HWC_NODE_ABOVE, hwcNode->GetName());
            return;
        }
    }
    hwcRects.emplace_back(dst);
}

void RSUniRenderVisitor::UpdateSurfaceOcclusionInfo()
{
    allDstCurVisVec_.insert(allDstCurVisVec_.end(), dstCurVisVec_.begin(), dstCurVisVec_.end());
    dstCurVisVec_.clear();
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparent(RSSurfaceRenderNode& surfaceNode) const
{
    // surfaceNode is transparent
    const auto& dirtyRect = surfaceNode.GetDirtyManager()->GetCurrentFrameDirtyRegion();
    auto oldDirtyInSurface = surfaceNode.GetOldDirtyInSurface();
    if (surfaceNode.IsTransparent()) {
        RectI transparentDirtyRect = oldDirtyInSurface.IntersectRect(dirtyRect);
        if (!transparentDirtyRect.IsEmpty()) {
            RS_LOGD("CheckMergeDisplayDirtyByTransparent global merge transparent dirty "
                "%{public}s: global dirty %{public}s, add rect %{public}s", surfaceNode.GetName().c_str(),
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                transparentDirtyRect.ToString().c_str());
            MergeDirtySurfaceToDssOrDirty(surfaceNode, transparentDirtyRect);
        }
    }
    // surfaceNode has transparent regions
    CheckMergeDisplayDirtyByTransparentRegions(surfaceNode);
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByZorderChanged(RSSurfaceRenderNode& surfaceNode) const
{
    auto oldDirtyInSurface = surfaceNode.GetOldDirtyInSurface();
    if (surfaceNode.GetZorderChanged()) {
        RS_LOGD("CheckMergeDisplayDirtyByZorderChanged global merge GetZorderChanged "
                "%{public}s: global dirty %{public}s, add rect %{public}s", surfaceNode.GetName().c_str(),
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                oldDirtyInSurface.ToString().c_str());
        curDisplayNode_->GetDirtyManager()->MergeDirtyRect(oldDirtyInSurface);
    }
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByPosChanged(RSSurfaceRenderNode& surfaceNode) const
{
    RectI lastFrameSurfacePos = curDisplayNode_->GetLastFrameSurfacePos(surfaceNode.GetId());
    RectI currentFrameSurfacePos = curDisplayNode_->GetCurrentFrameSurfacePos(surfaceNode.GetId());
    if (surfaceNode.GetAnimateState() || lastFrameSurfacePos != currentFrameSurfacePos) {
        RS_LOGD("CheckMergeDisplayDirtyByPosChanged global merge surface pos changed "
            "%{public}s: global dirty %{public}s, lastFrameRect %{public}s currentFrameRect %{public}s",
            surfaceNode.GetName().c_str(),
            curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
            lastFrameSurfacePos.ToString().c_str(), currentFrameSurfacePos.ToString().c_str());
        if (!lastFrameSurfacePos.IsEmpty()) {
            MergeDirtySurfaceToDssOrDirty(surfaceNode, lastFrameSurfacePos);
        }
        if (!currentFrameSurfacePos.IsEmpty()) {
            MergeDirtySurfaceToDssOrDirty(surfaceNode, currentFrameSurfacePos);
        }
    }
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByShadowChanged(RSSurfaceRenderNode& surfaceNode) const
{
    const auto& dirtyRect = surfaceNode.GetDirtyManager()->GetCurrentFrameDirtyRegion();
    auto oldDirtyInSurface = surfaceNode.GetOldDirtyInSurface();
    bool isShadowDisappear = !surfaceNode.GetRenderProperties().IsShadowValid() &&
        surfaceNode.IsShadowValidLastFrame();
    if (surfaceNode.GetRenderProperties().IsShadowValid() || isShadowDisappear) {
        RectI shadowDirtyRect = oldDirtyInSurface.IntersectRect(dirtyRect);
        // There are two situation here:
        // 1. SurfaceNode first has shadow or shadow radius is larger than the last frame,
        // dirtyRect == surfaceNode.GetOldDirtyInSurface()
        // 2. SurfaceNode remove shadow or shadow radius is smaller than the last frame,
        // dirtyRect > surfaceNode.GetOldDirtyInSurface()
        // So we should always merge dirtyRect here.
        if (!shadowDirtyRect.IsEmpty()) {
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(dirtyRect);
            RS_LOGD("CheckMergeDisplayDirtyByShadowChanged global merge ShadowValid %{public}s: "
                "global dirty %{public}s, add rect %{public}s", surfaceNode.GetName().c_str(),
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                dirtyRect.ToString().c_str());
        }
        if (isShadowDisappear) {
            surfaceNode.SetShadowValidLastFrame(false);
        }
    }
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyBySurfaceChanged() const
{
    std::vector<RectI> surfaceChangedRects = curDisplayNode_->GetSurfaceChangedRects();
    for (auto& surfaceChangedRect : surfaceChangedRects) {
        RS_LOGD("CheckMergeDisplayDirtyBySurfaceChanged global merge Surface closed, global dirty %{public}s,"
            "add rect %{public}s", curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
            surfaceChangedRect.ToString().c_str());
        if (!surfaceChangedRect.IsEmpty()) {
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(surfaceChangedRect);
        }
    }
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByAttraction(RSSurfaceRenderNode& surfaceNode) const
{
    if (surfaceNode.GetRenderProperties().IsAttractionValid()) {
        auto attractionDirtyRect_ = surfaceNode.GetRenderProperties().GetAttractionEffectCurrentDirtyRegion();
        RS_LOGD("CheckMergeDisplayDirtyByAttraction global merge attraction %{public}s: global dirty %{public}s,"
            "add rect %{public}s", surfaceNode.GetName().c_str(),
            curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
            attractionDirtyRect_.ToString().c_str());
        curDisplayNode_->GetDirtyManager()->MergeDirtyRect(attractionDirtyRect_);
    }
}

void RSUniRenderVisitor::MergeDirtySurfaceToDssOrDirty(RSSurfaceRenderNode& surfaceNode,
    const RectI& dirtyRect) const
{
    // The surfaceNode has been confirmed to be non-null at the invocation
    // this function is non-null
    if (surfaceNode.IsHardwareEnabledTopSurface() && !surfaceNode.IsHardwareForcedDisabled()) {
        curDisplayNode_->GetDirtyManager()->MergeHwcDirtyRect(dirtyRect);
    } else {
        curDisplayNode_->GetDirtyManager()->MergeDirtyRect(dirtyRect);
    }
}

void RSUniRenderVisitor::CheckMergeTopSurfaceForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    // The surfaceNode has been confirmed to be non-null at the invocation
    // this function is non-null
    if (surfaceNode->IsHardwareEnabledTopSurface() && (surfaceNode->GetIsLastFrameHwcEnabled() !=
        !surfaceNode->IsHardwareForcedDisabled())) {
        auto oldDirtyInSurface = surfaceNode->GetOldDirtyInSurface();
        RS_LOGD("Mouse dirty %{public}s", oldDirtyInSurface.ToString().c_str());
        curDisplayNode_->GetDirtyManager()->MergeDirtyRect(oldDirtyInSurface);
    }
}

void RSUniRenderVisitor::CheckMergeSurfaceDirtysForDisplay(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    // 1 Handles the case of transparent surface, merge transparent dirty rect
    CheckMergeDisplayDirtyByTransparent(*surfaceNode);
    // 2 Zorder changed case, merge surface dest Rect
    CheckMergeDisplayDirtyByZorderChanged(*surfaceNode);
    // 3 surfacePos chanded case, merge surface lastframe pos or curframe pos
    CheckMergeDisplayDirtyByPosChanged(*surfaceNode);
    // 4 shadow disappear and appear case.
    CheckMergeDisplayDirtyByShadowChanged(*surfaceNode);
    // 5 handle last and curframe surfaces which appear or disappear case
    CheckMergeDisplayDirtyBySurfaceChanged();
    // 6 handle surface has attraction effect
    CheckMergeDisplayDirtyByAttraction(*surfaceNode);
    // More: any other display dirty caused by surfaceNode should be added here like CheckMergeDisplayDirtByXXX
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparentRegions(RSSurfaceRenderNode& surfaceNode) const
{
    const auto& dirtyRect = surfaceNode.GetDirtyManager()->GetCurrentFrameDirtyRegion();
    if (surfaceNode.HasContainerWindow()) {
        // If a surface's dirty is intersect with container region (which can be considered transparent)
        // should be added to display dirty region.
        // Note: we use containerRegion rather transparentRegion to bypass inner corner dirty problem.
        auto containerRegion = surfaceNode.GetContainerRegion();
        auto surfaceDirtyRegion = Occlusion::Region{ Occlusion::Rect{ dirtyRect } };
        auto containerDirtyRegion = containerRegion.And(surfaceDirtyRegion);
        if (!containerDirtyRegion.IsEmpty()) {
            RS_LOGD("CheckMergeDisplayDirtyByContainer global merge containerDirtyRegion "
                "%{public}s: global dirty %{public}s, add region %{public}s", surfaceNode.GetName().c_str(),
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                containerDirtyRegion.GetRegionInfo().c_str());
            // plan: we can use surfacenode's absrect as containerRegion's bound
            const auto& rect = containerRegion.GetBoundRef();
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(
                RectI{ rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
        }
    } else {
        // warning: if a surfacenode has transparent region and opaque region, and its dirty pattern appears in
        // transparent region and opaque region in adjacent frame, may cause displaydirty region incomplete after
        // merge history (as surfacenode's dirty region merging opaque region will enlarge surface dirty region
        // which include transparent region but not counted in display dirtyregion)
        if (!surfaceNode.IsNodeDirty()) {
            return;
        }
        auto transparentRegion = surfaceNode.GetTransparentRegion();
        auto surfaceDirtyRegion = Occlusion::Region{ Occlusion::Rect{ dirtyRect } };
        Occlusion::Region transparentDirtyRegion = transparentRegion.And(surfaceDirtyRegion);
        if (!transparentDirtyRegion.IsEmpty()) {
            RS_LOGD("CheckMergeDisplayDirtyByTransparentRegions global merge TransparentDirtyRegion "
                "%{public}s: global dirty %{public}s, add region %{public}s", surfaceNode.GetName().c_str(),
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                transparentDirtyRegion.GetRegionInfo().c_str());
            const std::vector<Occlusion::Rect>& rects = transparentDirtyRegion.GetRegionRects();
            for (const auto& rect : rects) {
                MergeDirtySurfaceToDssOrDirty(surfaceNode, RectI{ rect.left_, rect.top_,
                    rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
            }
        }
    }
}

bool RSUniRenderVisitor::IfSkipInCalcGlobalDirty(RSSurfaceRenderNode& surfaceNode) const
{
    return hasMirrorDisplay_ ?
        surfaceNode.GetVisibleRegionInVirtual().IsEmpty() : surfaceNode.GetVisibleRegion().IsEmpty();
}

void RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparentFilter(
    std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    Occlusion::Region& accumulatedDirtyRegion)
{
    if (surfaceNode->IsMainWindowType() && IfSkipInCalcGlobalDirty(*surfaceNode)) {
        RS_LOGD("RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparentFilter surface:%{public}s "
            "which is occluded don't need to process filter", surfaceNode->GetName().c_str());
        return;
    }
    auto disappearedSurfaceRegionBelowCurrent =
        curDisplayNode_->GetDisappearedSurfaceRegionBelowCurrent(surfaceNode->GetId());
    accumulatedDirtyRegion.OrSelf(disappearedSurfaceRegionBelowCurrent);
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto filterVecIter = transparentCleanFilter_.find(surfaceNode->GetId());
    if (filterVecIter != transparentCleanFilter_.end()) {
        RS_LOGD("RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparentFilter surface:%{public}s "
            "has transparentCleanFilter", surfaceNode->GetName().c_str());
        // check accumulatedDirtyRegion influence filter nodes which in the current surface
        for (auto it = filterVecIter->second.begin(); it != filterVecIter->second.end(); ++it) {
            auto filterNode = nodeMap.GetRenderNode(it->first);
            if (filterNode == nullptr) {
                continue;
            }
            auto filterRegion = Occlusion::Region{ Occlusion::Rect{ it->second } };
            auto filterDirtyRegion = filterRegion.And(accumulatedDirtyRegion);
            if (!filterDirtyRegion.IsEmpty()) {
                if (filterNode->GetRenderProperties().GetBackgroundFilter()) {
                    // backgroundfilter affected by below dirty
                    filterNode->MarkFilterStatusChanged(false, false);
                }
                RS_LOGD("RSUniRenderVisitor::CheckMergeDisplayDirtyByTransparentFilter global merge filterRegion "
                    "%{public}s: global dirty %{public}s, add rect %{public}s", surfaceNode->GetName().c_str(),
                    curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                    it->second.ToString().c_str());
                curDisplayNode_->GetDirtyManager()->MergeDirtyRect(it->second);
                if (filterNode->GetRenderProperties().GetFilter()) {
                    // foregroundfilter affected by below dirty
                    filterNode->MarkFilterStatusChanged(true, false);
                }
            } else {
                globalFilter_.insert(*it);
            }
            filterNode->PostPrepareForBlurFilterNode(*(curDisplayNode_->GetDirtyManager()), needRequestNextVsync_);
        }
    }
    CheckFilterCacheFullyCovered(surfaceNode);
    auto surfaceDirtyRegion = Occlusion::Region{
        Occlusion::Rect{ surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion() } };
    accumulatedDirtyRegion.OrSelf(surfaceDirtyRegion);
}

void RSUniRenderVisitor::CheckFilterCacheFullyCovered(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    surfaceNode->SetFilterCacheFullyCovered(false);
    bool dirtyBelowContainsFilterNode = false;
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto& child : surfaceNode->GetVisibleFilterChild()) {
        auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(child);
        if (filterNode == nullptr || !filterNode->HasBlurFilter()) {
            continue;
        }
        if (ROSEN_EQ(filterNode->GetGlobalAlpha(), 1.f) && !dirtyBelowContainsFilterNode) {
            surfaceNode->CheckValidFilterCacheFullyCoverTarget(*filterNode, screenRect_);
        }
        if (filterNode->EffectNodeShouldPaint() && !filterNode->IsFilterCacheValid()) {
            dirtyBelowContainsFilterNode = true;
        }
    }
}

void RSUniRenderVisitor::UpdateOccludedStatusWithFilterNode(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    if (surfaceNode == nullptr) {
        return;
    }
    if (surfaceNode->HasBlurFilter()) {
        surfaceNode->SetOccludedStatus(surfaceNode->IsOccludedByFilterCache());
    }
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto& child : surfaceNode->GetVisibleFilterChild()) {
        auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(child);
        if (filterNode == nullptr || !filterNode->HasBlurFilter()) {
            continue;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderVisitor::UpdateOccludedStatusWithFilterNode "
            "surfaceNode: %s, filterNode:[%lld], IsOccludedByFilterCache:%d", surfaceNode->GetName().c_str(),
            filterNode->GetId(), surfaceNode->IsOccludedByFilterCache());
        filterNode->SetOccludedStatus(surfaceNode->IsOccludedByFilterCache());
    }
}

void RSUniRenderVisitor::CheckAndUpdateFilterCacheOcclusion(
    std::vector<RSBaseRenderNode::SharedPtr>& curMainAndLeashSurfaces) const
{
    if (!RSSystemParameters::GetFilterCacheOcculusionEnabled()) {
        return;
    }
    bool isScreenOccluded = false;
    // top-down traversal all mainsurface
    // if upper surface reuse filter cache which fully cover whole screen
    // mark lower layers for process skip
    std::for_each(curMainAndLeashSurfaces.begin(), curMainAndLeashSurfaces.end(),
        [this, &isScreenOccluded](RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (surfaceNode->IsMainWindowType()) {
            // reset occluded status for all mainwindow
            surfaceNode->UpdateOccludedByFilterCache(isScreenOccluded);
            if (surfaceNode->IsOccludedByFilterCache()) {
                UpdateOccludedStatusWithFilterNode(surfaceNode);
            }
        }
        isScreenOccluded = isScreenOccluded || surfaceNode->GetFilterCacheFullyCovered();
    });
}

void RSUniRenderVisitor::CheckMergeGlobalFilterForDisplay(Occlusion::Region& accumulatedDirtyRegion)
{
    // [planning] if not allowed containerNode filter, The following processing logic can be removed
    // Recursively traverses container nodes need filter
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto it = containerFilter_.begin(); it != containerFilter_.end(); ++it) {
        auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(it->first);
        if (filterNode == nullptr) {
            continue;
        }
        auto filterRegion = Occlusion::Region{ Occlusion::Rect{ it->second } };
        auto filterDirtyRegion = filterRegion.And(accumulatedDirtyRegion);
        RS_OPTIONAL_TRACE_NAME_FMT("CheckMergeGlobalFilterForDisplay::filternode:%llu, filterRect:%s, dirtyRegion:%s",
            filterNode->GetId(), it->second.ToString().c_str(), accumulatedDirtyRegion.GetRegionInfo().c_str());
        if (!filterDirtyRegion.IsEmpty()) {
            RS_LOGD("RSUniRenderVisitor::CheckMergeGlobalFilterForDisplay global merge, "
                "global dirty %{public}s, add container filterRegion %{public}s",
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                (it->second).ToString().c_str());
            if (filterNode->GetRenderProperties().GetBackgroundFilter()) {
                filterNode->MarkFilterStatusChanged(false, false); // background filter affected by below dirty
            }
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(it->second);
            if (filterNode->GetRenderProperties().GetFilter()) {
                filterNode->MarkFilterStatusChanged(true, false); // foreground filter affected by below dirty
            }
        } else {
            globalFilter_.insert(*it);
        }
        filterNode->UpdateFilterCacheWithSelfDirty();
        filterNode->PostPrepareForBlurFilterNode(*(curDisplayNode_->GetDirtyManager()), needRequestNextVsync_);
    }

    // Recursively traverses until the globalDirty do not change
    for (auto it = globalFilter_.begin(); it != globalFilter_.end();) {
        auto lastGlobalDirtyRect = curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion();
        auto filterRegion = Occlusion::Region{ Occlusion::Rect{ it->second } };
        auto lastGlobalDirtyRegion = Occlusion::Region{ Occlusion::Rect{ lastGlobalDirtyRect } };
        auto filterDirtyRegion = filterRegion.And(lastGlobalDirtyRegion);
        if (!filterDirtyRegion.IsEmpty()) {
            RS_LOGD("RSUniRenderVisitor::CheckMergeGlobalFilterForDisplay global merge, "
                "global dirty %{public}s, add filterRegion %{public}s",
                curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
                (it->second).ToString().c_str());
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(it->second);
            it = globalFilter_.erase(it);
            if (lastGlobalDirtyRect != curDisplayNode_->GetDirtyManager()->GetCurrentFrameDirtyRegion()) {
                // When DisplayDirtyRegion is changed, collect dirty filter region from begin.
                // After all filter region is added, the cycle will definitely stop.
                it = globalFilter_.begin();
            }
        } else {
            ++it;
        }
    }
}

void RSUniRenderVisitor::CollectEffectInfo(RSRenderNode& node)
{
    auto nodeParent = node.GetParent().lock();
    if (nodeParent == nullptr) {
        return;
    }
    if (node.GetRenderProperties().NeedFilter() || node.ChildHasVisibleFilter()) {
        nodeParent->SetChildHasVisibleFilter(true);
        nodeParent->UpdateVisibleFilterChild(node);
    }
    if (node.GetRenderProperties().GetUseEffect() || node.ChildHasVisibleEffect()) {
        nodeParent->SetChildHasVisibleEffect(true);
        nodeParent->UpdateVisibleEffectChild(node);
    }
    if (node.GetSharedTransitionParam() || node.ChildHasSharedTransition()) {
        nodeParent->SetChildHasSharedTransition(true);
    }
}

void RSUniRenderVisitor::PostPrepare(RSRenderNode& node, bool subTreeSkipped)
{
    auto curDirtyManager = curSurfaceNode_ ? curSurfaceDirtyManager_ : curDisplayDirtyManager_;
    auto isOccluded = curSurfaceNode_ ?
        curSurfaceNode_->IsMainWindowType() && curSurfaceNode_->GetVisibleRegion().IsEmpty() : false;
    if (subTreeSkipped && !isOccluded) {
        UpdateHwcNodeRectInSkippedSubTree(node);
        CheckFilterNodeInSkippedSubTreeNeedClearCache(node, *curDirtyManager);
        UpdateSubSurfaceNodeRectInSkippedSubTree(node);
    }
    if (node.GetRenderProperties().NeedFilter()) {
        UpdateHwcNodeEnableByFilterRect(
            curSurfaceNode_, node.GetOldDirtyInSurface(), NeedPrepareChindrenInReverseOrder(node));
        auto globalFilterRect = (node.IsInstanceOf<RSEffectRenderNode>() && !node.FirstFrameHasEffectChildren()) ?
            GetVisibleEffectDirty(node) : node.GetOldDirtyInSurface();
        node.CalVisibleFilterRect(prepareClipRect_);
        node.MarkClearFilterCacheIfEffectChildrenChanged();
        CollectFilterInfoAndUpdateDirty(node, *curDirtyManager, globalFilterRect);
        node.SetGlobalAlpha(curAlpha_);
    }
    CollectEffectInfo(node);
    node.MapAndUpdateChildrenRect();
    node.UpdateSubTreeInfo(prepareClipRect_);
    node.UpdateLocalDrawRect();
    node.ResetChangeState();
    if (isDrawingCacheEnabled_) {
        node.UpdateDrawingCacheInfoAfterChildren();
    }
    if (auto nodeParent = node.GetParent().lock()) {
        nodeParent->UpdateChildUifirstSupportFlag(node.GetUifirstSupportFlag());
        nodeParent->OpincUpdateNodeSupportFlag(node.OpincGetNodeSupportFlag());
    }
    if (node.GetSharedTransitionParam() && node.GetRenderProperties().GetSandBox()) {
        node.GetStagingRenderParams()->SetAlpha(curAlpha_);
    } else {
        node.GetStagingRenderParams()->SetAlpha(node.GetRenderProperties().GetAlpha());
    }

    // planning: only do this if node is dirty
    node.UpdateRenderParams();

    // add if node is dirty
    node.AddToPendingSyncList();
}

void RSUniRenderVisitor::CheckFilterNodeInSkippedSubTreeNeedClearCache(
    const RSRenderNode& rootNode, RSDirtyRegionManager& dirtyManager)
{
    bool rotationChanged = curDisplayNode_ ?
        curDisplayNode_->IsRotationChanged() || curDisplayNode_->IsLastRotationChanged() : false;
    bool hdrChanged = curDisplayNode_ ?
        RSLuminanceControl::Get().IsDimmingOn(curDisplayNode_->GetScreenId()) : false;
    bool rotationStatusChanged = curDisplayNode_ ?
        curDisplayNode_->GetPreRotationStatus() != curDisplayNode_->GetCurRotationStatus() : false;
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto& child : rootNode.GetVisibleFilterChild()) {
        auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(child);
        if (filterNode == nullptr) {
            continue;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("CheckFilterNodeInSkippedSubTreeNeedClearCache node[%lld]", filterNode->GetId());
        if (auto effectNode = RSRenderNode::ReinterpretCast<RSEffectRenderNode>(filterNode)) {
            UpdateRotationStatusForEffectNode(*effectNode);
        }
        filterNode->CheckBlurFilterCacheNeedForceClearOrSave(rotationChanged || hdrChanged, rotationStatusChanged);
        filterNode->MarkClearFilterCacheIfEffectChildrenChanged();
        if (filterNode->GetRenderProperties().GetBackgroundFilter()) {
            filterNode->UpdateFilterCacheWithBelowDirty(dirtyManager, false);
        }
        RectI filterRect;
        filterNode->UpdateFilterRegionInSkippedSubTree(dirtyManager, rootNode, filterRect, prepareClipRect_);
        UpdateHwcNodeEnableByFilterRect(curSurfaceNode_, filterNode->GetOldDirtyInSurface());
        CollectFilterInfoAndUpdateDirty(*filterNode, dirtyManager, filterRect);
    }
}

void RSUniRenderVisitor::UpdateHwcNodeRectInSkippedSubTree(const RSRenderNode& rootNode)
{
    if (RS_PROFILER_SHOULD_BLOCK_HWCNODE()) {
        return;
    }
    
    if (!curSurfaceNode_) {
        return;
    }
    const auto& hwcNodes = curSurfaceNode_->GetChildHardwareEnabledNodes();
    if (hwcNodes.empty()) {
        return;
    }
    for (auto hwcNode : hwcNodes) {
        auto hwcNodePtr = hwcNode.lock();
        if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree() || hwcNodePtr->GetCalcRectInPrepare()) {
            continue;
        }
        const auto& property = hwcNodePtr->GetRenderProperties();
        auto matrix = property.GetBoundsGeometry()->GetMatrix();
        auto parent = hwcNodePtr->GetParent().lock();
        bool findInRoot = parent ? parent->GetId() == rootNode.GetId() : false;
        while (parent && parent->GetType() != RSRenderNodeType::DISPLAY_NODE) {
            const auto& property = parent->GetRenderProperties();
            matrix.PostConcat(property.GetBoundsGeometry()->GetMatrix());
            parent = parent->GetParent().lock();
            if (!parent) {
                break;
            }
            findInRoot = parent->GetId() == rootNode.GetId() ? true : findInRoot;
        }
        if (!findInRoot) {
            continue;
        }
        if (parent) {
            const auto& parentProperty = parent->GetRenderProperties();
            matrix.PostConcat(parentProperty.GetBoundsGeometry()->GetMatrix());
        }
        auto surfaceHandler = hwcNodePtr->GetMutableRSSurfaceHandler();
        if (!(hwcNodePtr->GetTotalMatrix() == matrix) ||
            surfaceHandler->GetBufferSizeChanged() || surfaceHandler->CheckScalingModeChanged()) {
            const auto& properties = hwcNodePtr->GetRenderProperties();
            Drawing::Rect bounds = Drawing::Rect(0, 0, properties.GetBoundsWidth(), properties.GetBoundsHeight());
            Drawing::Rect absRect;
            matrix.MapRect(absRect, bounds);
            RectI rect = {std::round(absRect.left_), std::round(absRect.top_),
                std::round(absRect.GetWidth()), std::round(absRect.GetHeight())};
            UpdateDstRect(*hwcNodePtr, rect, prepareClipRect_);
            UpdateSrcRect(*hwcNodePtr, matrix, rect);
            UpdateHwcNodeByTransform(*hwcNodePtr);
            UpdateHwcNodeEnableBySrcRect(*hwcNodePtr);
            UpdateHwcNodeEnableByBufferSize(*hwcNodePtr);
        }
        hwcNodePtr->SetTotalMatrix(matrix);
    }
}

void RSUniRenderVisitor::UpdateHardwareStateByHwcNodeBackgroundAlpha(
    const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes)
{
    std::list<RectI> hwcRects;
    for (int i = 0; i < hwcNodes.size(); i++) {
        auto hwcNodePtr = hwcNodes[i].lock();
        if (!hwcNodePtr) {
            continue;
        }
        if (!hwcNodePtr->IsNodeHasBackgroundColorAlpha() && !hwcNodePtr->IsHardwareForcedDisabled()) {
            hwcRects.push_back(hwcNodePtr->GetDstRect());
        } else if (hwcNodePtr->IsNodeHasBackgroundColorAlpha() && !hwcNodePtr->IsHardwareForcedDisabled() &&
            hwcRects.size() != 0 && IsNodeAboveInsideOfNodeBelow(hwcNodePtr->GetDstRect(), hwcRects)) {
            continue;
        } else {
            hwcNodePtr->SetHardwareForcedDisabledState(true);
        }
    }
}

bool RSUniRenderVisitor::IsNodeAboveInsideOfNodeBelow(const RectI& rectAbove, std::list<RectI>& hwcNodeRectList)
{
    for (auto rectBelow: hwcNodeRectList) {
        if (rectAbove.IsInsideOf(rectBelow)) {
            return true;
        }
    }
    return false;
}

void RSUniRenderVisitor::CalcHwcNodeEnableByFilterRect(
    std::shared_ptr<RSSurfaceRenderNode>& node, const RectI& filterRect, bool isReverseOrder)
{
    if (!node) {
        return;
    }
    auto dstRect = node->GetDstRect();
    bool isIntersect = dstRect.Intersect(filterRect);
    if (isIntersect) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disabled by filter rect",
            node->GetName().c_str(), node->GetId());
        node->SetHardwareForcedDisabledState(true);
        node->SetHardWareDisabledByReverse(isReverseOrder);
        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(node->GetId(),
            HwcDisabledReasons::DISABLED_BY_FLITER_RECT, node->GetName());
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByFilterRect(
    std::shared_ptr<RSSurfaceRenderNode>& node, const RectI& filterRect, bool isReverseOrder)
{
    if (filterRect.IsEmpty()) {
        return;
    }
    if (!node) {
        const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
        if (selfDrawingNodes.empty()) {
            return;
        }
        for (auto hwcNode : selfDrawingNodes) {
            CalcHwcNodeEnableByFilterRect(hwcNode, filterRect, isReverseOrder);
        }
    } else {
        const auto& hwcNodes = node->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            return;
        }
        for (auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            CalcHwcNodeEnableByFilterRect(hwcNodePtr, filterRect, isReverseOrder);
        }
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByGlobalFilter(std::shared_ptr<RSSurfaceRenderNode>& node)
{
    auto cleanFilter = transparentCleanFilter_.find(node->GetId());
    bool cleanFilterFound = (cleanFilter != transparentCleanFilter_.end());
    auto dirtyFilter = transparentDirtyFilter_.find(node->GetId());
    bool dirtyFilterFound = (dirtyFilter != transparentDirtyFilter_.end());
    auto& curMainAndLeashSurfaces = curDisplayNode_->GetAllMainAndLeashSurfaces();
    for (auto it = curMainAndLeashSurfaces.rbegin(); it != curMainAndLeashSurfaces.rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr) {
            continue;
        }
        if (surfaceNode->GetId() == node->GetId()) {
            return;
        }
        const auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            continue;
        }
        for (auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr || hwcNodePtr->IsHardwareForcedDisabled()) {
                continue;
            }
            if (cleanFilterFound) {
                UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter->second, *hwcNodePtr);
                if (hwcNodePtr->IsHardwareForcedDisabled()) {
                    continue;
                }
            }
            if (dirtyFilterFound) {
                for (auto filter = dirtyFilter->second.begin(); filter != dirtyFilter->second.end(); ++filter) {
                    if (hwcNodePtr->GetDstRect().Intersect(filter->second)) {
                        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disabled by transparentDirtyFilter",
                            hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId());
                        hwcNodePtr->SetHardwareForcedDisabledState(true);
                        hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNodePtr->GetId(),
                            HwcDisabledReasons::DISABLED_BY_TRANSPARENT_DIRTY_FLITER, hwcNodePtr->GetName());
                        break;
                    }
                }
            }
        }
    }
}

void RSUniRenderVisitor::UpdateHwcNodeEnableByGlobalCleanFilter(
    const std::vector<std::pair<NodeId, RectI>>& cleanFilter, RSSurfaceRenderNode& hwcNodePtr)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto filter = cleanFilter.begin(); filter != cleanFilter.end(); ++filter) {
        if (hwcNodePtr.GetDstRect().Intersect(filter->second)) {
            auto& rendernode = nodeMap.GetRenderNode<RSRenderNode>(filter->first);
            if (rendernode == nullptr) {
                ROSEN_LOGD("RSUniRenderVisitor::UpdateHwcNodeByFilter: rendernode is null");
                continue;
            }

            if (rendernode->IsAIBarFilterCacheValid()) {
                ROSEN_LOGD("RSUniRenderVisitor::UpdateHwcNodeByFilter: skip intersection for using cache");
                continue;
            }
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%llu disabled by transparentCleanFilter",
                hwcNodePtr.GetName().c_str(), hwcNodePtr.GetId());
            hwcNodePtr.SetHardwareForcedDisabledState(true);
            hwcDisabledReasonCollection_.UpdateHwcDisabledReasonForDFX(hwcNodePtr.GetId(),
                HwcDisabledReasons::DISABLED_BY_TRANSPARENT_CLEAN_FLITER, hwcNodePtr.GetName());
            break;
        }
    }
}

inline static void ResetSubSurfaceNodesCalState(
    std::vector<std::pair<NodeId, std::weak_ptr<RSSurfaceRenderNode>>>& subSurfaceNodes)
{
    for (auto& [id, node] : subSurfaceNodes) {
        auto subSurfaceNodePtr = node.lock();
        if (!subSurfaceNodePtr) {
            continue;
        }
        subSurfaceNodePtr->SetCalcRectInPrepare(false);
    }
}

void RSUniRenderVisitor::UpdateSubSurfaceNodeRectInSkippedSubTree(const RSRenderNode& rootNode)
{
    if (!curSurfaceNode_) {
        return;
    }
    auto rootGeo = rootNode.GetRenderProperties().GetBoundsGeometry();
    if (!rootGeo) {
        return;
    }

    std::vector<std::pair<NodeId, std::weak_ptr<RSSurfaceRenderNode>>> allSubSurfaceNodes;
    curSurfaceNode_->GetAllSubSurfaceNodes(allSubSurfaceNodes);
    for (auto& [_, subSurfaceNode] : allSubSurfaceNodes) {
        auto subSurfaceNodePtr = subSurfaceNode.lock();
        Drawing::Matrix absMatrix;
        if (!subSurfaceNodePtr || subSurfaceNodePtr->GetCalcRectInPrepare() ||
            !subSurfaceNodePtr->GetAbsMatrixReverse(rootNode, absMatrix)) {
            continue;
        }

        Drawing::RectF absDrawRect;
        absMatrix.MapRect(absDrawRect, RSPropertiesPainter::Rect2DrawingRect(subSurfaceNodePtr->GetSelfDrawRect()));
        RectI subSurfaceRect = RectI(absDrawRect.GetLeft(), absDrawRect.GetTop(),
            absDrawRect.GetWidth(), absDrawRect.GetHeight());

        subSurfaceNodePtr->SetOldDirtyInSurface(subSurfaceRect.IntersectRect(prepareClipRect_));
        UpdateNodeVisibleRegion(*subSurfaceNodePtr);
        UpdateDstRect(*subSurfaceNodePtr, subSurfaceRect, prepareClipRect_);
        subSurfaceNodePtr->SetCalcRectInPrepare(true);
        if (subSurfaceNodePtr->IsLeashOrMainWindow()) {
            curMainAndLeashWindowNodesIds_.push(subSurfaceNodePtr->GetId());
            curAllMainAndLeashWindowNodesIds_.emplace_back(subSurfaceNodePtr->GetId());
            curDisplayNode_->RecordMainAndLeashSurfaces(subSurfaceNodePtr);
            CollectOcclusionInfoForWMS(*subSurfaceNodePtr);
        }
    }
    ResetSubSurfaceNodesCalState(allSubSurfaceNodes);
}

RectI RSUniRenderVisitor::GetVisibleEffectDirty(RSRenderNode& node) const
{
    RectI childEffectRect;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto& nodeId : node.GetVisibleEffectChild()) {
        if (auto& subnode = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
            childEffectRect = childEffectRect.JoinRect(subnode->GetOldDirtyInSurface());
        }
    }
    return childEffectRect;
}

void RSUniRenderVisitor::CollectFilterInfoAndUpdateDirty(RSRenderNode& node,
    RSDirtyRegionManager& dirtyManager, const RectI& globalFilterRect)
{
    bool isNodeAddedToTransparentCleanFilters = false;
    if (curSurfaceNode_) {
        bool isIntersect = dirtyManager.GetCurrentFrameDirtyRegion().Intersect(globalFilterRect);
        if (isIntersect) {
            dirtyManager.MergeDirtyRect(globalFilterRect);
        }
        if (node.GetRenderProperties().GetFilter()) {
            node.UpdateFilterCacheWithBelowDirty(dirtyManager, true);
        }
        node.UpdateFilterCacheWithSelfDirty();
        if (curSurfaceNode_->IsTransparent()) {
            globalFilterRects_.emplace_back(globalFilterRect);
            if (!isIntersect || (isIntersect && node.GetRenderProperties().GetBackgroundFilter() &&
                !node.IsBackgroundInAppOrNodeSelfDirty())) {
                // record nodes which has transparent clean filter
                RS_OPTIONAL_TRACE_NAME_FMT("CollectFilterInfoAndUpdateDirty::surfaceNode:%s, add node[%lld] to "
                    "transparentCleanFilter", curSurfaceNode_->GetName().c_str(), node.GetId());
                transparentCleanFilter_[curSurfaceNode_->GetId()].push_back({node.GetId(), globalFilterRect});
                isNodeAddedToTransparentCleanFilters = true;
            }
            if (isIntersect) {
                transparentDirtyFilter_[curSurfaceNode_->GetId()].push_back({node.GetId(), globalFilterRect});
                curDisplayDirtyManager_->MergeDirtyRect(globalFilterRect);
                RS_LOGD("RSUniRenderVisitor::CollectFilterInfoAndUpdateDirty global merge transparentDirtyFilter "
                    "%{public}s, global dirty %{public}s, add rect %{public}s", curSurfaceNode_->GetName().c_str(),
                    curDisplayDirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str(),
                    globalFilterRect.ToString().c_str());
            }
        } else {
            // record surface nodes and nodes in surface which has clean filter
            globalFilter_.insert({node.GetId(), globalFilterRect});
        }
    } else {
        globalFilterRects_.emplace_back(globalFilterRect);
        // record container nodes which need filter
        containerFilter_.insert({node.GetId(), globalFilterRect});
    }
    if (curSurfaceNode_ && !isNodeAddedToTransparentCleanFilters) {
        node.PostPrepareForBlurFilterNode(dirtyManager, needRequestNextVsync_);
    }
}

void RSUniRenderVisitor::UpdateSurfaceRenderNodeScale(RSSurfaceRenderNode& node)
{
    if (!node.IsLeashWindow()) {
        return;
    }
    auto& property = node.GetMutableRenderProperties();
    auto& geoPtr = (property.GetBoundsGeometry());
    if (geoPtr == nullptr) {
        return;
    }
    auto absMatrix = geoPtr->GetAbsMatrix();
    bool isScale = false;
    if (RSMainThread::Instance()->GetDeviceType() == DeviceType::PC) {
        isScale = (!ROSEN_EQ(absMatrix.Get(Drawing::Matrix::SCALE_X), 1.f, EPSILON_SCALE) ||
            !ROSEN_EQ(absMatrix.Get(Drawing::Matrix::SCALE_Y), 1.f, EPSILON_SCALE));
    } else {
        bool getMinMaxScales = false;
        // scaleFactors[0]-minimum scaling factor, scaleFactors[1]-maximum scaling factor
        Drawing::scalar scaleFactors[2];
        getMinMaxScales = absMatrix.GetMinMaxScales(scaleFactors);
        if (getMinMaxScales) {
            isScale = !ROSEN_EQ(scaleFactors[0], 1.f, EPSILON_SCALE) || !ROSEN_EQ(scaleFactors[1], 1.f, EPSILON_SCALE);
        }
        if (!getMinMaxScales) {
            RS_LOGD("getMinMaxScales fail, node:%{public}s %{public}" PRIu64 "", node.GetName().c_str(), node.GetId());
            const auto& dstRect = node.GetDstRect();
            float dstRectWidth = dstRect.GetWidth();
            float dstRectHeight = dstRect.GetHeight();
            float boundsWidth = property.GetBoundsWidth();
            float boundsHeight = property.GetBoundsHeight();
            isScale =
                !ROSEN_EQ(std::min(dstRectWidth, dstRectHeight), std::min(boundsWidth, boundsHeight), EPSILON_SCALE) ||
                !ROSEN_EQ(std::max(dstRectWidth, dstRectHeight), std::max(boundsWidth, boundsHeight), EPSILON_SCALE);
        }
    }
    node.SetIsScaleInPreFrame(node.IsScale());
    node.SetIsScale(isScale);
}

void RSUniRenderVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    RS_TRACE_NAME_FMT("RSUniRender::PrepareRootRenderNode:node[%llu] pid[%d] subTreeDirty[%d]",
        node.GetId(), ExtractPid(node.GetId()), node.IsSubTreeDirty());
    bool dirtyFlag = dirtyFlag_;
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    auto prepareClipRect = prepareClipRect_;

    auto nodeParent = (node.GetParent().lock());
    const auto& property = node.GetRenderProperties();
    bool geoDirty = property.IsGeoDirty();
    auto& geoPtr = (property.GetBoundsGeometry());
    auto prevAlpha = curAlpha_;
    curAlpha_ *= std::clamp(node.GetRenderProperties().GetAlpha(), 0.f, 1.f);

    if (curSurfaceDirtyManager_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::PrepareRootRenderNode curSurfaceDirtyManager is nullptr");
        return;
    }

    dirtyFlag_ = node.UpdateDrawRectAndDirtyRegion(
        *curSurfaceDirtyManager_, dirtyFlag_, prepareClipRect_, parentSurfaceNodeMatrix_);

    if (nodeParent == curSurfaceNode_) {
        const float rootWidth = property.GetFrameWidth() * property.GetScaleX();
        const float rootHeight = property.GetFrameHeight() * property.GetScaleY();
        Drawing::Matrix gravityMatrix;
        (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
            RectF { 0.0f, 0.0f, boundsRect_.GetWidth(), boundsRect_.GetHeight() },
            rootWidth, rootHeight, gravityMatrix);
        // Only Apply gravityMatrix when rootNode is dirty
        if (geoPtr != nullptr && (dirtyFlag || geoDirty)) {
            geoPtr->ConcatMatrix(gravityMatrix);
        }
    }

    if (geoPtr != nullptr) {
        parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    }

    bool isSubTreeNeedPrepare = node.IsSubTreeNeedPrepare(filterInGlobal_) || ForcePrepareSubTree();
    isSubTreeNeedPrepare ? QuickPrepareChildren(node) :
        node.SubTreeSkipPrepare(*curSurfaceDirtyManager_, curDirty_, dirtyFlag_, prepareClipRect_);
    PostPrepare(node, !isSubTreeNeedPrepare);

    curAlpha_ = prevAlpha;
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    dirtyFlag_ = dirtyFlag;
    prepareClipRect_ = prepareClipRect;
}

void RSUniRenderVisitor::ClearRenderGroupCache()
{
    std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
    cacheRenderNodeMap.clear();
}

void RSUniRenderVisitor::ChangeCacheRenderNodeMap(RSRenderNode& node, const uint32_t count)
{
    std::lock_guard<std::mutex> lock(cacheRenderNodeMapMutex);
    cacheRenderNodeMap[node.GetId()] = count;
}

void RSUniRenderVisitor::SetUniRenderThreadParam(std::unique_ptr<RSRenderThreadParams>& renderThreadParams)
{
    if (!renderThreadParams) {
        RS_LOGE("RSUniRenderVisitor::SetUniRenderThreadParam renderThreadParams is nullptr");
        return;
    }
    renderThreadParams->isPartialRenderEnabled_ = isPartialRenderEnabled_;
    renderThreadParams->isRegionDebugEnabled_ = isRegionDebugEnabled_;
    renderThreadParams->isDirtyRegionDfxEnabled_ = isDirtyRegionDfxEnabled_;
    renderThreadParams->isDisplayDirtyDfxEnabled_ = isDisplayDirtyDfxEnabled_;
    renderThreadParams->isOpaqueRegionDfxEnabled_ = isOpaqueRegionDfxEnabled_;
    renderThreadParams->isVisibleRegionDfxEnabled_ = isVisibleRegionDfxEnabled_;
    renderThreadParams->isAllSurfaceVisibleDebugEnabled_ = isAllSurfaceVisibleDebugEnabled_;
    renderThreadParams->isTargetDirtyRegionDfxEnabled_ = isTargetDirtyRegionDfxEnabled_;
    renderThreadParams->dirtyRegionDebugType_ = dirtyRegionDebugType_;
    renderThreadParams->isOpDropped_ = isOpDropped_;
    renderThreadParams->isUIFirstDebugEnable_ = isUIFirstDebugEnable_;
    renderThreadParams->dfxTargetSurfaceNames_ = std::move(dfxTargetSurfaceNames_);
    renderThreadParams->isVirtualDirtyEnabled_ = isVirtualDirtyEnabled_;
    renderThreadParams->isVirtualDirtyDfxEnabled_ = isVirtualDirtyDfxEnabled_;
    renderThreadParams->isExpandScreenDirtyEnabled_ = isExpandScreenDirtyEnabled_;
    renderThreadParams->hasMirrorDisplay_ = hasMirrorDisplay_;
}

void RSUniRenderVisitor::SetAppWindowNum(uint32_t num)
{
    appWindowNum_ = num;
}

void RSUniRenderVisitor::SendRcdMessage(RSDisplayRenderNode& node)
{
    if ((screenInfo_.state == ScreenState::HDI_OUTPUT_ENABLE) &&
        RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        using rcd_msg = RSSingleton<RsMessageBus>;
        rcd_msg::GetInstance().SendMsg<uint32_t, uint32_t>(TOPIC_RCD_DISPLAY_SIZE,
            screenInfo_.width, screenInfo_.height);
        rcd_msg::GetInstance().SendMsg<ScreenRotation>(TOPIC_RCD_DISPLAY_ROTATION,
            node.GetScreenRotation());
        rcd_msg::GetInstance().SendMsg<int>(TOPIC_RCD_DISPLAY_NOTCH,
            RSSystemParameters::GetHideNotchStatus());
    }
}

void RSUniRenderVisitor::ProcessUnpairedSharedTransitionNode()
{
    static auto unpairNode = [](const std::weak_ptr<RSRenderNode>& wptr) {
        auto sptr = wptr.lock();
        if (sptr == nullptr) {
            return;
        }
        // make sure parent regenerates ChildrenDrawable
        auto parent = sptr->GetParent().lock();
        if (parent == nullptr) {
            return;
        }
        parent->AddDirtyType(RSModifierType::CHILDREN);
        parent->ApplyModifiers();
        // avoid changing the paired status or unpairedShareTransitions_
        auto param = sptr->GetSharedTransitionParam();
        if (param == nullptr) {
            ROSEN_LOGE("RSUniRenderVisitor::ProcessUnpairedSharedTransitionNode: param is null");
            return;
        }
        param->paired_ = false;
        SharedTransitionParam::unpairedShareTransitions_.clear();
    };
    auto unpairedShareTransitions = std::move(SharedTransitionParam::unpairedShareTransitions_);
    for (auto& [id, wptr] : unpairedShareTransitions) {
        auto sharedTransitionParam = wptr.lock();
        // If the unpaired share transition is already deal with, do nothing
        if (!sharedTransitionParam || !sharedTransitionParam->paired_) {
            continue;
        }
        ROSEN_LOGE("RSUniRenderVisitor::ProcessUnpairedSharedTransitionNode: mark %s as unpaired",
            sharedTransitionParam->Dump().c_str());
        sharedTransitionParam->paired_ = false;
        unpairNode(sharedTransitionParam->inNode_);
        unpairNode(sharedTransitionParam->outNode_);
    }
}

NodeId RSUniRenderVisitor::FindInstanceChildOfDisplay(std::shared_ptr<RSRenderNode> node)
{
    if (node == nullptr) {
        return INVALID_NODEID;
    }
    auto nodeParent = node->GetParent().lock();
    if (nodeParent == nullptr) {
        return INVALID_NODEID;
    }
    if (nodeParent->GetType() == RSRenderNodeType::DISPLAY_NODE) {
        return node->GetId();
    } else {
        return FindInstanceChildOfDisplay(nodeParent);
    }
}

bool RSUniRenderVisitor::CheckColorFilterChange() const
{
    if (!RSMainThread::Instance()->IsAccessibilityConfigChanged()) {
        return false;
    }
    RS_LOGD("RSUniRenderVisitor::CheckColorFilterChange changed");
    return true;
}

void RSUniRenderVisitor::CheckMergeDebugRectforRefreshRate(std::vector<RSBaseRenderNode::SharedPtr>& surfaces)
{
    // Debug dirtyregion of show current refreshRation
    if (RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled()) {
        RectI tempRect = {100, 100, 500, 200};   // setDirtyRegion for RealtimeRefreshRate
        bool surfaceNodeSet = false;
        for (auto surface : surfaces) {
            auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surface);
            if (surfaceNode->GetName().find(RELIABLE_GESTURE_BACK_SURFACE_NAME) != std::string::npos) {
                // refresh rate rect for mainwindow
                auto& geoPtr = surfaceNode->GetRenderProperties().GetBoundsGeometry();
                tempRect = geoPtr->MapAbsRect(tempRect.ConvertTo<float>());
                curDisplayNode_->GetDirtyManager()->MergeDirtyRect(tempRect, true);
                surfaceNodeSet = true;
                break;
            }
        }
        if (!surfaceNodeSet) {
            auto &geoPtr = curDisplayNode_->GetRenderProperties().GetBoundsGeometry();
            tempRect = geoPtr->MapAbsRect(tempRect.ConvertTo<float>());
            curDisplayNode_->GetDirtyManager()->MergeDirtyRect(tempRect, true);
        }
    }
}

void RSUniRenderVisitor::CheckIsGpuOverDrawBufferOptimizeNode(RSSurfaceRenderNode& node)
{
    bool hasAnim = ancestorNodeHasAnimation_ || node.GetCurFrameHasAnimation();
    if (!node.IsScale() || hasAnim || curCornerRadius_.IsZero() || curAlpha_ < 1) {
        node.SetGpuOverDrawBufferOptimizeNode(false);
        return;
    }

    for (auto& child : *(node.GetChildren())) {
        if (!child || !(child->IsInstanceOf<RSSurfaceRenderNode>() &&
            RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child)->IsLeashOrMainWindow())) {
            continue;
        }
        auto rootNode = child->GetFirstChild();
        if (!rootNode) {
            break;
        }
        auto canvasNode = rootNode->GetFirstChild();
        if (!canvasNode) {
            break;
        }
        const auto& surfaceProperties = node.GetRenderProperties();
        const auto& canvasProperties = canvasNode->GetRenderProperties();
        if (canvasProperties.GetAlpha() >= 1
            && canvasProperties.GetBackgroundColor().GetAlpha() >= MAX_ALPHA
            && canvasProperties.GetFrameWidth() == surfaceProperties.GetFrameWidth()
            && canvasProperties.GetFrameHeight() == surfaceProperties.GetFrameHeight()) {
            node.SetGpuOverDrawBufferOptimizeNode(true);
            node.SetOverDrawBufferNodeCornerRadius(curCornerRadius_);
            return;
        }
    }

    node.SetGpuOverDrawBufferOptimizeNode(false);
}

} // namespace Rosen
} // namespace OHOS
