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

#include "typography_builder_impl.h"

#include <assert.h>
#include <iostream>
#include <memory>

#include "typography_impl.h"

namespace Texgine {
std::unique_ptr<TypographyBuilder> TypographyBuilder::Create(const TypographyStyle &ys,
    std::unique_ptr<FontProviders> fontProviders)
{
    return std::make_unique<TypographyBuilderImpl>(ys, std::move(fontProviders));
}

TypographyBuilderImpl::TypographyBuilderImpl(const TypographyStyle& ys,
                                             std::unique_ptr<FontProviders> fontProviders)
{
    ys_ = std::move(ys);
    fontProviders_ = std::move(fontProviders);
}

void TypographyBuilderImpl::PushStyle(const TextStyle& style)
{
    styleStack.push(style);
    lastTextSpan = nullptr;
}

void TypographyBuilderImpl::PopStyle()
{
    assert(styleStack.size() > 0);
    styleStack.pop();
}

void TypographyBuilderImpl::AppendSpan(const std::shared_ptr<AnySpan> &as)
{
    if (as == nullptr) {
        return;
    }

    VariantSpan vs(as);
    if (styleStack.empty()) {
        vs.SetTextStyle(ys_.ConvertToTextStyle());
    } else {
        vs.SetTextStyle(styleStack.top());
    }

    spans.push_back(vs);
    lastTextSpan = nullptr;
}

void TypographyBuilderImpl::AppendSpan(const std::shared_ptr<TextSpan> &ts)
{
    if (ts == nullptr) {
        return;
    }

    if (lastTextSpan != nullptr) {
        *lastTextSpan += *ts;
        return;
    }

    VariantSpan vs(ts);
    if (styleStack.empty()) {
        vs.SetTextStyle(ys_.ConvertToTextStyle());
    } else {
        vs.SetTextStyle(styleStack.top());
    }

    spans.push_back(vs);
    lastTextSpan = ts;
}

void TypographyBuilderImpl::AppendSpan(const std::string &text)
{
    AppendSpan(TextSpan::MakeFromText(text));
}

void TypographyBuilderImpl::AppendSpan(const std::u16string &text)
{
    AppendSpan(TextSpan::MakeFromText(text));
}

void TypographyBuilderImpl::AppendSpan(const std::u32string &text)
{
    AppendSpan(TextSpan::MakeFromText(text));
}

void TypographyBuilderImpl::AppendSpan(const std::vector<uint16_t> &text)
{
    AppendSpan(TextSpan::MakeFromText(text));
}

void TypographyBuilderImpl::AppendSpan(const std::vector<uint32_t> &text)
{
    AppendSpan(TextSpan::MakeFromText(text));
}

std::shared_ptr<Typography> TypographyBuilderImpl::Build()
{
    return std::make_shared<TypographyImpl>(ys_, spans, std::move(fontProviders_));
}
} // namespace Texgine
