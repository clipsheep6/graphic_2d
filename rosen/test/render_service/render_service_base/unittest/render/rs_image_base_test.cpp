/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "include/core/SkSurface.h"
#include "render/rs_image_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImageBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImageBaseTest::SetUpTestCase() {}
void RSImageBaseTest::TearDownTestCase() {}
void RSImageBaseTest::SetUp() {}
void RSImageBaseTest::TearDown() {}

/**
 * @tc.name: DrawImage001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, DrawImage001, TestSize.Level1)
{
    RSImageBase image;
    SkCanvas canvas;
    SkPaint paint;
    image.DrawImage(canvas, paint);
}

/**
 * @tc.name: SetSrcRect001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, SetSrcRect001, TestSize.Level1)
{
    RSImageBase image;
    RectF srcRect(0, 0, 20, 20);
    image.SetSrcRect(srcRect);
}

/**
 * @tc.name: SetDstRect001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImageBaseTest, SetDstRect001, TestSize.Level1)
{
    RSImageBase image;
    RectF dstRect(0, 0, 20, 20);
    image.SetSrcRect(dstRect);
}
}   // namespace OHOS::Rosen
