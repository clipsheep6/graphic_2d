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

#ifndef TYPEFACE_H
#define TYPEFACE_H
#include "drawing/engine_adapter/impl_interface/typeface_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Typeface {
public:
    Typeface() noexcept;
    virtual ~Typeface() = default;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return typefaceImpl_->DowncastingTo<T>();
    }

    size_t GetTableSize(uint32_t tag) const;

    size_t GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const;

    int GetUnitsPerEm() const;

    void GetFamilyName(std::shared_ptr<FontString> name) const;

    void ChangeInitTypefaceImpl(std::shared_ptr<TypefaceImpl> replaceTypefaceImpl);

    std::shared_ptr<FontStyle> GetFontStyle() const;

    static std::shared_ptr<Typeface> MakeFromStream(std::unique_ptr<MemoryStream> stream, int index = 0);

    static std::shared_ptr<Typeface> MakeFromFile(const std::string &path, int index = 0);
private:
    std::shared_ptr<TypefaceImpl> typefaceImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif