/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Typeface::Typeface() noexcept : typefaceImpl_(ImplFactory::CreateTypefaceImpl())
{}

std::shared_ptr<Typeface> Typeface::MakeFromStream(std::unique_ptr<MemoryStream> stream, int index) 
{
    return ImplFactory::CreateTypefaceImpl()->MakeFromStream(move(stream),index);
}

std::shared_ptr<Typeface> Typeface::MakeFromFile(const std::string &path, int index)
{
    return ImplFactory::CreateTypefaceImpl()->MakeFromFile(path,index);
}

size_t Typeface::GetTableSize(uint32_t tag) const
{
    return typefaceImpl_->GetTableSize(tag);
}

size_t Typeface::GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const
{
    return typefaceImpl_->GetTableData(tag,offset,length,data);
}

int Typeface::GetUnitsPerEm() const
{
    return typefaceImpl_->GetUnitsPerEm();
}

void Typeface::GetFamilyName(std::shared_ptr<FontString> name) const
{
    typefaceImpl_->GetFamilyName(name);
}

std::shared_ptr<FontStyle> Typeface::GetFontStyle() const
{
    return typefaceImpl_->GetFontStyle();
}

void Typeface::ChangeInitTypefaceImpl(std::shared_ptr<TypefaceImpl> replaceTypefaceImpl)
{
    typefaceImpl_ = replaceTypefaceImpl;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
