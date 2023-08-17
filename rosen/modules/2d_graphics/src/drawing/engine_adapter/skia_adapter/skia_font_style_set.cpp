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

#include "skia_font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaFontStyleSet::SkiaFontStyleSet(SkFontStyleSet *set)
{
    set_ = set;
}

SkiaFontStyleSet::~SkiaFontStyleSet()
{
    if (set_) {
        set_->unref();
    }
}

int SkiaFontStyleSet::Count() const 
{
    if (!set_) {
        return 0;
    }
    return set_->count();
}

/*void SkiaFontStyleSet::GetStyle(const int index, std::shared_ptr<SkiaFontStyle> style,
    std::shared_ptr<SkiaString> name) const
{
    if (set_ == nullptr || style == nullptr || name == nullptr) {
        return;
    }
    set_->getStyle(index, style->GetFontStyle().get(), name->GetString());
}*/

std::shared_ptr<SkiaTypeface> SkiaFontStyleSet::CreateTypeface(int index)
{
    if (set_ == nullptr) {
        return nullptr;
    }
    auto typeface = set_->createTypeface(index);
    return std::make_shared<SkiaTypeface>(typeface);
}

std::shared_ptr<SkiaTypeface> SkiaFontStyleSet::MatchStyle(std::shared_ptr<SkiaFontStyle> pattern)
{
    if (set_ == nullptr || pattern == nullptr || pattern->GetFontStyle() == nullptr) {
        return nullptr;
    }
    auto style = *pattern->GetFontStyle();
    auto typeface = set_->matchStyle(style);
    return std::make_shared<SkiaTypeface>(typeface);
}

std::shared_ptr<SkiaFontStyleSet> SkiaFontStyleSet::CreateEmpty()
{
    auto fss = SkFontStyleSet::CreateEmpty();
    return std::make_shared<SkiaFontStyleSet>(fss);
}

} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
