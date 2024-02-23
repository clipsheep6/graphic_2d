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

#include "skia_path_measure.h"
#include <memory>

#include "skia_path.h"
#include "include/core/SkPathMeasure.h"

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPathMeasure::SkiaPathMeasure(const Path& path, bool forceClosed, scalar resScale) noexcept
{
    auto skPathImpl = path.GetImpl<SkiaPath>();
    if (!skPathImpl) {
        LOGE("skiaPath nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }
    skPathMeature_ = SkPathMeasure(skPathImpl->GetPath(), forceClosed, resScale);
}

void SkiaPathMeasure::SetPath(const Path* path, bool forceClosed)
{
    auto skPathImpl = path->GetImpl<SkiaPath>();
    if (!skPathImpl) {
        LOGE("skiaPath nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }
    skPathMeature_.setPath(skPathImpl->GetPath(), forceClosed);
}

scalar SkiaPathMeasure::GetLength()
{
    return skPathMeature_.getLength();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS