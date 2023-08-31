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

#include "skia_font_style_set.h"
#include "skia_typeface.h"
#include "skia_font_style.h"
#include "skia_font_str.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaFontStyleSet::SkiaFontStyleSet(SkFontStyleSet *skFontStyleSet) noexcept
{
    skFontStyleSet_ = skFontStyleSet;
}

SkiaFontStyleSet::~SkiaFontStyleSet()
{
    if (skFontStyleSet_) {
        skFontStyleSet_->unref();
    }
}

int SkiaFontStyleSet::Count() const
{
    if (skFontStyleSet_ == nullptr) {
        return 0;
    }
    return skFontStyleSet_->count();
}

void SkiaFontStyleSet::GetStyle(const int index, std::shared_ptr<FontStyle> style,
    std::shared_ptr<FontString> name) const
{
    if (skFontStyleSet_ == nullptr || style == nullptr || name == nullptr) {
        return;
    }
    auto skiastyle = style->GetImpl<SkiaFontStyle>();
    auto skianame = style->GetImpl<SkiaFontStr>();
    skFontStyleSet_->getStyle(index, skiastyle->GetSkFontStyle().get(), skianame->GetString());
}

std::shared_ptr<Typeface> SkiaFontStyleSet::CreateTypeface(int index)
{
    if (skFontStyleSet_ == nullptr) {
        return nullptr;
    }
    auto skTypeface = skFontStyleSet_->createTypeface(index);
    auto skiaTypeface = std::make_shared<SkiaTypeface>(skTypeface);
    auto typeface = std::make_shared<Typeface>();
    typeface->ChangeInitTypefaceImpl(skiaTypeface);
    return typeface;
}

std::shared_ptr<Typeface> SkiaFontStyleSet::MatchStyle(std::shared_ptr<FontStyle> pattern)
{
    if (!pattern) {
        return nullptr;
    }
    auto skiaPattern = pattern->GetImpl<SkiaFontStyle>();
    if (skFontStyleSet_ == nullptr || skiaPattern->GetSkFontStyle() == nullptr) {
        return nullptr;
    }
    auto skStyle = skiaPattern->GetSkFontStyle();
    auto skTypeface = skFontStyleSet_->matchStyle(*skStyle);
    auto skiaTypeface = std::make_shared<SkiaTypeface>(skTypeface);
    auto typeface = std::make_shared<Typeface>();
    typeface->ChangeInitTypefaceImpl(skiaTypeface);
    return typeface;
}

std::shared_ptr<FontStyleSet> SkiaFontStyleSet::CreateEmpty()
{
    auto fss = SkFontStyleSet::CreateEmpty();
    auto skiaFontStyleSet = std::make_shared<SkiaFontStyleSet>(fss);
    auto fontStyleSet = std::make_shared<FontStyleSet>();
    fontStyleSet->InitFontStyleSetImpl(skiaFontStyleSet);
    return fontStyleSet;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
