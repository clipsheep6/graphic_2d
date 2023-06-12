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

#ifndef RS_SURFACE_FACTORY_H
#define RS_SURFACE_FACTORY_H

#include "render_context_base.h"

#include "rs_render_surface.h"
#include "drawing_context.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceFactory {
public:
    static std::shared_ptr<RSRenderSurface> CreateRSSurface(const PlatformName& platformName,
        const sptr<Surface>& surface, std::shared_ptr<DrawingContext> drawingContext = nullptr);
};
}
}
#endif