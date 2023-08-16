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

#ifndef SKIA_FONT_MANAGER__H
#define SKIA_FONT_MANAGER__H

#include <include/core/SkFontMgr.h>
#include <memory>

#include "impl_interface/font_manager_impl.h"
#include "skia_font_style.h"
#include "skia_font_style_set.h"
#include "skia_typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontManager : public FontManagerImpl{
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaFontManager();
    ~SkiaFontManager() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    static std::shared_ptr<SkiaFontManager> RefDefault();

    sk_sp<SkFontMgr> GetFontMgr() const;

    void SetFontMgr(const sk_sp<SkFontMgr> mgr);

    std::shared_ptr<SkiaTypeface> MatchFamilyStyleCharacter(const std::string &familyName,
        const SkiaFontStyle &style, const char* bcp47[], int bcp47Count, int32_t character);

    std::shared_ptr<SkiaFontStyleSet> MatchFamily(const std::string &familyName);

private:
    sk_sp<SkFontMgr> fontMgr_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif 
 