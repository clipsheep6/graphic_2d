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

#include "pipeline/rs_divided_ui_capture.h"

#include <memory>
#include <mutex>
#include <sys/mman.h>

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkMatrix.h"
#else
#include "utils/matrix.h"
#endif

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "render/rs_pixel_map_util.h"
#include "transaction/rs_render_service_client.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<Media::PixelMap> RSDividedUICapture::TakeLocalCapture()
{
    if (ROSEN_EQ(scaleX_, 0.f) || ROSEN_EQ(scaleY_, 0.f) || scaleX_ < 0.f || scaleY_ < 0.f) {
        ROSEN_LOGE("RSDividedUICapture::TakeLocalCapture: scale is invalid.");
        return nullptr;
    }
    auto node = RSRenderThread::Instance().GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(nodeId_);
    if (node == nullptr) {
        ROSEN_LOGE("RSDividedUICapture::TakeLocalCapture node is nullptr return");
        return nullptr;
    }
    node->ApplyModifiers();
    std::shared_ptr<Media::PixelMap> pixelmap = CreatePixelMapByNode(node);
    if (pixelmap == nullptr) {
        ROSEN_LOGE("RSDividedUICapture::TakeLocalCapture: pixelmap == nullptr!");
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    auto skSurface = CreateSurface(pixelmap);
    if (skSurface == nullptr) {
        return nullptr;
    }
#else
    auto drSurface = CreateSurface(pixelmap);
    if (drSurface == nullptr) {
        return nullptr;
    }
#endif
    std::shared_ptr<RSDividedUICaptureVisitor> visitor =
        std::make_shared<RSDividedUICaptureVisitor>(nodeId_, scaleX_, scaleY_);
    visitor->SetSurface(skSurface.get());
    if (!node->IsOnTheTree()) {
        node->Prepare(visitor);
    }
    node->Process(visitor);   
#ifdef RS_ENABLE_GL
    sk_sp<SkImage> img(skSurface.get()->makeImageSnapshot());
    if (!img) {
        RS_LOGE("RSDividedUICapture::TakeLocalCapture: img is nullptr");
        return nullptr;
    }
    if (!CopyDataToPixelMap(img, pixelmap)) {
        RS_LOGE("RSDividedUICapture::TakeLocalCapture: CopyDataToPixelMap failed");
        return nullptr;
    }
#endif
    return pixelmap;
}

bool RSDividedUICapture::CopyDataToPixelMap(sk_sp<SkImage> img, const std::shared_ptr<Media::PixelMap>& pixelmap)
{
    auto size = pixelmap->GetRowBytes() * pixelmap->GetHeight();
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
#ifdef ROSEN_OHOS
    int fd = AshmemCreate("RSDividedUICapture Data", size);
    if (fd < 0) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap AshmemCreate fd < 0");
        return false;
    }
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap AshmemSetProt error");
        ::close(fd);
        return false;
    }
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    auto data = static_cast<uint8_t*>(ptr);
    if (ptr == MAP_FAILED || ptr == nullptr) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap data is nullptr");
        ::close(fd);
        return false;
    }

    if (!img->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap readPixels failed");
        ::close(fd);
        return false;
    }
    void* fdPtr = new int32_t();
    *static_cast<int32_t*>(fdPtr) = fd;
    pixelmap->SetPixelsAddr(data, fdPtr, size, Media::AllocatorType::SHARE_MEM_ALLOC, nullptr);
#else
    auto data = (uint8_t *)malloc(size);
    if (data == nullptr) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap data is nullptr");
        return false;
    }
    if (!img->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap readPixels failed");
        free(data);
        data = nullptr;
        return false;
    }
    pixelmap->SetPixelsAddr(data, nullptr, size, Media::AllocatorType::HEAP_ALLOC, nullptr);
#endif
    return true;
}

std::shared_ptr<Media::PixelMap> RSDividedUICapture::CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node) const
{
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    return Media::PixelMap::Create(opts);
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkSurface> RSDividedUICapture::CreateSurface(const std::shared_ptr<Media::PixelMap>& pixelmap) const
#else
std::shared_ptr<Drawing::Surface> RSDividedUICapture::CreateSurface(
    const std::shared_ptr<Media::PixelMap>& pixelmap) const
#endif
{
    if (pixelmap == nullptr) {
        ROSEN_LOGE("RSDividedUICapture::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        ROSEN_LOGE("RSDividedUICapture::CreateSurface: address == nullptr");
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
#ifdef RS_ENABLE_GL
    auto renderContext = RSRenderThread::Instance().GetRenderContext();
    if (renderContext == nullptr) {
        ROSEN_LOGE("RSDividedUICapture::CreateSurface: renderContext == nullptr");
        return nullptr;
    }
    return SkSurface::MakeRenderTarget(renderContext->GetGrContext(), SkBudgeted::kNo, info);
#else
    return SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
#endif
#else
    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format);
    bitmap.SetPixels(address);

    auto surface = std::make_shared<Drawing::Surface>();
    surface->Bind(bitmap);
    return surface;
#endif
}

void RSDividedUICapture::RSDividedUICaptureVisitor::SetSurface(SkSurface* surface)
{
    if (surface == nullptr) {
        RS_LOGE("RSDividedUICaptureVisitor::SetSurface: surface == nullptr");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface);
    canvas_->scale(scaleX_, scaleY_);
    canvas_->SetDisableFilterCache(true);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessChildren(RSRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
}

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!node.ShouldPaint()) {
        ROSEN_LOGD("RSDividedUICaptureVisitor::ProcessRootRenderNode, no need process");
        return;
    }

    if (!canvas_) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::ProcessRootRenderNode, canvas is nullptr");
        return;
    }

#ifndef USE_ROSEN_DRAWING
    canvas_->save();
    ProcessCanvasRenderNode(node);
    canvas_->restore();
#else
    canvas_->Save();
    ProcessCanvasRenderNode(node);
    canvas_->Restore();
#endif
}

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.ShouldPaint()) {
        ROSEN_LOGD("RSDividedUICaptureVisitor::ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    if (node.GetId() == nodeId_) {
        // When drawing nodes, canvas will offset the bounds value, so we will move in reverse here first
        const auto& property = node.GetRenderProperties();
        auto geoPtr = (property.GetBoundsGeometry());
#ifndef USE_ROSEN_DRAWING
        SkMatrix relativeMatrix = SkMatrix::I();
        relativeMatrix.setScaleY(scaleX_);
        relativeMatrix.setScaleY(scaleY_);
        SkMatrix invertMatrix;
        if (geoPtr->GetMatrix().invert(&invertMatrix)) {
            relativeMatrix.preConcat(invertMatrix);
        }
        canvas_->setMatrix(relativeMatrix);
#else
        Drawing::Matrix relativeMatrix;
        relativeMatrix.Set(Drawing::Matrix::SCALE_X, scaleX_);
        relativeMatrix.Set(Drawing::Matrix::SCALE_Y, scaleY_);
        Drawing::Matrix invertMatrix;
        if (geoPtr->GetMatrix().Invert(invertMatrix)) {
            relativeMatrix.PreConcat(invertMatrix);
        }
        canvas_->SetMatrix(relativeMatrix);
#endif
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto canvasDrawingNode = node.ReinterpretCastTo<RSCanvasDrawingRenderNode>();
        if (!canvasDrawingNode->IsOnTheTree()) {
            canvasDrawingNode->ProcessRenderContents(*canvas_);
        } else {
#ifndef USE_ROSEN_DRAWING
            SkBitmap bitmap = canvasDrawingNode->GetBitmap();
#ifndef NEW_SKIA
            canvas_->drawBitmap(bitmap, 0, 0);
#else
            canvas_->drawImage(bitmap.asImage(), 0, 0);
#endif
#else
            Drawing::Bitmap bitmap = canvasDrawingNode->GetBitmap();
            canvas_->DrawBitmap(bitmap, 0, 0);
#endif
        }
    } else {
        node.ProcessRenderContents(*canvas_);
    }
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessEffectRenderNode(RSEffectRenderNode& node)
{
    if (!node.ShouldPaint()) {
        RS_LOGD("RSDividedUICapture::RSDividedUICaptureVisitor, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSDividedUICapture::RSDividedUICaptureVisitor, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}


class RSOffscreenRenderCallback : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!flag_) {
            pixelMap_ = pixelmap;
            flag_ = true;
        }
        conditionVariable_.notify_one();
    }
    bool IsReady() const
    {
        return flag_;
    }
    std::shared_ptr<Media::PixelMap> GetResult(long timeOut)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!conditionVariable_.wait_for(lock, std::chrono::milliseconds(timeOut), [this] { return IsReady(); })) {
            ROSEN_LOGE("wait for %{public}lu timeout", timeOut);
        }
        return pixelMap_;
    }
private:
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    std::mutex mutex_;
    std::condition_variable conditionVariable_;
    bool flag_ = false;
};

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGI(
            "RSDividedUICaptureVisitor::ProcessSurfaceRenderNode node : %{public}" PRIu64 " is invisible",
            node.GetId());
        return;
    }
    std::shared_ptr<RSOffscreenRenderCallback> callback = std::make_shared<RSOffscreenRenderCallback>();
    auto renderServiceClient = std::make_unique<RSRenderServiceClient>();
    renderServiceClient->TakeSurfaceCapture(node.GetId(), callback, scaleX_, scaleY_);
    std::shared_ptr<Media::PixelMap> pixelMap = callback->GetResult(2000);
    if (pixelMap == nullptr) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::TakeLocalCapture failed to get pixelmap, return nullptr!");
        return;
    }
    // draw pixelmap in canvas
#ifndef USE_ROSEN_DRAWING
    auto image = RSPixelMapUtil::ExtractSkImage(pixelMap);
    canvas_->drawImage(
        image, node.GetRenderProperties().GetBoundsPositionX(), node.GetRenderProperties().GetBoundsPositionY());
#else
    auto image = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    canvas_->DrawImage(*image, node.GetRenderProperties().GetBoundsPositionX(),
        node.GetRenderProperties().GetBoundsPositionY(), Drawing::SamplingOptions());
#endif
    ProcessChildren(node);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::PrepareChildren(RSRenderNode& node)
{
    node.ApplyChildrenModifiers();
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSDividedUICapture::RSDividedUICaptureVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareChildren(node);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareChildren(node);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    PrepareCanvasRenderNode(node);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::PrepareEffectRenderNode(RSEffectRenderNode& node)
{
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareChildren(node);
}

} // namespace Rosen
} // namespace OHOS
