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

#ifndef SKIA_TEXT_BLOB_BUILDER_H
#define SKIA_TEXT_BLOB_BUILDER_H

#include "text/text_font.h"
#include "skia_text_blob.h"
#include <include/core/SkTextBlob.h>
#include "impl_interface/text_blob_builder_impl.h"


namespace OHOS {
namespace Rosen {
namespace Drawing {

class DRAWING_API SkiaTextBlobBuilder : public TextBlobBuilderImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaTextBlobBuilder();
    ~SkiaTextBlobBuilder() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    std::shared_ptr<SkTextBlobBuilder> GetTextBlobBuilder() const;
    std::shared_ptr<TextBlob> Make() override;
    std::shared_ptr<RunBuffer> AllocRunPos(const Font& font, int count) override;

private:
    std::shared_ptr<SkTextBlobBuilder> textBlobBuilder_ = std::make_shared<SkTextBlobBuilder>();
    std::shared_ptr<RunBuffer> buffer_ = std::make_shared<RunBuffer>();
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_TEXT_BLOB_BUILDER_H
