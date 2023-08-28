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

#include <memory>

#include <include/core/SkTypeface.h>
#include "impl_interface/typeface_impl.h"
//#include "skia_font_style.h"
//#include "skia_stream.h"
//#include "skia_string.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTypeface : public TypefaceImpl{
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaTypeface() noexcept;
    explicit SkiaTypeface(SkTypeface *tf) noexcept;
    explicit SkiaTypeface(sk_sp<SkTypeface> typeface) noexcept;
    explicit SkiaTypeface(void *context) noexcept;
    ~SkiaTypeface() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    size_t GetTableSize(uint32_t tag) const override;

    size_t GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const override;

    int GetUnitsPerEm() const override;

    //void GetFamilyName(SkiaString *name) const override;

    //std::shared_ptr<SkiaFontStyle> GetFontStyle() const override;

    //static std::shared_ptr<SkiaTypeface> MakeFromStream(std::unique_ptr<SkiaMemoryStream> stream, int index = 0);

    static std::shared_ptr<SkiaTypeface> MakeFromFile(const std::string &path, int index = 0);
    
    sk_sp<SkTypeface> GetTypeface() const;
private:
    sk_sp<SkTypeface> typeface_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif 
