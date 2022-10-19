/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "jank_detector/rs_jank_detector.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSJankDetectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSJankDetectorTest::SetUpTestCase() {}
void RSJankDetectorTest::TearDownTestCase() {}
void RSJankDetectorTest::SetUp() {}
void RSJankDetectorTest::TearDown() {}

/**
 * @tc.name: UpdateUiDrawFrameMsg001
 * @tc.desc: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSJankDetectorTest, UpdateUiDrawFrameMsg001, TestSize.Level1)
{
    auto detector = std::make_shared<RSJankDetector>();
    detector->UpdateUiDrawFrameMsg(0,0,"");
}

    
} // namespace OHOS::Rosen
