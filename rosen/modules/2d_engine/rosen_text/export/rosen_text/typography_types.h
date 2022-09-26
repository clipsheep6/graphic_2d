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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_TYPES_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_TYPES_H

namespace OHOS {
namespace Rosen {
enum class TextDirection {
    RTL,
    LTR,
};

enum class TextAlign {
    Left,
    Right,
    Center,
    Justify,
    Start,
    End,
};

enum class BreakStrategy {
    Greedy = 0,
    HighQuality = 1,
    Balanced = 2
};

enum class WordBreakType {
    Normal = 0,
    BreakAll = 1,
    BreakWord = 2
};

enum TextDecoration {
    NONE = 0x0,
    UNDERLINE = 0x1,
    OVERLINE = 0x2,
    LINETHROUGH = 0x4,
};

enum class TextDecorationStyle {
    Solid,
    Double,
    Dotted,
    Dashed,
    Wavy,
};

enum class FontWeight {
    W100, // thin
    W200,
    W300,
    W400,
    W500,
    W600,
    W700,
    W800,
    W900,
};

enum class FontStyle {
    Normal,
    Italic,
};

enum class TextBaseline {
    Alphabetic,
    Ideographic,
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_TYPES_H
