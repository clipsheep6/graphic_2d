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

#include <memory>
#include "gtest/gtest.h"

#include "render/rs_aibar_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class AIBarFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AIBarFilterTest::SetUpTestCase() {}
void AIBarFilterTest::TearDownTestCase() {}
void AIBarFilterTest::SetUp() {}
void AIBarFilterTest::TearDown() {}

/**
 * @tc.name: testInterface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(AIBarFilterTest, testInterface, TestSize.Level1)
{
    std::shared_ptr<RSAIBarFilter> aiBarFilter = std::make_shared<RSAIBarFilter>();

#ifndef USE_ROSEN_DRAWING
    SkCanvas canvas;
    SkRect src;
    SkRect dst;
    sk_sp<SkImage> image;
#else
    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    std::shared_ptr<Drawing::Image> image;
#endif

    aiBarFilter->DrawImageRect(canvas, image, src, dst);
}
} // namespace Rosen
} // namespace OHOS
