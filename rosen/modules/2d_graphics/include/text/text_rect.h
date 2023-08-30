/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef TEXT_RECT_H
#define TEXT_RECT_H

#include "impl_interface/rect_impl.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TextRect {

    TextRect();
    ~TextRect() = default;

    // TextRect MakeLTRB(float left, float top, float right, float bottom);
    // TextRect MakeXYWH(float x, float y, float w, float h);
    // TextRect MakeWH(float w, float h);
    // TextRect MakeRect(Rect& rect);
    // Rect GetRect() const;

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

private:
    std::shared_ptr<RectImpl> impl_;
    Rect rect_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
