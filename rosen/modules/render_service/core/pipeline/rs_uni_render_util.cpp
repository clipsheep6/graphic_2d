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

#include "rs_uni_render_util.h"
#include <cstdint>

#include "pipeline/parallel_render/rs_parallel_render_manager.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr const char* ENTRY_VIEW = "EntryView";
}
void RSUniRenderUtil::MergeDirtyHistory(std::shared_ptr<RSDisplayRenderNode>& node, int32_t bufferAge,
    bool useAlignedDirtyRegion)
{
    // update all child surfacenode history
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        if (!surfaceDirtyManager->SetBufferAge(bufferAge)) {
            ROSEN_LOGE("RSUniRenderUtil::MergeVisibleDirtyRegion with invalid buffer age %d", bufferAge);
        }
        surfaceDirtyManager->IntersectDirtyRect(surfaceNode->GetOldDirtyInSurface());
        surfaceDirtyManager->UpdateDirty(useAlignedDirtyRegion);
    }
    // update display dirtymanager
    node->UpdateDisplayDirtyManager(bufferAge, useAlignedDirtyRegion);
}

Occlusion::Region RSUniRenderUtil::MergeVisibleDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node,
    bool useAlignedDirtyRegion)
{
    Occlusion::Region allSurfaceVisibleDirtyRegion;
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow() || surfaceNode->GetDstRect().IsEmpty()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        auto surfaceDirtyRect = surfaceDirtyManager->GetDirtyRegion();
        Occlusion::Rect dirtyRect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
            surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
        auto visibleRegion = surfaceNode->GetVisibleRegion();
        Occlusion::Region surfaceDirtyRegion { dirtyRect };
        Occlusion::Region surfaceVisibleDirtyRegion = surfaceDirtyRegion.And(visibleRegion);
        surfaceNode->SetVisibleDirtyRegion(surfaceVisibleDirtyRegion);
        if (useAlignedDirtyRegion) {
            Occlusion::Region alignedRegion = AlignedDirtyRegion(surfaceVisibleDirtyRegion);
            surfaceNode->SetAlignedVisibleDirtyRegion(alignedRegion);
            allSurfaceVisibleDirtyRegion.OrSelf(alignedRegion);
        } else {
            allSurfaceVisibleDirtyRegion = allSurfaceVisibleDirtyRegion.Or(surfaceVisibleDirtyRegion);
        }
    }
    return allSurfaceVisibleDirtyRegion;
}

void RSUniRenderUtil::SrcRectScaleDown(BufferDrawParam& params, const RSSurfaceRenderNode& node)
{
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    const auto& buffer = node.GetBuffer();
    const auto& surface = node.GetConsumer();
    if (buffer == nullptr || surface == nullptr) {
        return;
    }

    if (surface->GetScalingMode(buffer->GetSeqNum(), scalingMode) == GSERROR_OK &&
        scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        const RSProperties& property = node.GetRenderProperties();
        uint32_t newWidth = static_cast<uint32_t>(params.srcRect.width());
        uint32_t newHeight = static_cast<uint32_t>(params.srcRect.height());
        // Canvas is able to handle the situation when the window is out of screen, using bounds instead of dst.
        uint32_t boundsWidth = static_cast<uint32_t>(property.GetBoundsWidth());
        uint32_t boundsHeight = static_cast<uint32_t>(property.GetBoundsHeight());

        // If transformType is not a multiple of 180, need to change the correspondence between width & height.
        GraphicTransformType transformType = RSBaseRenderUtil::GetRotateTransform(surface->GetTransform());
        if (transformType == GraphicTransformType::GRAPHIC_ROTATE_270 ||
            transformType == GraphicTransformType::GRAPHIC_ROTATE_90) {
            std::swap(boundsWidth, boundsHeight);
        }

        if (newWidth * boundsHeight > newHeight * boundsWidth) {
            // too wide
            newWidth = boundsWidth * newHeight / boundsHeight;
        } else if (newWidth * boundsHeight < newHeight * boundsWidth) {
            // too tall
            newHeight = boundsHeight * newWidth / boundsWidth;
        } else {
            return;
        }

        uint32_t currentWidth = static_cast<uint32_t>(params.srcRect.width());
        uint32_t currentHeight = static_cast<uint32_t>(params.srcRect.height());
        if (newWidth < currentWidth) {
            // the crop is too wide
            uint32_t dw = currentWidth - newWidth;
            auto halfdw = dw / 2;
            params.srcRect = SkRect::MakeXYWH(params.srcRect.left() + static_cast<int32_t>(halfdw),
                                              params.srcRect.top(),
                                              static_cast<int32_t>(newWidth),
                                              params.srcRect.height());
        } else {
            // thr crop is too tall
            uint32_t dh = currentHeight - newHeight;
            auto halfdh = dh / 2;
            params.srcRect = SkRect::MakeXYWH(params.srcRect.left(),
                                              params.srcRect.top() + static_cast<int32_t>(halfdh),
                                              params.srcRect.width(),
                                              static_cast<int32_t>(newHeight));
        }
        RS_LOGD("RsDebug RSUniRenderUtil::SrcRectScaleDown surfaceNode id:%" PRIu64 ", srcRect [%f %f %f %f]",
            node.GetId(), params.srcRect.left(), params.srcRect.top(),
            params.srcRect.width(), params.srcRect.height());
    }
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSSurfaceRenderNode& node, bool forceCPU)
{
    BufferDrawParam params;
#ifdef RS_ENABLE_EGLIMAGE
    params.useCPU = forceCPU;
#else // RS_ENABLE_EGLIMAGE
    params.useCPU = true;
#endif // RS_ENABLE_EGLIMAGE
    params.paint.setAntiAlias(true);
#ifndef NEW_SKIA
    params.paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
#endif

    const RSProperties& property = node.GetRenderProperties();
    params.dstRect = SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight());

    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    if (buffer == nullptr) {
        return params;
    }
    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());

    auto& consumer = node.GetConsumer();
    if (consumer == nullptr) {
        return params;
    }
    auto transform = consumer->GetTransform();
    RectF localBounds = { 0.0f, 0.0f, property.GetBoundsWidth(), property.GetBoundsHeight() };
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, property.GetFrameGravity(), localBounds, params);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    SrcRectScaleDown(params, node);
    return params;
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSDisplayRenderNode& node, bool forceCPU)
{
    BufferDrawParam params;
#ifdef RS_ENABLE_EGLIMAGE
    params.useCPU = forceCPU;
#else // RS_ENABLE_EGLIMAGE
    params.useCPU = true;
#endif // RS_ENABLE_EGLIMAGE
    params.paint.setAntiAlias(true);
#ifndef NEW_SKIA
    params.paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
#endif

    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    params.dstRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    return params;
}

BufferDrawParam RSUniRenderUtil::CreateLayerBufferDrawParam(const LayerInfoPtr& layer, bool forceCPU)
{
    BufferDrawParam params;
#ifdef RS_ENABLE_EGLIMAGE
    params.useCPU = forceCPU;
#else // RS_ENABLE_EGLIMAGE
    params.useCPU = true;
#endif // RS_ENABLE_EGLIMAGE
    params.paint.setAntiAlias(true);
#ifndef NEW_SKIA
    params.paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
#endif
    params.paint.setAlphaf(layer->GetAlpha().gAlpha);

    sptr<SurfaceBuffer> buffer = layer->GetBuffer();
    if (buffer == nullptr) {
        return params;
    }
    params.acquireFence = layer->GetAcquireFence();
    params.buffer = buffer;
    params.srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    auto boundRect = layer->GetBoundSize();
    params.dstRect = SkRect::MakeWH(boundRect.w, boundRect.h);

    auto layerMatrix = layer->GetMatrix();
    params.matrix = SkMatrix::MakeAll(layerMatrix.scaleX, layerMatrix.skewX, layerMatrix.transX,
                                      layerMatrix.skewY, layerMatrix.scaleY, layerMatrix.transY,
                                      layerMatrix.pers0, layerMatrix.pers1, layerMatrix.pers2);

    int nodeRotation = RSUniRenderUtil::GetRotationFromMatrix(params.matrix); // rotation degree anti-clockwise
    auto layerTransform = layer->GetTransformType();
    // calculate clockwise rotation degree excluded rotation in total matrix
    int realRotation = (nodeRotation +
        RSBaseRenderUtil::RotateEnumToInt(RSBaseRenderUtil::GetRotateTransform(layerTransform))) % 360;
    auto flip = RSBaseRenderUtil::GetFlipTransform(layerTransform);
    // calculate transform in anti-clockwise
    auto transform = RSBaseRenderUtil::RotateEnumToInt(realRotation, flip);

    RectF localBounds = { 0.0f, 0.0f, static_cast<float>(boundRect.w), static_cast<float>(boundRect.h) };
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, Gravity::RESIZE, localBounds, params);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    return params;
}

void RSUniRenderUtil::DrawCachedImage(RSSurfaceRenderNode& node, RSPaintFilterCanvas& canvas, sk_sp<SkImage> image)
{
    if (image == nullptr) {
        return;
    }
    canvas.save();
    canvas.scale(node.GetRenderProperties().GetBoundsWidth() / image->width(),
        node.GetRenderProperties().GetBoundsHeight() / image->height());
    SkPaint paint;
#ifdef NEW_SKIA
    canvas.drawImage(image.get(), 0.0, 0.0, SkSamplingOptions(), &paint);
#else
    canvas.drawImage(image.get(), 0.0, 0.0, &paint);
#endif
    canvas.restore();
}

Occlusion::Region RSUniRenderUtil::AlignedDirtyRegion(const Occlusion::Region& dirtyRegion, int32_t alignedBits)
{
    Occlusion::Region alignedRegion;
    if (alignedBits <= 1) {
        return dirtyRegion;
    }
    for (const auto& dirtyRect : dirtyRegion.GetRegionRects()) {
        int32_t left = (dirtyRect.left_ / alignedBits) * alignedBits;
        int32_t top = (dirtyRect.top_ / alignedBits) * alignedBits;
        int32_t width = ((dirtyRect.right_ + alignedBits - 1) / alignedBits) * alignedBits - left;
        int32_t height = ((dirtyRect.bottom_ + alignedBits - 1) / alignedBits) * alignedBits - top;
        Occlusion::Rect rect = { left, top, left + width, top + height };
        Occlusion::Region singleAlignedRegion(rect);
        alignedRegion.OrSelf(singleAlignedRegion);
    }
    return alignedRegion;
}

bool RSUniRenderUtil::HandleSubThreadNode(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (node.IsMainThreadNode()) {
        return false;
    }
    if (!node.HasCachedTexture()) {
        RS_TRACE_NAME_FMT("HandleSubThreadNode wait %" PRIu64 "", node.GetId());
        RSParallelRenderManager::Instance()->WaitNodeTask(node.GetId());
        node.UpdateCompletedCacheSurface();
        RSParallelRenderManager::Instance()->SaveCacheTexture(node);
    }
    RS_TRACE_NAME_FMT("RSUniRenderUtil::HandleSubThreadNode %" PRIu64 "", node.GetId());
    node.DrawCacheSurface(canvas, true);
    return true;
}

int RSUniRenderUtil::GetRotationFromMatrix(SkMatrix matrix)
{
    float value[9];
    matrix.get9(value);

    int rAngle = static_cast<int>(-round(atan2(value[SkMatrix::kMSkewX], value[SkMatrix::kMScaleX]) * (180 / PI)));
    // transfer the result to anti-clockwise degrees
    // only rotation with 90°, 180°, 270° are composed through hardware,
    // in which situation the transformation of the layer needs to be set.
    static const std::map<int, int> supportedDegrees = {{90, 270}, {180, 180}, {-90, 90}, {-180, 180}};
    auto iter = supportedDegrees.find(rAngle);
    return iter != supportedDegrees.end() ? iter->second : 0;
}

void RSUniRenderUtil::AssignWindowNodes(const std::shared_ptr<RSDisplayRenderNode>& displayNode, uint64_t focusNodeId,
    std::list<std::shared_ptr<RSSurfaceRenderNode>>& mainThreadNodes,
    std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes,
    const std::unordered_map<NodeId, bool>& cacheSkippedNodeMap)
{
    if (displayNode == nullptr) {
        ROSEN_LOGE("RSUniRenderUtil::AssignWindowNodes display node is null");
        return;
    }
    RS_TRACE_NAME("AssignWindowNodes");
    std::shared_ptr<RSSurfaceRenderNode> entryViewNode = nullptr;
    bool entryViewNeedReassign = false;
    for (auto iter = displayNode->GetSortedChildren().begin(); iter != displayNode->GetSortedChildren().end(); iter++) {
        auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*iter);
        if (node == nullptr || !node->ShouldPaint()) {
            ROSEN_LOGE("RSUniRenderUtil::AssignWindowNodes nullptr found in sortedChildren, this should not happen");
            continue;
        }
        if (node->GetName() == ENTRY_VIEW) {
            entryViewNode = node;
            continue;
        }
        if (node->IsLeashWindow() && node->IsScale()) {
            entryViewNeedReassign = true;
        }
        bool isFocusNode = false;
        if (node->GetId() == focusNodeId) {
            isFocusNode = true;
        }
        if (!isFocusNode && node->IsLeashWindow()) {
            for (auto& child : node->GetSortedChildren()) {
                if (child && child->GetId() == focusNodeId) {
                    isFocusNode = true;
                    break;
                }
            }
        }
        if (node->HasFilter() || (!node->IsScale() && isFocusNode)) {
            AssignMainThreadNode(mainThreadNodes, node);
        } else {
            AssignSubThreadNode(subThreadNodes, node, cacheSkippedNodeMap);
        }
    }
    if (entryViewNode) {
        if (entryViewNeedReassign) {
            AssignMainThreadNode(mainThreadNodes, entryViewNode);
        } else {
            if (entryViewNode->HasFilter() || (!entryViewNode->IsScale() && entryViewNode->GetId() == focusNodeId)) {
                AssignMainThreadNode(mainThreadNodes, entryViewNode);
            } else {
                AssignSubThreadNode(subThreadNodes, entryViewNode, cacheSkippedNodeMap);
            }
        }
    } else {
        ROSEN_LOGW("RSUniRenderUtil::AssignWindowNodes EntryView is nullptr, should not happen");
    }
    SortSubThreadNodes(subThreadNodes);
}

void RSUniRenderUtil::AssignMainThreadNode(std::list<std::shared_ptr<RSSurfaceRenderNode>>& mainThreadNodes,
    const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node == nullptr) {
        ROSEN_LOGW("RSUniRenderUtil::AssignMainThreadNode node is nullptr");
        return;
    }
    mainThreadNodes.emplace_back(node);
    node->SetIsMainThreadNode(true);
    node->SetCacheType(CacheType::NONE);
    node->ClearCacheSurface();
}

void RSUniRenderUtil::AssignSubThreadNode(std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes,
    const std::shared_ptr<RSSurfaceRenderNode>& node, const std::unordered_map<NodeId, bool>& cacheSkippedNodeMap)
{
    if (node == nullptr) {
        ROSEN_LOGW("RSUniRenderUtil::AssignSubThreadNode node is nullptr");
        return;
    }
    subThreadNodes.emplace_back(node);
    node->SetIsMainThreadNode(false);
    node->UpdateCacheSurfaceDirtyManager(2); // 2 means buffer age
    node->SetCacheType(CacheType::ANIMATE_PROPERTY);
#ifdef RS_ENABLE_GL
    if (cacheSkippedNodeMap.count(node->GetId()) == 0 && node->GetCacheSurface()) {
        node->UpdateCompletedCacheSurface();
        RSParallelRenderManager::Instance()->SaveCacheTexture(*node);
    }
#endif
    if (node->HasCachedTexture()) {
        node->SetPriority(NodePriorityType::SUB_LOW_PRIORITY);
    } else {
        node->SetPriority(NodePriorityType::SUB_HIGH_PRIORITY);
    }
}

void RSUniRenderUtil::SortSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes)
{
    // sort subThreadNodes by priority and z-order
    subThreadNodes.sort([](const auto& first, const auto& second) -> bool {
        auto node1 = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(first);
        auto node2 = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(second);
        if (node1 == nullptr || node2 == nullptr) {
            ROSEN_LOGE(
                "RSUniRenderUtil::AssignWindowNodes sort nullptr found in subThreadNodes, this should not happen");
            return false;
        }
        if (node1->GetPriority() == node2->GetPriority()) {
            return node2->GetRenderProperties().GetPositionZ() < node1->GetRenderProperties().GetPositionZ();
        } else {
            return node1->GetPriority() < node2->GetPriority();
        }
    });
}
} // namespace Rosen
} // namespace OHOS
