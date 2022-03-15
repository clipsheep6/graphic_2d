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
#include "utils/point3.h"
#include "utils/scalar.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Point3Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void Point3Test::SetUpTestCase() {}
void Point3Test::TearDownTestCase() {}
void Point3Test::SetUp() {}
void Point3Test::TearDown() {}

/**
 * @tc.name: CreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, CreateAndDestory001, TestSize.Level1)
{
    // The best way to create Point3.
    Point3 point3_;
}

/**
 * @tc.name: CreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, CreateAndDestory002, TestSize.Level1)
{
    // The best way to create Point3.
    std::unique_ptr<Point3> point3Ptr_ = std::make_unique<Point3>();
    ASSERT_TRUE(point3Ptr_ != nullptr);
}

/**
 * @tc.name: CreateAndDestory003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, CreateAndDestory003, TestSize.Level1)
{
    // The best way to create Point3.
    Point3 point;
    Point3 p=point;
}

/**
 * @tc.name: CreateAndDestory004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, CreateAndDestory004, TestSize.Level1)
{
    // The best way to create Point3.
    std::unique_ptr<Point3> point3Ptr_ = std::make_unique<Point3>(12.5f, 66.9f, 201.8f);
    ASSERT_TRUE(point3Ptr_ != nullptr);
}

/**
 * @tc.name: CreateAndDestory005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, CreateAndDestory005, TestSize.Level1)
{
    // The best way to create Point3.
    std::unique_ptr<Point3> point3Ptr_ = std::make_unique<Point3>(20, 30, 66);
    ASSERT_TRUE(point3Ptr_ != nullptr);
}

/**
 * @tc.name: SetAndGetX001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, SetAndGetX001, TestSize.Level1)
{
    std::unique_ptr<Point3> point3Ptr_ = std::make_unique<Point3>();
    ASSERT_TRUE(point3Ptr_ != nullptr);
    point3Ptr_->SetX(101.3f);
    scalar x = point3Ptr_->GetX();
    EXPECT_EQ(x, 101.3f);
}

/**
 * @tc.name: SetAndGetX002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, SetAndGetX002, TestSize.Level1)
{
    std::unique_ptr<Point3> point3Ptr_ = std::make_unique<Point3>();
    ASSERT_TRUE(point3Ptr_ != nullptr);
    point3Ptr_->SetX(333.9f);
    scalar x = point3Ptr_->GetX();
    EXPECT_EQ(x, 333.9f);
}

/**
 * @tc.name: SetAndGetY001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, SetAndGetY001, TestSize.Level1)
{
    std::unique_ptr<Point3> point3Ptr_ = std::make_unique<Point3>();
    ASSERT_TRUE(point3Ptr_ != nullptr);
    point3Ptr_->SetY(11.6f);
    scalar y = point3Ptr_->GetY();
    EXPECT_EQ(y, 11.6f);
}

/**
 * @tc.name: SetAndGetY002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, SetAndGetY002, TestSize.Level1)
{
    std::unique_ptr<Point3> point3Ptr_ = std::make_unique<Point3>();
    ASSERT_TRUE(point3Ptr_ != nullptr);
    point3Ptr_->SetY(333.9f);
    scalar y = point3Ptr_->GetY();
    EXPECT_EQ(y, 333.9f);
}

/**
 * @tc.name: SetAndGetZ001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, SetAndGetZ001, TestSize.Level1)
{
    std::unique_ptr<Point3> point3Ptr_ = std::make_unique<Point3>();
    ASSERT_TRUE(point3Ptr_ != nullptr);
    point3Ptr_->SetZ(101.3f);
    scalar z = point3Ptr_->GetZ();
    EXPECT_EQ(z, 101.3f);
}

/**
 * @tc.name: SetAndGetZ002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, SetAndGetZ002, TestSize.Level1)
{
    std::unique_ptr<Point3> point3Ptr_ = std::make_unique<Point3>();
    ASSERT_TRUE(point3Ptr_ != nullptr);
    point3Ptr_->SetZ(333.9f);
    scalar z = point3Ptr_->GetZ();
    EXPECT_EQ(z, 333.9f);
}

/**
 * @tc.name: operatorAddEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorAddEqual001, TestSize.Level1)
{
    Point3 p;
    Point3 point3;
    point3 += p;
}

/**
 * @tc.name: operatorMinusEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorMinusEqual001, TestSize.Level1)
{
    Point3 p;
    Point3 point3;
    point3 -= p;
}

/**
 * @tc.name: operatorMultiplyEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorMultiplyEqual001, TestSize.Level1)
{
    Point3 point3;
    point3 *= 103.3f;
}

/**
 * @tc.name: operatorMultiplyEqual002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorMultiplyEqual002, TestSize.Level1)
{
    Point3 point3;
    point3 *= 133.3f;
}

/**
 * @tc.name: operatordivideEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatordivideEqual001, TestSize.Level1)
{
    Point3 p;
    Point3 point3;
    point3 -= p;
}

/**
 * @tc.name: operatordivideEqual002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatordivideEqual002, TestSize.Level1)
{
    Point3 p;
    Point3 point3;
    point3 -= p;
}

/**
 * @tc.name: operatorAdd1001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorAdd1001, TestSize.Level1)
{
    Point3 point1;
    Point3 point2;
    Point3 pointF= point1 + point2;
}

/**
 * @tc.name: operatorAdd2001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorAdd2001, TestSize.Level1)
{
    Point3 p1;
    Point3 p2;
    Point3 P3 = p1+p2;
}

/**
 * @tc.name: operatorAddl001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorMinus001, TestSize.Level1)
{
    Point3 point1;
    Point3 point2;
    Point3 pointF = point1 - point2;
}

/**
 * @tc.name: operatorMultiply1001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorMultiply001, TestSize.Level1)
{
    Point3 point1;
    Point3 pointF = 100.9f * point1;
}

/**
 * @tc.name: operatorMultiply1002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorMultiply002, TestSize.Level1)
{
    Point3 point1;
    Point3 pointF = 22.4f * point1;
}

/**
 * @tc.name: operatorMultiply2001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorMultiply2001, TestSize.Level1)
{
    Point3 point1;
    Point3 pointF = point1 * 99.3f;
}

/**
 * @tc.name: operatorMultiply2002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorMultiply2002, TestSize.Level1)
{
    Point3 point1;
    Point3 pointF = point1 * 22.4f;
}

/**
 * @tc.name: operatorDivide2001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorDivide2001, TestSize.Level1)
{
    Point3 point1;
    Point3 pointF = point1 / 22.4f;
}

/**
 * @tc.name: operatorDivide2002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorDivide2002, TestSize.Level1)
{
    Point3 point1;
    Point3 pointF = point1 / 500.8f;
}

/**
 * @tc.name: operatorEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorEEqual001, TestSize.Level1)
{
    Point3 point1;
    Point3 point2;
    ASSERT_TRUE(point2 != point1);
}

/**
 * @tc.name: operatorNotEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Point3Test, operatorNotEEqual001, TestSize.Level1)
{
    Point3 pointF1;
    Point3 pointF2;
    ASSERT_TRUE(pointF2 != pointF1);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS