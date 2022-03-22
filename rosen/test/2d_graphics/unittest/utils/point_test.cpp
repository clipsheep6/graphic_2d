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
 * @tc.name: UtilsPointTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest001, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>();
    EXPECT_EQ(0.0f, pointf->GetX());
    EXPECT_EQ(0.0f, pointf->GetY());
}

/**
 * @tc.name: UtilsPointTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest002, TestSize.Level1)
{
    PointF pointf1;
    pointf1.SetX(1.0f);
    PointF pointf2(pointf1);
    EXPECT_EQ(pointf1.GetX(), pointf2.GetX());
}

/**
 * @tc.name: UtilsPointTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest003, TestSize.Level1)
{
    PointF pointf1;
    pointf1.SetX(2.0f);
    PointF pointf2(pointf1);
    EXPECT_EQ(pointf1.GetX(), pointf2.GetX());
}

/**
 * @tc.name: UtilsPointTest004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest004, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>(1.0f, 2.0f);
    EXPECT_EQ(1.0f, pointf->GetX());
    EXPECT_EQ(2.0f, pointf->GetY());
}

/**
 * @tc.name: UtilsPointTest005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest005, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>(2.0f, 1.0f);
    EXPECT_EQ(2.0f, pointf->GetX());
    EXPECT_EQ(1.0f, pointf->GetY());
}

/**
 * @tc.name: UtilsPointTest006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest006, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>();
    pointf->SetX(1.0f);
    EXPECT_EQ(1.0f, pointf->GetX());
}

/**
 * @tc.name: UtilsPointTest007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest007, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>();
    pointf->SetX(2.0f);
    EXPECT_EQ(2.0f, pointf->GetX());
}

/**
 * @tc.name: UtilsPointTest008
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest008, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>();
    pointf->SetY(1.0f);
    EXPECT_EQ(1.0f, pointf->GetY());
}

/**
 * @tc.name: UtilsPointTest009
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest009, TestSize.Level1)
{
    std::unique_ptr<PointF> pointf = std::make_unique<PointF>();
    pointf->SetY(2.0f);
    EXPECT_EQ(2.0f, pointf->GetY());
}

/**
 * @tc.name: UtilsPointTest010
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest010, TestSize.Level1)
{
    PointF pointf1;
    PointF pointf2(1.0f, 2.0f);
    pointf1 += pointf2;
    EXPECT_EQ(1.0f, pointf1.GetX());
    EXPECT_EQ(2.0f, pointf1.GetY());
}

/**
 * @tc.name: UtilsPointTest011
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest011, TestSize.Level1)
{
    PointF pointf1;
    PointF pointf2(2.0f, 1.0f);
    pointf1 += pointf2;
    EXPECT_EQ(2.0f, pointf1.GetX());
    EXPECT_EQ(1.0f, pointf1.GetY());
}

/**
 * @tc.name: UtilsPointTest012
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest012, TestSize.Level1)
{
    PointF pointf1;
    PointF pointf2(1.0f, 2.0f);
    pointf1 -= pointf2;
    EXPECT_EQ(-1.0f, pointf1.GetX());
    EXPECT_EQ(-2.0f, pointf1.GetY());
}

/**
 * @tc.name: UtilsPointTest013
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest013, TestSize.Level1)
{
    PointF pointf1;
    PointF pointf2(2.0f, 1.0f);
    pointf1 -= pointf2;
    EXPECT_EQ(-2.0f, pointf1.GetX());
    EXPECT_EQ(-1.0f, pointf1.GetY());
}

/**
 * @tc.name: UtilsPointTest014
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest014, TestSize.Level1)
{
    PointF pointf1(2.0f, 3.0f);
    pointf1 *= 2.0f;
    EXPECT_EQ(4.0f, pointf1.GetX());
    EXPECT_EQ(6.0f, pointf1.GetY());
}

/**
 * @tc.name: UtilsPointTest015
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest015, TestSize.Level1)
{
    PointF pointf1(4.0f, 5.0f);
    pointf1 *= 2;
    EXPECT_EQ(8.0f, pointf1.GetX());
    EXPECT_EQ(10.0f, pointf1.GetY());
}

/**
 * @tc.name: UtilsPointTest016
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest016, TestSize.Level1)
{
    PointF pointf1(4.0f, 6.0f);
    pointf1 /= 2.0f;
    EXPECT_EQ(2.0f, pointf1.GetX());
    EXPECT_EQ(3.0f, pointf1.GetY());
}

/**
 * @tc.name: UtilsPointTest017
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest017, TestSize.Level1)
{
    PointF pointf1(6.0f, 8.0f);
    pointf1 /= 2;
    EXPECT_EQ(3.0f, pointf1.GetX());
    EXPECT_EQ(4.0f, pointf1.GetY());
}

/**
 * @tc.name: UtilsPointTest018
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest018, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(2.0f, 1.0f);
    PointF pointf3 = pointf1 + pointf2;
    EXPECT_EQ(3.0f, pointf3.GetX());
    EXPECT_EQ(3.0f, pointf3.GetY());
}

/**
 * @tc.name: UtilsPointTest019
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest019, TestSize.Level1)
{
    PointF pointf1(2.0f, 3.0f);
    PointF pointf2(4.0f, 5.0f);
    PointF pointf3 = pointf1 + pointf2;
    EXPECT_EQ(5.0f, pointf3.GetX());
    EXPECT_EQ(9.0f, pointf3.GetY());
}

/**
 * @tc.name: UtilsPointTest020
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest020, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(2.0f, 1.0f);
    PointF pointf3 = pointf1 - pointf2;
    EXPECT_EQ(-1.0f, pointf3.GetX());
    EXPECT_EQ(1.0f, pointf3.GetY());
}

/**
 * @tc.name: UtilsPointTest021
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest021, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(3.0f, 4.0f);
    PointF pointf3 = pointf1 - pointf2;
    EXPECT_EQ(-2.0f, pointf3.GetX());
    EXPECT_EQ(-2.0f, pointf3.GetY());
}

/**
 * @tc.name: UtilsPointTest022
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest022, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = 3.0f * pointf1;
    EXPECT_EQ(3.0f, pointf2.GetX());
    EXPECT_EQ(6.0f, pointf2.GetY());
}

/**
 * @tc.name: UtilsPointTest023
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest023, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = 4.0f * pointf1;
    EXPECT_EQ(4.0f, pointf2.GetX());
    EXPECT_EQ(8.0f, pointf2.GetY());
}

/**
 * @tc.name: UtilsPointTest024
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest024, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = pointf1 * 3.0f;
    EXPECT_EQ(3.0f, pointf2.GetX());
    EXPECT_EQ(6.0f, pointf2.GetY());
}

/**
 * @tc.name: UtilsPointTest025
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest025, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = pointf1 * 4.0f;
    EXPECT_EQ(4.0f, pointf2.GetX());
    EXPECT_EQ(8.0f, pointf2.GetY());
}

/**
 * @tc.name: UtilsPointTest026
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest026, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = pointf1 / 1.0f;
    EXPECT_EQ(1.0f, pointf2.GetX());
    EXPECT_EQ(2.0f, pointf2.GetY());
}

/**
 * @tc.name: UtilsPointTest027
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest027, TestSize.Level1)
{
    PointF pointf1(4.0f, 8.0f);
    PointF pointf2 = pointf1 / 2.0f;
    EXPECT_EQ(2.0f, pointf2.GetX());
    EXPECT_EQ(4.0f, pointf2.GetY());
}

/**
 * @tc.name: UtilsPointTest028
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest028, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = +pointf1;
    EXPECT_EQ(1.0f, pointf2.GetX());
    EXPECT_EQ(2.0f, pointf2.GetY());
}

/**
 * @tc.name: UtilsPointTest029
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest029, TestSize.Level1)
{
    PointF pointf1(3.0f, 4.0f);
    PointF pointf2 = +pointf1;
    EXPECT_EQ(3.0f, pointf2.GetX());
    EXPECT_EQ(4.0f, pointf2.GetY());
}

/**
 * @tc.name: UtilsPointTest030
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest030, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2 = -pointf1;
    EXPECT_EQ(-1.0f, pointf2.GetX());
    EXPECT_EQ(-2.0f, pointf2.GetY());
}

/**
 * @tc.name: UtilsPointTest031
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest031, TestSize.Level1)
{
    PointF pointf1(2.0f, 3.0f);
    PointF pointf2 = -pointf1;
    EXPECT_EQ(-2.0f, pointf2.GetX());
    EXPECT_EQ(-3.0f, pointf2.GetY());
}

/**
 * @tc.name: UtilsPointTest032
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest032, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(1.0f, 2.0f);
    EXPECT_TRUE(pointf1 == pointf2);
}

/**
 * @tc.name: UtilsPointTest033
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest033, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(2.0f, 3.0f);
    EXPECT_FALSE(pointf1 == pointf2);
}

/**
 * @tc.name: UtilsPointTest034
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest034, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(2.0f, 3.0f);
    EXPECT_TRUE(pointf1 != pointf2);
}

/**
 * @tc.name: UtilsPointTest035
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest035, TestSize.Level1)
{
    PointF pointf1(1.0f, 2.0f);
    PointF pointf2(1.0f, 2.0f);
    EXPECT_FALSE(pointf1 != pointf2);
}

/**
 * @tc.name: UtilsPointTest036
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest036, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>();
    EXPECT_EQ(0.0f, pointi->GetX());
    EXPECT_EQ(0.0f, pointi->GetY());
}

/**
 * @tc.name: UtilsPointTest037
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest037, TestSize.Level1)
{
    PointI pointi1;
    pointi1.SetX(1.0f);
    PointI pointi2(pointi1);
    EXPECT_EQ(pointi1.GetX(), pointi2.GetX());
}

/**
 * @tc.name: UtilsPointTest038
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest038, TestSize.Level1)
{
    PointI pointi1;
    pointi1.SetX(2.0f);
    PointI pointi2(pointi1);
    EXPECT_EQ(pointi1.GetX(), pointi2.GetX());
}

/**
 * @tc.name: UtilsPointTest039
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest039, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>(1.0f, 2.0f);
    EXPECT_EQ(1.0f, pointi->GetX());
    EXPECT_EQ(2.0f, pointi->GetY());
}

/**
 * @tc.name: UtilsPointTest040
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest040, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>(2.0f, 1.0f);
    EXPECT_EQ(2.0f, pointi->GetX());
    EXPECT_EQ(1.0f, pointi->GetY());
}

/**
 * @tc.name: UtilsPointTest041
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest041, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>();
    pointi->SetX(1.0f);
    EXPECT_EQ(1.0f, pointi->GetX());
}

/**
 * @tc.name: UtilsPointTest042
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest042, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>();
    pointi->SetX(2.0f);
    EXPECT_EQ(2.0f, pointi->GetX());
}

/**
 * @tc.name: UtilsPointTest043
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest043, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>();
    pointi->SetY(1.0f);
    EXPECT_EQ(1.0f, pointi->GetY());
}

/**
 * @tc.name: UtilsPointTest044
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest044, TestSize.Level1)
{
    std::unique_ptr<PointI> pointi = std::make_unique<PointI>();
    pointi->SetY(2.0f);
    EXPECT_EQ(2.0f, pointi->GetY());
}

/**
 * @tc.name: UtilsPointTest045
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest045, TestSize.Level1)
{
    PointI pointi1;
    PointI pointi2(1.0f, 2.0f);
    pointi1 += pointi2;
    EXPECT_EQ(1.0f, pointi1.GetX());
    EXPECT_EQ(2.0f, pointi1.GetY());
}

/**
 * @tc.name: UtilsPointTest046
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest046, TestSize.Level1)
{
    PointI pointi1;
    PointI pointi2(2.0f, 1.0f);
    pointi1 += pointi2;
    EXPECT_EQ(2.0f, pointi1.GetX());
    EXPECT_EQ(1.0f, pointi1.GetY());
}

/**
 * @tc.name: UtilsPointTest047
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest047, TestSize.Level1)
{
    PointI pointi1;
    PointI pointi2(1.0f, 2.0f);
    pointi1 -= pointi2;
    EXPECT_EQ(-1.0f, pointi1.GetX());
    EXPECT_EQ(-2.0f, pointi1.GetY());
}

/**
 * @tc.name: UtilsPointTest048
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest048, TestSize.Level1)
{
    PointI pointi1;
    PointI pointi2(2.0f, 1.0f);
    pointi1 -= pointi2;
    EXPECT_EQ(-2.0f, pointi1.GetX());
    EXPECT_EQ(-1.0f, pointi1.GetY());
}

/**
 * @tc.name: UtilsPointTest049
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest049, TestSize.Level1)
{
    PointI pointi1(2.0f, 3.0f);
    pointi1 *= 2;
    EXPECT_EQ(4.0f, pointi1.GetX());
    EXPECT_EQ(6.0f, pointi1.GetY());
}

/**
 * @tc.name: UtilsPointTest050
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest050, TestSize.Level1)
{
    PointI pointi1(4.0f, 5.0f);
    pointi1 *= 2.0f;
    EXPECT_EQ(8.0f, pointi1.GetX());
    EXPECT_EQ(10.0f, pointi1.GetY());
}

/**
 * @tc.name: UtilsPointTest051
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest051, TestSize.Level1)
{
    PointI pointi1(4.0f, 6.0f);
    pointi1 /= 2.0f;
    EXPECT_EQ(2.0f, pointi1.GetX());
    EXPECT_EQ(3.0f, pointi1.GetY());
}

/**
 * @tc.name: UtilsPointTest052
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest052, TestSize.Level1)
{
    PointI pointi1(6.0f, 8.0f);
    pointi1 /= 2.0f;
    EXPECT_EQ(3.0f, pointi1.GetX());
    EXPECT_EQ(4.0f, pointi1.GetY());
}

/**
 * @tc.name: UtilsPointTest053
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest053, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(2.0f, 1.0f);
    PointI pointi3 = pointi1 + pointi2;
    EXPECT_EQ(3.0f, pointi3.GetX());
    EXPECT_EQ(3.0f, pointi3.GetY());
}

/**
 * @tc.name: UtilsPointTest054
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest054, TestSize.Level1)
{
    PointI pointi1(2.0f, 3.0f);
    PointI pointi2(4.0f, 5.0f);
    PointI pointi3 = pointi1 + pointi2;
    EXPECT_EQ(5.0f, pointi3.GetX());
    EXPECT_EQ(9.0f, pointi3.GetY());
}

/**
 * @tc.name: UtilsPointTest055
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest055, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(2.0f, 1.0f);
    PointI pointi3 = pointi1 - pointi2;
    EXPECT_EQ(-1.0f, pointi3.GetX());
    EXPECT_EQ(1.0f, pointi3.GetY());
}

/**
 * @tc.name: UtilsPointTest056
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest056, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(3.0f, 4.0f);
    PointI pointi3 = pointi1 - pointi2;
    EXPECT_EQ(-2.0f, pointi3.GetX());
    EXPECT_EQ(-2.0f, pointi3.GetY());
}

/**
 * @tc.name: UtilsPointTest057
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest057, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = 3.0f * pointi1;
    EXPECT_EQ(3.0f, pointi2.GetX());
    EXPECT_EQ(6.0f, pointi2.GetY());
}

/**
 * @tc.name: UtilsPointTest058
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest058, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = 4.0f * pointi1;
    EXPECT_EQ(4.0f, pointi2.GetX());
    EXPECT_EQ(8.0f, pointi2.GetY());
}

/**
 * @tc.name: UtilsPointTest059
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest059, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = pointi1 * 3.0f;
    EXPECT_EQ(3.0f, pointi2.GetX());
    EXPECT_EQ(6.0f, pointi2.GetY());
}

/**
 * @tc.name: UtilsPointTest060
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest060, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = pointi1 * 4.0f;
    EXPECT_EQ(4.0f, pointi2.GetX());
    EXPECT_EQ(8.0f, pointi2.GetY());
}

/**
 * @tc.name: UtilsPointTest061
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest061, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = pointi1 / 1.0f;
    EXPECT_EQ(1.0f, pointi2.GetX());
    EXPECT_EQ(2.0f, pointi2.GetY());
}

/**
 * @tc.name: UtilsPointTest062
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest062, TestSize.Level1)
{
    PointI pointi1(4.0f, 8.0f);
    PointI pointi2 = pointi1 / 2.0f;
    EXPECT_EQ(2.0f, pointi2.GetX());
    EXPECT_EQ(4.0f, pointi2.GetY());
}

/**
 * @tc.name: UtilsPointTest063
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest063, TestSize.Level1)
{    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = +pointi1;
    EXPECT_EQ(1.0f, pointi2.GetX());
    EXPECT_EQ(2.0f, pointi2.GetY());
}

/**
 * @tc.name: UtilsPointTest064
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest064, TestSize.Level1)
{
    PointI pointi1(3.0f, 4.0f);
    PointI pointi2 = +pointi1;
    EXPECT_EQ(3.0f, pointi2.GetX());
    EXPECT_EQ(4.0f, pointi2.GetY());
}

/**
 * @tc.name: UtilsPointTest065
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest065, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2 = -pointi1;
    EXPECT_EQ(-1.0f, pointi2.GetX());
    EXPECT_EQ(-2.0f, pointi2.GetY());
}

/**
 * @tc.name: UtilsPointTest066
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest066, TestSize.Level1)
{
    PointI pointi1(2.0f, 3.0f);
    PointI pointi2 = -pointi1;
    EXPECT_EQ(-2.0f, pointi2.GetX());
    EXPECT_EQ(-3.0f, pointi2.GetY());
}

/**
 * @tc.name: UtilsPointTest067
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest067, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(1.0f, 2.0f);
    EXPECT_TRUE(pointi1 == pointi2);
}

/**
 * @tc.name: UtilsPointTest068
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest068, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(2.0f, 3.0f);
    EXPECT_FALSE(pointi1 == pointi2);
}

/**
 * @tc.name: UtilsPointTest069
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest069, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(2.0f, 3.0f);
    EXPECT_TRUE(pointi1 != pointi2);
}

/**
 * @tc.name: UtilsPointTest070
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PointTest, UtilsPointTest070, TestSize.Level1)
{
    PointI pointi1(1.0f, 2.0f);
    PointI pointi2(1.0f, 2.0f);
    EXPECT_FALSE(pointi1 != pointi2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

