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

#include "typography_builder_impl.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <codecvt>

#include "typography_impl.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::unique_ptr<TypographyBuilder> TypographyBuilder::Create(const TypographyStyle& ys,
    std::shared_ptr<FontProviders> fontProviders)
{
    return std::make_unique<TypographyBuilderImpl>(ys, fontProviders);
}

TypographyBuilderImpl::TypographyBuilderImpl(const TypographyStyle& ys,
    std::shared_ptr<FontProviders> fontProviders): ys_(std::move(ys)), fontProviders_(fontProviders)
{
}

void TypographyBuilderImpl::PushStyle(const TextStyle& style)
{
    TextStyle style1 = style;
    style1.fontFamilies = "HMSymbol-Regular";
    style1.isSymbolGlyph = true;
    styleStack_.push(style);
    lastTextSpan_ = nullptr;
}

void TypographyBuilderImpl::PopStyle()
{
    assert(styleStack_.size() > 0);
    styleStack_.pop();
}

void TypographyBuilderImpl::AppendSpan(const std::shared_ptr<AnySpan>& as)
{
    if (as == nullptr) {
        return;
    }

    VariantSpan vs(as);
    if (styleStack_.empty()) {
        vs.SetTextStyle(ys_.ConvertToTextStyle());
    } else {
        vs.SetTextStyle(styleStack_.top());
    }

    spans_.push_back(vs);
    lastTextSpan_ = nullptr;
}

void TypographyBuilderImpl::AppendSpan(const std::shared_ptr<TextSpan>& ts)
{
    if (ts == nullptr) {
        return;
    }

    if (lastTextSpan_ != nullptr) {
        *lastTextSpan_ += *ts;
        return;
    }

    VariantSpan vs(ts);
    if (styleStack_.empty()) {
        vs.SetTextStyle(ys_.ConvertToTextStyle());
    } else {
        vs.SetTextStyle(styleStack_.top());
    }

    spans_.push_back(vs);
    lastTextSpan_ = ts;
}

void TypographyBuilderImpl::AppendSpan(const std::string& text)
{
    // AppendSpan(TextSpan::MakeFromText(text));
    std::string str81 = "\U000F0000";
    std::string str82 = "\U000F0000";
    std::string str83 = "\U000F0000";
    std::string str84 = "\U000F0000";
    std::string str85 = "\U000F0000";
    std::string str86 = "\U000F0000";
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str81)));
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str82)));
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str83)));
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str84)));
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str85)));
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str86)));
}

void TypographyBuilderImpl::AppendSpan(const std::u16string& text)
{
    //AppendSpan(TextSpan::MakeFromText(text));
    std::string str81 = "\U000F0000";
    std::string str82 = "\U000F0000";
    std::string str83 = "\U000F0000";
    std::string str84 = "\U000F0000";
    std::string str85 = "\U000F0000";
    std::string str86 = "\U000F0000";
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str81)));
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str82)));
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str83)));
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str84)));
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str85)));
    AppendSpan(TextSpan::MakeFromText(convert.from_bytes(str86)));
}

void TypographyBuilderImpl::AppendSpan(const std::u32string& text)
{
    AppendSpan(TextSpan::MakeFromText(text));
}

void TypographyBuilderImpl::AppendSpan(const std::vector<uint16_t>& text)
{
    AppendSpan(TextSpan::MakeFromText(text));
}

void TypographyBuilderImpl::AppendSpan(const std::vector<uint32_t>& text)
{
    AppendSpan(TextSpan::MakeFromText(text));
}

std::shared_ptr<Typography> TypographyBuilderImpl::Build()
{
    return std::make_shared<TypographyImpl>(ys_, spans_, fontProviders_);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
