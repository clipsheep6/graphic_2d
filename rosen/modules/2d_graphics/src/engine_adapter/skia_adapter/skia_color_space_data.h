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

#ifndef SKIA_COLOR_SPACE_DATA_H
#define SKIA_COLOR_SPACE_DATA_H

#include "include/core/SkColorSpace.h"

#include "impl_interface/color_space_data.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct SkiaColorSpaceData : ColorSpaceData {
public:
    SkiaColorSpaceData() noexcept : ColorSpaceData(), space(nullptr) {}
    void SetColorSpace(void* s) override { space.reset(static_cast<SkColorSpace*>(s)); }
    void* GetColorSpace() const override { return space.get(); }
    sk_sp<SkColorSpace> space;
};
}
}
}
#endif