/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_CANVAS_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_CANVAS_NODE_COMMAND_H


#include "command/rs_command_templates.h"
#include "common/rs_macros.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
namespace Rosen {

enum RSCanvasNodeCommandType : uint16_t {
    CANVAS_NODE_CREATE,
    CANVAS_NODE_UPDATE_RECORDING,
    CANVAS_NODE_CLEAR_RECORDING,
    CANVAS_NODE_SET_SURFACE,
};

#ifndef USE_ROSEN_DRAWING
class DrawCmdList;
#else
namespace Drawing {
class DrawCmdList;
}
#endif

class RSB_EXPORT RSCanvasNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id, bool isSameLayerRender = false);
#ifndef USE_ROSEN_DRAWING
    static void UpdateRecording(
        RSContext& context, NodeId id, std::shared_ptr<DrawCmdList> drawCmds, uint16_t modifierType);
#else
    static void UpdateRecording(
        RSContext& context, NodeId id, std::shared_ptr<Drawing::DrawCmdList> drawCmds, uint16_t modifierType);
#endif
    static void ClearRecording(RSContext& context, NodeId id);
    static void SetSameRenderSurface(RSContext& context, NodeId id, SurfaceId surfaceId);
private:
#ifndef USE_ROSEN_DRAWING
    static bool AddCmdToSingleFrameComposer(std::shared_ptr<RSCanvasRenderNode> node,
        std::shared_ptr<DrawCmdList> drawCmds, RSModifierType type);
#else
    static bool AddCmdToSingleFrameComposer(std::shared_ptr<RSCanvasRenderNode> node,
        std::shared_ptr<Drawing::DrawCmdList> drawCmds, RSModifierType type);
#endif
};

ADD_COMMAND(RSCanvasNodeCreate, ARG(CANVAS_NODE, CANVAS_NODE_CREATE, RSCanvasNodeCommandHelper::Create, NodeId, bool))
ADD_COMMAND(RSCanvasNodeSetSurface,
    ARG(CANVAS_NODE, CANVAS_NODE_SET_SURFACE, RSCanvasNodeCommandHelper::SetSameRenderSurface, NodeId, SurfaceId))
#ifndef USE_ROSEN_DRAWING
ADD_COMMAND(RSCanvasNodeUpdateRecording,
    ARG(CANVAS_NODE, CANVAS_NODE_UPDATE_RECORDING, RSCanvasNodeCommandHelper::UpdateRecording, NodeId,
        std::shared_ptr<DrawCmdList>, uint16_t))
#else
ADD_COMMAND(RSCanvasNodeUpdateRecording,
    ARG(CANVAS_NODE, CANVAS_NODE_UPDATE_RECORDING, RSCanvasNodeCommandHelper::UpdateRecording, NodeId,
        std::shared_ptr<Drawing::DrawCmdList>, uint16_t))
#endif
ADD_COMMAND(RSCanvasNodeClearRecording,
    ARG(CANVAS_NODE, CANVAS_NODE_CLEAR_RECORDING, RSCanvasNodeCommandHelper::ClearRecording, NodeId))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_CANVAS_NODE_COMMAND_H
