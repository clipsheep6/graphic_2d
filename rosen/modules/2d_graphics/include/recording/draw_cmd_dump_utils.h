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

#ifndef DRAW_CMD_DUMP_UTILS
#define DRAW_CMD_DUMP_UTILS

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
#include "sandbox_utils.h"
#include "utils/log.h"
#include "utils/scalar.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

void DumpField(std::string& out, const Rect& rect);
void DumpField(std::string& out, const RectI& rect);
void DumpField(std::string& out, const Point& point);
void DumpField(std::string& out, const Point3& point);
void DumpField(std::string& out, const Color& color);
void DumpField(std::string& out, const Brush& brush);
void DumpField(std::string& out, const std::shared_ptr<Path>& path);
void DumpField(std::string& out, const std::shared_ptr<Image>& image);
void DumpField(std::string& out, const RoundRect& rrect);
void DumpField(std::string& out, const std::shared_ptr<Region>& region);

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

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif //DRAW_CMD_DUMP_UTILS
