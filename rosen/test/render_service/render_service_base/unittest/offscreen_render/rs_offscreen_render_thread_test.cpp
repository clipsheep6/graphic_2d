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
#include "include/offscreen_render/rs_offscreen_render_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSOffscreenRenderThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void Add();
};

void RSOffscreenRenderThreadTest::SetUpTestCase() {}
void RSOffscreenRenderThreadTest::TearDownTestCase() {}
void RSOffscreenRenderThreadTest::SetUp() {}
void RSOffscreenRenderThreadTest::TearDown() {}
void RSOffscreenRenderThreadTest::Add()
{
    return;
}

/**
 * @tc.name: PostTask001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOffscreenRenderThreadTest, PostTask001, TestSize.Level1)
{
    EXPECT_EQ(RSOffscreenRenderThread::Instance().handler_, nullptr);
    RSOffscreenRenderThread::Instance().PostTask(RSOffscreenRenderThreadTest::Add);
}

/**
 * @tc.name: Stop001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOffscreenRenderThreadTest, Stop001, TestSize.Level1)
{
    EXPECT_EQ(RSOffscreenRenderThread::Instance().runner_, nullptr);
    EXPECT_EQ(RSOffscreenRenderThread::Instance().handler_, nullptr);
    RSOffscreenRenderThread::Instance().Stop();
    EXPECT_EQ(RSOffscreenRenderThread::Instance().handler_, nullptr);
}

/**
 * @tc.name: Start001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOffscreenRenderThreadTest, Start001, TestSize.Level1)
{
    EXPECT_EQ(RSOffscreenRenderThread::Instance().runner_ , nullptr);
    EXPECT_EQ(RSOffscreenRenderThread::Instance().handler_, nullptr);
    RSOffscreenRenderThread::Instance().Start();
    EXPECT_NE(RSOffscreenRenderThread::Instance().runner_ , nullptr);
    EXPECT_NE(RSOffscreenRenderThread::Instance().handler_, nullptr);
}

/**
 * @tc.name: PostTask002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOffscreenRenderThreadTest, PostTask002, TestSize.Level1)
{
    RSOffscreenRenderThread::Instance().Start();
    EXPECT_NE(RSOffscreenRenderThread::Instance().handler_, nullptr);
    RSOffscreenRenderThread::Instance().PostTask(RSOffscreenRenderThreadTest::Add);
}

/**
 * @tc.name: Stop002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOffscreenRenderThreadTest, Stop002, TestSize.Level1)
{
    RSOffscreenRenderThread::Instance().Start();
    EXPECT_NE(RSOffscreenRenderThread::Instance().runner_ , nullptr);
    EXPECT_NE(RSOffscreenRenderThread::Instance().handler_, nullptr);
    RSOffscreenRenderThread::Instance().Stop();
    EXPECT_EQ(RSOffscreenRenderThread::Instance().handler_, nullptr);
}
}   // namespace OHOS::Rosen