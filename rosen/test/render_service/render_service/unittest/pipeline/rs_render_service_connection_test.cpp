/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "rs_irender_service.h"
#include "screen_manager/rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<RSIConnectionToken> token_;
};

sptr<RSIConnectionToken> RSRenderServiceConnectionTest::token_ = new IRemoteStub<RSIConnectionToken>();

void RSRenderServiceConnectionTest::SetUpTestCase() {}
void RSRenderServiceConnectionTest::TearDownTestCase() {}
void RSRenderServiceConnectionTest::SetUp() {}
void RSRenderServiceConnectionTest::TearDown() {}

/**
 * @tc.name: RSRenderServiceConnection
 * @tc.desc: Construct RSRenderServiceConnection
 * @tc.type: FUNC
 * @tc.require: issueI7ANSF
 */
HWTEST_F(RSRenderServiceConnectionTest, RSRenderServiceConnection, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->Start();
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    sptr<RSRenderServiceConnection> connection = new RSRenderServiceConnection(0, nullptr, RSMainThread::Instance(), screenManager, token_->AsObject(), nullptr);
    ASSERT_NE(connection, nullptr);
}

/**
 * @tc.name: RSRenderServiceConnection
 * @tc.desc: GetMemoryGraphic
 * @tc.type: FUNC
 * @tc.require: issueI7ANSF
 */
HWTEST_F(RSRenderServiceConnectionTest, GetMemoryGraphic, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->Start();

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    sptr<RSRenderServiceConnection> connection = new RSRenderServiceConnection(0, nullptr, RSMainThread::Instance(), screenManager, token_->AsObject(), nullptr);
    ASSERT_NE(connection, nullptr);

    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto id = screenManager->CreateVirtualScreen(name, width, height, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(connection->SetVirtualScreenSurface(id, psurface)));

    connection->GetMemoryGraphic(::getpid());
    connection->GetMemoryGraphics();
    screenManager->RemoveVirtualScreen(id);
}

/**
 * @tc.name: RSRenderServiceConnection
 * @tc.desc: GetScreenInfo
 * @tc.type: FUNC
 * @tc.require: issueI7ANSF
 */
HWTEST_F(RSRenderServiceConnectionTest, GetScreenInfo, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->Start();

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    sptr<RSRenderServiceConnection> connection = new RSRenderServiceConnection(0,
        nullptr, RSMainThread::Instance(), screenManager, token_->AsObject(), nullptr);
    ASSERT_NE(connection, nullptr);

    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto id = screenManager->CreateVirtualScreen(name, width, height, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(connection->SetVirtualScreenSurface(id, psurface)));

    RSScreenHDRCapability HDRCapability;
    connection->GetScreenHDRCapability(id, HDRCapability);
    screenManager->RemoveVirtualScreen(id);
}
} // namespace OHOS::Rosen