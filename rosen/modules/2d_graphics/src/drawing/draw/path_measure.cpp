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

#include "draw/path_measure.h"

#include "impl_factory.h"
#include "impl_interface/path_measure_impl.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
PathMeasure::PathMeasure() : pathMeasureImpl_(ImplFactory::CreatePathMeasureImpl()) {}

PathMeasure::PathMeasure(const Path& path, bool forceClosed, scalar resScale)
    : pathMeasureImpl_(ImplFactory::CreatePathMeasureImpl(path, forceClosed, resScale)) {}

void PathMeasure::SetPath(const Path* path, bool forceClosed)
{
    if (!pathMeasureImpl_) {
        LOGE("pathMeasureImpl_ nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }
    pathMeasureImpl_->SetPath(path, forceClosed);
}

scalar PathMeasure::GetLength()
{
    if (!pathMeasureImpl_) {
        LOGE("pathMeasureImpl_ nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return 0;
    }
    return pathMeasureImpl_->GetLength();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
