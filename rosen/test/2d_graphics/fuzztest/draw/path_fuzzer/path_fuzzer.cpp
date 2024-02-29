/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "path_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "draw/path.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
bool PathAddFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    Rect rect = GetObject<Rect>();
    Rect oval = GetObject<Rect>();
    path.AddRect(rect, GetObject<PathDirection>());
    path.AddOval(oval, GetObject<PathDirection>());
    path.Close();
    return true;
}

bool PathOpFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path1, path2, ending, out, op;
    scalar weight = GetObject<scalar>();
    scalar px = GetObject<scalar>();
    scalar py = GetObject<scalar>();
    path1.GetDrawingType();
    path1.BuildFromSVGString(GetObject<std::string>());
    path1.ConvertToSVGString();
    path1.Interpolate(ending, weight, out);
    path1.LineTo(px, py);
    path1.ArcTo(GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(),GetObject<scalar>());
    path1.ArcTo(GetObject<Point>(), GetObject<Point>(), GetObject<scalar>(), GetObject<scalar>());
    path1.ArcTo(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<PathDirection>(), GetObject<scalar>(), GetObject<scalar>());
    path1.ArcTo(GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    path1.CubicTo(px, py, GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    path1.CubicTo(GetObject<Point>(), GetObject<Point>(), GetObject<Point>());
    path1.QuadTo(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    path1.QuadTo(GetObject<Point>(), GetObject<Point>());

    path2.RMoveTo(px, py);
    path2.RLineTo(px, py);
    path2.RArcTo(px, py, GetObject<scalar>(), GetObject<PathDirection>(), GetObject<scalar>(), GetObject<scalar>());
    path2.RCubicTo(px, py, GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    path2.RQuadTo(px, py, GetObject<scalar>(), GetObject<scalar>());
    op.MoveTo(px, py);
    path2.Op(op, path1, PathOp::DIFFERENCE);
    path1.Close();
    path2.Reset();
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PathAddFuzzTest(data, size);
    OHOS::Rosen::Drawing::PathOpFuzzTest(data, size);
    return 0;
}