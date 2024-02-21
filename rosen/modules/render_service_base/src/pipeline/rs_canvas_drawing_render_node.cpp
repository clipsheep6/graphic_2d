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

#include "pipeline/rs_canvas_drawing_render_node.h"

#include "include/core/SkCanvas.h"
#include "src/image/SkImage_Base.h"
#ifdef NEW_SKIA
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#endif

#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "visitor/rs_node_visitor.h"
#include "rs_trace.h"
namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t DRAWCMDLIST_COUNT_LIMIT = 10;
}
RSCanvasDrawingRenderNode::RSCanvasDrawingRenderNode(NodeId id, const std::weak_ptr<RSContext>& context,
    bool isTextureExportNode) : RSCanvasRenderNode(id, context, isTextureExportNode)
{}

RSCanvasDrawingRenderNode::~RSCanvasDrawingRenderNode()
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifndef USE_ROSEN_DRAWING
    if (preThreadInfo_.second && skSurface_) {
        preThreadInfo_.second(std::move(skSurface_));
    }
#else
    if (preThreadInfo_.second && surface_) {
        preThreadInfo_.second(std::move(surface_));
    }
#endif
#endif
}

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifndef USE_ROSEN_DRAWING
bool RSCanvasDrawingRenderNode::ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas)
{
    auto preMatrix = canvas_->getTotalMatrix();
    auto preSurface = skSurface_;
    if (!ResetSurface(width, height, canvas)) {
        return false;
    }
    auto image = preSurface->makeImageSnapshot();
    if (!image) {
        return false;
    }
    GrSurfaceOrigin origin = kBottomLeft_GrSurfaceOrigin;
    auto sharedBackendTexture = image->getBackendTexture(false, &origin);
    if (!sharedBackendTexture.isValid()) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedBackendTexture is nullptr");
        return false;
    }
    auto sharedTexture = SkImage::MakeFromTexture(
        canvas.recordingContext(), sharedBackendTexture, origin, image->colorType(), image->alphaType(), nullptr);
    if (sharedTexture == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedTexture is nullptr");
        return false;
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (sharedTexture->isTextureBacked()) {
            RS_LOGI("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedTexture from texture to raster image");
            sharedTexture = sharedTexture->makeRasterImage();
        }
    }
    canvas_->drawImage(sharedTexture, 0.f, 0.f);
    if (preThreadInfo_.second && preSurface) {
        preThreadInfo_.second(std::move(preSurface));
    }
    preThreadInfo_ = curThreadInfo_;
    canvas_->setMatrix(preMatrix);
    canvas_->flush();
    return true;
}
#else
bool RSCanvasDrawingRenderNode::ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas)
{
    auto preMatrix = canvas_->GetTotalMatrix();
    auto preSurface = surface_;
    if (!ResetSurface(width, height, canvas)) {
        return false;
    }
    auto image = preSurface->GetImageSnapshot();
    if (!image) {
        return false;
    }
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto sharedBackendTexture = image->GetBackendTexture(false, &origin);
    if (!sharedBackendTexture.IsValid()) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedBackendTexture is nullptr");
        return false;
    }

    Drawing::BitmapFormat bitmapFormat = { image->GetColorType(), image->GetAlphaType() };
    auto sharedTexture = std::make_shared<Drawing::Image>();
    if (!sharedTexture->BuildFromTexture(*canvas.GetGPUContext(), sharedBackendTexture.GetTextureInfo(),
        origin, bitmapFormat, nullptr)) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedTexture is nullptr");
        return false;
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (sharedTexture->IsTextureBacked()) {
            RS_LOGI("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedTexture from texture to raster image");
            sharedTexture = sharedTexture->MakeRasterImage();
        }
    }
    canvas_->DrawImage(*sharedTexture, 0.f, 0.f, Drawing::SamplingOptions());
    if (preThreadInfo_.second && preSurface) {
        preThreadInfo_.second(std::move(preSurface));
    }
    preThreadInfo_ = curThreadInfo_;
    canvas_->SetMatrix(preMatrix);
    canvas_->Flush();
    return true;
}
#endif
#endif

#ifndef USE_ROSEN_DRAWING
void RSCanvasDrawingRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
    int width = 0;
    int height = 0;
    if (!GetSizeFromDrawCmdModifiers(width, height)) {
        return;
    }
    if (IsNeedResetSurface()) {
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
        if (preThreadInfo_.second && skSurface_) {
            preThreadInfo_.second(std::move(skSurface_));
        }
        preThreadInfo_ = curThreadInfo_;
#endif
        if (!ResetSurface(width, height, canvas)) {
            return;
        }
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    } else if ((isGpuSurface_) && (preThreadInfo_.first != curThreadInfo_.first)) {
        if (!ResetSurfaceWithTexture(width, height, canvas)) {
            return;
        }
    }
#else
    }
#endif
    if (!skSurface_) {
        return;
    }

    RSModifierContext context = { GetMutableRenderProperties(), canvas_.get() };
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);
    ApplyDrawCmdModifier(context, RSModifierType::OVERLAY_STYLE);

    SkMatrix mat;
    if (RSPropertiesPainter::GetGravityMatrix(
            GetRenderProperties().GetFrameGravity(), GetRenderProperties().GetFrameRect(), width, height, mat)) {
        canvas.concat(mat);
    }
    if (!recordingCanvas_) {
        skImage_ = skSurface_->makeImageSnapshot();
        if (skImage_) {
#ifndef ROSEN_ARKUI_X
            SKResourceManager::Instance().HoldResource(skImage_);
#endif
        }
    } else {
        auto cmds = recordingCanvas_->GetDrawCmdList();
        if (cmds && cmds->GetSize() > 0) {
            recordingCanvas_ = std::make_shared<RSRecordingCanvas>(width, height);
            canvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
            ProcessCPURenderInBackgroundThread(cmds);
        }
    }
    std::lock_guard<std::mutex> lock(imageMutex_);
    if (!skImage_) {
        return;
    }
    auto samplingOptions = SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kLinear);
    if (canvas.GetRecordingState()) {
        auto cpuImage = skImage_->makeRasterImage();
        canvas.drawImage(cpuImage, 0.f, 0.f, samplingOptions, nullptr);
        return;
    }
    canvas.drawImage(skImage_, 0.f, 0.f, samplingOptions, nullptr);
}

void RSCanvasDrawingRenderNode::ProcessCPURenderInBackgroundThread(std::shared_ptr<DrawCmdList> cmds)
{
    auto surface = skSurface_;
    auto nodeId = GetId();
    auto ctx = GetContext().lock();
    RSBackgroundThread::Instance().PostTask([cmds, surface, nodeId, ctx]() {
        if (!cmds || cmds->GetSize() == 0 || !surface || !ctx) {
            return;
        }
        auto node = ctx->GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId);
        if (!node || surface != node->skSurface_) {
            return;
        }
        cmds->Playback(*surface->getCanvas());
        auto skImage = surface->makeImageSnapshot();
        if (skImage) {
#ifndef ROSEN_ARKUI_X
            SKResourceManager::Instance().HoldResource(skImage);
#endif
        }
        std::lock_guard<std::mutex> lock(node->imageMutex_);
        node->skImage_ = skImage;
        ctx->PostTask([ctx, nodeId]() {
            if (auto node = ctx->GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId)) {
                node->SetDirty();
                ctx->RequestVsync();
            }
        });
    });
}

#else // USE_ROSEN_DRAWING
void RSCanvasDrawingRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
    int width = 0;
    int height = 0;
    RS_TRACE_NAME_FMT("RSCanvasDrawingRenderNode::ProcessRenderContents %llu", GetId());
    if (!GetSizeFromDrawCmdModifiers(width, height)) {
        return;
    }

    if (IsNeedResetSurface()) {
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
        if (preThreadInfo_.second && surface_) {
            preThreadInfo_.second(std::move(surface_));
        }
        preThreadInfo_ = curThreadInfo_;
#endif
        if (!ResetSurface(width, height, canvas)) {
            return;
        }
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
    } else if ((isGpuSurface_) && (preThreadInfo_.first != curThreadInfo_.first)) {
        if (!ResetSurfaceWithTexture(width, height, canvas)) {
            return;
        }
    }
#else
    }
#endif
    if (!surface_) {
        return;
    }

    RSModifierContext context = { GetMutableRenderProperties(), canvas_.get() };
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);
    ApplyDrawCmdModifier(context, RSModifierType::OVERLAY_STYLE);

    Rosen::Drawing::Matrix mat;
    if (RSPropertiesPainter::GetGravityMatrix(
        GetRenderProperties().GetFrameGravity(), GetRenderProperties().GetFrameRect(), width, height, mat)) {
        canvas.ConcatMatrix(mat);
    }
    if (!recordingCanvas_) {
        image_ = surface_->GetImageSnapshot();
        if (image_) {
            SKResourceManager::Instance().HoldResource(image_);
        }
    } else {
        auto cmds = recordingCanvas_->GetDrawCmdList();
        if (cmds && !cmds->IsEmpty()) {
            recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(width, height, false);
            canvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
            ProcessCPURenderInBackgroundThread(cmds);
        }
    }
    std::lock_guard<std::mutex> lock(imageMutex_);
    if (!image_) {
        return;
    }
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    Drawing::Paint paint;
    paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
    canvas.AttachPaint(paint);
    if (canvas.GetRecordingState()) {
        auto cpuImage = image_->MakeRasterImage();
        canvas.DrawImage(*cpuImage, 0.f, 0.f, samplingOptions);
    } else {
        canvas.DrawImage(*image_, 0.f, 0.f, samplingOptions);
    }
    canvas.DetachPaint();
}

void RSCanvasDrawingRenderNode::ProcessCPURenderInBackgroundThread(std::shared_ptr<Drawing::DrawCmdList> cmds)
{
    auto surface = surface_;
    auto nodeId = GetId();
    auto ctx = GetContext().lock();
    RSBackgroundThread::Instance().PostTask([cmds, surface, nodeId, ctx]() {
        if (!cmds || cmds->IsEmpty() || !surface || !ctx) {
            return;
        }
        auto node = ctx->GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId);
        if (!node || surface != node->surface_) {
            return;
        }
        cmds->Playback(*surface->GetCanvas());
        auto image = surface->GetImageSnapshot();
        if (image) {
            SKResourceManager::Instance().HoldResource(image);
        }
        std::lock_guard<std::mutex> lock(node->imageMutex_);
        node->image_ = image;
        ctx->PostTask([ctx, nodeId]() {
            if (auto node = ctx->GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId)) {
                node->SetDirty();
                ctx->RequestVsync();
            }
        });
    });
}
#endif

#ifndef USE_ROSEN_DRAWING
bool RSCanvasDrawingRenderNode::ResetSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    SkImageInfo info = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifdef NEW_SKIA
    auto grContext = canvas.recordingContext();
#else
    auto grContext = canvas.getGrContext();
#endif
    isGpuSurface_ = true;
    if (grContext == nullptr) {
        RS_LOGD("RSCanvasDrawingRenderNode::ResetSurface: GrContext is nullptr");
        isGpuSurface_ = false;
        skSurface_ = SkSurface::MakeRaster(info);
    } else {
        skSurface_ = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kNo, info);
        if (!skSurface_) {
            isGpuSurface_ = false;
            skSurface_ = SkSurface::MakeRaster(info);
            if (!skSurface_) {
                RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface SkSurface is nullptr");
                return false;
            }
            recordingCanvas_ = std::make_shared<RSRecordingCanvas>(width, height);
            canvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
            return true;
        }
    }

#else
    skSurface_ = SkSurface::MakeRaster(info);
#endif
    if (!skSurface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface SkSurface is nullptr");
        return false;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(skSurface_.get());
    return true;
}
#else
bool RSCanvasDrawingRenderNode::ResetSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    Drawing::ImageInfo info =
        Drawing::ImageInfo{ width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    auto gpuContext = canvas.GetGPUContext();
    isGpuSurface_ = true;
    if (gpuContext == nullptr) {
        RS_LOGD("RSCanvasDrawingRenderNode::ResetSurface: gpuContext is nullptr");
        isGpuSurface_ = false;
        surface_ = Drawing::Surface::MakeRaster(info);
    } else {
        surface_ = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
        if (!surface_) {
            isGpuSurface_ = false;
            surface_ = Drawing::Surface::MakeRaster(info);
            if (!surface_) {
                RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface surface is nullptr");
                return false;
            }
            recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(width, height, false);
            canvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
            return true;
        }
    }
#else
    surface_ = Drawing::Surface::MakeRaster(info);
#endif
    if (!surface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface surface is nullptr");
        return false;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface_.get());
    return true;
}
#endif

void RSCanvasDrawingRenderNode::ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type)
{
    std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
    auto it = drawCmdLists_.find(type);
    if (it == drawCmdLists_.end() || it->second.empty()) {
        return;
    }
    for (const auto& drawCmdList : it->second) {
        drawCmdList->Playback(*context.canvas_);
        drawCmdList->ClearOp();
    }
    it->second.clear();
}

bool WriteSkImageToPixelmap(std::shared_ptr<Drawing::Image> image, Drawing::ImageInfo info,
    std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect)
{
    return image->ReadPixels(
        info, pixelmap->GetWritablePixels(), pixelmap->GetRowStride(), rect->GetLeft(), rect->GetTop());
}

#ifndef USE_ROSEN_DRAWING
SkBitmap RSCanvasDrawingRenderNode::GetBitmap(const uint32_t tid)
{
    RS_LOGD("RSCanvasDrawingRenderNode::GetBitmap");
    SkBitmap bitmap;
    std::lock_guard<std::mutex> lock(drawingMutex_);
    if (!skImage_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: skImage_ is nullptr");
        return bitmap;
    }
    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: skImage_ used by multi threads");
        return bitmap;
    }
    if (!skImage_->asLegacyBitmap(&bitmap)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: asLegacyBitmap failed");
    }
    return bitmap;
}

bool RSCanvasDrawingRenderNode::GetPixelmap(std::shared_ptr<Media::PixelMap> pixelmap, const SkRect* rect,
    const uint32_t tid, std::shared_ptr<DrawCmdList> drawCmdList)
{
    RS_LOGD("RSCanvasDrawingRenderNode::GetPixelmap");
    std::lock_guard<std::mutex> lock(drawingMutex_);
    if (!pixelmap || !rect) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: pixelmap or rect is nullptr");
        return false;
    }

    if (!skSurface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: SkSurface is nullptr");
        return false;
    }

    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: SkSurface used by multi threads");
        return false;
    }

    sk_sp<SkImage> skImage = skSurface_->makeImageSnapshot();
    if (skImage == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: makeImageSnapshot failed");
        return false;
    }

    SkImageInfo info =
        SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    if (!skImage->readPixels(info, pixelmap->GetWritablePixels(), pixelmap->GetRowBytes(), rect->x(), rect->y())) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
        return false;
    }
    return true;
}

#else
Drawing::Bitmap RSCanvasDrawingRenderNode::GetBitmap(const uint64_t tid)
{
    Drawing::Bitmap bitmap;
    std::lock_guard<std::mutex> lock(drawingMutex_);
    if (!image_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: image_ is nullptr");
        return bitmap;
    }
    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: image_ used by multi threads");
        return bitmap;
    }
    if (!image_->AsLegacyBitmap(bitmap)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: asLegacyBitmap failed");
    }
    return bitmap;
}

bool RSCanvasDrawingRenderNode::GetPixelmap(std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect,
    const uint64_t tid, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    std::lock_guard<std::mutex> lock(drawingMutex_);
    if (!pixelmap || !rect) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: pixelmap is nullptr");
        return false;
    }

    if (!surface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: surface is nullptr");
        return false;
    }

    auto image = surface_->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: GetImageSnapshot failed");
        return false;
    }
    
    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: surface used by multi threads");
        return false;
    }

    Drawing::ImageInfo info = Drawing::ImageInfo{ pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    if (!drawCmdList) {
        if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
            RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
            return false;
        }
        return true;
    }
    std::shared_ptr<Drawing::Surface> surface;
    std::unique_ptr<RSPaintFilterCanvas> canvas;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    auto gpuContext = canvas_->GetGPUContext();
    if (gpuContext == nullptr) {
        if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
            RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
        }
        return false;
    } else {
        surface = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
        if (!surface) {
            if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
                RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
            }
            return false;
        }
        canvas = std::make_unique<RSPaintFilterCanvas>(surface.get());
    }
#else
    if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
    }
    return false;
#endif
    canvas->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    drawCmdList->Playback(*canvas, rect);
    auto pixelmapImage = surface->GetImageSnapshot();
    if (!WriteSkImageToPixelmap(pixelmapImage, info, pixelmap, rect)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
        return false;
    }
    return true;
}
#endif

bool RSCanvasDrawingRenderNode::GetSizeFromDrawCmdModifiers(int& width, int& height)
{
    auto it = GetDrawCmdModifiers().find(RSModifierType::CONTENT_STYLE);
    if (it == GetDrawCmdModifiers().end() || it->second.empty()) {
        return false;
    }
    for (const auto& modifier : it->second) {
        auto prop = modifier->GetProperty();
#ifndef USE_ROSEN_DRAWING
        if (auto cmd = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(prop)->Get()) {
#else
        if (auto cmd = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop)->Get()) {
#endif
            width = std::max(width, cmd->GetWidth());
            height = std::max(height, cmd->GetHeight());
        }
    }
    if (width <= 0 || height <= 0) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetSizeFromDrawCmdModifiers: The width or height of the canvas is less "
                "than or equal to 0");
        return false;
    }
    return true;
}

bool RSCanvasDrawingRenderNode::IsNeedResetSurface() const
{
#ifndef USE_ROSEN_DRAWING
    if (!skSurface_) {
#else
    if (!surface_ || !surface_->GetCanvas()) {
#endif
        return true;
    }
    return false;
}

void RSCanvasDrawingRenderNode::AddDirtyType(RSModifierType type)
{
    dirtyTypes_.set(static_cast<int>(type), true);
    for (auto& drawCmdModifier : GetDrawCmdModifiers()) {
        if (drawCmdModifier.second.empty()) {
            continue;
        }
        for (const auto& modifier : drawCmdModifier.second) {
            if (modifier == nullptr) {
                continue;
            }
            auto prop = modifier->GetProperty();
            if (prop == nullptr) {
                continue;
            }
#ifndef USE_ROSEN_DRAWING
            if (auto cmd = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(prop)->Get()) {
#else
            if (auto cmd = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(prop)->Get()) {
#endif
                std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
                drawCmdLists_[drawCmdModifier.first].emplace_back(cmd);
                // If such nodes are not drawn, The drawcmdlists don't clearOp during recording, As a result, there are
                // too many drawOp, so we need to add the limit of drawcmdlists.
                while (GetOldDirtyInSurface().IsEmpty() &&
                    drawCmdLists_[drawCmdModifier.first].size() > DRAWCMDLIST_COUNT_LIMIT) {
                    RS_LOGI("This Node[%{public}" PRIu64 "] with Modifier[%{public}hd] have drawcmdlist:%{public}zu",
                        GetId(), drawCmdModifier.first, drawCmdLists_[drawCmdModifier.first].size());
                    drawCmdLists_[drawCmdModifier.first].pop_front();
                }
            }
        }
    }
}

void RSCanvasDrawingRenderNode::ClearOp()
{
    std::lock_guard<std::mutex> lock(drawCmdListsMutex_);
    drawCmdLists_.clear();
}

void RSCanvasDrawingRenderNode::ResetSurface()
{
    if (preThreadInfo_.second && surface_) {
        preThreadInfo_.second(std::move(surface_));
    }
    surface_ = nullptr;
    recordingCanvas_ = nullptr;
}
} // namespace Rosen
} // namespace OHOS