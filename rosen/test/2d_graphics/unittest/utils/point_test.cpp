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
#include "utils/point.h"
#include "utils/scalar.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PointTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PointTest::SetUpTestCase() {}
void PointTest::TearDownTestCase() {}
void PointTest::SetUp() {}
void PointTest::TearDown() {}

/**
 * @tc.name: PointFCreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFCreateAndDestory001, TestSize.Level1)
{
    // The best way to create PointF.
    PointF pointF;
}

/**
 * @tc.name: PointFCreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFCreateAndDestory002, TestSize.Level1)
{
    // The best way to create PointF.
    std::unique_ptr<PointF> pointfPtr_ = std::make_unique<PointF>();
    ASSERT_TRUE(pointfPtr_ != nullptr);
}

/**
 * @tc.name: PointFCreateAndDestory003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFCreateAndDestory003, TestSize.Level1)
{
    // The best way to create PointF.
    PointF p;
    PointF pointF = p;
}

/**
 * @tc.name: PointFCreateAndDestory004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFCreateAndDestory004, TestSize.Level1)
{
    // The best way to create PointF.
    PointF pointF(100.1f, 25.6f);
}

/**
 * @tc.name: PointFCreateAndDestory005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFCreateAndDestory005, TestSize.Level1)
{
    // The best way to create PointF.
    PointF pointF(333.1f, 251.6f);
}

/**
 * @tc.name: PointFSetAndGetX001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFSetAndGetX001, TestSize.Level1)
{
    std::unique_ptr<PointF> pointfPtr_ = std::make_unique<PointF>();
    ASSERT_TRUE(pointfPtr_ != nullptr);
    pointfPtr_->SetX(101.3f);
    OHOS::Rosen::Drawing::scalar x = pointfPtr_->GetX();
    EXPECT_EQ(x, 101.3f);
}

/**
 * @tc.name: PointFSetAndGetX002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFSetAndGetX002, TestSize.Level1)
{
    std::unique_ptr<PointF> pointfPtr_ = std::make_unique<PointF>();
    ASSERT_TRUE(pointfPtr_ != nullptr);
    pointfPtr_->SetX(333.9f);
    OHOS::Rosen::Drawing::scalar x = pointfPtr_->GetX();
    EXPECT_EQ(x, 333.9f);
}

/**
 * @tc.name: PointFSetAndGetY001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFSetAndGetY001, TestSize.Level1)
{
    std::unique_ptr<PointF> pointfPtr_ = std::make_unique<PointF>();
    ASSERT_TRUE(pointfPtr_ != nullptr);
    pointfPtr_->SetY(101.3f);
    OHOS::Rosen::Drawing::scalar y = pointfPtr_->GetX();
    EXPECT_EQ(y, 101.3f);
}

/**
 * @tc.name: PointFSetAndGetY002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFSetAndGetY002, TestSize.Level1)
{
    std::unique_ptr<PointF> pointfPtr_ = std::make_unique<PointF>();
    ASSERT_TRUE(pointfPtr_ != nullptr);
    pointfPtr_->SetY(101.3f);
    OHOS::Rosen::Drawing::scalar y = pointfPtr_->GetY();
    EXPECT_EQ(y, 101.3f);
}

/**
 * @tc.name: PointFoperatorAddEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorAddEqual001, TestSize.Level1)
{
    PointF p;
    PointF pointF;
    pointF += p;
}

/**
 * @tc.name: PointFoperatorMinusEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorMinusEqual001, TestSize.Level1)
{
    PointF p;
    PointF pointF;
    pointF -= p;
}

/**
 * @tc.name: PointFoperatorMultiplyEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorMultiplyEqual001, TestSize.Level1)
{
    PointF pointF;
    pointF *= 103.3f;
}

/**
 * @tc.name: PointFoperatorMultiplyEqual002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorMultiplyEqual002, TestSize.Level1)
{
    PointF pointF;
    pointF *= 133.3f;
}

/**
 * @tc.name: PointFoperatorAdd1001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorAdd1001, TestSize.Level1)
{
    PointF pointF1;
    PointF pointF2;
    PointF pointF = pointF1 + pointF2;
}

/**
 * @tc.name: PointFoperatorAdd2001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorAdd2001, TestSize.Level1)
{
    PointF p1;
    PointF p2;
    PointF P3 = p1 + p2;
}

/**
 * @tc.name: PointFoperatorAddl001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorMinus001, TestSize.Level1)
{
    PointF pointF1;
    PointF pointF2;
    PointF pointF = pointF1 - pointF2;
}

/**
 * @tc.name: PointFoperatorMultiply1001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorMultiply001, TestSize.Level1)
{
    PointF pointF1;
    PointF pointF = 100.9f * pointF1 ;
}

/**
 * @tc.name: PointFoperatorMultiply1002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorMultiply002, TestSize.Level1)
{
    PointF pointF1;
    PointF pointF= 22.4f * pointF1 ;
}

/**
 * @tc.name: PointFoperatorMultiply2001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorMultiply2001, TestSize.Level1)
{
    PointF pointF1;
    PointF pointF = pointF1 * 99.3f;
}

/**
 * @tc.name: PointFoperatorMultiply2002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorMultiply2002, TestSize.Level1)
{
    PointF pointF1;
    PointF pointF = pointF1 * 22.4f;
}

/**
 * @tc.name: PointFoperatorDivide2001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorDivide2001, TestSize.Level1)
{
    PointF pointF1;
    PointF pointF = pointF1 / 22.4f;
}

/**
 * @tc.name: PointFoperatorDivide2002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorDivide2002, TestSize.Level1)
{
    PointF pointF1;
    PointF pointF = pointF1 / 500.8f;
}

/**
 * @tc.name: PointFoperatorEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorEEqual001, TestSize.Level1)
{
    PointF pointF1;
    PointF pointF2;
    ASSERT_TRUE(pointF1 != pointF2);
}

/**
 * @tc.name: PointFoperatorNotEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointFoperatorNotEEqual001, TestSize.Level1)
{
    PointF pointF1;
    PointF pointF2;
    ASSERT_TRUE(pointF1 != pointF2);
}

/**
 * @tc.name: PointICreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointICreateAndDestory001, TestSize.Level1)
{
    // The best way to create PointF.
    PointI pointI;
}

/**
 * @tc.name: PointICreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointICreateAndDestory002, TestSize.Level1)
{
    // The best way to create PointF.
    std::unique_ptr<PointI> pointIPtr_ = std::make_unique<PointI>();
    ASSERT_TRUE(pointIPtr_ != nullptr);
}

/**
 * @tc.name: PointFCreateAndDestory003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointICreateAndDestory003, TestSize.Level1)
{
    // The best way to create PointF.
    PointI p;
    PointI PointI = p;
}

/**
 * @tc.name: PointFCreateAndDestory004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointICreateAndDestory004, TestSize.Level1)
{
    // The best way to create PointF.
    PointI PointI(100, 55);
}

/**
 * @tc.name: PointFCreateAndDestory005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointICreateAndDestory005, TestSize.Level1)
{
    // The best way to create PointF.
    PointI PointI(333, 251);
}

/**
 * @tc.name: PointISetGetX001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointISetGetX001, TestSize.Level1)
{
    std::unique_ptr<PointI> pointIPtr_ = std::make_unique<PointI>();
    ASSERT_TRUE(pointIPtr_ != nullptr);
    pointIPtr_->SetY(101);
    OHOS::Rosen::Drawing::scalar x = pointIPtr_->GetY();
    EXPECT_EQ(x, 101);
}

/**
 * @tc.name: PointISetGetX002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointISetGetX002, TestSize.Level1)
{
    std::unique_ptr<PointI> pointIPtr_ = std::make_unique<PointI>();
    ASSERT_TRUE(pointIPtr_ != nullptr);
    pointIPtr_->SetY(333);
    OHOS::Rosen::Drawing::scalar x = pointIPtr_->GetY();
    EXPECT_EQ(x, 333);
}

/**
 * @tc.name: PointISetGetY001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointISetGetY001, TestSize.Level1)
{
    std::unique_ptr<PointI> pointIPtr_ = std::make_unique<PointI>();
    ASSERT_TRUE(pointIPtr_ != nullptr);
    pointIPtr_->SetY(101);
    OHOS::Rosen::Drawing::scalar y = pointIPtr_->GetY();
    EXPECT_EQ(y, 101);
}

/**
 * @tc.name: PointISetGetY002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointISetGetY002, TestSize.Level1)
{
    std::unique_ptr<PointI> pointIPtr_ = std::make_unique<PointI>();
    ASSERT_TRUE(pointIPtr_ != nullptr);
    pointIPtr_->SetY(333);
    OHOS::Rosen::Drawing::scalar y = pointIPtr_->GetY();
    EXPECT_EQ(y, 333);
}

/**
 * @tc.name: PointIoperatorAddEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorAddEqual001, TestSize.Level1)
{
    PointI p;
    PointI pointI;
    pointI += p;
}

/**
 * @tc.name: PointIoperatorMinusEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorMinusEqual001, TestSize.Level1)
{
    PointI p;
    PointI pointI;
    pointI -= p;
}

/**
 * @tc.name: PointIoperatorMultiplyEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorMultiplyEqual001, TestSize.Level1)
{
    PointI pointI;
    pointI *= 103.3f;
}

/**
 * @tc.name: PointIoperatorMultiplyEqual002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorMultiplyEqual002, TestSize.Level1)
{
    PointI pointI;
    pointI *= 133.3f;
}

/**
 * @tc.name: PointIoperatordivideEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatordivideEqual001, TestSize.Level1)
{
    PointI pointI;
    pointI /= 99.5f;
}

/**
 * @tc.name: PointIoperatordivideEqual002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatordivideEqual002, TestSize.Level1)
{
    PointI pointI;
    pointI /= 10.5f;
}

/**
 * @tc.name: PointIoperatorAdd1001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorAdd1001, TestSize.Level1)
{
    PointI pointI1;
    PointI pointI2;
    PointI pointI = pointI1 + pointI2;
}

/**
 * @tc.name: PointIoperatorAddl001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorMinus001, TestSize.Level1)
{
    PointI pointI1;
    PointI pointI2;
    PointI pointI = pointI1 - pointI2;
}

/**
 * @tc.name: PointIoperatorMultiply1001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorMultiply001, TestSize.Level1)
{
    PointI pointI1;
    PointI pointI = 100.9f * pointI1 ;
}

/**
 * @tc.name: PointIoperatorMultiply1002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorMultiply002, TestSize.Level1)
{
    PointI pointI1;
    PointI pointI = 22.4f * pointI1 ;
}

/**
 * @tc.name: PointIoperatorMultiply2001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorMultiply2001, TestSize.Level1)
{
    PointI pointI1;
    PointI pointI = pointI1 * 99.3f;
}

/**
 * @tc.name: PointIoperatorMultiply2002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorMultiply2002, TestSize.Level1)
{
    PointI pointI1;
    PointI pointI = pointI1 * 22.4f;
}

/**
 * @tc.name: PointIoperatorDivide2001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorDivide2001, TestSize.Level1)
{
    PointI pointI1;
    PointI pointI = pointI1 / 22.4f;
}

/**
 * @tc.name: PointIoperatorDivide2002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorDivide2002, TestSize.Level1)
{
    PointI pointI1;
    PointI pointI = pointI1 / 500.8f;
}

/**
 * @tc.name: PointIoperatorEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorEEqual001, TestSize.Level1)
{
    PointI pointI1;
    PointI pointI2;
    ASSERT_TRUE(pointI1 != pointI2);
}

/**
 * @tc.name: PointIoperatorNotEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(PointTest, PointIoperatorNotEEqual001, TestSize.Level1)
{
    PointI pointI1;
    PointI pointI2;
    ASSERT_TRUE(pointI1 != pointI2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

