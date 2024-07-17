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
#include "draw_bitmap_test.h"
#include "test_common.h"
namespace OHOS {
namespace Rosen {
void DrawBitmapTest::OnTestFunctionCpu(Drawing::Canvas* canvas)
{

    Drawing::Bitmap bit;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bit.Build(100, 100, format);
    std::shared_ptr<Drawing::Canvas> bitCanvas = std::make_shared<Drawing::Canvas>();
    if (bitCanvas == nullptr) {
        TestCommon::Log("failed to create bitmapCanvas");
    }
    bitCanvas->Bind(bit);
    bitCanvas->DrawColor(0xFFFFFFFF);

    canvas->DrawColor(0xFFFF0000);

    canvas->DrawBitmap(bit,100,100);

}
void DrawBitmapTest::OnTestFunctionGpuUpScreen(Drawing::Canvas* canvas)
{
    Drawing::Bitmap bit;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bit.Build(100, 100, format);
    std::shared_ptr<Drawing::Canvas> bitCanvas = std::make_shared<Drawing::Canvas>();
    if (bitCanvas == nullptr) {
        TestCommon::Log("failed to create bitmapCanvas");
    }
    bitCanvas->Bind(bit);
    bitCanvas->DrawColor(0xFFFFFFFF);

    canvas->DrawColor(0xFFFF0000);

    canvas->DrawBitmap(bit,100,100);
}
void DrawBitmapTest::OnTestPerformance(Drawing::Canvas* canvas)
{
}
} // namespace Rosen
} // namespace OHOS