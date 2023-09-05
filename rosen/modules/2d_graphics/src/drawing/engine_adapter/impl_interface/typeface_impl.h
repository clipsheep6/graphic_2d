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
#ifndef TYPEFACE_IMPL_H
#define TYPEFACE_IMPL_H

#include "base_impl.h"
#include "utils/scalar.h"
#include "text/memory_stream.h"
#include "text/font_string.h"
#include "text/font_style.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
class Typeface;
class TypefaceImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    TypefaceImpl() noexcept {};
    ~TypefaceImpl() override {};

    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }
    
    virtual size_t GetTableSize(uint32_t tag) const = 0;

    virtual size_t GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const = 0;

    virtual int GetUnitsPerEm() const = 0;

    virtual void GetFamilyName(std::shared_ptr<FontString> name) const = 0;

    virtual std::shared_ptr<FontStyle> GetFontStyle() const = 0;

    virtual std::shared_ptr<Typeface> MakeFromStream(std::unique_ptr<MemoryStream> stream, int index = 0) = 0;

    virtual std::shared_ptr<Typeface> MakeFromFile(const std::string &path, int index = 0) = 0;

};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // FONT_IMPL_H
