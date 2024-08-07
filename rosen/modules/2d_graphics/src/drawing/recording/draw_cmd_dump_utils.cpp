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

#include "recording/draw_cmd_dump_utils.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
