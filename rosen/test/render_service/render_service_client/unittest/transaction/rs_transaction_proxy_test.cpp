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

#include "transaction/rs_transaction_proxy.h"
#include "transaction/rs_render_thread_client.h"
#include "command/rs_base_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSTransactionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionProxyTest::SetUpTestCase() {}
void RSTransactionProxyTest::TearDownTestCase() {}
void RSTransactionProxyTest::SetUp() {}
void RSTransactionProxyTest::TearDown() {}

/**
 * @tc.name: SetRenderThreadClient001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */

HWTEST_F(RSTransactionProxyTest, SetRenderThreadClient001, TestSize.Level1)
{
    std::unique_ptr<RSIRenderClient> renderThreadClient = nullptr;
    ASSERT_EQ(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
}

/**
 * @tc.name: FlushImplicitTransaction001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction001, TestSize.Level1)
{
    uint64_t timestamp = 1; 
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: AddCommand001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommand001, TestSize.Level1)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(1, 1, 1);
    bool isRenderServiceCommand = true;
    FollowType followType = FollowType::NONE;
    NodeId nodeId = 10;
    RSTransactionProxy::GetInstance()->AddCommand(command, isRenderServiceCommand, followType, nodeId);
}

/**
 * @tc.name: FlushImplicitTransaction002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction002, TestSize.Level1)
{
    uint64_t timestamp = 1; 
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: AddCommand002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommand002, TestSize.Level1)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(1, 1, 1);
    bool isRenderServiceCommand = false;
    FollowType followType = FollowType::NONE;
    NodeId nodeId = 10;
    RSTransactionProxy::GetInstance()->AddCommand(command, isRenderServiceCommand, followType, nodeId);
}

/**
 * @tc.name: ExecuteSynchronousTask001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, ExecuteSynchronousTask001, TestSize.Level1)
{
    std::shared_ptr<RSSyncTask> task = nullptr;
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task);
}

/**
 * @tc.name: SetRenderThreadClient002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, SetRenderThreadClient002, TestSize.Level1)
{
    auto renderThreadClient = RSIRenderClient::CreateRenderThreadClient();
    ASSERT_NE(renderThreadClient, nullptr);
    RSTransactionProxy::GetInstance()->SetRenderThreadClient(renderThreadClient);
}

/**
 * @tc.name: FlushImplicitTransaction003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction003, TestSize.Level1)
{
    uint64_t timestamp = 1; 
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: AddCommand003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommand003, TestSize.Level1)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(1, 1, 1);
    bool isRenderServiceCommand = false;
    FollowType followType = FollowType::NONE;
    NodeId nodeId = 10;
    RSTransactionProxy::GetInstance()->AddCommand(command, isRenderServiceCommand, followType, nodeId);
}

/**
 * @tc.name: FlushImplicitTransaction004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction004, TestSize.Level1)
{
    uint64_t timestamp = 1; 
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: ExecuteSynchronousTask002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, ExecuteSynchronousTask002, TestSize.Level1)
{
    std::shared_ptr<RSSyncTask> task = nullptr;
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task);
}

/**
 * @tc.name: AddCommandFromRT001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommandFromRT001, TestSize.Level1)
{
    std::unique_ptr<RSCommand> command = nullptr;
    NodeId nodeId = 10;
    FollowType followType = FollowType::NONE;
    RSTransactionProxy::GetInstance()->AddCommandFromRT(command, nodeId, followType);
}

/**
 * @tc.name: MarkTransactionNeedSync001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, MarkTransactionNeedSync001, TestSize.Level1)
{
    RSTransactionProxy::GetInstance()->MarkTransactionNeedSync();
}

/**
 * @tc.name: MarkTransactionNeedCloseSync001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, MarkTransactionNeedCloseSync001, TestSize.Level1)
{
    int32_t transactionCount = 1;
    RSTransactionProxy::GetInstance()->MarkTransactionNeedCloseSync(transactionCount);
}

/**
 * @tc.name: FlushImplicitTransaction005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction005, TestSize.Level1)
{
    uint64_t timestamp = 1; 
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransaction006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransaction006, TestSize.Level1)
{
    uint64_t timestamp = 1; 
    RSTransactionProxy::GetInstance()->StartSyncTransaction();
    RSTransactionProxy::GetInstance()->Begin();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction(timestamp);
}

/**
 * @tc.name: FlushImplicitTransactionFromRT001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransactionFromRT001, TestSize.Level1)
{
    uint64_t timestamp = 1;
    RSTransactionProxy::GetInstance()->FlushImplicitTransactionFromRT(timestamp);
}

/**
 * @tc.name: AddCommandFromRT002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, AddCommandFromRT002, TestSize.Level1)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(1, 1, 1);
    NodeId nodeId = 10;
    FollowType followType = FollowType::NONE;
    RSTransactionProxy::GetInstance()->AddCommandFromRT(command, nodeId, followType);
}

/**
 * @tc.name: FlushImplicitTransactionFromRT002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, FlushImplicitTransactionFromRT002, TestSize.Level1)
{
    uint64_t timestamp = 1;
    RSTransactionProxy::GetInstance()->FlushImplicitTransactionFromRT(timestamp);
}

/**
 * @tc.name: CloseSyncTransaction001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, CloseSyncTransaction001, TestSize.Level1)
{
    RSTransactionProxy::GetInstance()->CloseSyncTransaction();
}

/**
 * @tc.name: CommitSyncTransaction001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, CommitSyncTransaction001, TestSize.Level1)
{
    uint64_t timestamp = 1;
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timestamp);
}

/**
 * @tc.name: CommitSyncTransaction002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, CommitSyncTransaction002, TestSize.Level1)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(1, 1, 1);
    bool isRenderServiceCommand = false;
    FollowType followType = FollowType::NONE;
    NodeId nodeId = 10;
    RSTransactionProxy::GetInstance()->AddCommand(command, isRenderServiceCommand, followType, nodeId);
    uint64_t timestamp = 1;
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timestamp);
}

/**
 * @tc.name: CommitSyncTransaction003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, CommitSyncTransaction003, TestSize.Level1)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(1, 1, 1);
    bool isRenderServiceCommand = true;
    FollowType followType = FollowType::NONE;
    NodeId nodeId = 10;
    RSTransactionProxy::GetInstance()->AddCommand(command, isRenderServiceCommand, followType, nodeId);
    uint64_t timestamp = 1;
    RSTransactionProxy::GetInstance()->CommitSyncTransaction(timestamp);
}

/**
 * @tc.name: MarkTransactionNeedSync002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, MarkTransactionNeedSync002, TestSize.Level1)
{
    RSTransactionProxy::GetInstance()->MarkTransactionNeedSync();
}

/**
 * @tc.name: MarkTransactionNeedCloseSync002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionProxyTest, MarkTransactionNeedCloseSync002, TestSize.Level1)
{
    int32_t transactionCount = 1;
    RSTransactionProxy::GetInstance()->MarkTransactionNeedCloseSync(transactionCount);
}
}   // namespace Rosen
}   // namespace OHOS