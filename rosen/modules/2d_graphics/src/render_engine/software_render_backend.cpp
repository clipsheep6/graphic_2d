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

#include "software_render_backend.h"

namespace OHOS {
namespace Rosen {
void SoftwareRenderBackend::Initialize()
{

}

void SoftwareRenderBackend::MakeCurrent()
{

}

void SoftwareRenderBackend::SwapBuffers()
{

}

void SoftwareRenderBackend::SetSurface(const sptr<Surface> surface, int width, int height)
{
    // constexpr int32_t pixelBase = 16;
    // requestConfig_.width = (width % pixelBase == 0) ? width : ((width / pixelBase + 1) * pixelBase);
    // requestConfig_.height = (height % pixelBase == 0) ? height : ((height / pixelBase + 1) * pixelBase);
    // flushConfig_.damage.w = width;
    // flushConfig_.damage.h = height;
}

void SoftwareRenderBackend::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    // flushConfig_.damage.x = left;
    // flushConfig_.damage.y = top;
    // flushConfig_.damage.w = width;
    // flushConfig_.damage.h = height;
}
}
}