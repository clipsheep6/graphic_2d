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

#include "patheffect_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "effect/path_effect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
bool PathEffectFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar radius = GetObject<scalar>();
    PathEffect::CreateCornerPathEffect(radius);
    Path path = GetObject<Path>();
    scalar advance = GetObject<scalar>();
    scalar phase = GetObject<scalar>();
    uint32_t style = GetObject<uint32_t>();
    PathEffect::CreatePathDashEffect(path, advance, phase, static_cast<PathDashStyle>(style));
    PathEffect::CreateDiscretePathEffect(GetObject<scalar>(), GetObject<scalar>(), GetObject<uint32_t>());
    CreateSumPathEffect(GetObject<PathEffect>(), GetObject<PathEffect>());
    CreateComposePathEffect(GetObject<PathEffect>(), GetObject<PathEffect>());
    PathEffectType t = GetObject<PathEffectType>();
    PathEffect(t, path, advance, phase, style);
    PathEffect(t, radius);
    PathEffect(t, GetObject<scalar>(), GetObject<scalar>(), GetObject<uint32_t>());
    PathEffect(t, GetObject<PathEffect>(), GetObject<PathEffect>());
    PathEffect(t);

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PathEffectFuzzTest(data, size);
    return 0;
}
