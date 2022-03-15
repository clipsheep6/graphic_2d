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
#include "utils/rect.h"
#include "utils/scalar.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RectTest::SetUpTestCase() {}
void RectTest::TearDownTestCase() {}
void RectTest::SetUp() {}
void RectTest::TearDown() {}

/**
 * @tc.name: RectFCreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestory001, TestSize.Level1)
{
    // The best way to create RectF.
    RectF rectf;
}

/**
 * @tc.name: RectFCreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestory002, TestSize.Level1)
{
    // The best way to create RectF.
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
}

/**
 * @tc.name: RectFCreateAndDestory003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestory003, TestSize.Level1)
{
    // The best way to create RectF.
    RectF rectf1;
    RectF rectf2 = rectf1;
}

/**
 * @tc.name: RectFCreateAndDestory004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestory004, TestSize.Level1)
{
    // The best way to create RectF.
    RectF rectf(11.1f, 22.6f, 235.99f, 45.1f);
}

/**
 * @tc.name: RectFCreateAndDestory005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestory005, TestSize.Level1)
{
    // The best way to create RectF.
    RectF rectf(11, 88, 60, 100);
}

/**
 * @tc.name: RectFCreateAndDestory006
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFCreateAndDestory006, TestSize.Level1)
{
    // The best way to create RectF.
    RectF rectf(11.1f, 22.6f, 50, 45.1f);
}

/**
 * @tc.name: RectFisValid001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFisValid001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->IsValid();
}

/**
 * @tc.name: RectFisValid002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFisValid002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->IsValid();
}

/**
 * @tc.name: RectFGetAndSetLeft001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetLeft001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetLeft(101.3f);
    auto left = rectfPtr->GetLeft();
    EXPECT_EQ(left, 101.3f);
}

/**
 * @tc.name: RectFGetAndSetLeft002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetLeft002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetLeft(12.3f);
    auto left = rectfPtr->GetLeft();
    EXPECT_EQ(left, 12.3f);
}

/**
 * @tc.name: RectFGetAndSetLeft003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetLeft003, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetLeft(100);
    auto left = rectfPtr->GetLeft();
    EXPECT_EQ(left, 100);
}

/**
 * @tc.name: RectFGetAndSetTop001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetTop001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetLeft(101.3f);
    auto left = rectfPtr->GetLeft();
    EXPECT_EQ(left, 101.3f);
}

/**
 * @tc.name: RectFGetAndSetTop002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetTop002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetLeft(12.3f);
    auto left = rectfPtr->GetLeft();
    EXPECT_EQ(left, 12.3f);
}


/**
 * @tc.name: RectFGetAndSetTop003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetTop003, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetTop(100);
    auto top = rectfPtr->GetTop();
    EXPECT_EQ(top, 100);
}

/**
 * @tc.name: RectFGetAndSetRight001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetRight001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetTop(101.3f);
    auto top = rectfPtr->GetTop();
    EXPECT_EQ(top, 101.3f);
}

/**
 * @tc.name: RectFGetAndSetTop002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetRight002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetRight(12.3f);
    auto right = rectfPtr->GetRight();
    EXPECT_EQ(right, 12.3f);
}


/**
 * @tc.name: RectFGetAndSetRight003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetRight003, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetRight(100);
    auto right = rectfPtr->GetRight();
    EXPECT_EQ(right, 100);
}

/**
 * @tc.name: RectFGetAndSetBottom001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetBottom001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetBottom(101.3f);
    auto bottom = rectfPtr->GetBottom();
    EXPECT_EQ(bottom, 101.3f);
}

/**
 * @tc.name: RectFGetAndSetBottom002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetBottom002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetBottom(12.3f);
    auto bottom = rectfPtr->GetBottom();
    EXPECT_EQ(bottom, 12.3f);
}


/**
 * @tc.name: RectFGetAndSetBottom003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetAndSetBottom003, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetBottom(100);
    auto bottom = rectfPtr->GetBottom();
    EXPECT_EQ(bottom, 100);
}

/**
 * @tc.name: RectFGetWidth001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetWidth001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->GetWidth();
}

/**
 * @tc.name: RectFGetHeight001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFGetHeight001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->GetHeight();
}

/**
 * @tc.name: RectFOffset001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFOffset001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->Offset(121.3f, 36.5f);
}

/**
 * @tc.name: RectFOffset002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFOffset002, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->Offset(420.9f, 3.5f);
}

/**
 * @tc.name: RectFOffset003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFOffset003, TestSize.Level1)
{
    std::unique_ptr<RectF> rectfPtr = std::make_unique<RectF>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->Offset(100, 100);
}

/**
 * @tc.name: RectFOperatorEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectFOperatorEEqual001, TestSize.Level1)
{
    RectF r1;
    RectF r2;
    ASSERT_TRUE(r1 != r2);
}

/**
 * @tc.name: RectICreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestory001, TestSize.Level1)
{
    // The best way to create RectI.
    RectI rect;
}

/**
 * @tc.name: RectICreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestory002, TestSize.Level1)
{
    // The best way to create RectI.
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
}

/**
 * @tc.name: RectICreateAndDestory003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestory003, TestSize.Level1)
{
    // The best way to create RectI.
    RectI rect1;
    RectI rect2 = rect1;
}

/**
 * @tc.name: RectICreateAndDestory004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestory004, TestSize.Level1)
{
    // The best way to create RectI.
    RectI rect(11, 22, 235, 45);
}

/**
 * @tc.name: RectICreateAndDestory005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestory005, TestSize.Level1)
{
    // The best way to create RectI.
    RectI rectf(11, 88, 60, 100);
}

/**
 * @tc.name: RectICreateAndDestory006
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectICreateAndDestory006, TestSize.Level1)
{
    // The best way to create RectI.
    RectI rectf(11, 12, 50, 45);
}

/**
 * @tc.name: RectIisValid001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIisValid001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->IsValid();
}

/**
 * @tc.name: RectIisValid002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIisValid002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->IsValid();
}

/**
 * @tc.name: RectIGetAndSetLeft001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetLeft001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetLeft(101);
    scalar left = rectfPtr->GetLeft();
    EXPECT_EQ(left, 101.3f);
}

/**
 * @tc.name: RectIGetAndSetLeft002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetLeft002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetLeft(12);
    scalar left = rectfPtr->GetLeft();
    EXPECT_EQ(left, 12);
}


/**
 * @tc.name: RectIGetAndSetLeft003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetLeft003, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetLeft(100);
    scalar left = rectfPtr->GetLeft();
    EXPECT_EQ(left, 100);
}

/**
 * @tc.name: RectIGetAndSetTop001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetTop001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetTop(101);
    scalar top = rectfPtr->GetTop();
    EXPECT_EQ(top, 101);
}

/**
 * @tc.name: RectIGetAndSetTop002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetTop002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetTop(12);
    scalar top = rectfPtr->GetTop();
    EXPECT_EQ(top, 12);
}


/**
 * @tc.name: RectIGetAndSetTop003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetTop003, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetTop(100);
    scalar top = rectfPtr->GetTop();
    EXPECT_EQ(top, 100);
}

/**
 * @tc.name: RectIGetAndSetRight001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetRight001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetRight(101);
    scalar right = rectfPtr->GetRight();
    EXPECT_EQ(right, 101);
}

/**
 * @tc.name: RectIGetAndSetTop002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetRight002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetRight(12);
    scalar right = rectfPtr->GetRight();
    EXPECT_EQ(right, 12);
}


/**
 * @tc.name: RectIGetAndSetRight003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetRight003, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetRight(100);
    scalar right = rectfPtr->GetRight();
    EXPECT_EQ(right, 100);
}

/**
 * @tc.name: RectIGetAndSetBottom001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetBottom001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetBottom(101);
    scalar bottom = rectfPtr->GetBottom();
    EXPECT_EQ(bottom, 101);
}

/**
 * @tc.name: RectIGetAndSetBottom002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetBottom002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetBottom(12);
    scalar bottom = rectfPtr->GetBottom();
    EXPECT_EQ(bottom, 12);
}


/**
 * @tc.name: RectIGetAndSetBottom003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetAndSetBottom003, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->SetBottom(100);
    scalar bottom = rectfPtr->GetBottom();
    EXPECT_EQ(bottom, 100);
}

/**
 * @tc.name: RectIGetWidth001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetWidth001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->GetWidth();
}

/**
 * @tc.name: RectIGetHeight001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIGetHeight001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->GetHeight();
}

/**
 * @tc.name: RectIOffset001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIOffset001, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->Offset(121, 36);
}

/**
 * @tc.name: RectIOffset002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIOffset002, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->Offset(420, 3);
}

/**
 * @tc.name: RectIOffset003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIOffset003, TestSize.Level1)
{
    std::unique_ptr<RectI> rectfPtr = std::make_unique<RectI>();
    ASSERT_TRUE(rectfPtr != nullptr);
    rectfPtr->Offset(100, 100);
}

/**
 * @tc.name: RectIOperatorEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIOperatorEEqual001, TestSize.Level1)
{
    RectI r1;
    RectI r2;
    ASSERT_TRUE(r1 != r2);
}

/**
 * @tc.name: RectIOperatorNotEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RectTest, RectIOperatorNotEEqual001, TestSize.Level1)
{
    RectI r1;
    RectI r2;
    ASSERT_TRUE(r1 != r2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS