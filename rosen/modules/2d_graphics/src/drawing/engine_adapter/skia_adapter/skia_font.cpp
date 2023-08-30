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

#include "skia_font.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void SkiaFont::SetTypeface(const std::shared_ptr<Typeface> typeface)
{
    if (typeface) {
        auto skiaTypeface = typeface->GetImpl<SkiaTypeface>();
        font_->setTypeface(skiaTypeface->GetTypeface());
    } else {
        font_->setTypeface(nullptr);
    }
}

void SkiaFont::SetSize(scalar textSize)
{
    font_->setSize(textSize);
}

// scalar SkiaFont::GetMetrics(FontMetrics *metrics) const
// {
//     if (!metrics) {
//         return 0;
//     }
//     auto skiaFontMetrics = metrics->GetImpl<SkiaFontMetrics>();
//     return font_->getMetrics(skiaFontMetrics->GetFontMetrics().get());
// }

std::shared_ptr<SkFont> SkiaFont::GetFont() const
{
    return font_;
}

} // Drawing
} // Rosen
} // OHOS
