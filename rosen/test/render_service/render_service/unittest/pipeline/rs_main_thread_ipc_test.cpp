/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <parameter.h>
#include <parameters.h>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "rs_test_util.h"

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_uni_render_engine.h"
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_system_properties.h"
#if defined(ACCESSIBILITY_ENABLE)
#include "accessibility_config.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMainThreadIPCTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMainThreadIPCTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSMainThreadIPCTest::TearDownTestCase() {}
void RSMainThreadIPCTest::SetUp() {}
void RSMainThreadIPCTest::TearDown() {}

/**
 * @tc.name: UnRegisterOcclusionChangeCallback
 * @tc.desc: Test RSMainThreadIPCTest.Animate
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadIPCTest, UnRegisterOcclusionChangeCallback, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->UnRegisterOcclusionChangeCallback(0);
}

/**
 * @tc.name: ProcessSyncRSTransactionData001
 * @tc.desc: Test ProcessSyncRSTransactionData when TransactionData do not need sync
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadIPCTest, ProcessSyncRSTransactionData001, TestSize.Level1)
{
    // when IsNeedSync() is false
    auto mainThread = RSMainThread::Instance();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    pid_t pid = 0;
    rsTransactionData->SetSyncId(1);
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);

    // when syncTransactionData_ is not empty and SyncId is larger
    rsTransactionData = std::make_unique<RSTransactionData>();
    rsTransactionData->MarkNeedSync();
    rsTransactionData->SetSyncId(0);
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);

    // when syncTransactionData_ is not empty and SyncId is equal or smaller
    rsTransactionData->SetSyncTransactionNum(1);
    rsTransactionData->SetSyncId(1);
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);
}

/**
 * @tc.name: ProcessSyncRSTransactionData002
 * @tc.desc: Test ProcessSyncRSTransactionData when TransactionData do not need sync
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadIPCTest, ProcessSyncRSTransactionData002, TestSize.Level1)
{
    // when IsNeedSync() is true & syncTransactionData_ is empty & isNeedCloseSync is true
    auto mainThread = RSMainThread::Instance();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    pid_t pid = 0;
    rsTransactionData->MarkNeedSync();
    rsTransactionData->MarkNeedCloseSync();
    rsTransactionData->SetSyncTransactionNum(1);
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);
}

/**
 * @tc.name: ProcessSyncTransactionCount
 * @tc.desc: Test ProcessSyncTransactionCount
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadIPCTest, ProcessSyncTransactionCount, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto rsTransactionData = std::make_unique<RSTransactionData>();

    rsTransactionData->SetParentPid(-1);
    mainThread->ProcessSyncTransactionCount(rsTransactionData);
    auto parentPid = rsTransactionData->GetParentPid();
    ASSERT_EQ(parentPid, -1);

    rsTransactionData->SetSyncTransactionNum(1);
    mainThread->ProcessSyncTransactionCount(rsTransactionData);
    ASSERT_EQ(rsTransactionData->GetSyncTransactionNum(), 1);

    rsTransactionData->MarkNeedCloseSync();
    mainThread->ProcessSyncTransactionCount(rsTransactionData);
    mainThread->StartSyncTransactionFallbackTask(rsTransactionData);
    ASSERT_EQ(rsTransactionData->IsNeedCloseSync(), true);
}

/**
 * @tc.name: ClassifyRSTransactionData001
 * @tc.desc: Test ClassifyRSTransactionData when nodeId is 0
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadIPCTest, ClassifyRSTransactionData001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->pendingEffectiveCommands_.clear();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = nullptr;
    NodeId nodeId = 0;
    FollowType followType = FollowType::NONE;
    rsTransactionData->AddCommand(command, nodeId, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->pendingEffectiveCommands_.empty(), true);
}

/**
 * @tc.name: ClassifyRSTransactionData002
 * @tc.desc: Test ClassifyRSTransactionData when nodeId is 1
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadIPCTest, ClassifyRSTransactionData002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->pendingEffectiveCommands_.clear();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = nullptr;
    NodeId nodeId = 1;
    FollowType followType = FollowType::NONE;
    rsTransactionData->AddCommand(command, nodeId, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->pendingEffectiveCommands_.empty(), true);
}

/**
 * @tc.name: ClassifyRSTransactionData003
 * @tc.desc: Test ClassifyRSTransactionData when followType is FOLLOW_TO_PARENT
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadIPCTest, ClassifyRSTransactionData003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->pendingEffectiveCommands_.clear();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = nullptr;
    NodeId nodeId = 1;
    FollowType followType = FollowType::FOLLOW_TO_PARENT;
    rsTransactionData->AddCommand(command, nodeId, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->cachedCommands_[nodeId].empty(), true);
}

/**
 * @tc.name: ClassifyRSTransactionData004
 * @tc.desc: Test ClassifyRSTransactionData when followType is FOLLOW_TO_PARENT
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadIPCTest, ClassifyRSTransactionData004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    // build the NodeTree
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context = {};
    auto node = std::make_shared<RSBaseRenderNode>(nodeId, context);
    auto childNode = std::make_shared<RSBaseRenderNode>(nodeId + 1, context);
    int index = 0;
    node->SetIsOnTheTree(true);
    node->AddChild(node, index);
    ASSERT_EQ(static_cast<int>(node->GetChildrenCount()), 0);
    node->AddChild(childNode, index);
    ASSERT_EQ(static_cast<int>(node->GetChildrenCount()), 1);
    ASSERT_TRUE(childNode->IsOnTheTree());

    mainThread->cachedCommands_.clear();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = nullptr;
    FollowType followType = FollowType::FOLLOW_TO_SELF;
    rsTransactionData->AddCommand(command, nodeId, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->cachedCommands_[nodeId].empty(), true);

    mainThread->cachedCommands_.clear();
    rsTransactionData = std::make_unique<RSTransactionData>();
    command = nullptr;
    followType = FollowType::FOLLOW_TO_PARENT;
    rsTransactionData->AddCommand(command, nodeId + 1, followType);
    mainThread->ClassifyRSTransactionData(rsTransactionData);
    ASSERT_EQ(mainThread->cachedCommands_[nodeId + 1].empty(), true);
}

/**
 * @tc.name: RecvAndProcessRSTransactionDataImmediatelyTest
 * @tc.desc: Test ecvAndProcessRSTransactionDataImmediately when transactionData is null
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RSMainThreadIPCTest, RecvAndProcessRSTransactionDataImmediatelyTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    mainThread->RecvAndProcessRSTransactionDataImmediately(transactionData);
    ASSERT_EQ(transactionData, nullptr);
}

/**
 * @tc.name: RecvRSTransactionData
 * @tc.desc: Test RecvRSTransactionData, when TransactionData is null
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadIPCTest, RecvRSTransactionData, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    mainThread->RecvRSTransactionData(transactionData);
    ASSERT_EQ(transactionData, nullptr);
}

/**
 * @tc.name: CallbackDrawContextStatusToWMS
 * @tc.desc: CallbackDrawContextStatusToWMS test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, CallbackDrawContextStatusToWMS, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->curDrawStatusVec_.push_back(0);
    mainThread->curDrawStatusVec_.push_back(1);
    mainThread->lastDrawStatusMap_.insert({1, 0});
    mainThread->occlusionListeners_.insert({0, nullptr});
    mainThread->CallbackDrawContextStatusToWMS();
    mainThread->curDrawStatusVec_.clear();
    mainThread->lastDrawStatusMap_.clear();
    mainThread->occlusionListeners_.clear();
}

/**
 * @tc.name: CallbackToWMS001
 * @tc.desc: CallbackToWMS test visible not changed
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, CallbackToWMS001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    VisibleData curVisVec;
    curVisVec.push_back({0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE});
    mainThread->lastVisVec_ = curVisVec;
    mainThread->CallbackToWMS(curVisVec);
    mainThread->lastVisVec_.clear();
}

/**
 * @tc.name: CallbackToWMS002
 * @tc.desc: CallbackToWMS test visible changed
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, CallbackToWMS002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    VisibleData curVisVec;
    curVisVec.push_back({0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE});
    curVisVec.push_back({1, WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE});
    mainThread->lastVisVec_.push_back({0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE});
    mainThread->lastVisVec_.push_back({1, WINDOW_LAYER_INFO_TYPE::INVISIBLE});
    mainThread->occlusionListeners_.insert({0, nullptr});
    mainThread->CallbackToWMS(curVisVec);
    mainThread->occlusionListeners_.clear();
    mainThread->lastVisVec_.clear();
}

/**
 * @tc.name: SurfaceOcclusionCallback001
 * @tc.desc: SurfaceOcclusionCallback with empty nodemap
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, SurfaceOcclusionCallback001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare listeners
    std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t> info(0, nullptr, {}, 0);
    mainThread->surfaceOcclusionListeners_.insert({0, info});
    // run
    mainThread->SurfaceOcclusionCallback();
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: SurfaceOcclusionCallback002
 * @tc.desc: SurfaceOcclusionCallback with corresponding nodemap
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, SurfaceOcclusionCallback002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare listeners
    std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t> info(0, nullptr, {}, 0);
    mainThread->surfaceOcclusionListeners_.insert({1, info});
    mainThread->surfaceOcclusionListeners_.insert({2, info});
    mainThread->surfaceOcclusionListeners_.insert({3, info});
    //prepare nodemap
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    node1->SetIsOnTheTree(true);
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    node2->SetIsOnTheTree(true);
    node2->instanceRootNodeId_ = INVALID_NODEID;
    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    node3->SetIsOnTheTree(true);
    node3->instanceRootNodeId_ = 1;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    // run
    mainThread->SurfaceOcclusionCallback();
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: RecvRSTransactionData002
 * @tc.desc: RecvRSTransactionData002 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, RecvRSTransactionData002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    auto transactionData1 = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    mainThread->RecvRSTransactionData(transactionData1);
    auto transactionData2 = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = false;
    mainThread->RecvRSTransactionData(transactionData2);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ClassifyRSTransactionData005
 * @tc.desc: ClassifyRSTransactionData005 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, ClassifyRSTransactionData005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.emplace(1, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    // prepare transactionData
    auto data = std::make_unique<RSTransactionData>();
    int dataIndex = 1;
    data->SetIndex(dataIndex);
    int dataPayloadSize = 3;
    data->payload_.resize(dataPayloadSize);
    NodeId id = 0;
    data->payload_[id] = std::tuple<NodeId,
        FollowType, std::unique_ptr<RSCommand>>(id, FollowType::NONE, nullptr);
    id = 1;
    data->payload_[id] = std::tuple<NodeId,
        FollowType, std::unique_ptr<RSCommand>>(id, FollowType::FOLLOW_TO_SELF, nullptr);
    id = 2;
    data->payload_[id] = std::tuple<NodeId,
        FollowType, std::unique_ptr<RSCommand>>(id, FollowType::FOLLOW_TO_PARENT, nullptr);
    mainThread->ClassifyRSTransactionData(data);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: RegisterSurfaceOcclusionChangeCallBack001
 * @tc.desc: RegisterSurfaceOcclusionChangeCallBack001 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, RegisterSurfaceOcclusionChangeCallBack001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 0;
    pid_t pid = 0;
    sptr<RSISurfaceOcclusionChangeCallback> callback = nullptr;
    std::vector<float> partitionPoints = {};
    mainThread->RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
    ASSERT_NE(mainThread->surfaceOcclusionListeners_.size(), 0);
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: RegisterSurfaceOcclusionChangeCallBack002
 * @tc.desc: RegisterSurfaceOcclusionChangeCallBack002 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, RegisterSurfaceOcclusionChangeCallBack002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 0;
    pid_t pid = 0;
    sptr<RSISurfaceOcclusionChangeCallback> callback = nullptr;
    std::vector<float> partitionPoints = {1.0f};
    mainThread->RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
    ASSERT_NE(mainThread->surfaceOcclusionListeners_.size(), 0);
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: ClearSurfaceOcclusionChangeCallBack
 * @tc.desc: RegisterSurfaceOcclusionChangeCallBack Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, ClearSurfaceOcclusionChangeCallback, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare listeners
    pid_t pid = 1;
    int level = 0;
    NodeId id = 1;
    std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t> info1(pid, nullptr, {}, level);
    mainThread->surfaceOcclusionListeners_.insert({id, info1});
    id = 2;
    pid = 2;
    std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t> info2(pid, nullptr, {}, level);
    mainThread->surfaceOcclusionListeners_.insert({id, info2});
    mainThread->ClearSurfaceOcclusionChangeCallback(pid);
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: SendCommands
 * @tc.desc: SendCommands Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, SendCommands, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SendCommands();
}

/**
 * @tc.name: ClearTransactionDataPidInfo001
 * @tc.desc: ClearTransactionDataPidInfo Test, remotePid = 0
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, ClearTransactionDataPidInfo001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    int remotePid = 0;
    mainThread->ClearTransactionDataPidInfo(remotePid);
}

/**
 * @tc.name: ClearTransactionDataPidInfo002
 * @tc.desc: ClearTransactionDataPidInfo Test, remotePid > 0
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, ClearTransactionDataPidInfo002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    int remotePid = 1;
    mainThread->ClearTransactionDataPidInfo(remotePid);
}

/**
 * @tc.name: AddTransactionDataPidInfo001
 * @tc.desc: AddTransactionDataPidInfo Test, no UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, AddTransactionDataPidInfo001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    pid_t pid = 0;
    mainThread->AddTransactionDataPidInfo(pid);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: AddTransactionDataPidInfo002
 * @tc.desc: AddTransactionDataPidInfo Test, UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadIPCTest, AddTransactionDataPidInfo002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    pid_t pid = 0;
    mainThread->AddTransactionDataPidInfo(pid);
    mainThread->isUniRender_ = isUniRender;
}
} // namespace OHOS::Rosen
