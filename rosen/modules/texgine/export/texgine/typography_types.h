/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_TYPES_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_TYPES_H

#include <memory>
#include <optional>
#include <vector>

#include <include/core/SkPaint.h>

#include "texgine/font_providers.h"

class SkCanvas;

namespace Texgine {
/** \enum BreakStrategy
 *  BreakStrategy is a strategy on how to wrap when
 *  there are multiple lines of text.
 */
enum class BreakStrategy {
    Greedy,                 //!< this strategy wraps lines when they have to.
    HighQuality,            //!< this strategy aims to make the line-wrapped result
                            //   look as neat as possible, not jagged.
    Balanced = HighQuality, //!< to be done.
};

/** \enum FontStyle
 *  FontStyle is font italic style.
 */
enum class FontStyle {
    Normal, //!< use no italic.
    Italic, //!< use italic.
    MAX,    //!< use for check.
};

/** \enum FontWeight
 *  FontWeight is font weight style.
 */
enum class FontWeight {
    W100 = 0, //!< weight value 100 thin.
    W200 = 1, //!< weight value 200.
    W300 = 2, //!< weight value 300.
    W400 = 3, //!< weight value 400 normal.
    W500 = 4, //!< weight value 500.
    W600 = 5, //!< weight value 600.
    W700 = 6, //!< weight value 700 bold.
    W800 = 7, //!< weight value 800.
    W900 = 8, //!< weight value 900.
    MAX,      //!< use for check.
};

/** \enum TextAlign
 *  TextAlign is text alignment style.
 *  The bit field has an alignment style and trim style.
 */
enum class TextAlign {
    Default = 0,   //!< alias for Start.
    Start = 1,     //!< LTR: align left, RTL: align right.
    End = 2,       //!< LTR: align right, RTL: align left.
    Left = 3,      //!< align left.
    Right = 4,     //!< align right.
    Center = 5,    //!< center alignment.
    Justify = 6,   //!< justified alignment.
    Scattered = 7, //!< scatter alignment.
    // trim part
    Trim = 0x8,    //!< when trim is enabled, whitespace is not considered.
    StartTrim = Start | Trim,
    EndTrim = End | Trim,
    LeftTrim = Left | Trim,
    RightTrim = Right | Trim,
    CenterTrim = Center | Trim,
    JustifyTrim = Justify | Trim,
    ScatteredTrim = Scattered | Trim,
};
TextAlign operator |(TextAlign lhs, TextAlign rhs);
TextAlign operator &(TextAlign lhs, TextAlign rhs);
TextAlign operator ^(TextAlign lhs, TextAlign rhs);
TextAlign operator ~(TextAlign lhs);
void operator &=(TextAlign &lhs, const TextAlign &rhs);
void operator |=(TextAlign &lhs, const TextAlign &rhs);
void operator ^=(TextAlign &lhs, const TextAlign &rhs);

/** \enum TextBaseline
 *  TextBaseline is text baseline style.
 */
enum class TextBaseline {
    Alphabetic,  //!< baseline is alphabetic.
    Ideographic, //!< baseline is ideographic.
};

/** \enum TextDecoration
 *  TextDecoration is text decoration style.
 *  Use bit fields for storage.
 */
enum class TextDecoration : int {
    None = 0x0,        //!< no style.
    Underline = 0x1,   //!< draw underline.
    Overline = 0x2,    //!< draw overline.
    Linethrough = 0x4, //!< draw line through text.
    Baseline = 0x8,    //!< draw baseline.
};
TextDecoration operator &(TextDecoration const &lhs, TextDecoration const &rhs);
TextDecoration operator |(TextDecoration const &lhs, TextDecoration const &rhs);
TextDecoration operator ^(TextDecoration const &lhs, TextDecoration const &rhs);
TextDecoration operator +(TextDecoration const &lhs, TextDecoration const &rhs);
void operator &=(TextDecoration &lhs, TextDecoration const &rhs);
void operator |=(TextDecoration &lhs, TextDecoration const &rhs);
void operator ^=(TextDecoration &lhs, TextDecoration const &rhs);
void operator +=(TextDecoration &lhs, TextDecoration const &rhs);

/** \enum TextDecorationStyle
 *  TextDecorationStyle is decoration line style.
 */
enum class TextDecorationStyle {
    Solid,  //!< solid line
    Double, //!< double line
    Dotted, //!< dotted line
    Dashed, //!< dashed line
    Wavy,   //!< wavy line
};

/** \enum TextDirection
 *  TextDirection is text direction style.
 */
enum class TextDirection {
    LTR, //!< left to right
    RTL, //!< right to left
};

/** \enum WordBreakType
 *  WordBreakType is the type of word break when multiline text wraps.
 */
enum class WordBreakType {
    Normal,    //!< to be done.
    BreakAll,  //!< break occur after any characters.
    BreakWord, //!< break only occur after word.
};
} // namespace Texgine

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_TYPES_H
