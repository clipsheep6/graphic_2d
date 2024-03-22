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
#include "egl_wrapper_surface.h"

#include "wrapper_log.h"

namespace OHOS {
EglWrapperSurface::EglWrapperSurface(EglWrapperDisplay *disp, EGLSurface surf)
    : EglWrapperObject(disp), surf_(surf)
{
    WLOGD("");
}

EglWrapperSurface::~EglWrapperSurface()
{
    WLOGD("");
    surf_ = nullptr;
}

EglWrapperSurface *EglWrapperSurface::GetWrapperSurface(EGLSurface surf)
{
    WLOGD("");
    return reinterpret_cast<EglWrapperSurface *>(surf);
}
} // namespace OHOS
