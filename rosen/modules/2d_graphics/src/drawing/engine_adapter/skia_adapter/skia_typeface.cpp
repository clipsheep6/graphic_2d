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
#include "skia_memory_stream.h"
#include "skia_font_string.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaTypeface::SkiaTypeface(): skTypeface_(SkTypeface::MakeDefault())
{    
}

SkiaTypeface::SkiaTypeface(SkTypeface *tf) : SkiaTypeface(sk_sp<SkTypeface>(tf))
{
    if (tf) {
        tf->ref();
    }
}

SkiaTypeface::SkiaTypeface(sk_sp<SkTypeface> typeface): skTypeface_(typeface)
{
}

SkiaTypeface::SkiaTypeface(void *context)
{
    auto stf = reinterpret_cast<SkTypeface *>(context);
    skTypeface_ = sk_sp<SkTypeface>(stf);
    if (stf) {
        stf->ref();
    }
}

size_t SkiaTypeface::GetTableSize(uint32_t tag) const
{
    if (!skTypeface_) {
        return 0;
    }
    return skTypeface_->getTableSize(tag);   
}

size_t SkiaTypeface::GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const
{
    if (!skTypeface_) {
        return 0;
    }
    return skTypeface_->getTableData(tag, offset, length, data);
}

int SkiaTypeface::GetUnitsPerEm() const
{
    if (!skTypeface_) {
        return 0;
    }
    return skTypeface_->getUnitsPerEm();
}

std::shared_ptr<Typeface> SkiaTypeface::MakeFromStream(std::unique_ptr<MemoryStream> stream, int index)
{
    if (stream == nullptr) {
        return nullptr;
    }
    auto skMemoryStreamImpl = stream->GetImpl<SkiaMemoryStream>();
    auto skTypeface = SkTypeface::MakeFromStream(skMemoryStreamImpl->GetSkStream());//return sk_sp<SkTypeface>
    auto otherTypeface = std::make_shared<Typeface>();
    auto otherSkiaTypeface = std::make_shared<SkiaTypeface>(skTypeface);
    otherTypeface->ChangeInitTypefaceImpl(otherSkiaTypeface);
    return otherTypeface;
}

std::shared_ptr<Typeface> SkiaTypeface::MakeFromFile(const std::string &path, int index)
{
    auto st = SkTypeface::MakeFromFile(path.c_str(), index); //sksp
    auto otherTypeface = std::make_shared<Typeface>();
    auto otherSkiaTypace = std::make_shared<SkiaTypeface>(st);
    otherTypeface->ChangeInitTypefaceImpl(otherSkiaTypace);
    return otherTypeface;
}

void SkiaTypeface::GetFamilyName(std::shared_ptr<FontString> name) const
{
    if (skTypeface_ == nullptr || name == nullptr) {
        return;
    }
    auto skiaStringName = name->GetImpl<SkiaFontString>();
    skTypeface_->getFamilyName(skiaStringName->GetString());
}

std::shared_ptr<FontStyle> SkiaTypeface::GetFontStyle() const
{
    if (skTypeface_ == nullptr) {
        return nullptr;
    }

    auto skFontStyle = skTypeface_->fontStyle();//skfontstyle
    auto fontStyle = std::make_shared<FontStyle>();
    auto skiaFontStyle = fontStyle->GetImpl<SkiaFontStyle>();
    skiaFontStyle->SetSkStyle(skFontStyle);
    fontStyle->InitFontStyleImpl(skiaFontStyle);
    return fontStyle;
}

sk_sp<SkTypeface> SkiaTypeface::GetTypeface() const
{
    return skTypeface_;
}

} // Drawing
} // Rosen
} // OHOS
