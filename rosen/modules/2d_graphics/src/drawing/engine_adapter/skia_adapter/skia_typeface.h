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

#ifndef SKIA_TYPEFACE_H
#define SKIA_TYPEFACE_H

#include <include/core/SkFont.h>
#include "text/typeface.h"
#include "skia_font_style.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTypeface : public TypefaceImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaTypeface();
    explicit SkiaTypeface(SkTypeface *tf);
    explicit SkiaTypeface(sk_sp<SkTypeface> typeface);
    explicit SkiaTypeface(void *context);
    ~SkiaTypeface() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }
    size_t GetTableSize(uint32_t tag) const override;

    size_t GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const override;

    int GetUnitsPerEm() const override;

    void GetFamilyName(std::shared_ptr<FontString> name) const override;

    std::shared_ptr<FontStyle> GetFontStyle() const override;

    std::shared_ptr<Typeface> MakeFromStream(std::unique_ptr<MemoryStream> stream, int index = 0) override;

    std::shared_ptr<Typeface> MakeFromFile(const std::string &path, int index = 0) override;
    sk_sp<SkTypeface> GetTypeface() const;
    void SetTypeface(const sk_sp<SkTypeface>);
private:
    sk_sp<SkTypeface> skTypeface_ = nullptr;
};
} // Drawing
} // Rosen
} // OHOS

#endif
