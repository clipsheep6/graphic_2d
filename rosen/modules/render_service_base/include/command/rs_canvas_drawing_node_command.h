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

#include "rs_command_templates.h"

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum RSCanvasDrawingNodeCommandType : uint16_t {
    CANVAS_DRAWING_NODE_CREATE,
    CANVAS_DRAWING_NODE_SET_WIDTH,
    CANVAS_DRAWING_NODE_SET_HEIGHT,
};

class RSB_EXPORT RSCanvasDrawingNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id);
    static void SetWidth(RSContext& context, NodeId id, int32_t width);
    static void SetHeight(RSContext& context, NodeId id, int32_t height);
};

ADD_COMMAND(RSCanvasDrawingNodeCreate,
    ARG(CANVAS_DRAWING_NODE, CANVAS_DRAWING_NODE_CREATE, RSCanvasDrawingNodeCommandHelper::Create, NodeId))
ADD_COMMAND(RSCanvasDrawingNodeSetWidth, ARG(CANVAS_DRAWING_NODE, CANVAS_DRAWING_NODE_SET_WIDTH,
                                             RSCanvasDrawingNodeCommandHelper::SetWidth, NodeId, int32_t))
ADD_COMMAND(RSCanvasDrawingNodeSetHeight, ARG(CANVAS_DRAWING_NODE, CANVAS_DRAWING_NODE_SET_HEIGHT,
                                              RSCanvasDrawingNodeCommandHelper::SetHeight, NodeId, int32_t))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_CANVAS_DRAWING_NODE_COMMAND_H