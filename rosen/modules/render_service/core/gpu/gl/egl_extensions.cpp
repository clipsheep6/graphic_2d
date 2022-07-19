/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "egl_extensions.h"

#include <unordered_set>

#include "egl_api.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
EGLExtensions::EGLExtensions(std::string version, std::string extensions)
    : version_(std::move(version)), extensions_(std::move(extensions))
{
    Init();
}

void EGLExtensions::Init()
{
    auto exts = StringSplit(extensions_, ' ');
    std::unordered_set<std::string> extsSet(exts.cbegin(), exts.cend());

    hasFenceSync_ = (extsSet.count("EGL_KHR_fence_sync") > 0);
    hasWaitSync_ = (extsSet.count("EGL_KHR_wait_sync") > 0);
    hasNativeFenceSync_ = (extsSet.count("EGL_OHOS_native_fence_sync") > 0);
}
} // namespace Rosen
} // namespace OHOS
