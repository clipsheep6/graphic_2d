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

#include "paint_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "draw/paint.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

void PaintFuzzTestInner01(Paint& paint)
{
    paint.GetStyle();
    paint.GetColor();
    paint.GetColor4f();
    paint.GetAlpha();
    paint.GetAlphaF();
    paint.GetWidth();
    paint.GetMiterLimit();
    paint.GetBlendMode();

    paint.GetCapStyle();
    paint.GetJoinStyle();
    paint.GetShaderEffect();
    paint.GetFilter();
    paint.HasFilter();
    paint.IsAntiAlias();
    paint.Reset();
    paint.Disable();
}

bool PaintFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Paint paint;
    Color color;

    Color color = GetObject<Color>();
    PaintStyle style = GetObject<PaintStyle>();
    Color4f cf = GetObject<Color4f>();
    int red = GetObject<int>();
    int gree = GetObject<int>();
    int blue = GetObject<int>();
    int alpha = GetObject<int>();
    paint.SetColor(color);
    paint.SetColor(cf);
    paint.SetStyle(style);
    paint.SetAlpha(GetObject<uint32_t>());
    paint.SetAlphaF(GetObject<scalar>());
    paint.SetARGB(red, gree, blue, alpha);
    paint.SetWidth(GetObject<scalar>());
    paint.SetMiterLimit(GetObject<scalar>());
    paint.SetCapStyle(GetObject<Pen::CapStyle>());
    paint.SetJoinStyle(GetObject<Pen::JoinStyle>());
    paint.SetBlendMode(GetObject<BlendMode>());
    paint.SetFilter(GetObject<Filter>());
    paint.SetAntiAlias(GetObject<bool>());

    PaintFuzzTestInner01(paint);

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PaintFuzzTest(data, size);
    return 0;
}