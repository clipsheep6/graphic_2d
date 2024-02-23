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

#ifndef SKIA_PATH_MEASURE_H
#define SKIA_PATH_MEASURE_H

#include <memory>
#include <vector>

#include "include/core/SkPathMeasure.h"

#include "impl_interface/path_measure_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPathMeasure : public PathMeasureImpl  {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaPathMeasure() noexcept = default;
    SkiaPathMeasure(const Path& path, bool forceClosed, scalar resScale = 1) noexcept;
    ~SkiaPathMeasure() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void SetPath(const Path*, bool forceClosed) override;
    scalar GetLength() override;

private:
    SkPathMeasure skPathMeature_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif