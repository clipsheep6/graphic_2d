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

#include "texgine/dynamic_font_provider.h"

#include <iostream>
#include <mutex>

#include "include/core/SkStream.h"

#include "dynamic_font_style_set.h"
#include "font_style_set.h"
#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#include "typeface.h"

namespace Texgine {
std::shared_ptr<DynamicFontProvider> DynamicFontProvider::Create() noexcept(true)
{
    return std::shared_ptr<DynamicFontProvider>(new DynamicFontProvider());
}

int DynamicFontProvider::LoadFont(const std::string &familyName, const void *data, size_t datalen) noexcept(true)
{
    if (data == nullptr) {
        LOG2EX(ERROR) << "data is nullptr!";
        return 1;
    }

    if (datalen == 0) {
        LOG2EX(ERROR) << "datalen is 0!";
        return 1;
    }

    auto stream = SkMemoryStream::MakeCopy(data, datalen);
    if (stream == nullptr) {
        LOG2EX(ERROR) << "stream is nullptr!";
        return 2;
    }

    auto skTypeface = SkTypeface::MakeFromStream(std::move(stream));
    if (skTypeface == nullptr) {
        LOG2EX(ERROR) << "skTypeface is nullptr!";
        return 2;
    }

    auto typeface = std::make_unique<Typeface>(skTypeface);
    LOG2EX_DEBUG() << "load font name:" << familyName;
    auto dfss = new DynamicFontStyleSet(std::move(typeface));
    fontStyleSetMap_[familyName] = std::make_shared<FontStyleSet>(dfss);
    return 0;
}

std::shared_ptr<FontStyleSet> DynamicFontProvider::MatchFamily(const std::string &familyName) noexcept(true)
{
    for (auto &[name, fss] : fontStyleSetMap_) {
        if (familyName == name) {
            return fss;
        }
    }
    return nullptr;
}
} // namespace Texgine
