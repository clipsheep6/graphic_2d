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

#ifndef SKIA_RECT_H
#define SKIA_RECT_H

#include <include/core/SkRect.h>
#include "impl_interface/rect_impl.h"
#include "text/text_rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaRect : public RectImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaRect();
    ~SkiaRect() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    SkiaRect MakeLTRB(float left, float top, float right, float bottom);
    SkiaRect MakeXYWH(float x, float y, float w, float h);
    SkiaRect MakeWH(float w, float h);

    std::shared_ptr<SkRect> GetRect() const;
    void SetRect(const SkRect &rect);

    float *fLeft_ = nullptr;
    float *fTop_ = nullptr;
    float *fRight_ = nullptr;
    float *fBottom_ = nullptr;

private:
    std::shared_ptr<SkRect> rect_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_RECT_H
