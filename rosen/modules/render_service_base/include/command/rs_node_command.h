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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H

#include "animation/rs_frame_rate_range.h"
#include "command/rs_command_templates.h"
#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties_def.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"

namespace OHOS {
namespace Rosen {

enum RSNodeCommandType : uint16_t {
    ADD_MODIFIER,
    REMOVE_MODIFIER,

    UPDATE_MODIFIER_BOOL,
    UPDATE_MODIFIER_FLOAT,
    UPDATE_MODIFIER_INT,
    UPDATE_MODIFIER_COLOR,
    UPDATE_MODIFIER_GRAVITY,
    UPDATE_MODIFIER_MATRIX3F,
    UPDATE_MODIFIER_QUATERNION,
    UPDATE_MODIFIER_FILTER_PTR,
    UPDATE_MODIFIER_IMAGE_PTR,
    UPDATE_MODIFIER_MASK_PTR,
    UPDATE_MODIFIER_PATH_PTR,
    UPDATE_MODIFIER_GRADIENT_BLUR_PTR,
    UPDATE_MODIFIER_SHADER_PTR,
    UPDATE_MODIFIER_VECTOR2F,
    UPDATE_MODIFIER_VECTOR4_BORDER_STYLE,
    UPDATE_MODIFIER_VECTOR4_COLOR,
    UPDATE_MODIFIER_VECTOR4F,
    UPDATE_MODIFIER_RRECT,
    UPDATE_MODIFIER_DRAW_CMD_LIST,
#ifndef USE_ROSEN_DRAWING
    UPDATE_MODIFIER_SKMATRIX,
#else
    UPDATE_MODIFIER_DRAWING_MATRIX,
#endif

    SET_FREEZE,
    MARK_DRIVEN_RENDER,
    MARK_DRIVEN_RENDER_ITEM_INDEX,
    MARK_DRIVEN_RENDER_FRAME_PAINT_STATE,
    MARK_CONTENT_CHANGED,
    SET_DRAW_REGION,

    REGISTER_GEOMETRY_TRANSITION,
    UNREGISTER_GEOMETRY_TRANSITION,

    MARK_NODE_GROUP,
    UPDATE_UI_FRAME_RATE_RANGE,
};

class RSB_EXPORT RSNodeCommandHelper {
public:
    static void AddModifier(RSContext& context, NodeId nodeId, const std::shared_ptr<RSRenderModifier>& modifier);
    static void RemoveModifier(RSContext& context, NodeId nodeId, PropertyId propertyId);

    template<typename T>
    static void UpdateModifier(RSContext& context, NodeId nodeId, T value, PropertyId id, bool isDelta)
    {
        std::shared_ptr<RSRenderPropertyBase> prop = std::make_shared<RSRenderProperty<T>>(value, id);
        auto& nodeMap = context.GetNodeMap();
        auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
        if (!node) {
            return;
        }
        auto modifier = node->GetModifier(id);
        if (modifier) {
            modifier->Update(prop, isDelta);
        }
    }

    static void SetFreeze(RSContext& context, NodeId nodeId, bool isFreeze);
    static void MarkNodeGroup(RSContext& context, NodeId nodeId, bool isNodeGroup);

    static void MarkDrivenRender(RSContext& context, NodeId nodeId, bool flag);
    static void MarkDrivenRenderItemIndex(RSContext& context, NodeId nodeId, int32_t index);
    static void MarkDrivenRenderFramePaintState(RSContext& context, NodeId nodeId, bool flag);
    static void MarkContentChanged(RSContext& context, NodeId nodeId, bool isChanged);
    static void SetDrawRegion(RSContext& context, NodeId nodeId, std::shared_ptr<RectF> rect);

    static void RegisterGeometryTransitionPair(RSContext& context, NodeId inNodeId, NodeId outNodeId);
    static void UnregisterGeometryTransitionPair(RSContext& context, NodeId inNodeId, NodeId outNodeId);
    static void UpdateUIFrameRateRange(RSContext& context, NodeId nodeId, const FrameRateRange& range);
};

ADD_COMMAND(RSAddModifier,
    ARG(RS_NODE, ADD_MODIFIER, RSNodeCommandHelper::AddModifier, NodeId, std::shared_ptr<RSRenderModifier>))
ADD_COMMAND(RSRemoveModifier,
    ARG(RS_NODE, REMOVE_MODIFIER, RSNodeCommandHelper::RemoveModifier, NodeId, PropertyId))

ADD_COMMAND(RSUpdatePropertyBool,
    ARG(RS_NODE, UPDATE_MODIFIER_BOOL, RSNodeCommandHelper::UpdateModifier<bool>,
        NodeId, bool, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyFloat,
    ARG(RS_NODE, UPDATE_MODIFIER_FLOAT, RSNodeCommandHelper::UpdateModifier<float>,
        NodeId, float, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyInt,
    ARG(RS_NODE, UPDATE_MODIFIER_INT, RSNodeCommandHelper::UpdateModifier<int>,
        NodeId, int, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyColor,
    ARG(RS_NODE, UPDATE_MODIFIER_COLOR, RSNodeCommandHelper::UpdateModifier<Color>,
        NodeId, Color, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyGravity,
    ARG(RS_NODE, UPDATE_MODIFIER_GRAVITY, RSNodeCommandHelper::UpdateModifier<Gravity>,
        NodeId, Gravity, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyMatrix3f,
    ARG(RS_NODE, UPDATE_MODIFIER_MATRIX3F, RSNodeCommandHelper::UpdateModifier<Matrix3f>,
        NodeId, Matrix3f, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyQuaternion,
    ARG(RS_NODE, UPDATE_MODIFIER_QUATERNION, RSNodeCommandHelper::UpdateModifier<Quaternion>,
        NodeId, Quaternion, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyFilter,
    ARG(RS_NODE, UPDATE_MODIFIER_FILTER_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSFilter>>,
        NodeId, std::shared_ptr<RSFilter>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyImage,
    ARG(RS_NODE, UPDATE_MODIFIER_IMAGE_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSImage>>,
        NodeId, std::shared_ptr<RSImage>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyMask,
    ARG(RS_NODE, UPDATE_MODIFIER_MASK_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSMask>>,
        NodeId, std::shared_ptr<RSMask>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyPath,
    ARG(RS_NODE, UPDATE_MODIFIER_PATH_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSPath>>,
        NodeId, std::shared_ptr<RSPath>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyLinearGradientBlurPara,
    ARG(RS_NODE, UPDATE_MODIFIER_GRADIENT_BLUR_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSLinearGradientBlurPara>>,
        NodeId, std::shared_ptr<RSLinearGradientBlurPara>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyShader,
    ARG(RS_NODE, UPDATE_MODIFIER_SHADER_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSShader>>,
        NodeId, std::shared_ptr<RSShader>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyVector2f,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR2F, RSNodeCommandHelper::UpdateModifier<Vector2f>,
        NodeId, Vector2f, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyBorderStyle,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR4_BORDER_STYLE, RSNodeCommandHelper::UpdateModifier<Vector4<uint32_t>>,
        NodeId, Vector4<uint32_t>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyVector4Color,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR4_COLOR, RSNodeCommandHelper::UpdateModifier<Vector4<Color>>,
        NodeId, Vector4<Color>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyVector4f,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR4F, RSNodeCommandHelper::UpdateModifier<Vector4f>,
        NodeId, Vector4f, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyRRect,
    ARG(RS_NODE, UPDATE_MODIFIER_RRECT, RSNodeCommandHelper::UpdateModifier<RRect>,
        NodeId, RRect, PropertyId, bool))
#ifndef USE_ROSEN_DRAWING
ADD_COMMAND(RSUpdatePropertyDrawCmdList,
    ARG(RS_NODE, UPDATE_MODIFIER_DRAW_CMD_LIST, RSNodeCommandHelper::UpdateModifier<DrawCmdListPtr>,
        NodeId, DrawCmdListPtr, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertySkMatrix,
    ARG(RS_NODE, UPDATE_MODIFIER_SKMATRIX, RSNodeCommandHelper::UpdateModifier<SkMatrix>,
        NodeId, SkMatrix, PropertyId, bool))
#else
ADD_COMMAND(RSUpdatePropertyDrawCmdList,
    ARG(RS_NODE, UPDATE_MODIFIER_DRAW_CMD_LIST, RSNodeCommandHelper::UpdateModifier<Drawing::DrawCmdListPtr>,
        NodeId, Drawing::DrawCmdListPtr, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyDrawingMatrix,
    ARG(RS_NODE, UPDATE_MODIFIER_DRAWING_MATRIX, RSNodeCommandHelper::UpdateModifier<Drawing::Matrix>,
        NodeId, Drawing::Matrix, PropertyId, bool))
#endif

ADD_COMMAND(RSSetFreeze,
    ARG(RS_NODE, SET_FREEZE, RSNodeCommandHelper::SetFreeze, NodeId, bool))
ADD_COMMAND(RSMarkNodeGroup,
    ARG(RS_NODE, MARK_NODE_GROUP, RSNodeCommandHelper::MarkNodeGroup, NodeId, bool))

ADD_COMMAND(RSMarkDrivenRender,
    ARG(RS_NODE, MARK_DRIVEN_RENDER, RSNodeCommandHelper::MarkDrivenRender, NodeId, bool))
ADD_COMMAND(RSMarkDrivenRenderItemIndex,
    ARG(RS_NODE, MARK_DRIVEN_RENDER_ITEM_INDEX, RSNodeCommandHelper::MarkDrivenRenderItemIndex, NodeId, int32_t))
ADD_COMMAND(RSMarkDrivenRenderFramePaintState,
    ARG(RS_NODE, MARK_DRIVEN_RENDER_FRAME_PAINT_STATE,
        RSNodeCommandHelper::MarkDrivenRenderFramePaintState, NodeId, bool))
ADD_COMMAND(RSMarkContentChanged,
    ARG(RS_NODE, MARK_CONTENT_CHANGED, RSNodeCommandHelper::MarkContentChanged, NodeId, bool))

ADD_COMMAND(RSSetDrawRegion,
    ARG(RS_NODE, SET_DRAW_REGION, RSNodeCommandHelper::SetDrawRegion,
        NodeId, std::shared_ptr<RectF>))

ADD_COMMAND(RSRegisterGeometryTransitionNodePair,
    ARG(RS_NODE, REGISTER_GEOMETRY_TRANSITION, RSNodeCommandHelper::RegisterGeometryTransitionPair, NodeId, NodeId))
ADD_COMMAND(RSUnregisterGeometryTransitionNodePair,
    ARG(RS_NODE, UNREGISTER_GEOMETRY_TRANSITION, RSNodeCommandHelper::UnregisterGeometryTransitionPair, NodeId, NodeId))
ADD_COMMAND(RSUpdateUIFrameRateRange,
    ARG(RS_NODE, UPDATE_UI_FRAME_RATE_RANGE, RSNodeCommandHelper::UpdateUIFrameRateRange, NodeId, FrameRateRange))
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H
