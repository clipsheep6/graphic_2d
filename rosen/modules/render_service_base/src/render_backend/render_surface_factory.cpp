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

#include "render_surface_factory.h"

#include "render_context_base.h"
#include "ohos/render_surface_ohos_gl.h"
#include "ohos/render_surface_ohos_raster.h"
#ifdef RS_ENABLE_VK
#include "ohos/render_surface_ohos_vk.h"
#endif

namespace OHOS {
namespace Rosen {
std::shared_ptr<RenderSurface> RenderSurfaceFactory::CreateRenderSurface(
    std::shared_ptr<RenderContextBase>& renderContext,
    sptr<Surface>& surface)
{
    const std::string& platformName = renderContext->GetPlatformName();
    std::shared_ptr<RenderSurface> renderSurface;
    if (platformName == PLATFORM_OHOS) {
#ifdef RS_ENABLE_GL
            renderSurface = std::make_shared<RenderSurfaceOhosGl>(surface);
#elif RS_ENABLE_VK
            renderSurface = std::make_shared<RenderSurfaceOhosVk>(surface);
#else
            renderSurface = std::make_shared<RenderSurfaceOhosRaster>(surface);
#endif
        renderSurface->SetRenderContext(renderContext);
    }
    return renderSurface;
}
}
}