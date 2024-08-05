/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#include "run_impl.h"
#include "drawing_painter_impl.h"
#include "text/font.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
namespace skt = skia::textlayout;

RunImpl::RunImpl(std::unique_ptr<skt::RunBase> runBase, const std::vector<PaintRecord>& paints)
    : runBase_(std::move(runBase)), paints_(paints)
{}

Drawing::Font RunImpl::GetFont() const
{
    if (!runBase_) {
        return {};
    }
    return runBase_->font();
}

size_t RunImpl::GetGlyphCount() const
{
    if (!runBase_) {
        return 0;
    }
    return runBase_->size();
}

std::vector<uint16_t> RunImpl::GetGlyphs(uint32_t start, uint32_t end) const
{
    if (!runBase_) {
        return {};
    }
    return runBase_->getGlyphs(start, end);
}

std::vector<Drawing::Point> RunImpl::GetPositions(uint32_t start, uint32_t end)
{
    if (!runBase_) {
        return {};
    }
    return runBase_->getPositions(start, end);
}

std::vector<Drawing::Point> RunImpl::GetOffsets()
{
    if (!runBase_) {
        return {};
    }
    return runBase_->getOffsets();
}

void RunImpl::Paint(Drawing::Canvas* canvas, double x, double y)
{
    if (!runBase_ || !canvas) {
        return;
    }
    RSCanvasParagraphPainter painter(canvas, paints_);
    runBase_->paint(&painter, x, y);
}

void RunImpl::GetStringRange(uint32_t* location, uint32_t* length)
{
    if(location == nullptr || length == nullptr) {
        return;
    } else if (!runBase_) {
        *location= 0;
        *length = 0;
        return;
    }
    runBase_->getStringRange(location, length);
}

std::vector<uint32_t> RunImpl::GetStringIndices(uint32_t start, uint32_t end)
{
    if (!runBase_) {
        return {};
    }
    return runBase_->getStringIndices(start, end);
}

Drawing::Rect RunImpl::GetImageBounds()
{
    if (!runBase_) {
        return {};
    }
    return runBase_->getImageBounds();
}

float RunImpl::GetTypographicBounds(float* ascent, float* descent, float* leading)
{
    if (ascent == nullptr || descent == nullptr || leading == nullptr) {
        return 0.0;
    } else if (!runBase_) {
        *ascent = 0.0;
        *descent = 0.0;
        *leading = 0.0;
        return 0.0;
    }
    return runBase_->getTypographicBounds(ascent, descent, leading);
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
