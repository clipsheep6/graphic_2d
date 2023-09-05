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

#ifndef TEXT_FONT_H
#define TEXT_FONT_H

#include "impl_interface/font_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

struct RunBuffer {
    uint16_t* glyphs;
    float* pos;
    char* utf8Text;
    uint32_t* clusters;
};

class Font {
public:
    Font();
    ~Font() {};

    void SetSize(scalar textSize);

    //float GetMetrics(FontMetricsImpl *metrics) const override;
    
    // virtual void SetTypeface(const std::shared_ptr<SkiaTypeface> tf);

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

private:
    std::shared_ptr<FontImpl> impl_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
