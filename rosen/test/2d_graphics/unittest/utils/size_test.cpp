/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "utils/size.h"
#include "utils/scalar.h"
#include "utils/rect.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SizeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SizeTest::SetUpTestCase() {}
void SizeTest::TearDownTestCase() {}
void SizeTest::SetUp() {}
void SizeTest::TearDown() {}

/**
 * @tc.name: SizeFCreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFCreateAndDestory001, TestSize.Level1)
{
    // The best way to create SizeF.
    SizeF size;
}

/**
 * @tc.name: SizeFCreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFCreateAndDestory002, TestSize.Level1)
{
    // The best way to create SizeF.
    SizeF sizef;
    SizeF s=sizef;
}

/**
 * @tc.name: SizeFCreateAndDestory003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFCreateAndDestory003, TestSize.Level1)
{
    // The best way to create SizeF.
    SizeF sizef;
    SizeF s=sizef;
}

/**
 * @tc.name: SizeFIsZero001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFIsZero001, TestSize.Level1)
{
    std::unique_ptr<SizeF> sizePtr = std::make_unique<SizeF>();
    ASSERT_TRUE(sizePtr != nullptr);
    sizePtr->IsZero();
}

/**
 * @tc.name: SizeFIsEmpty001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFIsEmpty001, TestSize.Level1)
{
    std::unique_ptr<SizeF> sizePtr = std::make_unique<SizeF>();
    ASSERT_TRUE(sizePtr != nullptr);
    sizePtr->IsEmpty();
}

/**
 * @tc.name: SizeFGetAndSetWidth001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetWidth001, TestSize.Level1)
{
    std::unique_ptr<SizeF> sizePtr = std::make_unique<SizeF>();
    ASSERT_TRUE(sizePtr != nullptr);
    sizePtr->SetWidth(92.5f);
    float width = sizePtr->Width();
    EXPECT_EQ(width, 92.5f);
}

/**
 * @tc.name: SizeFGetAndSetWidth002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetWidth002, TestSize.Level1)
{
    std::unique_ptr<SizeF> sizePtr = std::make_unique<SizeF>();
    ASSERT_TRUE(sizePtr != nullptr);
    sizePtr->SetWidth(162.5f);
    float width = sizePtr->Width();
    EXPECT_EQ(width, 162.5f);
}

/**
 * @tc.name: SizeFGetAndSetWidth003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetWidth003, TestSize.Level1)
{
    std::unique_ptr<SizeF> sizePtr = std::make_unique<SizeF>();
    ASSERT_TRUE(sizePtr != nullptr);
    sizePtr->SetWidth(102.5f);
    float width = sizePtr->Width();
    EXPECT_EQ(width, 102.5f);
}

/**
 * @tc.name: SizeFGetAndSetHeight001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetHeight001, TestSize.Level1)
{
    std::unique_ptr<SizeF> sizePtr = std::make_unique<SizeF>();
    ASSERT_TRUE(sizePtr != nullptr);
    sizePtr->SetHeight(102.5f);
    float height = sizePtr->Height();
    EXPECT_EQ(height, 102.5f);
}

/**
 * @tc.name: SizeFGetAndSetHeight002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetHeight002, TestSize.Level1)
{
    std::unique_ptr<SizeF> sizePtr = std::make_unique<SizeF>();
    ASSERT_TRUE(sizePtr != nullptr);
    sizePtr->SetHeight(131.5f);
    float height = sizePtr->Height();
    EXPECT_EQ(height, 131.5f);
}

/**
 * @tc.name: SizeFGetAndSetHeight003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SizeTest, SizeFGetAndSetHeight003, TestSize.Level1)
{
    std::unique_ptr<SizeF> sizePtr = std::make_unique<SizeF>();
    ASSERT_TRUE(sizePtr != nullptr);
    sizePtr->SetHeight(50);
    int height = sizePtr->Height();
    EXPECT_EQ(height, 50);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
