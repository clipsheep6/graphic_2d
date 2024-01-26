/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "c/drawing_rect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingRectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingRectTest::SetUpTestCase() {}
void NativeDrawingRectTest::TearDownTestCase() {}
void NativeDrawingRectTest::SetUp() {}
void NativeDrawingRectTest::TearDown() {}

/*
 * @tc.name: NativeDrawingRectTest001
 * @tc.desc: test for get height of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest001, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 400, 800);
    float height = OH_Drawing_RectGetHeight(rect);
    EXPECT_EQ(height, 800); // 800 means height
    OH_Drawing_RectDestroy(rect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS