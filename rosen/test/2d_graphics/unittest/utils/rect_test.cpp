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
 * @tc.name: UtilsRectTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest001, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    EXPECT_EQ(0, rectF->GetLeft());
    EXPECT_EQ(0, rectF->GetTop());
    EXPECT_EQ(0, rectF->GetRight());
    EXPECT_EQ(0, rectF->GetBottom());
}

/**
 * @tc.name: UtilsRectTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest002, TestSize.Level1)
{
    RectF rectf1;
    rectf1.SetLeft(1.0f);
    RectF rectf2(rectf1);
    EXPECT_EQ(rectf1.GetLeft(), rectf2.GetLeft());
}

/**
 * @tc.name: UtilsRectTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest003, TestSize.Level1)
{
    RectF rectf1;
    rectf1.SetLeft(2.0f);
    RectF rectf2(rectf1);
    EXPECT_EQ(rectf1.GetLeft(), rectf2.GetLeft());
}

/**
 * @tc.name: UtilsRectTest004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest004, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(1, rectF->GetLeft());
    EXPECT_EQ(2, rectF->GetTop());
    EXPECT_EQ(3, rectF->GetRight());
    EXPECT_EQ(4, rectF->GetBottom());
}

/**
 * @tc.name: UtilsRectTest005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest005, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(4.0f, 3.0f, 2.0f, 1.0f);
    EXPECT_EQ(4, rectF->GetLeft());
    EXPECT_EQ(3, rectF->GetTop());
    EXPECT_EQ(2, rectF->GetRight());
    EXPECT_EQ(1, rectF->GetBottom());
}

/**
 * @tc.name: UtilsRectTest006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest006, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(4.0f, 3.0f, 2.0f, 1.0f);
    EXPECT_FALSE(rectF->IsValid());
}

/**
 * @tc.name: UtilsRectTest007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest007, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_TRUE(rectF->IsValid());
}

/**
 * @tc.name: UtilsRectTest008
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest008, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetLeft(1.0f);
    EXPECT_EQ(1, rectF->GetLeft());
}

/**
 * @tc.name: UtilsRectTest009
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest009, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetLeft(2.0f);
    EXPECT_EQ(2, rectF->GetLeft());
}

/**
 * @tc.name: UtilsRectTest010
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest010, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetRight(1.0f);
    EXPECT_EQ(1, rectF->GetRight());
}

/**
 * @tc.name: UtilsRectTest011
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest011, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetRight(2.0f);
    EXPECT_EQ(2, rectF->GetRight());
}

/**
 * @tc.name: UtilsRectTest012
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest012, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetTop(1.0f);
    EXPECT_EQ(1, rectF->GetTop());
}

/**
 * @tc.name: UtilsRectTest013
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest013, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetTop(2.0f);
    EXPECT_EQ(2, rectF->GetTop());
}

/**
 * @tc.name: UtilsRectTest014
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest014, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetBottom(1.0f);
    EXPECT_EQ(1, rectF->GetBottom());
}

/**
 * @tc.name: UtilsRectTest015
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest015, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>();
    rectF->SetBottom(2.0f);
    EXPECT_EQ(2, rectF->GetBottom());
}

/**
 * @tc.name: UtilsRectTest016
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest016, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(1.0f, 2.0f, 3.0f, 4.0f);
    rectF->Offset(1.0f, 2.0f);
    EXPECT_EQ(2, rectF->GetLeft());
    EXPECT_EQ(4, rectF->GetRight());
    EXPECT_EQ(4, rectF->GetTop());
    EXPECT_EQ(6, rectF->GetBottom());
}

/**
 * @tc.name: UtilsRectTest017
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest017, TestSize.Level1)
{
    std::unique_ptr<RectF> rectF = std::make_unique<RectF>(1.0f, 2.0f, 3.0f, 4.0f);
    rectF->Offset(2.0f, 1.0f);
    EXPECT_EQ(3, rectF->GetLeft());
    EXPECT_EQ(5, rectF->GetRight());
    EXPECT_EQ(3, rectF->GetTop());
    EXPECT_EQ(5, rectF->GetBottom());
}

/**
 * @tc.name: UtilsRectTest018
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest018, TestSize.Level1)
{
    RectF rectf1;
    RectF rectf2;
    rectf1.SetLeft(1.0f);
    EXPECT_FALSE(rectf1==rectf2);
}

/**
 * @tc.name: UtilsRectTest019
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest019, TestSize.Level1)
{
    RectF rectf1;
    RectF rectf2;
    EXPECT_TRUE(rectf1==rectf2);
}

/**
 * @tc.name: UtilsRectTest020
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest020, TestSize.Level1)
{
    RectF rectf1;
    RectF rectf2;
    EXPECT_FALSE(rectf1!=rectf2);
}

/**
 * @tc.name: UtilsRectTest021
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest021, TestSize.Level1)
{
    RectF rectf1;
    RectF rectf2;
    rectf2.SetLeft(2.0f);
    EXPECT_TRUE(rectf1!=rectf2);
}

/**
 * @tc.name: UtilsRectTest022
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest022, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    EXPECT_EQ(0, rectI->GetLeft());
    EXPECT_EQ(0, rectI->GetTop());
    EXPECT_EQ(0, rectI->GetRight());
    EXPECT_EQ(0, rectI->GetBottom());
}

/**
 * @tc.name: UtilsRectTest023
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest023, TestSize.Level1)
{
    RectI recti1;
    recti1.SetLeft(1.0f);
    RectI recti2(recti1);
    EXPECT_EQ(recti1.GetLeft(), recti2.GetLeft());
}

/**
 * @tc.name: UtilsRectTest024
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest024, TestSize.Level1)
{
    RectI recti1;
    recti1.SetLeft(2.0f);
    RectI recti2(recti1);
    EXPECT_EQ(recti1.GetLeft(), recti2.GetLeft());
}

/**
 * @tc.name: UtilsRectTest025
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest025, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(1, rectI->GetLeft());
    EXPECT_EQ(2, rectI->GetTop());
    EXPECT_EQ(3, rectI->GetRight());
    EXPECT_EQ(4, rectI->GetBottom());
}

/**
 * @tc.name: UtilsRectTest026
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest026, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(4.0f, 3.0f, 2.0f, 1.0f);
    EXPECT_EQ(4, rectI->GetLeft());
    EXPECT_EQ(3, rectI->GetTop());
    EXPECT_EQ(2, rectI->GetRight());
    EXPECT_EQ(1, rectI->GetBottom());
}

/**
 * @tc.name: UtilsRectTest027
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest027, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(4.0f, 3.0f, 2.0f, 1.0f);
    EXPECT_FALSE(rectI->IsValid());
}

/**
 * @tc.name: UtilsRectTest028
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest028, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_TRUE(rectI->IsValid());
}

/**
 * @tc.name: UtilsRectTest029
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest029, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetLeft(1.0f);
    EXPECT_EQ(1, rectI->GetLeft());
}

/**
 * @tc.name: UtilsRectTest030
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest030, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetLeft(2.0f);
    EXPECT_EQ(2, rectI->GetLeft());
}

/**
 * @tc.name: UtilsRectTest031
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest031, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetRight(1.0f);
    EXPECT_EQ(1, rectI->GetRight());
}

/**
 * @tc.name: UtilsRectTest032
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest032, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetRight(2.0f);
    EXPECT_EQ(2, rectI->GetRight());
}

/**
 * @tc.name: UtilsRectTest033
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest033, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetTop(1.0f);
    EXPECT_EQ(1, rectI->GetTop());
}

/**
 * @tc.name: UtilsRectTest034
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest034, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetTop(2.0f);
    EXPECT_EQ(2, rectI->GetTop());
}

/**
 * @tc.name: UtilsRectTest035
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest035, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetBottom(1.0f);
    EXPECT_EQ(1, rectI->GetBottom());
}

/**
 * @tc.name: UtilsRectTest036
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest036, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>();
    rectI->SetBottom(2.0f);
    EXPECT_EQ(2, rectI->GetBottom());
}

/**
 * @tc.name: UtilsRectTest037
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest037, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(1.0f, 2.0f, 3.0f, 4.0f);
    rectI->Offset(1.0f, 2.0f);
    EXPECT_EQ(2, rectI->GetLeft());
    EXPECT_EQ(4, rectI->GetRight());
    EXPECT_EQ(4, rectI->GetTop());
    EXPECT_EQ(6, rectI->GetBottom());
}

/**
 * @tc.name: UtilsRectTest038
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest038, TestSize.Level1)
{
    std::unique_ptr<RectI> rectI = std::make_unique<RectI>(1.0f, 2.0f, 3.0f, 4.0f);
    rectI->Offset(2.0f, 1.0f);
    EXPECT_EQ(3, rectI->GetLeft());
    EXPECT_EQ(5, rectI->GetRight());
    EXPECT_EQ(3, rectI->GetTop());
    EXPECT_EQ(5, rectI->GetBottom());
}

/**
 * @tc.name: UtilsRectTest039
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest039, TestSize.Level1)
{
    RectI recti1;
    RectI recti2;
    recti1.SetLeft(1.0f);
    EXPECT_FALSE(recti1==recti2);
}

/**
 * @tc.name: UtilsRectTest040
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest040, TestSize.Level1)
{
    RectI recti1;
    RectI recti2;
    EXPECT_TRUE(recti1==recti2);
}

/**
 * @tc.name: UtilsRectTest041
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest041, TestSize.Level1)
{
    RectI recti1;
    RectI recti2;
    EXPECT_FALSE(recti1!=recti2);
}

/**
 * @tc.name: UtilsRectTest042
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RectTest, UtilsRectTest042, TestSize.Level1)
{
    RectI recti1;
    RectI recti2;
    recti2.SetLeft(2.0f);
    EXPECT_TRUE(recti1!=recti2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS