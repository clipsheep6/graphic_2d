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

#include "skia_typeface.h"

#include "src/ports/SkFontMgr_custom.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaTypeface::SkiaTypeface() noexcept : typeface_(SkTypeface::MakeDefault()) 
{
}

SkiaTypeface::SkiaTypeface(SkTypeface *tf) noexcept : SkiaTypeface(sk_sp<SkTypeface>(tf))
{
    if (tf) {
        tf->ref();
    }
}

SkiaTypeface::SkiaTypeface(sk_sp<SkTypeface> typeface) noexcept : typeface_(typeface)
{
}

SkiaTypeface::SkiaTypeface(void *context) noexcept
{
    auto stf = reinterpret_cast<SkTypeface *>(context);
    typeface_ = sk_sp<SkTypeface>(stf);
    if (stf) {
        stf->ref();
    }
}

sk_sp<SkTypeface> SkiaTypeface::GetTypeface() const
{
    return typeface_;
}

size_t SkiaTypeface::GetTableSize(uint32_t tag) const
{
    if (typeface_ == nullptr) {
        return 0;
    }
    return typeface_->getTableSize(tag);
}

size_t SkiaTypeface::GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const
{
    if (typeface_ == nullptr) {
        return 0;
    }
    return typeface_->getTableData(tag, offset, length, data);
}

int SkiaTypeface::GetUnitsPerEm() const
{
    if (typeface_ == nullptr) {
        return 0;
    }
    return typeface_->getUnitsPerEm();
}

/*std::shared_ptr<SkiaTypeface> SkiaTypeface::MakeFromStream(
    std::unique_ptr<SkiaMemoryStream> stream, int index)
{
    if (stream == nullptr) {
        return nullptr;
    }
    auto skTypeface = SkTypeface::MakeFromStream(stream->GetStream());
    return std::make_shared<SkiaTypeface>(skTypeface);
}*/

std::shared_ptr<SkiaTypeface> SkiaTypeface::MakeFromFile(const std::string &path, int index)
{
    auto st = SkTypeface::MakeFromFile(path.c_str(), index);
    return std::make_shared<SkiaTypeface>(st);
}

/*void SkiaTypeface::GetFamilyName(SkiaString *name) const
{
    if (typeface_ == nullptr || name == nullptr) {
        return;
    }
    typeface_->getFamilyName(name->GetString());
}

std::shared_ptr<SkiaFontStyle> SkiaTypeface::GetFontStyle() const
{
    if (typeface_ == nullptr) {
        return nullptr;
    }

    auto style = typeface_->fontStyle();
    auto texgineFontStyle = std::make_shared<SkiaFontStyle>();
    texgineFontStyle->SetStyle(style);
    return texgineFontStyle;
}*/
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
