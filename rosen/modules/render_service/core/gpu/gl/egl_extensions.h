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

#ifndef RENDER_SERVICE_CORE_GPU_EGL_EXTENSIONS_H
#define RENDER_SERVICE_CORE_GPU_EGL_EXTENSIONS_H

#include <string>

namespace OHOS {
namespace Rosen {
class EGLExtensions {
public:
    EGLExtensions(std::string version, std::string extensions);
    ~EGLExtensions() noexcept = default;

    EGLExtensions(const EGLExtensions&) = delete;
    void operator=(const EGLExtensions&) = delete;

    const std::string& Version() const
    {
        return version_;
    }

    const std::string& Extensions() const
    {
        return extensions_;
    }

    bool HasFenceSync() const
    {
        return hasFenceSync_;
    }

    bool HasNativeFenceSync() const
    {
        return hasNativeFenceSync_;
    }

    bool HasWaitSync() const
    {
        return hasWaitSync_;
    }

private:
    void Init();

    std::string version_;
    std::string extensions_;

    bool hasFenceSync_ = false;
    bool hasNativeFenceSync_ = false;
    bool hasWaitSync_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_GPU_EGL_EXTENSIONS_H
