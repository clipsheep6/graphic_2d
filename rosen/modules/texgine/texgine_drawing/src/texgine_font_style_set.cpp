/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "texgine_font_style_set.h"

namespace Texgine {
TexgineFontStyleSet::TexgineFontStyleSet(SkFontStyleSet *set)
{
    set_ = set;
}

TexgineFontStyleSet::~TexgineFontStyleSet()
{
    if (set_) {
        set_->unref();
    }
}

int TexgineFontStyleSet::Count()
{
    if (set_ == nullptr) {
        return 0;
    }
    return set_->count();
}

void TexgineFontStyleSet::GetStyle(int index, std::shared_ptr<TexgineFontStyle> style,
    std::shared_ptr<TexgineString> name)
{
    if (set_) {
        set_->getStyle(index, style->GetFontStyle().get(), name->GetString());
    }
}

std::shared_ptr<TexgineTypeface> TexgineFontStyleSet::CreateTypeface(int index)
{
    if (set_ == nullptr) {
        return nullptr;
    }
    auto typeface = set_->createTypeface(index);
    return std::make_shared<TexgineTypeface>(typeface);
}

std::shared_ptr<TexgineTypeface> TexgineFontStyleSet::MatchStyle(std::shared_ptr<TexgineFontStyle> pattern)
{
    if (set_ == nullptr) {
        return nullptr;
    }
    auto style = *pattern->GetFontStyle();
    auto typeface = set_->matchStyle(style);
    return std::make_shared<TexgineTypeface>(typeface);
}

std::shared_ptr<TexgineFontStyleSet> TexgineFontStyleSet::CreateEmpty()
{
    auto fss = SkFontStyleSet::CreateEmpty();
    return std::make_shared<TexgineFontStyleSet>(fss);
}
} // namespace Texgine