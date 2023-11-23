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

#include "typography_create.h"

#include "texgine_canvas.h"

#include "convert.h"
#include "typography.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<TypographyCreate> TypographyCreate::Create(const TypographyStyle& style,
    std::shared_ptr<FontCollection> collection)
{
    return std::make_unique<AdapterTextEngine::TypographyCreate>(style, collection);
}

namespace AdapterTextEngine {
TypographyCreate::TypographyCreate(const TypographyStyle& style,
    std::shared_ptr<OHOS::Rosen::FontCollection> collection)
{
    auto paragraphStyle = Convert(style);
    auto fontProviders = Convert(collection)->Get();
    builder_ = TextEngine::TypographyBuilder::Create(paragraphStyle, fontProviders);
}

void TypographyCreate::PushStyle(const TextStyle& style)
{
    auto txtTextStyle = Convert(style);
    auto fonts = txtTextStyle.fontFamilies;
    if (std::find(fonts.begin(), fonts.end(), "HMSymbol-Regular") != fonts.end()) {
        txtTextStyle.isSymbolGlyph = true;
    }
    builder_->PushStyle(txtTextStyle);
}

void TypographyCreate::PopStyle()
{
    builder_->PopStyle();
}

void TypographyCreate::AppendText(const std::u16string& text)
{
    if (text.size() == 10) {
        // std::vector<uint32_t> symbol1 = {0XF0000};
        // builder_->AppendSpan(symbol1);
        // std::vector<uint32_t> symbol2 = {0XF0001};
        // builder_->AppendSpan(symbol2);
        // std::vector<uint32_t> symbol3 = {0XF0002};
        // builder_->AppendSpan(symbol3);
        // std::vector<uint32_t> symbol4 = {0XF0003};
        // builder_->AppendSpan(symbol4);
        // std::vector<uint32_t> symbol5 = {0XF0004};
        // builder_->AppendSpan(symbol5);
        std::vector<uint32_t> symbol6 = {0XF0005};
        builder_->AppendSpan(symbol6);
        return;
    }

    builder_->AppendSpan(text);
}

class TextEnginePlaceholderRun : public TextEngine::AnySpan {
public:
    explicit TextEnginePlaceholderRun(const PlaceholderSpan &span): span_(span)
    {
    }

    double GetWidth() const override
    {
        return span_.width;
    }

    double GetHeight() const override
    {
        return span_.height;
    }

    TextEngine::AnySpanAlignment GetAlignment() const override
    {
        return Convert(span_.alignment);
    }

    TextEngine::TextBaseline GetBaseline() const override
    {
        return Convert(span_.baseline);
    }

    double GetLineOffset() const override
    {
        return span_.baselineOffset;
    }

    void Paint(TextEngine::TexgineCanvas &canvas, double offsetX, double offsetY) override
    {
    }

private:
    PlaceholderSpan span_;
};

void TypographyCreate::AppendPlaceholder(const PlaceholderSpan& span)
{
    auto anyspan = std::make_shared<TextEnginePlaceholderRun>(span);
    builder_->AppendSpan(anyspan);
}

std::unique_ptr<OHOS::Rosen::Typography> TypographyCreate::CreateTypography()
{
    return std::make_unique<Typography>(builder_->Build());
}
} // namespace AdapterTextEngine
} // namespace Rosen
} // namespace OHOS
