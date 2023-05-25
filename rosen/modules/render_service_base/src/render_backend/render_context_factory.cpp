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

#include "render_context_factory.h"

#include "ohos/render_context_ohos_gl.h"
#include "ohos/render_context_ohos_raster.h"
#ifdef RS_ENABLE_VK
#include "ohos/render_context_ohos_vk.h"
#endif
#ifdef ROSEN_WIN
#include "ohos/render_context_windows_gl.h"
#endif

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RenderContextBase> RenderContextFactory::CreateRenderContext(RenderType renderType)
{
    std::shared_ptr<RenderContextBase> renderContext;
#if defined(ROSEN_OHOS)
#if defined(RS_ENABLE_GL)
    if (renderType == RenderType::GLES) {
        renderContext = std::make_shared<RenderContextOhosGl>();
    } else {
        renderContext = std::make_shared<RenderContextOhosRaster>();
    }
#elif RS_ENABLE_VK
    renderContext = std::make_shared<RenderContextOhosVk>();
#else
    renderContext = std::make_shared<RenderContextOhosRaster>();
#endif
    renderContext->SetPlatformName(PlatformName::OHOS);
#elif ROSEN_WIN
    renderContext = std::make_shared<RenderContextWindowsGl>();
    renderContext->SetPlatformName(PlatformName::WINDOWS);
#endif
    renderContext->SetRenderType(renderType);
    ROSEN_LOGE("ZJ RenderContextFactory::CreateRenderContext");
    return renderContext;
}

}
}