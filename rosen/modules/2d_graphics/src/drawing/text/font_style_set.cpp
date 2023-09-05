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

#include "text/font_style_set.h"
#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
FontStyleSet::FontStyleSet() noexcept : fontStyleSetImpl_(ImplFactory::CreateFontStyleSetImpl())
{}

int FontStyleSet::Count() const
{
    return fontStyleSetImpl_->Count();
}

void FontStyleSet::GetStyle(const int index, std::shared_ptr<FontStyle> style, std::shared_ptr<FontString> name) const
{
    fontStyleSetImpl_->GetStyle(index,style,name);
}

std::shared_ptr<Typeface> FontStyleSet::CreateTypeface(int index)
{
    return fontStyleSetImpl_->CreateTypeface(index);
}

std::shared_ptr<Typeface> FontStyleSet::MatchStyle(const std::shared_ptr<FontStyle> pattern)
{
    return fontStyleSetImpl_->MatchStyle(pattern);
}

void FontStyleSet::InitFontStyleSetImpl(std::shared_ptr<FontStyleSetImpl> replaceStyleSetImpl)
{
    fontStyleSetImpl_ = move(replaceStyleSetImpl);
}

std::shared_ptr<FontStyleSet> FontStyleSet::CreateEmpty()
{
    return ImplFactory::CreateFontStyleSetImpl()->CreateEmpty();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
