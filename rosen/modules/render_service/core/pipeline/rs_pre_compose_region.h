/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RS_PRE_COMPOSE_REGION_H
#define RS_PRE_COMPOSE_REGION_H

#include <list>
#include <memory>
#include <refbase.h>
#include "pipeline/rs_paint_filter_canvas.h"
#include "rs_pre_compose_group.h"
#include "pipeline/rs_display_render_node.h"


namespace OHOS {
namespace Rosen {
class RSPreComposeManager : public RefBase {
public:
    static sptr<RSPreComposeManager> GetInstance();
    void Init(RenderContext *context, std::shared_ptr<RSPaintFilterCanvas> canvas);
};
} // namespace Rosen
} // namespace OHOS
 #endif // RS_PRE_COMPOSE_REGION_H