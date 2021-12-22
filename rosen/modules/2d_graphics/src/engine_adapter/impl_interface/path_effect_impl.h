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

#ifndef PATH_EFFECT_IMPL_H
#define PATH_EFFECT_IMPL_H

#include <memory>

#include "path_data.h"
#include "path_effect_data.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class PathDashStyle;
class PathEffectImpl {
public:
    PathEffectImpl() noexcept {}
    virtual ~PathEffectImpl() {}

    virtual void InitWithDash(const scalar intervals[], int count, scalar phase) = 0;
    virtual void InitWithPathDash(const PathData* d, scalar advance, scalar phase, PathDashStyle style) = 0;
    virtual void InitWithCorner(scalar radius) = 0;
    virtual void InitWithSum(const PathEffectData* d1, const PathEffectData* d2) = 0;
    virtual void InitWithCompose(const PathEffectData* d1, const PathEffectData* d2) = 0;
    const PathEffectData * GetPathEffectData() const { return pathEffectData_.get(); }
protected:
    std::shared_ptr<PathEffectData> pathEffectData_;
};
}
}
}
#endif