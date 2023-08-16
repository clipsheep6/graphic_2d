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
#include "rosen_text/typography_create.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
class OH_Drawing_TypographyCreateTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_TypographyCreateTest001
 * @tc.desc: test for append placeholder
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyCreateTest, OH_Drawing_TypographyCreateTest001, TestSize.Level1)
{
    PlaceholderSpan holder = {
        // means placeholder width is 10.0
        .width = 10.0,
        // means placeholder height is 5.0
        .height = 5.0,
        .alignment = PlaceholderVerticalAlignment::OFFSET_AT_BASELINE,
        .baseline = TextBaseline::ALPHABETIC,
        .baselineOffset = 0.0,
    };
    TypographyStyle style;
    std::shared_ptr<FontCollection> collection = FontCollection::Create();
    EXPECT_NE(collection, nullptr);
    std::unique_ptr<TypographyCreate> typography = TypographyCreate::Create(style, collection);
    EXPECT_NE(typography, nullptr);

    typography->AppendPlaceholder(holder);
}
} // namespace OHOS