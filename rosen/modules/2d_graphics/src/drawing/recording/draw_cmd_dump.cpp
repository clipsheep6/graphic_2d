/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <cstdint>
#include <sstream>

#include "platform/common/rs_system_properties.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd_list.h"
#include "recording/mem_allocator.h"
#include "recording/op_item.h"

#include "draw/brush.h"
#include "draw/path.h"
#include "draw/surface.h"
#include "effect/color_filter.h"
#include "effect/color_space.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"
#include "utils/log.h"
#include "utils/scalar.h"
#include "utils/system_properties.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
void DumpField(std::string& out, const Rect& rect)
{
    out += "[x:";
    out += std::to_string(rect.GetLeft()) + " y:";
    out += std::to_string(rect.GetTop()) + " width:";
    out += std::to_string(rect.GetWidth()) + " height:";
    out += std::to_string(rect.GetHeight());
    out += "]";
}

void DumpField(std::string& out, const RectI& rect)
{
    out += "[x:";
    out += std::to_string(rect.GetLeft()) + " y:";
    out += std::to_string(rect.GetTop()) + " width:";
    out += std::to_string(rect.GetWidth()) + " height:";
    out += std::to_string(rect.GetHeight());
    out += "]";
}

void DumpField(std::string& out, const Point& point)
{
    out += "[";
    out += std::to_string(point.GetX()) + " ";
    out += std::to_string(point.GetY());
    out += "]";
}

void DumpField(std::string& out, const Point3& point)
{
    out += "[";
    out += std::to_string(point.GetX()) + " ";
    out += std::to_string(point.GetY()) + " ";
    out += std::to_string(point.GetZ());
    out += "]";
}

void DumpField(std::string& out, const Color& color)
{
    std::ostringstream ss;
    ss << "0x" << std::hex << color.CastToColorQuad();
    out += "[RGBA-";
    out += ss.str();
    out += "]";
}

void DumpField(std::string& out, const Brush& brush)
{
    out += "[alpha:";
    out += std::to_string(brush.GetAlpha());
    out += " color";
    DumpField(out, brush.GetColor());
    out += " blendMode:" + std::to_string(static_cast<int>(brush.GetBlendMode()));
    out += " antiAlias:" + std::string(brush.IsAntiAlias() ? "true" : "false");
    out += "]";
}

void DumpField(std::string& out, const std::shared_ptr<Path>& path)
{
    if (!path) {
        out += "[null]";
        return;
    }
    const auto bounds = path->GetBounds();
    out += "[length:";
    out += std::to_string(path->GetLength(false)) + " bounds";
    DumpField(out, path->GetBounds());
    out += " valid:" + std::string(path->IsValid() ? "true" : "false");
    out += "]";
}

void DumpField(std::string& out, const std::shared_ptr<Image>& image)
{
    if (!image) {
        out += "[null]";
        return;
    }
    out += "[width:";
    out += std::to_string(image->GetWidth()) + " height:";
    out += std::to_string(image->GetHeight());
    out += "]";
}

template<class A, class F>
void DumpArray(std::string& out, const A& array, F func)
{
    out += "[";
    auto end = std::end(array);
    for (auto begin = std::begin(array); begin != end; begin++) {
        func(out, *begin);
        out += " ";
    }
    if (std::begin(array) != end) {
        out.pop_back();
    }
    out += "]";
}

void DumpField(std::string& out, const RoundRect& rrect)
{
    out += "[simple:";
    out += std::string(rrect.IsSimpleRoundRect() ? "true" : "false");
    out += " rect";
    DumpField(out, rrect.GetRect());
    out += " radius";
    using Pos = RoundRect::CornerPos;
    std::array<Point, Pos::CORNER_NUMBER> radius = {
        rrect.GetCornerRadius(Pos::TOP_LEFT_POS),
        rrect.GetCornerRadius(Pos::TOP_RIGHT_POS),
        rrect.GetCornerRadius(Pos::BOTTOM_RIGHT_POS),
        rrect.GetCornerRadius(Pos::BOTTOM_LEFT_POS)
    };
    DumpArray(out, radius, [](std::string& out, const Point& p) {
        DumpField(out, p);
    });
    out += "]";
}

void DumpField(std::string& out, const std::shared_ptr<Region>& region)
{
    if (!region) {
        out += "[null]";
        return;
    }
    out += "[empty:";
    out += std::string(region->IsEmpty() ? "true" : "false") + " isRect:";
    out += std::string(region->IsRect() ? "true" : "false") + " boundaryPath";
    Path path;
    region->GetBoundaryPath(&path);
    DumpField(out, std::make_shared<Path>(path));
    out += "]";
}
}

void DrawShadowStyleOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[plane";
    DumpField(out, planeParams_);
    out += " lightPos";
    DumpField(out, devLightPos_);
    out += " lightRadius:" + std::to_string(lightRadius_);
    out += " ambientColor";
    DumpField(out, ambientColor_);
    out += " spotColor_";
    DumpField(out, spotColor_);
    out += " shadowFlags:" + std::to_string(static_cast<int>(flag_));
    out += " isLimitElevation:" + std::string(isLimitElevation_ ? "true" : "false");
    out += "]";
}

void DrawShadowOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[plane";
    DumpField(out, planeParams_);
    out += " lightPos";
    DumpField(out, devLightPos_);
    out += " lightRadius:" + std::to_string(lightRadius_);
    out += " ambientColor";
    DumpField(out, ambientColor_);
    out += " spotColor";
    DumpField(out, spotColor_);
    out += " shadowFlags:" + std::to_string(static_cast<int>(flag_));
    out += " path";
    DumpField(out, path_);
    out += "]";
}

void DrawColorOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[color";
    DumpField(out, Color(color_));
    out += " blendMode:" + std::to_string(static_cast<int>(mode_));
    out += "]";
}

void DrawImageNineOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[center";
    DumpField(out, center_);
    out += " dst";
    DumpField(out, dst_);
    out += " filterMode:" + std::to_string(static_cast<int>(filter_));
    if (hasBrush_) {
        out += " brush";
        DumpField(out, brush_);
    }
    out += " image";
    DumpField(out, image_);
    out += "]";
}

void DrawImageLatticeOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[lattice[";
    out += "xCount:" + std::to_string(lattice_.fXCount);
    out += " yCount:" + std::to_string(lattice_.fYCount);
    out += " xDivs";
    DumpArray(out, lattice_.fXDivs, [](std::string& out, int n) {
        out += std::to_string(n);
    });
    out += " yDivs";
    DumpArray(out, lattice_.fYDivs, [](std::string& out, int n) {
        out += std::to_string(n);
    });
    out += " rectTypes";
    DumpArray(out, lattice_.fRectTypes, [](std::string& out, Lattice::RectType n) {
        out += std::to_string(static_cast<int>(n));
    });
    out += " bounds";
    DumpArray(out, lattice_.fBounds, [](std::string& out, const RectI& n) {
        DumpField(out, n);
    });
    out += " colors";
    DumpArray(out, lattice_.fColors, [](std::string& out, const Color& c) {
        std::ostringstream ss;
        ss << "0x" << std::hex << c.CastToColorQuad();
        out += ss.str();
    });
    out += " dst";
    DumpField(out, dst_);
    out += " filterMode:" + std::to_string(static_cast<int>(filter_));
    out += " image";
    DumpField(out, image_);
    out += "]";
}

void ClipRectOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[rect";
    DumpField(out, rect_);
    out += " clipOp:" + std::to_string(static_cast<int>(clipOp_));
    out += " antiAlias:" + std::string(doAntiAlias_ ? "true" : "false");
    out += "]";
}

void ClipIRectOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[rect";
    DumpField(out, rect_);
    out += " clipOp:" + std::to_string(static_cast<int>(clipOp_));
    out += "]";
}

void ClipRoundRectOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[rrect";
    DumpField(out, rrect_);
    out += " clipOp:" + std::to_string(static_cast<int>(clipOp_));
    out += " antiAlias:" + std::string(doAntiAlias_ ? "true" : "false");
    out += "]";
}

void ClipRegionOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[clipOp:";
    out += std::to_string(static_cast<int>(clipOp_)) + " region";
    DumpField(out, region_);
    out += "]";
}

void SetMatrixOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[matrix";
    Matrix::Buffer buffer;
    matrix_.GetAll(buffer);
    DumpArray(out, buffer, [](std::string& out, float v) {
        out += std::to_string(v);
    });
    out += "]";
}

void ConcatMatrixOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[matrix";
    Matrix::Buffer buffer;
    matrix_.GetAll(buffer);
    DumpArray(out, buffer, [](std::string& out, float v) {
        out += std::to_string(v);
    });
    out += "]";
}

void TranslateOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[x:";
    out += std::to_string(dx_) + " y:";
    out += std::to_string(dy_);
    out += "]";
}

void ScaleOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[x:";
    out += std::to_string(sx_) + " y:";
    out += std::to_string(sy_);
    out += "]";
}

void RotateOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[degree:";
    out += std::to_string(deg_) + " x:";
    out += std::to_string(sx_) + " y:";
    out += std::to_string(sy_);
    out += "]";
}

void ShearOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[x:";
    out += std::to_string(sx_) + " y:";
    out += std::to_string(sy_);
    out += "]";
}

void ClearOpItem::Dump(std::string& out)
{
    out += GetOpDesc();
    DumpField(out, Color(color_));
}

void SaveLayerOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[flags:";
    out += std::to_string(saveLayerFlags_) + " rect";
    DumpField(out, rect_);
    if (hasBrush_) {
        out += " brush";
        DumpField(out, brush_);
    }
    out += "]";
}

void ClipAdaptiveRoundRectOpItem::Dump(std::string& out)
{
    out += GetOpDesc() + "[radius";
    DumpArray(out, radiusData_, [](std::string& out, const Point& p) {
        DumpField(out, p);
    });
    out += "]";
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
