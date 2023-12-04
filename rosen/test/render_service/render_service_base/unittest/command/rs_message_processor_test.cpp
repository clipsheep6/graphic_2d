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

#include "command/rs_message_processor.h"
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSMessageProcessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMessageProcessorTest::SetUpTestCase() {}
void RSMessageProcessorTest::TearDownTestCase() {}
void RSMessageProcessorTest::SetUp() {}
void RSMessageProcessorTest::TearDown() {}

/**
 * @tc.name: testing
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSMessageProcessorTest, testing, TestSize.Level1)
{
    uint32_t pid = 1;
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction(pid));
    EXPECT_NE(nullptr, RSMessageProcessor::Instance().GetTransaction(pid));
    auto map = RSMessageProcessor::Instance().GetAllTransactions();
    EXPECT_NE(0, map.size());
}

} // namespace Rosen
} // namespace OHOS