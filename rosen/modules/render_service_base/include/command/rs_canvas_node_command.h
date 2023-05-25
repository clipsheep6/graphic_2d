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

#include "common/rs_macros.h"
#include "modifier/rs_modifier_type.h"
#include "rs_command_templates.h"

#ifdef USE_ROSEN_DRAWING
#include "recording/draw_cmd_list.h"
#endif

namespace OHOS {
namespace Rosen {

enum RSCanvasNodeCommandType : uint16_t {
    CANVAS_NODE_CREATE,
    CANVAS_NODE_UPDATE_RECORDING,
    CANVAS_NODE_CLEAR_RECORDING,
};

#ifndef USE_ROSEN_DRAWING
class DrawCmdList;
#endif

class RSB_EXPORT RSCanvasNodeCommandHelper {
public:
    static void Create(RSContext& context, NodeId id);
#ifndef USE_ROSEN_DRAWING
    static void UpdateRecording(
        RSContext& context, NodeId id, std::shared_ptr<DrawCmdList> drawCmds, uint16_t modifierType);
#else
    static void UpdateRecording(
        RSContext& context, NodeId id, std::shared_ptr<Drawing::DrawCmdList> drawCmds, uint16_t modifierType);
#endif
    static void ClearRecording(RSContext& context, NodeId id);
};

ADD_COMMAND(RSCanvasNodeCreate, ARG(CANVAS_NODE, CANVAS_NODE_CREATE, RSCanvasNodeCommandHelper::Create, NodeId))
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
