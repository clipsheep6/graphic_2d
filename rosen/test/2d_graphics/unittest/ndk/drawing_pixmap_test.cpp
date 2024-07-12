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

#include "drawing_error_code.h"
#include "drawing_pixmap.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPixmapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPixmapTest::SetUpTestCase() {}
void NativeDrawingPixmapTest::TearDownTestCase() {}
void NativeDrawingPixmapTest::SetUp() {}
void NativeDrawingPixmapTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPixmapTest_CreateAndDestroy001
 * @tc.desc: test for create and destroy pixmap.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPixmapTest, NativeDrawingPixmapTest_CreateAndDestroy001, TestSize.Level1)
{
    OH_Drawing_Pixmap *pixmap = OH_Drawing_PixmapCreate();
    EXPECT_NE(pixmap, nullptr);
    OH_Drawing_PixmapDestroy(pixmap);
}

/*
 * @tc.name: NativeDrawingPixmapTest_GetWidth002
 * @tc.desc: test for GetWidth.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPixmapTest, NativeDrawingPixmapTest_GetWidth002, TestSize.Level1)
{
    OH_Drawing_Pixmap *pixmap = OH_Drawing_PixmapCreate();
    EXPECT_NE(pixmap, nullptr);
    uint32_t width;
    OH_Drawing_ErrorCode code = OH_Drawing_PixmapGetWidth(nullptr, &width);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    code = OH_Drawing_PixmapGetWidth(pixmap, &width);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_PixmapDestroy(pixmap);
}

/*
 * @tc.name: NativeDrawingPixmapTest_GetHeight002
 * @tc.desc: test for GetHeight.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPixmapTest, NativeDrawingPixmapTest_GetHeight002, TestSize.Level1)
{
    OH_Drawing_Pixmap *pixmap = OH_Drawing_PixmapCreate();
    EXPECT_NE(pixmap, nullptr);
    uint32_t height;
    OH_Drawing_ErrorCode code = OH_Drawing_PixmapGetHeight(nullptr, &height);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    code = OH_Drawing_PixmapGetHeight(pixmap, &height);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_PixmapDestroy(pixmap);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS