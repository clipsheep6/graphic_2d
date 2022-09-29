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

#include "typography_create.h"

#include "convert.h"
#include "typography.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<TypographyCreate> TypographyCreate::Create(const TypographyStyle& style,
                                                           std::shared_ptr<FontCollection> collection)
{
    return std::make_unique<AdapterTexgine::TypographyCreate>(style, collection);
}

namespace AdapterTexgine {
TypographyCreate::TypographyCreate(const TypographyStyle& style, std::shared_ptr<OHOS::Rosen::FontCollection> collection)
{
    auto paragraphStyle = Convert(style);
    auto fontProviders = Convert(collection)->Get();
    builder_ = Texgine::TypographyBuilder::Create(paragraphStyle, std::move(fontProviders));
}

void TypographyCreate::PushStyle(const TextStyle& style)
{
    auto txtTextStyle = Convert(style);
    builder_->PushStyle(txtTextStyle);
}

void TypographyCreate::PopStyle()
{
    builder_->PopStyle();
}

void TypographyCreate::AppendText(const std::u16string& text)
{
    builder_->AppendSpan(text);
}

class TexginePlaceholderRun : public Texgine::AnySpan {
public:
    TexginePlaceholderRun(const PlaceholderSpan &span)
    {
        span_ = span;
    }

    double GetWidth() const override
    {
        return span_.width;
    }

    double GetHeight() const override
    {
        return span_.height;
    }

    Texgine::AnySpanAlignment GetAlignment() const override
    {
        return Convert(span_.alignment);
    }

    Texgine::TextBaseline GetBaseline() const override
    {
        return Convert(span_.baseline);
    }

    double GetLineOffset() const override
    {
        return span_.baselineOffset;
    }

    void Paint(SkCanvas &canvas, double offsetx, double offsety) override
    {
    }

private:
    PlaceholderSpan span_;
};

void TypographyCreate::AppendPlaceholder(const PlaceholderSpan& span)
{
    auto anyspan = std::make_shared<TexginePlaceholderRun>(span);
    builder_->AppendSpan(anyspan);
}

std::unique_ptr<OHOS::Rosen::Typography> TypographyCreate::CreateTypography()
{
    return std::make_unique<Typography>(builder_->Build());
}
} // namespace AdapterTexgine
} // namespace Rosen
} // namespace OHOS
