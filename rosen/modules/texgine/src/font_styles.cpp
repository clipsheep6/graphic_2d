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

#include <map>

#include "font_styles.h"
#include "texgine_exception.h"
#include "texgine/utils/exlog.h"

namespace Texgine {
namespace {
FontStyles::Slant ToSlant(const FontStyle style)
{
    std::map<FontStyle, FontStyles::Slant> switchMap = {
        {FontStyle::Normal, FontStyles::Slant::UPRIGHT},
        {FontStyle::Italic, FontStyles::Slant::ITALIC}
    };
    return switchMap[style];
}

SkFontStyle::Slant ToSkSlant(const FontStyles::Slant slant)
{
    std::map<FontStyles::Slant, SkFontStyle::Slant> switchMap = {
        {FontStyles::Slant::UPRIGHT, SkFontStyle::Slant::kUpright_Slant},
        {FontStyles::Slant::ITALIC, SkFontStyle::Slant::kItalic_Slant},
        {FontStyles::Slant::OBLIQUE, SkFontStyle::Slant::kOblique_Slant},
    };
    return switchMap[slant];
}
} // namespace

FontStyles::FontStyles(FontWeight weight, FontStyle style)
{
    if (!(0 <= static_cast<int>(weight) && weight < FontWeight::MAX)) {
        LOG2EX(ERROR) << "FontWeight Error!";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    if (!(0 <= static_cast<int>(style) && style < FontStyle::MAX)) {
        LOG2EX(ERROR) << "FontStyle Error!";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    slant_ = ToSlant(style);
    weight_ = static_cast<Weight>(static_cast<int>(weight) + 1);
}

FontStyles::FontStyles(FontStyles::Weight weight, FontStyles::Width width, FontStyles::Slant slant)
{
    if (!(0 <= static_cast<int>(weight) && weight < Weight::MAX)) {
        LOG2EX(ERROR) << "Weight Error!";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    if (!(0 <= static_cast<int>(width) && width < Width::MAX)) {
        LOG2EX(ERROR) << "Width Error!";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    if (!(0 <= static_cast<int>(slant) && slant < FontStyles::Slant::MAX)) {
        LOG2EX(ERROR) << "Slant Error!";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    weight_ = weight;
    width_ = width;
    slant_ = slant;
}

SkFontStyle FontStyles::ToSkFontStyle() const
{
    return SkFontStyle(static_cast<int>(weight_) * 100, static_cast<int>(width_) + 1, ToSkSlant(slant_));
}

bool FontStyles::operator ==(const FontStyles &rhs) const
{
    return weight_ == rhs.weight_ && width_ == rhs.width_ && slant_ == rhs.slant_;
}

bool FontStyles::operator !=(const FontStyles &rhs) const
{
    return !(*this == rhs);
}

bool FontStyles::operator <(const FontStyles& rhs) const
{
    if (weight_ != rhs.weight_) {
        return weight_ < rhs.weight_;
    }

    if (width_ != rhs.width_) {
        return width_ < rhs.width_;
    }
    return slant_ < rhs.slant_;
}
} // namespace Texgine
