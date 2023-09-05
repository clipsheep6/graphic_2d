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

#ifndef SKIA_FONT_MANAGER_H
#define SKIA_FONT_MANAGER_H

#include <memory>
#include <include/core/SkFontMgr.h>
#include "impl_interface/font_manager_impl.h"
#include "text/font_manager.h"
#include "skia_font_style.h"
#include "skia_font_style_set.h"
#include "skia_typeface.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontManager : public FontManagerImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaFontManager() noexcept;
    ~SkiaFontManager() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    std::shared_ptr<FontManager> RefDefault() override;

    std::shared_ptr<Typeface> MatchFamilyStyleCharacter(const std::string &familyName,
        const FontStyle &style, const char* bcp47[], int bcp47Count, int32_t character) override;

    std::shared_ptr<FontStyleSet> MatchFamily(const std::string &familyName) override;

    sk_sp<SkFontMgr> GetSkFontMgr() const;

    void SetSkFontMgr(const sk_sp<SkFontMgr> mgr);

private:
    sk_sp<SkFontMgr> skFontMgr_;
};
} // Drawing
} // Rosen
} // OHOS

#endif
