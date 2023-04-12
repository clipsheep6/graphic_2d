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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXT_SHAPER_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXT_SHAPER_H

#include "variant_span.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TextShaper {
public:
    int Shape(const VariantSpan &span,
              const TypographyStyle &ys,
              const std::unique_ptr<FontProviders> &fontProviders) const;
    int DoShape(std::shared_ptr<TextSpan> &span,
                const TextStyle &xs,
                const TypographyStyle &ys,
                const std::unique_ptr<FontProviders> &fontProviders) const;
    std::shared_ptr<TexgineTextBlob> GenerateTextBlob(const TexgineFont &font, const CharGroups &cgs,
        double &spanWidth, std::vector<double> &glyphWidths) const;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXT_SHAPER_H
