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
#include "skia_font_metrics.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaFontMetrics::SkiaFontMetrics() noexcept
{
    fFlags_ = &skFontMetrics_->fFlags;
    fTop_ = &skFontMetrics_->fTop;
    fAscent_ = &skFontMetrics_->fAscent;
    fDescent_ = &skFontMetrics_->fDescent;
    fBottom_ = &skFontMetrics_->fBottom;
    fLeading_ = &skFontMetrics_->fLeading;
    fAvgCharWidth_ = &skFontMetrics_->fAvgCharWidth;
    fMaxCharWidth_ = &skFontMetrics_->fMaxCharWidth;
    fXMin_ = &skFontMetrics_->fXMin;
    fXMax_ = &skFontMetrics_->fXMax;
    fXHeight_ = &skFontMetrics_->fXHeight;
    fCapHeight_ = &skFontMetrics_->fCapHeight;
    fUnderlineThickness_ = &skFontMetrics_->fUnderlineThickness;
    fUnderlinePosition_ = &skFontMetrics_->fUnderlinePosition;
    fStrikeoutThickness_ = &skFontMetrics_->fStrikeoutThickness;
    fStrikeoutPosition_ = &skFontMetrics_->fStrikeoutPosition;
}

std::shared_ptr<SkFontMetrics> SkiaFontMetrics::GetSkFontMetrics() const
{
    return skFontMetrics_;
}

void SkiaFontMetrics::SetSkFontMetrics(const std::shared_ptr<SkFontMetrics> metrics)
{
    if(!skFontMetrics_) {
        return ;
    }
    skFontMetrics_ = metrics;
    fFlags_ = &skFontMetrics_->fFlags;
    fTop_ = &skFontMetrics_->fTop;
    fAscent_ = &skFontMetrics_->fAscent;
    fDescent_ = &skFontMetrics_->fDescent;
    fBottom_ = &skFontMetrics_->fBottom;
    fLeading_ = &skFontMetrics_->fLeading;
    fAvgCharWidth_ = &skFontMetrics_->fAvgCharWidth;
    fMaxCharWidth_ = &skFontMetrics_->fMaxCharWidth;
    fXMin_ = &skFontMetrics_->fXMin;
    fXMax_ = &skFontMetrics_->fXMax;
    fXHeight_ = &skFontMetrics_->fXHeight;
    fCapHeight_ = &skFontMetrics_->fCapHeight;
    fUnderlineThickness_ = &skFontMetrics_->fUnderlineThickness;
    fUnderlinePosition_ = &skFontMetrics_->fUnderlinePosition;
    fStrikeoutThickness_ = &skFontMetrics_->fStrikeoutThickness;
    fStrikeoutPosition_ = &skFontMetrics_->fStrikeoutPosition;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
