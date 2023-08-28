/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "skia_rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

SkiaRect::SkiaRect(): rect_(std::make_shared<SkRect>())
{
    fBottom_ = &rect_->fBottom;
    fLeft_ = &rect_->fLeft;
    fTop_ = &rect_->fTop;
    fRight_ = &rect_->fRight;
}

SkiaRect SkiaRect::MakeLTRB(float left, float top, float right, float bottom)
{
    auto rect = std::make_shared<SkiaRect>();
    rect->SetRect(SkRect::MakeLTRB(left, top, right, bottom));
    return *rect;
}

SkiaRect SkiaRect::MakeXYWH(float x, float y, float w, float h)
{
    auto rect = std::make_shared<SkiaRect>();
    rect->SetRect(SkRect::MakeXYWH(x, y, w, h));
    return *rect;
}

SkiaRect SkiaRect::MakeWH(float w, float h)
{
    auto rect = std::make_shared<SkiaRect>();
    rect->SetRect(SkRect::MakeWH(w, h));
    return *rect;
}

std::shared_ptr<SkRect> SkiaRect::GetRect() const
{
    return rect_;
}

void SkiaRect::SetRect(const SkRect &rect)
{
    *rect_ = rect;
    fBottom_ = &rect_->fBottom;
    fLeft_ = &rect_->fLeft;
    fTop_ = &rect_->fTop;
    fRight_ = &rect_->fRight;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
