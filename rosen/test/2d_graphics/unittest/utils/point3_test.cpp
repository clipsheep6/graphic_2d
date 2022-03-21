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
 * @tc.name: UtilsPoint3Test001
 * @tc.desc: utils Point3 Function SetandGet XYZ Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test001, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    EXPECT_EQ(0.0f, point3->GetX());
    EXPECT_EQ(0.0f, point3->GetY());
    EXPECT_EQ(0.0f, point3->GetZ());
}

/**
 * @tc.name: UtilsPoint3Test002
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test002, TestSize.Level1)
{
    Point3 point1;
    point1.SetX(2.0f);
    Point3 point2(point1);
    EXPECT_TRUE(point1 == point2);
}

/**
 * @tc.name: UtilsPoint3Test003
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test003, TestSize.Level1)
{
    Point3 point1;
    point1.SetX(3.0f);
    Point3 point2(point1);
    EXPECT_TRUE(point1 == point2);
}

/**
 * @tc.name: UtilsPoint3Test004
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test004, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(1.0f, point3->GetX());
    EXPECT_EQ(2.0f, point3->GetY());
    EXPECT_EQ(3.0f, point3->GetZ());
}

/**
 * @tc.name: UtilsPoint3Test005
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test005, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>(4.0f, 5.0f, 6.0f);
    EXPECT_EQ(4.0f, point3->GetX());
    EXPECT_EQ(5.0f, point3->GetY());
    EXPECT_EQ(6.0f, point3->GetZ());
}

/**
 * @tc.name: UtilsPoint3Test006
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test006, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetX(1.0f);
    EXPECT_EQ(1.0f, point3->GetX());
}

/**
 * @tc.name: UtilsPoint3Test007
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test007, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetX(2.0f);
    EXPECT_EQ(2.0f, point3->GetX());
}

/**
 * @tc.name: UtilsPoint3Test008
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test008, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetY(1.0f);
    EXPECT_EQ(1.0f, point3->GetY());
}

/**
 * @tc.name: UtilsPoint3Test009
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test009, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetY(2.0f);
    EXPECT_EQ(2.0f, point3->GetY());
}

/**
 * @tc.name: UtilsPoint3Test010
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test010, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetZ(1.0f);
    EXPECT_EQ(1.0f, point3->GetZ());
}

/**
 * @tc.name: UtilsPoint3Test011
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test011, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetZ(2.0f);
    EXPECT_EQ(2.0f, point3->GetZ());
}

/**
 * @tc.name: UtilsPoint3Test012
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test012, TestSize.Level1)
{
    Point3 point1;
    Point3 point2(1.0f, 2.0f, 3.0f);
    point1 += point2;
    EXPECT_EQ(1.0f, point1.GetX());
    EXPECT_EQ(2.0f, point1.GetY());
    EXPECT_EQ(3.0f, point1.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test013
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test013, TestSize.Level1)
{
    Point3 point1;
    Point3 point2(3.0f, 2.0f, 1.0f);
    point1 += point2;
    EXPECT_EQ(3.0f, point1.GetX());
    EXPECT_EQ(2.0f, point1.GetY());
    EXPECT_EQ(1.0f, point1.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test014
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test014, TestSize.Level1)
{
    Point3 point1;
    Point3 point2(1.0f, 2.0f, 3.0f);
    point1 -= point2;
    EXPECT_EQ(-1.0f, point1.GetX());
    EXPECT_EQ(-2.0f, point1.GetY());
    EXPECT_EQ(-3.0f, point1.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test015
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test015, TestSize.Level1)
{
    Point3 point1;
    Point3 point2(3.0f, 2.0f, 1.0f);
    point1 -= point2;
    EXPECT_EQ(-3.0f, point1.GetX());
    EXPECT_EQ(-2.0f, point1.GetY());
    EXPECT_EQ(-1.0f, point1.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test016
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test016, TestSize.Level1)
{
    Point3 point3(1.0f, 2.0f, 3.0f);
    point3 *= 2;
    EXPECT_EQ(2.0f, point3.GetX());
    EXPECT_EQ(4.0f, point3.GetY());
    EXPECT_EQ(6.0f, point3.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test017
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test017, TestSize.Level1)
{
    Point3 point3(3.0f, 2.0f, 1.0f);
    point3 *= 2;
    EXPECT_EQ(6.0f, point3.GetX());
    EXPECT_EQ(4.0f, point3.GetY());
    EXPECT_EQ(2.0f, point3.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test018
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test018, TestSize.Level1)
{
    Point3 point3(2.0f, 4.0f, 6.0f);
    point3 /= 2.0f;
    EXPECT_EQ(1.0f, point3.GetX());
    EXPECT_EQ(2.0f, point3.GetY());
    EXPECT_EQ(3.0f, point3.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test019
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test019, TestSize.Level1)
{
    Point3 point3(4.0f, 8.0f, 10.0f);
    point3 /= 2.0f;
    EXPECT_EQ(2.0f, point3.GetX());
    EXPECT_EQ(4.0f, point3.GetY());
    EXPECT_EQ(5.0f, point3.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test020
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test020, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2;
    Point3 point3 = point1 + point2;
    EXPECT_EQ(1.0f, point3.GetX());
    EXPECT_EQ(2.0f, point3.GetY());
    EXPECT_EQ(3.0f, point3.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test021
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test021, TestSize.Level1)
{
    Point3 point1(3.0f, 2.0f, 1.0f);
    Point3 point2;
    Point3 point3 = point1 + point2;
    EXPECT_EQ(3.0f, point3.GetX());
    EXPECT_EQ(2.0f, point3.GetY());
    EXPECT_EQ(1.0f, point3.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test022
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test022, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2;
    Point3 point3 = point1 - point2;
    EXPECT_EQ(1.0f, point3.GetX());
    EXPECT_EQ(2.0f, point3.GetY());
    EXPECT_EQ(3.0f, point3.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test023
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test023, TestSize.Level1)
{
    Point3 point1(3.0f, 2.0f, 1.0f);
    Point3 point2;
    Point3 point3 = point1 - point2;
    EXPECT_EQ(3.0f, point3.GetX());
    EXPECT_EQ(2.0f, point3.GetY());
    EXPECT_EQ(1.0f, point3.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test024
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test024, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = 2.0f * point1;
    EXPECT_EQ(2.0f, point2.GetX());
    EXPECT_EQ(4.0f, point2.GetY());
    EXPECT_EQ(6.0f, point2.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test025
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test025, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = 3.0f * point1;
    EXPECT_EQ(3.0f, point2.GetX());
    EXPECT_EQ(6.0f, point2.GetY());
    EXPECT_EQ(9.0f, point2.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test026
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test026, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = point1 * 2.0f;
    EXPECT_EQ(2.0f, point2.GetX());
    EXPECT_EQ(4.0f, point2.GetY());
    EXPECT_EQ(6.0f, point2.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test027
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test027, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = point1 * 3.0f;
    EXPECT_EQ(3.0f, point2.GetX());
    EXPECT_EQ(6.0f, point2.GetY());
    EXPECT_EQ(9.0f, point2.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test028
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test028, TestSize.Level1)
{
    Point3 point1(2.0f, 4.0f, 6.0f);
    Point3 point2 = point1 / 2.0f;
    EXPECT_EQ(1.0f, point2.GetX());
    EXPECT_EQ(2.0f, point2.GetY());
    EXPECT_EQ(3.0f, point2.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test029
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test029, TestSize.Level1)
{
    Point3 point1(3.0f, 6.0f, 9.0f);
    Point3 point2 = point1 / 3.0f;
    EXPECT_EQ(1.0f, point2.GetX());
    EXPECT_EQ(2.0f, point2.GetY());
    EXPECT_EQ(3.0f, point2.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test030
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test030, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = +point1;
    EXPECT_EQ(1.0f, point2.GetX());
    EXPECT_EQ(2.0f, point2.GetY());
    EXPECT_EQ(3.0f, point2.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test031
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test031, TestSize.Level1)
{
    Point3 point1(3.0f, 2.0f, 1.0f);
    Point3 point2 = +point1;
    EXPECT_EQ(3.0f, point2.GetX());
    EXPECT_EQ(2.0f, point2.GetY());
    EXPECT_EQ(1.0f, point2.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test032
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test032, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = -point1;
    EXPECT_EQ(-1.0f, point2.GetX());
    EXPECT_EQ(-2.0f, point2.GetY());
    EXPECT_EQ(-3.0f, point2.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test033
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test033, TestSize.Level1)
{
    Point3 point1(3.0f, 2.0f, 1.0f);
    Point3 point2 = -point1;
    EXPECT_EQ(-3.0f, point2.GetX());
    EXPECT_EQ(-2.0f, point2.GetY());
    EXPECT_EQ(-1.0f, point2.GetZ());
}

/**
 * @tc.name: UtilsPoint3Test034
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test034, TestSize.Level1)
{
    Point3 point1;
    Point3 point2;
    EXPECT_TRUE(point1 == point2);
}

/**
 * @tc.name: UtilsPoint3Test035
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test035, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2;
    EXPECT_FALSE(point1 == point2);
}

/**
 * @tc.name: UtilsPoint3Test036
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test036, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2;
    EXPECT_TRUE(point1 != point2);
}

/**
 * @tc.name: UtilsPoint3Test037
 * @tc.desc: utils Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, UtilsPoint3Test037, TestSize.Level1)
{
    Point3 point1;
    Point3 point2;
    EXPECT_FALSE(point1 != point2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS