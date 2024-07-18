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

#include "draw_pictrue_test.h"
#include "image/picture.h"

namespace OHOS {
namespace Rosen {
using namespace Drawing;
void DrawPictrueTest::OnTestFunction(Drawing::Canvas* canvas)
{

std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
std::shared_ptr<Drawing::Picture> pic= std::make_shared<Drawing::Picture>(drawCmdList);
canvas->DrawBackground(0xFFFF0000);
canvas->Save();
canvas->DrawPicture(pic);
canvas->Restore();

}

void DrawPictrueTest::OnTestPerformance(Drawing::Canvas* canvas)
{
   
}
} // namespace Rosen
} // namespace OHOS