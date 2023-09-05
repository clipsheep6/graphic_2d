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

#include "skia_text_blob_builder.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<SkTextBlobBuilder> SkiaTextBlobBuilder::GetTextBlobBuilder() const
{
    return skTextBlobBuilder_;
}

std::shared_ptr<TextBlobBuilderImpl::RunBuffer> SkiaTextBlobBuilder::AllocRunPos(
    const Font &font, int count)
{
    auto skiaFont = font.GetImpl<SkiaFont>();
    if (skTextBlobBuilder_ == nullptr || skiaFont->GetFont() == nullptr) {
        return nullptr;
    }
    
    const auto &runBuffer = skTextBlobBuilder_->allocRunPos(*skiaFont->GetFont(), count);
    buffer_->glyphs = runBuffer.glyphs;
    buffer_->pos = runBuffer.pos;
    buffer_->utf8Text = runBuffer.utf8text;
    buffer_->clusters = runBuffer.clusters;
    return buffer_;
}

std::shared_ptr<TextBlob> SkiaTextBlobBuilder::Make()
{
    if (skTextBlobBuilder_ == nullptr) {
        return nullptr;
    }
    auto textBlob = std::make_shared<TextBlob>();
    auto skiaTextBlob = std::make_shared<SkiaTextBlob>();
    auto skBlob = skTextBlobBuilder_->make();
    if (skBlob == nullptr) {
        return nullptr;
    }
    skiaTextBlob->SetSkTextBlob(skBlob);
    textBlob->InitTextBlobImpl(skiaTextBlob);
    return textBlob;
}

} // Drawing
} // Rosen
} // OHOS
