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
#include <parameter.h>
#include <parameters.h>
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_qos_thread.h"
#include "pipeline/rs_render_engine.h"
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_system_properties.h"
#include "rs_test_util.h"
#if defined(ACCESSIBILITY_ENABLE)
#include "accessibility_config.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint64_t REFRESH_PERIOD = 16666667;
constexpr uint64_t SKIP_COMMAND_FREQ_LIMIT = 30;
class RSMainThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void* CreateParallelSyncSignal(uint32_t count);
};

void RSMainThreadTest::SetUpTestCase() {}
void RSMainThreadTest::TearDownTestCase() {}
void RSMainThreadTest::SetUp() {}
void RSMainThreadTest::TearDown() {}
void* RSMainThreadTest::CreateParallelSyncSignal(uint32_t count)
{
    (void)(count);
    return nullptr;
}

/**
 * @tc.name: Start001
 * @tc.desc: Test RSMainThreadTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Start001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->Start();
}

/**
 * @tc.name: Start002
 * @tc.desc: Test RSMainThreadTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Start002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = nullptr;
    mainThread->Start();
}

/**
 * @tc.name: ProcessCommand
 * @tc.desc: Test RSMainThreadTest.ProcessCommand
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, ProcessCommand, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ProcessCommand();
    mainThread->isUniRender_ = true;
    mainThread->lastAnimateTimestamp_ = 0;
    mainThread->timestamp_ = REFRESH_PERIOD + 1;
    mainThread->context_->purgeType_ = RSContext::PurgeType::GENTLY;
    mainThread->ProcessCommand();
    mainThread->context_->purgeType_ = RSContext::PurgeType::STRONGLY;
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: RsEventParamDump
 * @tc.desc: Test RSMainThreadTest.RsEventParamDump
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RsEventParamDump, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::string str = "";
    mainThread->RsEventParamDump(str);
    ASSERT_TRUE(str.empty());
}

/**
 * @tc.name: RemoveRSEventDetector001
 * @tc.desc: Test RSMainThreadTest.RemoveRSEventDetector, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RemoveRSEventDetector001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    mainThread->RemoveRSEventDetector();
}

/**
 * @tc.name: RemoveRSEventDetector002
 * @tc.desc: Test RSMainThreadTest.RemoveRSEventDetector, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RemoveRSEventDetector002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->RemoveRSEventDetector();
}

/**
 * @tc.name: InitRSEventDetector
 * @tc.desc: Test RSMainThreadTest.InitRSEventDetector, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, InitRSEventDetector, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->InitRSEventDetector();
}

/**
 * @tc.name: SetRSEventDetectorLoopStartTag001
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopStartTag, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopStartTag001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    mainThread->SetRSEventDetectorLoopStartTag();
}

/**
 * @tc.name: SetRSEventDetectorLoopStartTag002
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopStartTag, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopStartTag002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->SetRSEventDetectorLoopStartTag();
}

/**
 * @tc.name: SetRSEventDetectorLoopFinishTag001
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopFinishTag, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopFinishTag001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->SetRSEventDetectorLoopFinishTag();
    mainThread->isUniRender_ = true;
    mainThread->SetRSEventDetectorLoopFinishTag();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: SetRSEventDetectorLoopFinishTag002
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopFinishTag, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopFinishTag002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->SetRSEventDetectorLoopFinishTag();
}

/**
 * @tc.name: WaitUtilUniRenderFinished
 * @tc.desc: Test RSMainThreadTest.WaitUtilUniRenderFinished
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, WaitUtilUniRenderFinished, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->NotifyUniRenderFinish();
    mainThread->WaitUtilUniRenderFinished();
    ASSERT_EQ(mainThread->uniRenderFinished_, true);
}

/**
 * @tc.name: ProcessCommandForDividedRender001
 * @tc.desc: Test RSMainThreadTest.ProcessCommandForDividedRender
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, ProcessCommandForDividedRender001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->ProcessCommandForDividedRender();
}

/**
 * @tc.name: CalcOcclusion
 * @tc.desc: Test RSMainThreadTest.CalcOcclusion, doWindowAnimate_ is false, isUniRender_ is true
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CalcOcclusion, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = false;
    mainThread->isUniRender_ = true;
    mainThread->CalcOcclusion();
}

/**
 * @tc.name: CheckSurfaceVisChanged001
 * @tc.desc: Test RSMainThreadTest.CheckSurfaceVisChanged, pidVisMap is empty
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CheckSurfaceVisChanged001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::map<uint32_t, RSVisibleLevel> pidVisMap;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    auto isVisibleChanged = mainThread->CheckSurfaceVisChanged(pidVisMap, curAllSurfaces);
    ASSERT_EQ(false, isVisibleChanged);
}

/**
 * @tc.name: CheckSurfaceVisChanged002
 * @tc.desc: Test RSMainThreadTest.CheckSurfaceVisChanged, pidVisMap is not empty
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CheckSurfaceVisChanged002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::map<uint32_t, RSVisibleLevel> pidVisMap;
    pidVisMap[0] = RSVisibleLevel::RS_ALL_VISIBLE;
    mainThread->lastPidVisMap_[0] = RSVisibleLevel::RS_INVISIBLE;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    auto isVisibleChanged = mainThread->CheckSurfaceVisChanged(pidVisMap, curAllSurfaces);
    ASSERT_EQ(true, isVisibleChanged);
}

/**
 * @tc.name: CheckSurfaceVisChanged003
 * @tc.desc: Test RSMainThreadTest.CheckSurfaceVisChanged, pidVisMap is not empty, lastPidVisMap_ equals to pidVisMap
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CheckSurfaceVisChanged003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::map<uint32_t, RSVisibleLevel> pidVisMap;
    pidVisMap[0] = RSVisibleLevel::RS_ALL_VISIBLE;
    mainThread->lastPidVisMap_[0] = RSVisibleLevel::RS_ALL_VISIBLE;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    auto isVisibleChanged = mainThread->CheckSurfaceVisChanged(pidVisMap, curAllSurfaces);
    ASSERT_EQ(false, isVisibleChanged);
}

/**
 * @tc.name: Animate001
 * @tc.desc: Test RSMainThreadTest.Animate, doWindowAnimate_ is false
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Animate001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = false;
    mainThread->Animate(0);
}

/**
 * @tc.name: Animate002
 * @tc.desc: Test RSMainThreadTest.Animate, doWindowAnimate_ is true
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Animate002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = true;
    mainThread->Animate(0);
}

/**
 * @tc.name: UnRegisterOcclusionChangeCallback
 * @tc.desc: Test RSMainThreadTest.Animate
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, UnRegisterOcclusionChangeCallback, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->UnRegisterOcclusionChangeCallback(0);
}

/**
 * @tc.name: QosStateDump001
 * @tc.desc: When qosCal_ is false, QosStateDump's dump string is "QOS is disabled\n"
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, QosStateDump001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    RSQosThread::GetInstance()->SetQosCal(false);
    std::string str = "";
    mainThread->QosStateDump(str);
    ASSERT_EQ(str, "QOS is disabled\n");
}

/**
 * @tc.name: QosStateDump002
 * @tc.desc: When qosCal_ is true, QosStateDump's dump string is "QOS is enabled\n"
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, QosStateDump002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    RSQosThread::GetInstance()->SetQosCal(true);
    std::string str = "";
    mainThread->QosStateDump(str);
    ASSERT_EQ(str, "QOS is enabled\n");
}

/**
 * @tc.name: RenderServiceTreeDump
 * @tc.desc: Test RSMainThreadTest.RenderServiceTreeDump, str is an empty string
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RenderServiceTreeDump, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    mainThread->RenderServiceTreeDump(str);
}

/**
 * @tc.name: SetFocusAppInfo
 * @tc.desc: Test RSMainThreadTest.SetFocusAppInfo, input pid, uid is -1, str is an empty string
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetFocusAppInfo, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str, 0);
}

/**
 * @tc.name: ProcessSyncRSTransactionData001
 * @tc.desc: Test ProcessSyncRSTransactionData when TransactionData do not need sync
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, ProcessSyncRSTransactionData001, TestSize.Level1)
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

    // when syncTransactionData_ is not empty and SyncId is equal or smaller
    rsTransactionData->SetSyncTransactionNum(1);
    rsTransactionData->SetSyncId(1);
    mainThread->syncTransactionCount_ = 1;
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), true);
}

/**
 * @tc.name: ProcessSyncRSTransactionData002
 * @tc.desc: Test ProcessSyncRSTransactionData when TransactionData do not need sync
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, ProcessSyncRSTransactionData002, TestSize.Level1)
{
    // when IsNeedSync() is true & syncTransactionData_ is empty & isNeedCloseSync is true
    auto mainThread = RSMainThread::Instance();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    pid_t pid = 0;
    rsTransactionData->MarkNeedSync();
    rsTransactionData->MarkNeedCloseSync();
    rsTransactionData->SetSyncTransactionNum(1);
    mainThread->syncTransactionCount_ = 0;
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionData_.empty(), false);
}

/**
 * @tc.name: GetContext
 * @tc.desc: Test if context has been initialized
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, GetContext, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto& context = mainThread->GetContext();
    uint64_t time = 0;
    ASSERT_EQ(context.GetTransactionTimestamp(), time);
}

/**
 * @tc.name: ClassifyRSTransactionData001
 * @tc.desc: Test ClassifyRSTransactionData when nodeId is 0
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData001, TestSize.Level1)
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
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData002, TestSize.Level1)
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
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData003, TestSize.Level1)
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
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData004, TestSize.Level1)
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
 * @tc.name: AddActiveNode
 * @tc.desc: Test AddActiveNode, add invalid node id, check if fails
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, AddActiveNode, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->context_->activeNodesInRoot_.clear();
    // invalid nodeid
    NodeId id = INVALID_NODEID;
    auto node = std::make_shared<RSRenderNode>(id, mainThread->context_);
    mainThread->context_->AddActiveNode(node);
    ASSERT_EQ(static_cast<int>(mainThread->context_->activeNodesInRoot_.size()), 0);
}

/**
 * @tc.name: CheckAndUpdateInstanceContentStaticStatus01
 * @tc.desc: Test static instance(no dirty) would be classify as only basic geo transform
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSMainThreadTest, CheckAndUpdateInstanceContentStaticStatus01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->context_->activeNodesInRoot_.clear();
    // valid nodeid
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);
    mainThread->CheckAndUpdateInstanceContentStaticStatus(node);
    ASSERT_EQ(node->GetSurfaceCacheContentStatic(), true);
}

/**
 * @tc.name: CheckAndUpdateInstanceContentStaticStatus02
 * @tc.desc: Test new instance would not be classify as only basic geo transform
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSMainThreadTest, CheckAndUpdateInstanceContentStaticStatus02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->context_->activeNodesInRoot_.clear();
    // valid nodeid
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);
    node->SetIsOnTheTree(true, id, id);
    node->SetContentDirty();
    mainThread->context_->AddActiveNode(node);
    ASSERT_EQ(static_cast<int>(mainThread->context_->activeNodesInRoot_.size()), 1);
    mainThread->CheckAndUpdateInstanceContentStaticStatus(node);
    ASSERT_EQ(node->GetSurfaceCacheContentStatic(), false);
}

/**
 * @tc.name: WaitUtilDrivenRenderFinished
 * @tc.desc: Test WaitUtilDrivenRenderFinished, check if drivenRenderFinished_ is valid
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadTest, WaitUtilDrivenRenderFinished, TestSize.Level1)
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    auto mainThread = RSMainThread::Instance();
    mainThread->NotifyDrivenRenderFinish();
    mainThread->WaitUtilDrivenRenderFinished();
    ASSERT_EQ(mainThread->drivenRenderFinished_, true);
#endif
}

/**
 * @tc.name: RecvRSTransactionData
 * @tc.desc: Test RecvRSTransactionData, when TransactionData is null
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadTest, RecvRSTransactionData, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    mainThread->RecvRSTransactionData(transactionData);
    ASSERT_EQ(transactionData, nullptr);
}

/**
 * @tc.name: PostSyncTask
 * @tc.desc: Test PostSyncTask when handler is null or not
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadTest, PostSyncTask, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    RSTaskMessage::RSTask task = []() -> void { return; };
    mainThread->PostSyncTask(task);
}

/**
 * @tc.name: ShowWatermark
 * @tc.desc: ShowWatermark test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSMainThreadTest, ShowWatermark, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    const uint32_t color[8] = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    const int32_t offset = 0;
    Media::InitializationOptions opts;
    int32_t stride = 3;
    std::unique_ptr<Media::PixelMap> pixelMap1 = Media::PixelMap::Create(color, colorLength, offset, stride, opts);
    mainThread->ShowWatermark(std::move(pixelMap1), true);
    ASSERT_EQ(mainThread->GetWatermarkFlag(), true);
    mainThread->ShowWatermark(nullptr, false);
    ASSERT_EQ(mainThread->GetWatermarkFlag(), false);
}

/**
 * @tc.name: MergeToEffectiveTransactionDataMap001
 * @tc.desc: Test RSMainThreadTest.MergeToEffectiveTransactionDataMap
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, MergeToEffectiveTransactionDataMap001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->Start();
    ASSERT_EQ(mainThread->effectiveTransactionDataIndexMap_.empty(), true);
    mainThread->effectiveTransactionDataIndexMap_[0].first = 0;
    TransactionDataMap dataMap;
    auto data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(1);
    dataMap[0].emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(3);
    dataMap[0].emplace_back(std::move(data));
    dataMap[0].emplace_back(nullptr);
    mainThread->MergeToEffectiveTransactionDataMap(dataMap);
    mainThread->effectiveTransactionDataIndexMap_.clear();
}

/**
 * @tc.name: ProcessCommandForUniRender
 * @tc.desc: ProcessCommandForUniRender test with invalid data
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSMainThreadTest, ProcessCommandForUniRender, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_EQ(mainThread->effectiveTransactionDataIndexMap_.empty(), true);

    // // let lastindex same as timeout index and test timeout case at first
    mainThread->transactionDataLastWaitTime_[0] = 0;
    mainThread->timestamp_ = REFRESH_PERIOD * SKIP_COMMAND_FREQ_LIMIT + 1;
    mainThread->effectiveTransactionDataIndexMap_[0].first = 0;
    // default data with index 0
    auto data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(1);
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(3);
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(2);
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(std::move(data));
    // empty data
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(nullptr);
    mainThread->ProcessCommandForUniRender();
}

/**
 * @tc.name: GetWatermarkImg
 * @tc.desc: GetWatermarkImg test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, GetWatermarkImg, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->GetWatermarkImg();
}

/**
 * @tc.name: DoParallelComposition
 * @tc.desc: DoParallelComposition test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, DoParallelComposition, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context = {};
    auto node = std::make_shared<RSBaseRenderNode>(nodeId, context);
    auto childNode = std::make_shared<RSBaseRenderNode>(nodeId + 1, context);
    int index = 0;
    node->SetIsOnTheTree(true);
    node->AddChild(childNode, index);
    ASSERT_EQ(static_cast<int>(node->GetChildrenCount()), 1);
    ASSERT_TRUE(childNode->IsOnTheTree());

    auto mainThread = RSMainThread::Instance();
    RSInnovation::_s_createParallelSyncSignal = (void*)RSMainThreadTest::CreateParallelSyncSignal;
    if (RSInnovation::GetParallelCompositionEnabled(mainThread->isUniRender_)) {
        mainThread->DoParallelComposition(node);
    }
}

/**
 * @tc.name: SetIdleTimerExpiredFlag
 * @tc.desc: SetIdleTimerExpiredFlag test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetIdleTimerExpiredFlag, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->SetIdleTimerExpiredFlag(true);
}

/**
 * @tc.name: SetFocusLeashWindowId
 * @tc.desc: Test RSMainThreadTest.SetFocusLeashWindowId
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, SetFocusLeashWindowId, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->Start();
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node1, nullptr);
    RSSurfaceRenderNodeConfig config;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node2, nullptr);
    node1->SetParent(node2);
    node1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;

    mainThread->context_ = std::make_shared<RSContext>();
    mainThread->context_->nodeMap.renderNodeMap_[0] = node1;
    mainThread->focusNodeId_ = 0;
    mainThread->SetFocusLeashWindowId();
}

/**
 * @tc.name: SetIsCachedSurfaceUpdated
 * @tc.desc: Test RSMainThreadTest.SetIsCachedSurfaceUpdated
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, SetIsCachedSurfaceUpdated, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetIsCachedSurfaceUpdated(true);
}

/**
 * @tc.name: GetFocusAppBundleName
 * @tc.desc: Test RSMainThreadTest.GetFocusAppBundleName
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, GetFocusAppBundleName, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto name = mainThread->GetFocusAppBundleName();
}

/**
 * @tc.name: PrintCurrentStatus
 * @tc.desc: Test RSMainThreadTest.PrintCurrentStatus
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, PrintCurrentStatus, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->PrintCurrentStatus();
}

/**
 * @tc.name: SetDeviceType
 * @tc.desc: Test RSMainThreadTest.SetDeviceType
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, SetDeviceType, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    system::SetParameter("const.product.devicetype", "pc");
    mainThread->SetDeviceType();
    system::SetParameter("const.product.devicetype", "tablet");
    mainThread->SetDeviceType();
    system::SetParameter("const.product.devicetype", "others");
    mainThread->SetDeviceType();
    system::SetParameter("const.product.devicetype", "phone");
    mainThread->SetDeviceType();
}

/**
 * @tc.name: CacheCommands
 * @tc.desc: Test RSMainThreadTest.CacheCommands
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, CacheCommands, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(1);
    mainThread->cachedSkipTransactionDataMap_[0].emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(2);
    mainThread->cachedSkipTransactionDataMap_[0].emplace_back(std::move(data));
    mainThread->CacheCommands();
}

/**
 * @tc.name: CheckIfNodeIsBundle
 * @tc.desc: Test RSMainThreadTest.CheckIfNodeIsBundle
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, CheckIfNodeIsBundle, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSSurfaceRenderNodeConfig config;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    node->name_ = "WallpaperView";
    mainThread->CheckIfNodeIsBundle(node);
    ASSERT_TRUE(mainThread->noBundle_);
}

/**
 * @tc.name: InformHgmNodeInfo
 * @tc.desc: Test RSMainThreadTest.InformHgmNodeInfo
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, InformHgmNodeInfo, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->currentBundleName_ = "test";
    mainThread->InformHgmNodeInfo();
    mainThread->currentBundleName_ = "";
    mainThread->noBundle_ = true;
    mainThread->InformHgmNodeInfo();
}

/**
 * @tc.name: CheckParallelSubThreadNodesStatus
 * @tc.desc: Test RSMainThreadTest.CheckParallelSubThreadNodesStatus
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadTest, CheckParallelSubThreadNodesStatus, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    mainThread->subThreadNodes_.push_back(nullptr);
    mainThread->isUiFirstOn_ = false;
    mainThread->CheckParallelSubThreadNodesStatus();

    auto node1 = std::make_shared<RSSurfaceRenderNode>(0xFFFFFFFFFFFFFFFF);
    node1->cacheProcessStatus_ = CacheProcessStatus::DOING;
    node1->name_ = "node1";
    node1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node1->hasAbilityComponent_ = true;
    node1->abilityNodeIds_.emplace(10);
    node1->abilityNodeIds_.emplace(11);
    auto node2 = std::make_shared<RSSurfaceRenderNode>(2);
    node2->cacheProcessStatus_ = CacheProcessStatus::DOING;
    node2->name_ = "node2";
    node2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(0xFFFFFFFFFFFFFFF0);
    node3->cacheProcessStatus_ = CacheProcessStatus::DOING;
    node3->name_ = "node3";
    node3->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node3->hasAbilityComponent_ = false;
    // create child nodes
    auto childNode1 = std::make_shared<RSSurfaceRenderNode>(3);
    childNode1->name_ = "childNode1";
    childNode1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto childNode2 = std::make_shared<RSSurfaceRenderNode>(4);
    childNode2->name_ = "childNode2";
    childNode2->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto childNode3 = std::make_shared<RSSurfaceRenderNode>(5);
    childNode3->name_ = "childNode3";

    node2->AddChild(childNode1);
    node2->AddChild(childNode2);
    node2->AddChild(childNode3);
    mainThread->subThreadNodes_.push_back(node1);
    mainThread->subThreadNodes_.push_back(node2);
    mainThread->subThreadNodes_.push_back(node3);

    mainThread->cacheCmdSkippedInfo_.clear();
    mainThread->CheckParallelSubThreadNodesStatus();
}

/**
 * @tc.name: IsNeedSkip
 * @tc.desc: IsNeedSkip test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsNeedSkip, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->IsNeedSkip(0, 0);
}

/**
 * @tc.name: SkipCommandByNodeId001
 * @tc.desc: SkipCommandByNodeId test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SkipCommandByNodeId001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<std::unique_ptr<RSTransactionData>> transactionVec;
    mainThread->SkipCommandByNodeId(transactionVec, -1);
}

/**
 * @tc.name: SkipCommandByNodeId002
 * @tc.desc: SkipCommandByNodeId test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SkipCommandByNodeId002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare transactionVec
    std::vector<std::unique_ptr<RSTransactionData>> transactionVec;
    auto data = std::make_unique<RSTransactionData>();
    data->SetIndex(1);
    transactionVec.emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    data->SetIndex(2);
    data->payload_.resize(2);
    data->payload_[0] = std::tuple<NodeId, FollowType, std::unique_ptr<RSCommand>>(0, FollowType::NONE, nullptr);
    transactionVec.emplace_back(std::move(data));
    std::pair<std::vector<NodeId>, bool> info = {{0}, true};
    mainThread->cacheCmdSkippedInfo_.insert({0, info});
    mainThread->SkipCommandByNodeId(transactionVec, 0);
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes001
 * @tc.desc: ConsumeAndUpdateAllNodes test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ConsumeAndUpdateAllNodes001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes002
 * @tc.desc: ConsumeAndUpdateAllNodes test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ConsumeAndUpdateAllNodes002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CheckSubThreadNodeStatusIsDoing001
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckSubThreadNodeStatusIsDoing001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    auto rsSurfaceRenderNode1 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode1->SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
    auto rsSurfaceRenderNode2 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode2->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    mainThread->subThreadNodes_.emplace_back(rsSurfaceRenderNode1);
    mainThread->subThreadNodes_.emplace_back(rsSurfaceRenderNode2);
    mainThread->CheckSubThreadNodeStatusIsDoing(rsSurfaceRenderNode2->GetId());
}

/**
 * @tc.name: CheckSubThreadNodeStatusIsDoing002
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckSubThreadNodeStatusIsDoing002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    auto rsSurfaceRenderNode1 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode1->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    auto rsSurfaceRenderNode2 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode1->AddChild(rsSurfaceRenderNode2);
    mainThread->subThreadNodes_.emplace_back(rsSurfaceRenderNode1);
    mainThread->CheckSubThreadNodeStatusIsDoing(rsSurfaceRenderNode2->GetId());
}

/**
 * @tc.name: CollectInfoForHardwareComposer001
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CollectInfoForHardwareComposer001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CollectInfoForHardwareComposer002
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CollectInfoForHardwareComposer002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: IsLastFrameUIFirstEnbaled001
 * @tc.desc: IsLastFrameUIFirstEnbaled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsLastFrameUIFirstEnbaled001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    // not app window
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    mainThread->subThreadNodes_.push_back(node);
    mainThread->IsLastFrameUIFirstEnabled(0);
    // app window
    id = 2;
    node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    mainThread->subThreadNodes_.push_back(node);
    mainThread->IsLastFrameUIFirstEnabled(2);
}


/**
 * @tc.name: IsLastFrameUIFirstEnbaled002
 * @tc.desc: IsLastFrameUIFirstEnbaled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, IsLastFrameUIFirstEnbaled002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    // parent
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node1->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    // child
    id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node2->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node1->AddChild(node2);
    mainThread->subThreadNodes_.push_back(node1);
    mainThread->IsLastFrameUIFirstEnabled(2);
}

/**
 * @tc.name: CheckIfHardwareForcedDisabled
 * @tc.desc: CheckIfHardwareForcedDisabled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckIfHardwareForcedDisabled, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->CheckIfHardwareForcedDisabled();
}

/**
 * @tc.name: CollectInfoForDrivenRender001
 * @tc.desc: CollectInfoForDrivenRender test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CollectInfoForDrivenRender001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CollectInfoForDrivenRender002
 * @tc.desc: CollectInfoForDrivenRender test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CollectInfoForDrivenRender002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: WaitUntilDisplayNodeBufferReleased
 * @tc.desc: WaitUntilDisplayNodeBufferReleased test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, WaitUntilDisplayNodeBufferReleased, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    sptr<IConsumerSurface> csurf = IConsumerSurface::Create();
    node->SetConsumer(csurf);
    mainThread->WaitUntilDisplayNodeBufferReleased(*node);
}


/**
 * @tc.name: WaitUntilUnmarshallingTaskFinished001
 * @tc.desc: WaitUntilUnmarshallingTaskFinished test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, WaitUntilUnmarshallingTaskFinished001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->WaitUntilUnmarshallingTaskFinished();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: MergeToEffectiveTransactionDataMap
 * @tc.desc: MergeToEffectiveTransactionDataMap test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, MergeToEffectiveTransactionDataMap, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    TransactionDataMap tsDataMap;
    tsDataMap.emplace(0, std::vector<std::unique_ptr<RSTransactionData>>());
    auto data = std::make_unique<RSTransactionData>();
    data->SetIndex(1);
    tsDataMap[0].emplace_back(std::move(data));
    mainThread->MergeToEffectiveTransactionDataMap(tsDataMap);
}

/**
 * @tc.name: UniRender001
 * @tc.desc: UniRender test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, UniRender001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare nodes
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rootNode->AddChild(childDisplayNode, 0);
    mainThread->UniRender(rootNode);
}

/**
 * @tc.name: UniRender002
 * @tc.desc: UniRender test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, UniRender002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare nodes
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rootNode->AddChild(childDisplayNode, 0);
    // prepare status
    bool doDirectComposition = mainThread->doDirectComposition_;
    bool isDirty = mainThread->isDirty_;
    bool isAccessibilityConfigChanged = mainThread->isAccessibilityConfigChanged_;
    bool isCachedSurfaceUpdated = mainThread->isCachedSurfaceUpdated_;
    mainThread->doDirectComposition_ = true;
    mainThread->isDirty_ = false;
    mainThread->isAccessibilityConfigChanged_ = false;
    mainThread->isCachedSurfaceUpdated_ = false;
    mainThread->UniRender(rootNode);
    // status recover
    mainThread->doDirectComposition_ = doDirectComposition;
    mainThread->isDirty_ = isDirty;
    mainThread->isAccessibilityConfigChanged_ = isAccessibilityConfigChanged;
    mainThread->isCachedSurfaceUpdated_ = isCachedSurfaceUpdated;
}

/**
 * @tc.name: Render
 * @tc.desc: Render test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, Render, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    // uni render
    mainThread->isUniRender_ = true;
    mainThread->Render();
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CallbackDrawContextStatusToWMS
 * @tc.desc: CallbackDrawContextStatusToWMS test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CallbackDrawContextStatusToWMS, TestSize.Level1)
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
 * @tc.name: CheckSurfaceNeedProcess
 * @tc.desc: CheckSurfaceNeedProcess test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckSurfaceNeedProcess, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    OcclusionRectISet occRectISet;
    // not focus node
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    mainThread->CheckSurfaceNeedProcess(occRectISet, node);
    // focus node
    id = mainThread->focusNodeId_;
    node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    mainThread->CheckSurfaceNeedProcess(occRectISet, node);
}

/**
 * @tc.name: GetRegionVisibleLevel001
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, GetRegionVisibleLevel001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // empty region
    Occlusion::Region occRegion;
    mainThread->GetRegionVisibleLevel(occRegion, occRegion);
}

/**
 * @tc.name: GetRegionVisibleLevel002
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, GetRegionVisibleLevel002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // equal region
    Occlusion::Region occRegion;
    occRegion.rects_.emplace_back(Occlusion::Rect(0, 1, 1, 0));
    mainThread->GetRegionVisibleLevel(occRegion, occRegion);
}

/**
 * @tc.name: GetRegionVisibleLevel003
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, GetRegionVisibleLevel003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    Occlusion::Region occRegion1;
    occRegion1.rects_.emplace_back(Occlusion::Rect(0, 1, 1, 0));
    Occlusion::Region occRegion2;
    occRegion2.rects_.emplace_back(Occlusion::Rect(0, 2, 2, 0));
    mainThread->GetRegionVisibleLevel(occRegion1, occRegion2);
}

/**
 * @tc.name: CalcOcclusionImplementation
 * @tc.desc: CalcOcclusionImplementation test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CalcOcclusionImplementation, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    curAllSurfaces.emplace_back(nullptr);
    curAllSurfaces.emplace_back(node);
    mainThread->CalcOcclusionImplementation(curAllSurfaces);
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel
 * @tc.desc: SetVSyncRateByVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SetVSyncRateByVisibleLevel, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool vsyncEnabled = mainThread->vsyncControlEnabled_;
    auto systemAnimatedScenesList = mainThread->systemAnimatedScenesList_;
    // prepare state
    mainThread->systemAnimatedScenesList_.clear();
    mainThread->vsyncControlEnabled_ = true;
    auto generator = CreateVSyncGenerator();
    auto appVSyncController = std::make_shared<VSyncController>(generator, 0);
    auto appVSyncDistributor = std::make_shared<VSyncDistributor>(appVSyncController.get(), "app");
    mainThread->SetAppVSyncDistributor(appVSyncDistributor.get());
    std::map<uint32_t, RSVisibleLevel> pidVisMap;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    // pidVisMap init
    pidVisMap.insert({0, RSVisibleLevel::RS_ALL_VISIBLE});
    pidVisMap.insert({1, RSVisibleLevel::RS_SEMI_NONDEFAULT_VISIBLE});
    pidVisMap.insert({2, RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE});
    pidVisMap.insert({3, RSVisibleLevel::RS_INVISIBLE});
    pidVisMap.insert({4, RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE});
    pidVisMap.insert({5, RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL});
    // run
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    // state recover
    mainThread->vsyncControlEnabled_ = vsyncEnabled;
    mainThread->appVSyncDistributor_ = nullptr;
    mainThread->systemAnimatedScenesList_ = systemAnimatedScenesList;
}

/**
 * @tc.name: CallbackToWMS001
 * @tc.desc: CallbackToWMS test visible not changed
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CallbackToWMS001, TestSize.Level1)
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
HWTEST_F(RSMainThreadTest, CallbackToWMS002, TestSize.Level1)
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
 * @tc.desc: CallbackToWMS test visible changed
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SurfaceOcclusionCallback001, TestSize.Level1)
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
 * @tc.desc: CallbackToWMS test visible changed
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, SurfaceOcclusionCallback002, TestSize.Level1)
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
    node3->instanceRootNodeId_ = 0xFFFFFFFF;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    // run 
    mainThread->SurfaceOcclusionCallback();
    mainThread->surfaceOcclusionListeners_.clear();
}

/**
 * @tc.name: CalcOcclusion002
 * @tc.desc: CallbackToWMS test visible changed
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CalcOcclusion002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare states
    bool doWindowAnimate = mainThread->doWindowAnimate_;
    mainThread->doWindowAnimate_ = false;
    auto globalRootRenderNode = mainThread->context_->globalRootRenderNode_;
    // run with nullptr
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->CalcOcclusion();
    // run with one child node
    auto node1 = std::make_shared<RSRenderNode>(0, true);
    auto node2 = std::make_shared<RSRenderNode>(1, true);
    node1->AddChild(node2);
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->CalcOcclusion();
    // run with more than one surface
    auto node3 = std::make_shared<RSRenderNode>(0, true);
    RSDisplayNodeConfig config1;
    auto node4 = std::make_shared<RSDisplayRenderNode>(1, config1);
    RSSurfaceRenderNodeConfig config2;
    auto node5 = std::make_shared<RSSurfaceRenderNode>(config2);
    node3->AddChild(node4);
    node4->curAllSurfaces_.push_back(nullptr);
    node4->curAllSurfaces_.push_back(node5);
    mainThread->context_->globalRootRenderNode_ = node3;
    mainThread->CalcOcclusion();
    // recover
    mainThread->doWindowAnimate_ = doWindowAnimate;
    mainThread->context_->globalRootRenderNode_ = globalRootRenderNode;
}

/**
 * @tc.name: CheckSurfaceVisChanged
 * @tc.desc: CheckSurfaceVisChanged Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CheckSurfaceVisChanged, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare states
    auto systemAnimatedScenesList = mainThread->systemAnimatedScenesList_;
    mainThread->systemAnimatedScenesList_.push_back(std::make_pair(SystemAnimatedScenes::OTHERS, 0));
    std::map<uint32_t, RSVisibleLevel> pidVisMap;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    RSSurfaceRenderNodeConfig config;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    curAllSurfaces.push_back(node);
    auto isVisibleChanged = mainThread->CheckSurfaceVisChanged(pidVisMap, curAllSurfaces);
    ASSERT_EQ(true, isVisibleChanged);
    // recover
    mainThread->systemAnimatedScenesList_ = systemAnimatedScenesList;
}

/**
 * @tc.name: Aniamte
 * @tc.desc: Aniamte Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, Aniamte, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    mainThread->context_->RegisterAnimatingRenderNode(node);
    mainThread->Animate(mainThread->timestamp_);
}

/**
 * @tc.name: RecvRSTransactionData002
 * @tc.desc: RecvRSTransactionData002 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, RecvRSTransactionData002, TestSize.Level1)
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
    mainThread->isUniRender_ = isUniRender_;
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes003
 * @tc.desc: ConsumeAndUpdateAllNodes003 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ConsumeAndUpdateAllNodes003, TestSize.Level1)
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
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    node1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    config.id = 3;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    node2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    config.id = 4;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    node3->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node3->isHardwareEnabledNode_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CollectInfoForHardwareComposer003
 * @tc.desc: CollectInfoForHardwareComposer003 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, CollectInfoForHardwareComposer003, TestSize.Level1)
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
    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    node3->SetIsOnTheTree(true);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    auto node4 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node4->SetIsOnTheTree(true);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node4);
    auto node5 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node5->SetIsOnTheTree(true);
    node5->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node5->isHardwareEnabledNode_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node5);
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ClassifyRSTransactionData005
 * @tc.desc: ClassifyRSTransactionData005 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData005, TestSize.Level1)
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
    data->SetIndex(1);
    data->payload_.resize(2);
    data->payload_[0] = std::tuple<NodeId, FollowType, std::unique_ptr<RSCommand>>(0, FollowType::None, nullptr);
    data->payload_[1] = std::tuple<NodeId, FollowType, std::unique_ptr<RSCommand>>(2, FollowType::FOLLOW_TO_PARENT, nullptr);
    mainThread->ClassifyRSTransactionData(data);
}

/**
 * @tc.name: ReleaseAllNodesBuffer
 * @tc.desc: ReleaseAllNodesBuffer Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, ReleaseAllNodesBuffer, TestSize.Level1)
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
    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    node3->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node3->isHardwareEnabledNode_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    config.id = 4;
    auto node4 = std::make_shared<RSSurfaceRenderNode>(config);
    node4->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node4->isHardwareEnabledNode_ = true;
    node4->IsLastFrameHardwareEnabled_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node4);
    config.id = 5;
    auto node5 = std::make_shared<RSSurfaceRenderNode>(config);
    node5->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node5->isHardwareEnabledNode_ = true;
    node5->IsLastFrameHardwareEnabled_ = true;
    node5->IsCurrentFrameHardwareEnabled_ = false;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node5);
    mainThread->ReleaseAllNodesBuffer();
}

/**
 * @tc.name: PostTask001
 * @tc.desc: PostTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PostTask001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    RSTaskMessage::RSTask task = []() -> void { return; };
    mainThread->PostTask(task);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: PostTask002
 * @tc.desc: PostTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PostTask002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    RSTaskMessage::RSTask task = []() -> void { return; };
    std::string name = "test";
    int64_t delayTime = 0;
    AppExecFwk::EventQueue::Priority priority = AppExecFwk::EventQueue::Priority::IMMEDIATE;
    mainThread->PostTask(task, name, delayTime, priority);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: RemoveTask
 * @tc.desc: RemoveTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, RemoveTask, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    std::string name = "test";
    mainThread->RemoveTask(name);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: PostSyncTask002
 * @tc.desc: PostSyncTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, PostSyncTask002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    RSTaskMessage::RSTask task = []() -> void { return; };
    mainThread->PostSyncTask(task);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: RegisterSurfaceOcclusionChangeCallBack001
 * @tc.desc: RegisterSurfaceOcclusionChangeCallBack001 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadTest, RegisterSurfaceOcclusionChangeCallBack001, TestSize.Level1)
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
HWTEST_F(RSMainThreadTest, RegisterSurfaceOcclusionChangeCallBack002, TestSize.Level1)
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
} // namespace OHOS::Rosen
