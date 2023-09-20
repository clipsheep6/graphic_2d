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

#include "recording/draw_cmd.h"

#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd_list.h"
#include "recording/mem_allocator.h"
#include "recording/op_item.h"
#include "recording/path_cmd_list.h"
#include "recording/color_filter_cmd_list.h"
#include "recording/color_space_cmd_list.h"
#include "recording/image_filter_cmd_list.h"
#include "recording/mask_filter_cmd_list.h"
#include "recording/path_effect_cmd_list.h"
#include "recording/shader_effect_cmd_list.h"
#include "recording/region_cmd_list.h"

#include "draw/path.h"
#include "draw/brush.h"
#include "effect/color_filter.h"
#include "effect/color_space.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::unordered_map<uint32_t, CanvasPlayer::PlaybackFunc> CanvasPlayer::opPlaybackFuncLUT_ = {
    { DrawOpItem::POINT_OPITEM,             DrawPointOpItem::Playback },
    { DrawOpItem::POINTS_OPITEM,            DrawPointsOpItem::Playback },
    { DrawOpItem::LINE_OPITEM,              DrawLineOpItem::Playback },
    { DrawOpItem::RECT_OPITEM,              DrawRectOpItem::Playback },
    { DrawOpItem::ROUND_RECT_OPITEM,        DrawRoundRectOpItem::Playback },
    { DrawOpItem::NESTED_ROUND_RECT_OPITEM, DrawNestedRoundRectOpItem::Playback },
    { DrawOpItem::ARC_OPITEM,               DrawArcOpItem::Playback },
    { DrawOpItem::PIE_OPITEM,               DrawPieOpItem::Playback },
    { DrawOpItem::OVAL_OPITEM,              DrawOvalOpItem::Playback },
    { DrawOpItem::CIRCLE_OPITEM,            DrawCircleOpItem::Playback },
    { DrawOpItem::PATH_OPITEM,              DrawPathOpItem::Playback },
    { DrawOpItem::BACKGROUND_OPITEM,        DrawBackgroundOpItem::Playback },
    { DrawOpItem::SHADOW_OPITEM,            DrawShadowOpItem::Playback },
    { DrawOpItem::COLOR_OPITEM,             DrawColorOpItem::Playback },
    { DrawOpItem::BITMAP_OPITEM,            DrawBitmapOpItem::Playback },
    { DrawOpItem::IMAGE_OPITEM,             DrawImageOpItem::Playback },
    { DrawOpItem::IMAGE_RECT_OPITEM,        DrawImageRectOpItem::Playback },
    { DrawOpItem::PICTURE_OPITEM,           DrawPictureOpItem::Playback },
    { DrawOpItem::CLIP_RECT_OPITEM,         ClipRectOpItem::Playback },
    { DrawOpItem::CLIP_IRECT_OPITEM,        ClipIRectOpItem::Playback },
    { DrawOpItem::CLIP_ROUND_RECT_OPITEM,   ClipRoundRectOpItem::Playback },
    { DrawOpItem::CLIP_PATH_OPITEM,         ClipPathOpItem::Playback },
    { DrawOpItem::CLIP_REGION_OPITEM,       ClipRegionOpItem::Playback },
    { DrawOpItem::SET_MATRIX_OPITEM,        SetMatrixOpItem::Playback },
    { DrawOpItem::RESET_MATRIX_OPITEM,      ResetMatrixOpItem::Playback },
    { DrawOpItem::CONCAT_MATRIX_OPITEM,     ConcatMatrixOpItem::Playback },
    { DrawOpItem::TRANSLATE_OPITEM,         TranslateOpItem::Playback },
    { DrawOpItem::SCALE_OPITEM,             ScaleOpItem::Playback },
    { DrawOpItem::ROTATE_OPITEM,            RotateOpItem::Playback },
    { DrawOpItem::SHEAR_OPITEM,             ShearOpItem::Playback },
    { DrawOpItem::FLUSH_OPITEM,             FlushOpItem::Playback },
    { DrawOpItem::CLEAR_OPITEM,             ClearOpItem::Playback },
    { DrawOpItem::SAVE_OPITEM,              SaveOpItem::Playback },
    { DrawOpItem::SAVE_LAYER_OPITEM,        SaveLayerOpItem::Playback },
    { DrawOpItem::RESTORE_OPITEM,           RestoreOpItem::Playback },
    { DrawOpItem::DISCARD_OPITEM,           DiscardOpItem::Playback },
    { DrawOpItem::ATTACH_PEN_OPITEM,        AttachPenOpItem::Playback },
    { DrawOpItem::ATTACH_BRUSH_OPITEM,      AttachBrushOpItem::Playback },
    { DrawOpItem::DETACH_PEN_OPITEM,        DetachPenOpItem::Playback },
    { DrawOpItem::DETACH_BRUSH_OPITEM,      DetachBrushOpItem::Playback },
    { DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM, ClipAdaptiveRoundRectOpItem::Playback},
    { DrawOpItem::ADAPTIVE_IMAGE_OPITEM,    DrawAdaptiveImageOpItem::Playback},
    { DrawOpItem::ADAPTIVE_PIXELMAP_OPITEM, DrawAdaptivePixelMapOpItem::Playback},
};

CanvasPlayer::CanvasPlayer(Canvas& canvas, const CmdList& cmdList, const Rect& rect)
    : canvas_(canvas), cmdList_(cmdList), rect_(rect) {}

bool CanvasPlayer::Playback(uint32_t type, const void* opItem)
{
    if (type == DrawOpItem::OPITEM_HEAD) {
        return true;
    }

    auto it = opPlaybackFuncLUT_.find(type);
    if (it == opPlaybackFuncLUT_.end() || it->second == nullptr) {
        return false;
    }

    auto func = it->second;
    (*func)(*this, opItem);

    return true;
}

/* OpItem */
DrawPointOpItem::DrawPointOpItem(const Point& point) : DrawOpItem(POINT_OPITEM), point_(point) {}

void DrawPointOpItem::Playback(CanvasPlayer& player, const void *opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawPointOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawPointOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawPoint(point_);
}

DrawPointsOpItem::DrawPointsOpItem(PointMode mode, const std::pair<uint32_t, size_t> pts)
    : DrawOpItem(POINTS_OPITEM), mode_(mode), pts_(pts) {}

void DrawPointsOpItem::Playback(CanvasPlayer& player, const void *opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawPointsOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawPointsOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto pts = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, pts_);
    canvas.DrawPoints(mode_, pts.size(), pts.data());
}

DrawLineOpItem::DrawLineOpItem(const Point& startPt, const Point& endPt) : DrawOpItem(LINE_OPITEM),
    startPt_(startPt), endPt_(endPt) {}

void DrawLineOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawLineOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawLineOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawLine(startPt_, endPt_);
}

DrawRectOpItem::DrawRectOpItem(const Rect& rect) : DrawOpItem(RECT_OPITEM), rect_(rect) {}

void DrawRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawRectOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawRectOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawRect(rect_);
}

DrawRoundRectOpItem::DrawRoundRectOpItem(std::pair<uint32_t, size_t> radiusXYData, const Rect& rect)
    : DrawOpItem(ROUND_RECT_OPITEM), radiusXYData_(radiusXYData), rect_(rect) {}

void DrawRoundRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawRoundRectOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawRoundRectOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto radiusXYData = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, radiusXYData_);
    RoundRect roundRect(rect_, radiusXYData);

    canvas.DrawRoundRect(roundRect);
}

DrawNestedRoundRectOpItem::DrawNestedRoundRectOpItem(const std::pair<uint32_t, size_t> outerRadiusXYData,
    const Rect& outerRect, const std::pair<uint32_t, size_t> innerRadiusXYData, const Rect& innerRect)
    : DrawOpItem(NESTED_ROUND_RECT_OPITEM), outerRadiusXYData_(outerRadiusXYData),
    innerRadiusXYData_(innerRadiusXYData), outerRect_(outerRect), innerRect_(innerRect) {}

void DrawNestedRoundRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawNestedRoundRectOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawNestedRoundRectOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto outerRadiusXYData = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, outerRadiusXYData_);
    RoundRect outerRect(outerRect_, outerRadiusXYData);
    auto innerRadiusXYData = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, innerRadiusXYData_);
    RoundRect innerRect(innerRect_, innerRadiusXYData);

    canvas.DrawNestedRoundRect(outerRect, innerRect);
}

DrawArcOpItem::DrawArcOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle)
    : DrawOpItem(ARC_OPITEM), rect_(rect), startAngle_(startAngle), sweepAngle_(sweepAngle) {}

void DrawArcOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawArcOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawArcOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawArc(rect_, startAngle_, sweepAngle_);
}

DrawPieOpItem::DrawPieOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle)
    : DrawOpItem(PIE_OPITEM), rect_(rect), startAngle_(startAngle), sweepAngle_(sweepAngle) {}

void DrawPieOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawPieOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawPieOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawPie(rect_, startAngle_, sweepAngle_);
}

DrawOvalOpItem::DrawOvalOpItem(const Rect& rect) : DrawOpItem(OVAL_OPITEM), rect_(rect) {}

void DrawOvalOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawOvalOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawOvalOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawOval(rect_);
}

DrawCircleOpItem::DrawCircleOpItem(const Point& centerPt, scalar radius)
    : DrawOpItem(CIRCLE_OPITEM), centerPt_(centerPt), radius_(radius) {}

void DrawCircleOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawCircleOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawCircleOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawCircle(centerPt_, radius_);
}

DrawPathOpItem::DrawPathOpItem(const CmdListHandle& path) : DrawOpItem(PATH_OPITEM), path_(path) {}

void DrawPathOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawPathOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawPathOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto path = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, path_);
    if (path == nullptr) {
        LOGE("path is nullptr!");
        return;
    }

    canvas.DrawPath(*path);
}

DrawBackgroundOpItem::DrawBackgroundOpItem(const BrushHandle& brushHandle)
    : DrawOpItem(BACKGROUND_OPITEM), brushHandle_(brushHandle) {}

void DrawBackgroundOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawBackgroundOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawBackgroundOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
        cmdList, brushHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
        cmdList, brushHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
        cmdList, brushHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
        cmdList, brushHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
        cmdList, brushHandle_.maskFilterHandle);

    Filter filter;
    filter.SetColorFilter(colorFilter);
    filter.SetImageFilter(imageFilter);
    filter.SetMaskFilter(maskFilter);
    filter.SetFilterQuality(brushHandle_.filterQuality);

    const Color4f color4f = { brushHandle_.color.GetRedF(), brushHandle_.color.GetGreenF(),
        brushHandle_.color.GetBlueF(), brushHandle_.color.GetAlphaF() };

    Brush brush;
    brush.SetColor(color4f, colorSpace);
    brush.SetShaderEffect(shaderEffect);
    brush.SetBlendMode(brushHandle_.mode);
    brush.SetAntiAlias(brushHandle_.isAntiAlias);
    brush.SetFilter(filter);

    canvas.DrawBackground(brush);
}

DrawShadowOpItem::DrawShadowOpItem(const CmdListHandle& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
    : DrawOpItem(SHADOW_OPITEM), path_(path), planeParams_(planeParams), devLightPos_(devLightPos),
    lightRadius_(lightRadius), ambientColor_(ambientColor), spotColor_(spotColor), flag_(flag) {}

void DrawShadowOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawShadowOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawShadowOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto path = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, path_);
    if (path == nullptr) {
        LOGE("path is nullptr!");
        return;
    }

    canvas.DrawShadow(*path, planeParams_, devLightPos_, lightRadius_, ambientColor_, spotColor_, flag_);
}

DrawColorOpItem::DrawColorOpItem(ColorQuad color, BlendMode mode) : DrawOpItem(COLOR_OPITEM),
    color_(color), mode_(mode) {}

void DrawColorOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawColorOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawColorOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawColor(color_, mode_);
}

DrawBitmapOpItem::DrawBitmapOpItem(const ImageHandle& bitmap, scalar px, scalar py)
    : DrawOpItem(BITMAP_OPITEM), bitmap_(bitmap), px_(px), py_(py) {}

void DrawBitmapOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawBitmapOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawBitmapOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto bitmap = CmdListHelper::GetBitmapFromCmdList(cmdList, bitmap_);
    if (bitmap == nullptr) {
        return;
    }

    canvas.DrawBitmap(*bitmap, px_, py_);
}

DrawImageOpItem::DrawImageOpItem(const ImageHandle& image, scalar px, scalar py,
    const SamplingOptions& samplingOptions) : DrawOpItem(IMAGE_OPITEM),
    image_(std::move(image)), px_(px), py_(py), samplingOptions_(samplingOptions) {}

void DrawImageOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawImageOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawImageOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
    if (image == nullptr) {
        return;
    }

    canvas.DrawImage(*image, px_, py_, samplingOptions_);
}

DrawImageRectOpItem::DrawImageRectOpItem(const ImageHandle& image, const Rect& src, const Rect& dst,
    const SamplingOptions& sampling, SrcRectConstraint constraint) : DrawOpItem(IMAGE_RECT_OPITEM),
    image_(image), src_(src), dst_(dst), sampling_(sampling), constraint_(constraint) {}

void DrawImageRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawImageRectOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawImageRectOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
    if (image == nullptr) {
        return;
    }

    canvas.DrawImageRect(*image, src_, dst_, sampling_, constraint_);
}

DrawPictureOpItem::DrawPictureOpItem(const ImageHandle& picture) : DrawOpItem(PICTURE_OPITEM), picture_(picture) {}

void DrawPictureOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawPictureOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawPictureOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto picture = CmdListHelper::GetPictureFromCmdList(cmdList, picture_);
    if (picture == nullptr) {
        return;
    }

    canvas.DrawPicture(*picture);
}

ClipRectOpItem::ClipRectOpItem(const Rect& rect, ClipOp op, bool doAntiAlias)
    : DrawOpItem(CLIP_RECT_OPITEM), rect_(rect), clipOp_(op), doAntiAlias_(doAntiAlias) {}

void ClipRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ClipRectOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ClipRectOpItem::Playback(Canvas& canvas) const
{
    canvas.ClipRect(rect_, clipOp_, doAntiAlias_);
}

ClipIRectOpItem::ClipIRectOpItem(const RectI& rect, ClipOp op)
    : DrawOpItem(CLIP_IRECT_OPITEM), rect_(rect), clipOp_(op) {}

void ClipIRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ClipIRectOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ClipIRectOpItem::Playback(Canvas& canvas) const
{
    canvas.ClipIRect(rect_, clipOp_);
}

ClipRoundRectOpItem::ClipRoundRectOpItem(std::pair<uint32_t, size_t> radiusXYData, const Rect& rect, ClipOp op,
    bool doAntiAlias) : DrawOpItem(CLIP_ROUND_RECT_OPITEM), radiusXYData_(radiusXYData), rect_(rect), clipOp_(op),
    doAntiAlias_(doAntiAlias) {}

void ClipRoundRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ClipRoundRectOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void ClipRoundRectOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto radiusXYData = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, radiusXYData_);
    RoundRect roundRect(rect_, radiusXYData);

    canvas.ClipRoundRect(roundRect, clipOp_, doAntiAlias_);
}

ClipPathOpItem::ClipPathOpItem(const CmdListHandle& path, ClipOp clipOp, bool doAntiAlias)
    : DrawOpItem(CLIP_PATH_OPITEM), path_(path), clipOp_(clipOp), doAntiAlias_(doAntiAlias) {}

void ClipPathOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ClipPathOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void ClipPathOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto path = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, path_);
    if (path == nullptr) {
        LOGE("path is nullptr!");
        return;
    }

    canvas.ClipPath(*path, clipOp_, doAntiAlias_);
}

ClipRegionOpItem::ClipRegionOpItem(const CmdListHandle& region, ClipOp clipOp)
    : DrawOpItem(CLIP_REGION_OPITEM), region_(region), clipOp_(clipOp) {}

void ClipRegionOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ClipRegionOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void ClipRegionOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto region = CmdListHelper::GetFromCmdList<RegionCmdList, Region>(cmdList, region_);
    if (region == nullptr) {
        LOGE("region is nullptr!");
        return;
    }

    canvas.ClipRegion(*region, clipOp_);
}

SetMatrixOpItem::SetMatrixOpItem(const Matrix& matrix) : DrawOpItem(SET_MATRIX_OPITEM)
{
    matrix.GetAll(matrixBuffer_);
}

void SetMatrixOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const SetMatrixOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void SetMatrixOpItem::Playback(Canvas& canvas) const
{
    Matrix matrix;
    for (uint32_t i = 0; i < matrixBuffer_.size(); i++) {
        matrix.Set(static_cast<Matrix::Index>(i), matrixBuffer_[i]);
    }

    canvas.SetMatrix(matrix);
}

ResetMatrixOpItem::ResetMatrixOpItem() : DrawOpItem(RESET_MATRIX_OPITEM) {}

void ResetMatrixOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ResetMatrixOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ResetMatrixOpItem::Playback(Canvas& canvas) const
{
    canvas.ResetMatrix();
}

ConcatMatrixOpItem::ConcatMatrixOpItem(const Matrix& matrix) : DrawOpItem(CONCAT_MATRIX_OPITEM)
{
    matrix.GetAll(matrixBuffer_);
}

void ConcatMatrixOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ConcatMatrixOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ConcatMatrixOpItem::Playback(Canvas& canvas) const
{
    Matrix matrix;
    for (uint32_t i = 0; i < matrixBuffer_.size(); i++) {
        matrix.Set(static_cast<Matrix::Index>(i), matrixBuffer_[i]);
    }

    canvas.ConcatMatrix(matrix);
}

TranslateOpItem::TranslateOpItem(scalar dx, scalar dy) : DrawOpItem(TRANSLATE_OPITEM), dx_(dx), dy_(dy) {}

void TranslateOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const TranslateOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void TranslateOpItem::Playback(Canvas& canvas) const
{
    canvas.Translate(dx_, dy_);
}

ScaleOpItem::ScaleOpItem(scalar sx, scalar sy) : DrawOpItem(SCALE_OPITEM), sx_(sx), sy_(sy) {}

void ScaleOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ScaleOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ScaleOpItem::Playback(Canvas& canvas) const
{
    canvas.Scale(sx_, sy_);
}

RotateOpItem::RotateOpItem(scalar deg, scalar sx, scalar sy) : DrawOpItem(ROTATE_OPITEM), deg_(deg), sx_(sx), sy_(sy) {}

void RotateOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const RotateOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void RotateOpItem::Playback(Canvas& canvas) const
{
    canvas.Rotate(deg_, sx_, sy_);
}

ShearOpItem::ShearOpItem(scalar sx, scalar sy) : DrawOpItem(SHEAR_OPITEM), sx_(sx), sy_(sy) {}

void ShearOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ShearOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ShearOpItem::Playback(Canvas& canvas) const
{
    canvas.Shear(sx_, sy_);
}

FlushOpItem::FlushOpItem() : DrawOpItem(FLUSH_OPITEM) {}

void FlushOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const FlushOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void FlushOpItem::Playback(Canvas& canvas) const
{
    canvas.Flush();
}

ClearOpItem::ClearOpItem(ColorQuad color) : DrawOpItem(CLEAR_OPITEM), color_(color) {}

void ClearOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ClearOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ClearOpItem::Playback(Canvas& canvas) const
{
    canvas.Clear(color_);
}

SaveOpItem::SaveOpItem() : DrawOpItem(SAVE_OPITEM) {}

void SaveOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const SaveOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void SaveOpItem::Playback(Canvas& canvas) const
{
    canvas.Save();
}

SaveLayerOpItem::SaveLayerOpItem(const Rect& rect, bool hasBrush, const BrushHandle& brushHandle,
    const CmdListHandle& imageFilter, uint32_t saveLayerFlags) : DrawOpItem(SAVE_LAYER_OPITEM),
    rect_(rect), hasBrush_(hasBrush), brushHandle_(brushHandle), imageFilter_(imageFilter),
    saveLayerFlags_(saveLayerFlags) {}

void SaveLayerOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const SaveLayerOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void SaveLayerOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    const Rect* rectPtr = nullptr;
    if (rect_.IsValid()) {
        rectPtr = &rect_;
    }

    std::shared_ptr<Brush> brush = nullptr;
    if (hasBrush_) {
        auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
            cmdList, brushHandle_.colorSpaceHandle);
        auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
            cmdList, brushHandle_.shaderEffectHandle);
        auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
            cmdList, brushHandle_.colorFilterHandle);
        auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
            cmdList, brushHandle_.imageFilterHandle);
        auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
            cmdList, brushHandle_.maskFilterHandle);

        Filter filter;
        filter.SetColorFilter(colorFilter);
        filter.SetImageFilter(imageFilter);
        filter.SetMaskFilter(maskFilter);
        filter.SetFilterQuality(brushHandle_.filterQuality);

        const Color4f color4f = { brushHandle_.color.GetRedF(), brushHandle_.color.GetGreenF(),
            brushHandle_.color.GetBlueF(), brushHandle_.color.GetAlphaF() };

        brush = std::make_shared<Brush>();
        brush->SetColor(color4f, colorSpace);
        brush->SetShaderEffect(shaderEffect);
        brush->SetBlendMode(brushHandle_.mode);
        brush->SetAntiAlias(brushHandle_.isAntiAlias);
        brush->SetFilter(filter);
    }

    auto saveImageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(cmdList, imageFilter_);

    SaveLayerOps slo(rectPtr, brush.get(), saveImageFilter.get(), saveLayerFlags_);
    canvas.SaveLayer(slo);
}

RestoreOpItem::RestoreOpItem() : DrawOpItem(RESTORE_OPITEM) {}

void RestoreOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const RestoreOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void RestoreOpItem::Playback(Canvas& canvas) const
{
    canvas.Restore();
}

DiscardOpItem::DiscardOpItem() : DrawOpItem(DISCARD_OPITEM) {}

void DiscardOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DiscardOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DiscardOpItem::Playback(Canvas& canvas) const
{
    canvas.Discard();
}

AttachPenOpItem::AttachPenOpItem(const PenHandle& penHandle) : DrawOpItem(ATTACH_PEN_OPITEM), penHandle_(penHandle) {}

void AttachPenOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const AttachPenOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void AttachPenOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto pathEffect = CmdListHelper::GetFromCmdList<PathEffectCmdList, PathEffect>(
        cmdList, penHandle_.pathEffectHandle);
    auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
        cmdList, penHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
        cmdList, penHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
        cmdList, penHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
        cmdList, penHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
        cmdList, penHandle_.maskFilterHandle);

    Filter filter;
    filter.SetColorFilter(colorFilter);
    filter.SetImageFilter(imageFilter);
    filter.SetMaskFilter(maskFilter);
    filter.SetFilterQuality(penHandle_.filterQuality);

    const Color4f color4f = { penHandle_.color.GetRedF(), penHandle_.color.GetGreenF(),
        penHandle_.color.GetBlueF(), penHandle_.color.GetAlphaF() };
    Pen pen;
    pen.SetPathEffect(pathEffect);
    pen.SetColor(color4f, colorSpace);
    pen.SetShaderEffect(shaderEffect);
    pen.SetWidth(penHandle_.width);
    pen.SetMiterLimit(penHandle_.miterLimit);
    pen.SetCapStyle(penHandle_.capStyle);
    pen.SetJoinStyle(penHandle_.joinStyle);
    pen.SetColor(penHandle_.color);
    pen.SetBlendMode(penHandle_.mode);
    pen.SetAntiAlias(penHandle_.isAntiAlias);
    pen.SetFilter(filter);

    canvas.AttachPen(pen);
}

AttachBrushOpItem::AttachBrushOpItem(const BrushHandle& brushHandle)
    : DrawOpItem(ATTACH_BRUSH_OPITEM), brushHandle_(brushHandle) {}

void AttachBrushOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const AttachBrushOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void AttachBrushOpItem::Playback(Canvas& canvas, const CmdList& cmdList) const
{
    auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
        cmdList, brushHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
        cmdList, brushHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
        cmdList, brushHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
        cmdList, brushHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
        cmdList, brushHandle_.maskFilterHandle);

    Filter filter;
    filter.SetColorFilter(colorFilter);
    filter.SetImageFilter(imageFilter);
    filter.SetMaskFilter(maskFilter);
    filter.SetFilterQuality(brushHandle_.filterQuality);

    const Color4f color4f = { brushHandle_.color.GetRedF(), brushHandle_.color.GetGreenF(),
        brushHandle_.color.GetBlueF(), brushHandle_.color.GetAlphaF() };
    Brush brush;
    brush.SetColor(color4f, colorSpace);
    brush.SetShaderEffect(shaderEffect);
    brush.SetBlendMode(brushHandle_.mode);
    brush.SetAntiAlias(brushHandle_.isAntiAlias);
    brush.SetFilter(filter);

    canvas.AttachBrush(brush);
}

DetachPenOpItem::DetachPenOpItem() : DrawOpItem(DETACH_PEN_OPITEM) {}

void DetachPenOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DetachPenOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DetachPenOpItem::Playback(Canvas& canvas) const
{
    canvas.DetachPen();
}

DetachBrushOpItem::DetachBrushOpItem() : DrawOpItem(DETACH_BRUSH_OPITEM) {}

void DetachBrushOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DetachBrushOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DetachBrushOpItem::Playback(Canvas& canvas) const
{
    canvas.DetachBrush();
}

ClipAdaptiveRoundRectOpItem::ClipAdaptiveRoundRectOpItem(const std::pair<uint32_t, size_t>& radiusData)
    : DrawOpItem(CLIP_ADAPTIVE_ROUND_RECT_OPITEM), radiusData_(radiusData) {}

void ClipAdaptiveRoundRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ClipAdaptiveRoundRectOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_, player.rect_);
    }
}

void ClipAdaptiveRoundRectOpItem::Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect) const
{
    auto radius = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, radiusData_);
    auto roundRect = RoundRect(rect, radius);
    canvas.ClipRoundRect(roundRect, ClipOp::INTERSECT, true);
}

DrawAdaptiveImageOpItem::DrawAdaptiveImageOpItem(const ImageHandle& image, const AdaptiveImageInfo& rsImageInfo,
    const SamplingOptions& sampling, const bool isImage) : DrawOpItem(ADAPTIVE_IMAGE_OPITEM),
    image_(image), rsImageInfo_(rsImageInfo), sampling_(sampling), isImage_(isImage) {}

void DrawAdaptiveImageOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawAdaptiveImageOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_, player.rect_);
    }
}

void DrawAdaptiveImageOpItem::Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect) const
{
    if (isImage_) {
        auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
        if (image == nullptr) {
        LOGE("image is nullptr!");
            return;
        }
        AdaptiveImageHelper::DrawImage(canvas, rect, image, rsImageInfo_, sampling_);
        return;
    }
    auto data = CmdListHelper::GetCompressDataFromCmdList(cmdList, image_);
    if (data == nullptr) {
        LOGE("compress data is nullptr!");
        return;
    }
    AdaptiveImageHelper::DrawImage(canvas, rect, data, rsImageInfo_, sampling_);
}

DrawAdaptivePixelMapOpItem::DrawAdaptivePixelMapOpItem(const ImageHandle& pixelMap, const AdaptiveImageInfo& imageInfo,
    const SamplingOptions& smapling) : DrawOpItem(ADAPTIVE_PIXELMAP_OPITEM), pixelMap_(pixelMap),
    imageInfo_(imageInfo), smapling_(smapling) {}

void DrawAdaptivePixelMapOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const DrawAdaptivePixelMapOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_, player.rect_);
    }
}

void DrawAdaptivePixelMapOpItem::Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect) const
{
    auto pixelMap = CmdListHelper::GetPixelMapFromCmdList(cmdList, pixelMap_);
    if (pixelMap == nullptr) {
        LOGE("pixelMap is nullptr!");
        return;
    }
    AdaptiveImageHelper::DrawPixelMap(canvas, rect, pixelMap, imageInfo_, smapling_);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
