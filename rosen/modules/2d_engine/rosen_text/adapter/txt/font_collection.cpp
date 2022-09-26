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

#include "font_collection.h"

#include "include/core/SkTypeface.h"

#include "convert.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<FontCollection> FontCollection::Create()
{
    return std::make_shared<AdapterTxt::FontCollection>();
}

std::shared_ptr<FontCollection> FontCollection::From(std::shared_ptr<txt::FontCollection> fontCollection)
{
    return std::make_shared<AdapterTxt::FontCollection>(fontCollection);
}

namespace AdapterTxt {
FontCollection::FontCollection(std::shared_ptr<txt::FontCollection> fontCollection)
    : fontCollection_(fontCollection)
{
    dfmanager_ = sk_make_sp<txt::DynamicFontManager>();
    if (fontCollection_ == nullptr) {
        fontCollection_ = std::make_shared<txt::FontCollection>();
    }
}

std::shared_ptr<txt::FontCollection> FontCollection::Get()
{
    if (!disableSystemFont_) {
        fontCollection_->SetupDefaultFontManager();
    }
    fontCollection_->SetDynamicFontManager(dfmanager_);
    return fontCollection_;
}

void FontCollection::DisableFallback()
{
    fontCollection_->DisableFontFallback();
}

void FontCollection::DisableSystemFont()
{
    disableSystemFont_ = true;
}

void FontCollection::LoadFont(const std::string &familyName, const uint8_t *data, size_t datalen)
{
    auto stream = std::make_unique<SkMemoryStream>(data, datalen, true);
    auto typeface = SkTypeface::MakeFromStream(std::move(stream));
    if (familyName.empty()) {
        dfmanager_->font_provider().RegisterTypeface(typeface);
    } else {
        dfmanager_->font_provider().RegisterTypeface(typeface, familyName);
    }
    fontCollection_->ClearFontFamilyCache();
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
