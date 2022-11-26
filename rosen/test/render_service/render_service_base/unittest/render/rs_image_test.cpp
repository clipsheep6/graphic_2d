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

#include "gtest/gtest.h"
#include "include/render/rs_image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImageTest::SetUpTestCase() {}
void RSImageTest::TearDownTestCase() {}
void RSImageTest::SetUp() {}
void RSImageTest::TearDown() {}

/**
 * @tc.name: TestRSImage001
 * @tc.desc: IsEqual test.
 * @tc.type: FUNC
 */
HWTEST_F(RSImageTest, TestRSImage001, TestSize.Level1)
{
    RSImage image;
    RSImage other;
    image.IsEqual(other);
}

/**
 * @tc.name: TestRSImage002
 * @tc.desc: CanvasDrawImage test.
 * @tc.type: FUNC
 */
HWTEST_F(RSImageTest, TestRSImage002, TestSize.Level1)
{
    RSImage image;
    SkCanvas canvas;
    SkRect rect;
    SkPaint paint;
    bool isBackground = false;
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    isBackground = true;
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
}

/**
 * @tc.name: TestRSImage003
 * @tc.desc: ApplyImageFit test.
 * @tc.type: FUNC
 */
HWTEST_F(RSImageTest, TestRSImage003, TestSize.Level1)
{
    RSImage image;
    SkCanvas canvas;
    SkRect rect;
    SkPaint paint;
    bool isBackground = false;
    image.SetImageFit(0);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(5);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(2);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(3);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(4);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(6);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(1);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
}
} // namespace OHOS::Rosen
