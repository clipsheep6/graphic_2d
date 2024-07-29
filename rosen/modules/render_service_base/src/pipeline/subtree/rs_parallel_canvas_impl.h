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
#ifndef RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_CANVAS_IMPL_H
#define RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_CANVAS_IMPL_H

#include "./rs_parallel_recorder.h"
#include "./rs_parallel_canvas.h"

namespace OHOS::Rosen {
#ifdef USE_ROSEN_DRAWING
using namespace Drawing;

RSParallelStatusCanvas::RSParallelStatusCanvas(Drawing::Canvas* canvas)
    : RSPaintFilterCanvas(canvas)
{}

RSParallelStatusCanvas::~RSParallelStatusCanvas() = default;

void RSParallelStatusCanvas::SaveLayer(const SaveLayerOps& saveLayerRec)
{
    RS_LOGE("RSParallelStatusCanvas::SaveLayer Error");
    abort();
}

RSParallelDrawCanvas::RSParallelDrawCanvas(Drawing::Canvas* canvas)
    : RSPaintFilterCanvas(canvas)
{}

RSParallelDrawCanvas::RSParallelDrawCanvas(Drawing::Surface* surface)
    : RSPaintFilterCanvas(surface)
{}

RSParallelDrawCanvas::~RSParallelDrawCanvas() = default;

void RSParallelDrawCanvas::InheritStatus(RSParallelStatusCanvas* statusCanvas)
{
    //playback
    if (statusCanvas != nullptr) {
        auto recorder = statusCanvas->GetParallelRecorder();
        RSParallelPlayback(recorder, this);
    }
}

void RSParallelDrawCanvas::SetIsSubtreeParallel(bool canSharedDraw)
{
    canSharedDraw_ = canSharedDraw;
}

void RSParallelDrawCanvas::DrawImage(const Drawing::Image& image, const Drawing::scalar px,
                                     const Drawing::scalar py,
                                     const Drawing::SamplingOptions& sampling)
{
    if (canSharedDraw_) {
    auto gpuContext = GetGPUContext();
    auto draw = RSParallelResourceManager::Singleton().GenerateSharedImageForDraw(image, gpuContext, false);
    if (draw != nullptr) {
        RSPaintFilterCanvas::DrawImage(*draw, px, py, sampling);
        return ;
    }
    }
    RSPaintFilterCanvas::DrawImage(image, px, py, sampling);
}

void RSParallelDrawCanvas::DrawImageRect(const Drawing::Image& image, const Drawing::Rect& src, const Drawing::Rect& dst,
                                         const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint)
{
    if (canSharedDraw_) {
    auto gpuContext = GetGPUContext();
    auto draw = RSParallelResourceManager::Singleton().GenerateSharedImageForDraw(image, gpuContext);
    if (draw != nullptr) {
        RSPaintFilterCanvas::DrawImageRect(*draw, src, dst, sampling, constraint);
        return ;
    }
    }
    RSPaintFilterCanvas::DrawImageRect(image, src, dst, sampling, constraint);
}

void RSParallelDrawCanvas::DrawImageRect(const Drawing::Image& image,
                                         const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling)
{
    if (canSharedDraw_) {
    auto gpuContext = GetGPUContext();
    auto draw = RSParallelResourceManager::Singleton().GenerateSharedImageForDraw(image, gpuContext);
    if (draw != nullptr) {
        RSPaintFilterCanvas::DrawImageRect(*draw, dst, sampling);
        return ;
    }
    }
    RSPaintFilterCanvas::DrawImageRect(image, dst, sampling);
}

RSParallelCanvas::RSParallelCanvas(Drawing::Surface* surface)
    : RSPaintFilterCanvas(surface)
{}

size_t RSParallelCanvas::AddParallelStatusCanvas(Drawing::Canvas* raw)
{
    auto wrapper = std::make_shared<RSParallelStatusCanvas>(raw);
    pCanvasList_.push_back(wrapper.get());
    auto idx = pCanvasList_.size() - 1;
    statusCanvasMap_[idx] = wrapper;
    return idx;
}

void RSParallelCanvas::RemoveAllStatusCanvas()
{
    RemoveAll();
    AddCanvas(canvas_);
    statusCanvasMap_.clear();
}

void RSParallelCanvas::RemoveStatusCanvas(size_t idx)
{
    if (idx < pCanvasList_.size()) {
        pCanvasList_.erase(pCanvasList_.begin() + idx);
        statusCanvasMap_.erase(idx);
    }
}

#endif
}
#endif