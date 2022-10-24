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

#include "draw/path.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

namespace Drawing {
/*
* describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
* tips: only support basic type
*/
template<class T>
T GetObject()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

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
    path.AddRect(rect, PathDirection::CCW_DIRECTION);
    path.AddOval(oval, PathDirection::CCW_DIRECTION);
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
    uint32_t weight = GetObject<uint32_t>();
    uint32_t px = GetObject<uint32_t>();
    uint32_t py = GetObject<uint32_t>();
    path1.Interpolate(ending, weight, out);
    path1.LineTo(px, py);
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