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

#include "skia_color_space.h"
#include "skia_color_space_data.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaColorSpace::SkiaColorSpace() noexcept
{
    colorSpaceData_ = std::make_shared<SkiaColorSpaceData>();
    colorSpaceData_->type = ColorSpaceData::SKIA_COLOR_SPACE;
}

void SkiaColorSpace::InitWithSRGB()
{
    auto s = SkColorSpace::MakeSRGB();
    colorSpaceData_->SetColorSpace(s.release());
}

void SkiaColorSpace::InitWithSRGBLinear()
{
    auto s = SkColorSpace::MakeSRGBLinear();
    colorSpaceData_->SetColorSpace(s.release());
}

sk_sp<SkColorSpace> SkiaColorSpace::MutableColorSpace()
{
    return sk_sp<SkColorSpace>(static_cast<SkColorSpace*>(colorSpaceData_->GetColorSpace()));
}
}
}
}