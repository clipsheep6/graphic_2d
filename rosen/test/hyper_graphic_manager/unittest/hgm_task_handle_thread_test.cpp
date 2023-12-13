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

#include <gtest/gtest.h>
#include <test_header.h>

#include "hgm_task_handle_thread.h"
#include <thread>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HgmTaskHandleThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HgmTaskHandleThreadTest::SetUpTestCase() {}
void HgmTaskHandleThreadTest::TearDownTestCase() {}
void HgmTaskHandleThreadTest::SetUp() {}
void HgmTaskHandleThreadTest::TearDown() {}


/*
 * @tc.name: Instance
 * @tc.desc: Test Instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, Instance, TestSize.Level1)
{
    HgmTaskHandleThread& instance1 = HgmTaskHandleThread::Instance();
    HgmTaskHandleThread& instance2 = HgmTaskHandleThread::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

/*
 * @tc.name: PostTask001
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostTask001, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    HgmTaskHandleThread::Instance().PostTask(func);
    int64_t delayTime = 1;
    HgmTaskHandleThread::Instance().PostTask(func, delayTime);
}

/*
 * @tc.name: PostTask002
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostTask002, TestSize.Level1)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    int count = 0;
    instance.PostTask([&count](){ count++; }, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(count, 1);
}

/*
 * @tc.name: PostTask003
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostTask003, TestSize.Level1)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    int count = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    instance.PostTask([&count](){ count++; }, 1000);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_EQ(count, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    auto end_time = std::chrono::high_resolution_clock::now();
    EXPECT_EQ(count, 1);
    EXPECT_GE(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count(), 1000);
}
} // namespace Rosen
} // namespace OHOS
