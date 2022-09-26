/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_TEXT_STYLE_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_TEXT_STYLE_H

#include <map>
#include <string>
#include <vector>

#include "include/core/SkPaint.h"

#include "typography_types.h"

namespace OHOS {
namespace Rosen {
class FontFeatures {
public:
    void SetFeature(std::string tag, int value);
    std::string GetFeatureSettings() const;
    const std::map<std::string, int> &GetFontFeatures() const;
    bool operator ==(const FontFeatures& rhs) const;

private:
    std::map<std::string, int> featureMap_;
};

struct TextShadow {
    SkColor color_ = SK_ColorBLACK;
    SkPoint offset_;
    double blurRadius_ = 0.0;

    TextShadow();
    TextShadow(SkColor color, SkPoint offset, double blurRadius);
    bool operator ==(const TextShadow& rhs) const;
    bool operator !=(const TextShadow& rhs) const;
    bool HasShadow() const;
};

struct TextStyle {
    SkColor color_ = SK_ColorWHITE;
    TextDecoration decoration_ = TextDecoration::NONE;
    SkColor decorationColor_ = SK_ColorTRANSPARENT;
    TextDecorationStyle decorationStyle_ = TextDecorationStyle::Solid;
    double decorationThicknessMultiplier_ = 1.0;
    FontWeight fontWeight_ = FontWeight::W400;
    FontStyle fontStyle_ = FontStyle::Normal;
    TextBaseline textBaseline_ = TextBaseline::Alphabetic;
    std::vector<std::string> fontFamilies_;
    double fontSize_ = 14.0;
    double letterSpacing_ = 0.0;
    double wordSpacing_ = 0.0;
    double height_ = 1.0;
    bool hasHeightOverride_ = false;
    std::string locale_;
    std::optional<SkPaint> background_;
    std::optional<SkPaint> foreground_;
    std::vector<TextShadow> textShadows_;
    FontFeatures fontFeatures_;

    bool operator ==(const TextStyle &rhs) const;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TEXT_STYLE_H
