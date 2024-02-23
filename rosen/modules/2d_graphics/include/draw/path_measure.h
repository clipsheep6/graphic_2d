/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef PATH_MEASURE_H
#define PATH_MEASURE_H

#include <memory>
#include <cstdint>

#include "impl_interface/path_measure_impl.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class DRAWING_API PathMeasure {
public:
    PathMeasure();
    PathMeasure(const Path& path, bool forceClosed, scalar resScale = 1);
    virtual ~PathMeasure() = default;

    void SetPath(const Path*, bool forceClosed);
    scalar GetLength();

private:
    std::shared_ptr<PathMeasureImpl> pathMeasureImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif