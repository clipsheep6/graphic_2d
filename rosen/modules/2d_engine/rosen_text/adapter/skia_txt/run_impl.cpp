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

#include "run_impl.h"
#include "impl/run_impl.h"
#include "convert.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
RunImpl::RunImpl(std::unique_ptr<SPText::Run> run): run_(std::move(run))
{
}

Drawing::Font RunImpl::GetFont() const
{
    if (!run_) {
        return {};
    }
    return run_->GetFont();
}

size_t RunImpl::GetGlyphCount() const
{
    if (!run_) {
        return 0;
    }
    return run_->GetGlyphCount();
}

std::vector<uint16_t> RunImpl::GetGlyphs(uint32_t start, uint32_t end) const
{
    if (!run_) {
        return {};
    }
    return run_->GetGlyphs(start, end);
}

std::vector<Drawing::Point> RunImpl::GetPositions(uint32_t start, uint32_t end)
{
    if (!run_) {
        return {};
    }
    return run_->GetPositions(start, end);
}

std::vector<Drawing::Point> RunImpl::GetOffsets()
{
    if (!run_) {
        return {};
    }
    return run_->GetOffsets();
}

void RunImpl::Paint(Drawing::Canvas *canvas, double x, double y)
{
    if (!run_) {
        return;
    }
    return run_->Paint(canvas, x, y);
}

void RunImpl::GetStringRange(uint32_t* location, uint32_t* length)
{
    if(location == nullptr || length == nullptr) {
        return;
    } else if (!run_) {
        *location= 0;
        *length = 0;
        return;
    }
    run_->GetStringRange(location, length);
}

std::vector<uint32_t> RunImpl::GetStringIndices(uint32_t start, uint32_t end)
{
    if (!run_) {
        return {};
    }
    return run_->GetStringIndices(start, end);
}

Drawing::Rect RunImpl::GetImageBounds()
{
    if (!run_) {
        return {};
    }
    return run_->GetImageBounds();
}

float RunImpl::GetTypographicBounds(float* ascent, float* descent, float* leading)
{
    if (ascent == nullptr || descent == nullptr || leading == nullptr) {
        return 0.0;
    } else if (!run_) {
        *ascent = 0.0;
        *descent = 0.0;
        *leading = 0.0;
        return 0.0;
    }
    return run_->GetTypographicBounds(ascent, descent, leading);
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
