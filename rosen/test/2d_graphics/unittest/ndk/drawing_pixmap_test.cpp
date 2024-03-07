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

#include "c/drawing_pixmap.h"
#include "c/drawing_types.h"
#include "image/pixmap.h"

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
protected:
    OH_Drawing_Pixmap* pixmap_ = nullptr;
};

void NativeDrawingPixmapTest::SetUpTestCase() {}
void NativeDrawingPixmapTest::TearDownTestCase() {}
void NativeDrawingPixmapTest::SetUp()
{
    pixmap_ = OH_Drawing_PixmapCreate();
    ASSERT_NE(pixmap_, nullptr);
}

void NativeDrawingPixmapTest::TearDown()
{
    if (pixmap_ != nullptr) {
        OH_Drawing_PixmapDestroy(pixmap_);
        pixmap_ = nullptr;
    }
}

/*
 * @tc.name: NativeDrawingPixmapTest_PixmapGetWidth001
 * @tc.desc: test for OH_Drawing_PixmapGetWidth.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPixmapTest, NativeDrawingPixmapTest_PixmapGetWidth001, TestSize.Level1)
{
    ASSERT_NE(nullptr, pixmap_);
    EXPECT_EQ(0, OH_Drawing_PixmapGetWidth(pixmap_));
}

/*
 * @tc.name: NativeDrawingPixmapTest_PixmapGetHeight001
 * @tc.desc: test for OH_Drawing_PixmapGetHeight.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPixmapTest, NativeDrawingPixmapTest_PixmapGetHeight001, TestSize.Level1)
{
    ASSERT_NE(nullptr, pixmap_);
    EXPECT_EQ(0, OH_Drawing_PixmapGetHeight(pixmap_));
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS