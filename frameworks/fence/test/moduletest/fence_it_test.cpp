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

#include "fence_it_test.h"

#include <chrono>
#include <sys/stat.h>
#include <unistd.h>
#include <thread>

#include <linux/sync_file.h>
#include "test_header.h"

namespace OHOS {
void FenceITest::SetUpTestCase()
{
}

void FenceITest::TearDownTestCase()
{
    csurface = nullptr;
    producer = nullptr;
    psurface = nullptr;
}

void FenceITest::OnBufferAvailable()
{
}

namespace {
    int32_t ret = 0;
    int32_t fenceFd = -1;
    int32_t timeline = 0;
    int act = -1;
    std::thread t([] (int32_t fd, int32_t timeline) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        act = TimelineActivate(timeline, 1);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        TimelineActivate(timeline, 1);
    }, fenceFd, timeline);
}

HWTEST_F(FenceITest, TestIsSupportSwSync01, testing::ext::TestSize.Level1)
{
    ASSERT_TRUE(IsSupportSwSync());
}

HWTEST_F(FenceITest, TestCreatTimeline01, testing::ext::TestSize.Level2)
{
    timeline = CreateTimeline();
    ASSERT_NE(timeline, -1);
}

HWTEST_F(FenceITest, TestCreatFenceFromTimeline01, testing::ext::TestSize.Level2)
{ 
    fenceFd = CreateFenceFromTimeline(timeline, "CreatFence", 2);
    ASSERT_NE(fenceFd, -1);
}

HWTEST_F(FenceITest, TestCreatFenceFromTimeline02, testing::ext::TestSize.Level2)
{ 
    fenceFd = CreateFenceFromTimeline(-1, "CreatFence", 2);
    ASSERT_NE(fenceFd, -1);
}

HWTEST_F(FenceITest, TestFenceHold01, testing::ext::TestSize.Level2)
{
    ret = FenceHold(-1, 1 * 1000);
    ASSERT_EQ(ret, -1);
}

HWTEST_F(FenceITest, TestFenceHold02, testing::ext::TestSize.Level2)
{
    ret = FenceHold(fenceFd, 3000);
    ASSERT_GT(ret, -1);
}

HWTEST_F(FenceITest, TestTimelineActive01, testing::ext::TestSize.Level2)
{
    t.join();
    ASSERT_GE(act, 0);
}

HWTEST_F(FenceITest, TestTimelineActive02, testing::ext::TestSize.Level2)
{
    int ret = TimelineActivate(-1, 1);
    ASSERT_EQ(ret, errno);
}


HWTEST_F(FenceITest, TestFenceGetStatus01, testing::ext::TestSize.Level2)
{
    FenceStatus fs = FenceGetStatus(fenceFd);
    ASSERT_NE(fs, ERROR);
}

HWTEST_F(FenceITest, TestFenceGetStatus02, testing::ext::TestSize.Level2)
{
    FenceStatus fs = FenceGetStatus(-1);
    ASSERT_EQ(fs, ERROR);
}

HWTEST_F(FenceITest, TestFenceMerge01, testing::ext::TestSize.Level2)
{
        int32_t ret = FenceMerge("merge", -1, -1);
        ASSERT_EQ(ret, -1);
}

HWTEST_F(FenceITest, TestFenceMerge02, testing::ext::TestSize.Level2)
{
        int32_t fenceFd = -1;
        int32_t timeline = 0;
        timeline = CreateTimeline();
        fenceFd = CreateFenceFromTimeline(timeline, "createFence", 2);
        int32_t fenceMerged = FenceMerge("merge", fenceFd, fenceFd);
        ASSERT_GE(fenceMerged, 0);
        close(fenceMerged);
        close(fenceFd);
        close(timeline);
}
}
