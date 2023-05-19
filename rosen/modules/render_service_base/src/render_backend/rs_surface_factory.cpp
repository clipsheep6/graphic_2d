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

#include "rs_surface_factory.h"

#include "render_context.h"
#include "ohos/rs_surface_ohos_gl.h"
#include "ohos/rs_surface_ohos_raster.h"
#ifdef RS_ENABLE_VK
#include "ohos/rs_surface_ohos_vulkan.h"
#endif
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSSurface> RSSurfaceFactory::CreateRSSurface(const sptr<Surface>& surface)
{
    std::shared_ptr<RSSurface> rsSurface;
#ifdef ROSEN_OHOS
#ifdef RS_ENABLE_GL
            rsSurface = std::make_shared<RSSurfaceOhosGl>(surface);
#elif RS_ENABLE_VK
            rsSurface = std::make_shared<RSSurfaceOhosVk>(surface);
#else
            rsSurface = std::make_shared<RSSurfaceOhosRaster>(surface);
#endif
#endif
    LOGD("ZJ RSSurfaceFactory::CreateRSSurface");
    return rsSurface;
}
}
}