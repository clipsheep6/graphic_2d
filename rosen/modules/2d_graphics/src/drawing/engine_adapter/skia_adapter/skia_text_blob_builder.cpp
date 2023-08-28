/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "skia_text_blob_builder.h"
#include "skia_font.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

SkiaTextBlobBuilder::SkiaTextBlobBuilder() {}

std::shared_ptr<SkTextBlobBuilder> SkiaTextBlobBuilder::GetTextBlobBuilder() const
{
    return textBlobBuilder_;
}

std::shared_ptr<RunBuffer> SkiaTextBlobBuilder::AllocRunPos(
    const Font& font, int count)
{
    if (textBlobBuilder_ == nullptr) {
        return nullptr;
    }
    auto text_font = font.GetImpl<SkiaFont>();
    if (text_font == nullptr) {
        return nullptr;
    }

    const auto &runBuffer = textBlobBuilder_->allocRunPos(*text_font->GetFont(), count);
    buffer_->glyphs = runBuffer.glyphs;
    buffer_->pos = runBuffer.pos;
    buffer_->utf8Text = runBuffer.utf8text;
    buffer_->clusters = runBuffer.clusters;
    return buffer_;
    return nullptr;
}

std::shared_ptr<TextBlob> SkiaTextBlobBuilder::Make()
{
    if (textBlobBuilder_ == nullptr) {
        return nullptr;
    }
    auto tb = std::make_shared<TextBlob>();
    auto blob = textBlobBuilder_->make();
    if (blob == nullptr) {
        return nullptr;
    }

    auto text =  tb->GetImpl<SkiaTextBlob>();
    text->SetTextBlob(blob);
    return tb;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
