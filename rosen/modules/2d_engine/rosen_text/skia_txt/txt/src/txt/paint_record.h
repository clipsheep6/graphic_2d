/*
 * Copyright 2017 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIB_TXT_SRC_PAINT_RECORD_H_
#define LIB_TXT_SRC_PAINT_RECORD_H_

#include "draw/pen.h"
#include "draw/brush.h"
#include "third_party/skia/include/core/SkColor.h"

namespace txt {

struct PaintRecord {
    using RSBrush = OHOS::Rosen::Drawing::Brush;
    using RSPen = OHOS::Rosen::Drawing::Pen;
    using RSColor = OHOS::Rosen::Drawing::Color;

    RSBrush brush;
    RSPen pen;

    PaintRecord() {}
    PaintRecord(RSBrush brush, RSPen pen) : brush(brush), pen(pen) {}

    void SetColor(const RSColor &c) {
        brush.SetColor(c);
        pen.SetColor(c);
    }

    void SetColor(SkColor c) {
        SetColor(ToRSColor(c));
    }

    bool operator ==(const PaintRecord& rhs) const {
        return brush == rhs.brush && pen == rhs.pen;
    }

    bool operator !=(const PaintRecord& rhs) const {
        return !(*this == rhs);
    }

    static RSColor ToRSColor(SkColor c) {
        return RSColor(
            SkColorGetR(c),
            SkColorGetG(c),
            SkColorGetB(c),
            SkColorGetA(c)
        );
    }
};

}  // namespace txt

#endif  // LIB_TXT_SRC_PAINT_RECORD_H_
