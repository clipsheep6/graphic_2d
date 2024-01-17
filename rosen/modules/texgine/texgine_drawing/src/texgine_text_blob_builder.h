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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_TEXT_BLOB_BUILDER_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_TEXT_BLOB_BUILDER_H

#include <memory>

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkTextBlob.h>
#else
#include "drawing.h"
#endif

#include "texgine_font.h"
#include "texgine_text_blob.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineTextBlobBuilder {
public:
#ifndef USE_ROSEN_DRAWING
    /*
     * @brief Get the SkTextBlobBuilder.
     *        SkTextBlobBuilder is helper class for constructing SkTextBlob
     */
    std::shared_ptr<SkTextBlobBuilder> GetTextBlobBuilder() const;
#else
    std::shared_ptr<RSTextBlobBuilder> GetTextBlobBuilder() const;
#endif
    /*
     * @brief Returns run with storage for glyphs and SkPoint positions
     * @param font SkFont used for this run
     * @param The count The number of glyphs
     * @return Writable glyph buffer and SkPoint buffer
     */
#ifndef USE_ROSEN_DRAWING
    const SkTextBlobBuilder::RunBuffer& AllocRunPos(const TexgineFont &font, int count);
#else
    const RSTextBlobBuilder::RunBuffer& AllocRunPos(const TexgineFont &font, int count);
#endif

    /*
     * @brief Create TexgineTextBlob
     */
    std::shared_ptr<TexgineTextBlob> Make();

private:
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<SkTextBlobBuilder> textBlobBuilder_ = std::make_shared<SkTextBlobBuilder>();
#else
    std::shared_ptr<RSTextBlobBuilder> textBlobBuilder_ = std::make_shared<RSTextBlobBuilder>();
#endif
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_TEXT_BLOB_BUILDER_H
