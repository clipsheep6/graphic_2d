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
#include "utils/round_rect.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RoundRectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RoundRectTest::SetUpTestCase() {}
void RoundRectTest::TearDownTestCase() {}
void RoundRectTest::SetUp() {}
void RoundRectTest::TearDown() {}

/**
 * @tc.name: CreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, CreateAndDestory001, TestSize.Level1)
{
    // The best way to create RoundRect.
    RoundRect roundRect_;
}

/**
 * @tc.name: CreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, CreateAndDestory002, TestSize.Level1)
{
    // The best way to create RoundRect.
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRectPtr_ != nullptr);
}

/**
 * @tc.name: CreateAndDestory003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, CreateAndDestory003, TestSize.Level1)
{
    // The best way to create RoundRect.
    RoundRect roundRect;
    RoundRect r=roundRect;
}

/**
 * @tc.name: CreateAndDestory004
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, CreateAndDestory004, TestSize.Level1)
{
    // The best way to create RoundRect.
    Rect r;
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>(r, 12.6f, 77.4f);
    ASSERT_TRUE(roundRectPtr_ != nullptr);
}

/**
 * @tc.name: CreateAndDestory005
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, CreateAndDestory005, TestSize.Level1)
{
    // The best way to create RoundRect.
    Rect r;
    std::vector<Point> radiusXY;
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>(r, radiusXY);
    ASSERT_TRUE(roundRectPtr_ != nullptr);
}

/**
 * @tc.name: SetAndGetCornerRadius001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, SetAndGetCornerRadius001, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRectPtr_ != nullptr);
    roundRectPtr_->SetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS, 111.3f, 84.5f);
}

/**
 * @tc.name: SetAndGetCornerRadius2001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, SetAndGetCornerRadius2001, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRectPtr_ != nullptr);
    roundRectPtr_->GetCornerRadius(Drawing::RoundRect::BOTTOM_RIGHT_POS);
}

/**
 * @tc.name: SetAndGetCornerRadius002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, SetAndGetCornerRadius002, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRectPtr_ != nullptr);
    roundRectPtr_->SetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS, 111.3f, 84.5f);
}

/**
 * @tc.name: SetAndGetCornerRadius2002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, SetAndGetCornerRadius2002, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRectPtr_ != nullptr);
    roundRectPtr_->GetCornerRadius(Drawing::RoundRect::TOP_RIGHT_POS);
}

/**
 * @tc.name: SetAndGetCornerRadius003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, SetAndGetCornerRadius003, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRectPtr_ != nullptr);
    roundRectPtr_->SetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS, 111.3f, 84.5f);
}

/**
 * @tc.name: SetAndGetCornerRadius2003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, SetAndGetCornerRadius2003, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRectPtr_ != nullptr);
    roundRectPtr_->GetCornerRadius(Drawing::RoundRect::TOP_LEFT_POS);
}

/**
 * @tc.name: SetAndGetRect001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, SetAndGetRect001, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRectPtr_ != nullptr);
    Rect rect;
    roundRectPtr_->SetRect(rect);
    Rect rect2 = roundRectPtr_->GetRect();
    EXPECT_EQ(rect2, rect);
}

/**
 * @tc.name: Offset001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, Offset001, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRectPtr_ != nullptr);
    roundRectPtr_->Offset(54.6f, 432.8f);
}

/**
 * @tc.name: Offset002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RoundRectTest, Offset002, TestSize.Level1)
{
    std::unique_ptr<RoundRect> roundRectPtr_ = std::make_unique<RoundRect>();
    ASSERT_TRUE(roundRectPtr_ != nullptr);
    roundRectPtr_->Offset(200, 40.8f);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS