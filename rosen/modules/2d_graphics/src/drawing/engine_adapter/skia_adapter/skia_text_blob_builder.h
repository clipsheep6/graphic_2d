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

#ifndef SKIA_TEXT_BLOB_BUILDER_H
#define SKIA_TEXT_BLOB_BUILDER_H

#include <memory>
#include <include/core/SkTextBlob.h>
#include "impl_interface/text_blob_builder_impl.h"
#include "text/text_blob.h"
#include "skia_font.h"
#include "skia_text_blob.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTextBlobBuilder : public TextBlobBuilderImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaTextBlobBuilder() noexcept {};
    ~SkiaTextBlobBuilder() override {};
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }
    
    std::shared_ptr<SkTextBlobBuilder> GetTextBlobBuilder() const;

    /*
     * @brief Returns run with storage for glyphs and SkPoint positions
     * @param font SkFont used for this run
     * @param The count The number of glyphs
     * @return Writable glyph buffer and SkPoint buffer
     */
    std::shared_ptr<RunBuffer> AllocRunPos(const Font &font, int count) override;

    /*
     * @brief Create TexgineTextBlob
     */
    std::shared_ptr<TextBlob> Make() override;

private:
    std::shared_ptr<SkTextBlobBuilder> skTextBlobBuilder_ = std::make_shared<SkTextBlobBuilder>();
    std::shared_ptr<RunBuffer> buffer_ = std::make_shared<RunBuffer>();
};
} // Drawing
} // Rosen
} // OHOS

#endif
