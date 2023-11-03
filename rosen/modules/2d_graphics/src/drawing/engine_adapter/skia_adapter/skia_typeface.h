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

#include "include/core/SkData.h"
#include "include/core/SkTypeface.h"

#include "impl_interface/typeface_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTypeface : public TypefaceImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    explicit SkiaTypeface(sk_sp<SkTypeface> skTypeface);
    ~SkiaTypeface() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    sk_sp<SkTypeface> GetTypeface() const;

    std::string GetFamilyName() const override;
    FontStyle GetFontStyle() const override;
    size_t GetTableSize(uint32_t tag) const override;
    size_t GetTableData(uint32_t tag, size_t offset, size_t length, void* data) const override;
    bool GetItalic() const override;
    uint32_t GetUniqueID() const override;

    static sk_sp<SkData> SerializeTypeface(SkTypeface* typeface, void* ctx);
    static sk_sp<SkTypeface> DeserializeTypeface(const void* data, size_t length, void* ctx);

private:
    SkiaTypeface() = default;

    sk_sp<SkTypeface> skTypeface_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif