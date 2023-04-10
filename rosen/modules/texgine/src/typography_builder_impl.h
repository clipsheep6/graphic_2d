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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TYPOGRAPHY_BUILDER_IMPL_H
#define ROSEN_MODULES_TEXGINE_SRC_TYPOGRAPHY_BUILDER_IMPL_H

#include <stack>

#include "texgine/typography_builder.h"
#include "variant_span.h"

namespace Texgine {
class TypographyBuilderImpl : public TypographyBuilder {
public:
    TypographyBuilderImpl(const TypographyStyle& ys, std::unique_ptr<FontProviders> fontProviders);
    void PushStyle(const TextStyle &style) override;
    void PopStyle() override;
    void AppendSpan(const std::shared_ptr<AnySpan> &as) override;
    void AppendSpan(const std::shared_ptr<TextSpan> &text);
    void AppendSpan(const std::string &text) override;
    void AppendSpan(const std::u16string &text) override;
    void AppendSpan(const std::u32string &text) override;
    void AppendSpan(const std::vector<uint16_t> &text) override;
    void AppendSpan(const std::vector<uint32_t> &text) override;
    std::shared_ptr<Typography> Build() override;

private:
    TypographyStyle ys_;
    std::unique_ptr<FontProviders> fontProviders_ = nullptr;
    std::stack<TextStyle> styleStack;
    std::vector<VariantSpan> spans;
    std::shared_ptr<TextSpan> lastTextSpan = nullptr;
};
} // namespace Texgine

#endif // ROSEN_MODULES_TEXGINE_SRC_TYPOGRAPHY_BUILDER_IMPL_H
