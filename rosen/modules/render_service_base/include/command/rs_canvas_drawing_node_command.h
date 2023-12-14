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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_CANVAS_DRAWING_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_CANVAS_DRAWING_NODE_COMMAND_H

#include "command/rs_command_templates.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum RSCanvasDrawingNodeCommandType : uint16_t {
    CANVAS_DRAWING_NODE_CREATE,
    CANVAS_DRAWING_NODE_SET_SURFACE,
};

class RSB_EXPORT RSCanvasDrawingNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id, bool isSameLayerRender);
    static void SetSameRenderSurface(RSContext& context, NodeId id, SurfaceId surfaceId);
};

ADD_COMMAND(RSCanvasDrawingNodeCreate,
    ARG(CANVAS_DRAWING_NODE, CANVAS_DRAWING_NODE_CREATE, RSCanvasDrawingNodeCommandHelper::Create, SurfaceId, bool))
ADD_COMMAND(RSCanvasDrawingNodeSetSurface,
    ARG(CANVAS_DRAWING_NODE, CANVAS_DRAWING_NODE_SET_SURFACE,
        RSCanvasDrawingNodeCommandHelper::SetSameRenderSurface, NodeId, NodeId))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_CANVAS_DRAWING_NODE_COMMAND_H