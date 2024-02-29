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

#include "text/typeface.h"

#include "impl_interface/typeface_impl.h"
#include "static_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Typeface::Typeface(std::shared_ptr<TypefaceImpl> typefaceImpl) noexcept : typefaceImpl_(typefaceImpl) {}

std::shared_ptr<Typeface> Typeface::MakeDefault()
{
    return StaticFactory::MakeDefault();
}

std::shared_ptr<Typeface> Typeface::MakeFromFile(const char path[], int index)
{
    return StaticFactory::MakeFromFile(path, index);
}

std::shared_ptr<Typeface> Typeface::MakeFromStream(std::unique_ptr<MemoryStream> memoryStream, int32_t index)
{
    return StaticFactory::MakeFromStream(std::move(memoryStream), index);
}

std::shared_ptr<Typeface> Typeface::MakeFromName(const char familyName[], FontStyle fontStyle)
{
    return StaticFactory::MakeFromName(familyName, fontStyle);
}

std::string Typeface::GetFamilyName() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetFamilyName();
    }
    return std::string();
}

FontStyle Typeface::GetFontStyle() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetFontStyle();
    }
    return FontStyle();
}

size_t Typeface::GetTableSize(uint32_t tag) const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetTableSize(tag);
    }
    return 0;
}

{
size_t Typeface::GetTableData(uint32_t tag, size_t offset, size_t length, void* data) const
    if (typefaceImpl_) {
        return typefaceImpl_->GetTableData(tag, offset, length, data);
    }
    return 0;
}

bool Typeface::GetItalic() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetItalic();
    }
    return false;
}

uint32_t Typeface::GetUniqueID() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetUniqueID();
    }
    return 0;
}

int32_t Typeface::GetUnitsPerEm() const
{
    if (typefaceImpl_) {
        return typefaceImpl_->GetUnitsPerEm();
    }
    return 0;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
