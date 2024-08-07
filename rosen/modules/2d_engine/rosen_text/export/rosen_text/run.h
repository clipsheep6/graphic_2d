/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_RUN_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_RUN_H

#include "draw/canvas.h"
#include "text/font.h"
#include "utils/point.h"

namespace OHOS {
namespace Rosen {
class Run {
public:
    virtual ~Run() = default;

    virtual Drawing::Font GetFont() const = 0;
    virtual size_t GetGlyphCount() const = 0;
    virtual std::vector<uint16_t> GetGlyphs(uint32_t start, uint32_t end) const = 0;
    virtual std::vector<Drawing::Point> GetPositions(uint32_t start, uint32_t end) = 0;
    virtual std::vector<Drawing::Point> GetOffsets() = 0;
    virtual void Paint(Drawing::Canvas *canvas, double x, double y) = 0;
    virtual void GetStringRange(uint32_t* location, uint32_t* length) = 0;
    virtual std::vector<uint32_t> GetStringIndices(uint32_t start, uint32_t end) = 0;
    virtual Drawing::Rect GetImageBounds() = 0;
    virtual float GetTypographicBounds(float* ascent, float* descent, float* leading) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_RUN_H
