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

#include "vsync_it_test.h"

#include <securec.h>

#include <gtest/gtest.h>
#include <hilog/log.h>
#include <vsync_helper.h>

using namespace OHOS;
#define LOG GTEST_LOG_(INFO)

class VsyncHelperItTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
    }
    static void TearDownTestCase()
    {
    }
};

namespace {

constexpr int64_t SEC_TO_USEC = 1000 * 1000;
constexpr int64_t ONE_FRAME_TIME = 1 * SEC_TO_USEC / 60; // 1second, 60Hz
constexpr int64_t PERFORMANCE_COUNT = 1000;
int64_t GetNowTime()
{
    struct timeval time;
    gettimeofday(&time, nullptr);
    return SEC_TO_USEC * time.tv_sec + time.tv_usec;
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_0100
 * @tc.name    TestCurrent1
 * @tc.desc    Current
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestCurrent1, testing::ext::TestSize.Level1)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    auto testFunc = []() {
        sptr<VsyncHelper> vsync = VsyncHelper::Current();
        ASSERT_NE(vsync, nullptr);

        AppExecFwk::EventRunner::Current()->Stop();
    };

    handler->PostTask(testFunc);
    runner->Run();
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_0200
 * @tc.name    TestCurrent2
 * @tc.desc    Current
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestCurrent2, testing::ext::TestSize.Level1)
{
    sptr<VsyncHelper> vsync = VsyncHelper::Current();
    ASSERT_EQ(vsync, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_0300
 * @tc.name    TestCurrent3
 * @tc.desc    Current
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestCurrent3, testing::ext::TestSize.Level2)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    auto testFunc = []() {
        auto startTime = GetNowTime();
        sptr<VsyncHelper> vsync = VsyncHelper::Current();
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;

        ASSERT_NE(vsync, nullptr);
        ASSERT_LT(timeSpend, ONE_FRAME_TIME);

        AppExecFwk::EventRunner::Current()->Stop();
    };

    handler->PostTask(testFunc);
    runner->Run();
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_0400
 * @tc.name    TestCurrent4
 * @tc.desc    Current
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestCurrent4, testing::ext::TestSize.Level2)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    auto testFunc = []() {
        auto startTime = GetNowTime();
        for (int i = 0; i < PERFORMANCE_COUNT; i++) {
            sptr<VsyncHelper> vsync = VsyncHelper::Current();
            ASSERT_NE(vsync, nullptr);
        }
        auto endTime = GetNowTime();
        auto meanTime = (endTime - startTime) / PERFORMANCE_COUNT;

        ASSERT_LT(meanTime, ONE_FRAME_TIME);

        AppExecFwk::EventRunner::Current()->Stop();
    };

    handler->PostTask(testFunc);
    runner->Run();
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_0500
 * @tc.name    TestFromHandler1
 * @tc.desc    FromHandler
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestFromHandler1, testing::ext::TestSize.Level1)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);

    ASSERT_NE(vsync, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_0600
 * @tc.name    TestFromHandler2
 * @tc.desc    FromHandler
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestFromHandler2, testing::ext::TestSize.Level2)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    auto startTime = GetNowTime();
    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;

    ASSERT_NE(vsync, nullptr);
    ASSERT_LT(timeSpend, ONE_FRAME_TIME);
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_0700
 * @tc.name    TestFromHandler3
 * @tc.desc    FromHandler
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestFromHandler3, testing::ext::TestSize.Level2)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    auto startTime = GetNowTime();
    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
        ASSERT_NE(vsync, nullptr);
    }
    auto endTime = GetNowTime();
    auto meanTime = (endTime - startTime) / PERFORMANCE_COUNT;

    ASSERT_LT(meanTime, ONE_FRAME_TIME);
}

auto cbFunc = [](int64_t, void *) {  };
struct FrameCallback cb = {
    .timestamp_ = 0,
    .userdata_ = nullptr,
    .callback_ = cbFunc,
};

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_0800
 * @tc.name    TestRequestFrameCallback1
 * @tc.desc    RequestFrameCallback
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestRequestFrameCallback1, testing::ext::TestSize.Level1)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
    ASSERT_NE(vsync, nullptr);

    auto vret = vsync->RequestFrameCallback(cb);
    ASSERT_EQ(vret, VSYNC_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_0900
 * @tc.name    TestRequestFrameCallback2
 * @tc.desc    RequestFrameCallback
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestRequestFrameCallback2, testing::ext::TestSize.Level2)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
    ASSERT_NE(vsync, nullptr);

    auto startTime = GetNowTime();
    auto vret = vsync->RequestFrameCallback(cb);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;

    ASSERT_EQ(vret, VSYNC_ERROR_OK);
    ASSERT_LT(timeSpend, ONE_FRAME_TIME);
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_1000
 * @tc.name    TestRequestFrameCallback3
 * @tc.desc    RequestFrameCallback
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestRequestFrameCallback3, testing::ext::TestSize.Level2)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
    ASSERT_NE(vsync, nullptr);

    auto startTime = GetNowTime();
    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        auto vret = vsync->RequestFrameCallback(cb);
        ASSERT_EQ(vret, VSYNC_ERROR_OK);
    }
    auto endTime = GetNowTime();
    auto meanTime = (endTime - startTime) / PERFORMANCE_COUNT;

    ASSERT_LT(meanTime, ONE_FRAME_TIME);
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_1100
 * @tc.name    TestRequestFrameCallback5
 * @tc.desc    RequestFrameCallback
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestRequestFrameCallback5, testing::ext::TestSize.Level1)
{
    struct FrameCallback ncb = {
        .frequency_ = 67,
        .timestamp_ = 0,
        .userdata_ = nullptr,
        .callback_ = cbFunc,
    };

    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
    ASSERT_NE(vsync, nullptr);

    auto vret = vsync->RequestFrameCallback(ncb);
    ASSERT_EQ(vret, VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_1200
 * @tc.name    TestRequestFrameCallback4
 * @tc.desc    RequestFrameCallback
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestRequestFrameCallback4, testing::ext::TestSize.Level1)
{
    struct FrameCallback ncb = {
        .timestamp_ = 0,
        .userdata_ = nullptr,
        .callback_ = nullptr,
    };

    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
    ASSERT_NE(vsync, nullptr);

    auto vret = vsync->RequestFrameCallback(ncb);
    ASSERT_EQ(vret, VSYNC_ERROR_NULLPTR);
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_1300
 * @tc.name    TestGetSupportedVsyncFrequencys1
 * @tc.desc    GetSupportedVsyncFrequencys
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestGetSupportedVsyncFrequencys1, testing::ext::TestSize.Level1)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
    ASSERT_NE(vsync, nullptr);

    std::vector<uint32_t> vec;
    auto vret = vsync->GetSupportedVsyncFrequencys(vec);
    ASSERT_EQ(vret, VSYNC_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_1400
 * @tc.name    TestGetSupportedVsyncFrequencys1
 * @tc.desc    GetSupportedVsyncFrequencys
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestGetSupportedVsyncFrequencys2, testing::ext::TestSize.Level1)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
    ASSERT_NE(vsync, nullptr);

    std::vector<uint32_t> vec;
    auto vret = vsync->GetSupportedVsyncFrequencys(vec);
    ASSERT_EQ(vret, VSYNC_ERROR_OK);
    ASSERT_FALSE(vec.empty());
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_1500
 * @tc.name    TestGetSupportedVsyncFrequencys2
 * @tc.desc    GetSupportedVsyncFrequencys
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestGetSupportedVsyncFrequencys3, testing::ext::TestSize.Level2)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    std::vector<uint32_t> vec;

    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
    ASSERT_NE(vsync, nullptr);

    auto startTime = GetNowTime();
    vsync->GetSupportedVsyncFrequencys(vec);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, ONE_FRAME_TIME);
}

/*
 * @tc.number    FRAMEWORKS_VSYNC_TEST_MODULETEST_VSYNC_IT_TEST_1600
 * @tc.name    TestGetSupportedVsyncFrequencys3
 * @tc.desc    GetSupportedVsyncFrequencys
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(VsyncHelperItTest, TestGetSupportedVsyncFrequencys4, testing::ext::TestSize.Level2)
{
    auto runner = AppExecFwk::EventRunner::Create();
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    std::vector<uint32_t> vec;

    sptr<VsyncHelper> vsync = VsyncHelper::FromHandler(handler);
    ASSERT_NE(vsync, nullptr);

    auto startTime = GetNowTime();
    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        auto vret = vsync->GetSupportedVsyncFrequencys(vec);
        ASSERT_EQ(vret, VSYNC_ERROR_OK);
    }
    auto endTime = GetNowTime();
    auto meanTime = (endTime - startTime) / PERFORMANCE_COUNT;

    ASSERT_LT(meanTime, ONE_FRAME_TIME);
}

}   //namespace