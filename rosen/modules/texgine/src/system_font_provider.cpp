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

#include "texgine/system_font_provider.h"

#include <mutex>

#include "font_style_set.h"
#include "texgine_exception.h"
#include "texgine/utils/exlog.h"

namespace Texgine {
std::shared_ptr<SystemFontProvider> SystemFontProvider::GetInstance() noexcept(true)
{
    if (sfp == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (sfp == nullptr) {
            sfp.reset(new SystemFontProvider());
        }
    }
    return sfp;
}

std::shared_ptr<FontStyleSet> SystemFontProvider::MatchFamily(const std::string &familyName) noexcept(true)
{
    auto skFontMgr = SkFontMgr::RefDefault();
    if (skFontMgr == nullptr) {
        LOG2EX(ERROR) << "skFontMgr is nullptr!";
        return nullptr;
    }

    auto skFontStyleSet = skFontMgr->matchFamily(familyName.c_str());
    if (skFontStyleSet == nullptr) {
        LOG2EX_DEBUG() << "skFontStyleSet is nullptr!";
    }

    return std::make_shared<FontStyleSet>(skFontStyleSet);
}
} // namespace Texgine
